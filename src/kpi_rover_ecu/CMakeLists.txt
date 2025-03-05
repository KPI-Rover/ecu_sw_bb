cmake_minimum_required(VERSION 3.10)
project(kpi_rover_ecu)

set(CMAKE_CXX_STANDARD 11)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# Update include path to be relative to this CMakeLists.txt location
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
	
set(LIB_SOURCE_FILES
    src/TCPServerReceiver.cpp
    src/motorsProcessor.cpp
)

set(HEADER_FILES
    include/TCPServerReceiver.h
    include/config.h 
    include/motorsProcessor.h		
)

# Create library target
add_library(${PROJECT_NAME}_lib STATIC ${LIB_SOURCE_FILES} ${HEADER_FILES})
target_include_directories(${PROJECT_NAME}_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Link appropriate dependencies based on platform
target_link_libraries(${PROJECT_NAME}_lib PUBLIC 
        Threads::Threads
        robotcontrol
)

add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_lib)
