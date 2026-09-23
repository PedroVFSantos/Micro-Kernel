#include "ipc.hpp"
#include "scheduler.hpp"

namespace Manco {
    struct Mailbox {
        Message msgs[IPC::MAILBOX_SIZE];
        int count;
    };

    static Mailbox mailboxes[Scheduler::MAX_TASKS];

    bool IPC::send(uint32_t dest, Message* msg) {
        if (!scheduler.exists(dest) || scheduler.task(dest).state == TaskState::FINISHED) return false;

        Mailbox& box = mailboxes[dest];
        if (box.count == MAILBOX_SIZE) return false;

        int sender = scheduler.current();
        msg->sender_id = sender == -1 ? ANY : (uint32_t)sender;
        box.msgs[box.count++] = *msg;

        scheduler.wake(dest);

        // send é um ponto de escalonamento: se o destino acordou e tem burst menor,
        // o SJF escolhe ele agora, se não a gente volta direto.
        if (sender != -1) scheduler.yield();
        return true;
    }

    void IPC::receive(uint32_t source, Message* msg) {
        Mailbox& box = mailboxes[scheduler.current()];

        while (true) {
            for (int i = 0; i < box.count; i++) {
                if (source != ANY && box.msgs[i].sender_id != source) continue;

                *msg = box.msgs[i];
                for (int j = i; j < box.count - 1; j++) {
                    box.msgs[j] = box.msgs[j + 1];
                }
                box.count--;
                return;
            }
            scheduler.block();
        }
    }
}
