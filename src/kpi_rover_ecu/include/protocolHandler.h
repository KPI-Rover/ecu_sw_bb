#ifndef PROTOCOLHANDLER_H
#define PROTOCOLHANDLER_H

#include "config.h"
#include "motorsController.h"

class ProtocolHanlder {
public:
	MotorController* mainController; 

	ProtocolHanlder(MotorController* motorDriver);

	vector<uint8_t> handleMessage(vector<uint8_t> message);

private:
	vector<uint8_t> handleSetMotorSpeed(vector<uint8_t> message);
	vector<uint8_t> handleGetApiVersion(vector<uint8_t> message);
	vector<uint8_t> handleSetAllMotorsSpeed(vector<uint8_t> message);
	vector<uint8_t> handleGetEncoder(vector<uint8_t> message);
	vector<uint8_t> handleGetAllEncoders(vector<uint8_t> message);
};

#endif