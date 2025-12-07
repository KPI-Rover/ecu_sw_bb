#include "motorsController.h"

#include <rc/encoder.h>
#include <rc/motor.h>
#include <rc/start_stop.h>

#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <thread>
#include <vector>

#include "EncoderController.h"
#include "loggingIncludes.h"
#include "motorConfig.h"

constexpr double kKillProcessTimeout = 2.0;

int MotorController::Init(const std::vector<MotorConfig>& _motors, uint8_t _motorNumber) {
    LOG_DEBUG << "Initializing rc drivers";
    if (rc_kill_existing_process(kKillProcessTimeout) < -2) {
        LOG_ERROR << "rc_kill_existing_process";
        return -1;
    }
    // start signal handler so we can exit cleanly

    if (rc_enable_signal_handler() == -1) {
        LOG_ERROR << "failed to start signal handler";
        // return nullptr;
        return -1;
    }

    if (rc_motor_init_freq(RC_MOTOR_DEFAULT_PWM_FREQ) == -1) {
        LOG_ERROR << "failed to start with frequency";
        return -1;
    }

    motor_number_ = _motorNumber;
    running_ = false;

    for (int i = 0; i < motor_number_; ++i) {
        uint8_t motor_num = _motors[i].GetNumber();
        bool is_inverted = _motors[i].IsInverted();
        LOG_INFO << "Creating Motor from config index " << i << ": number=" << static_cast<int>(motor_num)
                 << ", inverted=" << is_inverted;
        motors_.emplace_back(motor_num, is_inverted, _motors[i].GetPidCoefs(), _motors[i].GetDerivativeFilterAlpha());
    }

    return 0;
}

int MotorController::SetSetpoint(int channel, int newRPM) {
    if (channel >= motor_number_) {
        LOG_WARNING << "Channel out of range";
        return -1;
    }

    const int kRes = motors_[channel].SetSetpoint(newRPM);
    if (kRes != 0) {
        LOG_ERROR << "Error while set new RPM";
        return -1;
    }

    return 0;
}

int MotorController::StopMotor(int channel) {
    if (motors_[channel].MotorStop() != 0) {
        LOG_ERROR << "Error while stoppping motor";
        return -1;
    }
    // std::cout << "[INFO][RC] Set RPM to stop because of command";

    return 0;
}

void MotorController::Destroy() {
    // delete[] motors;
    LOG_DEBUG << "Clean up used rc drivers";
    rc_motor_cleanup();
    rc_encoder_cleanup();
}

int MotorController::GetEncoderCounter(int channel) { return motors_[channel].GetEncoderCounter(); }

int MotorController::GetMotorsNumber() { return motor_number_; }

void MotorController::ThreadFunction() {
    LOG_INFO << "MotorController thread started";

    while (running_) {
        auto start_time = std::chrono::steady_clock::now();

        // Call TimerTask for each motor
        for (int i = 0; i < motor_number_; ++i) {
            motors_[i].TimerTask();
        }

        // Sleep to maintain 10ms period
        auto end_time = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        auto sleep_time = std::chrono::milliseconds(10) - elapsed;

        if (sleep_time.count() > 0) {
            std::this_thread::sleep_for(sleep_time);
        } else {
            LOG_WARNING << "MotorController loop took longer than 10ms: " << elapsed.count() << "ms";
        }
    }

    LOG_INFO << "MotorController thread stopped";
}

void MotorController::Start() {
    LOG_DEBUG << "Starting MotorController thread";

    if (running_) {
        LOG_WARNING << "MotorController thread already running";
        return;
    }

    running_ = true;
    control_thread_ = std::thread(&MotorController::ThreadFunction, this);
}

void MotorController::Stop() {
    LOG_DEBUG << "Stopping MotorController thread";

    if (!running_) {
        LOG_WARNING << "MotorController thread not running";
        return;
    }

    running_ = false;

    if (control_thread_.joinable()) {
        control_thread_.join();
    }

    LOG_INFO << "MotorController Stopped";
}
