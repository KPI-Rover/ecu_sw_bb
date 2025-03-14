#include "motor.h"
#include <robotcontrol.h>
#include <iostream>

//using namespace std;

Motor::Motor(int assignedNumber, bool isInverted) : motorNumber_(assignedNumber), inverted_(isInverted), currentDutyCycle_(0) {}


int Motor::MotorGo(int newRPM) {
    currentDutyCycle_ = GetDC(newRPM);
    if (inverted_) {
        currentDutyCycle_ = -currentDutyCycle_;
    }

    if (rc_motor_set(motorNumber_, currentDutyCycle_) != 0) {
        std::cout << "[ERROR][RC] rc_motor_set" << '\n';
        return -1;
    }

    return 0;
}

int Motor::MotorStop() const  {
    if (rc_motor_brake(motorNumber_) != 0) {
        std::cout << "[ERROR][RC] rc_motor_brake" << '\n';
        return -1;
    }

    rc_usleep(BRAKE_TIME);

    if (rc_motor_free_spin(motorNumber_) != 0) {
        std::cout << "[ERROR][RC] rc_motor_free_spin" << '\n';
        return -1;
    }

    return 0;
}

int Motor::GetRPM() const {
    const int kEncoderTicks = rc_encoder_read(motorNumber_);
    rc_encoder_write(motorNumber_, 0);
    return kEncoderTicks;
}

double Motor::GetDC(int entryRPM) { return static_cast<double>(entryRPM) / MAX_RPM; }