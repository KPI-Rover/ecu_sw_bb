#ifndef KPIROVERECU_H
#define KPIROVERECU_H

#include <atomic>
#include <cstdint>
#include <thread>
#include <vector>

#include "IMUController.h"
#include "ITransport.h"
#include "motorConfig.h"
#include "motorsController.h"
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
    void DebugThreadFunction();
    void IMUThreadFucntion(IMUController *workClass);

        KPIRoverECU(ProtocolHanlder *_protocolHandler, ITransport *_transport,
                IMUController *_imuController, MotorController *_motorController,
                const std::vector<MotorConfig> &_motorConfigs, int _motorNumber, const char *_server_address,
                int _server_portnum);
    bool Start();
    void Stop();

   private:
    ProtocolHanlder *protocol_handler_;
    ITransport *transport_;
    IMUController *imu_controller_;
    MotorController *motor_controller_;
    std::vector<MotorConfig> motor_configs_;
    uint8_t motor_number_;
    std::string server_address_;
    int server_port_;
    std::thread timerThread_;
    std::thread processingThread_;
    std::thread debugThread_;
    std::thread imuThread_;
    std::atomic<bool> runningProcess_;
    std::atomic<bool> runningState_;
    std::atomic<int> counter_;

    static int GetCounter();
};

#endif
