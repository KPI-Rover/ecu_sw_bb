#include "protocolHandler.h"

#include <netinet/in.h>

#include <cstdint>
#include <cstring>
#include <iostream>
#include <vector>

#include "motorsController.h"

using std::vector;

constexpr uint8_t ProtocolHanlder::kApiVersion;
constexpr uint8_t ProtocolHanlder::kIdGetApiVersion;
constexpr uint8_t ProtocolHanlder::kIdSetMotorSpeed;
constexpr uint8_t ProtocolHanlder::kIdSetAllMotorsSpeed;
constexpr uint8_t ProtocolHanlder::kIdGetEncoder;
constexpr uint8_t ProtocolHanlder::kIdGetAllEncoders;

ProtocolHanlder::ProtocolHanlder(MotorController* motorDriver) : motors_controller_(motorDriver) {}

vector<uint8_t> ProtocolHanlder::HandleSetMotorSpeed(const vector<uint8_t>& message) {
    const uint8_t kMotorId = message[1];
    int32_t k_motor_rpm = 0;
    vector<uint8_t> ret_val;

    memcpy(&k_motor_rpm, &message[2], sizeof(int32_t));
    k_motor_rpm = static_cast<int32_t>(ntohl(k_motor_rpm));

    std::cout << "[COMMAND] motor " << static_cast<int>(kMotorId) << " new rpm " << static_cast<int>(k_motor_rpm)
              << '\n';
    std::cout << "[COMMAND] motor " << static_cast<int>(kMotorId) << " new rpm " << static_cast<int>(k_motor_rpm)
              << '\n';
    std::cout << "[INFO] Motor set run" << '\n';

    if (motors_controller_->SetMotorRPM(static_cast<int>(kMotorId), static_cast<int>(k_motor_rpm)) != 0) {
        std::cout << "[ERROR] Error setMotorRPM, retry connection" << '\n';
        return {};
    }

    // buffer.assign(BUFFERSIZE, 0);
    ret_val.push_back(ProtocolHanlder::kIdSetMotorSpeed);

    return ret_val;
}

vector<uint8_t> ProtocolHanlder::HandleGetApiVersion(const vector<uint8_t>& message) {
    std::cout << "[COMMAND] get api version " << '\n';
    std::cout << "[INFO] Client Api version " << message[0] << '\n';

    vector<uint8_t> ret_val;

    ret_val.push_back(ProtocolHanlder::kIdGetApiVersion);

    const uint8_t kResponse = kApiVersion;
    ret_val.push_back(kResponse);

    return ret_val;
}

vector<uint8_t> ProtocolHanlder::HandleSetAllMotorsSpeed(const vector<uint8_t>& message) {
    std::vector<int32_t> motors_rpm_arr(motors_controller_->GetMotorsNumber(), 0);
    vector<uint8_t> ret_val;

    for (int i = 0; i < motors_controller_->GetMotorsNumber(); i++) {
        memcpy(&motors_rpm_arr[i], &message[1 + i * sizeof(int32_t)], sizeof(int32_t));
        motors_rpm_arr[i] = static_cast<int32_t>(ntohl(motors_rpm_arr[i]));
    }

    for (int i = 0; i < motors_controller_->GetMotorsNumber(); i++) {
        if (motors_rpm_arr[i] != 0) {
            std::cout << "[COMMAND] motor " << i << " new rpm " << static_cast<int>(motors_rpm_arr[i]) << '\n';
        }

        if (motors_controller_->SetMotorRPM(i, static_cast<int>(motors_rpm_arr[i])) != 0) {
            std::cout << "[ERROR] Error setMotorRPM, retry connection" << '\n';
            return {};
        }
    }

    ret_val.push_back(ProtocolHanlder::kIdSetAllMotorsSpeed);
    return ret_val;
}

vector<uint8_t> ProtocolHanlder::HandleGetEncoder(const vector<uint8_t>& message) {
    const uint8_t kMotorId = message[1];
    vector<uint8_t> ret_val;
    std::cout << "[COMMAND] get motor " << static_cast<int>(kMotorId) << " encoder " << '\n';

    const int32_t kMotorRpm = motors_controller_->GetEncoderCounter(kMotorId);
    ret_val.push_back(ProtocolHanlder::kIdGetEncoder);

    auto k_motor_rpm_order = static_cast<int32_t>(htonl(kMotorRpm));
    uint8_t buffer[sizeof(int32_t)];
    std::memcpy(buffer, &k_motor_rpm_order, sizeof(int32_t));
    ret_val.insert(ret_val.end(), buffer, buffer + sizeof(int32_t));
    return ret_val;
}

// TODO: Get rid of the [[maybe_unused]] attribute
vector<uint8_t> ProtocolHanlder::HandleGetAllEncoders(const vector<uint8_t>& message [[maybe_unused]]) const {
    std::cout << "[COMMAND] get all encoders " << '\n';
    vector<uint8_t> ret_val;
    ret_val.push_back(ProtocolHanlder::kIdGetAllEncoders);

    uint8_t buffer[sizeof(int32_t)];
    for (int i = 0; i < motors_controller_->GetMotorsNumber(); i++) {
        auto encoder_rpm = static_cast<int32_t>(htonl(motors_controller_->GetEncoderCounter(i)));
        std::memcpy(buffer, &encoder_rpm, sizeof(int32_t));
        ret_val.insert(ret_val.end(), buffer, buffer + sizeof(int32_t));
    }

    return ret_val;
}

vector<uint8_t> ProtocolHanlder::HandleMessage(const vector<uint8_t>& message) {
    const uint8_t kCmdId = message[0];
    vector<uint8_t> ret_val;  // std::cout << "server get command: " <<  static_cast<int>(kCmdId) << '\n';

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

    } else {
        std::cout << "[ERROR] Command " << static_cast<int>(kCmdId) << " wasn't designated" << '\n';
        return {};
    }

    return ret_val;
}

vector<uint8_t> ProtocolHanlder::MotorsStopMessage() {
    vector<uint8_t> ret_val;
    int32_t stop_value = 0;
    ret_val.push_back(ProtocolHanlder::kIdSetAllMotorsSpeed);
    uint8_t buffer[sizeof(int32_t)];
    for (int i = 0; i < motors_controller_->GetMotorsNumber(); i++) {
        std::memcpy(buffer, &stop_value, sizeof(int32_t));
        ret_val.insert(ret_val.end(), buffer, buffer + sizeof(int32_t));
    }

    return ret_val;
}
