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
#include "pico/stdlib.h"
#include <stdio.h>

#define SWAP_BYTES(color) ((uint16_t)(color>>8) | (uint16_t)(color<<8))

const uint LED_PIN = 25;

int main() {

    stdio_init_all();
    ili9341_init(1);

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
        
    gpio_put(LED_PIN, 1);
    sleep_ms(100);
    gpio_put(LED_PIN, 0);
    sleep_ms(100);

    puts("Waterfall demonstration\n");

    // Scroll up range
    ili9341_set_command(0x33);
    ili9341_command_param(0x00);
    ili9341_command_param(0x00); 
    ili9341_command_param(0x01);
    ili9341_command_param(0x40); 
    ili9341_command_param(0x00);
    ili9341_command_param(0x00); 

    // Column address range set
    ili9341_set_command(ILI9341_CASET);
    // SC=0, EC=239
    ili9341_command_param(0x00);
    ili9341_command_param(0x00); 
    ili9341_command_param(0x00);
    ili9341_command_param(0xef); 

    // Page address range set
    ili9341_set_command(ILI9341_PASET);
    // SP=319, EP=319
    ili9341_command_param(0x01);
    ili9341_command_param(0x3f);
    ili9341_command_param(0x01);
    ili9341_command_param(0x3f);

    int i = 0;

    // Don't exit
    while (true) {

        // Start writing into display RAM
        ili9341_set_command(ILI9341_RAMWR);

        int16_t fg = SWAP_BYTES(0x02B0); // Blue: 00000 010101 10000

        // Write a line of spectrum
        uint16_t buffer[240] = { 0 };
        buffer[i++] = 0xffff;
        ili9341_write_data(buffer, 240 * 2);

        // Scroll
        ili9341_set_command(0x37);
        ili9341_command_param(0x00);
        ili9341_command_param(0x01); 

        sleep_ms(200);
    }
}
