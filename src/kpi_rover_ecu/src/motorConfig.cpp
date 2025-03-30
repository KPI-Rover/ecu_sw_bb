#include "motorConfig.h"

MotorConfig::MotorConfig(uint8_t assignedNumber, bool invertedStatus, const array<float, 3>& _coefs)
    : number_(assignedNumber), inverted_(invertedStatus) {
    // pidCoefs_.assign(_coefs.begin(), _coefs.end());
    pidCoefs_ = _coefs;
}

array<float, 3> MotorConfig::GetPidCoefs() const { return pidCoefs_; }

uint8_t MotorConfig::GetNumber() const { return number_; }

bool MotorConfig::IsInverted() const { return inverted_; }