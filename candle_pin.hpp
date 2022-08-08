#ifndef CANDLE_PIN_H
#define CANDLE_PIN_H

#include "base_pin.hpp"

uint16_t const CANDLE_POWER_DELTAWINDOW = 7500;
uint16_t const FLICKER_POWER_MIN = 12000;
uint16_t const FLICKER_POWER_MAX = 65530;
uint16_t const FLICKER_DESCENDING_STEPSIZE = 12000;
uint16_t const FLICKER_ASCENDING_STEPSIZE = 20000;
uint16_t const FLICKER_RESETTING_STEPSIZE = 20000;

enum candle_state
{
    CANDLE_IDLE,
    FLICKER_DESCENDING,
    FLICKER_ASCENDING,
    FLICKER_RESETTING
};

struct candle_pin : public base_pin
{
    uint16_t flicker_init_power;
    int8_t flicker_reset_direction;
    candle_state state;

    candle_pin(uint pin_num);

    virtual void iterate() override;
};

#endif
