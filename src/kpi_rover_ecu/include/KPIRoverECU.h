#ifndef KPIROVERECU_H
#define KPIROVERECU_H

#include "TCPTransport.h"
#include "config.h"
#include "protocolHandler.h"

class KPIRoverECU {
   public:
    
    void timerThreadFuction(ProtocolHanlder *workClass);
    void processingThreadFunction();

    KPIRoverECU(ProtocolHanlder *_protocolHandler, TCPTransport *_tcpTransport);
    bool start();
    void stop();

   private:
    ProtocolHanlder *protocol_handler_;
    TCPTransport *tcp_transport_;
    thread timerThread, processingThread;
    atomic<bool> runningProcess_;
    atomic<bool> runningState_;
    atomic<int> counter;

    int get_counter();
};

#endif