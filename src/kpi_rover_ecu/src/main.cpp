#include "KPIRoverECU.h"
#include "TCPTransport.h"
#include "motorConfig.h"
#include "motorsController.h"
#include "protocolHandler.h"


atomic<bool> running_program(true);
void InterruptSignalHandler(int signal);
char* GetPrimaryApi();

int main(int argc, char* argv[]) {
    char* server_address = GetPrimaryApi();
    const int kDefaultPortNum = 5500;
    int server_portnum = kDefaultPortNum;

    // Command-line options
    int opt = 0;
    while ((opt = getopt(argc, argv, "a:p:")) != -1) {
        switch (opt) {
            case 'a':
                server_address = optarg;
                break;
            case 'p':
                server_portnum = atoi(optarg);
                break;
            default:
                std::cout << "Usage: " << argv[0] << '\n';
                std::cout << " [-a server_address] " << '\n';
                std::cout << " [-p server_portnum]" << '\n';
                return EXIT_FAILURE;
        }
    }
    // sem_init(&stopProgramSem, 0, 0);

    MotorController motors_processor;
    const uint8_t kMotorNumber = MOTORS_NUMBER;
    MotorConfig shassis_array[] = {MotorConfig(1, false), MotorConfig(2, false), MotorConfig(3, true),
                                   MotorConfig(4, true)};

    motors_processor.Init(shassis_array, kMotorNumber);
    ProtocolHanlder protocol_handler(&motors_processor);

    TCPTransport tcp_transport(server_address, server_portnum);

    if (tcp_transport.Init() == -1) {
        std::cout << "[ERROR] Error creating socket" << '\n';
        tcp_transport.Destroy();
        return 1;
    }

    std::cout << "start ..." << '\n';

    KPIRoverECU kpi_rover_ecu(&protocol_handler, &tcp_transport);

    if (!kpi_rover_ecu.Start()) {
        std::cout << "Error In intitalizing main class" << '\n';
        return 1;
    }

    if (std::signal(SIGINT, InterruptSignalHandler) == SIG_ERR) {
        std::cerr << "Error: Unable to set signal handler for SIGINT" << '\n';
        return EXIT_FAILURE;
    }

    if (std::signal(SIGTERM, InterruptSignalHandler) == SIG_ERR) {
        std::cerr << "Error: Unable to set signal handler for SIGTERM" << '\n';
        return EXIT_FAILURE;
    }

    while (running_program) {}

    kpi_rover_ecu.Stop();
    motors_processor.Destroy();

    return 0;
}

char* GetPrimaryApi() {
    /*
         Automatically finding IP address for hosting server
        returns string of IP-address
     */
    struct ifaddrs *ifaddr, *ifa;
    int family = 0, status = 0;
    char* host = new char[NI_MAXHOST];

    if (getifaddrs(&ifaddr) == -1) {
        perror("[ERROR] getifaddrs");
        std::exit(EXIT_FAILURE);
    }

    for (ifa = ifaddr; ifa != nullptr; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == nullptr) {
            continue;
        }

        family = ifa->ifa_addr->sa_family;

        if (family == AF_INET) {  // Check it is IPv4
            status = getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in), host, NI_MAXHOST, nullptr, 0, NI_NUMERICHOST);
            if (status != 0) {
                std::cerr << "[ERROR] failed: " << gai_strerror(status) << '\n';
                std::exit(EXIT_FAILURE);
            }
            // Ignore loopback address
            if (std::strcmp(ifa->ifa_name, "lo") != 0) {
                break;
            }
        }
    }

    freeifaddrs(ifaddr);
    return host;
}

void InterruptSignalHandler(int signal) { running_program = false; }
