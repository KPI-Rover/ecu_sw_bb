#include "KPIRoverECU.h"

#include <rc/time.h>

#include <csignal>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <vector>

#include "TCPTransport.h"
#include "protocolHandler.h"

KPIRoverECU::KPIRoverECU(ProtocolHanlder* _protocolHandler, TCPTransport* _tcpTransport)
    : protocol_handler_(_protocolHandler),
      tcp_transport_(_tcpTransport),
      counter_(GetCounter()),
      runningProcess_(true),
      runningState_(false) {}

bool KPIRoverECU::Start() {
    tcp_transport_->Start();
    timerThread_ = std::thread([this] { TimerThreadFuction(this->protocol_handler_); });
    processingThread_ = std::thread([this] { ProcessingThreadFunction(); });

    if (!timerThread_.joinable() || !processingThread_.joinable()) {
        std::cout << "Error creating thread" << '\n';
        return false;
    }

    return true;
}

void KPIRoverECU::TimerThreadFuction(ProtocolHanlder* workClass) {
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

            rc_usleep(kTimeStop * kOneSecondMicro);
        }
    }
}

void KPIRoverECU::ProcessingThreadFunction() {
    while (runningProcess_) {
        std::vector<uint8_t> message;
        if (tcp_transport_->Receive(message)) {
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
    tcp_transport_->Destroy();
}

int KPIRoverECU::GetCounter() { return (kTimeStop * kOneSecondMicro) / kTimerPrecision; }
