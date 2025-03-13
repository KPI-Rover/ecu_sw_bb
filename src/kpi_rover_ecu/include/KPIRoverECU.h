#ifndef KPIROVERECU_H
#define KPIROVERECU_H

#include "config.h"
#include "TCPTransport.h"
#include "protocolHandler.h"

class KPIRoverECU {
public:
	ProtocolHanlder *protocolHandler;
	TCPTransport *tcpTransport;
	void timerThreadFuction(ProtocolHanlder *workClass);
	void processingThreadFunction();

	KPIRoverECU(ProtocolHanlder* _protocolHandler, TCPTransport* _tcpTransport);
	bool start();
	void stop();

private:
	

	thread timerThread, processingThread;
	atomic<bool> runningProcess;
	atomic<bool> runningState;
	atomic<int> counter;

	int get_counter();

	

};

#endif