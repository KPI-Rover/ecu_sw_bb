#ifndef MOTORSCONTROLLER_H
#define MOTORSCONTROLLER_H

#include <array>
#include <cstdint>
#include <iostream>
#include <vector>

#include "motor.h"
#include "motorConfig.h"

using namespace std;

class MotorController {
   public:
    int Init(const std::vector<MotorConfig>& _motors, uint8_t _motorNumber);
    int SetMotorRPM(int channel, int newRPM);
    int StopMotor(int channel);

    int GetEncoderCounter(int channel);
    int GetMotorsNumber();
    void Destroy();

   private:
    uint8_t motor_number_;
    std::vector<Motor> motors_;
};

#endif