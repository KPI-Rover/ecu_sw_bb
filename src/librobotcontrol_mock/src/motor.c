#include "motor.h"
#include <stdint.h>

static double motor_states[RC_MOTOR_MAX_CHANNELS] = {0.0};
static int motor_initialized = 0;

int rc_motor_init(void) {
    motor_initialized = 1;
    for(int i = 0; i < RC_MOTOR_MAX_CHANNELS; i++) {
        motor_states[i] = 0.0;
    }
    return 0;
}

int rc_motor_init_freq(int frequency) {
    if (frequency < 0) return -1;
    motor_initialized = 1;
    for(int i = 0; i < RC_MOTOR_MAX_CHANNELS; i++) {
        motor_states[i] = 0.0;
    }
    return 0;
}

int rc_motor_cleanup(void) {
    motor_initialized = 0;
    for(int i = 0; i < RC_MOTOR_MAX_CHANNELS; i++) {
        motor_states[i] = 0.0;
    }
    return 0;
}

int rc_motor_set(int motor, double duty) {
    if (!motor_initialized) return -1;
    if (motor < 0 || motor >= RC_MOTOR_MAX_CHANNELS) return -1;
    if (duty > 1.0) duty = 1.0;
    if (duty < -1.0) duty = -1.0;
    motor_states[motor] = duty;
    return 0;
}

int rc_motor_brake(int motor) {
    if (!motor_initialized) return -1;
    if (motor < 0 || motor >= RC_MOTOR_MAX_CHANNELS) return -1;
    motor_states[motor] = 0.0;
    return 0;
}

int rc_motor_free_spin(int motor) {
    if (!motor_initialized) return -1;
    if (motor < 0 || motor >= RC_MOTOR_MAX_CHANNELS) return -1;
    return 0;
}

double rc_motor_get_duty(int motor) {
    if (!motor_initialized) return 0.0;
    if (motor < 0 || motor >= RC_MOTOR_MAX_CHANNELS) return 0.0;
    return motor_states[motor];
}
