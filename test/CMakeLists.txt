cmake_minimum_required(VERSION 3.10)
project(kpi_rover_tests)

# Enable testing before adding tests
enable_testing()

# Set output directory for test binaries
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Find system-installed GoogleTest
find_package(GTest REQUIRED)

# Find Threads
find_package(Threads REQUIRED)

# Add the build directory to the library search path
link_directories(${CMAKE_BINARY_DIR}/lib)

# REMOVED: Don't add_subdirectory again since it already exists elsewhere
# add_subdirectory(${CMAKE_SOURCE_DIR}/src/librobotcontrol_mock ${CMAKE_BINARY_DIR}/librobotcontrol_mock)

# Define test executable
add_executable(test_tcp_server 
    ${CMAKE_CURRENT_SOURCE_DIR}/test_tcp_server.cpp
    ${CMAKE_SOURCE_DIR}/src/kpi_rover_ecu/src/TCPServerReceiver.cpp
    ${CMAKE_SOURCE_DIR}/src/kpi_rover_ecu/src/motorsProcessor.cpp
)

# Ensure the mock library include directory is used
target_include_directories(test_tcp_server 
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src/kpi_rover_ecu/include
        ${CMAKE_SOURCE_DIR}/src/librobotcontrol_mock/include
        ${GTEST_INCLUDE_DIRS}
)

# Make test_tcp_server depend on the correct target name
add_dependencies(test_tcp_server kpi_rover_robotcontrol_mock)

# Link with library - using the correct target name
target_link_libraries(test_tcp_server
    PRIVATE
        GTest::GTest
        GTest::Main
        Threads::Threads
        kpi_rover_robotcontrol_mock  # Use the correct target name
)

# Explicitly add test to CTest with full path to ensure it's found
add_test(
    NAME test_tcp_server 
    COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_tcp_server
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

# Set test properties
set_tests_properties(test_tcp_server PROPERTIES
    ENVIRONMENT "PATH=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}:$ENV{PATH}"
)

# Print diagnostic information for debugging
message(STATUS "Test added: test_tcp_server -> ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_tcp_server")
