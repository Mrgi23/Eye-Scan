/*
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2026 Aleksa Markovic <aleksamarkovic96@outlook.com>
 */
#include "utility/Frame.hpp"
#include "utility/Templates.hpp"

#include <gtest/gtest.h>

static types::Frame dummy()
{
  std::shared_ptr<uint16_t[]> buffer(new uint16_t[1]);
  return types::Frame{ buffer.get(), { 1, 1 }, 2, buffer };
}

TEST(Queue, Push)
{
  types::Queue<types::Frame, 4> queue;
  EXPECT_EQ(queue.push(std::move(dummy())), types::Status::SUCCESS) <<  "Push failed unexpectedly";
  EXPECT_EQ(queue.push(std::move(dummy())), types::Status::SUCCESS) <<  "Push failed unexpectedly";
  EXPECT_EQ(queue.push(std::move(dummy())), types::Status::SUCCESS) <<  "Push failed unexpectedly";
  EXPECT_EQ(queue.push(std::move(dummy())), types::Status::SUCCESS) <<  "Push failed unexpectedly";
  EXPECT_EQ(queue.push(std::move(dummy())), types::Status::FAIL) <<  "Push should have failed due to full queue";
}

TEST(Queue, Pop)
{
  types::Queue<types::Frame, 4> queue;
  queue.push(std::move(dummy()));
  queue.push(std::move(dummy()));
  queue.push(std::move(dummy()));
  queue.push(std::move(dummy()));

  types::Frame value;
  EXPECT_EQ(queue.pop(value), types::Status::SUCCESS) <<  "Pop failed unexpectedly";
  EXPECT_NE(value.owner(), nullptr) <<  "Pop returned incorrect value";

  EXPECT_EQ(queue.pop(value), types::Status::SUCCESS) <<  "Pop failed unexpectedly";
  EXPECT_NE(value.owner(), nullptr) <<  "Pop returned incorrect value";

  EXPECT_EQ(queue.pop(value), types::Status::SUCCESS) <<  "Pop failed unexpectedly";
  EXPECT_NE(value.owner(), nullptr) <<  "Pop returned incorrect value";

  EXPECT_EQ(queue.pop(value), types::Status::SUCCESS) <<  "Pop failed unexpectedly";
  EXPECT_NE(value.owner(), nullptr) <<  "Pop returned incorrect value";

  EXPECT_EQ(queue.pop(value), types::Status::FAIL) <<  "Pop should have failed due to empty queue";
}

TEST(Queue, Clear)
{
  types::Queue<types::Frame, 4> queue;
  queue.push(std::move(dummy()));
  queue.push(std::move(dummy()));
  queue.push(std::move(dummy()));
  queue.push(std::move(dummy()));
  queue.clear();

  types::Frame value;
  EXPECT_EQ(queue.pop(value), types::Status::FAIL) <<  "Pop should have failed due to empty queue after clear";
}
