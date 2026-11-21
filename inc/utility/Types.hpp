/*
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2026 Aleksa Markovic <aleksamarkovic96@outlook.com>
 */
#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>

/**
 * @namespace types
 * @brief Lightweight fundamental enums and utility objects used across the system.
 */
namespace types
{
  /**
   * @struct CameraControls
   * @brief Camera control parameters.
   *
   * @param uint32_t Desired frames per second for the camera stream. Must be > 0.
   * @param std::optional<bool> Optional flag to enable/disable auto-exposure. If not set, defaults to enabled.
   * @param std::optional<int32_t> Optional manual exposure time in microseconds. If auto-exposure is enabled, this value is ignored.
   * @param std::optional<float> Optional manual analogue gain. If auto-exposure is enabled, this value is ignored.
   */
  struct CameraControls
  {
    uint32_t fps = 30;
    std::optional<bool> autoExposure;
    std::optional<int32_t> exposureTime;
    std::optional<float> analogueGain;
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
