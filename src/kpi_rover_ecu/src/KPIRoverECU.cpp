#include "KPIRoverECU.h"
#include "config.h"
#include "TCPTransport.h"
#include "protocolHandler.h"


KPIRoverECU::KPIRoverECU(ProtocolHanlder* _protocolHandler, TCPTransport* _tcpTransport) {
	protocolHandler = _protocolHandler;
	tcpTransport  = _tcpTransport;
	counter.store(get_counter());

	runningProcess.store(true);
	runningState.store(false);

	
}

bool KPIRoverECU::start() {
	tcpTransport->start();
	timerThread = thread([this] { timerThreadFuction(this->protocolHandler); });
	processingThread = thread([this] { processingThreadFunction(); });

	// timerThread = thread(timerThreadFuction, protocolHandler);
	// processingThread = thread(processingThreadFunction, this);


	if (!timerThread.joinable() || !processingThread.joinable()) {
		cout << "Error creating thread" << endl;
		return false;
	}

	return true;

}

void KPIRoverECU::timerThreadFuction(ProtocolHanlder *workClass) {
    vector<uint8_t> stopVector;
    int32_t stopValue  = 0;
    stopVector.push_back(ID_SET_ALL_MOTORS_SPEED);
    for (int i = 0; i < MOTORS_NUMBER; i++) {
        stopVector.insert(stopVector.end(), 
                      reinterpret_cast<uint8_t*>(&stopValue), 
                      reinterpret_cast<uint8_t*>(&stopValue) + sizeof(int32_t));
    }

    while (!runningState) {
        if (counter > 0) {
            usleep(TIMERPRECISION);
            counter--;

            if (counter == 0 ) {
                cout << "[INFO] Motor set to stop" << endl;
            }

        } else if (counter == 0) {
            // command to stop all motors
            workClass->handleMessage(stopVector);

            sleep(TIMESTOP);
            
                
        }
            
    }


}

void KPIRoverECU::processingThreadFunction() {

	while (runningProcess) {
		vector<uint8_t> message;
        if (tcpTransport->receive(message)) {
            counter.store(get_counter());
            vector<uint8_t> return_message = protocolHandler->handleMessage(message);
            tcpTransport->send(return_message);
        }
	}
}

void KPIRoverECU::stop() {
	cout << "END of program" << endl;
	cout << "joining threads" << endl;
	runningState = true;
	if (timerThread.joinable()) {
		timerThread.join();
	}
	runningProcess = false;
	if (processingThread.joinable()) {
		processingThread.join();
	}
	cout << "destroying drivers" << endl;
	tcpTransport->destroy();
}

int KPIRoverECU::get_counter() {
    return  (TIMESTOP * ONESECONDMICRO) / TIMERPRECISION;
}


