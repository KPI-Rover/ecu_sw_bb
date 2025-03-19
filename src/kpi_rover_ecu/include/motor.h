#ifndef MOTOR_H
#define MOTOR_H

#include <robotcontrol.h>
#include <iostream>

#define MIN_RPM 8000
#define MAX_RPM 26500
#define BRAKE_TIME 100000  // 100 ms

using namespace std;

class Motor {
   public:
    Motor(int assignedNumber, bool isInverted);
    int MotorGo(int newRPM);
    int MotorStop() const ;
    int GetEncoderCounter() const ;

   private:
    int motorNumber_;
    bool inverted_;
    double currentDutyCycle_;
    double GetDC(int entryRPM);
};

#endif  // MOTOR_H