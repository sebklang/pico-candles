cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(test_project C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
pico_sdk_init()

add_executable(fire
    main.cpp
    base_pin.hpp
    base_pin.cpp
    candle_pin.hpp
    candle_pin.cpp
    crystal_pin.hpp
    crystal_pin.cpp
)

pico_enable_stdio_usb(fire 1)
pico_add_extra_outputs(fire)
target_link_libraries(fire pico_stdlib hardware_pwm)