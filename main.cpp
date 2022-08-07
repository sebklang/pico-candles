#include <array>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "base_pin.hpp"
#include "candle_pin.hpp"
#include "crystal_pin.hpp"

int main()
{
    std::array<base_pin*, 5> pins = {
        new candle_pin(0),
        new candle_pin(6),
        new candle_pin(10),

        new crystal_pin(22),
        new crystal_pin(28)
    };

    // Main program loop
    while (true)
    {
        for (uint i = 0; i < pins.size(); i++)
            pins[i]->iterate();

        sleep_ms(SLEEP_TIME_MS);
    }
}
