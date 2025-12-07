#ifndef MOTOR_H
#define MOTOR_H

#include <array>
#include <chrono>
#include <memory>

#include "Odometer.h"
#include "PIDRegulator.h"

class Motor {
   public:
    static constexpr int kMaxRpm = 26500;
    static constexpr int kLoopTicks = 1328;
    static constexpr int kSpeedIndexMultipler = 100;
    static constexpr int kSecondsMinute = 60;
    static constexpr int kMiliSecondsSeconds = 1000;

    Motor(int assigned_number, bool is_inverted, std::array<float, 3> _coeficients, float dFilterAlpha = 0.1f);
    int SetSetpoint(int newRPM);
    int MotorStop() const;
    int GetEncoderCounter();
    void TimerTask();

   private:
    float GetActualRpm(int _ticks, float _timeSegment);
    float GetTimeSegment();
    int SetPWM(int inputRPM);

    PIDRegulator pidRegulator_;
    int setpointRpm_;
    int actualRpm_;
    std::chrono::time_point<std::chrono::high_resolution_clock> lastTimePoint_;

    int motorNumber_;
    bool inverted_;
    double currentDutyCycle_;
    double GetDC(int entryRPM);

    std::unique_ptr<Odometer> pidOdometer_;  // Odometer for PID control (measures velocity)
    std::unique_ptr<Odometer> rosOdometer_;  // Odometer for external counter reading (accumulates)

    int log_counter_;
};

#endif  // MOTOR_H
