#ifndef ENCODER_H
#define ENCODER_H

#include <mutex>

/**
 * @class Encoder
 * @brief Thread-safe wrapper for hardware encoder channels
 *
 * Provides thread-safe access to encoder count data from RC_Encoder hardware.
 * Each instance represents one physical encoder channel (1-4).
 */
class Encoder {
   public:
    /**
     * @brief Construct a new Encoder object
     * @param channel Hardware encoder channel number (1-4)
     */
    explicit Encoder(int channel);

    /**
     * @brief Get the current encoder count
     * @return int Current count value (thread-safe)
     */
    int GetCount();

   private:
    int channel_;
    std::mutex count_mutex_;
};

#endif  // ENCODER_H
