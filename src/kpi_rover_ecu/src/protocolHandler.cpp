#include "protocolHandler.h"

#include "config.h"

ProtocolHanlder::ProtocolHanlder(MotorController* motorDriver) { main_controller_ = motorDriver; }

vector<uint8_t> ProtocolHanlder::handleSetMotorSpeed(vector<uint8_t> message) {
    const uint8_t motor_id = message[1];
    int32_t motor_rpm = 0;
    vector<uint8_t> ret_val;

    memcpy(&motor_rpm, &message[2], sizeof(int32_t));
    motor_rpm = ntohl(motor_rpm);

    std::cout << "[COMMAND] motor " << static_cast<int>(motor_id) << " new rpm " << static_cast<int>(motor_rpm) << '\n';
    std::cout << "[INFO] Motor set run" << '\n';

    if (main_controller_->setMotorRPM(static_cast<int>(motor_id), static_cast<int>(motor_rpm)) != 0) {
        std::cout << "[ERROR] Error setMotorRPM, retry connection" << '\n';
        return {};
    }

    // buffer.assign(BUFFERSIZE, 0);
    ret_val.push_back(ID_SET_MOTOR_SPEED);

    return ret_val;
}

vector<uint8_t> ProtocolHanlder::handleGetApiVersion(vector<uint8_t> message) {
    std::cout << "[COMMAND] get api version " << '\n';
    vector<uint8_t> ret_val;

    // buffer.assign(BUFFERSIZE, 0);
    ret_val.push_back(ID_GET_API_VERSION);

    const uint8_t response = 1;  // in next versions it can get value from some function
    ret_val.push_back(response);

    return ret_val;
}

vector<uint8_t> ProtocolHanlder::handleSetAllMotorsSpeed(vector<uint8_t> message) {
    std::vector<int32_t> motors_rpm_arr(MOTORS_NUMBER, 0);
    vector<uint8_t> ret_val;

    for (int i = 0; i < main_controller_->motor_number_; i++) {
        memcpy(&motors_rpm_arr[i], &message[1 + i * sizeof(int32_t)], sizeof(int32_t));
        motors_rpm_arr[i] = ntohl(motors_rpm_arr[i]);
    }

    for (int i = 0; i < main_controller_->motor_number_; i++) {
        if (motors_rpm_arr[i] != 0) {
            std::cout << "[COMMAND] motor " << i << " new rpm " << static_cast<int>(motors_rpm_arr[i]) << '\n';
        }

        if (main_controller_->setMotorRPM(i, static_cast<int>(motors_rpm_arr[i])) != 0) {
            std::cout << "[ERROR] Error setMotorRPM, retry connection" << '\n';
            return {};
        }
    }

    ret_val.push_back(ID_SET_ALL_MOTORS_SPEED);
    return ret_val;
}

vector<uint8_t> ProtocolHanlder::handleGetEncoder(vector<uint8_t> message) {
    const uint8_t motor_id = message[1];
    vector<uint8_t> ret_val;
    std::cout << "[COMMAND] get motor " << static_cast<int>(motor_id) << " encoder " << '\n';

    const int32_t motor_rpm = main_controller_->getMotorRPM(motor_id);

    ret_val.push_back(ID_GET_ENCODER);

    int32_t motor_rpm_order = static_cast<int32_t>(htonl(motor_rpm));
    ret_val.insert(ret_val.end(), reinterpret_cast<uint8_t*>(&motor_rpm_order),
                   reinterpret_cast<uint8_t*>(&motor_rpm_order) + sizeof(int32_t));
    return ret_val;
}

vector<uint8_t> ProtocolHanlder::handleGetAllEncoders(vector<uint8_t> message) {
    std::cout << "[COMMAND] get all encoders " << '\n';
    vector<uint8_t> ret_val;
    ret_val.push_back(ID_GET_ALL_ENCODERS);

    for (int i = 0; i < main_controller_->motor_number_; i++) {
        int32_t encoder_rpm = static_cast<int32_t>(htonl(main_controller_->getMotorRPM(i)));
        ret_val.insert(ret_val.end(), reinterpret_cast<uint8_t*>(&encoder_rpm),
                       reinterpret_cast<uint8_t*>(&encoder_rpm) + sizeof(int32_t));
    }

    return ret_val;
}

vector<uint8_t> ProtocolHanlder::handleMessage(vector<uint8_t> message) {
    const uint8_t cmd_id = message[0];
    vector<uint8_t> ret_val;  // std::cout << "server get command: " <<  static_cast<int>(cmd_id) << '\n';

    if (cmd_id == ID_SET_MOTOR_SPEED) {
        ret_val = handleSetMotorSpeed(message);

    } else if (cmd_id == ID_GET_API_VERSION) {
        ret_val = handleGetApiVersion(message);

    } else if (cmd_id == ID_SET_ALL_MOTORS_SPEED) {
        ret_val = handleSetAllMotorsSpeed(message);

    } else if (cmd_id == ID_GET_ENCODER) {
        ret_val = handleGetEncoder(message);

    } else if (cmd_id == ID_GET_ALL_ENCODERS) {
        ret_val = handleGetAllEncoders(message);

    } else {
        std::cout << "[ERROR] Command " << static_cast<int>(cmd_id) << " wasn't designated" << '\n';
        return {};
    }

    return ret_val;
}
