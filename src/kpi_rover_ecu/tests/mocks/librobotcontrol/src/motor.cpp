#include <rc/motor.h>
#include <gmock/gmock.h>
#include "mock_rc_motor.h"
#include <memory>

static std::unique_ptr<MockRCMotor> g_mock_rc_motor;

MockRCMotor& GetMockRCMotor() {
    if (!g_mock_rc_motor) {
        g_mock_rc_motor = std::make_unique<MockRCMotor>();
    }
    return *g_mock_rc_motor;
}

void ResetMockRCMotor() {
    g_mock_rc_motor = std::make_unique<MockRCMotor>();
}

void DestroyMockRCMotor() {
    g_mock_rc_motor.reset();
}

extern "C" {
    int rc_motor_init(void) {
        return GetMockRCMotor().init();
    }

    int rc_motor_cleanup(void) {
        return GetMockRCMotor().cleanup();
    }

    int rc_motor_set(int motor, double duty) {
        return GetMockRCMotor().set(motor, duty);
    }

    int rc_motor_free_spin(int motor) {
        return GetMockRCMotor().free_spin(motor);
    }

    int rc_motor_brake(int motor) {
        return GetMockRCMotor().brake(motor);
    }

    int rc_motor_set_all(double duty) {
        return GetMockRCMotor().set_all(duty);
    }
    
    int rc_motor_init_freq(int frequency) {
        return GetMockRCMotor().init_freq(frequency);
    }
}

