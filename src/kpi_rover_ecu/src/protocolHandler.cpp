#include "protocolHandler.h"
#include "config.h"



ProtocolHanlder::ProtocolHanlder(MotorController* motorDriver) {
	mainController = motorDriver;
}

vector<uint8_t> ProtocolHanlder::handleSetMotorSpeed(vector<uint8_t> message) {
	uint8_t motor_id = message[1];
    int32_t motor_rpm;
	vector<uint8_t> ret_val;

    memcpy(&motor_rpm, &message[2], sizeof(int32_t));
    motor_rpm = ntohl(motor_rpm);				
    
    cout << "[COMMAND] motor " << static_cast<int>(motor_id) 
         << " new rpm " << static_cast<int>(motor_rpm) << endl;
    cout << "[INFO] Motor set run" << endl;

    if (mainController->setMotorRPM(static_cast<int>(motor_id), 
                                      static_cast<int>(motor_rpm)) != 0) {
        cout << "[ERROR] Error setMotorRPM, retry connection" << endl;
		return vector<uint8_t>();
    }

    //buffer.assign(BUFFERSIZE, 0);
    ret_val.push_back(ID_SET_MOTOR_SPEED);

	return ret_val;
}

vector<uint8_t> ProtocolHanlder::handleGetApiVersion(vector<uint8_t> message) {
	cout << "[COMMAND] get api version " << endl;
	vector<uint8_t> ret_val;

    //buffer.assign(BUFFERSIZE, 0);
    ret_val.push_back(ID_GET_API_VERSION);
    
    uint8_t response = 1; // in next versions it can get value from some function
    ret_val.push_back(response);
    
	return ret_val;
}

vector<uint8_t> ProtocolHanlder::handleSetAllMotorsSpeed(vector<uint8_t> message) {
	std::vector<int32_t> motors_rpm_arr(MOTORS_NUMBER, 0);
	vector<uint8_t> ret_val;
    
    for (int i = 0; i < mainController->motorNumber; i++) {
        memcpy(&motors_rpm_arr[i], &message[1 + i * sizeof(int32_t)], sizeof(int32_t));
        motors_rpm_arr[i] = ntohl(motors_rpm_arr[i]);
    }

    for (int i = 0; i < mainController->motorNumber; i++) {
        if (motors_rpm_arr[i] != 0) {
            cout << "[COMMAND] motor " << i 
             << " new rpm " << static_cast<int>(motors_rpm_arr[i]) << endl;
        }
        
        if (mainController->setMotorRPM(i, static_cast<int>(motors_rpm_arr[i])) != 0) {
            cout << "[ERROR] Error setMotorRPM, retry connection" << endl;
            return vector<uint8_t>();
        }
    }


    ret_val.push_back(ID_SET_ALL_MOTORS_SPEED);
    return ret_val;
}

vector<uint8_t> ProtocolHanlder::handleGetEncoder(vector<uint8_t> message) {
	uint8_t motor_id = message[1];
	vector<uint8_t> ret_val;
    cout << "[COMMAND] get motor " << static_cast<int>(motor_id) << " encoder " << endl;
    
    int32_t motor_rpm = mainController->getMotorRPM(motor_id);

    ret_val.push_back(ID_GET_ENCODER);

    int32_t motor_rpm_order = htonl(motor_rpm);
    ret_val.insert(ret_val.end(), 
                  reinterpret_cast<uint8_t*>(&motor_rpm_order), 
                  reinterpret_cast<uint8_t*>(&motor_rpm_order) + sizeof(int32_t));
	return ret_val;
}

vector<uint8_t> ProtocolHanlder::handleGetAllEncoders(vector<uint8_t> message) {
	cout << "[COMMAND] get all encoders " << endl;
	vector<uint8_t> ret_val;
    ret_val.push_back(ID_GET_ALL_ENCODERS);

    for (int i = 0; i < mainController->motorNumber; i++) {
        int32_t encoder_RPM = htonl(mainController->getMotorRPM(i));
        ret_val.insert(ret_val.end(), 
                      reinterpret_cast<uint8_t*>(&encoder_RPM), 
                      reinterpret_cast<uint8_t*>(&encoder_RPM) + sizeof(int32_t));
    }

	return ret_val;
}

vector<uint8_t> ProtocolHanlder::handleMessage(vector<uint8_t> message) {
	uint8_t cmd_id = message[0];
	vector<uint8_t> ret_val;       //cout << "server get command: " <<  static_cast<int>(cmd_id) << endl; 

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
		cout << "[ERROR] Command " << static_cast<int>(cmd_id) << " wasn't designated" << endl;
		return vector<uint8_t>();
	}

	return ret_val;
}

