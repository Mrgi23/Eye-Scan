/*
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2026 Aleksa Markovic <aleksamarkovic96@outlook.com>
 */
#pragma once

#include <cstddef>
#include <cstdint>

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

  /**
   * @struct VirtualMemory
   * @brief Representation of a virtual memory block.
   *
   * @param int32_t File descriptor associated with the memory block.
   * @param types::Size The size of the memory block.
   * @param size_t The length of the memory block.
   * @param size_t The stride of the memory block.
   * @param uint64_t The offset within the memory block.
   */
  struct VirtualMemory
  {
    types::Size size;
    size_t length = 0;
    size_t stride = 0;
    uint64_t offset = 0;
    int fd = -1;
  };
} // namespace types
