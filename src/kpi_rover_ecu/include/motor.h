#ifndef MOTOR_H
#define MOTOR_H

#include <array>
#include <chrono>

#include "PIDRegulator.h"

class Motor {
   public:
    static constexpr int kMaxRpm = 26500;
    static constexpr int kLoopTicks = 821;
    static constexpr int kSpeedIndexMultipler = 100;
    static constexpr int kSecondsMinute = 60;
    static constexpr int kMiliSecondsSeconds = 1000;

    Motor(int assigned_number, bool is_inverted, std::array<float, 3> _coeficients);
    int MotorGo(int newRPM);
    int MotorStop() const;
    int GetEncoderCounter();

   private:
    float GetSpeedError(int _ticks, float _timeSegment);
    float GetTimeSegment();
    int MotorSet(int inputRPM);

    PIDRegulator pidRegulator_;
    int setpointRpm_;
    int actualRpm_;
    std::chrono::time_point<std::chrono::high_resolution_clock> lastTimePoint_;

    int motorNumber_;
    bool inverted_;
    double currentDutyCycle_;
    double GetDC(int entryRPM);
};

#endif  // MOTOR_H