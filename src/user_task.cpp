#include "scheduler.hpp"
#include "ipc.hpp"
#include "isr.hpp"
#include "console.hpp"

// Tarefas de demonstração. Ainda rodam em ring 0 e no mesmo espaço de endereço,
// então dá pra mandar ponteiro de string direto na mensagem.

namespace Manco {
    static uint32_t console_server_id;

    static void send_print(const char* text) {
        Message msg{};
        msg.type = MessageType::SYS_PRINT;
        msg.data[0] = (uint32_t)(uintptr_t)text;
        IPC::send(console_server_id, &msg);
    }

    // Único dono da tela: as outras tarefas pedem pra imprimir via IPC
    static void console_server() {
        Message msg;
        while (true) {
            IPC::receive(IPC::ANY, &msg);
            if (msg.type != MessageType::SYS_PRINT) continue;

            console.print("  [", Color::DarkGray);
            console.print(scheduler.task(msg.sender_id).name, Color::LightBlue);
            console.print("] ", Color::DarkGray);
            console.print((const char*)(uintptr_t)msg.data[0]);
            console.put_char('\n');
        }
    }

    // Simula uso de CPU pelo tempo estimado da tarefa
    static void burn_cpu() {
        const Task& self = scheduler.task(scheduler.current());
        uint32_t start = timer_ticks();
        while (timer_ticks() - start < self.burst_time) {
            asm volatile("hlt");
        }
    }

    static void worker() {
        send_print("working...");
        burn_cpu();
        send_print("finished");
    }

    void spawn_demo_tasks() {
        // O servidor tem o menor burst pra ser escolhido assim que chega mensagem
        console_server_id = scheduler.add_task("console-srv", console_server, 1);

        // Ordem de chegada proposital fora de ordem de tamanho
        scheduler.add_task("compiler", worker, 150);
        scheduler.add_task("backup", worker, 50);
        scheduler.add_task("indexer", worker, 100);
        scheduler.add_task("shell", worker, 20);
    }
}
