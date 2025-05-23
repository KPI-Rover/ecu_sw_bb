cmake_minimum_required(VERSION 3.10)
project(kpi_rover_ecu)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# Add include directories globally so all targets can find the headers
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_subdirectory(src)

add_executable(${PROJECT_NAME} src/main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE
    kpi_rover_ecu_core
    Threads::Threads
    robotcontrol
    glog
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
