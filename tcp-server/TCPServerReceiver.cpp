#include "TCPServerReceiver.h"



int TCPServer::init() {
	//int sockfd, client_sockfd;
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

	cout << "Started server on " << server_address << ":" << server_portnum << endl;
	return sockfd;

}

void* TCPServer::serverThreadFunc() {
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, nullptr);	
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, nullptr);		


	//int *main_sockfd = (int *) parameters;

	int n_recved, client_sockfd;
	char buffer[BUFFERSIZE];
	
	

	while (1) {
		client_sockfd = accept(sockfd, nullptr, nullptr);

		if (client_sockfd == -1) {
			perror("Error: accept()");
			break;
		}
		while (1) {
			n_recved = recv(client_sockfd, buffer, BUFFERSIZE, 0);
			if (n_recved > 0) {
				cout << "server get message: " << buffer << endl;	
				/*
				 Processing data, calling processor functions


				 */
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

void TCPServer::start() {
	
	if (pthread_create(&serverThread_id, nullptr, &serverThreadFuncWrapper, this) != 0){
		perror("Error: pthread_create()");
		exit(EXIT_FAILURE);
	}
	


}


void TCPServer::destroy() { 
	if (pthread_cancel(serverThread_id) != 0) {
		perror("Error: pthread_cancel()");
		exit(EXIT_FAILURE);
	}
	delete[] server_address;
	pthread_join(serverThread_id, nullptr);
	close(sockfd);
}