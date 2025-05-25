#include "TCPTransport.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstdint>
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>

#include "loggingIncludes.h"

constexpr std::size_t kBufferSize = 1024;
constexpr int kTimeoutMs = 50;

TCPTransport::TCPTransport(const char *ip_address, int port)
    : sockfd_(-1),
      client_sockfd_(-1),
      server_portnum_(port),
      running_(true),
      server_address_(new char[strlen(ip_address) + 1]),
      source_port_(-1) {
    strncpy(server_address_, ip_address, strlen(ip_address) + 1);
}

TCPTransport::~TCPTransport() {
    running_ = false;
    shutdown(sockfd_, SHUT_RDWR);
    LOG_INFO << "joining thread ... ";
    acceptThread_.join();
    LOG_INFO << "closing socket";
    close(client_sockfd_);
    close(sockfd_);
    delete[] server_address_;
}

int TCPTransport::Init() {
    struct sockaddr_in serv_addr = {};
    int reuseaddr = 1;
    source_address_[0] = '\0';

    sockfd_ = socket(AF_INET, SOCK_STREAM, 0);
    

    if (sockfd_ < 0) {
        LOG_ERROR << "Socket()";
        return -1;
    }
    LOG_DEBUG << "Created socket";

    if (setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof(int)) != 0) {
        LOG_ERROR << "setsockopt()";
        return -1;
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(server_address_);
    serv_addr.sin_port = htons(server_portnum_);
    LOG_DEBUG << "Set socket parameters";

    if (bind(sockfd_, reinterpret_cast<struct sockaddr *>(&serv_addr), sizeof(serv_addr)) != 0) {
        LOG_ERROR << "bind()";
        return -1;
    }
    LOG_DEBUG << "Socket binded";

    if (listen(sockfd_, TCPTransport::kNumSlots) != 0) {
        LOG_ERROR << "listen()";
        return -1;
    }
    LOG_DEBUG << "Start listening";

    LOG_INFO << " Started server on " << server_address_ << ":" << server_portnum_;

    return sockfd_;
}

void TCPTransport::Start() {
    acceptThread_ = std::thread([this]() {
        struct sockaddr_in client_addr {};
        socklen_t client_add_size = sizeof(client_addr);

        while (running_) {
            LOG_INFO << "Waiting for connection...";
            client_sockfd_ = accept(sockfd_, reinterpret_cast<sockaddr *>(&client_addr), &client_add_size);
            if (client_sockfd_ >= 0) {
                inet_ntop(AF_INET, &client_addr.sin_addr, source_address_, INET_ADDRSTRLEN);
                source_port_ = static_cast<int>(ntohs(client_addr.sin_port));
                LOG_INFO << "Client connected, source address: " << std::string(source_address_) << " source port: " << source_port_;
                while (true) {  // Use true instead of 1
                    std::uint8_t buffer[kBufferSize];
                    const ssize_t kBytesReceived = recv(client_sockfd_, buffer, sizeof(buffer), 0);
                    if (kBytesReceived > 0) {
                        const std::vector<std::uint8_t> kMessage(buffer, buffer + kBytesReceived);
                        messageQueue_.Push(kMessage);
                        LOG_DEBUG << "Get message, put in MessageQueue";
                    } else {
                        LOG_INFO << "Client disconnected.";
                        close(client_sockfd_);
                        client_sockfd_ = -1;
                        break;
                    }
                }
            }
        }
    });
}

bool TCPTransport::Send(const std::vector<std::uint8_t> &data) {
    if (client_sockfd_ < 0) {
        return false;
    }
    LOG_DEBUG << "Send TCP packet to TCP client";

    return ::send(client_sockfd_, data.data(), data.size(), 0) != -1;
}

bool TCPTransport::Receive(std::vector<std::uint8_t> &data) { return messageQueue_.Pop(data, kTimeoutMs); }

std::string TCPTransport::GetClientIp() { return std::string(source_address_); }

int TCPTransport::GetClientPort() { return source_port_; }

void TCPTransport::Destroy() {
    running_ = false;
    shutdown(sockfd_, SHUT_RDWR);
    LOG_INFO << "joining thread ... ";
    acceptThread_.join();
    LOG_INFO << "closing socket";
    close(client_sockfd_);
    close(sockfd_);
    delete[] server_address_;
}
