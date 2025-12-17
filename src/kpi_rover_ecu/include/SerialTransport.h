#ifndef SERIALTRANSPORT_H
#define SERIALTRANSPORT_H

#include <atomic>
#include <cstdint>
#include <string>
#include <thread>
#include <vector>

#include "ITransport.h"
#include "messageQueue.h"

class SerialTransport : public ITransport {
   public:
    SerialTransport(const std::string& device, int baudRate);
    ~SerialTransport() override;

    bool Send(const std::vector<std::uint8_t>& data) override;
    bool Receive(std::vector<std::uint8_t>& data) override;
    int Init() override;
    void Start() override;
    void Destroy() override;
    std::string GetClientIp() override;

   private:
    void ReadThread();
    uint16_t CalculateCRC16(const uint8_t* data, size_t length);

    std::string device_;
    int baudRate_;
    int fd_;
    std::atomic<bool> running_;
    std::thread readThread_;
    MessageQueue messageQueue_;
};

#endif
