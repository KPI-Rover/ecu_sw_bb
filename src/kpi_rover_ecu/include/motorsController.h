#ifndef MOTORSCONTROLLER_H
#define MOTORSCONTROLLER_H

#include "config.h"
#include "motor.h"
#include "motorConfig.h"

class MotorController {
   public:
    uint8_t motor_number_;

    int Init(MotorConfig _motors[], uint8_t _motorNumber);

    int SetMotorRPM(int channel, int newRPM);
    int StopMotor(int channel);

    int GetMotorRPM(int channel);

    void Destroy();
    
   private:
    vector<Motor> motors_;
    
};

#endif