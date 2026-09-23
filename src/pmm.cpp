#include "pmm.hpp"

namespace Manco {
    // 32768 blocos de 4KB = 128MB, o suficiente pro que o QEMU dá por padrão
    uint32_t mmap_array[32768 / 32];
    uint32_t* PMM::mmap = mmap_array;
    uint32_t PMM::total_blocks = 32768;
    uint32_t PMM::used_blocks = 0;

    void PMM::init(uint32_t mem_size, uint32_t kernel_end) {
        for (uint32_t i = 0; i < (total_blocks / 32); i++) {
            mmap[i] = 0xFFFFFFFF;
        }
        used_blocks = total_blocks;

        // Abaixo de 1MB tem BIOS, VGA etc, e logo depois vem o kernel.
        // Então só libera de kernel_end até o fim da RAM.
        uint32_t first = (kernel_end + BLOCK_SIZE - 1) / BLOCK_SIZE;
        uint32_t last = mem_size / BLOCK_SIZE;
        if (last > total_blocks) last = total_blocks;

        for (uint32_t bit = first; bit < last; bit++) {
            release_block(bit);
        }
    }

    void PMM::release_block(uint32_t bit) {
        if (mmap[bit / 32] & (1u << (bit % 32))) {
            mmap[bit / 32] &= ~(1u << (bit % 32));
            used_blocks--;
        }
    }

    void PMM::use_block(uint32_t bit) {
        if (!(mmap[bit / 32] & (1u << (bit % 32)))) {
            mmap[bit / 32] |= (1u << (bit % 32));
            used_blocks++;
        }
    }

    int PMM::first_free() {
        for (uint32_t i = 0; i < total_blocks / 32; i++) {
            if (mmap[i] != 0xFFFFFFFF) {
                for (int j = 0; j < 32; j++) {
                    uint32_t bit = 1u << j;
                    if (!(mmap[i] & bit)) return i * 32 + j;
                }
            }
        }
        return -1;
    }

    uint32_t PMM::alloc_block() {
        int bit = first_free();
        if (bit == -1) return 0;
        use_block(bit);
        return (uint32_t)bit * BLOCK_SIZE;
    }

    void PMM::free_block(uint32_t addr) {
        uint32_t bit = addr / BLOCK_SIZE;
        if (bit < total_blocks) release_block(bit);
    }

    uint32_t PMM::free_blocks() {
        return total_blocks - used_blocks;
    }
}
