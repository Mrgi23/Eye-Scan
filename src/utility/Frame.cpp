/*
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2026 Aleksa Markovic
 */
#include "utility/Frame.hpp"

#include <opencv2/imgproc.hpp>

namespace types
{
  Frame::Frame(uint16_t* pixels, const Size& size, size_t stride,  std::shared_ptr<void> owner) noexcept
    : m_pixels(pixels), m_size(size), m_stride(stride), m_owner(std::move(owner))
  {
  }

  cv::Mat Frame::mat() const
  {
    if (!m_pixels)
      return cv::Mat();

    cv::Mat image(
      static_cast<int>(m_size.height),
      static_cast<int>(m_size.width),
      CV_16UC1,
      const_cast<uint16_t*>(m_pixels),
      m_stride
    );

    cv::Mat result;
    cv::cvtColor(image, result, cv::COLOR_BayerBG2GRAY);
    return result;
  }

  Size Frame::size() const noexcept
  {
    return m_size;
  }

  const std::shared_ptr<void>& Frame::owner() const noexcept
  {
    return m_owner;
  }
} // namespace types
