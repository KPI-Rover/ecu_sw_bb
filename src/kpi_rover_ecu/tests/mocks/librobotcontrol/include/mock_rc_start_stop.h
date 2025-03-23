#ifndef MOCK_RC_START_STOP_H
#define MOCK_RC_START_STOP_H

#include <gmock/gmock.h>
#include <rc/start_stop.h>

#include <memory>

/**
 * @brief Mock class for RC start stop functions
 */
class StartStopMock {
   public:
    MOCK_METHOD(rc_state_t, get_state, (), (const));
    MOCK_METHOD(void, set_state, (rc_state_t new_state), ());
    MOCK_METHOD(int, print_state, (), (const));
    MOCK_METHOD(int, make_pid_file, (), ());
    MOCK_METHOD(int, kill_existing_process, (float timeout_s), ());
    MOCK_METHOD(int, remove_pid_file, (), ());
    MOCK_METHOD(int, enable_signal_handler, (), ());
    MOCK_METHOD(int, disable_signal_handler, (), ());

    // Singleton pattern
    static StartStopMock& getInstance();
};

// Matcher for state comparison
MATCHER_P(StateEquals, state, "States are not equal") { return arg == state; }

/**
 * @brief Get the Start Stop Mock instance and set up default expectations
 *
 * @return StartStopMock& Reference to the singleton mock instance
 */
StartStopMock& getStartStopMock();

/**
 * @brief Set up default actions for the mock functions
 */
void setupDefaultMockActions();

/**
 * @brief Destroy the StartStopMock instance
 * This releases the memory used by the mock object
 */
void DestroyStartStopMock();

#endif  // MOCK_RC_START_STOP_H
