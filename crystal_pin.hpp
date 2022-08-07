#ifndef CRYSTAL_PIN_H
#define CRYSTAL_PIN_H

#include "pico/stdlib.h"
#include "base_pin.hpp"

uint16_t const PULSATE_POWER_MIN = 5000;
uint16_t const PULSATE_POWER_MAX = 38000;
uint16_t const PULSATE_DESCENDING_STEPSIZE = 1200;
uint16_t const PULSATE_ASCENDING_STEPSIZE = 400;

enum crystal_state
{
    PULSATE_DESCENDING,
    PULSATE_ASCENDING
};

struct crystal_pin : public base_pin
{
    uint16_t const pulsate_speed_delta;
    crystal_state state;

    crystal_pin(uint pin_num);

    void iterate();
};

#endif
