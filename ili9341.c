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
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "pico/stdlib.h"

#include "ili9341.h"


// TFT Display Module Connections:
//
// (pin 1) VCC        5V/3.3V power input
// (pin 2) GND        Ground
// (pin 3) CS         LCD chip select signal, low level enable
// (pin 4) RESET      LCD reset signal, low level reset
// (pin 5) DC/RS      LCD register / data selection signal; high level: register, low level: data
// (pin 6) SDI(MOSI)  SPI bus write data signal
// (pin 7) SCK        SPI bus clock signal
// (pin 8) LED        Backlight control; if not controlled, connect 3.3V always bright
// (pin 9) SDO(MISO)  SPI bus read data signal; optional

// Connection to the PI. 
//
// IMPORTANT: Please remember that these are GP# numbers, not 
// the physical pin numbers!
//
ili9341_config_t ili9341_config = {
    .port = spi0,
    .pin_miso = 4,
    .pin_cs = 5,
    .pin_sck = 2,
    .pin_mosi = 3,
    .pin_reset = 6,
    .pin_dc = 7 
};

static inline void cs_select() {
    asm volatile("nop \n nop \n nop");
    gpio_put(ili9341_config.pin_cs, 0);  // Active low
    asm volatile("nop \n nop \n nop");
}

static inline void cs_deselect() {
    asm volatile("nop \n nop \n nop");
    gpio_put(ili9341_config.pin_cs, 1);
    asm volatile("nop \n nop \n nop");
}

void ili9341_set_command(uint8_t cmd) {
    cs_select();
    gpio_put(ili9341_config.pin_dc, 0);
    spi_write_blocking(ili9341_config.port, &cmd, 1);
    gpio_put(ili9341_config.pin_dc, 1);
    cs_deselect();
}

void ili9341_command_param(uint8_t data) {
    cs_select();
    spi_write_blocking(ili9341_config.port, &data, 1);
    cs_deselect();
}

void ili9341_command_param16(uint16_t data) {
    uint8_t hi = (data >> 8);
    uint8_t lo = (data & 0xff);
    cs_select();
    spi_write_blocking(ili9341_config.port, &hi, 1);
    spi_write_blocking(ili9341_config.port, &lo, 1);
    cs_deselect();
}

void ili9341_write_data(void *buffer, int bytes) {
    cs_select();
    spi_write_blocking(ili9341_config.port, buffer, bytes);
    cs_deselect();
}

void ili9341_write_data_continuous(void *buffer, int bytes) {
    spi_write_blocking(ili9341_config.port, buffer, bytes);
}

void ili9341_init(int mode) {

    // Configure the SPI port to run at at 0.5 MHz.
    spi_init(ili9341_config.port, 500 * 1000);
    // TODO: UNDERSTAMD THIS
    spi_set_baudrate(ili9341_config.port, 75000 * 1000);

    // Configure the pins that are being used for the SPI bus
    gpio_set_function(ili9341_config.pin_miso, GPIO_FUNC_SPI);
    gpio_set_function(ili9341_config.pin_sck, GPIO_FUNC_SPI);
    gpio_set_function(ili9341_config.pin_mosi, GPIO_FUNC_SPI);

    // Chip select is active-low, so we'll initialise it to a driven-high state
    gpio_init(ili9341_config.pin_cs);
    gpio_set_dir(ili9341_config.pin_cs, GPIO_OUT);
    // TODO: THIS DOESN'T LOOK RIGHT - TEST THE CHANGE!
    gpio_put(ili9341_config.pin_cs, 0);

    // Reset is active-low
    gpio_init(ili9341_config.pin_reset);
    gpio_set_dir(ili9341_config.pin_reset, GPIO_OUT);
    gpio_put(ili9341_config.pin_reset, 1);

    // high = command, low = data
    gpio_init(ili9341_config.pin_dc);
    gpio_set_dir(ili9341_config.pin_dc, GPIO_OUT);
    gpio_put(ili9341_config.pin_dc, 0);

    // Request a hard reset of the ILI9341 
    sleep_ms(10);
    gpio_put(ili9341_config.pin_reset, 0);
    sleep_ms(10);
    gpio_put(ili9341_config.pin_reset, 1);

    // ----------------------------------------------------------------------

    // Software reset (0x01)
    //   "It will be necessary to wait 5msec before sending new command following software reset. The 
    //   display module loads all display supplier factory default values to the registers during 
    //   this 5msec."
    ili9341_set_command(ILI9341_SWRESET);
    // TODO: CHECK SHORTER?
    sleep_ms(100);

    // Gamma Set (0x26) 
    //   "This command is used to select the desired Gamma curve for the current display. A maximum of 
    //   4 fixed gamma curves can be selected."
    ili9341_set_command(ILI9341_GAMMASET);
    // Curve 1 is selected because it's the only one that is built into the chip.
    ili9341_command_param(0x01);

    // Positive gamma correction (0xE0)
    //   "Set the gray scale voltage to adjust the gamma characteristics of the TFT panel."
    ili9341_set_command(ILI9341_GMCTRP1);
    // There are 15 parameters being set.
    // TODO: UNDERSTAND THESE PARAMETERS
    ili9341_write_data((uint8_t[15])
        { 
            0x0f, 0x31, 0x2b, 0x0c, 0x0e, 0x08, 0x4e, 0xf1, 0x37, 0x07, 0x10, 0x03, 0x0e, 0x09, 0x00 
        }, 15);

    // Negative gamma correction (0xE1)
    //   "Set the gray scale voltage to adjust the gamma characteristics of the TFT panel"
    ili9341_set_command(ILI9341_GMCTRN1);
    // There are 15 parameters being set.
    // TODO: UNDERSTAND THESE PARAMETERS
    ili9341_write_data((uint8_t[15])
        { 
            0x00, 0x0e, 0x14, 0x03, 0x11, 0x07, 0x31, 0xc1, 0x48, 0x08, 0x0f, 0x0c, 0x31, 0x36, 0x0f 
        }, 15);

    // Memory access control (0x36)
    //   "This command defines read/write scanning direction of frame memory"
    ili9341_set_command(ILI9341_MADCTL);
    // 0100 1000
    //
    // From page 208 of the datasheet:
    //
    // 010 = "Direct to (239-Physical Column Pointer), Direct to Physical Page Pointer"
    //
    // From MSB to LSB:
    //   MY: Row address order (0)
    //   MX: Column address order (1)
    //   MV: Column/row exchange (0)
    //   ML: Vertical refresh order (0=Sends top first)
    //   BGR: RGB-BGR Order. (1=Blue/Green/Red)
    //   MH: Horizontal Refresh ORDER (0=Sends left first)
    //   X[1:0]: Two unused bits
    if (mode == 0)
        ili9341_command_param(0x48);
    else if (mode == 1)
        ili9341_command_param(0x48);

    // COLMOD: Pixel Format Set (0x3A)
    //   "This command sets the pixel format for the RGB image data used by the interface. DPI [2:0] 
    //   is the pixel format select of RGB interface and DBI [2:0] is the pixel format of MCU 
    //   interface.""
    ili9341_set_command(ILI9341_PIXFMT);
    // 0101 0101
    // From MSB to LSB:
    //   X: Unused
    //   DPI[2:0]: For RGB interface. (101=16 bits per pixel)
    //   X: Unused
    //   DBI[2:0]: For MCE interface.  (101=16 bits per pixel)
    ili9341_command_param(0x05);

    // Frame Rate Control (In Normal Mode/Full Colors) (xB1) 
    ili9341_set_command(ILI9341_FRMCTR1);
    // Parameter 1=0000 0000
    //   0[5:0] Zeroes
    //   DIVA[1:0]: Division ratio (00=set to 1)
    // Parameter 2=0001 1101
    //   0:[2:0] Zeroes
    //   RTNA[4:0]: 70 Hz
    ili9341_command_param(0x00);
    ili9341_command_param(0x1B);

    // Sleep Out (0x11) 
    ili9341_set_command(ILI9341_SLPOUT);

    // Display ON (0x29)
    ili9341_set_command(ILI9341_DISPON);

    // Column Address Set (0x2A) 
    //   "This command is used to define area of frame memory where MCU can access. This command 
    //   makes no change on the other driver status. The values of SC [15:0] and EC [15:0] are 
    //   referred when RAMWR command comes. Each value represents one column line in the 
    //   Frame Memory."
    //
    // Here we set the start column to 0 and the end column to 0239
    //
    ili9341_set_command(ILI9341_CASET);
    // Parameter 1: Start column SC15:SC08
    ili9341_command_param(0x00);
    // Parameter 2: Start column SC07:SC00
    ili9341_command_param(0x00);  
    // Parameter 3: End column EC15:EC08
    ili9341_command_param(0x00);
    // Parameter 4: Edn column EC07:EC00
    ili9341_command_param(0xef);  

    // Page Address Set (0x2B)
    //   "This command is used to define area of frame memory where MCU can access. This command 
    //   makes no change on the other driver status. The values of SP [15:0] and EP [15:0] are 
    //   referred when RAMWR command comes. Each value represents one Page line in the 
    //   Frame Memory."
    // 
    // Here we set the start page to 0 and teh end page to 319
    ili9341_set_command(ILI9341_PASET);
    // Parameter 1: Start page SP15:SP08
    ili9341_command_param(0x00);
    // Parameter 2: Start page SP07:SP00
    ili9341_command_param(0x00);  
    // Parameter 3: End page EP15:EP08
    ili9341_command_param(0x01);
    // Parameter 4: End page EP07:EP00
    ili9341_command_param(0x3f);  
}

uint16_t makeRGB(uint8_t r, uint8_t g, uint8_t b) {
  uint16_t a = ((uint16_t)r << 11) | ((uint16_t)g << 5) | (uint16_t)b;
  // Swap endians
  return a >> 8 | a << 8; 
}
