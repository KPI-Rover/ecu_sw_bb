#ifndef TCPTRANSPORT_H
#define TCPTRANSPORT_H

#include "ITransport.h"
#include "config.h"
#include "messageQueue.h"

class TCPTransport : public ITransport {
   public:
    TCPTransport(const char* ip_address, int port);
    bool Send(const vector<uint8_t>& data) override;
    bool Receive(vector<uint8_t>& data) override;
    void Start() override;
    int Init() override;
    void Destroy() override;

   private:
    int sockfd_, client_sockfd_;
    char* server_address_;
    int server_portnum_;
    atomic<bool> running_;
    thread acceptThread_;

    MessageQueue messageQueue_;
};

#endif