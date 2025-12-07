#ifndef MOTORSCONTROLLER_H
#define MOTORSCONTROLLER_H

#include <atomic>
#include <cstdint>
#include <thread>
#include <vector>

#include "motor.h"
#include "motorConfig.h"

class MotorController {
   public:
    int Init(const std::vector<MotorConfig>& _motors, uint8_t _motorNumber);
    int SetSetpoint(int channel, int newRPM);
    int StopMotor(int channel);

    int GetEncoderCounter(int channel);
    int GetMotorsNumber();
    void Destroy();

    void Start();
    void Stop();

   private:
    void ThreadFunction();

    uint8_t motor_number_;
    std::vector<Motor> motors_;
    std::thread control_thread_;
    std::atomic<bool> running_;
};

#endif
