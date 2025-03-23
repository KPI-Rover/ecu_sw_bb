#ifndef MESSAGEQUEUE_H
#define MESSAGEQUEUE_H

#include <pthread.h>

#include <cstdint>
#include <queue>
#include <vector>

class MessageQueue {
   public:
    void Push(const std::vector<uint8_t>& msg);
    bool Pop(std::vector<uint8_t>& msg, int timeout_ms);
    void Clear();

   private:
    static constexpr int kMicrosecondsPerSecond = 1000000;
    static constexpr int kMillisecondsPerSecond = 1000;

    std::queue<std::vector<uint8_t>> queue_;
    pthread_mutex_t mutex_ = PTHREAD_MUTEX_INITIALIZER;  // NOLINT(misc-include-cleaner)
    pthread_cond_t cv_ = PTHREAD_COND_INITIALIZER;       // NOLINT(misc-include-cleaner)
};

#endif