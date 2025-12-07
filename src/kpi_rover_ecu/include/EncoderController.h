#ifndef ENCODERCONTROLLER_H
#define ENCODERCONTROLLER_H

#include <array>
#include <memory>
#include <mutex>

#include "Encoder.h"
#include "Odometer.h"

/**
 * @class EncoderController
 * @brief Manages all hardware encoder instances and creates odometers
 *
 * Static singleton-style controller that manages exactly 4 hardware encoder channels.
 * Provides thread-safe access to encoders and factory method for odometers.
 */
class EncoderController {
   public:
    static constexpr int kEncoderChannels = 4;

    /**
     * @brief Initialize hardware encoders and create Encoder objects
     * @return int 0 on success, -1 on failure
     * @note Calls rc_encoder_init() and creates 4 Encoder instances
     */
    static int Init();

    /**
     * @brief Cleanup hardware and destroy all encoder objects
     * @note Calls rc_encoder_cleanup() and releases all Encoder shared_ptrs
     */
    static void Destroy();

    /**
     * @brief Factory method to create a new Odometer
     * @param channel Hardware encoder channel (1-4) to link odometer to
     * @param rpm_factor Number of encoder ticks per revolution
     * @param inverted If true, odometer returns inverted values
     * @return std::unique_ptr<Odometer> Unique pointer to new odometer, or nullptr on error
     * @note Each odometer gets a shared_ptr to the encoder, allowing multiple odometers per encoder
     */
    static std::unique_ptr<Odometer> CreateOdometer(int channel, int rpm_factor, bool inverted = false);

   private:
    // Delete constructors to prevent instantiation
    EncoderController() = delete;
    ~EncoderController() = delete;
    EncoderController(const EncoderController&) = delete;
    EncoderController& operator=(const EncoderController&) = delete;

    static std::array<std::shared_ptr<Encoder>, kEncoderChannels> encoders_;
    static std::mutex pool_mutex_;
    static bool initialized_;
};

#endif  // ENCODERCONTROLLER_H
