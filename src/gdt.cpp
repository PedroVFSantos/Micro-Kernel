#include "gdt.hpp"

namespace Manco {
    GDTEntry gdt[3];
    GDTPointer gdt_ptr;

    extern "C" void gdt_flush(uint32_t);

    void set_gdt_gate(int32_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran) {
        gdt[num].base_low    = (base & 0xFFFF);
        gdt[num].base_middle = (base >> 16) & 0xFF;
        gdt[num].base_high   = (base >> 24) & 0xFF;
        gdt[num].limit_low   = (limit & 0xFFFF);
        gdt[num].granularity = (limit >> 16) & 0x0F;
        gdt[num].granularity |= gran & 0xF0;
        gdt[num].access      = access;
    }

    void init_gdt() {
        gdt_ptr.limit = (sizeof(GDTEntry) * 3) - 1;
        gdt_ptr.base  = (uint32_t)(uintptr_t)&gdt;

        set_gdt_gate(0, 0, 0, 0, 0);
        set_gdt_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);
        set_gdt_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);

        gdt_flush((uint32_t)(uintptr_t)&gdt_ptr);
    }
}