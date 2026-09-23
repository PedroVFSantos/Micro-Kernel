#pragma once
#include <stdint.h>

namespace Manco {
    enum class TaskState { READY, RUNNING, BLOCKED, FINISHED };

    struct Task {
        uint32_t id;
        const char* name;
        uint32_t stack_ptr;
        uint32_t stack_page;
        TaskState state;
        uint32_t burst_time; // estimativa em ticks do timer
        void (*entry)();

        // Estatísticas, em ticks
        uint32_t arrival_tick;
        uint32_t start_tick;
        uint32_t finish_tick;
        bool started;
    };

    // SJF não-preemptivo: a tarefa escolhida roda até terminar, bloquear ou chamar yield().
    // O timer não tira ninguém da CPU, ele só serve pra medir o tempo.
    class Scheduler {
    public:
        static const int MAX_TASKS = 16;

        int add_task(const char* name, void (*entry_point)(), uint32_t estimated_time);
        // Loop do escalonador, roda no contexto do kernel_main e só volta quando não há mais nada pronto
        void run();
        void print_stats();

        // Chamadas feitas de dentro das tarefas
        void yield();
        void block();
        [[noreturn]] void exit();
        void wake(uint32_t id);

        int current() const { return current_task; }
        bool exists(uint32_t id) const { return id < (uint32_t)task_count; }
        const Task& task(uint32_t id) const { return tasks[id]; }
    private:
        Task tasks[MAX_TASKS];
        int task_count = 0;
        int current_task = -1;
        uint32_t scheduler_esp = 0;

        int find_shortest_job(); 
        void switch_to_scheduler();
        static void task_entry();
    };

    extern Scheduler scheduler;
}
