cmake_minimum_required(VERSION 3.10)
project(kpi_rover_ecu)

if(BUILD_VARIANT STREQUAL "Target")
    option(BUILD_LIBROBOT_CONTROL "Build librobotcontrol using original Makefile" ON)
    add_subdirectory(external/librobotcontrol)
    add_subdirectory(src/kpi_rover_ecu)
endif()

if(BUILD_VARIANT STREQUAL "Tests")
    enable_testing()
    add_subdirectory(src/kpi_rover_ecu/src)
    add_subdirectory(src/kpi_rover_ecu/tests)
endif()
