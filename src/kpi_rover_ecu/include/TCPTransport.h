#ifndef TCPTRANSPORT_H
#define TCPTRANSPORT_H

#include <atomic>
#include <cstdint>
#include <cstring>
#include <thread>
#include <vector>

#include "ITransport.h"
#include "messageQueue.h"

class TCPTransport : public ITransport {
   public:
    static constexpr int kNumSlots = 5;
    static constexpr int kBufferSize = 64;

    TCPTransport(const char* ip_address, int port);
    TCPTransport(const TCPTransport&) = delete;
    TCPTransport& operator=(const TCPTransport&) = delete;
    TCPTransport(TCPTransport&&) = delete;
    TCPTransport& operator=(TCPTransport&&) = delete;
    ~TCPTransport() override;

    bool Send(const std::vector<std::uint8_t>& data) override;
    bool Receive(std::vector<std::uint8_t>& data) override;
    void Start() override;
    int Init() override;
    void Destroy() override;

   private:
    int sockfd_, client_sockfd_;
    char* server_address_;
    int server_portnum_;
    std::atomic<bool> running_;
    std::thread acceptThread_;
    MessageQueue messageQueue_;
};

#endif
