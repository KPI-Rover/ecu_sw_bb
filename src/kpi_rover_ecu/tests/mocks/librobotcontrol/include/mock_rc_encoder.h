#ifndef MOCK_RC_ENCODER_H
#define MOCK_RC_ENCODER_H

#include <rc/encoder.h>
#include <gmock/gmock.h>
#include <memory>

/**
 * @brief Mock class for RC encoder functions
 */
class MockRCEncoder {
public:
    MOCK_METHOD(int, init, ());
    MOCK_METHOD(int, cleanup, ());
    MOCK_METHOD(int, read, (int encoder));
    MOCK_METHOD(int, write, (int encoder, int pos));
    MOCK_METHOD(int, eqep_read, (int ch));
    MOCK_METHOD(int, pru_read, (int ch));
};

/**
 * @brief Get the mock RC encoder instance
 * 
 * @return MockRCEncoder& Reference to the singleton mock instance
 */
MockRCEncoder& GetMockRCEncoder();

/**
 * @brief Reset the mock RC encoder instance to a fresh state
 */
void ResetMockRCEncoder();

/**
 * @brief Destroy the MockRCEncoder instance
 * This releases the memory used by the mock object
 */
void DestroyMockRCEncoder();

#endif // MOCK_RC_ENCODER_H
