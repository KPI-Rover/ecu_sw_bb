#ifndef MOTOR_H
#define MOTOR_H

#include "config.h"

class Motor {
public:
    int motorNumber;

    Motor(int assignedNumber);
    int motorGo(int newRPM);
    int motorStop();
    int getRPM();

private:
    double currentDutyCycle;
    double getDC(int entryRPM);
};

#endif // MOTOR_H