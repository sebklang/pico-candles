cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(test_project C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
pico_sdk_init()

add_executable(fire
    fire.c
)

pico_enable_stdio_usb(fire 1)
pico_add_extra_outputs(fire)
target_link_libraries(fire pico_stdlib hardware_pwm)