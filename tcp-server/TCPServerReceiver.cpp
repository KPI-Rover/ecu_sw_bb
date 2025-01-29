#include "TCPServerReceiver.h"

char* get_primary_ip() {
        struct ifaddrs *ifaddr, *ifa;
        int family, s;
        char *host = new char[NI_MAXHOST];

        if (getifaddrs(&ifaddr) == -1) {
                perror("getifaddrs");
                exit(EXIT_FAILURE);
        }

        for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
                if (ifa->ifa_addr == NULL)
                        continue;

                family = ifa->ifa_addr->sa_family;

                if (family == AF_INET) { // Check it is IPv4
                        s = getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in),
                                        host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
                        if (s != 0) {
                                printf("getnameinfo() failed: %s\n", gai_strerror(s));
                                exit(EXIT_FAILURE);
                        }
                        // Ignore loopback address
                        if (strcmp(ifa->ifa_name, "lo") != 0) {
                                break;
                        }
                }
        }

        freeifaddrs(ifaddr);
        return host;
}


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

int TCPServer::get_counter() {
	return  (timeStop * 1000000) / timerPrecision;
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
			continue;
		}
		while (1) {
			n_recved = recv(client_sockfd, buffer, BUFFERSIZE, 0);
			if (n_recved > 0) {
				cout << "server get message: " << buffer << endl;
				pthread_mutex_lock(&timer_mutex);
				counter = get_counter();
				
				pthread_mutex_unlock(&timer_mutex);
				/*
				 Processing data, calling processor functions


				 */
				if (strcmp(buffer, "end") == 0) {
					stopThread = true;			
				}

			} else if (n_recved < 0) {
				perror("recv()");
				continue;
			}
		}
		close(client_sockfd);
	}

	return NULL;
}




void* TCPServer::timerThreadFunc() {
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, nullptr);	
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, nullptr);

	while (1) {
		if (counter > 0) {
			usleep(100000);
			pthread_mutex_lock(&timer_mutex);
			counter--;

			pthread_mutex_unlock(&timer_mutex);
		} else if (counter == 0) {
			// command to stop all motors
			usleep(1000000); // just every second reminder  
			cout << "MOTORS STOP!!" << endl;
				
		}
			
	}
	

	return NULL;	
}



void TCPServer::start() {

	if (pthread_create(&serverThread_id, nullptr, &serverThreadFuncWrapper, this) != 0){
		perror("Error: pthread_create()");
		exit(EXIT_FAILURE);
	}
		
	if (pthread_create(&timerThread_id, nullptr, &timerThreadFuncWrapper, this) != 0){
		perror("Error: pthread_create()");
		exit(EXIT_FAILURE);
	}
	

}


void TCPServer::destroy() { 
	if (pthread_cancel(serverThread_id) != 0) {
		perror("Error: pthread_cancel()");
		exit(EXIT_FAILURE);
	}
	
	if (pthread_cancel(timerThread_id) != 0) {
		perror("Error: pthread_cancel()");
		exit(EXIT_FAILURE);
	}
	
	delete[] server_address;
	pthread_join(serverThread_id, nullptr);
	pthread_join(timerThread_id, nullptr);
	close(sockfd);
}
