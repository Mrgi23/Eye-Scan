/*
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2026 Aleksa Markovic <aleksamarkovic96@outlook.com>
 */
#pragma once

#include "utility/Enums.hpp"

#include <memory>

/**
 * @namespace hardware
 * @brief Hardware interfaces for low‑level device operations (Camera, DMA, LED,
 * GPU).
 */
namespace hardware
{
  /**
   * @namespace drivers
   * @brief Low-level hardware driver interfaces.
   */
  namespace drivers
  {
    /**
     * @class IGPUDriver
     * @brief Virtual interface for GPU hardware drivers.
     */
    class IGPUDriver
    {
    public:
      virtual ~IGPUDriver() = default;

      virtual void open(void*) = 0;
      virtual bool isOpen() const = 0;
      virtual bool addTexture(void*, const std::shared_ptr<void>&) = 0;
      virtual void draw() = 0;
      virtual void close() = 0;
    };
  } // namespace drivers

  class IGPU
  {
  public:
    virtual ~IGPU() = default;

    /**
     * @brief Open display pipeline and create window/surface.
     *
     * Initializes EGL display and context for rendering into external surfaces.
     *
     * @param void* Pointer to the display.
     * @return Status::SUCCESS on success,
     *         Status::FAIL if opening fails.
     */
    virtual types::Status open(void*) = 0;

    /**
     * @brief Render a single frame to the display window.
     *
     * Data is rendered directly from DMA buffers. The buffer must be a valid DMA
     * buffer set via setOwner().
     *
     * @param void* Pointer to the surface.
     * @param std::shared_ptr<void> Virtual memory owner of the DMA buffer.
     * @return Status::SUCCESS on success,
     *         Status::FAIL if rendering fails.
     */
    virtual types::Status render(void*, const std::shared_ptr<void>&) = 0;

    /**
     * @brief Close the display and release all resources.
     *
     * @return Status::SUCCESS on success,
     *         Status::FAIL if closing fails.
     */
    virtual types::Status close() = 0;
  };
} // namespace hardware
