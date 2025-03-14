#ifndef PROTOCOLHANDLER_H
#define PROTOCOLHANDLER_H

#include "config.h"
#include "motorsController.h"

class ProtocolHanlder {
   public:
    

    ProtocolHanlder(MotorController* motorDriver);

    vector<uint8_t> HandleMessage(vector<uint8_t> message);

   private:
    MotorController* main_controller_;
    vector<uint8_t> HandleSetMotorSpeed(vector<uint8_t> message);
    vector<uint8_t> HandleGetApiVersion(vector<uint8_t> message) const ;
    vector<uint8_t> HandleSetAllMotorsSpeed(vector<uint8_t> message);
    vector<uint8_t> HandleGetEncoder(vector<uint8_t> message);
    vector<uint8_t> HandleGetAllEncoders(vector<uint8_t> message) const ;
};

#endif