#include <gmock/gmock.h>
#include <rc/start_stop.h>

#include <memory>

#include "mock_rc_start_stop.h"

// Global state variable
static rc_state_t current_state = UNINITIALIZED;
static std::unique_ptr<StartStopMock> g_start_stop_mock;

// Implementation of the static method declared in the header
StartStopMock& StartStopMock::getInstance() {
    // Return the global instance instead of a local static instance
    if (!g_start_stop_mock) {
        g_start_stop_mock = std::make_unique<StartStopMock>();
        setupDefaultMockActions();
    }
    return *g_start_stop_mock;
}

// Implementation of the C API using the mock class
extern "C" {

rc_state_t rc_get_state(void) { return StartStopMock::getInstance().get_state(); }

void rc_set_state(rc_state_t new_state) {
    StartStopMock::getInstance().set_state(new_state);
    current_state = new_state;  // Keep track of state for default behavior
}

int rc_print_state(void) { return StartStopMock::getInstance().print_state(); }

int rc_make_pid_file(void) { return StartStopMock::getInstance().make_pid_file(); }

int rc_kill_existing_process(float timeout_s) { return StartStopMock::getInstance().kill_existing_process(timeout_s); }

int rc_remove_pid_file(void) { return StartStopMock::getInstance().remove_pid_file(); }

int rc_enable_signal_handler(void) { return StartStopMock::getInstance().enable_signal_handler(); }

int rc_disable_signal_handler(void) { return StartStopMock::getInstance().disable_signal_handler(); }

}  // extern "C"

// Set up default actions for the mock functions
void setupDefaultMockActions() {
    auto& mock = StartStopMock::getInstance();

    ON_CALL(mock, get_state()).WillByDefault(::testing::Return(current_state));

    ON_CALL(mock, set_state(::testing::_)).WillByDefault([](rc_state_t state) { current_state = state; });

    ON_CALL(mock, print_state()).WillByDefault(::testing::Return(0));

    ON_CALL(mock, make_pid_file()).WillByDefault(::testing::Return(0));

    ON_CALL(mock, kill_existing_process(::testing::_)).WillByDefault(::testing::Return(0));

    ON_CALL(mock, remove_pid_file()).WillByDefault(::testing::Return(0));

    ON_CALL(mock, enable_signal_handler()).WillByDefault(::testing::Return(0));

    ON_CALL(mock, disable_signal_handler()).WillByDefault(::testing::Return(0));
}

// Function to get access to the mock for setting expectations in tests
StartStopMock& getStartStopMock() {
    if (!g_start_stop_mock) {
        g_start_stop_mock = std::make_unique<StartStopMock>();
        setupDefaultMockActions();  // Make sure default actions are set up when creating the mock
    }
    return *g_start_stop_mock;
}

void ResetStartStopMock() {
    g_start_stop_mock = std::make_unique<StartStopMock>();
    setupDefaultMockActions();  // Add this line to set up default actions after reset
}

void DestroyStartStopMock() { g_start_stop_mock.reset(); }
