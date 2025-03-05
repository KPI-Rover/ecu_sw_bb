cmake_minimum_required(VERSION 3.10)
project(ecu_sw_bb VERSION 1.0)

# Enable testing support at the top level
enable_testing()


if(${BUILD_VARIANT} STREQUAL "Target")
    add_subdirectory(src/kpi_rover_ecu)
    option(BUILD_LIBROBOT_CONTROL "Build librobotcontrol using original Makefile" ON)
    add_subdirectory(external/librobotcontrol/library)
endif()

if(${BUILD_VARIANT} STREQUAL "GTests")
    add_subdirectory(src/librobotcontrol_mock)
    add_subdirectory(test)
endif()

# Print diagnostic information to help with debugging
message(STATUS "CMAKE_BINARY_DIR: ${CMAKE_BINARY_DIR}")
message(STATUS "Test binary location: ${CMAKE_BINARY_DIR}/bin/test_tcp_server")
message(STATUS "CTest Executable: ${CMAKE_CTEST_COMMAND}")

