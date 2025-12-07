#include "EncoderController.h"

#include <rc/encoder.h>

#include "loggingIncludes.h"

// Initialize static members
std::array<std::shared_ptr<Encoder>, EncoderController::kEncoderChannels> EncoderController::encoders_;
std::mutex EncoderController::pool_mutex_;
bool EncoderController::initialized_ = false;

int EncoderController::Init() {
    LOG_DEBUG << "EncoderController::Init()";

    std::lock_guard<std::mutex> lock(pool_mutex_);

    if (initialized_) {
        LOG_WARNING << "EncoderController already initialized";
        return 0;
    }

    // Initialize hardware encoders
    if (rc_encoder_init() != 0) {
        LOG_ERROR << "Failed to initialize rc_encoder";
        return -1;
    }

    // Create Encoder objects for all 4 channels (1-4)
    for (int i = 0; i < kEncoderChannels; ++i) {
        const int kChannel = i + 1;  // Channels are 1-based
        encoders_[i] = std::make_shared<Encoder>(kChannel);
        LOG_DEBUG << "Created Encoder for channel " << kChannel;
    }

    initialized_ = true;
    LOG_INFO << "EncoderController initialized successfully";
    return 0;
}

void EncoderController::Destroy() {
    LOG_DEBUG << "EncoderController::Destroy()";

    std::lock_guard<std::mutex> lock(pool_mutex_);

    if (!initialized_) {
        LOG_WARNING << "EncoderController not initialized, nothing to destroy";
        return;
    }

    // Clear all encoder shared pointers
    for (auto& encoder : encoders_) {
        encoder.reset();
    }

    // Cleanup hardware
    rc_encoder_cleanup();

    initialized_ = false;
    LOG_INFO << "EncoderController destroyed successfully";
}

std::unique_ptr<Odometer> EncoderController::CreateOdometer(int channel, int rpm_factor, bool inverted) {
    LOG_DEBUG << "EncoderController::CreateOdometer(channel: " << channel << ", rpm_factor: " << rpm_factor
              << ", inverted: " << inverted << ")";

    if (channel < 1 || channel > kEncoderChannels) {
        LOG_ERROR << "Invalid encoder channel: " << channel << " (must be 1-" << kEncoderChannels << ")";
        return nullptr;
    }

    if (rpm_factor <= 0) {
        LOG_ERROR << "Invalid rpm_factor: " << rpm_factor << " (must be > 0)";
        return nullptr;
    }

    std::lock_guard<std::mutex> lock(pool_mutex_);

    if (!initialized_) {
        LOG_ERROR << "EncoderController not initialized";
        return nullptr;
    }

    const int kIndex = channel - 1;  // Convert to 0-based index
    auto encoder = encoders_[kIndex];

    if (!encoder) {
        LOG_ERROR << "Encoder not found for channel " << channel;
        return nullptr;
    }

    auto odometer = std::unique_ptr<Odometer>(new Odometer(encoder, rpm_factor, inverted));
    LOG_INFO << "Created Odometer for channel " << channel << " with rpm_factor " << rpm_factor << " and inverted "
             << inverted;

    return odometer;
}
