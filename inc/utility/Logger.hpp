/*
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2026 Aleksa Markovic <aleksamarkovic96@outlook.com>
 */
#pragma once

#include "utility/Enums.hpp"

#include <functional>
#include <string_view>

/**
 * @namespace logger
 * @brief Lightweight logging utilities for informational, warning, and error messages.
 */
namespace logger
{
  /**
   * @brief Type alias for the log callback function.
   */
  using LogCallback = std::function<void(const std::string&)>;

  /**
   * @brief Set the callback function for handling log messages.
   *
   * @param callback Function to be called for each log message.
   */
  void SetLogCallback(LogCallback callback);

  /**
   * @brief Log an informational message.
   *
   * @param std::string_view Text to be logged.
   * @return types::Status::SUCCESS Always returned to indicate successful logging.
   */
  types::Status Info(std::string_view);

  /**
   * @brief Log a warning message.
   *
   * @param std::string_view Text to be logged.
   * @return types::Status::FAIL Always returned to indicate a warning condition.
   */
  types::Status Warning(std::string_view);

  /**
   * @brief Log an error message and throw a typed exception.
   *
   * @param std::string_view Text to be logged before throwing.
   * @param types::Error Error category determining which exception type is thrown.
   * @throw std::invalid_argument when error == types::Error::INVALID_ARGUMENT.
   * @throw std::out_of_range when error == types::Error::OUT_OF_RANGE.
   * @throw std::runtime_error when error == types::Error::RUNTIME_ERROR.
   */
  [[noreturn]] void Error(std::string_view, types::Error);
} // namespace logger
