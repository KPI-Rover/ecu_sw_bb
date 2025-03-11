//#include "TCPServerReceiver.h"
#include "TCPTransport.h"
#include "config.h"
#include "motorsController.h"
#include "protocolHandler.h"
#include "motorConfig.h"
using namespace std;


//sem_t stopProgramSem; /// global value of semaphor that is ckecked to stop rpogrma

atomic<bool> runningProgram(false);
atomic<bool> runningState(false);
atomic<int> counter(0);

void interruptSignalHandler(int signal); 
int get_counter();

void timerThreadFuction(ProtocolHanlder *workClass);

char* get_primary_ip();


int main(int argc, char* argv[]) {
    char *server_address = get_primary_ip();
    int server_portnum = 5500;

    // Command-line options
    int opt;
    while ((opt = getopt(argc, argv, "a:p:")) != -1) {
        switch (opt) {
            case 'a':
                server_address = optarg;
                break;
            case 'p':
                server_portnum = atoi(optarg);
                break;
            default:
                cerr << "Usage: " << argv[0] << endl;
                   cerr << " [-a server_address] " << endl; 
                cerr << " [-p server_portnum]" << endl;
            return EXIT_FAILURE;
        }
    }
    //sem_init(&stopProgramSem, 0, 0);
    


    MotorController motors_processor;
    MotorConfig shassis_array[4] = {
        MotorConfig(1, false),
        MotorConfig(2, false),
        MotorConfig(3, true),
        MotorConfig(4, true)
    };

    motors_processor.init(shassis_array);
    ProtocolHanlder protocolHandler_(&motors_processor);
    counter.store(get_counter());
    thread timerThread(timerThreadFuction, &protocolHandler_);

    TCPTransport TcpTransport_(server_address, server_portnum);

    if (TcpTransport_.init() == -1) {
        cout << "[ERROR] Error creating socket" << endl;
        TcpTransport_.destroy();
        return 1;
    }

    cout << "start ..." << endl;

    signal(SIGINT, interruptSignalHandler); // initializing of custom signal handlers
    signal(SIGTERM, interruptSignalHandler); // initializing of custom signal handlers

    cout << "start to boot socket server" << endl;
    TcpTransport_.start();

    cout << "starting main cycle" << endl;

    while (true) {
        if (runningProgram) {
            // TcpTransport_.destroy();
            // motors_processor.destroy();
            cout << "END of program" << endl;
            cout << "joining threads" << endl;
            runningState = true;
            if (timerThread.joinable()) {
                timerThread.join();
            }
            cout << "destroying drivers" << endl;
            motors_processor.destroy();
            TcpTransport_.destroy();
            return 0;
            //break;
        }
        vector<uint8_t> message;
        if (TcpTransport_.receive(message)) {
            counter.store(get_counter());
            vector<uint8_t> return_message = protocolHandler_.handleMessage(message);
            TcpTransport_.send(return_message);
        }
    }

    
    // sem_wait(&stopProgramSem);
    // cout << "Stopping ..." << endl;

    // server.destroy();
    // motors_processor.destroy();
    
    return 0;
}

void timerThreadFuction(ProtocolHanlder *workClass) {
    vector<uint8_t> stopVector;
    int32_t stopValue  = 0;
    stopVector.push_back(ID_SET_ALL_MOTORS_SPEED);
    for (int i = 0; i < MOTORS_NUMBER; i++) {
        stopVector.insert(stopVector.end(), 
                      reinterpret_cast<uint8_t*>(&stopValue), 
                      reinterpret_cast<uint8_t*>(&stopValue) + sizeof(int32_t));
    }

    while (!runningState) {
        if (counter > 0) {
            usleep(TIMERPRECISION);
            counter--;

            if (counter == 0 ) {
                cout << "[INFO] Motor set to stop" << endl;
            }

        } else if (counter == 0) {
            // command to stop all motors
            workClass->handleMessage(stopVector);

            sleep(TIMESTOP);
            
                
        }
            
    }


}

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

int get_counter() {
    return  (TIMESTOP * ONESECONDMICRO) / TIMERPRECISION;
}

void interruptSignalHandler(int signal) {
    
    cout << "Get signal to stop program" << endl;
    runningProgram = true;
    //sem_post(&stopProgramSem);
    //exit(0);
    
}
