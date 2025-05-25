#include "motorsController.h"

#include <rc/encoder.h>
#include <rc/motor.h>
#include <rc/start_stop.h>

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <vector>

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

    if (rc_encoder_init() == -1) {
        LOG_ERROR << "failed to start with encoder";
        return -1;
    }

    LOG_DEBUG << "Create array of Motor instances";
    motor_number_ = _motorNumber;

    for (int i = 0; i < motor_number_; ++i) {
        motors_.emplace_back(_motors[i].GetNumber(), _motors[i].IsInverted(), _motors[i].GetPidCoefs());
    }

    return 0;
}

int MotorController::SetMotorRPM(int channel, int newRPM) {
    if (channel >= motor_number_) {
        LOG_WARNING << "Channel out of range";
        return -1;
    }

    const int kRes = motors_[channel].MotorGo(newRPM);
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
