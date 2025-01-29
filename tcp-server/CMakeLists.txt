cmake_minimum_required(VERSION 3.10)
project(tcp-server) #temporary

#set(PROJECT_NAME main)
set(CMAKE_C_COMPILER /usr/bin/arm-linux-gnueabihf-gcc-8)
set(CMAKE_CXX_COMPILER /usr/bin/arm-linux-gnueabihf-g++-8)

set(CMAKE_CXX_STANDARD 11)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
	
set(SOURCE_FILES
	server.cpp
	TCPServerReceiver.cpp
)

set(HEADER_FILES
	TCPServerReceiver.h		
)

add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${HEADER_FILES})

target_link_libraries(${PROJECT_NAME} Threads::Threads)



