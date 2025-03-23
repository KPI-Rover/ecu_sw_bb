#include <gtest/gtest.h>

#include <atomic>
#include <chrono>
#include <thread>
#include <vector>

#include "../include/messageQueue.h"

class MessageQueueTest : public ::testing::Test {
   protected:
    MessageQueue queue;

    void SetUp() override {
        // Clear the queue before each test
        queue.Clear();
    }
};

// Test that pushing a message works
TEST_F(MessageQueueTest, PushTest) {
    std::vector<uint8_t> message = {1, 2, 3, 4};
    queue.Push(message);

    std::vector<uint8_t> received;
    bool result = queue.Pop(received, 0);

    EXPECT_TRUE(result);
    EXPECT_EQ(message, received);
}

// Test that popping from an empty queue returns false
TEST_F(MessageQueueTest, PopEmptyTest) {
    std::vector<uint8_t> received;
    bool result = queue.Pop(received, 10);

    EXPECT_FALSE(result);
}

// Test timeout behavior of Pop
TEST_F(MessageQueueTest, PopTimeoutTest) {
    auto start = std::chrono::high_resolution_clock::now();

    std::vector<uint8_t> received;
    bool result = queue.Pop(received, 100);  // 100ms timeout

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    EXPECT_FALSE(result);
    EXPECT_GE(duration, 90);  // Allow some small system timing variation
}

// Test that Pop returns immediately when message is available
TEST_F(MessageQueueTest, PopImmediateTest) {
    std::vector<uint8_t> message = {5, 6, 7, 8};
    queue.Push(message);

    auto start = std::chrono::high_resolution_clock::now();

    std::vector<uint8_t> received;
    bool result = queue.Pop(received, 1000);  // 1s timeout, but shouldn't wait

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    EXPECT_TRUE(result);
    EXPECT_EQ(message, received);
    EXPECT_LT(duration, 50);  // Should return almost immediately
}

// Test that Clear empties the queue
TEST_F(MessageQueueTest, ClearTest) {
    std::vector<uint8_t> message1 = {1, 2, 3};
    std::vector<uint8_t> message2 = {4, 5, 6};

    queue.Push(message1);
    queue.Push(message2);
    queue.Clear();

    std::vector<uint8_t> received;
    bool result = queue.Pop(received, 0);

    EXPECT_FALSE(result);
}

// Test FIFO behavior with multiple messages
TEST_F(MessageQueueTest, FIFOTest) {
    std::vector<uint8_t> message1 = {1, 2, 3};
    std::vector<uint8_t> message2 = {4, 5, 6};
    std::vector<uint8_t> message3 = {7, 8, 9};

    queue.Push(message1);
    queue.Push(message2);
    queue.Push(message3);

    std::vector<uint8_t> received1, received2, received3;

    queue.Pop(received1, 0);
    queue.Pop(received2, 0);
    queue.Pop(received3, 0);

    EXPECT_EQ(message1, received1);
    EXPECT_EQ(message2, received2);
    EXPECT_EQ(message3, received3);
}

// Test multithreaded Push and Pop operations
TEST_F(MessageQueueTest, ThreadedTest) {
    const int numMessages = 100;
    std::atomic<int> receivedCount(0);

    // Producer thread
    std::thread producer([this, numMessages]() {
        for (int i = 0; i < numMessages; i++) {
            std::vector<uint8_t> message = {static_cast<uint8_t>(i & 0xFF)};
            queue.Push(message);
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    });

    // Consumer thread
    std::thread consumer([this, numMessages, &receivedCount]() {
        for (int i = 0; i < numMessages; i++) {
            std::vector<uint8_t> received;
            if (queue.Pop(received, 200)) {
                receivedCount++;
            }
        }
    });

    producer.join();
    consumer.join();

    EXPECT_EQ(numMessages, receivedCount);
}
