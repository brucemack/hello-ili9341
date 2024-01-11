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

#include "pico/stdlib.h"

#include "../ili9341.h"
#include "../textmode.h"

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
    ili9341_init(0, &ili9341_config);

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
        
    gpio_put(LED_PIN, 1);
    sleep_ms(100);
    gpio_put(LED_PIN, 0);
    sleep_ms(100);

    puts("Hello TFT!\n");
    
    mode0_set_cursor(0, 0);

    mode0_color_t fg = MODE0_WHITE;
    mode0_color_t bg = MODE0_BLACK;
    
    int i = 0; 
    while (true) {

        i++;

        char text[64];
        sprintf(text, "Hello Izzy and Henry %d!\n", i);
        mode0_print(text);
        
        fg = (fg+1) % 16;
        if (fg == 0) {
            bg = (bg+1) % 16;
            mode0_set_background(bg);
        }
        mode0_set_foreground(fg);
    }
}
