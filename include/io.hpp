#pragma once
#include <stdint.h> 

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ( "outb %b0, %w1" : : "a"(val), "Nd"(port) );
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ( "inb %w1, %b0" : "=a"(ret) : "Nd"(port) );
    return ret;
}

// Porta 0x80 não é usada, escrever nela só dá tempo pro PIC processar o comando
static inline void io_wait() {
    outb(0x80, 0);
}
