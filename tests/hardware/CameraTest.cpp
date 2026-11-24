/*
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2026 Aleksa Markovic <aleksamarkovic96@outlook.com>
 */
#include "hardware/Camera.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

class MockCameraDriver : public hardware::drivers::ICameraDriver
{
  public:
    MOCK_METHOD(void, open, (const std::string&), (override));
    MOCK_METHOD(bool, isOpen, (), (const, override));
    MOCK_METHOD(void, connect, (bool), (override));
    MOCK_METHOD(void, configure, (const types::Size&, const hardware::CameraControls&), (override));
    MOCK_METHOD(bool, isConfigured, (), (const, override));
    MOCK_METHOD(void, start, (), (override));
    MOCK_METHOD(bool, capture, (types::Frame&), (override));
    MOCK_METHOD(void, stop, (), (override));
    MOCK_METHOD(bool, isActive, (), (const, override));
    MOCK_METHOD(void, close, (), (override));
};

class CameraTest : public hardware::Camera
{
  public:
    void setDriver(std::unique_ptr<MockCameraDriver> driver)
    {
      hardware::Camera::setDriver(std::move(driver));
    }
};

TEST(Camera, Open)
{
  using ::testing::Return;

  {
    auto driver = std::make_unique<MockCameraDriver>();
    EXPECT_CALL(*driver, isOpen).WillOnce(Return(true));

    CameraTest camera;
    camera.setDriver(std::move(driver));
    EXPECT_EQ(camera.open("imx477"), types::Status::SUCCESS) << "Camera did not open successfully";
  }

  {
    auto driver = std::make_unique<MockCameraDriver>();
    EXPECT_CALL(*driver, isOpen).WillOnce(Return(false));
    EXPECT_CALL(*driver, open).Times(1);

    CameraTest camera;
    camera.setDriver(std::move(driver));
    EXPECT_EQ(camera.open("imx477"), types::Status::SUCCESS) << "Camera did not open successfully";
  }
}

TEST(Camera, Configure)
{
  using ::testing::Return;

  {
    auto driver = std::make_unique<MockCameraDriver>();
    EXPECT_CALL(*driver, isOpen).WillOnce(Return(false));

    CameraTest camera;
    camera.setDriver(std::move(driver));
    EXPECT_EQ(
      camera.configure(types::Size{1920, 1080}, hardware::CameraControls{}),
      types::Status::FAIL
    ) << "Camera did not fail to configure";
  }

  {
    auto driver = std::make_unique<MockCameraDriver>();
    EXPECT_CALL(*driver, isOpen).WillOnce(Return(true));
    EXPECT_CALL(*driver, isActive).WillOnce(Return(true));

    CameraTest camera;
    camera.setDriver(std::move(driver));
    EXPECT_EQ(
      camera.configure(types::Size{1920, 1080}, hardware::CameraControls{}),
      types::Status::FAIL
    ) << "Camera did not fail to configure";
  }

  {
    auto driver = std::make_unique<MockCameraDriver>();
    EXPECT_CALL(*driver, isOpen).WillOnce(Return(true));
    EXPECT_CALL(*driver, isActive).WillOnce(Return(false));
    EXPECT_CALL(*driver, configure).Times(1);
    EXPECT_CALL(*driver, isConfigured).WillOnce(Return(false));

    CameraTest camera;
    camera.setDriver(std::move(driver));
    EXPECT_EQ(
      camera.configure(types::Size{1920, 1080}, hardware::CameraControls{}),
      types::Status::FAIL
    ) << "Camera did not fail to configure";
  }

  {
    auto driver = std::make_unique<MockCameraDriver>();
    EXPECT_CALL(*driver, isOpen).WillOnce(Return(true));
    EXPECT_CALL(*driver, isActive).WillOnce(Return(false));
    EXPECT_CALL(*driver, configure).Times(1);
    EXPECT_CALL(*driver, isConfigured).WillOnce(Return(true));

    CameraTest camera;
    camera.setDriver(std::move(driver));
    EXPECT_EQ(
      camera.configure(types::Size{1920, 1080}, hardware::CameraControls{}),
      types::Status::SUCCESS
    ) << "Camera did not configure successfully";
  }
}

TEST(Camera, Start)
{
  using ::testing::Return;

  {
    auto driver = std::make_unique<MockCameraDriver>();
    EXPECT_CALL(*driver, isOpen).WillOnce(Return(false));

    CameraTest camera;
    camera.setDriver(std::move(driver));
    EXPECT_EQ(camera.start(), types::Status::FAIL) << "Camera did not fail to start";
  }

  {
    auto driver = std::make_unique<MockCameraDriver>();
    EXPECT_CALL(*driver, isOpen).WillOnce(Return(true));
    EXPECT_CALL(*driver, isConfigured).WillOnce(Return(false));

    CameraTest camera;
    camera.setDriver(std::move(driver));
    EXPECT_EQ(camera.start(), types::Status::FAIL) << "Camera did not fail to start";
  }

  {
    auto driver = std::make_unique<MockCameraDriver>();
    EXPECT_CALL(*driver, isOpen).WillOnce(Return(true));
    EXPECT_CALL(*driver, isConfigured).WillOnce(Return(true));
    EXPECT_CALL(*driver, isActive).WillOnce(Return(true));

    CameraTest camera;
    camera.setDriver(std::move(driver));
    EXPECT_EQ(camera.start(), types::Status::SUCCESS) << "Camera did not start successfully";
  }

  {
    auto driver = std::make_unique<MockCameraDriver>();
    EXPECT_CALL(*driver, isOpen).WillOnce(Return(true));
    EXPECT_CALL(*driver, isConfigured).WillOnce(Return(true));
    EXPECT_CALL(*driver, isActive).Times(2).WillRepeatedly(Return(false));
    EXPECT_CALL(*driver, start).Times(1);

    CameraTest camera;
    camera.setDriver(std::move(driver));
    EXPECT_EQ(camera.start(), types::Status::FAIL) << "Camera did not fail to start";
  }

  {
    auto driver = std::make_unique<MockCameraDriver>();
    EXPECT_CALL(*driver, isOpen).WillOnce(Return(true));
    EXPECT_CALL(*driver, isConfigured).WillOnce(Return(true));
    EXPECT_CALL(*driver, isActive).Times(2).WillOnce(Return(false)).WillOnce(Return(true));
    EXPECT_CALL(*driver, start).Times(1);

    CameraTest camera;
    camera.setDriver(std::move(driver));
    EXPECT_EQ(camera.start(), types::Status::SUCCESS) << "Camera did not start successfully";
  }
}

TEST(Camera, Capture)
{
  using ::testing::Return;

  {
    auto driver = std::make_unique<MockCameraDriver>();
    EXPECT_CALL(*driver, isOpen).WillOnce(Return(false));

    CameraTest camera;
    camera.setDriver(std::move(driver));

    types::Frame frame;
    EXPECT_EQ(camera.capture(frame), types::Status::FAIL) << "Camera did not fail to capture";
  }

  {
    auto driver = std::make_unique<MockCameraDriver>();
    EXPECT_CALL(*driver, isOpen).WillOnce(Return(true));
    EXPECT_CALL(*driver, isConfigured).WillOnce(Return(false));

    CameraTest camera;
    camera.setDriver(std::move(driver));

    types::Frame frame;
    EXPECT_EQ(camera.capture(frame), types::Status::FAIL) << "Camera did not fail to capture";
  }

  {
    auto driver = std::make_unique<MockCameraDriver>();
    EXPECT_CALL(*driver, isOpen).WillOnce(Return(true));
    EXPECT_CALL(*driver, isConfigured).WillOnce(Return(true));
    EXPECT_CALL(*driver, isActive).WillOnce(Return(false));

    CameraTest camera;
    camera.setDriver(std::move(driver));

    types::Frame frame;
    EXPECT_EQ(camera.capture(frame), types::Status::FAIL) << "Camera did not fail to capture";
  }

  {
    auto driver = std::make_unique<MockCameraDriver>();
    EXPECT_CALL(*driver, isOpen).WillOnce(Return(true));
    EXPECT_CALL(*driver, isConfigured).WillOnce(Return(true));
    EXPECT_CALL(*driver, isActive).WillOnce(Return(true));
    EXPECT_CALL(*driver, capture).WillOnce(Return(false));

    CameraTest camera;
    camera.setDriver(std::move(driver));

    types::Frame frame;
    EXPECT_EQ(camera.capture(frame), types::Status::FAIL) << "Camera did not fail to capture";
  }

  {
    auto driver = std::make_unique<MockCameraDriver>();
    EXPECT_CALL(*driver, isOpen).WillOnce(Return(true));
    EXPECT_CALL(*driver, isConfigured).WillOnce(Return(true));
    EXPECT_CALL(*driver, isActive).WillOnce(Return(true));
    EXPECT_CALL(*driver, capture).WillOnce(Return(true));

    CameraTest camera;
    camera.setDriver(std::move(driver));

    types::Frame frame;
    EXPECT_EQ(camera.capture(frame), types::Status::SUCCESS) << "Camera did not capture successfully";
  }
}

TEST(Camera, Stop)
{
  using ::testing::Return;

  {
    auto driver = std::make_unique<MockCameraDriver>();
    EXPECT_CALL(*driver, isOpen).WillOnce(Return(false));

    CameraTest camera;
    camera.setDriver(std::move(driver));
    EXPECT_EQ(camera.stop(), types::Status::FAIL) << "Camera did not fail to stop";
  }

  {
    auto driver = std::make_unique<MockCameraDriver>();
    EXPECT_CALL(*driver, isOpen).WillOnce(Return(true));
    EXPECT_CALL(*driver, isActive).WillOnce(Return(false));

    CameraTest camera;
    camera.setDriver(std::move(driver));
    EXPECT_EQ(camera.stop(), types::Status::SUCCESS) << "Camera did not stop successfully";
  }

  {
    auto driver = std::make_unique<MockCameraDriver>();
    EXPECT_CALL(*driver, isOpen).WillOnce(Return(true));
    EXPECT_CALL(*driver, isActive).WillOnce(Return(true));
    EXPECT_CALL(*driver, stop).Times(1);

    CameraTest camera;
    camera.setDriver(std::move(driver));
    EXPECT_EQ(camera.stop(), types::Status::SUCCESS) << "Camera did not stop successfully";
  }
}

TEST(Camera, Close)
{
  using ::testing::Return;

  {
    auto driver = std::make_unique<MockCameraDriver>();
    EXPECT_CALL(*driver, isOpen).WillOnce(Return(false));

    CameraTest camera;
    camera.setDriver(std::move(driver));
    EXPECT_EQ(camera.close(), types::Status::SUCCESS) << "Camera did not close successfully";
  }

  {
    auto driver = std::make_unique<MockCameraDriver>();
    EXPECT_CALL(*driver, isOpen).WillOnce(Return(true));
    EXPECT_CALL(*driver, isActive).WillOnce(Return(true));

    CameraTest camera;
    camera.setDriver(std::move(driver));
    EXPECT_EQ(camera.close(), types::Status::FAIL) << "Camera did not fail to close";
  }

  {
    auto driver = std::make_unique<MockCameraDriver>();
    EXPECT_CALL(*driver, isOpen).WillOnce(Return(true));
    EXPECT_CALL(*driver, isActive).WillOnce(Return(false));
    EXPECT_CALL(*driver, close).Times(1);

    CameraTest camera;
    camera.setDriver(std::move(driver));
    EXPECT_EQ(camera.close(), types::Status::SUCCESS) << "Camera did not close successfully";
  }
}
