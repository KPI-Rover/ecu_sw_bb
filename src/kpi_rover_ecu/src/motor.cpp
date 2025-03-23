#include "motor.h"

#include <rc/encoder.h>
#include <rc/motor.h>

#include <iostream>

Motor::Motor(int assigned_number, bool is_inverted)
    : motorNumber_(assigned_number), inverted_(is_inverted), currentDutyCycle_(0) {}

int Motor::MotorGo(int newRPM) {
    if (newRPM > kMaxRpm) {
        std::cout << "[Warning] RPM out of range\n";
        newRPM = kMaxRpm;
    }

    if (newRPM < kMinRpm) {
        std::cout << "[Warning] RPM out of range\n";
        newRPM = kMinRpm;
    }

    currentDutyCycle_ = GetDC(newRPM);
    if (inverted_) {
        currentDutyCycle_ = -currentDutyCycle_;
    }

    if (rc_motor_set(motorNumber_, currentDutyCycle_) != 0) {
        std::cout << "[ERROR][RC] rc_motor_set\n";
        return -1;
    }

    return 0;
}

int Motor::MotorStop() const {
    if (rc_motor_brake(motorNumber_) != 0) {
        std::cout << "[ERROR][RC] rc_motor_brake\n";
        return -1;
    }

    return 0;
}

int Motor::GetEncoderCounter() const {
    const int kEncoderTicks = rc_encoder_read(motorNumber_);
    rc_encoder_write(motorNumber_, 0);
    return kEncoderTicks;
}

double Motor::GetDC(int entryRPM) { return static_cast<double>(entryRPM) / kMaxRpm; }