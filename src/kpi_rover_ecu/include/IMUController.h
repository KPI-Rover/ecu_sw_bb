#ifndef IMUCONTROLLER_H
#define IMUCONTROLLER_H

#include <rc/mpu.h>

#include <atomic>
#include <cstdint>
#include <vector>

class IMUController {
	public:
	 IMUController();
	 int Init();
	 void SetEnable();
	 void SetDisable();
	 bool GetEnable();
	 void Stop();
	 std::vector<float> GetData();
	 uint8_t GetId();
	 
	private:
	 const int kIdGetCommand = 0x06;
	 const int kI2cBus = 2;
	 const int kGpioIntPinChip = 3;
	 const int kGpioIntPinPin = 21;
	 const int kDmpSampleRate = 100;
	 const int kEnableMagnetometer = 1;

	 std::atomic<bool> isStarted_;
	 rc_mpu_config_t configuration_;
	 std::vector<float> actualData_;
	 rc_mpu_data_t data_;
	
	 std::vector<float> GetAccel();
	 std::vector<float> GetGyro();
	 std::vector<float> GetQaternion();
};

#endif
