/*
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2026 Aleksa Markovic <aleksamarkovic96@outlook.com>
 */
#pragma once

/**
 * @namespace types
 * @brief Lightweight fundamental enums and utility objects used across the system.
 */
namespace types
{
  /**
   * @enum Status
   * @brief Success or failure of an operation.
   */
  enum class Status
  {
    FAIL,
    SUCCESS
  };

  /**
   * @enum Error
   * @brief Categorized error types for exception‑throwing operations.
   */
  enum class Error
  {
    INVALID_ARGUMENT,
    OUT_OF_RANGE,
    RUNTIME_ERROR
  };
} // namespace types
