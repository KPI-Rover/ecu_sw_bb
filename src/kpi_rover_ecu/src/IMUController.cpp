#include "IMUController.h"

#include <rc/mpu.h>

#include <iostream>
#include <pthread.h>
#include <vector>

IMUController::IMUController() : isStarted_(false), configuration_(rc_mpu_default_config()), actualData_({}) {
	configuration_.i2c_bus = kI2cBus;
    configuration_.gpio_interrupt_pin_chip = kGpioIntPinChip;
    configuration_.gpio_interrupt_pin = kGpioIntPinPin;
    configuration_.dmp_sample_rate = kDmpSampleRate;
    configuration_.enable_magnetometer = kEnableMagnetometer;
}

int IMUController::Init() {
    if(rc_mpu_initialize_dmp(&data_, configuration_)) {
        std::cout << "Initializing MPU failed" << '\n';
        return -1;
    }

	return 0;
}

void IMUController::SetEnable() { isStarted_ = true; }

bool IMUController::GetEnable() { return isStarted_; }

std::vector<float> IMUController::GetData() {
	if (isStarted_ != true) {
		return std::vector<float>();
	}

	rc_mpu_read_accel(&data_);
	rc_mpu_read_gyro(&data_);

    actualData_.resize(10);
    const std::vector<float> kAccelData = GetAccel();
    std::copy(kAccelData.begin(), kAccelData.begin() + 3, actualData_.begin());

    const std::vector<float> kGyroData = GetGyro();
    std::copy(kGyroData.begin(), kGyroData.begin() + 3, actualData_.begin() + 3);

    const std::vector<float> kQaternionData = GetQaternion();
    std::copy(kQaternionData.begin(), kQaternionData.begin() + 4, actualData_.begin() + 6);

    return actualData_;
}

std::vector<float> IMUController::GetAccel() {
	std::vector<float> ret_val;

	ret_val.push_back(data_.accel[0]);
	ret_val.push_back(data_.accel[1]);
	ret_val.push_back(data_.accel[2]);
	
	return ret_val;
}

std::vector<float> IMUController::GetGyro() {
	std::vector<float> ret_val;

	ret_val.push_back(data_.gyro[0]);
	ret_val.push_back(data_.gyro[1]);
	ret_val.push_back(data_.gyro[2]);
	
	return ret_val;
}

std::vector<float> IMUController::GetQaternion() {
	std::vector<float> ret_val;

	ret_val.push_back(data_.dmp_quat[QUAT_W]);
	ret_val.push_back(data_.dmp_quat[QUAT_X]);
	ret_val.push_back(data_.dmp_quat[QUAT_Y]);
	ret_val.push_back(data_.dmp_quat[QUAT_Z]);
	
	return ret_val;
}

void IMUController::Stop() {
	rc_mpu_power_off();
}
