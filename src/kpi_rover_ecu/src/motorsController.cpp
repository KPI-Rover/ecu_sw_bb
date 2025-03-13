#include "motorsController.h"

#include "config.h"
#include "motor.h"
#include "motorConfig.h"

int MotorController::init(MotorConfig _motors[], uint8_t _motorNumber) {
    if (rc_kill_existing_process(2.0) < -2) {
        perror("[ERROR][RC] rc_kill_existing process: ");
        return -1;
    }
    // start signal handler so we can exit cleanly

    if (rc_enable_signal_handler() == -1) {
        fprintf(stderr, "[ERROR][RC] failed to start signal handler\n");
        // return nullptr;
        return -1;
    }

    if (rc_motor_init_freq(RC_MOTOR_DEFAULT_PWM_FREQ) == -1) {
        perror("[ERROR][RC] failed to start with frequency");
        return -1;
    }

    if (rc_encoder_init()) {
        perror("[ERROR][RC] failed to start with frequency");
        return -1;
    }

    motor_number_ = _motorNumber;

    for (int i = 0; i < motor_number_; ++i) {
        motors_.push_back(Motor(_motors[i].getNumber(), _motors[i].isInverted()));
    }

    return 0;
}

int MotorController::setMotorRPM(int channel, int newRPM) {
    if (abs(newRPM) > MIN_RPM) {
        if (newRPM >= MAX_RPM) {
            std::cout << "[INFO][RC] Set RPM to max value" << '\n';
            if (motors_[channel].motorGo(MAX_RPM) != 0) {
                std::cout << "[ERROR][RC] Error while stet new RPM" << '\n';
                return -1;
            }
        } else {
            if (motors_[channel].motorGo(newRPM) != 0) {
                std::cout << "[ERROR][RC] Error while stet new RPM" << '\n';
                return -1;
            }
        }

    } else {
        if (abs(newRPM) != 0) {
            std::cout << "[INFO][RC] Set RPM to stop because RPM less than MIN_RPM" << '\n';
        }

        if (motors_[channel].motorStop() != 0) {
            std::cout << "[ERROR][RC] Error while set new RPM" << '\n';
            return -1;
        }
    }

    return 0;
}

int MotorController::stopMotor(int channel) {
    if (motors_[channel].motorStop() != 0) {
        std::cout << "[ERROR][RC] Error while stoppping motor" << '\n';
        return -1;
    }
    // std::cout << "[INFO][RC] Set RPM to stop because of command" << '\n';

    return 0;
}

void MotorController::destroy() {
    // delete[] motors;
    rc_motor_cleanup();
    rc_encoder_cleanup();
}

int MotorController::getMotorRPM(int channel) { return motors_[channel].getRPM(); }

// MotorProcessor proc;
