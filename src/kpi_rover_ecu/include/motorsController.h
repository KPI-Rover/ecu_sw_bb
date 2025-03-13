#ifndef MOTORSCONTROLLER_H
#define MOTORSCONTROLLER_H

#include "config.h"
#include "motor.h"
#include "motorConfig.h"

class MotorController {
   public:
    uint8_t motor_number_;

    int init(MotorConfig _motors[], uint8_t _motorNumber);

    int setMotorRPM(int channel, int newRPM);
    int stopMotor(int channel);

    int getMotorRPM(int channel);

    void destroy();
    
   private:
    vector<Motor> motors_;
    
};

#endif