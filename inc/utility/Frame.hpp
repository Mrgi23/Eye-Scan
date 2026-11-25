/*
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2026 Aleksa Markovic <aleksamarkovic96@outlook.com>
 */
#pragma once

#include "utility/Macros.hpp"
#include "utility/Types.hpp"

#include <opencv2/core/mat.hpp>

#include <memory>

/**
 * @namespace types
 * @brief Lightweight fundamental enums and utility objects used across the system.
 */
namespace types
{
  /**
   * @class Frame
   * @brief 2D data buffer with size, stride, and memory ownership.
   */
  class Frame
  {
  public:
    /**
     * @brief Construct new Frame object.
     */
    Frame() noexcept = default;

    /**
     * @brief Construct new Frame object.
     *
     * @param uint16_t* The raw pixel data.
     * @param Size The size of the frame.
     * @param size_t The stride (number of bytes per row).
     * @param std::shared_ptr<void> The owner of the pixel data.
     */
    Frame(uint16_t*, const Size&, size_t, std::shared_ptr<void>) noexcept;

    /**
     * @brief Move‑construct new Frame object.
     *
     * @param Frame&& The Frame object to move from.
     */
    Frame(Frame&& other) noexcept = default;

    /**
     * @brief Move‑assign from another Frame.
     *
     * @param Frame&& The Frame object to move from.
     */
    Frame& operator=(Frame&& other) noexcept = default;

    /**
     * @brief Destroy the Frame object.
     */
    ~Frame() noexcept = default;

    DISABLE_COPY(Frame)

    /**
     * @brief Get the OpenCV Mat representation of the frame.
     *
     * @return OpenCV Mat object.
     */
    cv::Mat mat() const;

    /**
     * @brief Get the frame size.
     *
     * @return Current size.
     */
    Size size() const noexcept;

    /**
     * @brief Get the owner of the data of this frame (type-erased).
     *
     * @return Const reference to the shared pointer to owner.
     */
    const std::shared_ptr<void>& owner() const noexcept;

  private:
    uint16_t* m_pixels = nullptr;
    size_t m_stride = 0;
    Size m_size;
    std::shared_ptr<void> m_owner;
  };
} // namespace types
