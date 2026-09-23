#include "idt.hpp"

// Gerado em interrupts.s: endereços dos stubs dos vetores 0-47
extern "C" uint32_t isr_stub_table[48];

namespace Manco {
    IDTEntry idt[256];
    IDTPointer idt_ptr;

    extern "C" void load_idt(uint32_t);

    void set_idt_gate(int n, uint32_t handler, uint16_t selector, uint8_t flags) {
        idt[n].base_low = handler & 0xFFFF;
        idt[n].base_high = (handler >> 16) & 0xFFFF;
        idt[n].selector = selector;
        idt[n].zero = 0;
        idt[n].flags = flags;
    }

    void init_idt() {
        idt_ptr.limit = (sizeof(IDTEntry) * 256) - 1;
        idt_ptr.base = (uint32_t)(uintptr_t)&idt;

        // 0-31 exceções da CPU, 32-47 IRQs do PIC. O resto fica com present=0,
        // então se algo disparar lá a CPU gera #GP e cai no handler de exceção.
        for (int i = 0; i < 48; i++) {
            set_idt_gate(i, isr_stub_table[i], 0x08, 0x8E);
        }

        load_idt((uint32_t)(uintptr_t)&idt_ptr);
    }
}
