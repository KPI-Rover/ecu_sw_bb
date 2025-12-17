#include "IMUController.h"

#include <rc/mpu.h>
#include <iostream>
#include <cstring>

#include "loggingIncludes.h"

IMUController* IMUController::instance_ = nullptr;

IMUController::IMUController() 
    : configuration_(rc_mpu_default_config()),
      data_{},
      cached_data_{} {
    instance_ = this;
    
    // Enable VLOG(1) for this file specifically
    google::SetVLOGLevel("IMUController", 1);

    configuration_.i2c_bus = kI2cBus;
    configuration_.gpio_interrupt_pin_chip = kGpioIntPinChip;
    configuration_.gpio_interrupt_pin = kGpioIntPinPin;
    configuration_.dmp_sample_rate = kDmpSampleRate;
    configuration_.enable_magnetometer = kEnableMagnetometer;
    configuration_.dmp_fetch_accel_gyro = 1;
}

IMUController::~IMUController() {
    Stop();
}

int IMUController::Init() {
    if (rc_mpu_initialize_dmp(&data_, configuration_) != 0) {
        std::cout << "Initializing MPU failed" << '\n';
        return -1;
    }
    rc_mpu_set_dmp_callback(&IMUController::DMPCallbackWrapper);
    return 0;
}

void IMUController::Stop() {
    rc_mpu_power_off();
}

void IMUController::DMPCallbackWrapper() {
    if (instance_) {
        instance_->OnDMPCallback();
    }
}

void IMUController::OnDMPCallback() {
    std::lock_guard<std::mutex> lock(data_mutex_);
    
    // Copy data from library struct to cached struct (casting double to float)
    for(int i=0; i<3; ++i) {
        cached_data_.accel[i] = static_cast<float>(data_.accel[i]);
        cached_data_.gyro[i] = static_cast<float>(data_.gyro[i]);
        cached_data_.mag[i] = static_cast<float>(data_.mag[i]);
    }
    for(int i=0; i<4; ++i) {
        cached_data_.quat[i] = static_cast<float>(data_.dmp_quat[i]);
    }
}

std::vector<float> IMUController::GetAccel() {
    std::lock_guard<std::mutex> lock(data_mutex_);
    return {cached_data_.accel[0], cached_data_.accel[1], cached_data_.accel[2]};
}

std::vector<float> IMUController::GetGyro() {
    std::lock_guard<std::mutex> lock(data_mutex_);
    return {cached_data_.gyro[0], cached_data_.gyro[1], cached_data_.gyro[2]};
}

std::vector<float> IMUController::GetQaternion() {
    std::lock_guard<std::mutex> lock(data_mutex_);
    return {cached_data_.quat[0], cached_data_.quat[1], cached_data_.quat[2], cached_data_.quat[3]};
}

std::vector<float> IMUController::GetMag() {
    std::lock_guard<std::mutex> lock(data_mutex_);
    return {cached_data_.mag[0], cached_data_.mag[1], cached_data_.mag[2]};
}
