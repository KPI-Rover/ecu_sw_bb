#ifndef TCPSERVERRECEIVER_H
#define TCPSERVERRECEIVER_H

#include <cstring>
#include <iostream>
#include <unistd.h>
#include <errno.h>
#include <resolv.h>
#include <pthread.h>
#include <csignal>

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
	
	static void* timerThreadFuncWrapper(void *arg) {
		reinterpret_cast<TCPServer*>(arg)->timerThreadFunc();
	        return nullptr;	
	}

	int init();
	void start();
	void destroy();
		


private:
	pthread_mutex_t timer_mutex = PTHREAD_MUTEX_INITIALIZER;
	pthread_t serverThread_id, timerThread_id;
	int sockfd;
	const int BUFFERSIZE = 1024;
	const int NUMSLOTS = 5;
	const int timeStop = 1; // in seconds
	const int timerPrecision = 100000; // 100 miliseconds in microseconds	


	int get_counter();
	int counter = get_counter(); // 100 ms - timer precision . I use usleep() function
	void* serverThreadFunc();
	void* timerThreadFunc();

};

#endif 
