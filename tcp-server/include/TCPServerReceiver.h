#ifndef TCPSERVERRECEIVER_H
#define TCPSERVERRECEIVER_H

#include "config.h"
#include "motorsProcessor.h"

using namespace std;

char* get_primary_ip();

class TCPServer {
public:
    char *server_address;
    int server_portnum;

    sem_t *progSemaphore;
    MotorProcessor *commandProcessor;

    TCPServer(const char *ip_address, int port, MotorProcessor *proc, sem_t *sem) {
        server_address = new char[strlen(ip_address) + 1 ];
        strcpy(server_address, ip_address);

        commandProcessor = proc; // set for using motorProcess class inside our tcp server
        progSemaphore = sem;

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



    int get_counter();
    int counter = get_counter(); // 100 ms - timer precision . I use usleep() function
    void* serverThreadFunc();
    void* timerThreadFunc();

};

#endif 
