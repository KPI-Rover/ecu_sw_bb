#include "motor.h"
#include "PIDRegulator.h"

#include <rc/encoder.h>
#include <rc/motor.h>

#include <iostream>

Motor::Motor(int assigned_number, bool is_inverted, std::array<float, 3> _coeficients)
    : motorNumber_(assigned_number), inverted_(is_inverted), currentDutyCycle_(0), currentRpm_(0) {
    pidRegulator_.Init(_coeficients, kLoopTicks, kMaxRpm);
}

int Motor::MotorGo(int newRPM) {
    if (newRPM > kMaxRpm) {
        std::cout << "[Warning] RPM out of range\n";
        newRPM = kMaxRpm;
    }

    if (newRPM < -kMaxRpm) {
        std::cout << "[Warning] RPM out of range\n";
        newRPM = -kMaxRpm;
    }


    if (MotorSet(newRPM) == -1) {
        return -1;
    }

    currentRpm_ = newRPM;
    return 0;
}

int Motor::MotorSet(int inputRPM) {
    currentDutyCycle_ = GetDC(inputRPM);
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

int Motor::GetEncoderCounter() {
    const int kEncoderTicks = rc_encoder_read(motorNumber_);
    int pid_encoder_ticks = kEncoderTicks;
    if (!inverted_) {
        pid_encoder_ticks *= -1;
    }

    const int kPidOutput = pidRegulator_.Run(currentRpm_, pid_encoder_ticks);
    rc_encoder_write(motorNumber_, 0);

    MotorSet(currentRpm_ + kPidOutput);
    return kEncoderTicks;
}

double Motor::GetDC(int entryRPM) { return static_cast<double>(entryRPM) / kMaxRpm; }