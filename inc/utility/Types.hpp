/*
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2026 Aleksa Markovic <aleksamarkovic96@outlook.com>
 */
#pragma once

#include <cstddef>

/**
 * @namespace types
 * @brief Lightweight fundamental enums and utility objects used across the system.
 */
namespace types
{
  /**
   * @struct Size
   * @brief Two‑dimensional extent.
   *
   * @param size_t Horizontal extent.
   * @param size_t Vertical extent.
   */
  struct Size
  {
    size_t width = 0;
    size_t height = 0;
  };
} // namespace types
