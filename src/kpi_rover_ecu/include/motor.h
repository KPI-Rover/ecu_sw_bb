#ifndef MOTOR_H
#define MOTOR_H

#include "config.h"

class Motor {
   public:
    Motor(int assignedNumber, bool isInverted);
    int MotorGo(int newRPM);
    int MotorStop() const ;
    int GetRPM() const ;

   private:
    int motorNumber_;
    bool inverted_;
    double currentDutyCycle_;
    double GetDC(int entryRPM);
};

#endif  // MOTOR_H