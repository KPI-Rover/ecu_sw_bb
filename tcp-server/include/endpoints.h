#ifndef ENDPOINTS_H
#define ENDPOINTS_H

#include "config.h"
#include "motorsProcessor.h"

size_t set_motor_speed(MotorProcessor *commandProcessor, char* buffer);
size_t set_all_motors_speed(MotorProcessor *commandProcessor, char* buffer);

#endif