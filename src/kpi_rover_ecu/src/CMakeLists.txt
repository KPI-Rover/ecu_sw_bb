add_library(kpi_rover_ecu_core STATIC
    KPIRoverECU.cpp
    motor.cpp
    motorsController.cpp
    protocolHandler.cpp
    TCPTransport.cpp
    motorConfig.cpp
    messageQueue.cpp
    PIDRegulator.cpp
)

# Include public headers
target_include_directories(kpi_rover_ecu_core PUBLIC
    ${CMAKE_SOURCE_DIR}/src/kpi_rover_ecu/include
    ${CMAKE_SOURCE_DIR}/external/librobotcontrol/library/include
)
