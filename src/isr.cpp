#include "isr.hpp"
#include "io.hpp"
#include "console.hpp"

namespace Manco {
    static const char* exception_names[32] = {
        "Divide by zero", "Debug", "NMI", "Breakpoint", "Overflow",
        "Bound range exceeded", "Invalid opcode", "Device not available",
        "Double fault", "Coprocessor segment overrun", "Invalid TSS",
        "Segment not present", "Stack-segment fault", "General protection fault",
        "Page fault", "Reserved", "x87 floating point", "Alignment check",
        "Machine check", "SIMD floating point", "Virtualization", "Control protection",
        "Reserved", "Reserved", "Reserved", "Reserved", "Reserved", "Reserved",
        "Hypervisor injection", "VMM communication", "Security", "Reserved"
    };

    static IrqHandler irq_handlers[16];
    static volatile uint32_t ticks = 0;

    constexpr uint16_t PIC1_CMD = 0x20, PIC1_DATA = 0x21;
    constexpr uint16_t PIC2_CMD = 0xA0, PIC2_DATA = 0xA1;
    constexpr uint8_t PIC_EOI = 0x20;

    // Por padrão o PIC manda as IRQs 0-7 pros vetores 8-15, que batem com as exceções
    // da CPU (IRQ0 do timer ia parecer um double fault). Então remapeia pra 32-47.
    void init_pic() {
        outb(PIC1_CMD, 0x11); io_wait();   // ICW1: init + vai ter ICW4
        outb(PIC2_CMD, 0x11); io_wait();
        outb(PIC1_DATA, IRQ_BASE); io_wait();     // ICW2: offset
        outb(PIC2_DATA, IRQ_BASE + 8); io_wait();
        outb(PIC1_DATA, 0x04); io_wait();  // ICW3: slave no IRQ2
        outb(PIC2_DATA, 0x02); io_wait();
        outb(PIC1_DATA, 0x01); io_wait();  // ICW4: modo 8086
        outb(PIC2_DATA, 0x01); io_wait();

        // Tudo mascarado, register_irq_handler libera o que for usado
        outb(PIC1_DATA, 0xFB); // IRQ2 aberto pro cascade
        outb(PIC2_DATA, 0xFF);
    }

    void register_irq_handler(int irq, IrqHandler handler) {
        irq_handlers[irq] = handler;

        uint16_t port = irq < 8 ? PIC1_DATA : PIC2_DATA;
        uint8_t line = irq < 8 ? irq : irq - 8;
        outb(port, inb(port) & ~(1 << line));
    }

    static void timer_handler(Registers*) {
        ticks++;
    }

    void init_timer() {
        // PIT roda a 1.193182 MHz, canal 0 no modo 3 (square wave)
        uint32_t divisor = 1193182 / TIMER_HZ;
        outb(0x43, 0x36);
        outb(0x40, divisor & 0xFF);
        outb(0x40, (divisor >> 8) & 0xFF);
        register_irq_handler(0, timer_handler);
    }

    uint32_t timer_ticks() {
        return ticks;
    }

    static void exception_panic(Registers* regs) {
        console.print("\n*** EXCEPTION: ", Color::LightRed);
        console.print(exception_names[regs->int_no], Color::LightRed);
        console.print(" (vector ", Color::LightRed);
        console.print_dec(regs->int_no, 0, Color::LightRed);
        console.print(")\n    eip=", Color::LightRed);
        console.print_hex(regs->eip, Color::LightRed);
        console.print(" err=", Color::LightRed);
        console.print_hex(regs->err_code, Color::LightRed);
        console.print("\nSystem halted.\n", Color::LightRed);

        asm volatile("cli");
        while (true) asm volatile("hlt");
    }

    extern "C" void isr_dispatch(Registers* regs) {
        if (regs->int_no < 32) {
            exception_panic(regs);
        }

        int irq = regs->int_no - IRQ_BASE;
        if (irq_handlers[irq]) {
            irq_handlers[irq](regs);
        }

        if (irq >= 8) outb(PIC2_CMD, PIC_EOI);
        outb(PIC1_CMD, PIC_EOI);
    }
}
