#ifndef IMUCONTROLLER_H
#define IMUCONTROLLER_H

#include <rc/mpu.h>

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <thread>
#include <vector>

#include "UDPClient.h"

constexpr size_t kActualDataSize = 10;

class IMUController {
   public:
    IMUController();
    int Init();
    void Start(UDPClient* udp_client);
    void ConnectUDP(std::string ip, int port);
    void SetEnable();
    void SetDisable();
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
    const int kTimerPrecision = 200000;  // 20ms
    const uint16_t k16MaxCount = 65535;

    std::atomic<bool> isStarted_;
    std::atomic<bool> isSending_;
    std::thread processingThread_;
    UDPClient* udp_client_;

    rc_mpu_config_t configuration_;
    std::vector<float> actualData_;
    rc_mpu_data_t data_;

    std::vector<float> GetAccel();
    std::vector<float> GetGyro();
    std::vector<float> GetQaternion();
    void ThreadFunction();
};

#endif
