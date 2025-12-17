#include "protocolHandler.h"

#include <netinet/in.h>

#include <cstdint>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>

#include "loggingIncludes.h"
#include "motorsController.h"

using std::vector;

constexpr uint8_t ProtocolHanlder::kApiVersion;
constexpr uint8_t ProtocolHanlder::kIdGetApiVersion;
constexpr uint8_t ProtocolHanlder::kIdSetMotorSpeed;
constexpr uint8_t ProtocolHanlder::kIdSetAllMotorsSpeed;
constexpr uint8_t ProtocolHanlder::kIdGetEncoder;
constexpr uint8_t ProtocolHanlder::kIdGetAllEncoders;
constexpr uint8_t ProtocolHanlder::kIdGetImu;

ProtocolHanlder::ProtocolHanlder(MotorController& motorDriver, IMUController& imuController, ITransport& transport)
    : motors_controller_(motorDriver), imu_controller_(imuController), transport_(transport) {}

vector<uint8_t> ProtocolHanlder::HandleSetMotorSpeed(const vector<uint8_t>& message) {
    LOG_INFO << "Get command: set one motor";
    const uint8_t kMotorId = message[1];
    int32_t k_motor_rpm = 0;
    vector<uint8_t> ret_val;

    memcpy(&k_motor_rpm, &message[2], sizeof(int32_t));
    k_motor_rpm = static_cast<int32_t>(ntohl(k_motor_rpm));

    if (motors_controller_.SetSetpoint(static_cast<int>(kMotorId), static_cast<int>(k_motor_rpm)) != 0) {
        LOG_ERROR << "Error in SetSetpoint, retry connection";
        return {};
    }
    LOG_DEBUG << "motor " << static_cast<int>(kMotorId) << " new rpm " << static_cast<int>(k_motor_rpm);
    LOG_DEBUG << "Build response ";
    // buffer.assign(BUFFERSIZE, 0);
    ret_val.push_back(ProtocolHanlder::kIdSetMotorSpeed);
    ret_val.push_back(0);  // Status OK

    return ret_val;
}

vector<uint8_t> ProtocolHanlder::HandleGetApiVersion(const vector<uint8_t>& message) {
    LOG_INFO << "Get command: get api version";
    LOG_DEBUG << "Client Api version " << message[0];

    LOG_DEBUG << "Build response ";
    vector<uint8_t> ret_val;

    ret_val.push_back(ProtocolHanlder::kIdGetApiVersion);

    const uint8_t kResponse = kApiVersion;
    ret_val.push_back(kResponse);

    return ret_val;
}

vector<uint8_t> ProtocolHanlder::HandleSetAllMotorsSpeed(const vector<uint8_t>& message) {
    LOG_INFO << "Get command: set all motors ";

    std::vector<int32_t> motors_rpm_arr(motors_controller_.GetMotorsNumber(), 0);
    vector<uint8_t> ret_val;

    for (int i = 0; i < motors_controller_.GetMotorsNumber(); i++) {
        memcpy(&motors_rpm_arr[i], &message[1 + i * sizeof(int32_t)], sizeof(int32_t));
        motors_rpm_arr[i] = static_cast<int32_t>(ntohl(motors_rpm_arr[i]));
    }

    for (int i = 0; i < motors_controller_.GetMotorsNumber(); i++) {
        if (motors_rpm_arr[i] != 0) {
            LOG_DEBUG << "motor " << i << " new rpm " << static_cast<int>(motors_rpm_arr[i]);
        }

        if (motors_controller_.SetSetpoint(i, static_cast<int>(motors_rpm_arr[i])) != 0) {
            LOG_ERROR << "Error in setMotorRPM, retry connection";
            return {};
        }
    }
    LOG_DEBUG << "Build response ";
    ret_val.push_back(ProtocolHanlder::kIdSetAllMotorsSpeed);
    ret_val.push_back(0);  // Status OK
    return ret_val;
}

vector<uint8_t> ProtocolHanlder::HandleGetEncoder(const vector<uint8_t>& message) {
    LOG_INFO << "Get command: get one encoder ";
    const uint8_t kMotorId = message[1];
    vector<uint8_t> ret_val;
    LOG_DEBUG << "Get motor " << static_cast<int>(kMotorId) << " encoder ";

    const int32_t kMotorRpm = motors_controller_.GetEncoderCounter(kMotorId);
    LOG_DEBUG << "Build response ";
    ret_val.push_back(ProtocolHanlder::kIdGetEncoder);

    auto k_motor_rpm_order = static_cast<int32_t>(htonl(kMotorRpm));
    uint8_t buffer[sizeof(int32_t)];
    std::memcpy(buffer, &k_motor_rpm_order, sizeof(int32_t));
    ret_val.insert(ret_val.end(), buffer, buffer + sizeof(int32_t));
    return ret_val;
}

// TODO: Get rid of the [[maybe_unused]] attribute
vector<uint8_t> ProtocolHanlder::HandleGetAllEncoders(const vector<uint8_t>& message [[maybe_unused]]) const {
    LOG_INFO << "Get command: get all encoders ";
    vector<uint8_t> ret_val;
    ret_val.push_back(ProtocolHanlder::kIdGetAllEncoders);

    LOG_DEBUG << "Build response ";
    uint8_t buffer[sizeof(int32_t)];
    for (int i = 0; i < motors_controller_.GetMotorsNumber(); i++) {
        auto encoder_rpm = static_cast<int32_t>(htonl(motors_controller_.GetEncoderCounter(i)));
        std::memcpy(buffer, &encoder_rpm, sizeof(int32_t));
        ret_val.insert(ret_val.end(), buffer, buffer + sizeof(int32_t));
    }

    return ret_val;
}

static void appendFloatLittleEndian(std::vector<uint8_t>& vec, float value) {
    uint32_t val_as_int;
    std::memcpy(&val_as_int, &value, sizeof(float));
    
    // Explicitly pack as Little Endian (LSB first)
    uint8_t bytes[4];
    bytes[0] = (val_as_int >> 0) & 0xFF;
    bytes[1] = (val_as_int >> 8) & 0xFF;
    bytes[2] = (val_as_int >> 16) & 0xFF;
    bytes[3] = (val_as_int >> 24) & 0xFF;
    
    vec.insert(vec.end(), bytes, bytes + 4);
}

vector<uint8_t> ProtocolHanlder::HandleGetImu(const vector<uint8_t>& message) {
    LOG_INFO << "Get command: get imu";
    vector<uint8_t> ret_val;
    ret_val.push_back(ProtocolHanlder::kIdGetImu);

    // Get Data from IMU Controller
    std::vector<float> accel = imu_controller_.GetAccel();
    std::vector<float> gyro = imu_controller_.GetGyro();
    std::vector<float> mag = imu_controller_.GetMag();
    std::vector<float> quat = imu_controller_.GetQaternion();

    // Check if we have valid data (all vectors should be populated)
    if (accel.size() < 3 || gyro.size() < 3 || mag.size() < 3 || quat.size() < 4) {
        LOG_WARNING << "IMU data not ready or incomplete";
        // Fill with zeros ? or return empty? 
        // Protocol specifies format, so filling with zeros is safer for parsing
        for (int i = 0; i < 13; ++i) appendFloatLittleEndian(ret_val, 0.0f);
        return ret_val;
    }

    // Append Accel (X, Y, Z)
    appendFloatLittleEndian(ret_val, accel[0]);
    appendFloatLittleEndian(ret_val, accel[1]);
    appendFloatLittleEndian(ret_val, accel[2]);

    // Append Gyro (X, Y, Z)
    appendFloatLittleEndian(ret_val, gyro[0]);
    appendFloatLittleEndian(ret_val, gyro[1]);
    appendFloatLittleEndian(ret_val, gyro[2]);

    // Append Mag (X, Y, Z)
    appendFloatLittleEndian(ret_val, mag[0]);
    appendFloatLittleEndian(ret_val, mag[1]);
    appendFloatLittleEndian(ret_val, mag[2]);

    // Append Quat (W, X, Y, Z)
    appendFloatLittleEndian(ret_val, quat[0]); // W
    appendFloatLittleEndian(ret_val, quat[1]); // X
    appendFloatLittleEndian(ret_val, quat[2]); // Y
    appendFloatLittleEndian(ret_val, quat[3]); // Z

    return ret_val;
}

vector<uint8_t> ProtocolHanlder::HandleMessage(const vector<uint8_t>& message) {
    std::stringstream ss;
    for (const auto& byte : message) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte) << " ";
    }
    LOG_INFO << "Received data: " << ss.str();

    if (message.empty()) {
        LOG_WARNING << "Received empty message";
        return {};
    }

    const uint8_t kCmdId = message[0];
    vector<uint8_t> ret_val;  // std::cout << "server get command: " <<  static_cast<int>(kCmdId);

    if (kCmdId == ProtocolHanlder::kIdSetMotorSpeed) {
        ret_val = HandleSetMotorSpeed(message);

    } else if (kCmdId == ProtocolHanlder::kIdGetApiVersion) {
        ret_val = HandleGetApiVersion(message);

    } else if (kCmdId == ProtocolHanlder::kIdSetAllMotorsSpeed) {
        ret_val = HandleSetAllMotorsSpeed(message);

    } else if (kCmdId == ProtocolHanlder::kIdGetEncoder) {
        ret_val = HandleGetEncoder(message);

    } else if (kCmdId == ProtocolHanlder::kIdGetAllEncoders) {
        ret_val = HandleGetAllEncoders(message);

    } else if (kCmdId == ProtocolHanlder::kIdGetImu) {
        ret_val = HandleGetImu(message);

    } else {
        LOG_WARNING << "Symbol " << static_cast<int>(kCmdId) << " wasn't designated as command ID";
        return {};
    }

    return ret_val;
}

vector<uint8_t> ProtocolHanlder::MotorsStopMessage() {
    vector<uint8_t> ret_val;
    int32_t stop_value = 0;
    ret_val.push_back(ProtocolHanlder::kIdSetAllMotorsSpeed);
    uint8_t buffer[sizeof(int32_t)];
    for (int i = 0; i < motors_controller_.GetMotorsNumber(); i++) {
        std::memcpy(buffer, &stop_value, sizeof(int32_t));
        ret_val.insert(ret_val.end(), buffer, buffer + sizeof(int32_t));
    }

    return ret_val;
}
