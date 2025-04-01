#ifndef PIDREGULATOR_H
#define PIDREGULATOR_H

#pragma once

#include <array>

class PIDRegulator {
   public:
    void Init(std::array<float, 3> _coeficients);
    int Run(float kError, float kTimeDt);

   private:
    float kp_ = 0;
    float ki_ = 0;
    float kd_ = 0;
    float previousError_ = 0;
    float integral_ = 0;
    const float kIntegralLimit = 26500;
};

#endif
