#include "KPIRoverECU.h"

#include <arpa/inet.h>
#include <rc/time.h>
#include <stddef.h>

#include <csignal>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#include "EncoderController.h"
#include "IMUController.h"
#include "TCPTransport.h"
#include "UDPClient.h"
#include "loggingIncludes.h"
#include "protocolHandler.h"

KPIRoverECU::KPIRoverECU(ProtocolHanlder *_protocolHandler, TCPTransport *_tcpTransport, UDPClient *_udpClient,
                         IMUController *_imuController, MotorController *_motorController,
                         const std::vector<MotorConfig> &_motorConfigs, uint8_t _motorNumber,
                         const std::string &_serverAddress, int _serverPort)
    : protocol_handler_(_protocolHandler),
      tcp_transport_(_tcpTransport),
      imu_controller_(_imuController),
      udp_client_(_udpClient),
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

    // Initialize TCP Transport
    if (tcp_transport_->Init() == -1) {
        LOG_ERROR << "Error creating socket";
        imu_controller_->Stop();
        motor_controller_->Destroy();
        EncoderController::Destroy();
        return false;
    }

    tcp_transport_->Start();

    motor_controller_->Start();
    imu_controller_->Start(udp_client_);
    
    LOG_DEBUG << "Starting all thread in KPIRoverECU";

    timerThread_ = std::thread([this] { TimerThreadFuction(this->protocol_handler_); });

    processingThread_ = std::thread([this] { ProcessingThreadFunction(); });
    
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
        if (tcp_transport_->Receive(message)) {
            LOG_DEBUG << "TCP packet received, processing it";
            imu_controller_->SetEnable();

            counter_.store(GetCounter());
            LOG_DEBUG << "set timer counter to GetCounter()";
            const std::vector<uint8_t> kReturnMessage = protocol_handler_->HandleMessage(message);
            LOG_DEBUG << "get TCP response";
            tcp_transport_->Send(kReturnMessage);
            LOG_DEBUG << "Send TCP response";
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

    motor_controller_->Stop();
    LOG_INFO << "destroying drivers";
    motor_controller_->Destroy();
    imu_controller_->Stop();
    tcp_transport_->Destroy();
    udp_client_->Destroy();
    EncoderController::Destroy();
}

int KPIRoverECU::GetCounter() { return (kTimeStop * kOneSecondMicro) / kTimerPrecision; }
