#include "PIDRegulator.h"

// PIDRegulator::PIDRegulator(vector<float> _coeficients, const int iLimit, const int oLimit) : kp_(_coeficients[0]),
// 														ki_(_coeficients[1]),
// kd_(_coeficients[2]), 														lastTimePoint_(chrono::high_resolution_clock::now()), 														integralLimit_(iLimit),
// outputLimit_(oLimit) {}

void PIDRegulator::Init(array<float, 3> _coeficients, int _loopsTicks, int _maxRPM) {
    kp_ = _coeficients[0];
    ki_ = _coeficients[1];
    kd_ = _coeficients[2];

    loopsTicks_ = _loopsTicks;
    maxRPM_ = _maxRPM;
    lastTimePoint_ = chrono::high_resolution_clock::now();
    // cout << "PID regulator initialized" << '\n';
}

int PIDRegulator::Run(int setpoint, int ticks_value) {
    const auto kCurrentTimePoint = std::chrono::high_resolution_clock::now();
    const std::chrono::duration<double, std::milli> kElapsedMilliseconds =
        kCurrentTimePoint - lastTimePoint_;
    lastTimePoint_ = kCurrentTimePoint;
    const auto kTimeDt = static_cast<float>(kElapsedMilliseconds.count());

    const float kRevolutions = static_cast<float>(ticks_value) / static_cast<float>(loopsTicks_);

    const float kInputPoint = static_cast<float>(
        std::round((kRevolutions * SECONDSTOMINUTE * MILISECONDSTOSECOND) / kTimeDt)) *
                            SPEEDINDEXMULTIPLIER;
    const float kError = static_cast<float>(setpoint) - kInputPoint; // get rid of abs() !!!
    cout << "set point " << setpoint << " current point " << kInputPoint << " error " << kError << '\n';

    const float kPTerm = kp_ * kError;

    integral_ += kError * kTimeDt;
    if (integral_ > integralLimit_) {
        integral_ = integralLimit_;
    }
    if (integral_ < -integralLimit_) {
        integral_ = -integralLimit_;
    }

    const float kITerm = ki_ * integral_;

    const float kDerivativePart = (kTimeDt > 0) ? (kError - previousError_) / kTimeDt : 0.0F;
    const float kDTerm = kd_ * kDerivativePart;

    previousError_ = kError;

    return static_cast<int>(kPTerm + kITerm + kDTerm);
}