#include "gpio.h"
#include <stdint.h>

static uint8_t gpio_states[RC_GPIO_MAX_PINS] = {0};
static rc_gpio_mode_t gpio_modes[RC_GPIO_MAX_PINS] = {RC_GPIO_INPUT};

int rc_gpio_init(int pin, rc_gpio_mode_t mode) {
    if (pin < 0 || pin >= RC_GPIO_MAX_PINS) return -1;
    gpio_modes[pin] = mode;
    return 0;
}

int rc_gpio_cleanup(int pin) {
    if (pin < 0 || pin >= RC_GPIO_MAX_PINS) return -1;
    gpio_states[pin] = 0;
    gpio_modes[pin] = RC_GPIO_INPUT;
    return 0;
}

int rc_gpio_set_value(int pin, int value) {
    if (pin < 0 || pin >= RC_GPIO_MAX_PINS) return -1;
    if (gpio_modes[pin] != RC_GPIO_OUTPUT) return -1;
    gpio_states[pin] = value ? 1 : 0;
    return 0;
}

int rc_gpio_get_value(int pin) {
    if (pin < 0 || pin >= RC_GPIO_MAX_PINS) return -1;
    return gpio_states[pin];
}
