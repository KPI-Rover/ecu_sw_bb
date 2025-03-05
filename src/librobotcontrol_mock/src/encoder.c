#include "encoder.h"
#include <stdint.h>

static int encoder_positions[RC_ENCODER_MAX_CHANNELS] = {0};
static double encoder_velocities[RC_ENCODER_MAX_CHANNELS] = {0.0};
static int velocities_enabled = 0;
static int encoder_initialized = 0;

int rc_encoder_init(void) {
    encoder_initialized = 1;
    for(int i = 0; i < RC_ENCODER_MAX_CHANNELS; i++) {
        encoder_positions[i] = 0;
        encoder_velocities[i] = 0.0;
    }
    return 0;
}

int rc_encoder_cleanup(void) {
    encoder_initialized = 0;
    velocities_enabled = 0;
    return 0;
}

int rc_encoder_read(int ch) {
    if (!encoder_initialized) return -1;
    if (ch < 0 || ch >= RC_ENCODER_MAX_CHANNELS) return -1;
    return encoder_positions[ch];
}

int rc_encoder_write(int ch, int pos) {
    if (!encoder_initialized) return -1;
    if (ch < 0 || ch >= RC_ENCODER_MAX_CHANNELS) return -1;
    encoder_positions[ch] = pos;
    return 0;
}

int rc_encoder_enable_velocities(int hz) {
    if (!encoder_initialized) return -1;
    if (hz < 1) return -1;
    velocities_enabled = 1;
    return 0;
}

int rc_encoder_disable_velocities(void) {
    velocities_enabled = 0;
    return 0;
}

double rc_encoder_get_velocity(int ch) {
    if (!encoder_initialized || !velocities_enabled) return 0.0;
    if (ch < 0 || ch >= RC_ENCODER_MAX_CHANNELS) return 0.0;
    return encoder_velocities[ch];
}
