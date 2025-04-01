#include "PIDRegulator.h"

#include <array>
#include <chrono>
#include <cmath>
#include <iostream>
#include <vector>

void PIDRegulator::Init(std::array<float, 3> _coeficients, int _loopsTicks, int _maxRPM) {
    kp_ = _coeficients[0];
    ki_ = _coeficients[1];
    kd_ = _coeficients[2];

    loopsTicks_ = _loopsTicks;
    maxRPM_ = _maxRPM;
    lastTimePoint_ = std::chrono::high_resolution_clock::now();
    // cout << "PID regulator initialized" << '\n';
}

int PIDRegulator::Run(int setpoint, int ticks_value) {
    const auto kCurrentTimePoint = std::chrono::high_resolution_clock::now();
    const std::chrono::duration<double, std::milli> kElapsedMilliseconds = kCurrentTimePoint - lastTimePoint_;
    lastTimePoint_ = kCurrentTimePoint;
    const auto kTimeDt = static_cast<float>(kElapsedMilliseconds.count());

    const float kRevolutions = static_cast<float>(ticks_value) / static_cast<float>(loopsTicks_);

    const float kInputPoint =
        static_cast<float>(std::round((kRevolutions * SECONDSTOMINUTE * MILISECONDSTOSECOND) / kTimeDt)) *
        kSpeedIndexMultipler_;

    const float kError = static_cast<float>(setpoint) - kInputPoint;
    std::cout << "set point " << setpoint << " current point " << kInputPoint << " error " << kError << '\n';

    const float kPTerm = kp_ * kError;

    integral_ += kError * kTimeDt;
    if (integral_ > kintegralLimit_) {
        integral_ = kintegralLimit_;
    }
    if (integral_ < -kintegralLimit_) {
        integral_ = -kintegralLimit_;
    }

    const float kITerm = ki_ * integral_;

    const float kDerivativePart = (kTimeDt > 0) ? (kError - previousError_) / kTimeDt : 0.0F;
    const float kDTerm = kd_ * kDerivativePart;

    previousError_ = kError;

    return static_cast<int>(kPTerm + kITerm + kDTerm);
}