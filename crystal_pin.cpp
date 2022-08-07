#include "crystal_pin.hpp"

crystal_pin::crystal_pin(uint pin_num)
    : base_pin(pin_num)
    , pulsate_speed_delta(0)
    , state((rand() & 1) ? PULSATE_DESCENDING : PULSATE_ASCENDING)
    // Above line may cause uint16_t overflow if for ex. state is
    // set to PULSATE_DESCENDING and the initial power to a value
    // less than PULSATE_POWER_MIN, or vice versa. However, this
    // is not a serious problem and can only happen at powerup.
{}

void crystal_pin::iterate()
{
    int const randr = rand();
    switch (state)
    {
    case PULSATE_DESCENDING:
        if (power > PULSATE_POWER_MIN + PULSATE_DESCENDING_STEPSIZE + pulsate_speed_delta)
            power -= PULSATE_DESCENDING_STEPSIZE - pulsate_speed_delta;
        else
            state = PULSATE_ASCENDING;
        break;

    case PULSATE_ASCENDING:
        if (power < PULSATE_POWER_MAX - PULSATE_ASCENDING_STEPSIZE - pulsate_speed_delta)
            power += PULSATE_ASCENDING_STEPSIZE + pulsate_speed_delta;
        else
            state = PULSATE_DESCENDING;
        break;
    }

    pwm_set_chan_level(slice_num, channel, power);
}
