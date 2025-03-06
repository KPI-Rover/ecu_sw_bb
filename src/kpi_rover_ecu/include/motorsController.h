#ifndef MOTORSCONTROLLER_H
#define MOTORSCONTROLLER_H

#include "config.h"
#include "motor.h"

class MotorController {
public:
    Motor* motors;
    
    int init(const int* motorsArray);
    
    int setMotorRPM(int channel, int newRPM);
    int stopMotor(int channel);

    int getMotorRPM(int channel);

    void destroy();


};




#endif