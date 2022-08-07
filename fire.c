#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include <stdlib.h>

#define len(arr) (sizeof(arr) / sizeof(arr[0]))

uint32_t const SLEEP_TIME_MS = 40;
uint16_t const CANDLE_POWER_DELTAWINDOW = 7500;

uint16_t const FLICKER_POWER_MIN = 12000;
uint16_t const FLICKER_POWER_MAX = 65530;
uint16_t const FLICKER_DESCENDING_STEPSIZE = 12000;
uint16_t const FLICKER_ASCENDING_STEPSIZE = 20000;
uint16_t const FLICKER_RESETTING_STEPSIZE = 20000;

uint16_t const PULSATE_POWER_MIN = 5000; //7000;
uint16_t const PULSATE_POWER_MAX = 38000;
uint16_t const PULSATE_DESCENDING_STEPSIZE = 1200;
uint16_t const PULSATE_ASCENDING_STEPSIZE = 400;

enum pin_state
{
    CANDLE_IDLE,

    FLICKER_DESCENDING,
    FLICKER_ASCENDING,
    FLICKER_RESETTING,

    PULSATE_DESCENDING,
    PULSATE_ASCENDING
};

struct pwm_pin
{
    uint slice_num;                 // unchanging
    enum pwm_chan channel;          // unchanging
    uint16_t pulsate_speed_delta;
    uint16_t flicker_init_power;    // occasionally changing
    int8_t flicker_reset_direction; // occasionally changing; 1 or -1
    enum pin_state state;           // regularly changing
    uint16_t power;                 // constantly changing
};

int main()
{
    // Freely choosable and only used in automating the `pins` initialization
    uint const pin_nums[] = {0, 6, 10, 22, 28};
    uint const candle_pin_nums[] = {0, 6, 10}; // Must be subset of pin_nums
    uint const crystal_pin_nums[] = {22, 28};   // Must be subset of pin_nums

    // Declare and initialize general pin properties
    struct pwm_pin pins[len(pin_nums)];
    for (uint i = 0; i < len(pins); i++)
    {
        gpio_set_function(pin_nums[i], GPIO_FUNC_PWM);
        pins[i].slice_num = pwm_gpio_to_slice_num(pin_nums[i]);
        pins[i].channel = pwm_gpio_to_channel(pin_nums[i]);
        pwm_set_wrap(pins[i].slice_num, 65535);
        pwm_set_enabled(pins[i].slice_num, true);
    }
    
    // Initialize candle pin properties
    for (uint i = 0; i < len(candle_pin_nums); i++)
    {
        pins[i].power = 30000; // initial value
        pins[i].flicker_init_power = 0; // arbitrary
        pins[i].pulsate_speed_delta = 0; // unused
        pins[i].state = CANDLE_IDLE;
    }

    // Initialize crystal pin properties
    for (uint i = len(candle_pin_nums); i < len(candle_pin_nums) + len(crystal_pin_nums); i++)
    {
        pins[i].power = rand() % 65535;
        pins[i].flicker_init_power = 0; // unused
        pins[i].pulsate_speed_delta = 0; // (rand() % 700) - 350; // unchanging
        pins[i].state = (rand() & 1) ? PULSATE_ASCENDING : PULSATE_DESCENDING;
    }

    // Main program loop
    while (true)
    {
        for (uint i = 0; i < len(pins); i++)
        {
            struct pwm_pin *const pin = &pins[i];
            int const randr = rand();
            switch (pin->state)
            {
            // ========== CANDLE STATES ==========
            
            case CANDLE_IDLE:
                pin->power += randr % CANDLE_POWER_DELTAWINDOW
                            - (CANDLE_POWER_DELTAWINDOW * pin->power >> 16); // MAX_POWER was here

                if (randr < RAND_MAX / 150) // (200) divide by desired avg number of updates before flicker
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
                    pin->state = CANDLE_IDLE;
                break;

            // ========== CRYSTAL STATES ==========

            case PULSATE_DESCENDING:
                if (pin->power > PULSATE_POWER_MIN + PULSATE_DESCENDING_STEPSIZE + pin->pulsate_speed_delta)
                    pin->power -= PULSATE_DESCENDING_STEPSIZE - pin->pulsate_speed_delta;
                else
                    pin->state = PULSATE_ASCENDING;
                break;

            case PULSATE_ASCENDING:
                if (pin->power < PULSATE_POWER_MAX - PULSATE_ASCENDING_STEPSIZE - pin->pulsate_speed_delta)
                    pin->power += PULSATE_ASCENDING_STEPSIZE + pin->pulsate_speed_delta;
                else
                    pin->state = PULSATE_DESCENDING;
                break;

            default:
                return 1;
            }

            pwm_set_chan_level(pin->slice_num, pin->channel, pin->power);
        }

        sleep_ms(SLEEP_TIME_MS);
    }
}
