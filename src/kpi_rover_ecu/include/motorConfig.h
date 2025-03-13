#ifndef MOTORCONFIG_H
#define MOTORCONFIG_H

#include "config.h"

class MotorConfig {
   public:
    MotorConfig(uint8_t assignedNumber, bool invertedStatus);

    uint8_t getNumber();

    bool isInverted();

   private:
    uint8_t number;
    bool inverted;
};

#endif