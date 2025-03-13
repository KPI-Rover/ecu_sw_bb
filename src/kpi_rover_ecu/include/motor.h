#ifndef MOTOR_H
#define MOTOR_H

#include "config.h"

class Motor {
   public:
    Motor(int assignedNumber, bool isInverted);
    int motorGo(int newRPM);
    int motorStop();
    int getRPM();

   private:
    int motorNumber;
    bool inverted;
    double currentDutyCycle;
    double getDC(int entryRPM);
};

#endif  // MOTOR_H