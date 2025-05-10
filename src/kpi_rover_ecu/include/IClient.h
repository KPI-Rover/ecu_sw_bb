#ifndef ICLIENT_H
#define ICLIENT_H

#include <cstdint>
#include <string>
#include <vector>

class IClient {
   protected:
    IClient() = default;

   public:
    virtual bool Send(const std::vector<std::uint8_t>& data) = 0;
    virtual bool Receive(std::vector<std::uint8_t>& data) = 0;
    virtual int Init(std::string ip_address, int port) = 0;
    virtual void Destroy() = 0;
    // virtual ~IClient() = default;

    IClient(const IClient&) = delete;
    IClient& operator=(const IClient&) = delete;
    IClient(IClient&&) = delete;
    IClient& operator=(IClient&&) = delete;
};

#endif
