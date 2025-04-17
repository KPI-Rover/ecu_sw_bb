#include <gmock/gmock.h>
#include <rc/mpu.h>

#include <memory>

#include "mock_rc_imu.h"

static std::unique_ptr<MockRCMPU> g_mock_rc_mpu;

MockRCMPU& GetMockRCMPU() {
    if (!g_mock_rc_mpu) {
        g_mock_rc_mpu = std::make_unique<MockRCMPU>();
    }
    return *g_mock_rc_mpu;
}

void ResetMockRCMPU() { g_mock_rc_mpu = std::make_unique<MockRCMPU>(); }

void DestroyMockRCMPU() { g_mock_rc_mpu.reset(); }

extern "C" {
int rc_mpu_initialize_dmp(rc_mpu_data_t* data, rc_mpu_config_t config) {
    return GetMockRCMPU().initialize_dmp(data, config);
}

int rc_mpu_power_off(void) {
    GetMockRCMPU().power_off();
    return 0;  // Возвращаем успешный код
}

int rc_mpu_read_accel(rc_mpu_data_t* data) { return GetMockRCMPU().read_accel(data); }

int rc_mpu_read_gyro(rc_mpu_data_t* data) { return GetMockRCMPU().read_gyro(data); }

// Реализация rc_mpu_default_config
rc_mpu_config_t rc_mpu_default_config(void) {
    rc_mpu_config_t config = {};
    config.i2c_bus = 2;  // Настройка I2C шины
    config.gpio_interrupt_pin_chip = 3;
    config.gpio_interrupt_pin = 21;
    config.dmp_sample_rate = 100;
    config.enable_magnetometer = 1;
    return config;
}
}
