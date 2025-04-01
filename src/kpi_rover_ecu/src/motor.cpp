#include "motor.h"

#include <rc/encoder.h>
#include <rc/motor.h>

#include <array>
#include <chrono>
#include <cmath>
#include <iostream>

#include "PIDRegulator.h"

Motor::Motor(int assigned_number, bool is_inverted, std::array<float, 3> _coeficients)
    : motorNumber_(assigned_number), inverted_(is_inverted), currentDutyCycle_(0), setpointRpm_(0), actualRpm_(0) {
    pidRegulator_.Init(_coeficients);
    lastTimePoint_ = std::chrono::high_resolution_clock::now();
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

    setpointRpm_ = newRPM;
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

    const auto kCurrentTimePoint = std::chrono::high_resolution_clock::now();
    const std::chrono::duration<double, std::milli> kElapsedMilliseconds = kCurrentTimePoint - lastTimePoint_;
    lastTimePoint_ = kCurrentTimePoint;
    const auto kTimeDt = static_cast<float>(kElapsedMilliseconds.count());

    const float kRevolutions = static_cast<float>(pid_encoder_ticks) / static_cast<float>(kLoopTicks);
    const float kInputPoint =
        static_cast<float>(std::round((kRevolutions * SECONDSTOMINUTE * MILISECONDSTOSECOND) / kTimeDt)) *
        kSpeedIndexMultipler;
    actualRpm_ = static_cast<int>(kInputPoint);
    const float kError = static_cast<float>(setpointRpm_) - kInputPoint;

    std::cout << "set point " << setpointRpm_ << " current point " << kInputPoint << " error " << kError << '\n';

    const int kPidOutput = pidRegulator_.Run(kError, kTimeDt);
    rc_encoder_write(motorNumber_, 0);

    MotorSet(setpointRpm_ + kPidOutput);
    return kEncoderTicks;
}

double Motor::GetDC(int entryRPM) { return static_cast<double>(entryRPM) / kMaxRpm; }