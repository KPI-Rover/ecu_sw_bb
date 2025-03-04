cmake_minimum_required(VERSION 3.10)
project(ecu_sw_bb)

# Add librobotcontrol subdirectory
option(BUILD_LIBROBOT_CONTROL "Build librobotcontrol using original Makefile" ON)
add_subdirectory(external/librobotcontrol/library)

# Create interface library for librobotcontrol (not needed anymore, using target directly)
add_subdirectory(src/kpi_rover_ecu)
