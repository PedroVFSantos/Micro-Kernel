#include "kernel.hpp"
#include "console.hpp"
#include "gdt.hpp"
#include "idt.hpp"
#include "isr.hpp"
#include "pmm.hpp"
#include "scheduler.hpp"

extern "C" char _kernel_end[];

namespace Manco {
    void spawn_demo_tasks();

    void panic(const char* msg) {
        console.print("\nKERNEL PANIC: ", Color::LightRed);
        console.print(msg, Color::LightRed);
        asm volatile("cli");
        while (true) asm volatile("hlt");
    }
}

using namespace Manco;

static void ok(const char* msg) {
    console.print("[OK] ", Color::LightGreen);
    console.print(msg);
    console.put_char('\n');
}

extern "C" void kernel_main(uint32_t magic, MultibootInfo* mbi) {
    init_serial();
    set_vga_palette();
    console.clear(Color::Black);
    console.print("MancoOS booting...\n", Color::LightCyan);

    init_gdt();
    ok("GDT loaded (flat code/data segments)");

    init_idt();
    init_pic();
    init_timer();
    asm volatile("sti");
    ok("IDT loaded, PIC remapped to 32-47, PIT at 100 Hz");

    if (magic != MULTIBOOT_MAGIC || !(mbi->flags & 0x1)) {
        panic("bootloader did not provide memory info");
    }
    uint32_t mem_size = (mbi->mem_upper + 1024) * 1024;
    PMM::init(mem_size, (uint32_t)(uintptr_t)_kernel_end);
    console.print("[OK] ", Color::LightGreen);
    console.print("PMM: ");
    console.print_dec(mem_size / (1024 * 1024));
    console.print(" MB of RAM, ");
    console.print_dec(PMM::free_blocks());
    console.print(" free 4K blocks\n");

    spawn_demo_tasks();
    ok("Starting SJF scheduler\n");

    scheduler.run();

    scheduler.print_stats();
    console.print("\nNo runnable tasks left, idling.\n", Color::DarkGray);

    while(true) {
        asm volatile("hlt");
    }
}
