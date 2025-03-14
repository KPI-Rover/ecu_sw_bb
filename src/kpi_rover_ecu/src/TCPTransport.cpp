#include "TCPTransport.h"

#include "ITransport.h"
#include "config.h"
#include "messageQueue.h"

TCPTransport::TCPTransport(const char *ip_address, int port) : server_portnum_(port), running_(true), server_address_(new char[strlen(ip_address) + 1]) {
    //server_address_ = new char[strlen(ip_address) + 1];
    strcpy(server_address_, ip_address);
}

int TCPTransport::Init() {
    /*
    Fucntions responsible for initialising new socket (TCP socket)
*/

    struct sockaddr_in serv_addr;
    int reuseaddr = 1;

    sockfd_ = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd_ < 0) {
        perror("[ERROR] Socket()");
        return -1;
    }

    if (setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR, (void *)&reuseaddr, (socklen_t)sizeof(int)) != 0) {
        perror("[ERROR] setsockopt()");
        return -1;
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(server_address_);
    serv_addr.sin_port = htons(server_portnum_);

    if (bind(sockfd_, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) != 0) {
        perror("[ERROR] bind()");
        return -1;
    }

    if (listen(sockfd_, NUMSLOTS) != 0) {
        perror("[ERROR] listen");
        return -1;
    }

    std::cout << "[INFO] Started server on " << server_address_ << ":" << server_portnum_ << '\n';

    return sockfd_;
}

void TCPTransport::Start() {
    acceptThread_ = thread([this]() {
        while (running_) {
            std::cout << "Waiting for connection..." << '\n';
            client_sockfd_ = accept(sockfd_, nullptr, nullptr);
            if (client_sockfd_ >= 0) {
                std::cout << "Client connected." << '\n';
                while (1) {
                    uint8_t buffer[1024];
                    const ssize_t kBytesReceived = recv(client_sockfd_, buffer, sizeof(buffer), 0);
                    if (kBytesReceived > 0) {
                        const vector<uint8_t> kMessage(buffer, buffer + kBytesReceived);
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

bool TCPTransport::Send(const vector<uint8_t> &data) {
    if (client_sockfd_ < 0) return false;
    return ::send(client_sockfd_, data.data(), data.size(), 0) != -1;
}

bool TCPTransport::Receive(vector<uint8_t> &data) { return messageQueue_.Pop(data, 50); }

void TCPTransport::Destroy() {
    running_ = false;
    shutdown(sockfd_, SHUT_RDWR);
    std::cout << "joining thread ... " << '\n';
    acceptThread_.join();
    std::cout << "closing socket" << '\n';
    close(client_sockfd_);
    close(sockfd_);
}