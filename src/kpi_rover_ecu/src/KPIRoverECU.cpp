#include "KPIRoverECU.h"

#include <arpa/inet.h>
#include <rc/time.h>
#include <stddef.h>

#include <csignal>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#include "IMUController.h"
#include "loggingIncludes.h"
#include "TCPTransport.h"
#include "UDPClient.h"
#include "protocolHandler.h"

KPIRoverECU::KPIRoverECU(ProtocolHanlder *_protocolHandler, TCPTransport *_tcpTransport, UDPClient *_udpClient,
                         IMUController *_imuController)
    : protocol_handler_(_protocolHandler),
      tcp_transport_(_tcpTransport),
      imu_controller_(_imuController),
      udp_client_(_udpClient),
      counter_(GetCounter()),
      runningProcess_(true),
      runningState_(false) {}

bool KPIRoverECU::Start() {
    tcp_transport_->Start();
    LOG_DEBUG << "Starting all thread in KPIRoverECU";
    timerThread_ = std::thread([this] { TimerThreadFuction(this->protocol_handler_); });
    processingThread_ = std::thread([this] { ProcessingThreadFunction(); });
    imuThread_ = std::thread([this] { IMUThreadFucntion(this->imu_controller_); });
    LOG_DEBUG << "All thread in KPIRoverECU started";

    if (!timerThread_.joinable() || !processingThread_.joinable() || !imuThread_.joinable()) {
        LOG_ERROR << "Error creating thread";
        return false;
    }

    return true;
}

void KPIRoverECU::IMUThreadFucntion(IMUController *workClass) {
    uint16_t packet_number = 0;

    std::string destination_address;
    int destination_port = 0;

    while (runningProcess_) {
        if (destination_address.empty()) {
            LOG_DEBUG << "Udp server address is unknown, get address from tcp server clients";
            destination_address = tcp_transport_->GetClientIp();
            destination_port = tcp_transport_->GetClientPort();
            if (!destination_address.empty()) {
                LOG_DEBUG << "Get UDP server address, initializing UDPClient";
                udp_client_->Init(destination_address, destination_port);
            }

        } else {
            const std::vector<float> kImuData = workClass->GetData();
            if (!kImuData.empty()) {
                if (packet_number == k16MaxCount) {
                    packet_number = 0;
                    LOG_DEBUG << "set UDP packet number to 0";
                }
                packet_number += 1;

                LOG_DEBUG << "build UDP packet " << packet_number << " for UDP server";
                std::vector<uint8_t> send_val;
                send_val.push_back(workClass->GetId());

                uint16_t send_packet_number = htons(packet_number);
                auto *bytes = reinterpret_cast<uint8_t *>(&send_packet_number);
                for (size_t i = 0; i < 2; ++i) {
                    send_val.push_back(bytes[i]);
                }

                float insert_value = 0;
                uint32_t value = 0;

                for (const float kImuValue : kImuData) {
                    insert_value = kImuValue;
                    std::memcpy(&value, &insert_value, sizeof(float));
                    value = ntohl(value);
                    bytes = reinterpret_cast<uint8_t *>(&value);

                    for (size_t j = 0; j < sizeof(uint32_t); ++j) {
                        send_val.push_back(bytes[j]);
                    }
                }
                
                LOG_DEBUG << "use UDP client to send message";
                udp_client_->Send(send_val);
            }
        }
        rc_usleep(kTimerPrecision);
    }
}

void KPIRoverECU::TimerThreadFuction(ProtocolHanlder *workClass) {
    const std::vector<uint8_t> kStopVector = workClass->MotorsStopMessage();

    while (!runningState_) {
        if (counter_ > 0) {
            rc_usleep(kTimerPrecision);
            counter_--;
            LOG_DEBUG << "counter decrement: " << counter_;

            if (counter_ == 0) {
                LOG_INFO << "Motor set to stop";
            }

        } else if (counter_ == 0) {
            // command to stop all motors
            LOG_DEBUG << "Send command to stop all motors";
            workClass->HandleMessage(kStopVector);
            imu_controller_->SetDisable();
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
    if (imuThread_.joinable()) {
        imuThread_.join();
        LOG_DEBUG << "IMUThread joined";
    }

    LOG_INFO << "destroying drivers";
    // tcp_transport_->Destroy();
}

int KPIRoverECU::GetCounter() { return (kTimeStop * kOneSecondMicro) / kTimerPrecision; }
