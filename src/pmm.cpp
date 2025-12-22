#include "pmm.hpp"

namespace Manco {
    uint32_t mmap_array[32768 / 32];
    uint32_t* PMM::mmap = mmap_array;
    uint32_t PMM::total_blocks = 32768;

    void PMM::init(uint32_t mem_size) {
        for (uint32_t i = 0; i < (total_blocks / 32); i++) {
            mmap[i] = 0xFFFFFFFF;
        }
    }

    void PMM::free_block(uint32_t bit) {
        mmap[bit / 32] &= ~(1 << (bit % 32));
    }

    void PMM::use_block(uint32_t bit) {
        mmap[bit / 32] |= (1 << (bit % 32));
    }

    int PMM::first_free() {
        for (uint32_t i = 0; i < total_blocks / 32; i++) {
            if (mmap[i] != 0xFFFFFFFF) {
                for (int j = 0; j < 32; j++) {
                    uint32_t bit = 1 << j;
                    if (!(mmap[i] & bit)) return i * 32 + j;
                }
            }
        }
        return -1;
    }
}