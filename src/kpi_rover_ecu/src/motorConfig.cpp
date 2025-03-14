#include "motorConfig.h"

MotorConfig::MotorConfig(uint8_t assignedNumber, bool invertedStatus) : number_(assignedNumber), inverted_(invertedStatus) {}

uint8_t MotorConfig::GetNumber() const { return number_; }

bool MotorConfig::IsInverted() const { return inverted_; }