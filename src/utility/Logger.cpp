/*
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2026 Aleksa Markovic <aleksamarkovic96@outlook.com>
 */
#include "utility/Logger.hpp"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <mutex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

namespace logger
{
  std::mutex s_logCallbackMutex;
  LogCallback s_logCallback;

  static void log(std::string_view level, std::string_view message)
  {
    std::scoped_lock lock(s_logCallbackMutex);
    if (!s_logCallback)
      return;

    const auto now = std::chrono::system_clock::now();
    const auto time = std::chrono::system_clock::to_time_t(now);
    std::tm localTime {};
    localtime_r(&time, &localTime);

    std::ostringstream timestamp;
    timestamp << std::put_time(&localTime, "%H:%M:%S");
    const std::string fullMessage = "[" + timestamp.str() + "] [" + std::string(level) + "] " + std::string(message);

    s_logCallback(fullMessage);
  }

  void SetLogCallback(LogCallback callback)
  {
    std::scoped_lock lock(s_logCallbackMutex);
    s_logCallback = std::move(callback);
  }

  types::Status Info(std::string_view message)
  {
    log("INFO", message);
    return types::Status::SUCCESS;
  }

  types::Status Warning(std::string_view message)
  {
    log("WARNING", message);
    return types::Status::FAIL;
  }

  [[noreturn]] void Error(std::string_view message, types::Error error)
  {
    log("ERROR", message);

    switch (error)
    {
      case types::Error::INVALID_ARGUMENT:
        throw std::invalid_argument(std::string(message));
      case types::Error::OUT_OF_RANGE:
        throw std::out_of_range(std::string(message));
      case types::Error::RUNTIME_ERROR:
        throw std::runtime_error(std::string(message));
    }
    std::terminate(); // LCOV_EXCL_LINE
  }
} // namespace logger
