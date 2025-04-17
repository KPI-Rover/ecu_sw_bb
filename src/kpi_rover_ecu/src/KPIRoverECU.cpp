#include "KPIRoverECU.h"

#include <arpa/inet.h>
#include <rc/time.h>

#include <csignal>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <vector>

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
    timerThread_ = std::thread([this] { TimerThreadFuction(this->protocol_handler_); });
    processingThread_ = std::thread([this] { ProcessingThreadFunction(); });
    imuThread_ = std::thread([this] { IMUThreadFucntion(this->imu_controller_); });

    if (!timerThread_.joinable() || !processingThread_.joinable()) {
        std::cout << "Error creating thread" << '\n';
        return false;
    }

    return true;
}

void KPIRoverECU::IMUThreadFucntion(IMUController *workClass) {
    uint16_t packet_number = 0;

    while (runningProcess_) {
        if (workClass->GetEnable()) {
            if (packet_number == k16MaxCount) {
                packet_number = 0;
            }
            packet_number += 1;

            const std::vector<float> kImuData = workClass->GetData();
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

            udp_client_->Send(send_val);
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

            if (counter_ == 0) {
                std::cout << "[INFO] Motor set to stop" << '\n';
            }

        } else if (counter_ == 0) {
            // command to stop all motors
            workClass->HandleMessage(kStopVector);
            imu_controller_->SetDisable();
            rc_usleep(kTimeStop * kOneSecondMicro);
        }
    }
}

void KPIRoverECU::ProcessingThreadFunction() {
    while (runningProcess_) {
        std::vector<uint8_t> message;
        if (tcp_transport_->Receive(message)) {
            if (!imu_controller_->GetEnable()) {
                imu_controller_->SetEnable();
            }

            counter_.store(GetCounter());
            const std::vector<uint8_t> kReturnMessage = protocol_handler_->HandleMessage(message);
            tcp_transport_->Send(kReturnMessage);
        }
    }
}

void KPIRoverECU::Stop() {
    std::cout << "END of program" << '\n';
    std::cout << "joining threads" << '\n';
    runningState_ = true;
    if (timerThread_.joinable()) {
        timerThread_.join();
    }
    runningProcess_ = false;
    if (processingThread_.joinable()) {
        processingThread_.join();
    }
    std::cout << "destroying drivers" << '\n';
    // tcp_transport_->Destroy();
}

int KPIRoverECU::GetCounter() { return (kTimeStop * kOneSecondMicro) / kTimerPrecision; }
