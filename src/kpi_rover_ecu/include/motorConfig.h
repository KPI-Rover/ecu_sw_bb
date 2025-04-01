#ifndef MOTOR_CONFIG_H
#define MOTOR_CONFIG_H

#include <array>
#include <cstdint>

class MotorConfig {
   public:
    MotorConfig(uint8_t assignedNumber, bool invertedStatus, const std::array<float, 3>& _coefs);

    std::array<float, 3> GetPidCoefs() const;
    uint8_t GetNumber() const;

    bool IsInverted() const;

   private:
    uint8_t number_;
    bool inverted_;
    std::array<float, 3> pidCoefs_ = {};
};

#endif  // MOTOR_CONFIG_H