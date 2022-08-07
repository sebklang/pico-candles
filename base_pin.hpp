#ifndef BASE_PIN_H
#define BASE_PIN_H

#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"

uint32_t const SLEEP_TIME_MS = 40;

struct base_pin
{
    uint const slice_num;
    uint const channel;
    uint16_t power;

    base_pin(uint pin_num);
    virtual ~base_pin();

    virtual void iterate() = 0;
};

#endif
