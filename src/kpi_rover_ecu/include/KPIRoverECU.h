#ifndef KPIROVERECU_H
#define KPIROVERECU_H

#include <atomic>
#include <cstdint>
#include <thread>

#include "IMUController.h"
#include "TCPTransport.h"
#include "UDPClient.h"
#include "protocolHandler.h"

// Constants for timing control
static constexpr std::uint32_t kTimerPrecision = 100000;   // 100 milliseconds in microsecond (for timer)
static constexpr std::uint32_t kOneSecondMicro = 1000000;  // 1 s in microseconds
static constexpr std::uint32_t kOneSecondMilli = 1000;     // 1 s in milliseconds
static constexpr std::uint32_t kTimeStop = 1;              // 5 seconds
static constexpr std::uint32_t k16MaxCount = 65535;

class KPIRoverECU {
   public:
    void TimerThreadFuction(ProtocolHanlder *workClass);
    void ProcessingThreadFunction();
    void IMUThreadFucntion(IMUController *workClass);

    KPIRoverECU(ProtocolHanlder *_protocolHandler, TCPTransport *_tcpTransport, UDPClient *_udpClient,
                IMUController *_imuController);
    bool Start();
    void Stop();

   private:
    ProtocolHanlder *protocol_handler_;
    TCPTransport *tcp_transport_;
    IMUController *imu_controller_;
    UDPClient *udp_client_;
    std::thread timerThread_;
    std::thread processingThread_;
    std::thread imuThread_;
    std::atomic<bool> runningProcess_;
    std::atomic<bool> runningState_;
    std::atomic<int> counter_;

    static int GetCounter();
};

#endif
