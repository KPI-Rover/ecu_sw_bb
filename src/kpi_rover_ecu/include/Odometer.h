#ifndef ODOMETER_H
#define ODOMETER_H

#include <chrono>
#include <memory>

#include "Encoder.h"

/**
 * @class Odometer
 * @brief Calculates RPM from encoder ticks over time
 *
 * Tracks encoder count changes and timing to calculate rotational speed.
 * Multiple odometers can share the same encoder for different purposes
 * (e.g., public reporting vs. private PID control).
 */
class Odometer {
   public:
    /**
     * @brief Construct a new Odometer object
     * @param encoder Shared pointer to the Encoder to monitor
     * @param rpm_factor Number of encoder ticks per revolution
     * @param inverted If true, GetDiff() and GetRPM() return inverted values
     */
    Odometer(std::shared_ptr<Encoder> encoder, int rpm_factor, bool inverted = false);

    /**
     * @brief Get the encoder count difference since last read
     * @return int Number of ticks elapsed since last GetDiff() call
     * @note Updates internal state (last_read_count and last_read_time)
     */
    int GetDiff();

    /**
     * @brief Calculate current RPM based on encoder changes
     * @return int Calculated RPM value
     * @note Formula: (count_diff / rpm_factor) * (60 / elapsed_seconds)
     * @note Updates internal state (last_read_count and last_read_time)
     */
    int GetRPM();

   private:
    std::shared_ptr<Encoder> linked_encoder_;
    int last_read_count_;
    std::chrono::time_point<std::chrono::high_resolution_clock> last_read_time_;
    int rpm_factor_;
    bool inverted_;
};

#endif  // ODOMETER_H
