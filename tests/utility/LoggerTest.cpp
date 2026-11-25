/*
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2026 Aleksa Markovic <aleksamarkovic96@outlook.com>
 */
#include "utility/Logger.hpp"

#include <gtest/gtest.h>

TEST(Logger, Info)
{
  std::string msg;
  logger::SetLogCallback([&msg](const std::string& logMsg) { msg = logMsg; });

  types::Status rc = logger::Info("OK");
  EXPECT_EQ(rc, types::Status::SUCCESS) << "Logger Info did not return SUCCESS";
  EXPECT_FALSE(msg.empty()) << "Logger did not set the message correctly";
  EXPECT_NE(msg.find("[INFO]"), std::string::npos)
    << "Logger message does not contain the expected log level";
  EXPECT_NE(msg.find("OK"), std::string::npos)
    << "Logger message does not contain the expected text";
}

TEST(Logger, Warning)
{
  std::string msg;
  logger::SetLogCallback([&msg](const std::string& logMsg) { msg = logMsg; });

  types::Status rc = logger::Warning("Careful");

  EXPECT_EQ(rc, types::Status::FAIL) << "Logger Warning did not return FAIL";
  EXPECT_FALSE(msg.empty()) << "Logger did not set the message correctly";
  EXPECT_NE(msg.find("[WARNING]"), std::string::npos)
    << "Logger message does not contain the expected log level";
  EXPECT_NE(msg.find("Careful"), std::string::npos)
    << "Logger message does not contain the expected text";
}

TEST(Logger, Error)
{
  std::string msg;
  logger::SetLogCallback([&msg](const std::string& logMsg) { msg = logMsg; });

  EXPECT_THROW(
    logger::Error("Invalid argument", types::Error::INVALID_ARGUMENT),
    std::invalid_argument
  ) << "Logger Error did not throw the expected exception";
  EXPECT_FALSE(msg.empty()) << "Logger did not set the message correctly";
  EXPECT_NE(msg.find("[ERROR]"), std::string::npos)
    << "Logger message does not contain the expected log level";
  EXPECT_NE(msg.find("Invalid argument"), std::string::npos)
    << "Logger message does not contain the expected text";

  EXPECT_THROW(
    logger::Error("Out of range", types::Error::OUT_OF_RANGE), std::out_of_range
  ) << "Logger Error did not throw the expected exception for another error";

  logger::SetLogCallback(nullptr);
  msg.clear();
  EXPECT_THROW(logger::Error("", types::Error::RUNTIME_ERROR), std::runtime_error)
    << "Logger Error did not throw the expected exception for another error";
  EXPECT_TRUE(msg.empty()
  ) << "Logger message is empty due to the log callback being set to nullptr";
}
