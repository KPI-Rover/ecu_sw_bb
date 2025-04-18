#ifndef KPIROVERECU_H
#define KPIROVERECU_H

#include <atomic>
#include <cstdint>
#include <thread>

#include "TCPTransport.h"
#include "protocolHandler.h"

// Constants for timing control
static constexpr std::uint32_t kTimerPrecision = 100000;   // 100 milliseconds in microsecond (for timer)
static constexpr std::uint32_t kOneSecondMicro = 1000000;  // 1 s in microseconds
static constexpr std::uint32_t kOneSecondMilli = 1000;     // 1 s in milliseconds
static constexpr std::uint32_t kTimeStop = 5;              // 5 seconds

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
    std::thread timerThread_;
    std::thread processingThread_;
    std::atomic<bool> runningProcess_;
    std::atomic<bool> runningState_;
    std::atomic<int> counter_;

    static int GetCounter();
};

#endif
