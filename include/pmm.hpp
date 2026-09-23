#pragma once
#include <stdint.h>

namespace Manco {
    class PMM {
    public:
        static constexpr uint32_t BLOCK_SIZE = 4096;

        // Marca como livre tudo entre kernel_end e mem_size
        static void init(uint32_t mem_size, uint32_t kernel_end);
        // Retorna o endereço físico de um bloco de 4KB, ou 0 se acabou a memória
        static uint32_t alloc_block();
        static void free_block(uint32_t addr);
        static uint32_t free_blocks();
    private:
        static void use_block(uint32_t bit);
        static void release_block(uint32_t bit);
        static int first_free();
        static uint32_t* mmap;
        static uint32_t total_blocks;
        static uint32_t used_blocks;
    };
}
