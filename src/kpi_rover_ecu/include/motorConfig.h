#ifndef MOTOR_CONFIG_H
#define MOTOR_CONFIG_H

#include <cstdint>

class MotorConfig {
   public:
    MotorConfig(uint8_t assignedNumber, bool invertedStatus);

    uint8_t GetNumber() const;

    bool IsInverted() const;

   private:
    uint8_t number_;
    bool inverted_;
};

#endif  // MOTOR_CONFIG_H