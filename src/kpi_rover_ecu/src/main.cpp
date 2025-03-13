//#include "TCPServerReceiver.h"
#include "TCPTransport.h"
#include "config.h"
#include "motorsController.h"
#include "protocolHandler.h"
#include "motorConfig.h"
#include "KPIRoverECU.h"
using namespace std;



atomic<bool> runningProgram(true);
void interruptSignalHandler(int signal); 
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
    uint8_t motorNumber = 4;
    MotorConfig shassis_array[] = {
        MotorConfig(1, false),
        MotorConfig(2, false),
        MotorConfig(3, true),
        MotorConfig(4, true)
    };

    motors_processor.init(shassis_array, motorNumber);
    ProtocolHanlder protocolHandler_(&motors_processor);

    TCPTransport TcpTransport_(server_address, server_portnum);

    if (TcpTransport_.init() == -1) {
        cout << "[ERROR] Error creating socket" << endl;
        TcpTransport_.destroy();
        return 1;
    }

    cout << "start ..." << endl;

    

    KPIRoverECU kpiRoverECU(&protocolHandler_, &TcpTransport_);

    if (!kpiRoverECU.start() ) {
        cout << "Error In intitalizing main class" << endl;
        return 1;
    }

    signal(SIGINT, interruptSignalHandler); // initializing of custom signal handlers
    signal(SIGTERM, interruptSignalHandler); // initializing of custom signal handlers

    while(runningProgram) {}

    kpiRoverECU.stop();
    motors_processor.destroy();
    
    return 0;
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

void interruptSignalHandler(int signal) {

    runningProgram = true;

}


