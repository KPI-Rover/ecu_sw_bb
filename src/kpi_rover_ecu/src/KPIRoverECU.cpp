#include "KPIRoverECU.h"

// #include "TCPTransport.h"
// #include "config.h"
// #include "protocolHandler.h"

KPIRoverECU::KPIRoverECU(ProtocolHanlder* _protocolHandler, TCPTransport* _tcpTransport) : protocol_handler_(_protocolHandler), tcp_transport_(_tcpTransport) {
    // protocol_handler_ = _protocolHandler;
    // tcp_transport_ = _tcpTransport;
    counter_.store(GetCounter());

    runningProcess_.store(true);
    runningState_.store(false);
}

bool KPIRoverECU::Start() {
    tcp_transport_->Start();
    timerThread_ = thread([this] { TimerThreadFuction(this->protocol_handler_); });
    processingThread_ = thread([this] { ProcessingThreadFunction(); });


    if (!timerThread_.joinable() || !processingThread_.joinable()) {
        std::cout << "Error creating thread" << '\n';
        return false;
    }

    return true;
}

void KPIRoverECU::TimerThreadFuction(ProtocolHanlder* workClass) {
    vector<uint8_t> stop_vector = workClass->MotorsStopMessage();


    while (!runningState_) {
        if (counter_ > 0) {
            rc_usleep(TIMERPRECISION);
            counter_--;

            if (counter_ == 0) {
                std::cout << "[INFO] Motor set to stop" << '\n';
            }

        } else if (counter_ == 0) {
            // command to stop all motors
            workClass->HandleMessage(stop_vector);

            rc_usleep(TIMESTOP * ONESECONDMICRO);
        }
    }
}

void KPIRoverECU::ProcessingThreadFunction() {
    while (runningProcess_) {
        vector<uint8_t> message;
        if (tcp_transport_->Receive(message)) {
            counter_.store(GetCounter());
            const vector<uint8_t> kReturnMessage = protocol_handler_->HandleMessage(message);
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

int KPIRoverECU::GetCounter() { return (TIMESTOP * ONESECONDMICRO) / TIMERPRECISION; }
