#include "protocolHandler.h"

#include <arpa/inet.h>
#include "motorsController.h"

ProtocolHanlder::ProtocolHanlder(MotorController* motorDriver) : main_controller_(motorDriver) {}

vector<uint8_t> ProtocolHanlder::HandleSetMotorSpeed(vector<uint8_t> message) {
    const uint8_t kMotorId = message[1];
    int32_t k_motor_rpm = 0;
    vector<uint8_t> ret_val;

    memcpy(&k_motor_rpm, &message[2], sizeof(int32_t));
    k_motor_rpm = static_cast<int32_t>(ntohl(k_motor_rpm));

    std::cout << "[COMMAND] motor " << static_cast<int>(kMotorId) << " new rpm " << static_cast<int>(k_motor_rpm) << '\n';
    std::cout << "[INFO] Motor set run" << '\n';

    if (main_controller_->SetMotorRPM(static_cast<int>(kMotorId), static_cast<int>(k_motor_rpm)) != 0) {
        std::cout << "[ERROR] Error setMotorRPM, retry connection" << '\n';
        return {};
    }

    // buffer.assign(BUFFERSIZE, 0);
    ret_val.push_back(ID_SET_MOTOR_SPEED);

    return ret_val;
}

vector<uint8_t> ProtocolHanlder::HandleGetApiVersion(vector<uint8_t> message) const {
    std::cout << "[COMMAND] get api version " << '\n';
    vector<uint8_t> ret_val;

    // buffer.assign(BUFFERSIZE, 0);
    ret_val.push_back(ID_GET_API_VERSION);

    const uint8_t kResponse = 1;  // in next versions it can get value from some function
    ret_val.push_back(kResponse);

    return ret_val;
}

vector<uint8_t> ProtocolHanlder::HandleSetAllMotorsSpeed(vector<uint8_t> message) {
    std::vector<int32_t> motors_rpm_arr(main_controller_->GetMotorsNumber(), 0);
    vector<uint8_t> ret_val;

    for (int i = 0; i < main_controller_->GetMotorsNumber(); i++) {
        memcpy(&motors_rpm_arr[i], &message[1 + i * sizeof(int32_t)], sizeof(int32_t));
        motors_rpm_arr[i] = static_cast<int32_t>(ntohl(motors_rpm_arr[i]));
    }

    for (int i = 0; i < main_controller_->GetMotorsNumber(); i++) {
        if (motors_rpm_arr[i] != 0) {
            std::cout << "[COMMAND] motor " << i << " new rpm " << static_cast<int>(motors_rpm_arr[i]) << '\n';
        }

        if (main_controller_->SetMotorRPM(i, static_cast<int>(motors_rpm_arr[i])) != 0) {
            std::cout << "[ERROR] Error setMotorRPM, retry connection" << '\n';
            return {};
        }
    }

    ret_val.push_back(ID_SET_ALL_MOTORS_SPEED);
    return ret_val;
}

vector<uint8_t> ProtocolHanlder::HandleGetEncoder(vector<uint8_t> message) {
    const uint8_t kMotorId = message[1];
    vector<uint8_t> ret_val;
    std::cout << "[COMMAND] get motor " << static_cast<int>(kMotorId) << " encoder " << '\n';

    const int32_t kMotorRpm = main_controller_->GetEncoderCounter(kMotorId);

    ret_val.push_back(ID_GET_ENCODER);

    int32_t k_motor_rpm_order = static_cast<int32_t>(htonl(kMotorRpm));
    ret_val.insert(ret_val.end(), reinterpret_cast<uint8_t*>(&k_motor_rpm_order),
                   reinterpret_cast<uint8_t*>(&k_motor_rpm_order) + sizeof(int32_t));
    return ret_val;
}

vector<uint8_t> ProtocolHanlder::HandleGetAllEncoders(vector<uint8_t> message) const  {
    std::cout << "[COMMAND] get all encoders " << '\n';
    vector<uint8_t> ret_val;
    ret_val.push_back(ID_GET_ALL_ENCODERS);

    for (int i = 0; i < main_controller_->GetMotorsNumber(); i++) {
        int32_t encoder_rpm = static_cast<int32_t>(htonl(main_controller_->GetEncoderCounter(i)));
        ret_val.insert(ret_val.end(), reinterpret_cast<uint8_t*>(&encoder_rpm),
                       reinterpret_cast<uint8_t*>(&encoder_rpm) + sizeof(int32_t));
    }

    return ret_val;
}

vector<uint8_t> ProtocolHanlder::HandleMessage(vector<uint8_t> message) {
    const uint8_t kCmdId = message[0];
    vector<uint8_t> ret_val;  // std::cout << "server get command: " <<  static_cast<int>(kCmdId) << '\n';

    if (kCmdId == ID_SET_MOTOR_SPEED) {
        ret_val = HandleSetMotorSpeed(message);

    } else if (kCmdId == ID_GET_API_VERSION) {
        ret_val = HandleGetApiVersion(message);

    } else if (kCmdId == ID_SET_ALL_MOTORS_SPEED) {
        ret_val = HandleSetAllMotorsSpeed(message);

    } else if (kCmdId == ID_GET_ENCODER) {
        ret_val = HandleGetEncoder(message);

    } else if (kCmdId == ID_GET_ALL_ENCODERS) {
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
    ret_val.push_back(ID_SET_ALL_MOTORS_SPEED);
    for (int i = 0; i < main_controller_->GetMotorsNumber(); i++) {
        ret_val.insert(ret_val.end(), reinterpret_cast<uint8_t*>(&stop_value),
                          reinterpret_cast<uint8_t*>(&stop_value) + sizeof(int32_t));
    }

    return ret_val;
}
