#include "motor.h"

using namespace std;

Motor::Motor(int assignedNumber, bool isInverted) {
    motorNumber = assignedNumber;
    inverted = isInverted;
}

int Motor::motorGo(int newRPM) {
    currentDutyCycle = getDC(newRPM);
    if (inverted) {
        currentDutyCycle = -currentDutyCycle;
    }

    if (rc_motor_set(motorNumber, currentDutyCycle) != 0) {
        cout << "[ERROR][RC] rc_motor_set" << endl;
        return -1;
    }

    return 0;
}

int Motor::motorStop() {
    if (rc_motor_brake(motorNumber) != 0) {
        cout << "[ERROR][RC] rc_motor_brake" << endl;
        return -1;
    }

    rc_usleep(BRAKE_TIME);

    if (rc_motor_free_spin(motorNumber) != 0) {
        cout << "[ERROR][RC] rc_motor_free_spin" << endl;
        return -1;
    }

    return 0;
}

int Motor::getRPM() {
    int encoderTicks = rc_encoder_read(motorNumber);
    rc_encoder_write(motorNumber, 0);
    return encoderTicks;
}

double Motor::getDC(int entryRPM) {
    return static_cast<double>(entryRPM) / MAX_RPM;
}