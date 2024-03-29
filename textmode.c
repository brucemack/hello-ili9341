/*
This program is free software: you can redistribute it and/or modify it under 
the terms of the GNU General Public License as published by the Free 
Software Foundation, either version 3 of the License, or (at your option) any 
later version.

This program is distributed in the hope that it will be useful, but WITHOUT 
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS 
FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with 
this program. If not, see <https://www.gnu.org/licenses/>.
*/
#include <string.h>

#include "pico/stdlib.h"
#include "hardware/spi.h"

#include "ili9341.h"
#include "font_0.h"
#include "textmode.h"

#define TEXT_HEIGHT 24
#define TEXT_WIDTH 53

#define SWAP_BYTES(color) ((uint16_t)(color>>8) | (uint16_t)(color<<8))

static mode0_color_t screen_bg_color = MODE0_BLACK;
static mode0_color_t screen_fg_color = MODE0_WHITE;  // TODO need to store a color per cell
static int cursor_x = 0;
static int cursor_y = 0;
static uint8_t screen[TEXT_HEIGHT * TEXT_WIDTH] = { 0 };
static uint8_t colors[TEXT_HEIGHT * TEXT_WIDTH] = { 0 };
static uint8_t show_cursor = 0;

static int depth = 0;

/*
Color notes:

Transmit format (16-bit):
| R4 R3 R2 R1 R0 G5 G4 G3 | G2 G1 G0 B4 B3 B2 B1 B0 |

The representation below uses the same order as the transmit format.
BUT REMEMBER: Cortex M0 is little endian by default so we need to 
*swap the bytes* to ensure that the bits are streamed out 
on the SPI interface in the same order as they are being typed into
the source code below.
*/
static uint16_t palette[16] = {
    SWAP_BYTES(0x0000), // Black
    SWAP_BYTES(0x49E5), 
    SWAP_BYTES(0xB926), // Red:  10111 001001 00110
    SWAP_BYTES(0xE371),
    SWAP_BYTES(0x9CF3), // Grey: 10011 100111 10011
    SWAP_BYTES(0xA324),
    SWAP_BYTES(0xEC46),
    SWAP_BYTES(0xF70D),
    SWAP_BYTES(0xffff), // White
    SWAP_BYTES(0x1926),
    SWAP_BYTES(0x2A49),
    SWAP_BYTES(0x4443),
    SWAP_BYTES(0xA664),
    SWAP_BYTES(0x02B0), // Blue: 00000 010101 10000
    SWAP_BYTES(0x351E),
    SWAP_BYTES(0xB6FD)
};

void mode0_clear(mode0_color_t color) {
    mode0_begin();
    int size = TEXT_WIDTH*TEXT_HEIGHT;
    memset(screen, 0, size);
    memset(colors, color, size);
    mode0_set_cursor(0, 0);
    mode0_end();
}

void mode0_set_foreground(mode0_color_t color) {
    mode0_begin();
    screen_fg_color = color;
    mode0_end();
}

void mode0_set_background(mode0_color_t color) {
    mode0_begin();
    screen_bg_color = color;
    mode0_end();
}

void mode0_set_cursor(uint8_t x, uint8_t y) {
    cursor_x = x;
    cursor_y = y;
}

void mode0_show_cursor() {
    mode0_begin();
    show_cursor = 1;
    mode0_end();
}

void mode0_hide_cursor() {
    mode0_begin();
    show_cursor = 0;
    mode0_end();
}

uint8_t mode0_get_cursor_x() {
    return cursor_x;
}

uint8_t mode0_get_cursor_y() {
    return cursor_y;
}

void mode0_putc(char c) {
    mode0_begin();
    
    if (cursor_y >= TEXT_HEIGHT) {
        mode0_scroll_vertical(cursor_y-TEXT_HEIGHT+1);
        cursor_y = TEXT_HEIGHT-1;
    }

    int idx = cursor_y*TEXT_WIDTH + cursor_x;
    if (c == '\n') {
        // fill the rest of the line with empty content + the current bg color
        memset(screen+idx, 0, TEXT_WIDTH-cursor_x);
        memset(colors+idx, screen_bg_color, TEXT_WIDTH-cursor_x);
        cursor_y++;
        cursor_x = 0;
    } else if (c == '\r') {
        //cursor_x = 0;
    } else if (c>=32 && c<=127) {
        screen[idx] = c-32;
        colors[idx] = ((screen_fg_color & 0xf) << 4) | (screen_bg_color & 0xf);
        
        cursor_x++;
        if (cursor_x >= TEXT_WIDTH) {
            cursor_x = 0;
            cursor_y++;
        }
    }
    
    mode0_end();
}

void mode0_print(const char *str) {
    mode0_begin();
    char c;
    while (c = *str++) {
        mode0_putc(c);
    }
    mode0_end();
}

void mode0_write(const char *str, int len) {
    mode0_begin();
    for (int i=0; i<len; i++) {
        mode0_putc(*str++);
    }
    mode0_end();
}


inline void mode0_begin() {
    depth++;
}

inline void mode0_end() {
    if (--depth == 0) {
        mode0_draw_screen();
    }
}

void mode0_draw_region(uint8_t x, uint8_t y, uint8_t width, uint8_t height) {
    // TODO
    mode0_draw_screen();
}

void mode0_draw_screen() {
    // assert depth == 0?
    depth = 0;
    
    // This simple demonstration draws the ENTIRE SCREEN each time (based
    // on the text memory that we have)

    // Column address range set
    ili9341_set_command(ILI9341_CASET);
    // SC=0, EC=239
    ili9341_command_param(0x00);
    ili9341_command_param(0x00); 
    ili9341_command_param(0x00);
    ili9341_command_param(0xef); 

    // Page address range set
    ili9341_set_command(ILI9341_PASET);
    // SP=0, EP=319
    ili9341_command_param(0x00);
    ili9341_command_param(0x00);
    ili9341_command_param(0x01);
    ili9341_command_param(0x3f);

    // Start writing into display RAM
    ili9341_set_command(ILI9341_RAMWR);

    // This demo writes one column of text at a time.  That corresponds
    // to 6 pages of 240 columns each (keep in mind that landscape  
    // format is being used here).
    uint16_t buffer[6 * 240];  

    int screen_idx = 0;

    // For each text column
    for (int x = 0; x < TEXT_WIDTH; x++) {

        // Create one column of screen information        
        uint16_t *buffer_idx = buffer;
        
        // The characters are 6 pixels wide
        for (int bit = 0; bit < 6; bit++) {
            uint8_t mask = 64 >> bit;
            // For each text row, STARTING FROM THE BOTTOM
            for (int y = TEXT_HEIGHT - 1; y >=0; y--) {

                // The FG color is stored in the top 4 bits of the local buffer
                uint16_t fg_color = palette[colors[y * TEXT_WIDTH + x] >> 4];
                // The BG color is stored in the bottom 4 bits of the local buffer
                uint16_t bg_color = palette[colors[y * TEXT_WIDTH + x] & 0x0f];
                // Special background override
                if (show_cursor && (cursor_x == x) && (cursor_y == y)) {
                    bg_color = MODE0_GREEN;
                }                                
                
                // Draw the character into the buffer using the font bitmap
                // NOTE: We are loading this buffer from the bottom up!
                const uint8_t character = screen[y * TEXT_WIDTH + x];
                const uint8_t* pixel_data = font_0_data[character];
                for (int j = 10; j >= 1; j--) {
                    *buffer_idx++ = (pixel_data[j] & mask) ? fg_color : bg_color;
                }
            }
        }
        
        // Now send the slice.  Please note that each pixel is 16 bits.  The buffer
        // is stored in row-major format starting from the bottom of the screen.
        ili9341_write_data(buffer, 6 * 240 * 2);
    }
    
    // 53 columns of text * 6 pixels wide for each character means
    // we only sent 318 columns of pixels. In order to avoid leaving
    // garbage on the right of the screen we write black here:
    uint16_t extra_buffer[2 * 240] = { 0 };
    ili9341_write_data(extra_buffer, 2 * 240 * 2);
}

void mode0_scroll_vertical(int8_t amount) {

    mode0_begin();
    
    if (amount > 0) {
        int size1 = TEXT_WIDTH*amount;
        int size2 = TEXT_WIDTH*TEXT_HEIGHT - size1;
        
        memmove(screen, screen+size1, size2);
        memmove(colors, colors+size1, size2);
        memset(screen+size2, 0, size1);
        memset(colors+size2, screen_bg_color, size1);
    } else if (amount < 0) {
        amount = -amount;
        int size1 = TEXT_WIDTH*amount;
        int size2 = TEXT_WIDTH*TEXT_HEIGHT - size1;

        memmove(screen+size1, screen, size2);
        memmove(colors+size1, colors, size2);
        memset(screen, 0, size1);
        memset(colors, screen_bg_color, size1);
    }
    
    mode0_end();
}

void mode0_scroll_test(int rows) {
    // Set scroll range
    ili9341_set_command(0x33);
    ili9341_command_param(0x00);
    ili9341_command_param(0x00); 
    ili9341_command_param(0x01);
    ili9341_command_param(0x40); 
    ili9341_command_param(0x00);
    ili9341_command_param(0x00); 
    // Scroll
    ili9341_set_command(0x37);
    ili9341_command_param(0x00);
    ili9341_command_param(16); 
}
