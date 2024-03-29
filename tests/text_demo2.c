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

#include "../ili9341.h"
#include "../font_0.h"

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

    puts("Text demonstration 2\n");

    ili9341_init(0, &ili9341_config);

    ili9341_clear();

    const char* msg0 = "Hello Izzy!";
    ili9341_render_text(msg0, 
        (uint16_t)0xffff, ili9341_makeRGB(0, 0, 0b11111), 
        0, 0, 30,
        6, 10, font_0_data);

    const char* msg1 = "Hello Henry!";
    ili9341_render_text(msg1, 
        (uint16_t)0xffff, ili9341_makeRGB(0, 0b111111, 0), 
        0, 1, 30,
        6, 10, font_0_data);
 
    // Don't exit
    while (true) {
        sleep_ms(500);
    }
}
