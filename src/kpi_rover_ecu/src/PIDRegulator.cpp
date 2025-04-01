#include "PIDRegulator.h"

#include <array>
#include <cmath>

void PIDRegulator::Init(std::array<float, 3> _coeficients) {
    kp_ = _coeficients[0];
    ki_ = _coeficients[1];
    kd_ = _coeficients[2];
}

int PIDRegulator::Run(float kError, float kTimeDt) {
    const float kPTerm = kp_ * kError;

    integral_ += kError * kTimeDt;
    if (integral_ > kIntegralLimit) {
        integral_ = kIntegralLimit;
    }
    if (integral_ < -kIntegralLimit) {
        integral_ = -kIntegralLimit;
    }

    const float kITerm = ki_ * integral_;

    const float kDerivativePart = (kTimeDt > 0.0F) ? (kError - previousError_) / kTimeDt : 0.0F;
    const float kDTerm = kd_ * kDerivativePart;

    previousError_ = kError;

    return static_cast<int>(kPTerm + kITerm + kDTerm);
}
