#include "TCPTransport.h"
#include "ITransport.h"
#include "config.h"
#include "messageQueue.h"

TCPTransport::TCPTransport(const char *ip_address, int port) {
	server_address = new char[strlen(ip_address) + 1 ];
    strcpy(server_address, ip_address);
	server_portnum = port;

	running = true;
}

int TCPTransport::init() {
	/*
        Fucntions responsible for initialising new socket (TCP socket)
    */
    
    struct sockaddr_in serv_addr;
    int reuseaddr = 1;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) {
        perror("[ERROR] Socket()");
        return -1;
    }

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, 
                (void *) &reuseaddr, (socklen_t) sizeof(int)) != 0) {
        perror("[ERROR] setsockopt()");
        return -1;
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(server_address);
    serv_addr.sin_port = htons(server_portnum);
    
    
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) != 0) {
        perror("[ERROR] bind()");
        return -1;
        
    }

    if (listen(sockfd, NUMSLOTS) != 0) {
        perror("[ERROR] listen");
        return -1;
    }

    cout << "[INFO] Started server on " << server_address << ":" << server_portnum << endl;


    return sockfd;
}

void TCPTransport::start() {
	acceptThread = thread([this]() {
		while (running) {
			cout << "Waiting for connection..." << endl;
			client_sockfd = accept(sockfd, nullptr, nullptr);
			if (client_sockfd >= 0) {
				cout << "Client connected." << endl;
				while (1) {
					uint8_t buffer[1024];
					ssize_t bytes_received = recv(client_sockfd, buffer, sizeof(buffer), 0);
					if (bytes_received > 0) {
						vector<uint8_t> message(buffer, buffer+bytes_received);
						messageQueue_.push(message);
					} else {
						cout << "Client disconnected." << endl;
						close(client_sockfd);
						client_sockfd = -1;
						break;
					}

				}
			}
		}
	});
}

bool TCPTransport::send(const vector<uint8_t>& data) {
	if (client_sockfd < 0) return false;
	return ::send(client_sockfd, data.data(), data.size(), 0) != -1;
}

bool TCPTransport::receive(vector<uint8_t>& data) {
	return messageQueue_.pop(data, 50);

}

void TCPTransport::destroy() {
	running = false;
	shutdown(sockfd, SHUT_RDWR);
	cout << "joining thread ... " << endl;
	acceptThread.join();
	cout << "closing socket" << endl;
	close(client_sockfd);
	close(sockfd);
}