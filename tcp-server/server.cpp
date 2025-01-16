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

#define BUFFERSIZE 1024
#define NUMSLOTS 5

pthread_t serverThread_id;
bool stopThread = false;



int init(char *server_address, int server_portnum) {
	int sockfd, client_sockfd;
	struct sockaddr_in serv_addr;
	int reuseaddr = 1;

	//cout << "Hello , start of tcp server" << endl;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd < 0) {
		perror("Error: Socket()");
		return -1;
	}

	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, 
				(void *) &reuseaddr, (socklen_t) sizeof(int)) != 0) {
		perror("setsockopt()");
		return -1;
	}

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(server_address);
       	serv_addr.sin_port = htons(server_portnum);
	
	
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) != 0) {
                 perror("bind()");
                 return -1;
         
	}

	if (listen(sockfd, NUMSLOTS) != 0) {
		perror("listen");
		return -1;
	}
	return sockfd;

}

void* serverThreadFunc(void* parameters) {
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, nullptr);	
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, nullptr);		


	int *sockfd = (int *) parameters;

	int n_recved, client_sockfd;
	char buffer[BUFFERSIZE];
	
	

	while (1) {
		client_sockfd = accept(*sockfd, nullptr, nullptr);

		if (client_sockfd == -1) {
			perror("Error: accept()");
			break;
		}
		while (1) {
			n_recved = recv(client_sockfd, buffer, BUFFERSIZE, 0);
			if (n_recved > 0) {
				cout << "server get message: " << buffer << endl;	
				// processing data
				if (strcmp(buffer, "end") == 0) {
					stopThread = true;			
				}

			} else if (n_recved < 0) {
				perror("recv()");
				break;
			}
		}
		close(client_sockfd);
	}

	return NULL;
}

void start(int *sockfd ) {
	
	if (pthread_create(&serverThread_id, nullptr, &serverThreadFunc, (void *) sockfd) != 0) {
		perror("Error: pthread_create()");
		exit(EXIT_FAILURE);
	}
	

	/*
	int n_recved, client_sockfd;
	char buffer[BUFFERSIZE];

	while (1) {
		client_sockfd = accept(sockfd, nullptr, nullptr);

		if (client_sockfd == -1) {
			perror("accept()");
			continue;
		}
		while (1) {
			n_recved = recv(client_sockfd, buffer, BUFFERSIZE, 0);
			if (n_recved > 0) {
				cout << "server get message: " << buffer << endl;	
				// processing data


			} else if (n_recved < 0) {
				perror("recv()");
				break;
			}
		}
		close(client_sockfd);
	}
	*/

}


void closeSocket(int sockfd) { 
	if (pthread_cancel(serverThread_id) != 0) {
		perror("Error: pthread_cancel()");
		exit(EXIT_FAILURE);
	}

	pthread_join(serverThread_id, nullptr);
	close(sockfd);
}

int main() {
	char *server_address = "192.168.1.14";
	int server_portnum = 5500;
	int sockfd = init(server_address, server_portnum);	
		

	if (sockfd <= -1) {
		cout << "Cannot create server" << endl;
		return 1;
	} else {
		cout << "started tcp server on " << server_address << ":" << server_portnum << endl;
				
	}		
	start(&sockfd);
	
	while (!stopThread) {}

	closeSocket(sockfd);
	return 0;
}
