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
    private:
        int column = 0;
        int row = 0;
        uint16_t* const buffer = (uint16_t*)0xB8000;
    };

    void set_vga_palette(); 
}