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

// Connection to the PI. 
//
// IMPORTANT: Please remember that these are GP# numbers, not 
// the physical pin numbers!
//
ili9341_config_t ili9341_config = {
    .port = spi0,
    .pin_sck =   18,
    .pin_mosi =  19,  // (SPI_TX)
    .pin_miso =  16,  // (SPI_RX)
    .pin_cs =    17,
    .pin_reset = 14,
    .pin_dc =    15 
};

int main() {

    stdio_init_all();

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
        
    gpio_put(LED_PIN, 1);
    sleep_ms(1000);
    gpio_put(LED_PIN, 0);
    sleep_ms(1000);

    puts("Waterfall demonstration 2\n");

    ili9341_init(0, &ili9341_config);

    // Clear and setup top/bottom borders

    // The column address range will always be the same
    ili9341_set_command(ILI9341_CASET);
    ili9341_command_param16(0x00);
    ili9341_command_param16(239); 

    ili9341_set_command(ILI9341_PASET);
    ili9341_command_param16(0);
    ili9341_command_param16(319);

    ili9341_set_command(ILI9341_RAMWR);

    uint16_t fg0 = makeRGB(0, 0b111111, 0); 
    uint16_t fg1 = 0xffff; 

    uint16_t buffer[240];

    for (int i = 0; i < 320; i++) {
        for (int j = 0; j < 240; j++) {
            if (i < 10) {
                buffer[j] = fg0;
            } else if (i >= 310) {
                buffer[j] = fg1;
            } else {
                buffer[j] = 0;
            }
        }
        ili9341_write_data(buffer, 240 * 2);
    }

    // Setup the scroll range
    ili9341_set_command(0x33);
    // Top 10 pages will be untouched
    ili9341_command_param16(10);
    // Middle area is scrolled
    ili9341_command_param16(320 - 20);
    // Bottom 10 pages will be untouched
    ili9341_command_param16(10);

    // The starting Y location is at the bottom of the "middle area"
    int y = 10;
    int x = 120;

    while (true) {

        // Scroll so that the current row is at the bottom of the middle
        // area.
        ili9341_set_command(0x37);
        ili9341_command_param16(y);

        // Notice that we are moving to different page locations each time.
        // This is necessary to achieve the scrolling visual
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

        // Wrap around, avoiding the top and bottom areas
        y = y + 1;
        if (y > 309) {
           y = 10;
        }

        // Drift around
        if (rand() % 10 >= 5) {
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
        sleep_ms(5);
    }
}

