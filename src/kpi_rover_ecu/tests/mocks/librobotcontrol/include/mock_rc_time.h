#ifndef MOCK_RC_TIME_H
#define MOCK_RC_TIME_H

#include <gmock/gmock.h>
#include <rc/time.h>

#include <memory>

class MockRCTime {
   public:
    MOCK_METHOD(void, nanosleep, (uint64_t ns));
    MOCK_METHOD(void, usleep, (unsigned int us));
    MOCK_METHOD(uint64_t, nanos_since_epoch, ());
    MOCK_METHOD(uint64_t, nanos_since_boot, ());
    MOCK_METHOD(uint64_t, timespec_to_micros, (struct timespec ts));
    MOCK_METHOD(uint64_t, timespec_to_nanos, (struct timespec ts));
    MOCK_METHOD(void, timespec_add, (struct timespec * start, double seconds));
};

/**
 * @brief Get the mock RC time instance
 *
 * @return MockRCTime& Reference to the singleton mock instance
 */
MockRCTime& GetMockRCTime();

/**
 * @brief Reset the mock RC time instance to a fresh state
 */
void ResetMockRCTime();

/**
 * @brief Destroy the MockRCTime instance
 * This releases the memory used by the mock object
 */
void DestroyMockRCTime();

#endif  // MOCK_RC_TIME_H
