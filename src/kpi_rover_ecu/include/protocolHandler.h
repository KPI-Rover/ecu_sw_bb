#ifndef PROTOCOLHANDLER_H
#define PROTOCOLHANDLER_H

#include <cstdint>
#include <cstring>
#include <vector>

#include "motorsController.h"

class ProtocolHanlder {
   public:
    static constexpr uint8_t kApiVersion = 0x01;
    static constexpr uint8_t kIdGetApiVersion = 0x01;
    static constexpr uint8_t kIdSetMotorSpeed = 0x02;
    static constexpr uint8_t kIdSetAllMotorsSpeed = 0x03;
    static constexpr uint8_t kIdGetEncoder = 0x04;
    static constexpr uint8_t kIdGetAllEncoders = 0x05;

    explicit ProtocolHanlder(MotorController* motorDriver);

    std::vector<uint8_t> HandleMessage(const std::vector<uint8_t>& message);
    std::vector<uint8_t> MotorsStopMessage();

   private:
    MotorController* main_controller_;
    std::vector<uint8_t> HandleSetMotorSpeed(const std::vector<uint8_t>& message);
    std::vector<uint8_t> HandleGetApiVersion(const std::vector<uint8_t>& message);
    std::vector<uint8_t> HandleSetAllMotorsSpeed(const std::vector<uint8_t>& message);
    std::vector<uint8_t> HandleGetEncoder(const std::vector<uint8_t>& message);
    std::vector<uint8_t> HandleGetAllEncoders(const std::vector<uint8_t>& message) const;
};

#endif