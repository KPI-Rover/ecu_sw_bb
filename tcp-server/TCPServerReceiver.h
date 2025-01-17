#ifndef TCPSERVERRECEIVER_H
#define TCPSERVERRECEIVER_H

#include <cstring>
#include <iostream>
#include <unistd.h>
#include <errno.h>
#include <resolv.h>
#include <pthread.h>

#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>

using namespace std;

class TCPServer {
public:
	char *server_address;
	int server_portnum;
	
	bool stopThread = false;

	TCPServer(const char *ip_address, int port) {
		server_address = new char[strlen(ip_address) + 1 ];
		strcpy(server_address, ip_address);

		server_portnum = port;
	};
	
	static void* serverThreadFuncWrapper(void *arg) {
		reinterpret_cast<TCPServer*>(arg)->serverThreadFunc();
	        return nullptr;	
		
	}

	int init();
	void start();
	void destroy();



private:
	pthread_t serverThread_id;
	int sockfd;
	//struct sockaddr_in serv_addr;

	int BUFFERSIZE = 1024;
	int NUMSLOTS = 5;
	

	void* serverThreadFunc();

};

#endif 
