#ifndef UDPCLIENT_H
#define UDPCLIENT_H

#include <arpa/inet.h>
#include <netinet/in.h>

#include <cstdint>
#include <string>
#include <vector>

#include "IClient.h"

class UDPClient : public IClient {
   public:
    UDPClient();

    UDPClient(const UDPClient&) = delete;
    UDPClient& operator=(const UDPClient&) = delete;
    UDPClient(UDPClient&&) = delete;
    UDPClient& operator=(UDPClient&&) = delete;

    int Init(std::string ip_address, int port) override;
    bool Send(const std::vector<std::uint8_t>& data) override;
    bool Receive(std::vector<std::uint8_t>& data) override;
    void Destroy() override;
    // ~UDPClient() override;

   private:
    int sockfd_;
    sockaddr_in serverStruct_;
    char server_address_[INET_ADDRSTRLEN];
    int server_portnum_;
};

#endif
