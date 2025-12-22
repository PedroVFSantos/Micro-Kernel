#pragma once
#include <stdint.h>

namespace Manco {
    class PMM {
    public:
        static void init(uint32_t mem_size);
        static void free_block(uint32_t addr);
        static void use_block(uint32_t addr);
        static int first_free();
    private:
        static uint32_t* mmap;
        static uint32_t total_blocks;
    };
}