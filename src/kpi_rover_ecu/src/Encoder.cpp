#include "Encoder.h"

#include <rc/encoder.h>

#include "loggingIncludes.h"

Encoder::Encoder(int channel) : channel_(channel) { LOG_DEBUG << "Encoder::Encoder(" << channel << ")"; }

int Encoder::GetCount() {
    std::lock_guard<std::mutex> lock(count_mutex_);
    LOG_DEBUG << "Encoder::GetCount() channel " << channel_;

    const int kEncoderTicks = rc_encoder_read(channel_);

    LOG_DEBUG << "Encoder::GetCount() channel " << channel_ << " returns: " << kEncoderTicks;
    return kEncoderTicks;
}
