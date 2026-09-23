#include "console.hpp"
#include "io.hpp"

namespace Manco {
    Console console;

    static constexpr uint16_t COM1 = 0x3F8;

    void init_serial() {
        outb(COM1 + 1, 0x00); // sem interrupções
        outb(COM1 + 3, 0x80); // DLAB on
        outb(COM1 + 0, 0x03); // divisor 3 = 38400 baud
        outb(COM1 + 1, 0x00);
        outb(COM1 + 3, 0x03); // 8N1
        outb(COM1 + 2, 0xC7); // FIFO
    }

    static void serial_put(char c) {
        while (!(inb(COM1 + 5) & 0x20)) {}
        outb(COM1, c);
    }

    void Console::clear(Color bg) {
        for (int y = 0; y < HEIGHT; y++) {
            for (int x = 0; x < WIDTH; x++) {
                const int index = y * WIDTH + x;
                buffer()[index] = ((uint16_t)bg << 12) | ' ';
            }
        }
        column = 0;
        row = 0;
    }

    void Console::scroll() {
        uint16_t* vga = buffer();
        for (int i = 0; i < (HEIGHT - 1) * WIDTH; i++) {
            vga[i] = vga[i + WIDTH];
        }
        // última linha em branco, mantendo a cor de fundo que já estava lá
        for (int x = 0; x < WIDTH; x++) {
            vga[(HEIGHT - 1) * WIDTH + x] = (vga[(HEIGHT - 1) * WIDTH + x] & 0xF000) | ' ';
        }
        row = HEIGHT - 1;
    }

    void Console::put_char(char c, Color fg, Color bg) {
        serial_put(c);

        if (c == '\n') {
            column = 0;
            row++;
        } else {
            const int index = row * WIDTH + column;
            uint16_t attribute = ((uint8_t)bg << 4) | ((uint8_t)fg & 0x0F);
            buffer()[index] = (attribute << 8) | (uint8_t)c;
            column++;
        }

        if (column >= WIDTH) {
            column = 0;
            row++;
        }
        if (row >= HEIGHT) {
            scroll();
        }
    }

    void Console::print(const char* str, Color fg, Color bg) {
        for (int i = 0; str[i] != '\0'; i++) {
            put_char(str[i], fg, bg);
        }
    }

    void Console::print_dec(uint32_t value, int width, Color fg) {
        char digits[10];
        int n = 0;
        do {
            digits[n++] = '0' + (value % 10);
            value /= 10;
        } while (value > 0);

        for (int i = n; i < width; i++) put_char(' ', fg);
        while (n > 0) put_char(digits[--n], fg);
    }

    void Console::print_hex(uint32_t value, Color fg) {
        const char* hex = "0123456789ABCDEF";
        print("0x", fg);
        for (int shift = 28; shift >= 0; shift -= 4) {
            put_char(hex[(value >> shift) & 0xF], fg);
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
