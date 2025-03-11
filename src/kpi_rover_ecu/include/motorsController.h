#ifndef MOTORSCONTROLLER_H
#define MOTORSCONTROLLER_H

#include "config.h"
#include "motor.h"
#include "motorConfig.h"

class MotorController {
public:
    Motor* motors;
    uint8_t motorNumber;
    
    int init(MotorConfig _motors[]);
    
    int setMotorRPM(int channel, int newRPM);
    int stopMotor(int channel);

    int getMotorRPM(int channel);

    void destroy();


};

#endif