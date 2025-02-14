#include "TCPServerReceiver.h"
#include "config.h"
#include "motorsProcessor.h"

char* get_primary_ip() {
	/*
	 	Automatically finding IP adress for hosting server
		returns string of IP-address
	 */
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
		/*
			Fucntions responsible for initialising new socket (TCP socket)
		 */
		

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

		commandProcessor->init(SHASSIARR); // initializing of commandProcessor  

		return sockfd;

}

int TCPServer::get_counter() {
	return  (TIMESTOP * 1000000) / TIMERPRECISION;
}

void* TCPServer::serverThreadFunc() {
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, nullptr);	
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, nullptr);		


	//int *main_sockfd = (int *) parameters;

	int n_recved, client_sockfd;
	char buffer[BUFFERSIZE];
	
	

	while (1) {
		client_sockfd = accept(sockfd, nullptr, nullptr); // accept new connection

		if (client_sockfd == -1) {
			perror("Error: accept()");
			continue;
		}
		while (1) {
			memset(buffer, 0, BUFFERSIZE);
			/* receiving reauests */
			n_recved = recv(client_sockfd, buffer, BUFFERSIZE, 0); // receiving data from connestion
			if (n_recved > 0) {
				/* locking mutex for updating timer */
				pthread_mutex_lock(&timer_mutex);
				counter = get_counter();
				
				pthread_mutex_unlock(&timer_mutex);
				/* unlocking mutex above */

				uint8_t cmd_id = buffer[0];
				cout << "server get command: " <<  static_cast<int>(cmd_id) << endl;
				if (cmd_id == ID_SET_MOTOR_SPEED) {
					uint8_t motor_id = buffer[1];
					int32_t motor_rpm;
					
					memcpy(&motor_rpm, buffer+2, sizeof(int32_t));
					motor_rpm = ntohl(motor_rpm);				
					
					cout << "motor to do " << static_cast<int>(motor_id) << endl;
					cout << "motor new rpm " << static_cast<int>(motor_rpm) << endl;



					memset(buffer, 0, BUFFERSIZE); // cleaning buffer
					memset(buffer, ID_SET_MOTOR_SPEED, sizeof(uint8_t));

					if (send(client_sockfd, buffer, 8, 0) < 0) {
						// sending response
						perror("send()");
						break; 
					}

				} else if (cmd_id == ID_GET_API_VERSION) {
					cout << "command get_api_version isn't ready yet" << endl;

				} else if (cmd_id == ID_SET_ALL_MOTORS_SPEED) {
					//cout << "command set_all_motors_speed is not ready yet" << endl;
					int32_t motors_rpm_arr[4];

					for (int i = 0; i < 4; i++) {
						memcpy(motors_rpm_arr+i , buffer+1+i * sizeof(int32_t), sizeof(int32_t));
						motors_rpm_arr[i] = ntohl(motors_rpm_arr[i]);

					}

					for (int i = 0; i < 4; i++) {
						cout << "motor " << i << " new rpm " << static_cast<int>(motors_rpm_arr[i]) << endl;
					}

					memset(buffer, 0, BUFFERSIZE); // cleaning buffer
					memset(buffer, ID_SET_ALL_MOTORS_SPEED, sizeof(uint8_t));

					if (send(client_sockfd, buffer, 8, 0) < 0) {
						// sending response
						perror("send()");
						break; 
					}

				} else if (cmd_id == ID_GET_ENCODER) {
					cout << "command get encoder is not ready yet" << endl;
					
					
				} else if (cmd_id == ID_GET_ENCODER) {
					cout << "command get encoder is not ready yet" << endl;
				} else {
					cout << "cannot recognize command" << endl;

				}


				/* Sending response */
				


				// temporary if for ending of conneciton
				if (strcmp(buffer, "end") == 0) {
					stopThread = true;			
				}

			} else if (n_recved <= 0) {
				perror("recv()");
				break;
			}
		}
		close(client_sockfd);
		cout << "closing connetion " << endl;
	}

	return NULL;
}




void* TCPServer::timerThreadFunc() {
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, nullptr);	
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, nullptr);

	/*
		THread functions responsible for timer 1 s to stop motorrs
		NEEED REFACTORING, (I have no time for this)
	 */

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
	/* 
	Functions responsible for starting threads for timer and listening for new messages
	 */
	if (pthread_create(&serverThread_id, nullptr, &serverThreadFuncWrapper, this) != 0){ // thread for listening new messages
		perror("Error: pthread_create()");
		exit(EXIT_FAILURE);
	}
		
	if (pthread_create(&timerThread_id, nullptr, &timerThreadFuncWrapper, this) != 0){ // thread for timer
		perror("Error: pthread_create()");
		exit(EXIT_FAILURE);
	}
	

}


void TCPServer::destroy() { 
	/* 
	function  responsible for cleaning data and joining threads
	 */
	if (pthread_cancel(serverThread_id) != 0) { // canceling listening thread
		perror("Error: pthread_cancel()");
		exit(EXIT_FAILURE);
	}
	
	if (pthread_cancel(timerThread_id) != 0) { // cancleing timer thread
		perror("Error: pthread_cancel()");
		exit(EXIT_FAILURE);
	}
	
	commandProcessor->destroy();

	delete[] server_address; // cleaning data from heap
	pthread_join(serverThread_id, nullptr); // joining threads
	pthread_join(timerThread_id, nullptr);
	close(sockfd); // closing socket
}
