#ifndef MOTOR_H
#define MOTOR_H

#include <array>
#include <chrono>

#include "PIDRegulator.h"

#define SECONDSTOMINUTE 60
#define MILISECONDSTOSECOND 1000

class Motor {
   public:
    static constexpr int kMinRpm = 8000;
    static constexpr int kMaxRpm = 26500;
    static constexpr int kLoopTicks = 821;
    static constexpr int kSpeedIndexMultipler = 100;

    Motor(int assigned_number, bool is_inverted, std::array<float, 3> _coeficients);
    int MotorGo(int newRPM);
    int MotorStop() const;
    int GetEncoderCounter();

   private:
    int MotorSet(int inputRPM);
    static constexpr int kBrakeTime = 100000;  // 100 ms

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