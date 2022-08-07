#include "candle_pin.hpp"

candle_pin::candle_pin(uint pin_num)
    : base_pin(pin_num)
    , state(CANDLE_IDLE)
{}

void candle_pin::iterate()
{
    int const randr = rand();
    switch (state)
    {
    case CANDLE_IDLE:
        power += randr % CANDLE_POWER_DELTAWINDOW
                - (CANDLE_POWER_DELTAWINDOW * power >> 16); // MAX_POWER was here

        if (randr < RAND_MAX / 150) // divide by desired avg number of updates before flicker
        {
            flicker_init_power = power;
            flicker_reset_direction = FLICKER_POWER_MAX < power ? 1 : -1;
            state = FLICKER_DESCENDING;
        }
        break;

    case FLICKER_DESCENDING:
        if (power > FLICKER_POWER_MIN + FLICKER_DESCENDING_STEPSIZE)
            power -= FLICKER_DESCENDING_STEPSIZE;
        else
            state = FLICKER_ASCENDING;
        break;

    case FLICKER_ASCENDING:
        if (power < FLICKER_POWER_MAX - FLICKER_ASCENDING_STEPSIZE)
            power += FLICKER_ASCENDING_STEPSIZE;
        else
        {
            power = FLICKER_POWER_MAX; // might be necessary to prevent edge case bugs
            state = FLICKER_RESETTING; 
        }
        break;

    case FLICKER_RESETTING:
        if (abs(power - flicker_init_power) > FLICKER_RESETTING_STEPSIZE)
        power += flicker_reset_direction * FLICKER_RESETTING_STEPSIZE;
    else
        state = CANDLE_IDLE;
        break;
    }

    pwm_set_chan_level(slice_num, channel, power);
}