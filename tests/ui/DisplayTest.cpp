/*
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2026 Aleksa Markovic <aleksamarkovic96@outlook.com>
 */
#include "ui/Display.hpp"

#include <QTest>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

class MockGPU : public hardware::IGPU
{
public:
  MOCK_METHOD(types::Status, open, (void*), (override));
  MOCK_METHOD(
    types::Status, render, (void*, const std::shared_ptr<void>&), (override)
  );
  MOCK_METHOD(types::Status, close, (), (override));
};

class DisplayTest : public ui::Display
{
public:
  void setInterface(std::unique_ptr<MockGPU> gpu)
  {
    ui::Display::setInterface(std::move(gpu));
  }
};

TEST(Display, Initialization)
{
  {
    auto engine = std::make_unique<MockGPU>();
    EXPECT_CALL(*engine, open).WillOnce(testing::Return(types::Status::FAIL));

    DisplayTest display;
    display.setInterface(std::move(engine));

    display.show();
    EXPECT_FALSE(QTest::qWaitForWindowExposed(&display, 50))
      << "Window should be closed after open failure";
  }

  {
    auto engine = std::make_unique<MockGPU>();
    EXPECT_CALL(*engine, open).WillOnce(testing::Return(types::Status::SUCCESS));
    EXPECT_CALL(*engine, close).Times(1);

    DisplayTest display;
    display.setInterface(std::move(engine));

    display.show();
    EXPECT_TRUE(QTest::qWaitForWindowExposed(&display, 50))
      << "Failed to expose the display window";
  }

  {
    using ::testing::AnyNumber;

    auto engine = std::make_unique<MockGPU>();
    EXPECT_CALL(*engine, open).WillOnce(testing::Return(types::Status::SUCCESS));
    EXPECT_CALL(*engine, render).Times(AnyNumber());
    EXPECT_CALL(*engine, close).Times(1);

    DisplayTest display;
    display.setInterface(std::move(engine));

    types::Frame frame{nullptr, {2028, 1080}, 1, std::make_shared<uint16_t>(1)};
    display.onNewFrame(std::make_shared<types::Frame>(std::move(frame)));

    display.show();
    EXPECT_TRUE(QTest::qWaitForWindowExposed(&display, 50))
      << "Failed to expose the display window";
  }
}
