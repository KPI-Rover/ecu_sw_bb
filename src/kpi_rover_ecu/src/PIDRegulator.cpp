#include "PIDRegulator.h"

#include <array>
#include <cmath>

#include "loggingIncludes.h"

void PIDRegulator::Init(std::array<float, 3> _coeficients) {
    kp_ = _coeficients[0];
    ki_ = _coeficients[1];
    kd_ = _coeficients[2];
}

int PIDRegulator::Run(float actual, float setpoint, float dt) {
    // dt is delta time from previous run, in milliseconds — convert to seconds
    LOG_DEBUG << "PIDRegulator::Run(" << actual << ", " << setpoint << ", " << dt << " ms)";
    float error = setpoint - actual;
    float dtSec = dt / 1000.0F;  // Convert milliseconds to seconds

    // Reset integral when setpoint is 0
    if (setpoint == 0.0F) {
        integral_ = 0.0F;
    }

    // Proportional term
    float pTerm = kp_ * error;

    // Derivative term with filtering
    float dTerm = 0.0F;
    if (!firstRun_ && dtSec > 0.0F) {
        // Calculate raw derivative
        float dRaw = (error - previousError_) / dtSec;

        // Apply low-pass filter to reduce noise
        // dFiltered = alpha * dRaw + (1 - alpha) * dFiltered_prev
        dFiltered_ = dFilterAlpha_ * dRaw + (1.0F - dFilterAlpha_) * dFiltered_;

        // Calculate derivative term
        dTerm = kd_ * dFiltered_;
    } else {
        firstRun_ = false;
    }

    // Update state for next iteration
    previousError_ = error;

    // Integrator handling
    integral_ += error * dtSec;  // accumulate integral (error in same units as setpoint/actual)
    // Anti-windup: clamp integral term
    if (integral_ > integralLimit_) integral_ = integralLimit_;
    if (integral_ < -integralLimit_) integral_ = -integralLimit_;
    float iTerm = ki_ * integral_;

    // Calculate total output (P + I + D)
    float output = pTerm + iTerm + dTerm;

    // Force output to 0 when setpoint is 0
    if (setpoint == 0.0F) {
        output = 0.0F;
    }

    // Clamp output to actuator limits
    if (output > maxOutput_) output = maxOutput_;
    if (output < minOutput_) output = minOutput_;
    if (setpoint > 0 && output < 0.0F) {
        output = 0.0F;
    }
    if (setpoint < 0 && output > 0.0F) {
        output = 0.0F;
    }

    LOG_DEBUG << "PIDRegulator::Run() error:" << error << " pTerm: " << pTerm << " dTerm: " << dTerm;
    LOG_DEBUG << "PIDRegulator::Run() dFiltered_:" << dFiltered_ << " dtSec: " << dtSec;
    LOG_DEBUG << "PIDRegulator::Run() kp_:" << kp_ << " kd_:" << kd_;

    int ret = static_cast<int>(output);
    LOG_DEBUG << "PIDRegulator::Run() returns : " << ret;
    return ret;
}
