/*
 *  SPDX-License-Identifier: Apache-2.0
 *  Copyright (c) 2026 Aleksa Markovic <aleksamarkovic96@outlook.com>
 */
#include "hardware/Camera.hpp"
#include "hardware/DMA.hpp"
#include "utility/Logger.hpp"
#include "utility/Templates.hpp"

#include <libcamera/camera.h>
#include <libcamera/camera_manager.h>
#include <libcamera/control_ids.h>
#include <libcamera/controls.h>
#include <libcamera/formats.h>
#include <libcamera/framebuffer.h>
#include <libcamera/framebuffer_allocator.h>
#include <libcamera/property_ids.h>
#include <libcamera/request.h>
#include <libcamera/stream.h>

#include <atomic>
#include <cstdlib>
#include <format>
#include <utility>

namespace hardware
{
  // LCOV_EXCL_START
  namespace drivers
  {
    class CameraDriver : public ICameraDriver
    {
    public:
      CameraDriver() noexcept;
      ~CameraDriver() override;

      DISABLE_COPY_AND_MOVE(CameraDriver)

      void open(const std::string& device) override;
      bool isOpen() const noexcept override;
      void connect(bool on) noexcept override;
      void configure(const types::Size& size, const CameraControls& controls)
        override;
      bool isConfigured() const noexcept override;
      void start() override;
      bool capture(types::Frame& frame) override;
      void stop() override;
      bool isActive() const noexcept override;
      void close() override;

    private:
      void callback(libcamera::Request* request);

    private:
      std::unique_ptr<libcamera::CameraManager> m_manager;
      std::unique_ptr<libcamera::ControlList> m_controls;
      std::vector<libcamera::StreamRole> m_roles;
      std::shared_ptr<libcamera::Camera> m_camera;
      std::unique_ptr<libcamera::CameraConfiguration> m_config;
      libcamera::StreamConfiguration* m_stream = nullptr;

      std::unique_ptr<libcamera::FrameBufferAllocator> m_allocator;
      std::vector<libcamera::FrameBuffer*> m_buffers;
      std::vector<std::unique_ptr<libcamera::Request>> m_requests;
      types::Queue<types::Frame, 4> m_frames;
      int m_warmup = 15;

      bool m_isOpen = false;
      bool m_isConfigured = false;
      bool m_isActive = false;
    };

    CameraDriver::CameraDriver() noexcept
    {
      m_manager = std::make_unique<libcamera::CameraManager>();
      m_roles = {libcamera::StreamRole::Raw};
      m_controls =
        std::make_unique<libcamera::ControlList>(libcamera::controls::controls);
    }

    CameraDriver::~CameraDriver()
    {
      if (m_camera)
      {
        stop();
        connect(false);
        close();
      }
    }

    void CameraDriver::open(const std::string& device)
    {
      {
        const std::string tuningFileName = device.find("noir") == std::string::npos
          ? std::format("{}_noir.json", device)
          : std::format("{}.json", device);
        const std::string tuningFile =
          std::format("/usr/share/libcamera/ipa/rpi/pisp/{}", tuningFileName);
        setenv("LIBCAMERA_RPI_TUNING_FILE", tuningFile.c_str(), 1);
      }
      if (m_manager->start())
        logger::Error(
          "Camera: Failed to start Camera manager.", types::Error::RUNTIME_ERROR
        );

      for (const auto& camera : m_manager->cameras())
      {
        const auto dev = camera->properties().get(libcamera::properties::Model);
        if (dev.has_value() && dev == device)
        {
          m_camera = camera;
          break;
        }
      }

      if (!m_camera)
      {
        close();
        logger::Error(
          std::format("Camera: Camera with device '{}' not found.", device),
          types::Error::INVALID_ARGUMENT
        );
      }

      if (m_camera->acquire())
      {
        close();
        logger::Error(
          "Camera: Failed to acquire camera.", types::Error::RUNTIME_ERROR
        );
      }

      connect(true);
      m_isOpen = true;
    }

    bool CameraDriver::isOpen() const noexcept
    {
      return m_isOpen;
    }

    void CameraDriver::connect(bool on) noexcept
    {
      if (on)
        m_camera->requestCompleted.connect(this, &CameraDriver::callback);
      else
        m_camera->requestCompleted.disconnect(this, &CameraDriver::callback);
    }

    void CameraDriver::configure(
      const types::Size& size, const CameraControls& controls
    )
    {
      m_config = m_camera->generateConfiguration(m_roles);
      if (!m_config || m_config->empty())
      {
        logger::Warning("Camera: Failed to generate camera configuration.");
        return;
      }
      m_stream = &m_config->at(0);
      m_stream->pixelFormat = libcamera::formats::SRGGB16;
      m_stream->size = {
        static_cast<uint32_t>(size.width), static_cast<uint32_t>(size.height)
      };
      if (m_config->validate() == libcamera::CameraConfiguration::Invalid)
      {
        logger::Warning("Camera: Failed to generate camera configuration.");
        return;
      }

      if (m_camera->configure(m_config.get()))
      {
        logger::Warning("Camera: Failed to configure camera.");
        return;
      }

      if (controls.fps == 0)
        logger::Error("Camera: FPS cannot be zero.", types::Error::INVALID_ARGUMENT);

      const int64_t frameTime = 1000000 / controls.fps;
      std::array<int64_t, 2> limits = {frameTime, frameTime};
      m_controls->set(libcamera::controls::FrameDurationLimits, limits);

      const bool autoExposure = controls.autoExposure.value_or(true);
      m_controls->set(libcamera::controls::AeEnable, autoExposure);

      if (!autoExposure)
      {
        const int32_t exposureTime = controls.exposureTime.value_or(frameTime / 2);
        if (exposureTime < 0 || exposureTime > frameTime)
          logger::Error(
            "Camera: Exposure time exceeds maximum allowed by FPS.",
            types::Error::INVALID_ARGUMENT
          );
        m_controls->set(libcamera::controls::ExposureTime, exposureTime);

        const float analogueGain = controls.analogueGain.value_or(1.0f);
        if (analogueGain < 1.0f || analogueGain > 16.0f)
          logger::Error(
            "Camera: Analogue gain must be between 1.0 and 16.0.",
            types::Error::INVALID_ARGUMENT
          );
        m_controls->set(libcamera::controls::AnalogueGain, analogueGain);
      }
      m_isConfigured = true;
    }

    bool CameraDriver::isConfigured() const noexcept
    {
      return m_isConfigured;
    }

    void CameraDriver::start()
    {
      m_allocator = std::make_unique<libcamera::FrameBufferAllocator>(m_camera);
      if (m_allocator->allocate(m_stream->stream()) < 0)
      {
        stop();
        logger::Warning("Camera: Failed to allocate memory for camera stream.");
        return;
      }
      m_buffers.clear();
      for (const auto& buffer : m_allocator->buffers(m_stream->stream()))
        m_buffers.push_back(buffer.get());

      m_requests.clear();
      m_requests.reserve(m_buffers.size());
      for (auto& buffer : m_buffers)
      {
        std::unique_ptr<libcamera::Request> request = m_camera->createRequest();
        if (!request || request->addBuffer(m_stream->stream(), buffer) < 0)
        {
          stop();
          logger::Warning("Camera: Failed to create requests for camera stream.");
          return;
        }
        m_requests.push_back(std::move(request));
      }

      if (m_camera->start(m_controls.get()))
      {
        logger::Warning("Camera: Failed to start camera.");
        stop();
        return;
      }

      for (auto& req : m_requests)
      {
        if (m_camera->queueRequest(req.get()) < 0)
        {
          stop();
          logger::Warning("Camera: Failed to queue camera request.");
          return;
        }
      }
      m_isActive = true;
    }

    bool CameraDriver::capture(types::Frame& frame)
    {
      if (m_frames.pop(frame) == types::Status::SUCCESS)
        return true;
      return false;
    }

    void CameraDriver::stop()
    {
      m_camera->stop();
      m_requests.clear();
      m_buffers.clear();
      m_allocator.reset();
      m_frames.clear();
      m_isActive = false;
    }

    bool CameraDriver::isActive() const noexcept
    {
      return m_isActive;
    }

    void CameraDriver::close()
    {
      m_camera->release();
      m_camera.reset();
      m_manager->stop();
      m_isOpen = false;
    }

    void CameraDriver::callback(libcamera::Request* request)
    {
      if (request->status() == libcamera::Request::RequestCancelled)
        return;

      if (m_warmup > 0)
      {
        m_warmup--;
        request->reuse(libcamera::Request::ReuseBuffers);
        if (m_camera->queueRequest(request) < 0)
          logger::Error(
            "Camera: Camera stream is lost due to an error.",
            types::Error::RUNTIME_ERROR
          );
        return;
      }

      if (m_stream && request->buffers().count(m_stream->stream()) > 0)
      {
        libcamera::FrameBuffer* const buffer =
          request->buffers().at(m_stream->stream());
        const auto& plane = buffer->planes()[0];

        const std::shared_ptr<DMA> dma = std::make_shared<DMA>();
        const VirtualMemory vmem{
          {m_stream->stream()->configuration().size.width,
           m_stream->stream()->configuration().size.height},
          plane.length,
          m_stream->stream()->configuration().stride,
          plane.offset,
          plane.fd.get()
        };

        if (dma->connect(vmem) == types::Status::FAIL)
        {
          request->reuse(libcamera::Request::ReuseBuffers);
          if (m_camera->queueRequest(request) < 0)
            logger::Error(
              "Camera: Camera stream is lost due to an error.",
              types::Error::RUNTIME_ERROR
            );
          return;
        }

        uint16_t* data = nullptr;
        if (dma->cpu(data) == types::Status::SUCCESS)
        {
          const auto owner = std::static_pointer_cast<void>(dma);
          types::Frame frame(data, vmem.size, vmem.stride, owner);
          m_frames.push(std::move(frame));
        }

        request->reuse(libcamera::Request::ReuseBuffers);
        if (m_camera->queueRequest(request) < 0)
          logger::Error(
            "Camera: Camera stream is lost due to an error.",
            types::Error::RUNTIME_ERROR
          );
      }
    }
  } // namespace drivers

  // LCOV_EXCL_STOP

  Camera::Camera() noexcept : m_driver(std::make_unique<drivers::CameraDriver>())
  {
    setenv("LIBCAMERA_LOG_LEVELS", "*:FATAL", 1);
  }

  Camera::~Camera() = default;

  types::Status Camera::open(const std::string& device)
  {
    if (m_driver->isOpen())
      return logger::Info("Camera: Camera is already opened.");

    m_driver->open(device);
    return logger::Info("Camera: Camera opened successfully.");
  }

  types::Status Camera::configure(
    const types::Size& size, const CameraControls& config
  )
  {
    if (!m_driver->isOpen())
      return logger::Warning("Camera: Camera is not opened. Call open first.");

    if (m_driver->isActive())
      return logger::Warning(
        "Camera: Cannot configure while streaming. Call stop first."
      );

    m_driver->configure(size, config);
    return m_driver->isConfigured()
      ? logger::Info("Camera: Camera configurated successful.")
      : types::Status::FAIL;
  }

  types::Status Camera::start()
  {
    if (!m_driver->isOpen())
      return logger::Warning("Camera: Camera is not opened. Call open first.");

    if (!m_driver->isConfigured())
      return logger::Warning(
        "Camera: Camera is not configured. Call configure first."
      );

    if (m_driver->isActive())
      return logger::Info("Camera: Camera is already active.");

    m_driver->start();
    return m_driver->isActive()
      ? logger::Info("Camera: Camera started successfully.")
      : types::Status::FAIL;
  }

  types::Status Camera::capture(types::Frame& frame)
  {
    if (!m_driver->isOpen())
      return logger::Warning("Camera: Camera is not opened. Call open first.");

    if (!m_driver->isConfigured())
      return logger::Warning(
        "Camera: Camera is not configured. Call configure first."
      );

    if (!m_driver->isActive())
      return logger::Warning("Camera: Camera is not active. Call start first.");

    return m_driver->capture(frame) ? types::Status::SUCCESS : types::Status::FAIL;
  }

  types::Status Camera::stop()
  {
    if (!m_driver->isOpen())
      return logger::Warning("Camera: Camera is not opened. Call open first.");

    if (!m_driver->isActive())
      return logger::Info("Camera: Camera is already stopped.");

    m_driver->stop();
    return logger::Info("Camera: Camera stopped successfully.");
  }

  types::Status Camera::close()
  {
    if (!m_driver->isOpen())
      return logger::Info("Camera: Camera is already closed.");

    if (m_driver->isActive())
      return logger::Warning(
        "Camera: Cannot close camera while streaming is active. Call stop first."
      );

    m_driver->close();
    return logger::Info("Camera: Camera closed successfully.");
  }

  void Camera::setDriver(std::unique_ptr<drivers::ICameraDriver> driver)
  {
    m_driver = std::move(driver);
  }
} // namespace hardware
