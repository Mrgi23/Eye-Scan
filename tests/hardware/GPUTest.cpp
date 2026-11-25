/*
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2026 Aleksa Markovic <aleksamarkovic96@outlook.com>
 */
#include "hardware/GPU.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

class MockGPUDriver : public hardware::drivers::IGPUDriver
{
public:
  MOCK_METHOD(void, open, (void*), (override));
  MOCK_METHOD(bool, isOpen, (), (const, override));
  MOCK_METHOD(bool, addTexture, (void*, const std::shared_ptr<void>&), (override));
  MOCK_METHOD(void, draw, (), (override));
  MOCK_METHOD(void, close, (), (override));
};

class GPUTest : public hardware::GPU
{
public:
  void setDriver(std::unique_ptr<MockGPUDriver> driver)
  {
    hardware::GPU::setDriver(std::move(driver));
  }
};

TEST(GPU, Open)
{
  using ::testing::Return;

  {
    auto driver = std::make_unique<MockGPUDriver>();
    EXPECT_CALL(*driver, isOpen).WillOnce(Return(true));

    GPUTest gpu;
    gpu.setDriver(std::move(driver));
    EXPECT_EQ(gpu.open(nullptr), types::Status::SUCCESS)
      << "GPU did not open successfully";
  }

  {
    auto driver = std::make_unique<MockGPUDriver>();
    EXPECT_CALL(*driver, isOpen).Times(2).WillRepeatedly(Return(false));
    EXPECT_CALL(*driver, open).Times(1);
    EXPECT_CALL(*driver, close).Times(1);

    GPUTest gpu;
    gpu.setDriver(std::move(driver));
    EXPECT_EQ(gpu.open(nullptr), types::Status::FAIL) << "GPU did not fail to open";
  }

  {
    auto driver = std::make_unique<MockGPUDriver>();
    EXPECT_CALL(*driver, isOpen)
      .Times(2)
      .WillOnce(Return(false))
      .WillOnce(Return(true));
    EXPECT_CALL(*driver, open).Times(1);

    GPUTest gpu;
    gpu.setDriver(std::move(driver));
    EXPECT_EQ(gpu.open(nullptr), types::Status::SUCCESS)
      << "GPU did not open successfully";
  }
}

TEST(GPU, Render)
{
  using ::testing::Return;

  {
    auto driver = std::make_unique<MockGPUDriver>();
    EXPECT_CALL(*driver, isOpen).WillOnce(Return(false));

    GPUTest gpu;
    gpu.setDriver(std::move(driver));
    EXPECT_EQ(gpu.render(nullptr, {}), types::Status::FAIL)
      << "GPU did not fail to render";
  }

  {
    auto driver = std::make_unique<MockGPUDriver>();
    EXPECT_CALL(*driver, isOpen).WillOnce(Return(true));
    EXPECT_CALL(*driver, addTexture).WillOnce(Return(false));

    GPUTest gpu;
    gpu.setDriver(std::move(driver));
    EXPECT_EQ(gpu.render(nullptr, {}), types::Status::FAIL)
      << "GPU did not fail to render";
  }

  {
    auto driver = std::make_unique<MockGPUDriver>();
    EXPECT_CALL(*driver, isOpen).WillOnce(Return(true));
    EXPECT_CALL(*driver, addTexture).WillOnce(Return(true));
    EXPECT_CALL(*driver, draw).Times(1);

    GPUTest gpu;
    gpu.setDriver(std::move(driver));
    EXPECT_EQ(gpu.render(nullptr, {}), types::Status::SUCCESS)
      << "GPU did not render successfully";
  }
}

TEST(GPU, Close)
{
  using ::testing::Return;

  {
    auto driver = std::make_unique<MockGPUDriver>();
    EXPECT_CALL(*driver, isOpen).WillOnce(Return(false));

    GPUTest gpu;
    gpu.setDriver(std::move(driver));
    EXPECT_EQ(gpu.close(), types::Status::SUCCESS)
      << "GPU did not close successfully";
  }

  {
    auto driver = std::make_unique<MockGPUDriver>();
    EXPECT_CALL(*driver, isOpen).WillOnce(Return(true));
    EXPECT_CALL(*driver, close).Times(1);

    GPUTest gpu;
    gpu.setDriver(std::move(driver));
    EXPECT_EQ(gpu.close(), types::Status::SUCCESS)
      << "GPU did not close successfully";
  }
}
