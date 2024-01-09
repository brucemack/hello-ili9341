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

#include "textmode.h"

const uint LED_PIN = 25;

int main() {

    mode0_init();

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
        
    gpio_put(LED_PIN, 1);
    sleep_ms(1000);
    gpio_put(LED_PIN, 0);
    sleep_ms(1000);

    puts("Hello TFT!\n");
    
    mode0_set_cursor(0, 0);

    mode0_color_t fg = MODE0_WHITE;
    mode0_color_t bg = MODE0_BLACK;
    
    for (int i = 0; i < 24; i++) {

        char text[64];
        sprintf(text, "Hello Izzy and Henry %d!\n", i)
        mode0_print(text);
        
        //sleep_ms(500);
        fg = (fg+1) % 16;
        if (fg == 0) {
            bg = (bg+1) % 16;
            mode0_set_background(bg);
        }
        mode0_set_foreground(fg);
    }

    // Pause
    sleep_ms(2000);

    // Scroll
    mode00_scroll_test(0x0080);

    // Don't exit
    while (true) {
        sleep_ms(500);
    }
}
