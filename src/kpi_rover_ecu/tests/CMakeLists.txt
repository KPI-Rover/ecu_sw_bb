cmake_minimum_required(VERSION 3.14)
project(ecu_sw_testing CXX)

cmake_policy(SET CMP0079 NEW)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g --coverage -fprofile-arcs -ftest-coverage")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0 -g --coverage -fprofile-arcs -ftest-coverage")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} --coverage")
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

option(BUILD_GMOCK "Build GoogleMock along with GoogleTest" ON)

include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.16.0
)
FetchContent_MakeAvailable(googletest)

include_directories(${gtest_SOURCE_DIR}/include ${gmock_SOURCE_DIR}/include)

include(GoogleTest)

add_subdirectory(mocks/librobotcontrol)

add_executable(unitTests
    test_motor.cpp
    test_protocol.cpp
    test_message_queue.cpp
    test_motor_controller.cpp
    test_pid_regulator.cpp
    test_imu_controller.cpp
)

target_link_libraries(unitTests
    PRIVATE
        kpi_rover_ecu_core
        robotcontrol_mock
        GTest::gtest_main
        GTest::gmock
        pthread
)

target_include_directories(unitTests PRIVATE
    ${CMAKE_SOURCE_DIR}/mocks/librobotcontrol/include
    ${CMAKE_SOURCE_DIR}/external/librobotcontrol/library/include
)

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/test-reports)

gtest_discover_tests(unitTests
    WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    PROPERTIES ENVIRONMENT "GTEST_OUTPUT=xml:${CMAKE_BINARY_DIR}/test-reports/")

find_program(LCOV lcov)
find_program(GENHTML genhtml)

set(COVERAGE_DIR "${CMAKE_BINARY_DIR}/src/kpi_rover_ecu/src")

if(LCOV AND GENHTML)
    add_custom_target(coverage
        # Clean previous coverage data
        COMMAND find ${CMAKE_BINARY_DIR} -name '*.gcda' -delete
        COMMAND ${LCOV} --zerocounters --directory ${COVERAGE_DIR}

        # Create baseline
        COMMAND ${LCOV}
            --rc branch_coverage=1
            --rc geninfo_unexecuted_blocks=1
            --rc geninfo_auto_base=1
            --capture
            --initial
            --directory ${COVERAGE_DIR}
            --output-file coverage_base.info

        # Run tests
        COMMAND ctest --output-on-failure

        # Capture test coverage
        COMMAND ${LCOV}
            --rc branch_coverage=1
            --rc geninfo_unexecuted_blocks=1
            --rc geninfo_auto_base=1
            --capture
            --directory ${COVERAGE_DIR}
            --output-file coverage_test.info

        # Combine base and test coverage
        COMMAND ${LCOV}
            --rc branch_coverage=1
            --rc geninfo_unexecuted_blocks=1
            --rc geninfo_auto_base=1
            --directory ${COVERAGE_DIR}
            --add-tracefile coverage_base.info
            --add-tracefile coverage_test.info
            --output-file coverage.info

        # Remove test/mocks folders
        COMMAND ${LCOV}
            --rc branch_coverage=1
            --rc geninfo_unexecuted_blocks=1
            --rc geninfo_auto_base=1
            --remove coverage.info 
                "/usr/*"
            --output-file coverage_filtered.info

            # Generate HTML report
        COMMAND ${GENHTML}
            coverage_filtered.info
            --rc branch_coverage=1
            --output-directory coverage_report

        COMMAND ${CMAKE_COMMAND} -E echo "Coverage report generated in ${CMAKE_BINARY_DIR}/coverage_report/index.html"
        COMMAND ${CMAKE_COMMAND} -E echo "XML test reports in ${CMAKE_BINARY_DIR}/test-reports/"
        
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Running tests and generating coverage report for kpi_rover_ecu_core"
        VERBATIM
    )
else()
    message(WARNING "lcov or genhtml not found, coverage report generation disabled.")
endif()