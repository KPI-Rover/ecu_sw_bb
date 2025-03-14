#include "messageQueue.h"

#include "config.h"
#include <queue>
#include <iostream>
#include <time.h>

void MessageQueue::Push(const vector<uint8_t>& msg) {
    pthread_mutex_lock(&mutex_);
    queue_.push(msg);
    pthread_cond_signal(&cv_);
    pthread_mutex_unlock(&mutex_);
}

bool MessageQueue::Pop(vector<uint8_t>& msg, int timeout_ms) {
    // this_thread::sleep_for(chrono::milliseconds(timeout_ms));
    pthread_mutex_lock(&mutex_);

    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += timeout_ms / 1000;
    ts.tv_nsec += (timeout_ms % 1000) * ONESECONDMICRO;
    if (ts.tv_nsec >= 1000000000) {
        ts.tv_sec++;
        ts.tv_nsec -= 1000000000;
    }

    while (queue_.empty()) {
        if (pthread_cond_timedwait(&cv_, &mutex_, &ts) == ETIMEDOUT) {
            pthread_mutex_unlock(&mutex_);
            return false;
        }
    }

    msg = queue_.front();
    queue_.pop();
    pthread_mutex_unlock(&mutex_);

    return true;
}

void MessageQueue::Clear() {
    pthread_mutex_lock(&mutex_);
    std::queue<std::vector<uint8_t>> empty;
    std::swap(queue_, empty);
    pthread_mutex_unlock(&mutex_);
}