#ifndef TCPTRANSPORT_H
#define TCPTRANSPORT_H

#include <arpa/inet.h>
#include <atomic>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <thread>
#include <vector>
#include "ITransport.h"
#include "messageQueue.h"

#define NUMSLOTS 5              // how many connections can server process
#define BUFFERSIZE 64           // maximum size of buffer

using namespace std;

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