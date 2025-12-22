#include "scheduler.hpp"

extern "C" void switch_task(uint32_t* old_esp, uint32_t* new_esp);

namespace Manco {
    void Scheduler::add_task(void (*entry_point)(), uint32_t estimated_time) {
        if (task_count < 16) {
            tasks[task_count].id = task_count;
            tasks[task_count].state = TaskState::READY;
            tasks[task_count].burst_time = estimated_time;
            
            // Em um kernel real, aqui inicializaríamos o stack frame
            // Para este teste de portfólio, apenas registramos os dados
            task_count++;
        }
    }

    int Scheduler::find_shortest_job() {
        int shortest_idx = -1;
        uint32_t min_time = 0xFFFFFFFF;

        for (int i = 0; i < task_count; i++) {
            if (tasks[i].state == TaskState::READY && tasks[i].burst_time < min_time) {
                min_time = tasks[i].burst_time;
                shortest_idx = i;
            }
        }
        return shortest_idx;
    }

    void Scheduler::schedule() {
        if (task_count <= 1) return;

        int next_idx = find_shortest_job();
        if (next_idx == -1 || next_idx == current_task) return;

        int prev_idx = current_task;
        current_task = next_idx;

        if (prev_idx != -1) tasks[prev_idx].state = TaskState::READY;
        tasks[current_task].state = TaskState::RUNNING;

        if (prev_idx != -1) {
            switch_task(&tasks[prev_idx].stack_ptr, &tasks[current_task].stack_ptr);
        }
    }
}