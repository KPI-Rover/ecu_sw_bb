#ifndef RC_ENCODER_H
#define RC_ENCODER_H

#ifdef __cplusplus
extern "C" {
#endif

#define RC_ENCODER_MAX_CHANNELS 4

int rc_encoder_init(void);
int rc_encoder_cleanup(void);
int rc_encoder_read(int ch);
int rc_encoder_write(int ch, int pos);
int rc_encoder_enable_velocities(int hz);
int rc_encoder_disable_velocities(void);
double rc_encoder_get_velocity(int ch);

#ifdef __cplusplus
}
#endif

#endif
