#ifndef MOTORCONFIG_H
#define MOTORCONFIG_H

//#include "config.h"
#include <iostream>
#include <cstdint>

using namespace std;

class MotorConfig {
   public:
    MotorConfig(uint8_t assignedNumber, bool invertedStatus);

    uint8_t GetNumber() const ;

    bool IsInverted() const ;

   private:
    uint8_t number_;
    bool inverted_;
};

#endif