#ifndef MOTORSCONTROLLER_H
#define MOTORSCONTROLLER_H

#include "config.h"
#include "motor.h"
#include "motorConfig.h"

class MotorController {
   public:
    vector<Motor> motors;
    uint8_t motorNumber;

    int init(MotorConfig _motors[], uint8_t _motorNumber);

    int setMotorRPM(int channel, int newRPM);
    int stopMotor(int channel);

    int getMotorRPM(int channel);

    void destroy();
};

#endif