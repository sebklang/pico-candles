#include <array>
#include "candle_pin.hpp"
#include "crystal_pin.hpp"

int main()
{
    std::array<base_pin*, 4> pins = {
        new candle_pin(0),
        new candle_pin(6),
        new candle_pin(10),

        new crystal_pin(28)
    };

    // Main program loop
    while (true)
    {
        for (uint i = 0; i < pins.size(); i++)
            pins[i]->iterate();

        sleep_ms(40);
    }
}
