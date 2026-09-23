#pragma once
#include <stdint.h>

namespace Manco {
    enum class Color : uint8_t {
        Black = 0, Blue = 1, Green = 2, Cyan = 3, Red = 4,
        Magenta = 5, Brown = 6, LightGray = 7, DarkGray = 8,
        LightBlue = 9, LightGreen = 10, LightCyan = 11,
        LightRed = 12, Pink = 13, Yellow = 14, White = 15
    };

    class Console {
    public:
        static const int WIDTH = 80;
        static const int HEIGHT = 25;
        void clear(Color bg = Color::Black);
        void put_char(char c, Color fg = Color::White, Color bg = Color::Black);
        void print(const char* str, Color fg = Color::White, Color bg = Color::Black);
        // width > 0 alinha à direita com espaços
        void print_dec(uint32_t value, int width = 0, Color fg = Color::White);
        void print_hex(uint32_t value, Color fg = Color::White);
    private:
        int column = 0;
        int row = 0;
        static constexpr uintptr_t VGA_MEMORY = 0xB8000;
        uint16_t* buffer() const { return reinterpret_cast<uint16_t*>(VGA_MEMORY); }
        void scroll();
    };

    // Tudo que vai pra tela também sai na COM1 (dá pra ver com qemu -serial stdio)
    void init_serial();
    void set_vga_palette(); 

    extern Console console;
}
