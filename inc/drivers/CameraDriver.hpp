/*
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2026 Aleksa Markovic <aleksamarkovic96@outlook.com>
 */
#pragma once

#include "utility/Enums.hpp"
#include "utility/Frame.hpp"

#include <string>

/**
 * @namespace drivers
 * @brief Low-level hardware driver interfaces.
 */
namespace drivers
{
  /**
   * @class ICameraDriver
   * @brief Virtual interface for camera hardware drivers.
   */
  class ICameraDriver
  {
    public:
      virtual ~ICameraDriver() = default;

      virtual void open(const std::string& device) = 0;
      virtual bool isOpen() const = 0;
      virtual void connect(bool) = 0;
      virtual void configure(const types::Size&, const types::CameraControls&) = 0;
      virtual bool isConfigured() const = 0;
      virtual void start() = 0;
      virtual bool capture(types::Frame& frame) = 0;
      virtual void stop() = 0;
      virtual bool isActive() const = 0;
      virtual void close() = 0;
  };
} // namespace drivers

/**
 * @namespace hardware
 * @brief Hardware interfaces for low‑level device operations (Camera, DMA, LED, GPU).
 */
namespace hardware
{
  /**
   * @class ICamera
   * @brief Virtual interface for camera operations.
   */
  class ICamera
  {
    public:
      virtual ~ICamera() = default;

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
      virtual types::Status open(const std::string&) = 0;

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
      virtual types::Status configure(const types::Size&, const types::CameraControls&) = 0;

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
      virtual types::Status start() = 0;

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
      virtual types::Status capture(types::Frame&) = 0;

      /**
       * @brief Stop camera streaming.
       *
       * Stops the camera, frees allocated buffers, clears the frame queue,
       * and marks the camera as inactive.
       *
       * @return Status::SUCCESS on success,
       *         Status::FAIL if stopping fails.
       */
      virtual types::Status stop() = 0;

       /**
       * @brief Close the camera and release all resources.
       *
       * Releases the camera, stops the camera manager, clears buffers,
       * resets internal state, and disconnects handlers.
       *
       * @return Status::SUCCESS on success,
       *         Status::FAIL if closing fails.
       */
      virtual types::Status close() = 0;
  };
} // namespace hardware
