#include "console.hpp"
#include "gdt.hpp"
#include "idt.hpp"
#include "pmm.hpp"
#include "scheduler.hpp"

namespace Manco {
    extern void task_vga_service();
    extern void task_input_service();
}

Manco::Scheduler sched;

extern "C" void kernel_main() {
    Manco::Console vga;
    Manco::set_vga_palette();
    vga.clear(Manco::Color::Black);
    vga.print("MancoOS Finalizing Boot...\n", Manco::Color::LightCyan);

    Manco::init_gdt();
    Manco::init_idt();
    Manco::PMM::init(128 * 1024 * 1024);

    sched.add_task(Manco::task_vga_service, 10);
    sched.add_task(Manco::task_input_service, 50);

    vga.print("[OK] Starting SJF Scheduler...\n", Manco::Color::LightGreen);

    sched.schedule();

    while(true) {
        asm volatile("hlt");
    }
}