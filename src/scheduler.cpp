#include "scheduler.hpp"
#include "pmm.hpp"
#include "isr.hpp"
#include "console.hpp"

extern "C" void switch_task(uint32_t* old_esp, uint32_t* new_esp);

namespace Manco {
    Scheduler scheduler;

    static void print_padded(const char* s, int width) {
        int len = 0;
        while (s[len]) len++;
        console.print(s);
        for (int i = len; i < width; i++) console.put_char(' ');
    }

    int Scheduler::add_task(const char* name, void (*entry_point)(), uint32_t estimated_time) {
        if (task_count >= MAX_TASKS) return -1;

        uint32_t page = PMM::alloc_block();
        if (page == 0) return -1;

        Task& t = tasks[task_count];
        t.id = task_count;
        t.name = name;
        t.entry = entry_point;
        t.state = TaskState::READY;
        t.burst_time = estimated_time;
        t.stack_page = page;
        t.arrival_tick = timer_ticks();
        t.start_tick = 0;
        t.finish_tick = 0;
        t.started = false;

        // Monta a pilha como se a tarefa já tivesse chamado switch_task antes:
        // switch_task faz pop ebx, esi, edi, ebp e depois ret, que cai no task_entry.
        uint32_t* sp = (uint32_t*)(page + PMM::BLOCK_SIZE);
        *--sp = 0;                          // endereço de retorno do task_entry (nunca usado)
        *--sp = (uint32_t)(uintptr_t)task_entry;
        *--sp = 0;                          // ebp
        *--sp = 0;                          // edi
        *--sp = 0;                          // esi
        *--sp = 0;                          // ebx
        t.stack_ptr = (uint32_t)(uintptr_t)sp;

        return task_count++;
    }

    void Scheduler::task_entry() {
        scheduler.tasks[scheduler.current_task].entry();
        scheduler.exit();
    }

    int Scheduler::find_shortest_job() {
        int shortest_idx = -1;
        uint32_t min_time = 0xFFFFFFFF;

        // Empate fica com quem chegou primeiro (menor índice), por isso o < estrito
        for (int i = 0; i < task_count; i++) {
            if (tasks[i].state == TaskState::READY && tasks[i].burst_time < min_time) {
                min_time = tasks[i].burst_time;
                shortest_idx = i;
            }
        }
        return shortest_idx;
    }

    void Scheduler::run() {
        while (true) {
            int next = find_shortest_job();
            if (next == -1) break;

            Task& t = tasks[next];
            if (!t.started) {
                t.started = true;
                t.start_tick = timer_ticks();
                console.print("[sched] ", Color::Yellow);
                console.print("start ");
                print_padded(t.name, 12);
                console.print(" burst=");
                console.print_dec(t.burst_time);
                console.print(" tick=");
                console.print_dec(t.start_tick);
                console.put_char('\n');
            }

            t.state = TaskState::RUNNING;
            current_task = next;
            switch_task(&scheduler_esp, &t.stack_ptr);
            // A tarefa devolveu a CPU (yield, block ou exit)
            current_task = -1;

            if (t.state == TaskState::FINISHED) {
                PMM::free_block(t.stack_page);
                t.stack_page = 0;
                console.print("[sched] ", Color::Yellow);
                console.print("done  ");
                print_padded(t.name, 12);
                console.print(" tick=");
                console.print_dec(t.finish_tick);
                console.put_char('\n');
            }
        }
    }

    void Scheduler::switch_to_scheduler() {
        switch_task(&tasks[current_task].stack_ptr, &scheduler_esp);
    }

    void Scheduler::yield() {
        tasks[current_task].state = TaskState::READY;
        switch_to_scheduler();
    }

    void Scheduler::block() {
        tasks[current_task].state = TaskState::BLOCKED;
        switch_to_scheduler();
    }

    void Scheduler::exit() {
        tasks[current_task].finish_tick = timer_ticks();
        tasks[current_task].state = TaskState::FINISHED;
        switch_to_scheduler();
        // O scheduler nunca volta pra uma tarefa FINISHED
        while (true) asm volatile("hlt");
    }

    void Scheduler::wake(uint32_t id) {
        if (exists(id) && tasks[id].state == TaskState::BLOCKED) {
            tasks[id].state = TaskState::READY;
        }
    }

    void Scheduler::print_stats() {
        console.print("\ntask         burst   wait  turnaround\n", Color::LightCyan);

        uint32_t total_wait = 0, fcfs_wait = 0, fcfs_clock = 0, finished = 0;
        for (int i = 0; i < task_count; i++) {
            const Task& t = tasks[i];
            print_padded(t.name, 12);

            if (t.state != TaskState::FINISHED) {
                console.print("   (blocked, waiting for messages)\n", Color::DarkGray);
                continue;
            }

            uint32_t wait = t.start_tick - t.arrival_tick;
            console.print_dec(t.burst_time, 6);
            console.print_dec(wait, 7);
            console.print_dec(t.finish_tick - t.arrival_tick, 12);
            console.put_char('\n');

            total_wait += wait;
            // Mesmas tarefas em ordem de chegada, pra comparar
            fcfs_wait += fcfs_clock;
            fcfs_clock += t.burst_time;
            finished++;
        }

        if (finished == 0) return;

        // Sem FPU no kernel, então uma casa decimal na mão
        auto print_avg = [finished](uint32_t total) {
            uint32_t x10 = total * 10 / finished;
            console.print_dec(x10 / 10);
            console.put_char('.');
            console.print_dec(x10 % 10);
            console.print(" ticks\n");
        };

        console.print("\navg wait (SJF, measured):     ", Color::LightGreen);
        print_avg(total_wait);
        console.print("avg wait (FCFS, same bursts): ", Color::LightGray);
        print_avg(fcfs_wait);
    }
}
