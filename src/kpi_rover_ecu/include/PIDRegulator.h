#ifndef PIDREGULATOR_H
#define PIDREGULATOR_H

#pragma once

#include <array>

class PIDRegulator {
   public:
    void Init(std::array<float, 3> _coeficients);
    int Run(float actual, float setpoint, float dt);
    void SetOutputLimits(float minOut, float maxOut) {
        minOutput_ = minOut;
        maxOutput_ = maxOut;
    }
    void SetDerivativeFilterAlpha(float alpha) { dFilterAlpha_ = alpha; }

   private:
    float kp_ = 0;
    float ki_ = 0;
    float kd_ = 0;
    float previousError_ = 0;
    float integral_ = 0;
    float integralLimit_ = 26500;
    // Output limits (same units as setpoint/actual, e.g. RPM)
    float maxOutput_ = 120000.0F;
    float minOutput_ = -120000.0F;
    // Derivative filter state
    float dFiltered_ = 0.0F;
    float dFilterAlpha_ = 0.1F;  // 0..1, higher = less filtering
    bool firstRun_ = true;
};

#endif
