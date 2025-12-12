#include "IMUController.h"

#include <arpa/inet.h>
#include <rc/mpu.h>
#include <rc/time.h>

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <vector>

#include "loggingIncludes.h"

IMUController::IMUController()
    : data_{},
      isStarted_(false),
      isSending_(false),
      udp_client_(nullptr),
      configuration_(rc_mpu_default_config()),
      actualData_({}) {
    // Enable VLOG(1) for this file specifically
    google::SetVLOGLevel("IMUController", 1);
    
    configuration_.i2c_bus = kI2cBus;
    configuration_.gpio_interrupt_pin_chip = kGpioIntPinChip;
    configuration_.gpio_interrupt_pin = kGpioIntPinPin;
    configuration_.dmp_sample_rate = kDmpSampleRate;
    configuration_.enable_magnetometer = kEnableMagnetometer;
}

int IMUController::Init() {
    if (rc_mpu_initialize_dmp(&data_, configuration_) != 0) {
        std::cout << "Initializing MPU failed" << '\n';
        return -1;
    }

    return 0;
}

void IMUController::Start(UDPClient* udp_client) {
    udp_client_ = udp_client;
    isStarted_ = true;
    processingThread_ = std::thread([this] { ThreadFunction(); });
}

void IMUController::ConnectUDP(std::string ip, int port) {
    if (udp_client_ != nullptr) {
        udp_client_->Init(ip, port);
        isSending_ = true;
    }
}

void IMUController::ThreadFunction() {
    uint16_t packet_number = 0;

    while (isStarted_) {
        if (isSending_ && udp_client_ != nullptr) {
             const std::vector<float> kImuData = GetData();
            if (!kImuData.empty()) {
                if (packet_number == k16MaxCount) {
                   packet_number = 0;
                }
                packet_number += 1;

                std::vector<uint8_t> send_val;
                send_val.push_back(GetId());

                uint16_t send_packet_number = htons(packet_number);
                auto* bytes = reinterpret_cast<uint8_t*>(&send_packet_number);
                for (size_t i = 0; i < 2; ++i) {
                    send_val.push_back(bytes[i]);
                }

                float insert_value = 0;
                uint32_t value = 0;

                for (const float kImuValue : kImuData) {
                    insert_value = kImuValue;
                    std::memcpy(&value, &insert_value, sizeof(float));
                    value = ntohl(value);
                    bytes = reinterpret_cast<uint8_t*>(&value);

                    for (size_t j = 0; j < sizeof(uint32_t); ++j) {
                        send_val.push_back(bytes[j]);
                    }
                }

                udp_client_->Send(send_val);
            }
        }
        rc_usleep(kTimerPrecision);
    }
}

void IMUController::SetEnable() { isStarted_ = true; }

void IMUController::SetDisable() { isStarted_ = false; }

std::vector<float> IMUController::GetData() {
    if (!isStarted_) {
        return {};
    }
    LOG_DEBUG << "Read data from accel and gyro";
    rc_mpu_read_accel(&data_);
    rc_mpu_read_gyro(&data_);

    actualData_.resize(kActualDataSize);
    const std::vector<float> kAccelData = GetAccel();
    LOG_DEBUG << "Get data from accelerometr";
    const int kAccelDataSize = static_cast<int>(kAccelData.size());
    std::copy(kAccelData.begin(), kAccelData.begin() + kAccelDataSize, actualData_.begin());

    const std::vector<float> kGyroData = GetGyro();
    LOG_DEBUG << "Get data from gyroscope";
    const int kGyroDataSize = static_cast<int>(kGyroData.size());
    std::copy(kGyroData.begin(), kGyroData.begin() + kGyroDataSize, actualData_.begin() + kAccelDataSize);

    const std::vector<float> kQaternionData = GetQaternion();
    LOG_DEBUG << "Get data from magnetometer (qaternion)";
    const int kQaternionDataSize = static_cast<int>(kQaternionData.size());
    std::copy(kQaternionData.begin(), kQaternionData.begin() + kQaternionDataSize,
              actualData_.begin() + kAccelDataSize + kGyroDataSize);

    return actualData_;
}

std::vector<float> IMUController::GetAccel() {
    std::vector<float> ret_val;

    ret_val.push_back(static_cast<float>(data_.accel[0]));
    ret_val.push_back(static_cast<float>(data_.accel[1]));
    ret_val.push_back(static_cast<float>(data_.accel[2]));

    return ret_val;
}

std::vector<float> IMUController::GetGyro() {
    std::vector<float> ret_val;

    ret_val.push_back(static_cast<float>(data_.gyro[0]));
    ret_val.push_back(static_cast<float>(data_.gyro[1]));
    ret_val.push_back(static_cast<float>(data_.gyro[2]));

    return ret_val;
}

std::vector<float> IMUController::GetQaternion() {
    std::vector<float> ret_val;

    ret_val.push_back(static_cast<float>(data_.dmp_quat[QUAT_W]));
    ret_val.push_back(static_cast<float>(data_.dmp_quat[QUAT_X]));
    ret_val.push_back(static_cast<float>(data_.dmp_quat[QUAT_Y]));
    ret_val.push_back(static_cast<float>(data_.dmp_quat[QUAT_Z]));

    return ret_val;
}

void IMUController::Stop() {
    isStarted_ = false;
    isSending_ = false;
    if (processingThread_.joinable()) {
        processingThread_.join();
    }
    rc_mpu_power_off();
}

uint8_t IMUController::GetId() { return kIdGetCommand; }
