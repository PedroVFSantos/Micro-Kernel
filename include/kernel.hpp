#pragma once
#include <stdint.h>

namespace Manco {
    constexpr uint32_t MULTIBOOT_MAGIC = 0x2BADB002;

    // Só os campos que a gente usa da struct multiboot_info (spec v1)
    struct MultibootInfo {
        uint32_t flags;
        uint32_t mem_lower; // KB abaixo de 1MB
        uint32_t mem_upper; // KB acima de 1MB
    } __attribute__((packed));

    [[noreturn]] void panic(const char* msg);
}
