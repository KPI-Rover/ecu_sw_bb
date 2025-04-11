#ifndef UDPCLIENT_H
#define UDPCLIENT_H

#include <cstdint>
#include <vector>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "ITransport.h"

class UDPClient : public ITransport {
	public:
	 UDPClient(const char* ip_address, int port);
		
	 UDPClient(const UDPClient&) = delete;
	 UDPClient& operator=(const UDPClient&) = delete; 
	 UDPClient(UDPClient&&) = delete;             
	 UDPClient& operator=(UDPClient&&) = delete;

	 int Init() override ;
	 bool Send(const std::vector<std::uint8_t> &data) override ;
	 bool Receive(std::vector<std::uint8_t>& data) override ;
     void Start() override ;
     void Destroy() override ;
	 ~UDPClient() override ;

	private:
	 int sockfd_;
	 sockaddr_in serverStruct_;
	 char* server_address_;
	 int client_portnum_;
};

#endif
