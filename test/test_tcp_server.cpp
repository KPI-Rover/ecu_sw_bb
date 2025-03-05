#include <gtest/gtest.h>
#include "TCPServerReceiver.h"
#include "motorsProcessor.h"

class TCPServerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code that will be called before each test
    }

    void TearDown() override {
        // Cleanup code that will be called after each test
    }
};

TEST_F(TCPServerTest, InitializationTest) {
    MotorProcessor motorProc;
    int motors[] = {1, 2, 3, 4};
    EXPECT_EQ(motorProc.init(motors), 0);
    motorProc.destroy();
}

TEST_F(TCPServerTest, MotorControlTest) {
    MotorProcessor motorProc;
    int motors[] = {1, 2, 3, 4};
    EXPECT_EQ(motorProc.init(motors), 0);
    
    // Test setting motor RPM
    EXPECT_EQ(motorProc.setMotorRPM(0, 100), 0);
    EXPECT_EQ(motorProc.getMotorRPM(0), 0); // Initial read should be 0 as we reset counter
    
    // Test stopping motor
    EXPECT_EQ(motorProc.stopMotor(0), 0);
    
    motorProc.destroy();
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
