#ifndef RC_MOTOR_H
#define RC_MOTOR_H

#ifdef __cplusplus
extern "C" {
#endif

#define RC_MOTOR_MAX_CHANNELS 4
#define RC_MOTOR_DEFAULT_PWM_FREQ 25000

int rc_motor_init(void);
int rc_motor_cleanup(void);
int rc_motor_set(int motor, double duty);
int rc_motor_brake(int motor);
int rc_motor_free_spin(int motor);
double rc_motor_get_duty(int motor);
int rc_motor_init_freq(int frequency);

#ifdef __cplusplus
}
#endif

#endif
