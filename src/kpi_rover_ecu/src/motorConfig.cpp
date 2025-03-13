#include "motorConfig.h"

#include "config.h"

MotorConfig::MotorConfig(uint8_t assignedNumber, bool invertedStatus) {
    number_ = assignedNumber;
    inverted_ = invertedStatus;
}

uint8_t MotorConfig::getNumber() { return number_; }

bool MotorConfig::isInverted() { return inverted_; }