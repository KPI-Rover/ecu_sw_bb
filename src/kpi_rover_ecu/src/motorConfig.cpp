#include "motorConfig.h"

#include <cstdint>

MotorConfig::MotorConfig(uint8_t assignedNumber, bool invertedStatus, const std::array<float, 3>& _coefs)
    : number_(assignedNumber), inverted_(invertedStatus), pidCoefs_(_coefs) {}

std::array<float, 3> MotorConfig::GetPidCoefs() const { return pidCoefs_; }

uint8_t MotorConfig::GetNumber() const { return number_; }

bool MotorConfig::IsInverted() const { return inverted_; }