cmake_minimum_required(VERSION 3.10)
project(tcp-server) #temporary

#set(PROJECT_NAME main)
#set(CMAKE_C_COMPILER /usr/bin/arm-linux-gnueabihf-gcc-8)
#set(CMAKE_CXX_COMPILER /usr/bin/arm-linux-gnueabihf-g++-8)
set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)
set(CMAKE_CXX_STANDARD 11)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/include)
	
set(SOURCE_FILES
	src/server.cpp
	src/TCPServerReceiver.cpp
)

set(HEADER_FILES
	include/TCPServerReceiver.h		
)

add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${HEADER_FILES})

target_link_libraries(${PROJECT_NAME} Threads::Threads)



