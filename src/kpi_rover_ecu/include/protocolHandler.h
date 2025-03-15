#ifndef PROTOCOLHANDLER_H
#define PROTOCOLHANDLER_H

#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <vector>
#include "motorsController.h"

#define ID_GET_API_VERSION 0x01
#define ID_SET_MOTOR_SPEED 0x02
#define ID_SET_ALL_MOTORS_SPEED 0x03
#define ID_GET_ENCODER 0x04
#define ID_GET_ALL_ENCODERS 0x05

class ProtocolHanlder {
   public:
    

    ProtocolHanlder(MotorController* motorDriver);

    vector<uint8_t> HandleMessage(vector<uint8_t> message);
    vector<uint8_t> MotorsStopMessage();

   private:
    MotorController* main_controller_;
    vector<uint8_t> HandleSetMotorSpeed(vector<uint8_t> message);
    vector<uint8_t> HandleGetApiVersion(vector<uint8_t> message) const ;
    vector<uint8_t> HandleSetAllMotorsSpeed(vector<uint8_t> message);
    vector<uint8_t> HandleGetEncoder(vector<uint8_t> message);
    vector<uint8_t> HandleGetAllEncoders(vector<uint8_t> message) const ;
};

#endif