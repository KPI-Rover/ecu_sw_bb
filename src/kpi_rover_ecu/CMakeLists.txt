cmake_minimum_required(VERSION 3.10)
project(kpi_rover_ecu)

set(CMAKE_CXX_STANDARD 11)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# Update include path to be relative to this CMakeLists.txt location
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
	
set(SOURCE_FILES
    src/main.cpp
    src/motorsController.cpp
    src/motor.cpp
    src/protocolHandler.cpp
    src/TCPTransport.cpp
    src/motorConfig.cpp
    src/messageQueue.cpp
)

set(HEADER_FILES
    include/config.h 
    include/motorsController.h
    include/motor.h
    include/protocolHandler.h
    include/TCPTransport.h 
    include/ITransport.h
    include/motorConfig.h
    include/messageQueue.h	
)

add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${HEADER_FILES})

# Add include directories to target properties
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Link the pthread library and robotcontrol
target_link_libraries(${PROJECT_NAME} PRIVATE 
    Threads::Threads
    robotcontrol
)

