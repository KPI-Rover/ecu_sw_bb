#ifndef MESSAGEQUEUE_H
#define MESSAGEQUEUE_H

#include "config.h"

class MessageQueue {
   public:
    void Push(const vector<uint8_t>& msg);
    bool Pop(vector<uint8_t>& msg, int timeout_ms);
    void Clear();

   private:
    queue<vector<uint8_t>> queue_;
    pthread_mutex_t mutex_ = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t cv_ = PTHREAD_COND_INITIALIZER;
    // mutex mutex_;
    // condition_variable cv_;
};

#endif