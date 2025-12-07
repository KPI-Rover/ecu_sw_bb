#include <getopt.h>
#include <glog/logging.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <atomic>
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
    float coef_p = 0;
    float coef_i = 0;
    float coef_d = 0;
    bool has_coef_p = false;
    bool has_coef_i = false;
    bool has_coef_d = false;
    std::string logging_directory = "./log";

    // Command-line options
    int opt = 0;
    while ((opt = getopt(argc, argv, "a:p:l:o:q:w:e:")) != -1) {
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
            case 'q':
                coef_p = strtof(optarg, nullptr);
                has_coef_p = true;
                break;
            case 'w':
                coef_i = strtof(optarg, nullptr);
                has_coef_i = true;
                break;
            case 'e':
                coef_d = strtof(optarg, nullptr);
                has_coef_d = true;
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
    FLAGS_alsologtostderr = false;
    std::string log_file = logging_directory + "/current.log";
    google::SetLogDestination(google::GLOG_INFO, log_file.c_str());
    google::SetLogDestination(google::GLOG_WARNING, log_file.c_str());
    google::SetLogDestination(google::GLOG_ERROR, log_file.c_str());
    LOG_INFO << "Logger was set up." << "Directory to log: " << FLAGS_log_dir;

    // Create all objects in main
    MotorController motors_processor;
    const uint8_t kMotorNumber = 4;

    // Conservative default PID: kp = 8.0, ki = 0.0, kd = 1.0 unless overridden via flags (-q, -w, -e)
    const float default_kp = 8.0f;
    const float default_ki = 2.00f;
    const float default_kd = 0.01f;
    const float default_alpha = 0.7f;

    const float active_kp = has_coef_p ? coef_p : default_kp;
    const float active_ki = has_coef_i ? coef_i : default_ki;
    const float active_kd = has_coef_d ? coef_d : default_kd;
    const float active_alpha = default_alpha;

    const std::vector<MotorConfig> kShassisVector = {
        MotorConfig(3, false, {active_kp, active_ki, active_kd}, active_alpha),
        MotorConfig(4, false, {active_kp, active_ki, active_kd}, active_alpha),
        MotorConfig(1, true, {active_kp, active_ki, active_kd}, active_alpha),
        MotorConfig(2, true, {active_kp, active_ki, active_kd}, active_alpha),
    };

    ProtocolHanlder protocol_handler(motors_processor);
    IMUController imu_controller;
    TCPTransport tcp_transport(server_address, server_portnum);
    UDPClient udp_client;

    LOG_INFO << "start ...";

    // Pass configuration to KPIRoverECU, Init functions will be called from Start()
    KPIRoverECU kpi_rover_ecu(&protocol_handler, &tcp_transport, &udp_client, &imu_controller, &motors_processor,
                              kShassisVector, kMotorNumber, server_address, server_portnum);

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
