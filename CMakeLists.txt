cmake_minimum_required(VERSION 3.10)
project(kpi_rover_motor_control) #temporary

#set(PROJECT_NAME main)
set(CMAKE_C_COMPILER /usr/bin/arm-linux-gnueabihf-gcc-8)
set(CMAKE_CXX_COMPILER /usr/bin/arm-linux-gnueabihf-g++-8)
#set(CMAKE_C_COMPILER gcc)
#set(CMAKE_CXX_COMPILER g++)
set(CMAKE_CXX_STANDARD 11)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/include)
	
set(SOURCE_FILES
	src/main.cpp
	src/TCPServerReceiver.cpp
	src/motorsProcessor.cpp
)

set(HEADER_FILES
	include/TCPServerReceiver.h
	include/config.h 
	include/motorsProcessor.h		
)

add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${HEADER_FILES})
target_include_directories(${PROJECT_NAME} PRIVATE include /rootfs/usr/include/include)

# Link the pthread library
target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)

# Find librobotcontrol.so
find_library(ROBOTCONTROL_LIB robotcontrol PATHS /rootfs/usr/lib)
# Check if the library was found
if(NOT ROBOTCONTROL_LIB)
    message(FATAL_ERROR "librobotcontrol.so not found")
endif()
target_link_libraries(${PROJECT_NAME} PRIVATE ${ROBOTCONTROL_LIB})



