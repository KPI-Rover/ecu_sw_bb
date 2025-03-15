#ifndef KPIROVERECU_H
#define KPIROVERECU_H

#include "TCPTransport.h"
//#include "config.h"
#include "protocolHandler.h"
#include <atomic>
#include <condition_variable>
#include <csignal>
#include <cstring>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>
#include <unistd.h>

#define TIMERPRECISION 100000   // 100 miliseconds in microsecond (for timer)
#define ONESECONDMICRO 1000000  // 1 s in microseconds
#define ONESECONDMILI 1000
#define TIMESTOP 1  // 1 second befre stopping all motors. If no new command is received over TCP for 1 second, all motors must stop.

using namespace std;

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