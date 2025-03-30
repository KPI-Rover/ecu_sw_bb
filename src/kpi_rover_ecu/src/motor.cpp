#include "motor.h"

Motor::Motor(int assignedNumber, bool isInverted, array<float, 3> _coeficients)
    : motorNumber_(assignedNumber), inverted_(isInverted), currentDutyCycle_(0), currentRPM_(0) {
    pidRegulator_.Init(_coeficients, LOOP_TICKS, MAX_RPM);
}

int Motor::MotorGo(int newRPM) {
    if (MotorSet(newRPM) == -1) {
        return -1;
    }
    currentRPM_ = newRPM;
    cout << "CURRENT RPM" << currentRPM_ << '\n';
    return 0;
}

int Motor::MotorSet(int inputRPM) {
    currentDutyCycle_ = GetDC(inputRPM);
    if (inverted_) {
        currentDutyCycle_ = -currentDutyCycle_;
    }

    if (rc_motor_set(motorNumber_, currentDutyCycle_) != 0) {
        std::cout << "[ERROR][RC] rc_motor_set\n";
        return -1;
    }

    return 0;
}

int Motor::MotorStop() {
    if (rc_motor_brake(motorNumber_) != 0) {
        std::cout << "[ERROR][RC] rc_motor_brake" << '\n';
        return -1;
    }

    rc_usleep(BRAKE_TIME);

    if (rc_motor_free_spin(motorNumber_) != 0) {
        std::cout << "[ERROR][RC] rc_motor_free_spin" << '\n';
        return -1;
    }
    currentRPM_ = 0;
    return 0;
}

int Motor::GetEncoderCounter() {
    int encoder_ticks = rc_encoder_read(motorNumber_);
    if (!inverted_) {
        encoder_ticks *= -1;
    }

    const int kPidOutput = pidRegulator_.Run(currentRPM_, encoder_ticks);
    rc_encoder_write(motorNumber_, 0);

    MotorSet(currentRPM_ + kPidOutput);
    return encoder_ticks;
}

double Motor::GetDC(int entryRPM) { return static_cast<double>(entryRPM) / kMaxRpm; }