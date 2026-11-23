/*
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2026 Aleksa Markovic <aleksamarkovic96@outlook.com>
 */
#pragma once

#include "drivers/CameraDriver.hpp"
#include "utility/Macros.hpp"

/**
 * @namespace hardware
 * @brief Hardware interfaces for low‑level device operations (Camera, DMA, LED, GPU).
 */
namespace hardware
{
  /**
   * @class Camera
   * @brief High-Level camera interface for streaming and capturing frames.
   */
  class Camera : public ICamera
  {
    public:
      /**
       * @brief Construct new Camera object.
       */
      Camera() noexcept;

      /**
       * @brief Destroy the Camera object.
       */
      ~Camera() override;

      DISABLE_COPY_AND_MOVE(Camera)

      /**
       * @brief Initialize a camera by name.
       *
       * Starts the camera manager, locates the requested camera, acquires it,
       * and connects the request completion callback.
       *
       * @param std::string Camera device name.
       * @return Status::SUCCESS on success,
       *         Status::FAIL on recoverable errors.
       * @throw std::runtime_error on critical failures (camera manager start failure or camera acquisition failure).
       * @throw std::invalid_argument when the specified camera device is not found.
       */
      types::Status open(const std::string&) override;

      /**
       * @brief Configure the camera stream and controls.
       *
       * Applies stream size, validates the configuration,
       * configures the camera, and applies control settings such as FPS,
       * exposure, gain, and white balance.
       *
       * @param types::Size Desired stream size.
       * @param types::CameraControls Control configuration parameters.
       * @return Status::SUCCESS on success,
       *         Status::FAIL if configuration fails.
       */
      types::Status configure(const types::Size&, const types::CameraControls&) override;

      /**
       * @brief Start camera streaming.
       *
       * Allocates buffers, creates requests, starts the camera, and queues all
       * initial requests. Frames will begin arriving asynchronously.
       *
       * @return Status::SUCCESS on success,
       *         Status::FAIL if streaming cannot be started.
       * @throw std::invalid_argument if camera control configuration is invalid.
       */
      types::Status start() override;

      /**
       * @brief Retrieve the next captured frame.
       *
       * Pops a frame from the internal multi-threaded queue. If no frame is available,
       * returns FAIL without modifying the output parameter.
       *
       * @param types::Frame Output frame object to populate.
       * @return Status::SUCCESS if a frame was retrieved,
       *         Status::FAIL if no frame is available.
       * @throw std::runtime_error if the camera stream is lost due to an error.
       */
      types::Status capture(types::Frame&) override;

      /**
       * @brief Stop camera streaming.
       *
       * Stops the camera, frees allocated buffers, clears the frame queue,
       * and marks the camera as inactive.
       *
       * @return Status::SUCCESS on success,
       *         Status::FAIL if stopping fails.
       */
      types::Status stop() override;

      /**
       * @brief Close the camera and release all resources.
       *
       * Releases the camera, stops the camera manager, clears buffers,
       * resets internal state, and disconnects handlers.
       *
       * @return Status::SUCCESS on success,
       *         Status::FAIL if closing fails.
       */
      types::Status close() override;

    protected:
      /**
       * @brief Set the camera driver for the Camera object.
       *
       * @param std::unique_ptr<drivers::ICameraDriver> Unique pointer to the camera driver.
       */
      void setDriver(std::unique_ptr<drivers::ICameraDriver>);

    private:
      std::unique_ptr<drivers::ICameraDriver> m_driver;
  };
} // namespace hardware
