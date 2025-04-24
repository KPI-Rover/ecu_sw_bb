#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <vector>
#include "IMUController.h"

// Mock libraray for rc/mpu functionality
#include "mocks/librobotcontrol/include/mock_rc_imu.h"

class IMUControllerTest : public ::testing::Test {
   protected:
    IMUController imu_controller;

    void SetUp() override {
        // Reset Mock for MPU
        ResetMockRCMPU();

        // Set up default behaviors for mocks
        ON_CALL(GetMockRCMPU(), initialize_dmp(::testing::_, ::testing::_)).WillByDefault(::testing::Return(0));
        ON_CALL(GetMockRCMPU(), power_off()).WillByDefault(::testing::Return());
        ON_CALL(GetMockRCMPU(), read_accel(::testing::_)).WillByDefault(::testing::Return(0));
        ON_CALL(GetMockRCMPU(), read_gyro(::testing::_)).WillByDefault(::testing::Return(0));
    }

    void TearDown() override {
        // Clean up mock expectations
        EXPECT_CALL(GetMockRCMPU(), power_off()).Times(1);
        imu_controller.Stop();

        // Destroy Mock
        DestroyMockRCMPU();
    }
};

// Test IMUController::Init
TEST_F(IMUControllerTest, InitSuccess) {
    EXPECT_CALL(GetMockRCMPU(), initialize_dmp(::testing::_, ::testing::_)).WillOnce(::testing::Return(0));
    int result = imu_controller.Init();
    ASSERT_EQ(result, 0);
}

TEST_F(IMUControllerTest, InitFailure) {
    EXPECT_CALL(GetMockRCMPU(), initialize_dmp(::testing::_, ::testing::_)).WillOnce(::testing::Return(-1));
    int result = imu_controller.Init();
    ASSERT_EQ(result, -1);
}

// Test IMUController::GetData
TEST_F(IMUControllerTest, GetDataWhenDisabled) {
    imu_controller.SetDisable();
    std::vector<float> data = imu_controller.GetData();
    ASSERT_TRUE(data.empty());
}

TEST_F(IMUControllerTest, GetDataWhenEnabled) {
    imu_controller.SetEnable();

    // Настройка моков для чтения данных акселерометра, гироскопа, кватернионов
    EXPECT_CALL(GetMockRCMPU(), read_accel(::testing::_)).WillOnce(::testing::Return(0));
    EXPECT_CALL(GetMockRCMPU(), read_gyro(::testing::_)).WillOnce(::testing::Return(0));

    // Получение данных
    std::vector<float> data = imu_controller.GetData();

    // Проверка того, что данные возвращены
    ASSERT_EQ(data.size(), 10);  // Убедимся, что размер данных соответствует ожидаемому
    SUCCEED();  // Успешное завершение теста
}

// Test IMUController::Stop
TEST_F(IMUControllerTest, Stop) {
    EXPECT_CALL(GetMockRCMPU(), power_off()).Times(1);
    imu_controller.Stop();
}

// Test IMUController::GetId
TEST_F(IMUControllerTest, GetId) {
    uint8_t id = imu_controller.GetId();
    ASSERT_EQ(id, 0x06);
}
