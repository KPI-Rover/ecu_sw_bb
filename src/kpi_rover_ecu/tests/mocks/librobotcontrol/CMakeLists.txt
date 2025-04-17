# Create the mock library
add_library(robotcontrol_mock STATIC
    src/encoder.cpp
    src/motor.cpp
    src/time.cpp
    src/start_stop.cpp
    src/imu.cpp
)

target_include_directories(robotcontrol_mock PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/external/librobotcontrol/library/include
)

target_link_libraries(robotcontrol_mock PUBLIC
    kpi_rover_ecu_core
)

