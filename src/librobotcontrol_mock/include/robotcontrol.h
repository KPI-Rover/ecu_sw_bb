#ifndef ROBOTCONTROL_H
#define ROBOTCONTROL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "gpio.h"
#include "motor.h"
#include "encoder.h"
#include <unistd.h>

// Common return codes
#define RBCTL_OK    0
#define RBCTL_ERROR -1

// Sleep utility function
static inline void rc_usleep(unsigned int us) {
    usleep(us);
}

// Process control functions
int rc_kill_existing_process(float timeout_s);
int rc_enable_signal_handler(void);

#ifdef __cplusplus
}
#endif

#endif // ROBOTCONTROL_H
