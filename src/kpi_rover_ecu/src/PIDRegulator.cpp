#include "PIDRegulator.h"

#include <array>
#include <cmath>

void PIDRegulator::Init(std::array<float, 3> _coeficients) {
    kp_ = _coeficients[0];
    ki_ = _coeficients[1];
    kd_ = _coeficients[2];
}

int PIDRegulator::Run(float actual, float setpoint, float dt) {
    float error = setpoint - actual;
    float pTerm = kp_ * error;

    integral_ += error * dt;
    if (integral_ > integralLimit_) integral_ = integralLimit_;
    if (integral_ < -integralLimit_) integral_ = -integralLimit_;

    float iTerm = ki_ * integral_;
    float dTerm = kd_ * ((dt > 0.0F) ? (error - previousError_) / dt : 0.0F);

    previousError_ = error;

    float output = pTerm + iTerm + dTerm;

    // Optionally clamp output to max/min here if needed
    // if (output > maxOutput_) output = maxOutput_;
    // if (output < minOutput_) output = minOutput_;

    return static_cast<int>(output);
}
