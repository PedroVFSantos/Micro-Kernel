#pragma once
#include <stdint.h>

namespace Manco {
    // Mesmo layout que o isr_common (interrupts.s) deixa na pilha
    struct Registers {
        uint32_t ds;
        uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
        uint32_t int_no, err_code;
        uint32_t eip, cs, eflags;
    } __attribute__((packed));

    using IrqHandler = void (*)(Registers*);

    // IRQs do PIC são remapeadas pros vetores 32-47
    constexpr int IRQ_BASE = 32;

    void init_pic();
    void register_irq_handler(int irq, IrqHandler handler);

    // PIT
    constexpr uint32_t TIMER_HZ = 100;
    void init_timer();
    uint32_t timer_ticks();
}
