/*
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2026 Aleksa Markovic <aleksamarkovic96@outlook.com>
 */
#pragma once

#include <bitset>
#include <cstddef>

constexpr size_t BITSET_SIZE = 2048;

/**
 * @namespace types
 * @brief Lightweight fundamental enums and utility objects used across the system.
 */
namespace types
{
  /**
   * @struct EyeCode
   * @brief Encoded representation of the eye features.
   *
   * @param std::bitset<BITSET_SIZE> The encoded data.
   * @param std::bitset<BITSET_SIZE> The mask indicating valid bits.
   */
  struct EyeCode
  {
    std::bitset<BITSET_SIZE> data;
    std::bitset<BITSET_SIZE> mask = std::bitset<BITSET_SIZE>().set();
  };

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
