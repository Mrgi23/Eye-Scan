/*
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2026 Aleksa Markovic <aleksamarkovic96@outlook.com>
 */
#pragma once

#include "drivers/GPUDriver.hpp"
#include "utility/Macros.hpp"

/**
 * @namespace hardware
 * @brief Hardware interfaces for low‑level device operations (Camera, DMA, LED,
 * GPU).
 */
namespace hardware
{
  /**
   * @class GPU
   * @brief High‑level GPU interface for controlling rendering and display
   * operations.
   */
  class GPU : public IGPU
  {
  public:
    /**
     * @brief Construct new GPU object.
     */
    GPU() noexcept;

    /**
     * @brief Destroy the GPU object.
     */
    ~GPU() override;

    DISABLE_COPY_AND_MOVE(GPU)

    /**
     * @brief Open display pipeline and create window/surface.
     *
     * Initializes EGL display and context for rendering into external surfaces.
     *
     * @param void* Pointer to the display.
     * @return Status::SUCCESS on success,
     *         Status::FAIL if opening fails.
     */
    types::Status open(void*) override;

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
    types::Status render(void*, const std::shared_ptr<void>&) override;

    /**
     * @brief Close the display and release all resources.
     *
     * @return Status::SUCCESS on success,
     *         Status::FAIL if closing fails.
     */
    types::Status close() override;

  protected:
    /**
     * @brief Set the GPU driver for this GPU object.
     *
     * @param std::unique_ptr<drivers::IGPUDriver> Unique pointer to the GPU driver.
     */
    void setDriver(std::unique_ptr<drivers::IGPUDriver>);

  private:
    std::unique_ptr<drivers::IGPUDriver> m_driver;
  };
} // namespace hardware
