/*
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2026 Aleksa Markovic <aleksamarkovic96@outlook.com>
 */
#include "utility/Frame.hpp"

#include <gtest/gtest.h>

TEST(Frame, Create)
{
  {
    types::Frame frame;
    cv::Mat matFrame = frame.mat();

    EXPECT_TRUE(matFrame.empty()) << "Mat frame should be empty";
    EXPECT_EQ(matFrame.rows, 0) << "Mat frame has incorrect number of rows";
    EXPECT_EQ(matFrame.cols, 0) << "Mat frame has incorrect number of columns";
    EXPECT_EQ(matFrame.step, 0) << "Mat frame has incorrect step size";
    EXPECT_EQ(frame.owner(), nullptr) << "Frame owner pointer should be nullptr";
  }

  {
    uint16_t buffer[50];
    buffer[0] = 23;
    buffer[1] = 42;

    types::Size size{ 5, 10 };
    size_t stride = 20;

    types::Frame frame(buffer, size, stride, std::make_shared<int>(1));
    cv::Mat matFrame = frame.mat();

    ASSERT_FALSE(matFrame.empty()) << "Mat frame should not be empty";
    ASSERT_EQ(matFrame.rows, static_cast<int>(frame.size().height)) << "Mat frame has incorrect number of rows";
    ASSERT_EQ(matFrame.cols, static_cast<int>(frame.size().width)) << "Mat frame has incorrect number of columns";
    EXPECT_EQ(matFrame.step, stride) << "Mat frame has incorrect step size";
    EXPECT_NE(frame.owner(), nullptr) << "Frame owner pointer is incorrect";

    EXPECT_EQ(matFrame.at<uint16_t>(0, 0), buffer[0]) << "Mat frame has incorrect value at (0, 0)";
    EXPECT_EQ(matFrame.at<uint16_t>(0, 1), buffer[1]) << "Mat frame has incorrect value at (0, 1)";
  }
}
