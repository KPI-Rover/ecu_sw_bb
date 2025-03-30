#ifndef MOTOR_H
#define MOTOR_H

#include <robotcontrol.h>

#include <array>
#include <iostream>
#include <vector>

#include "PIDRegulator.h"

#define MIN_RPM 8000
#define MAX_RPM 26500
#define BRAKE_TIME 100000  // 100 ms
#define LOOP_TICKS 821

using namespace std;

class Motor {
   public:
    Motor(int assignedNumber, bool isInverted, array<float, 3> _coeficients);
    int MotorGo(int newRPM);
    int MotorStop();
    int GetEncoderCounter();

   private:
    static constexpr int kBrakeTime = 100000;  // 100 ms

    int motorNumber_;
    int currentRPM_ = 0;
    bool inverted_;
    double currentDutyCycle_;
    PIDRegulator pidRegulator_;
    double GetDC(int entryRPM);
    int MotorSet(int inputRPM);
};

#endif  // MOTOR_H