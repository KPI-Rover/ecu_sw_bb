#include "motor.h"

#include <rc/encoder.h>
#include <rc/motor.h>

#include <array>
#include <chrono>
#include <cmath>
#include <iostream>
#include <ratio>

#include "loggingIncludes.h"
#include "PIDRegulator.h"

Motor::Motor(int assigned_number, bool is_inverted, std::array<float, 3> _coeficients)
    : motorNumber_(assigned_number), inverted_(is_inverted), currentDutyCycle_(0), setpointRpm_(0), actualRpm_(0) {
    pidRegulator_.Init(_coeficients);
    lastTimePoint_ = std::chrono::high_resolution_clock::now();
}

int Motor::MotorGo(int newRPM) {
    if (newRPM > kMaxRpm) {
        LOG_WARNING << " RPM out of range for motor" << motorNumber_;
        newRPM = kMaxRpm;
    }

    if (newRPM < -kMaxRpm) {
        LOG_WARNING << "RPM out of range for motor" << motorNumber_;
        newRPM = -kMaxRpm;
    }
    LOG_DEBUG << "Set new RPM for motor " << motorNumber_;
    if (MotorSet(newRPM) == -1) {
        LOG_ERROR << "MotorSet failed";
        return -1;
    }

    setpointRpm_ = newRPM;
    return 0;
}

int Motor::MotorSet(int inputRPM) {
    LOG_DEBUG << "Get duty cycle ";
    currentDutyCycle_ = GetDC(inputRPM);
    if (inverted_) {
        currentDutyCycle_ = -currentDutyCycle_;
    }
    LOG_DEBUG << "new duty cycle for motor" << motorNumber_;
    LOG_DEBUG << "rc_motor_set new dutycycle for motor" << motorNumber_;
    if (rc_motor_set(motorNumber_, currentDutyCycle_) != 0) {
        LOG_ERROR << "rc_motor_set";
        return -1;
    }

    return 0;
}

int Motor::MotorStop() const {
    if (rc_motor_brake(motorNumber_) != 0) {
        LOG_ERROR << "rc_motor_brake for motor" << motorNumber_;
        return -1;
    }

    return 0;
}

float Motor::GetTimeSegment() {
    LOG_DEBUG << "Get time segment for motor" << motorNumber_;
    const auto kCurrentTimePoint = std::chrono::high_resolution_clock::now();
    const std::chrono::duration<double, std::milli> kElapsedMilliseconds = kCurrentTimePoint - lastTimePoint_;
    lastTimePoint_ = kCurrentTimePoint;
    return static_cast<float>(kElapsedMilliseconds.count());
}

float Motor::GetActualRpm(int _ticks, float _timeSegment) {
    LOG_DEBUG << "Get actual RPM for " << _timeSegment << " for motor" << motorNumber_;
    const float kRevolutions = static_cast<float>(_ticks) / static_cast<float>(kLoopTicks);
    const float kInputPoint =
        (std::round((kRevolutions * kSecondsMinute * kMiliSecondsSeconds) / _timeSegment)) * kSpeedIndexMultipler;
    return kInputPoint;
}

int Motor::GetEncoderCounter() {
    LOG_DEBUG << "get encoder for motor" << motorNumber_;
    const int kEncoderTicks = rc_encoder_read(motorNumber_);
    int pid_encoder_ticks = kEncoderTicks;
    if (!inverted_) {
        LOG_DEBUG << "Change encoder value sign for motor" << motorNumber_;
        pid_encoder_ticks *= -1;
    }

    const float kTimeDt = GetTimeSegment();
    actualRpm_ = static_cast<int>(GetActualRpm(pid_encoder_ticks, kTimeDt));
    const int kError = setpointRpm_ - actualRpm_;
    LOG_INFO << "set point " << setpointRpm_ << " current point " << actualRpm_ << " error " << kError << " for motor" << motorNumber_;

    LOG_DEBUG << "Run PID regulator";
    const int kPidOutput = pidRegulator_.Run(static_cast<float>(kError), kTimeDt);
    rc_encoder_write(motorNumber_, 0);
    LOG_DEBUG << "set encoder value to " << 0;
    LOG_DEBUG << "Set additional result PID value " << kPidOutput << " for motor " << motorNumber_;
    MotorSet(setpointRpm_ + kPidOutput);
    return pid_encoder_ticks;
}

double Motor::GetDC(int entryRPM) { return static_cast<double>(entryRPM) / kMaxRpm; }
