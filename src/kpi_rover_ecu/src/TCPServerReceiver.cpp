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

    
    commandProcessor->init(SHASSIARR); // initializing of commandProcessor  

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

                uint8_t cmd_id = buffer[0];
                //cout << "server get command: " <<  static_cast<int>(cmd_id) << endl;
                if (cmd_id == ID_SET_MOTOR_SPEED) {
                    
                    uint8_t motor_id = buffer[1];
                    //int32_t motor_rpm;
                    
                    memcpy(&motor_rpm, buffer+2, sizeof(int32_t));
                    motor_rpm = ntohl(motor_rpm);				
                    
                    cout << "[COMMAND] motor " << static_cast<int>(motor_id) << " new rpm " << static_cast<int>(motor_rpm) << endl;
                    cout << "[INFO] Motor set run" << endl;

                    if (commandProcessor->setMotorRPM(static_cast<int>(motor_id), static_cast<int>(motor_rpm)) != 0) {
                        cout << "[ERROR] Error setMotorRPM, retry connection" << endl;
                        continue; //?
                    }
                    


                    memset(buffer, 0, BUFFERSIZE); // cleaning buffer
                    memset(buffer, ID_SET_MOTOR_SPEED, sizeof(uint8_t));
                    buffer_cursor = sizeof(uint8_t);


                } else if (cmd_id == ID_GET_API_VERSION) {
                    
                    cout << "[COMMAND] get api version " << endl;

                    memset(buffer, 0, BUFFERSIZE);
                    memset(buffer, ID_GET_API_VERSION, sizeof(uint8_t));
                    buffer_cursor = sizeof(uint8_t);
                    uint8_t response = 1;
                    memcpy(buffer+buffer_cursor, &response, sizeof(uint8_t));
                    buffer_cursor += sizeof(uint8_t);



                } else if (cmd_id == ID_SET_ALL_MOTORS_SPEED) {

                    for (int i = MOTOR_ID_START; i < MOTOR_ID_START + MOTORS_NUMBER; i++ ) {
                        memcpy(motors_rpm_arr+i , buffer+1+i * sizeof(int32_t), sizeof(int32_t));
                        motors_rpm_arr[i] = ntohl(motors_rpm_arr[i]);

                    }

                    for (int i = MOTOR_ID_START; i < MOTOR_ID_START + MOTORS_NUMBER; i++ ) {
                        cout << "[COMMAND] motor " << i << " new rpm " << static_cast<int>(motors_rpm_arr[i]) << " " << endl;
                        if (commandProcessor->setMotorRPM(i, static_cast<int>(motors_rpm_arr[i])) != 0) {
                            cout << "[ERROR] Error setMotorRPM, retry connection" << endl;
                            continue; //? or break
                        }
                        
                    }

                    cout << endl;
                    cout << "[INFO] Motor set run" << endl;

                    memset(buffer, 0, BUFFERSIZE); // cleaning buffer
                    memset(buffer, ID_SET_ALL_MOTORS_SPEED, sizeof(uint8_t));
                    buffer_cursor = sizeof(uint8_t);


                } else if (cmd_id == ID_GET_ENCODER) {
                    
                    uint8_t motor_id = buffer[1];
                    cout << "[COMMAND] get motor " << static_cast<int>(motor_id) << " encoder " << endl;
                    //int32_t motor_rpm;

                    motor_rpm = commandProcessor->getMotorRPM(motor_id);

                    memset(buffer, 0, BUFFERSIZE);
                    memset(buffer, ID_GET_ENCODER, sizeof(uint8_t));
                    buffer_cursor = sizeof(uint8_t);

                    int32_t motor_rpm_order  = htonl(motor_rpm);
                    memcpy(buffer + buffer_cursor, &motor_rpm_order, sizeof(int32_t));
                    buffer_cursor += sizeof(int32_t);


                    
                } else if (cmd_id == ID_GET_ALL_ENCODERS) {
                    cout << "[COMMAND] get all encoders " << endl;
                    
                    memset(buffer, 0, BUFFERSIZE);
                    memset(buffer, ID_GET_ALL_ENCODERS, sizeof(uint8_t));
                    buffer_cursor = sizeof(uint8_t);

                    for (int i = MOTOR_ID_START; i < MOTOR_ID_START + MOTORS_NUMBER; i++ ) {
                        int32_t encoder_RPM = htonl(commandProcessor->getMotorRPM(i));
                        memcpy(buffer + buffer_cursor, &encoder_RPM, sizeof(int32_t));
                        buffer_cursor += sizeof(int32_t);
                    }
                    
                }

                /* Sending response */
                if (buffer_cursor != 0) {
                    if (send(client_sockfd, buffer, buffer_cursor, 0) < 0) {
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

    while (1) {
        if (counter > 0) {
            usleep(TIMERPRECISION);
            pthread_mutex_lock(&timer_mutex); // locking mutex to decrease tier value
            counter--;
            pthread_mutex_unlock(&timer_mutex); // unlocking mutex for increasing timer value 
            if (counter == 0 ) {
                cout << "[INFO] Motor set to stop" << endl;
            }

        } else if (counter == 0) {
            // command to stop all motors
            commandProcessor->stopMotor(0);
            commandProcessor->stopMotor(1);
            commandProcessor->stopMotor(2);
            commandProcessor->stopMotor(3);
            //usleep(1000000); // just every second reminder  
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

    commandProcessor->destroy();
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
