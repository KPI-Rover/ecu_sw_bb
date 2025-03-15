#ifndef MOTORSCONTROLLER_H
#define MOTORSCONTROLLER_H

#include "motor.h"
#include "motorConfig.h"
#include <iostream>
#include <vector>
#include <cstdint>

using namespace std;

class MotorController {
   public:
    int Init(MotorConfig _motors[], uint8_t _motorNumber);
    int SetMotorRPM(int channel, int newRPM);
    int StopMotor(int channel);

    int GetMotorRPM(int channel);
    int GetMotorsNumber();
    void Destroy();
    
   private:
    uint8_t motor_number_;
    vector<Motor> motors_;
    
};

#endif