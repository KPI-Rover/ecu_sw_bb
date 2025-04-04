#include "TCPTransport.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <vector>

constexpr std::size_t kBufferSize = 1024;
constexpr int kTimeoutMs = 50;

TCPTransport::TCPTransport(const char *ip_address, int port)
    : sockfd_(-1),
      client_sockfd_(-1),
      server_portnum_(port),
      running_(true),
      server_address_(new char[strlen(ip_address) + 1]) {
    strncpy(server_address_, ip_address, strlen(ip_address) + 1);
}

TCPTransport::~TCPTransport() {
    running_ = false;
    shutdown(sockfd_, SHUT_RDWR);
    std::cout << "joining thread ... " << '\n';
    acceptThread_.join();
    std::cout << "closing socket" << '\n';
    close(client_sockfd_);
    close(sockfd_);
    delete[] server_address_;
}

int TCPTransport::Init() {
    struct sockaddr_in serv_addr = {};
    int reuseaddr = 1;

    sockfd_ = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd_ < 0) {
        perror("[ERROR] Socket()");
        return -1;
    }

    if (setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof(int)) != 0) {
        perror("[ERROR] setsockopt()");
        return -1;
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(server_address_);
    serv_addr.sin_port = htons(server_portnum_);

    if (bind(sockfd_, reinterpret_cast<struct sockaddr *>(&serv_addr), sizeof(serv_addr)) != 0) {
        perror("[ERROR] bind()");
        return -1;
    }

    if (listen(sockfd_, TCPTransport::kNumSlots) != 0) {
        perror("[ERROR] listen");
        return -1;
    }

    std::cout << "[INFO] Started server on " << server_address_ << ":" << server_portnum_ << '\n';

    return sockfd_;
}

void TCPTransport::Start() {
    acceptThread_ = std::thread([this]() {
        while (running_) {
            std::cout << "Waiting for connection..." << '\n';
            client_sockfd_ = accept(sockfd_, nullptr, nullptr);
            if (client_sockfd_ >= 0) {
                std::cout << "Client connected." << '\n';
                while (true) {  // Use true instead of 1
                    std::uint8_t buffer[kBufferSize];
                    const ssize_t kBytesReceived = recv(client_sockfd_, buffer, sizeof(buffer), 0);
                    if (kBytesReceived > 0) {
                        const std::vector<std::uint8_t> kMessage(buffer, buffer + kBytesReceived);
                        messageQueue_.Push(kMessage);
                    } else {
                        std::cout << "Client disconnected." << '\n';
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
    return ::send(client_sockfd_, data.data(), data.size(), 0) != -1;
}

bool TCPTransport::Receive(std::vector<std::uint8_t> &data) { return messageQueue_.Pop(data, kTimeoutMs); }

void TCPTransport::Destroy() {
    running_ = false;
    shutdown(sockfd_, SHUT_RDWR);
    std::cout << "joining thread ... " << '\n';
    acceptThread_.join();
    std::cout << "closing socket" << '\n';
    close(client_sockfd_);
    close(sockfd_);
}
