#include "idt.hpp"

extern "C" void dummy_handler() {
    __asm__ volatile("iret");
}

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

        for(int i = 0; i < 256; i++) {
            set_idt_gate(i, (uint32_t)dummy_handler, 0x08, 0x8E);
        }

        load_idt((uint32_t)(uintptr_t)&idt_ptr);
    }
}