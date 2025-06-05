#include <getopt.h>
#include <glog/logging.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>

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
#include "loggingIncludes.h"
#include "motorConfig.h"
#include "motorsController.h"
#include "protocolHandler.h"

using std::atomic;
using std::signal;
using std::string;
using std::vector;

atomic<bool> running_program(true);
void InterruptSignalHandler(int signal);
bool checkDirectory(const std::string& path);
bool createDirectory(const std::string& path);

int main(int argc, char* argv[]) {
    const char* server_address = "0.0.0.0";
    const int kDefaultPortNum = 5500;
    const int kBase = 10;  // Named constant for base 10
    int server_portnum = kDefaultPortNum;
    int log_level = 1;
    std::string logging_directory = "./log";

    // Command-line options
    int opt = 0;
    while ((opt = getopt(argc, argv, "a:p:l:o:")) != -1) {
        switch (opt) {
            case 'a':
                server_address = optarg;
                break;
            case 'p':
                server_portnum = strtol(optarg, nullptr, kBase);
                break;
            case 'l':
                log_level = strtol(optarg, nullptr, kBase);
                break;
            case 'o':
                logging_directory = optarg;
                break;
            default:
                std::cout << "Usage: " << argv[0];
                std::cout << " [-a server_address] ";
                std::cout << " [-p server_portnum]";
                std::cout << " [-l log level]";
                std::cout << " [-o output direcotry. Example \"./log\" ]";
                return EXIT_FAILURE;
        }
    }
    // sem_init(&stopProgramSem, 0, 0);
    /* Glog initializing */

    google::InitGoogleLogging(argv[0]);
    if (log_level == 0) {
        FLAGS_stderrthreshold = 0;
        FLAGS_v = 1;
    } else if (log_level >= 1 && log_level <= 4) {
        FLAGS_stderrthreshold = log_level - 1;
    } else {
        FLAGS_stderrthreshold = 0;
    }

    if (!checkDirectory(logging_directory)) {
        if (!createDirectory(logging_directory)) {
            std::cout << "Failed to create directory: " << logging_directory << '\n';
            return EXIT_FAILURE;
        }
    }

    FLAGS_log_dir = logging_directory;
    LOG_INFO << "Logger was set up." << "Directory to log: " << FLAGS_log_dir;

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
        LOG_ERROR << "Error initializing IMU controller";
        imu_controller.Stop();
        return 1;
    }

    TCPTransport tcp_transport(server_address, server_portnum);
    UDPClient udp_client;

    if (tcp_transport.Init() == -1) {
        LOG_ERROR << "Error creating socket";
        tcp_transport.Destroy();
        udp_client.Destroy();
        return 1;
    }

    LOG_INFO << "start ...";

    KPIRoverECU kpi_rover_ecu(&protocol_handler, &tcp_transport, &udp_client, &imu_controller);

    if (!kpi_rover_ecu.Start()) {
        LOG_ERROR << "Error In intitalizing main class";
        return 1;
    }

    if (std::signal(SIGINT, InterruptSignalHandler) == SIG_ERR) {
        LOG_ERROR << "Unable to set signal handler for SIGINT";
        return EXIT_FAILURE;
    }

    if (std::signal(SIGTERM, InterruptSignalHandler) == SIG_ERR) {
        LOG_ERROR << "Unable to set signal handler for SIGTERM";
        return EXIT_FAILURE;
    }

    while (running_program) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    kpi_rover_ecu.Stop();
    motors_processor.Destroy();

    return 0;
}

bool checkDirectory(const std::string& path) {
    struct stat info;
    return stat(path.c_str(), &info) == 0 && (info.st_mode & S_IFDIR);
}

bool createDirectory(const std::string& path) {
    mode_t mode = 0755;
    int ret = mkdir(path.c_str(), mode);
    if (ret == 0 || errno == EEXIST) {
        return true;
    } else {
        std::cerr << "mkdir error: " << std::strerror(errno) << '\n';
        return false;
    }
}

void InterruptSignalHandler(int signal) { running_program = false; }
