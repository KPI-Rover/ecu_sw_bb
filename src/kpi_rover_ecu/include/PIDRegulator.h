#ifndef PIDREGULATOR_H
#define PIDREGULATOR_H

#pragma once

#include <chrono>
#include <array>

#define SECONDSTOMINUTE 60
#define MILISECONDSTOSECOND 1000
#define SPEEDINDEXMULTIPLIER 100

class PIDRegulator {
   public:
    // PIDRegulator(vector<float> _coeficients);
    void Init(std::array<float, 3> _coeficients, int _loopsTicks, int _maxRPM);
    int Run(int setpoint, int ticksValue);

   private:
    float kp_;
    float ki_;
    float kd_;
    int previousError_ = 0;
    int integral_ = 0;
    const int integralLimit_ = 26500;
    const int outputLimit_ = 26500;
    int loopsTicks_;
    int maxRPM_;

    std::chrono::time_point<std::chrono::high_resolution_clock> lastTimePoint_;
};

#endif