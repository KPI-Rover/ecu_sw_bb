#ifndef TCPTRANSPORT_H
#define TCPTRANSPORT_H

#include "config.h"
#include "ITransport.h"

class TCPTransport : public ITransport {
public:
	

	TCPTransport(const char *ip_address, int port);
	bool send(const vector<uint8_t>& data) override;
    bool receive(vector<uint8_t>& data) override;
	void start() override;
    int init() override;
    void destroy() override;

private:
	int sockfd, client_sockfd;
	char *server_address;
	int server_portnum;
	atomic<bool> running;
	thread acceptThread;

};

#endif