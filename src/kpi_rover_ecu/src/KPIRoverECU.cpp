#include "KPIRoverECU.h"

#include "TCPTransport.h"
#include "config.h"
#include "protocolHandler.h"

KPIRoverECU::KPIRoverECU(ProtocolHanlder* _protocolHandler, TCPTransport* _tcpTransport) {
    protocol_handler_ = _protocolHandler;
    tcp_transport_ = _tcpTransport;
    counter.store(get_counter());

    runningProcess_.store(true);
    runningState_.store(false);
}

bool KPIRoverECU::start() {
    tcp_transport_->start();
    timerThread = thread([this] { timerThreadFuction(this->protocol_handler_); });
    processingThread = thread([this] { processingThreadFunction(); });


    if (!timerThread.joinable() || !processingThread.joinable()) {
        std::cout << "Error creating thread" << '\n';
        return false;
    }

    return true;
}

void KPIRoverECU::timerThreadFuction(ProtocolHanlder* workClass) {
    vector<uint8_t> stopVector;
    int32_t stopValue = 0;
    stopVector.push_back(ID_SET_ALL_MOTORS_SPEED);
    for (int i = 0; i < MOTORS_NUMBER; i++) {
        stopVector.insert(stopVector.end(), reinterpret_cast<uint8_t*>(&stopValue),
                          reinterpret_cast<uint8_t*>(&stopValue) + sizeof(int32_t));
    }

    while (!runningState_) {
        if (counter > 0) {
            usleep(TIMERPRECISION);
            counter--;

            if (counter == 0) {
                std::cout << "[INFO] Motor set to stop" << '\n';
            }

        } else if (counter == 0) {
            // command to stop all motors
            workClass->handleMessage(stopVector);

            sleep(TIMESTOP);
        }
    }
}

void KPIRoverECU::processingThreadFunction() {
    while (runningProcess_) {
        vector<uint8_t> message;
        if (tcp_transport_->receive(message)) {
            counter.store(get_counter());
            const vector<uint8_t> return_message = protocol_handler_->handleMessage(message);
            tcp_transport_->send(return_message);
        }
    }
}

void KPIRoverECU::stop() {
    std::cout << "END of program" << '\n';
    std::cout << "joining threads" << '\n';
    runningState_ = true;
    if (timerThread.joinable()) {
        timerThread.join();
    }
    runningProcess_ = false;
    if (processingThread.joinable()) {
        processingThread.join();
    }
    std::cout << "destroying drivers" << '\n';
    tcp_transport_->destroy();
}

int KPIRoverECU::get_counter() { return (TIMESTOP * ONESECONDMICRO) / TIMERPRECISION; }
