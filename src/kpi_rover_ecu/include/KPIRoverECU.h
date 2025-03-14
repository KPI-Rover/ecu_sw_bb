#ifndef KPIROVERECU_H
#define KPIROVERECU_H

#include "TCPTransport.h"
#include "config.h"
#include "protocolHandler.h"
#include <atomic>

class KPIRoverECU {
   public:
    
    void TimerThreadFuction(ProtocolHanlder *workClass);
    void ProcessingThreadFunction();

    KPIRoverECU(ProtocolHanlder *_protocolHandler, TCPTransport *_tcpTransport);
    bool Start();
    void Stop();

   private:
    ProtocolHanlder *protocol_handler_;
    TCPTransport *tcp_transport_;
    thread timerThread_, processingThread_;
    atomic<bool> runningProcess_;
    atomic<bool> runningState_;
    atomic<int> counter_;

    int GetCounter();
};

#endif