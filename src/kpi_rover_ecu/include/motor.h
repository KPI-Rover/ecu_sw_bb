#ifndef MOTOR_H
#define MOTOR_H

#include <robotcontrol.h>

#include <array>
#include <iostream>
#include <vector>

#include "PIDRegulator.h"


using namespace std;

class Motor {
   public:
    static constexpr int kMinRpm = 8000;
    static constexpr int kMaxRpm = 26500;
    static constexpr int kLoopTicks = 821;


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