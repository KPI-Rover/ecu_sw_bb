#include "motorsController.h"

#include "motor.h"
#include "motorConfig.h"

int MotorController::Init(MotorConfig _motors[], uint8_t _motorNumber) {
    if (rc_kill_existing_process(2.0) < -2) {
        std::cout << "[ERROR][RC] rc_kill_existing_process" << '\n';
        return -1;
    }
    // start signal handler so we can exit cleanly

    if (rc_enable_signal_handler() == -1) {
        std::cout << "[ERROR][RC] failed to start signal handler" << '\n';
        // return nullptr;
        return -1;
    }

    if (rc_motor_init_freq(RC_MOTOR_DEFAULT_PWM_FREQ) == -1) {
        std::cout << "[ERROR][RC] failed to start with frequency" << '\n';
        return -1;
    }

    if (rc_encoder_init() == -1) {
        std::cout << "[ERROR][RC] failed to start with encoder" << '\n';
        return -1;
    }

    motor_number_ = _motorNumber;

    for (int i = 0; i < motor_number_; ++i) {
        motors_.emplace_back(Motor(_motors[i].GetNumber(), _motors[i].IsInverted()));
    }

    return 0;
}

int MotorController::SetMotorRPM(int channel, int newRPM) {
    if (abs(newRPM) > MIN_RPM) {
        if (newRPM >= MAX_RPM) {
            std::cout << "[INFO][RC] Set RPM to max value" << '\n';
            if (motors_[channel].MotorGo(MAX_RPM) != 0) {
                std::cout << "[ERROR][RC] Error while stet new RPM" << '\n';
                return -1;
            }
        } else {
            if (motors_[channel].MotorGo(newRPM) != 0) {
                std::cout << "[ERROR][RC] Error while stet new RPM" << '\n';
                return -1;
            }
        }

    } else {
        if (abs(newRPM) != 0) {
            std::cout << "[INFO][RC] Set RPM to stop because RPM less than MIN_RPM" << '\n';
        }

        if (motors_[channel].MotorStop() != 0) {
            std::cout << "[ERROR][RC] Error while set new RPM" << '\n';
            return -1;
        }
    }

    return 0;
}

int MotorController::StopMotor(int channel) {
    if (motors_[channel].MotorStop() != 0) {
        std::cout << "[ERROR][RC] Error while stoppping motor" << '\n';
        return -1;
    }
    // std::cout << "[INFO][RC] Set RPM to stop because of command" << '\n';

    return 0;
}

void MotorController::Destroy() {
    // delete[] motors;
    rc_motor_cleanup();
    rc_encoder_cleanup();
}

int MotorController::GetEncoderCounter(int channel) { return motors_[channel].GetEncoderCounter(); }

int MotorController::GetMotorsNumber() { return motor_number_; }

// MotorProcessor proc;
