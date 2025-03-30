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

int PIDRegulator::Run(int setpoint, int ticksValue) {
    auto current_time_point = chrono::high_resolution_clock::now();
    chrono::duration<double, std::milli> elapsed_milliseconds = current_time_point - lastTimePoint_;
    lastTimePoint_ = current_time_point;
    int time_dt = static_cast<uint32_t>(elapsed_milliseconds.count());

    float revolutions = static_cast<float>(ticksValue) / static_cast<float>(loopsTicks_);

    int input_point =
        static_cast<int>(round((revolutions * SECONDSTOMINUTE * MILISECONDSTOSECOND) / time_dt)) * SPEEDINDEXMULTIPLIER;
    int error = setpoint - input_point;  // get rid of abs() !!!
    // cout << "set point " << setpoint << " current point " << input_point << " error " << error << '\n';

    float P_term = kp_ * static_cast<float>(error);

    integral_ += error * time_dt;
    if (integral_ > integralLimit_) integral_ = integralLimit_;
    if (integral_ < -integralLimit_) integral_ = -integralLimit_;

    float I_term = ki_ * integral_;

    int derivative_part = (time_dt > 0) ? (error - previousError_) / time_dt : 0;
    float D_term = kd_ * derivative_part;

    previousError_ = error;

    return static_cast<int>(P_term + I_term + D_term);
}