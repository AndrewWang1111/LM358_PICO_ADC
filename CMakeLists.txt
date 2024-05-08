# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)
set(PICO_SDK_PATH "/home/wjl/pico/pico-sdk")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(linkList C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(linkList linkList.cpp  ssd1306.c AudioFFT.cpp)

pico_set_program_name(linkList "linkList")
pico_set_program_version(linkList "0.1")

pico_enable_stdio_uart(linkList 1)
pico_enable_stdio_usb(linkList 0)

# Add the standard library to the build
target_link_libraries(linkList pico_stdlib hardware_adc hardware_i2c  hardware_vreg hardware_dma)

pico_add_extra_outputs(linkList)

