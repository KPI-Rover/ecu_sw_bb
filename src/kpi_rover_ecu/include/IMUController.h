#ifndef IMUCONTROLLER_H
#define IMUCONTROLLER_H

#include <rc/mpu.h>
#include <mutex>
#include <vector>

class IMUController {
   public:
    IMUController();
    ~IMUController();

    int Init();
    void Stop();

    std::vector<float> GetAccel();
    std::vector<float> GetGyro();
    std::vector<float> GetQaternion();
    std::vector<float> GetMag();

   private:
    // Constants
    const int kI2cBus = 2;
    const int kGpioIntPinChip = 3;
    const int kGpioIntPinPin = 21;
    const int kDmpSampleRate = 100;
    const int kEnableMagnetometer = 1;

    // Singleton instance for C-style callback
    static IMUController* instance_;

    rc_mpu_config_t configuration_;
    rc_mpu_data_t data_; // Internal buffer for rc_mpu library

    // Thread-safe cached data
    struct CachedData {
        float accel[3];
        float gyro[3];
        float quat[4];
        float mag[3];
    } cached_data_;
    
    mutable std::mutex data_mutex_;

    static void DMPCallbackWrapper();
    void OnDMPCallback();
};

#endif
