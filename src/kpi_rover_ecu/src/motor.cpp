#include "motor.h"

#include <rc/encoder.h>
#include <rc/motor.h>

#include <array>
#include <chrono>
#include <cmath>
#include <iostream>
#include <ratio>

#include "EncoderController.h"
#include "PIDRegulator.h"
#include "loggingIncludes.h"

Motor::Motor(int assigned_number, bool is_inverted, std::array<float, 3> _coeficients, float dFilterAlpha)
    : motorNumber_(assigned_number),
      inverted_(is_inverted),
      currentDutyCycle_(0),
      setpointRpm_(0),
      actualRpm_(0),
      log_counter_(0) {
    LOG_INFO << "Motor " << assigned_number << " created with inverted=" << is_inverted;
    pidRegulator_.Init(_coeficients);
    pidRegulator_.SetDerivativeFilterAlpha(dFilterAlpha);
    // Ensure PID output is limited to the same units the Motor expects
    // Motor::kMaxRpm is the upper limit for setpoint / actual (scaled by kSpeedIndexMultipler)
    pidRegulator_.SetOutputLimits(-static_cast<float>(kMaxRpm), static_cast<float>(kMaxRpm));
    lastTimePoint_ = std::chrono::high_resolution_clock::now();

    pidOdometer_ = EncoderController::CreateOdometer(assigned_number, kLoopTicks, is_inverted);
    rosOdometer_ = EncoderController::CreateOdometer(assigned_number, kLoopTicks, is_inverted);

    if (!pidOdometer_ || !rosOdometer_) {
        LOG_ERROR << "Failed to create odometers for motor " << assigned_number;
    }
}

int Motor::SetSetpoint(int newRPM) {
    LOG_DEBUG << "Motor::SetSetpoint(" << newRPM << ")";

    if (newRPM > kMaxRpm) {
        LOG_WARNING << " RPM out of range for motor" << motorNumber_;
        newRPM = kMaxRpm;
    }

    if (newRPM < -kMaxRpm) {
        LOG_WARNING << "RPM out of range for motor" << motorNumber_;
        newRPM = -kMaxRpm;
    }

    setpointRpm_ = newRPM;

    LOG_DEBUG << "Motor::SetSetpoint() returns : 0";

    return 0;
}

int Motor::SetPWM(int inputRPM) {
    LOG_DEBUG << "Motor::SetPWM(" << inputRPM << ")";

    // Convert from PID regulator units (scaled RPM) to duty cycle [-1.0, 1.0]
    double duty = GetDC(inputRPM);

    // Clamp duty to [-1.0, 1.0] to ensure rc_motor_set gets valid values
    if (duty > 1.0) {
        LOG_WARNING << "Motor::SetPWM() duty clipped: " << duty << " -> 1.0 for motor " << motorNumber_;
        duty = 1.0;
    }
    if (duty < -1.0) {
        LOG_WARNING << "Motor::SetPWM() duty clipped: " << duty << " -> -1.0 for motor " << motorNumber_;
        duty = -1.0;
    }

    if (inverted_) {
        duty = -duty;
    }

    currentDutyCycle_ = duty;

    LOG_DEBUG << "new duty cycle for motor" << motorNumber_ << " = " << currentDutyCycle_;

    if (rc_motor_set(motorNumber_, currentDutyCycle_) != 0) {
        LOG_DEBUG << "Motor::SetPWM() returns : -1";
        return -1;
    }

    LOG_DEBUG << "Motor::SetPWM() returns : 0";
    return 0;
}

int Motor::MotorStop() const {
    LOG_DEBUG << "Motor::MotorStop()";

    if (rc_motor_brake(motorNumber_) != 0) {
        LOG_ERROR << "rc_motor_brake for motor " << motorNumber_;
        LOG_DEBUG << "Motor::MotorStop() returns : -1";
        return -1;
    }

    LOG_DEBUG << "Motor::MotorStop() returns : 0";
    return 0;
}

float Motor::GetTimeSegment() {
    LOG_DEBUG << "Motor::GetTimeSegment()";
    const auto kCurrentTimePoint = std::chrono::high_resolution_clock::now();
    const std::chrono::duration<double, std::milli> kElapsedMilliseconds = kCurrentTimePoint - lastTimePoint_;
    lastTimePoint_ = kCurrentTimePoint;
    const float result = static_cast<float>(kElapsedMilliseconds.count());
    LOG_DEBUG << "Motor::GetTimeSegment() returns : " << result;
    return result;
}

float Motor::GetActualRpm(int _ticks, float _timeSegment) {
    LOG_DEBUG << "Motor::GetActualRpm(" << _ticks << ", " << _timeSegment << ")";
    LOG_DEBUG << "Get actual RPM " << _timeSegment << " for motor " << motorNumber_;
    const float kRevolutions = static_cast<float>(_ticks) / static_cast<float>(kLoopTicks);
    const float kInputPoint =
        (std::round((kRevolutions * kSecondsMinute * kMiliSecondsSeconds) / _timeSegment)) * kSpeedIndexMultipler;
    LOG_DEBUG << "Motor::GetActualRpm() returns : " << kInputPoint;
    return kInputPoint;
}

int Motor::GetEncoderCounter() {
    LOG_DEBUG << "Motor::GetEncoderCounter()";

    if (!rosOdometer_) {
        LOG_ERROR << "rosOdometer_ is null";
        return 0;
    }

    int encoderTicks = rosOdometer_->GetDiff();
    LOG_DEBUG << "Motor::GetEncoderCounter() returns : " << encoderTicks;
    return encoderTicks;
}

double Motor::GetDC(int entryRPM) {
    LOG_DEBUG << "Motor::GetDC(" << entryRPM << ")";
    const double res = static_cast<double>(entryRPM) / kMaxRpm;
    LOG_DEBUG << "Motor::GetDC() returns : " << res;
    return res;
}

void Motor::TimerTask() {
    if (!pidOdometer_) {
        LOG_ERROR << "pidOdometer_ is null";
        return;
    }

    const float kTimeDt = GetTimeSegment();

    actualRpm_ = pidOdometer_->GetRPM();

    const int kError = setpointRpm_ - actualRpm_;

    const int kPidOutput = pidRegulator_.Run(actualRpm_, setpointRpm_, kTimeDt);

    const int kPWM = SetPWM(kPidOutput);

    // Log only every 100th iteration to reduce I/O overhead
    // if (++log_counter_ >= 100) {
    // LOG_INFO << "PID DUMP Motor: " << motorNumber_ << " Set point: " << setpointRpm_ << " Current value: " << actualRpm_
    //          << " Error: " << kError << " PID output: " << kPidOutput << " PWM: " << currentDutyCycle_;
    // log_counter_ = 0;
    //}
}
