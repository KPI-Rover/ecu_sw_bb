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
#include <vector>

UDPClient::UDPClient(const char* ip_address, int port): sockfd_(-1), client_portnum_(port), server_address_(new char[strlen(ip_address) + 1]) {
	strncpy(server_address_, ip_address, strlen(ip_address) + 1);
}

void UDPClient::Start() {}

int UDPClient::Init() {
	std::cout << "initialize socket" << '\n';
	sockfd_ = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd_ == -1) {
		std::cout << "Error while creating socket" << '\n';
		return -1;
	}

	memset(&serverStruct_, 0, sizeof(serverStruct_));
    serverStruct_.sin_family = AF_INET;
    serverStruct_.sin_addr.s_addr = inet_addr(server_address_); // Server IP address (localhost)
    serverStruct_.sin_port = htons(client_portnum_);

	return 0;
}

bool UDPClient::Send(const std::vector<std::uint8_t> &data) {
	if (sockfd_ < 0) {
		return false;
	}

	return sendto(sockfd_, data.data(), data.size(), 0, reinterpret_cast<sockaddr*>(&serverStruct_), sizeof(serverStruct_)) != -1;
}

bool UDPClient::Receive(std::vector<std::uint8_t> &data) { return false; }

void UDPClient::Destroy() {}

UDPClient::~UDPClient() {
	shutdown(sockfd_, SHUT_WR);

	close(sockfd_);
	delete[] server_address_;
}
