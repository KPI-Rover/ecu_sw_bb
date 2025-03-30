#ifndef MOTOR_CONFIG_H
#define MOTOR_CONFIG_H

// #include "config.h"
#include <array>
#include <cstdint>
#include <iostream>
#include <vector>

using namespace std;

class MotorConfig {
   public:
    MotorConfig(uint8_t assignedNumber, bool invertedStatus, const array<float, 3>& _coefs);

    array<float, 3> GetPidCoefs() const;
    uint8_t GetNumber() const;
    bool IsInverted() const;

   private:
    uint8_t number_;
    bool inverted_;
    array<float, 3> pidCoefs_ = {};
};

#endif  // MOTOR_CONFIG_H