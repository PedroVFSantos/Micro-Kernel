#include "console.hpp"
#include "io.hpp"

namespace Manco {
    void Console::clear(Color bg) {
        for (int y = 0; y < HEIGHT; y++) {
            for (int x = 0; x < WIDTH; x++) {
                const int index = y * WIDTH + x;
                buffer[index] = ((uint16_t)bg << 12) | ' ';
            }
        }
    }

    void Console::put_char(char c, Color fg, Color bg) {
        if (c == '\n') {
            column = 0;
            row++;
        } else {
            const int index = row * WIDTH + column;
            uint16_t attribute = ((uint8_t)bg << 4) | ((uint8_t)fg & 0x0F);
            buffer[index] = (attribute << 8) | c;
            column++;
        }

        if (column >= WIDTH) {
            column = 0;
            row++;
        }
    }

    void Console::print(const char* str, Color fg, Color bg) {
        for (int i = 0; str[i] != '\0'; i++) {
            put_char(str[i], fg, bg);
        }
    }

  void set_vga_palette() {
        auto set_color = [](uint8_t index, uint8_t r, uint8_t g, uint8_t b) {
            outb(0x3C8, index);
            outb(0x3C9, r >> 2);
            outb(0x3C9, g >> 2);
            outb(0x3C9, b >> 2);
        };

        set_color(0, 0x18, 0x03, 0x03); 
        set_color(1, 0x3D, 0x02, 0x02); 
        set_color(7, 0xC9, 0xC1, 0xC1); 
        set_color(15, 0xD1, 0xB4, 0xB4); 
    }

}
