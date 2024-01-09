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
#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "ili9341.h"

#define SWAP_BYTES(color) ((uint16_t)(color>>8) | (uint16_t)(color<<8))

const uint LED_PIN = 25;

int main() {

    stdio_init_all();

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
        
    gpio_put(LED_PIN, 1);
    sleep_ms(1000);
    gpio_put(LED_PIN, 0);
    sleep_ms(1000);

    puts("Waterfall demonstration 2\n");

    ili9341_init(1);

    // Clear
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

    uint16_t buffer[240] = { 0 };
    for (int i = 0; i < 320; i++) {
        ili9341_write_data(buffer, 240 * 2);
    }

    // Don't exit
    int y = 319;
    int x = 120;

    for (int i = 0; i < 240; i++) {

        // Column address range set
        ili9341_set_command(ILI9341_CASET);
        // SC=0, EC=239
        ili9341_command_param(0x00);
        ili9341_command_param(0x00); 
        ili9341_command_param(0x00);
        ili9341_command_param(0xef); 

        // Page address range set
        ili9341_set_command(ILI9341_PASET);
        ili9341_command_param16(y);
        ili9341_command_param16(y);

        // Start writing into display RAM
        ili9341_set_command(ILI9341_RAMWR);

        uint16_t fg = SWAP_BYTES(0x02B0); // Blue: 00000 010101 10000

        // Write a line of spectrum
        uint16_t buffer[240] = { 0 };
        buffer[x] = fg;
        ili9341_write_data(buffer, 240 * 2);

        y = y + 1;
        if (y > 319) {
            y = 0;
        }

        // Scroll up range
        ili9341_set_command(0x33);
        ili9341_command_param(0x00);
        ili9341_command_param(0x00); 
        ili9341_command_param(0x01);
        ili9341_command_param(0x40); 
        ili9341_command_param(0x00);
        ili9341_command_param(0x00); 

        // Scroll
        ili9341_set_command(0x37);
        ili9341_command_param16(y + 1);

        if (rand() % 10 > 5) {
            x = x + 1;
        } else {
            x = x - 1;
        }

        if (x < 0) {
            x = 0;
        }
        if (x > 239) {
            x = 239;
        }

        sleep_ms(20);
    }

    while (true) {
        sleep_ms(10);
    }
}
