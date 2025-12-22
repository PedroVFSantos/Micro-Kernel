#pragma once
#include <stdint.h>

namespace Manco {
    enum class TaskState { READY, RUNNING, BLOCKED };

    struct Task {
        uint32_t id;
        uint32_t stack_ptr;
        TaskState state;
        uint32_t burst_time;
    };

    class Scheduler {
    public:
        void schedule(); 
        void add_task(void (*entry_point)(), uint32_t estimated_time);
    private:
        Task tasks[16];
        int task_count = 0;
        int current_task = -1;

        int find_shortest_job(); 
    };
}