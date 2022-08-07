#include "base_pin.hpp"

base_pin::base_pin(uint pin_num)
    : slice_num(pwm_gpio_to_slice_num(pin_num))
    , channel(pwm_gpio_to_channel(pin_num))
    , power(rand() % 65536)
{
    gpio_set_function(pin_num, GPIO_FUNC_PWM);
    pwm_set_wrap(slice_num, 65535);
    pwm_set_enabled(slice_num, true);
}

base_pin::~base_pin() {}
