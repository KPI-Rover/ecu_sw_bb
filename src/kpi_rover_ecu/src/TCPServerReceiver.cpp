#include "TCPServerReceiver.h"
#include "config.h"
#include "motorsController.h"

char* get_primary_ip() {
    /*
         Automatically finding IP adress for hosting server
        returns string of IP-address
     */
    struct ifaddrs *ifaddr, *ifa;
    int family, s;
    char *host = new char[NI_MAXHOST];

    if (getifaddrs(&ifaddr) == -1) {
        perror("[ERROR] getifaddrs");
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
                printf("[ERROR] failed: %s\n", gai_strerror(s));
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

TCPServer::TCPServer(const char *ip_address, int port, ProtocolHanlder *pHandler, sem_t *sem) {
    server_address = new char[strlen(ip_address) + 1 ];
    strcpy(server_address, ip_address);

    protocolHandler_ = pHandler;
    progSemaphore = sem;

    server_portnum = port;
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

int TCPServer::get_counter() {
    return  (TIMESTOP * 1000000) / TIMERPRECISION;
}

void* TCPServer::serverThreadFunc() {
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, nullptr);	
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, nullptr);		

    int n_recved, client_sockfd;
    char buffer[BUFFERSIZE];
    size_t buffer_cursor;
    int32_t motors_rpm_arr[4];
    int32_t motor_rpm;
    

    while (1) {
        client_sockfd = accept(sockfd, nullptr, nullptr); // accept new connection

        if (client_sockfd == -1) {
            perror("[ERROR] accept()");
            continue;
        }
        while (1) {
            /* set up start conditions for new message */
            memset(buffer, 0, BUFFERSIZE);
            memset(motors_rpm_arr, 0, 4 * sizeof(int32_t));
            buffer_cursor = 0;
            motor_rpm = 0;

            /* receiving reauests */
            n_recved = recv(client_sockfd, buffer, BUFFERSIZE, 0); // receiving data from connestion
            if (n_recved > 0) {
                /* locking mutex for updating timer */
                pthread_mutex_lock(&timer_mutex);
                counter = get_counter();
                
                pthread_mutex_unlock(&timer_mutex);
                /* unlocking mutex above */
                vector<uint8_t> vector_buffer(buffer, buffer+n_recved);

                vector<uint8_t> return_message = protocolHandler_->handleMessage(vector_buffer);

                if (!return_message.empty()) {
                    if (send(client_sockfd, return_message.data(), return_message.size(), 0) < 0) {
                        // sending response
                        perror("[ERROR] send()");
                        break; 
                    }
                }
                
            } else if (n_recved <= 0) {
                
                if (n_recved < 0) {
                    perror("[ERROR] recv()");
                }
                break;
            }
        }
        close(client_sockfd);

        cout << "[INFO] closing connetion " << endl;
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

     /* Creating vector to all set mototrs to 0 */
    vector<uint8_t> stopVector;
    int32_t stopValue  = 0;
    stopVector.push_back(ID_SET_ALL_MOTORS_SPEED);
    for (int i = 0; i < MOTORS_NUMBER; i++) {
        stopVector.insert(stopVector.end(), 
                      reinterpret_cast<uint8_t*>(&stopValue), 
                      reinterpret_cast<uint8_t*>(&stopValue) + sizeof(int32_t));
    }

    while (1) {
        if (counter > 0) {
            usleep(TIMERPRECISION);
            pthread_mutex_lock(&timer_mutex); // locking mutex to decrease tier value
            counter--;
            cout << "counter " << counter << endl;
            pthread_mutex_unlock(&timer_mutex); // unlocking mutex for increasing timer value 
            if (counter == 0 ) {
                cout << "[INFO] Motor set to stop" << endl;
            }

        } else if (counter == 0) {
            // command to stop all motors
            protocolHandler_->handleMessage(stopVector);

            sleep(TIMESTOP);
            
                
        }
            
    }
    

    return NULL;	
}

int TCPServer::start() {
    /* 
    Functions responsible for starting threads for timer and listening for new messages
     */
    if (pthread_create(&serverThread_id, nullptr, &serverThreadFuncWrapper, this) != 0){ // thread for listening new messages
        perror("[ERROR] rx/tx pthread_create()");
        return -1;
    }
        
    if (pthread_create(&timerThread_id, nullptr, &timerThreadFuncWrapper, this) != 0){ // thread for timer
        perror("[ERROR] timer pthread_create()");
        return -1;
    }
    
    return 0;

}


void TCPServer::destroy() { 
    /* 
    function  responsible for cleaning data and joining threads
     */

    
    if (pthread_cancel(serverThread_id) != 0) { // canceling listening thread
        perror("[ERROR] pthread_cancel()");
        exit(EXIT_FAILURE);
    }
    
    if (pthread_cancel(timerThread_id) != 0) { // cancleing timer thread
        perror("[ERROR] pthread_cancel()");
        exit(EXIT_FAILURE);
    }
    
    //sem_post(progSemaphore);
    delete[] server_address; // cleaning data from heap
    pthread_join(serverThread_id, nullptr); // joining threads
    pthread_join(timerThread_id, nullptr);
    close(sockfd); // closing socket
}
