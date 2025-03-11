#include "motorConfig.h"
#include "config.h"

MotorConfig::MotorConfig(uint8_t assignedNumber, bool invertedStatus) {
	number = assignedNumber;
	inverted = invertedStatus;
}

uint8_t MotorConfig::getNumber() {
	return  number;
}

bool MotorConfig::isInverted() {
	return inverted;
}