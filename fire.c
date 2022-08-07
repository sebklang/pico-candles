#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include <stdlib.h>

uint     const NUM_PINS = 3;
uint32_t const SLEEP_TIME_MS = 40;
uint16_t const POWER_DELTAWINDOW = 7500;

uint16_t const FLICKER_POWER_MIN = 12000;
uint16_t const FLICKER_POWER_MAX = 60000;
uint16_t const FLICKER_DESCENDING_STEPSIZE = 12000;
uint16_t const FLICKER_ASCENDING_STEPSIZE = 20000;
uint16_t const FLICKER_RESETTING_STEPSIZE = 20000;

enum pin_state
{
    IDLE,
    FLICKER_DESCENDING,
    FLICKER_ASCENDING,
    FLICKER_RESETTING
};

struct pwm_pin
{
    uint slice_num;                 // unchanging
    enum pwm_chan channel;          // unchanging
    uint16_t flicker_init_power;    // occasionally changing
    int8_t flicker_reset_direction; // occasionally changing; 1 or -1
    enum pin_state state;           // regularly changing
    uint16_t power;                 // constantly changing
};

int main()
{
    // Freely choosable and only used in automating the `pins` initialization
    uint const pin_nums[] = {0, 6, 10};

    // Declare and initialize pins
    struct pwm_pin pins[NUM_PINS];
    for (uint i = 0; i < NUM_PINS; i++)
    {
        struct pwm_pin *const pin = &pins[i];
        gpio_set_function(pin_nums[i], GPIO_FUNC_PWM);
        pin->slice_num = pwm_gpio_to_slice_num(pin_nums[i]);
        pin->channel = pwm_gpio_to_channel(pin_nums[i]);
        pin->power = 30000; // initial value
        pin->flicker_init_power = 0; // arbitrary
        pin->state = IDLE;
        pwm_set_wrap(pin->slice_num, 65535);
        pwm_set_enabled(pin->slice_num, true);
    }

    // Main program loop
    while (true)
    {
        for (uint i = 0; i < NUM_PINS; i++)
        {
            struct pwm_pin *const pin = &pins[i];
            int const randr = rand();
            switch (pin->state)
            {
            case IDLE:
                pin->power += randr % POWER_DELTAWINDOW
                            - (POWER_DELTAWINDOW * pin->power >> 16); // MAX_POWER was here

                if (randr < RAND_MAX / 200) // divide by desired avg number of updates before flicker
                {
                    pin->flicker_init_power = pin->power;
                    pin->flicker_reset_direction = FLICKER_POWER_MAX < pin->power ? 1 : -1;
                    pin->state = FLICKER_DESCENDING;
                }
                break;

            case FLICKER_DESCENDING:
                if (pin->power > FLICKER_POWER_MIN + FLICKER_DESCENDING_STEPSIZE)
                    pin->power -= FLICKER_DESCENDING_STEPSIZE;
                else
                    pin->state = FLICKER_ASCENDING;
                break;

            case FLICKER_ASCENDING:
                if (pin->power < FLICKER_POWER_MAX - FLICKER_ASCENDING_STEPSIZE)
                    pin->power += FLICKER_ASCENDING_STEPSIZE;
                else
                {
                    pin->power = FLICKER_POWER_MAX; // might be necessary to prevent edge case bugs
                    pin->state = FLICKER_RESETTING; 
                }
                break;

            case FLICKER_RESETTING:
                if (abs(pin->power - pin->flicker_init_power) > FLICKER_RESETTING_STEPSIZE)
                    pin->power += pin->flicker_reset_direction * FLICKER_RESETTING_STEPSIZE;
                else
                    pin->state = IDLE;
                break;

            default:
                pin->state = IDLE;
                break;
            }

            pwm_set_chan_level(pin->slice_num, pin->channel, pin->power);
        }

        sleep_ms(SLEEP_TIME_MS);
    }
}
