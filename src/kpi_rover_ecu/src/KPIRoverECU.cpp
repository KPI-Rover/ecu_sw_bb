#include "KPIRoverECU.h"

#include <arpa/inet.h>
#include <rc/time.h>
#include <stddef.h>

#include <csignal>
#include <cstdint>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "EncoderController.h"
#include "IMUController.h"
#include "ITransport.h"
#include "loggingIncludes.h"
#include "protocolHandler.h"

KPIRoverECU::KPIRoverECU(ProtocolHanlder *_protocolHandler, ITransport *_transport,
                         IMUController *_imuController, MotorController *_motorController,
                         const std::vector<MotorConfig> &_motorConfigs, int _motorNumber,
                         const char *_serverAddress, int _serverPort)
    : protocol_handler_(_protocolHandler),
      transport_(_transport),
      imu_controller_(_imuController),
      motor_controller_(_motorController),
      motor_configs_(_motorConfigs),
      motor_number_(_motorNumber),
      server_address_(_serverAddress),
      server_port_(_serverPort),
      counter_(GetCounter()),
      runningProcess_(true),
      runningState_(false) {}

bool KPIRoverECU::Start() {
    // Initialize EncoderController first
    
    if (EncoderController::Init() != 0) {
        LOG_ERROR << "Failed to initialize EncoderController";
        return false;
    }

    // Initialize MotorController
    if (motor_controller_->Init(motor_configs_, motor_number_) != 0) {
        LOG_ERROR << "Failed to initialize MotorController";
        EncoderController::Destroy();
        return false;
    }

    // Initialize IMUController
    if (imu_controller_->Init() == -1) {
        LOG_ERROR << "Error initializing IMU controller";
        motor_controller_->Destroy();
        EncoderController::Destroy();
        return false;
    }

    // Initialize Transport
    if (transport_->Init() == -1) {
        LOG_ERROR << "Error creating transport";
        imu_controller_->Stop();
        motor_controller_->Destroy();
        EncoderController::Destroy();
        return false;
    }

    transport_->Start();

    motor_controller_->Start();
    
    LOG_DEBUG << "Starting all thread in KPIRoverECU";

    //timerThread_ = std::thread([this] { TimerThreadFuction(this->protocol_handler_); });

    processingThread_ = std::thread([this] { ProcessingThreadFunction(); });
    //debugThread_ = std::thread([this] { DebugThreadFunction(); });
    
    LOG_DEBUG << "All thread in KPIRoverECU started";

    return true;
}

void KPIRoverECU::TimerThreadFuction(ProtocolHanlder *workClass) {
    const std::vector<uint8_t> kStopVector = workClass->MotorsStopMessage();

    while (!runningState_) {
        if (counter_ > 0) {
            rc_usleep(kTimerPrecision);
            counter_--;
            LOG_DEBUG << "counter decrement: " << counter_;

        } else  {
            // command to stop all motors
            LOG_ERROR << "Send command to stop all motors";
            workClass->HandleMessage(kStopVector);
            // imu_controller_->SetDisable(); // Do not disable IMU on timer timeout
            rc_usleep(kTimeStop * kOneSecondMicro);
        }
    }
}

void KPIRoverECU::ProcessingThreadFunction() {
    LOG_DEBUG << "start cycle in main thread";
    while (runningProcess_) {
        std::vector<uint8_t> message;
        if (transport_->Receive(message)) {
            LOG_DEBUG << "Packet received, processing it";

            counter_.store(GetCounter());
            LOG_DEBUG << "set timer counter to GetCounter()";
            const std::vector<uint8_t> kReturnMessage = protocol_handler_->HandleMessage(message);
            
            std::stringstream ss;
            for (const auto& byte : kReturnMessage) {
                ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte) << " ";
            }
            LOG_INFO << "Sending response: " << ss.str();

            LOG_DEBUG << "get response";
            transport_->Send(kReturnMessage);
            LOG_DEBUG << "Send response";
        }
    }
}

void KPIRoverECU::Stop() {
    LOG_INFO << "END of program";
    LOG_INFO << "joining threads";
    runningState_ = true;
    if (timerThread_.joinable()) {
        timerThread_.join();
        LOG_DEBUG << "timerThread_ joined";
    }
    runningProcess_ = false;
    if (processingThread_.joinable()) {
        processingThread_.join();
        LOG_DEBUG << "main thread joined";
    }
    if (debugThread_.joinable()) {
        debugThread_.join();
        LOG_DEBUG << "debugThread_ joined";
    }

    motor_controller_->Stop();
    LOG_INFO << "destroying drivers";
    motor_controller_->Destroy();
    imu_controller_->Stop();
    transport_->Destroy();
    EncoderController::Destroy();
}

int KPIRoverECU::GetCounter() { return (kTimeStop * kOneSecondMicro) / kTimerPrecision; }

void KPIRoverECU::DebugThreadFunction() {
    std::srand(std::time(nullptr));
    uint32_t debug_counter = 0;
    while (runningProcess_) {
        std::vector<uint8_t> packet;
        packet.reserve(50);
        packet.push_back(0xBE);
        packet.push_back(0xDA);
        packet.push_back((debug_counter >> 24) & 0xFF);
        packet.push_back((debug_counter >> 16) & 0xFF);
        packet.push_back((debug_counter >> 8) & 0xFF);
        packet.push_back(debug_counter & 0xFF);

        // Fill the rest with random values (random length from 5 to 100)
        int random_length = 5 + (std::rand() % 96); // 5 to 100
        for (int i = 0; i < random_length; ++i) {
            packet.push_back(static_cast<uint8_t>(std::rand() & 0xFF));
        }

        transport_->Send(packet);
        
        debug_counter++;
        rc_usleep(10000); // 50ms
    }
}
