#ifndef RC_GPIO_H
#define RC_GPIO_H

#ifdef __cplusplus
extern "C" {
#endif

#define RC_GPIO_MAX_PINS 128

typedef enum {
    RC_GPIO_INPUT  = 0,
    RC_GPIO_OUTPUT = 1
} rc_gpio_mode_t;

int rc_gpio_init(int pin, rc_gpio_mode_t mode);
int rc_gpio_cleanup(int pin);
int rc_gpio_set_value(int pin, int value);
int rc_gpio_get_value(int pin);

#ifdef __cplusplus
}
#endif

#endif
