#include "UDPClient.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

UDPClient::UDPClient() : sockfd_(-1), server_portnum_(0), server_address_{} {}

int UDPClient::Init(std::string ip_address, int port) {
    std::strncpy(server_address_, ip_address.c_str(), INET_ADDRSTRLEN);
    server_address_[INET_ADDRSTRLEN - 1] = '\0';

    server_portnum_ = port;

    std::cout << "initialize socket" << '\n';
    sockfd_ = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd_ == -1) {
        std::cout << "Error while creating socket" << '\n';
        return -1;
    }

    memset(&serverStruct_, 0, sizeof(serverStruct_));
    serverStruct_.sin_family = AF_INET;
    serverStruct_.sin_addr.s_addr = inet_addr(server_address_);  // Server IP address (localhost)
    serverStruct_.sin_port = htons(server_portnum_);

    return 0;
}

bool UDPClient::Send(const std::vector<std::uint8_t> &data) {
    if (sockfd_ < 0) {
        return false;
    }

    return sendto(sockfd_, data.data(), data.size(), 0, reinterpret_cast<sockaddr *>(&serverStruct_),
                  sizeof(serverStruct_)) != -1;
}

bool UDPClient::Receive(std::vector<std::uint8_t> &data) { return false; }

void UDPClient::Destroy() {}

UDPClient::~UDPClient() {
    shutdown(sockfd_, SHUT_WR);

    close(sockfd_);
}
