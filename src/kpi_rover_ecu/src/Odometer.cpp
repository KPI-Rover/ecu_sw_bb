#include "Odometer.h"

#include <cmath>

#include "loggingIncludes.h"

Odometer::Odometer(std::shared_ptr<Encoder> encoder, int rpm_factor, bool inverted)
    : linked_encoder_(encoder),
      last_read_count_(0),
      last_read_time_(std::chrono::high_resolution_clock::now()),
      rpm_factor_(rpm_factor),
      inverted_(inverted) {
    LOG_DEBUG << "Odometer::Odometer(encoder, rpm_factor: " << rpm_factor << ", inverted: " << inverted << ")";
}

int Odometer::GetDiff() {
    LOG_DEBUG << "Odometer::GetDiff()";

    const int kCurrentCount = linked_encoder_->GetCount();
    int kDiff = kCurrentCount - last_read_count_;

    last_read_count_ = kCurrentCount;
    last_read_time_ = std::chrono::high_resolution_clock::now();

    if (!inverted_) {
        kDiff = -kDiff;
    }

    LOG_DEBUG << "Odometer::GetDiff() returns: " << kDiff;
    return kDiff;
}

int Odometer::GetRPM() {
    LOG_DEBUG << "Odometer::GetRPM()";

    const auto kCurrentTime = std::chrono::high_resolution_clock::now();
    const int kCurrentCount = linked_encoder_->GetCount();

    const std::chrono::duration<double, std::milli> kElapsedMilliseconds = kCurrentTime - last_read_time_;
    const double kElapsedSeconds = kElapsedMilliseconds.count() / 1000.0;

    if (kElapsedSeconds <= 0.0) {
        LOG_WARNING << "Odometer::GetRPM() invalid time interval: " << kElapsedSeconds;
        return 0;
    }

    const int kCountDiff = kCurrentCount - last_read_count_;

    // Update state
    last_read_count_ = kCurrentCount;
    last_read_time_ = kCurrentTime;

    // Calculate RPM: (count_diff / rpm_factor) * (60 / elapsed_seconds)
    // rpm_factor is ticks per revolution
    const double kRevolutions = static_cast<double>(kCountDiff) / static_cast<double>(rpm_factor_);
    const double kRPM = (kRevolutions * 60.0) / kElapsedSeconds;

    // Scale RPM by 100 for precision BEFORE rounding to preserve decimal info
    const double kScaledRPM = kRPM * 100.0;
    int kRPMInt = static_cast<int>(std::round(kScaledRPM));

    if (!inverted_) {
        kRPMInt = -kRPMInt;
    }

    LOG_DEBUG << "Odometer::GetRPM() count_diff: " << kCountDiff << " elapsed_sec: " << kElapsedSeconds
              << " rpm: " << kRPMInt;

    return kRPMInt;
}
