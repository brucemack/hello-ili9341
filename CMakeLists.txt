cmake_minimum_required(VERSION 3.13)
include(pico_sdk_import.cmake)
project(hello-ili9341 C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(main
  ili9341.c
  font_0.c
  tests/text_demo2.c
)

#add_executable(main
#  ili9341.c
#  tests/waterfall_demo.c
#)

#add_executable(main
#  ili9341.c
#  font_0.c
#  textmode.c
#  tests/textmode_demo.c
#)

pico_enable_stdio_usb(main 1)
pico_enable_stdio_uart(main 1)
target_link_libraries(main pico_stdlib hardware_i2c hardware_spi)

