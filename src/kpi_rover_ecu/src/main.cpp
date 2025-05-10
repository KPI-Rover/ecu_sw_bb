#include <getopt.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <chrono>
#include <csignal>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include "IMUController.h"
#include "KPIRoverECU.h"
#include "TCPTransport.h"
#include "UDPClient.h"
#include "motorConfig.h"
#include "motorsController.h"
#include "protocolHandler.h"

using std::atomic;
using std::signal;
using std::string;
using std::vector;

atomic<bool> running_program(true);
void InterruptSignalHandler(int signal);

int main(int argc, char* argv[]) {
    const char* server_address = "0.0.0.0";
    const int kDefaultPortNum = 5500;
    const int kBase = 10;  // Named constant for base 10
    int server_portnum = kDefaultPortNum;

    // Command-line options
    int opt = 0;
    while ((opt = getopt(argc, argv, "a:p:")) != -1) {
        switch (opt) {
            case 'a':
                server_address = optarg;
                break;
            case 'p':
                server_portnum = strtol(optarg, nullptr, kBase);
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
    const uint8_t kMotorNumber = 4;
    const std::vector<MotorConfig> kShassisVector = {
        MotorConfig(3, false, {1.5, 0.056, 1.5}),
        MotorConfig(4, false, {1.5, 0.056, 1.5}),
        MotorConfig(1, true, {1.5, 0.056, 1.5}),
        MotorConfig(2, true, {1.5, 0.056, 1.5}),
    };

    motors_processor.Init(kShassisVector, kMotorNumber);
    ProtocolHanlder protocol_handler(&motors_processor);

    IMUController imu_controller;

    if (imu_controller.Init() == -1) {
        std::cout << "[ERROR] Error initializing IMU controller" << '\n';
        imu_controller.Stop();
        return 1;
    }

    TCPTransport tcp_transport(server_address, server_portnum);
    UDPClient udp_client;

    if (tcp_transport.Init() == -1) {
        std::cout << "[ERROR] Error creating socket" << '\n';
        tcp_transport.Destroy();
        udp_client.Destroy();
        return 1;
    }

    std::cout << "start ..." << '\n';

    KPIRoverECU kpi_rover_ecu(&protocol_handler, &tcp_transport, &udp_client, &imu_controller);

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

    while (running_program) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    kpi_rover_ecu.Stop();
    motors_processor.Destroy();

    return 0;
}

void InterruptSignalHandler(int signal) { running_program = false; }
