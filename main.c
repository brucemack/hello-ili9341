/*
Hello World ILI9341 (TFT Display)

# Make sure that the PICO_SDK_PATH is set properly

cd /home/bruce/pico/hello-ili9341
# You need main.c and CMakeLists.txt
cp ../pico-sdk/external/pico_sdk_import.cmake .
mkdir build
cd build
cmake ..
make
# Connect the PI PICO to USB while pressing the BOOTSEL button.
# And then:
cp main.uf2 /media/bruce/RPI-RP2

# Make sure the SWD is connected properly:
# GPIO24 (Pin 18) to SWDIO
# GPIO25 (Pin 22) to SWCLK
# GND (Pin 20) to SWGND

# Use this command to flash:
openocd -f interface/raspberrypi-swd.cfg -f target/rp2040.cfg -c "program main.elf verify reset exit"

# Looking at the serial port:
minicom -b 115200 -o -D /dev/ttyACM0
*/
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "pico/binary_info.h"

#define I2C0_SDA 4 // Pin 6: I2C channel 0 - data
#define I2C0_SCL 5 // Pin 7: I2C channel 0 - clock

const uint LED_PIN = 25;

int main() {
 
    stdio_init_all();

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
        
    gpio_put(LED_PIN, 1);
    sleep_ms(1000);
    gpio_put(LED_PIN, 0);
    sleep_ms(1000);

    puts("Hello World\n");
    
    while (1) {
        gpio_put(LED_PIN, 0);
        sleep_ms(1000);
        gpio_put(LED_PIN, 1);
        sleep_ms(1000);
    }
}

