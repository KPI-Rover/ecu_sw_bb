cmake_minimum_required(VERSION 3.10)
project(ecu-bbb)

# Set the C and C++ compilers (replace arm-linux-gnueabihf-gcc and arm-linux-gnueabihf-g++ with your cross-compilers)
set(CMAKE_C_COMPILER /usr/bin/arm-linux-gnueabihf-gcc-8)
set(CMAKE_CXX_COMPILER /usr/bin/arm-linux-gnueabihf-g++-8)
set(CMAKE_CXX_STANDARD 11)

# Add the pthread library
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# Add your source files here
set(SOURCE_FILES
    src/main.cpp
    src/SerialReceiver.cpp
    src/CommandProcessor.cpp
)

# Add your header files here
set(HEADER_FILES
    include/SerialReceiver.h
    include/CommandProcessor.h
)

# Add the executable
add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${HEADER_FILES})

# Set include directories
# target_include_directories(${PROJECT_NAME} PRIVATE include)
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

# Set output directories
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)
