#include <stddef.h>
#include <stdint.h>

// O GCC pode gerar chamadas pra essas funções mesmo com -ffreestanding
// (cópia de struct, zerar array...), então o kernel precisa ter as suas.
extern "C" {
    void* memset(void* dest, int value, size_t n) {
        uint8_t* d = (uint8_t*)dest;
        for (size_t i = 0; i < n; i++) d[i] = (uint8_t)value;
        return dest;
    }

    void* memcpy(void* dest, const void* src, size_t n) {
        uint8_t* d = (uint8_t*)dest;
        const uint8_t* s = (const uint8_t*)src;
        for (size_t i = 0; i < n; i++) d[i] = s[i];
        return dest;
    }

    void* memmove(void* dest, const void* src, size_t n) {
        uint8_t* d = (uint8_t*)dest;
        const uint8_t* s = (const uint8_t*)src;
        if (d < s) {
            for (size_t i = 0; i < n; i++) d[i] = s[i];
        } else {
            for (size_t i = n; i > 0; i--) d[i - 1] = s[i - 1];
        }
        return dest;
    }
}
