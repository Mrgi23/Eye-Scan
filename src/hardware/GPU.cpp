/*
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2026 Aleksa Markovic <aleksamarkovic96@outlook.com>
 */
#define GL_GLEXT_PROTOTYPES

#include "hardware/GPU.hpp"
#include "hardware/DMA.hpp"
#include "utility/Logger.hpp"

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <libdrm/drm_fourcc.h>

// LCOV_EXCL_START
namespace
{
  const GLfloat vertices[] = {
    -1.0f, -1.0f, 0.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f,  0.0f, 0.0f, 1.0f, 1.0f,  1.0f, 0.0f,
  };

  const char* fragmentShaderSrc =
    "precision highp float;\n"
    "uniform sampler2D uTexRaw;\n"
    "uniform vec2 uTextureSize;\n"
    "uniform vec2 uTexelSize;\n"
    "varying vec2 vTexCoord;\n"
    "\n"
    "float sampleRaw(vec2 offset) {\n"
    "  return texture2D(uTexRaw, vTexCoord + offset).r;\n"
    "}\n"
    "\n"
    "vec3 toneMap(vec3 color) {\n"
    "  return (color * (2.51 * color + 0.03)) /\n"
    "         (color * (2.43 * color + 0.59) + 0.14);\n"
    "}\n"
    "\n"
    "float linearToSrgb(float value) {\n"
    "  return value <= 0.0031308 ? 12.92 * value :\n"
    "         1.055 * pow(value, 1.0 / 2.4) - 0.055;\n"
    "}\n"
    "\n"
    "void main() {\n"
    "  vec2 pixel = floor(vTexCoord * uTextureSize);\n"
    "  vec2 parity = mod(pixel, 2.0);\n"
    "  vec2 x = vec2(uTexelSize.x, 0.0);\n"
    "  vec2 y = vec2(0.0, uTexelSize.y);\n"
    "  float center = sampleRaw(vec2(0.0));\n"
    "  float horizontal = 0.5 * (sampleRaw(-x) + sampleRaw(x));\n"
    "  float vertical = 0.5 * (sampleRaw(-y) + sampleRaw(y));\n"
    "  float diagonal = 0.25 * (sampleRaw(-x - y) + sampleRaw(x - y) +\n"
    "                           sampleRaw(-x + y) + sampleRaw(x + y));\n"
    "  float green = 0.25 * (sampleRaw(-x) + sampleRaw(x) +\n"
    "                        sampleRaw(-y) + sampleRaw(y));\n"
    "  vec3 rgb;\n"
    "\n"
    "  if (parity.y < 0.5) {\n"
    "    if (parity.x < 0.5)\n"
    "      rgb = vec3(center, green, diagonal);\n"
    "    else\n"
    "      rgb = vec3(horizontal, center, vertical);\n"
    "  } else {\n"
    "    if (parity.x < 0.5)\n"
    "      rgb = vec3(vertical, center, horizontal);\n"
    "    else\n"
    "      rgb = vec3(diagonal, green, center);\n"
    "  }\n"
    "\n"
    "  rgb *= vec3(1.65, 1.0, 1.55);\n"
    "  rgb = mat3(\n"
    "    1.45, -0.20, -0.03,\n"
    "   -0.30,  1.35, -0.28,\n"
    "   -0.15, -0.15,  1.31\n"
    "  ) * rgb;\n"
    "  rgb = toneMap(max(rgb, 0.0));\n"
    "  rgb = vec3(linearToSrgb(rgb.r), linearToSrgb(rgb.g), linearToSrgb(rgb.b));\n"
    "  float gray = dot(clamp(rgb, 0.0, 1.0), vec3(0.2126, 0.7152, 0.0722));\n"
    "  gl_FragColor = vec4(vec3(gray), 1.0);\n"
    "}\n";
  const char* vertexShaderSrc = "attribute vec2 aPos;\n"
                                "attribute vec2 aTexCoord;\n"
                                "varying vec2 vTexCoord;\n"
                                "void main() {\n"
                                "  vTexCoord = aTexCoord;\n"
                                "  gl_Position = vec4(aPos, 0.0, 1.0);\n"
                                "}\n";

  struct GlExtensions
  {
    PFNEGLCREATEIMAGEKHRPROC eglCreateImageKHR = nullptr;
    PFNEGLDESTROYIMAGEKHRPROC eglDestroyImageKHR = nullptr;
    PFNGLEGLIMAGETARGETTEXTURE2DOESPROC glEGLImageTarget = nullptr;

    PFNGLGENVERTEXARRAYSOESPROC glGenVertexArraysOES = nullptr;
    PFNGLBINDVERTEXARRAYOESPROC glBindVertexArrayOES = nullptr;
    PFNGLDELETEVERTEXARRAYSOESPROC glDeleteVertexArraysOES = nullptr;

    bool initialize()
    {
      eglCreateImageKHR = reinterpret_cast<PFNEGLCREATEIMAGEKHRPROC>(
        eglGetProcAddress("eglCreateImageKHR")
      );
      eglDestroyImageKHR = reinterpret_cast<PFNEGLDESTROYIMAGEKHRPROC>(
        eglGetProcAddress("eglDestroyImageKHR")
      );
      glEGLImageTarget = reinterpret_cast<PFNGLEGLIMAGETARGETTEXTURE2DOESPROC>(
        eglGetProcAddress("glEGLImageTargetTexture2DOES")
      );

      glGenVertexArraysOES = reinterpret_cast<PFNGLGENVERTEXARRAYSOESPROC>(
        eglGetProcAddress("glGenVertexArraysOES")
      );
      glBindVertexArrayOES = reinterpret_cast<PFNGLBINDVERTEXARRAYOESPROC>(
        eglGetProcAddress("glBindVertexArrayOES")
      );
      glDeleteVertexArraysOES = reinterpret_cast<PFNGLDELETEVERTEXARRAYSOESPROC>(
        eglGetProcAddress("glDeleteVertexArraysOES")
      );

      return eglCreateImageKHR && eglDestroyImageKHR && glEGLImageTarget &&
        glGenVertexArraysOES && glBindVertexArrayOES && glDeleteVertexArraysOES;
    }
  };
} // namespace

namespace hardware
{
  namespace drivers
  {
    class GPUDriver : public IGPUDriver
    {
    public:
      GPUDriver() noexcept = default;
      ~GPUDriver() override;

      DISABLE_COPY_AND_MOVE(GPUDriver)

      virtual void open(void* display) override;
      virtual bool isOpen() const noexcept override;
      virtual bool addTexture(void* surface, const std::shared_ptr<void>& data)
        override;
      virtual void draw() override;
      virtual void close() override;

    private:
      void clearTexture();

    private:
      GlExtensions ext{};

      EGLDisplay m_display = EGL_NO_DISPLAY;
      EGLContext m_context = EGL_NO_CONTEXT;
      EGLImageKHR m_image = EGL_NO_IMAGE_KHR;
      GLuint m_texture = 0;

      GLuint m_program = 0;
      GLuint m_vertexArray = 0;
      GLuint m_vertexBuffer = 0;

      GLsizei m_width = 0;
      GLsizei m_height = 0;

      bool m_isOpen = false;
    };

    GPUDriver::~GPUDriver()
    {
      close();
    }

    void GPUDriver::open(void* display)
    {
      m_display = static_cast<EGLDisplay>(display);
      if (m_display == EGL_NO_DISPLAY)
      {
        logger::Warning("GPU: Invalid EGL display.");
        return;
      }
      eglBindAPI(EGL_OPENGL_ES_API);

      static const EGLint attrs[] = {EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE};
      m_context = eglCreateContext(display, nullptr, EGL_NO_CONTEXT, attrs);
      if (m_context == EGL_NO_CONTEXT)
      {
        logger::Warning("GPU: Failed to create EGL context.");
        return;
      }

      if (!eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, m_context))
      {
        logger::Warning("GPU: Failed to make EGL context current.");
        return;
      }

      if (!ext.initialize())
      {
        logger::Warning("GPU: Failed to initialize EGL/GLES extensions.");
        return;
      }

      auto compileShader = [](GLenum type, const char* src) -> GLuint
      {
        GLuint s = glCreateShader(type);
        glShaderSource(s, 1, &src, nullptr);
        glCompileShader(s);
        GLint ok = 0;
        glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
        if (!ok)
        {
          glDeleteShader(s);
          return 0;
        }
        return s;
      };

      GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSrc);
      GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSrc);
      if (!fragmentShader || !vertexShader)
      {
        if (fragmentShader)
          glDeleteShader(fragmentShader);
        if (vertexShader)
          glDeleteShader(vertexShader);

        logger::Warning("GPU: Failed to compile main shaders.");
        return;
      }

      if (m_program)
        glDeleteProgram(m_program);

      m_program = glCreateProgram();
      glAttachShader(m_program, vertexShader);
      glAttachShader(m_program, fragmentShader);
      glLinkProgram(m_program);

      GLint linked = 0;
      glGetProgramiv(m_program, GL_LINK_STATUS, &linked);
      glDeleteShader(vertexShader);
      glDeleteShader(fragmentShader);
      if (!linked)
      {
        glDeleteProgram(m_program);
        m_program = 0;
        logger::Warning("GPU: Failed to link main shader program.");
        return;
      }

      if (m_vertexArray == 0)
      {
        ext.glGenVertexArraysOES(1, &m_vertexArray);
        ext.glBindVertexArrayOES(m_vertexArray);

        glGenBuffers(1, &m_vertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        GLint posLoc = glGetAttribLocation(m_program, "aPos");
        glEnableVertexAttribArray(posLoc);
        glVertexAttribPointer(
          posLoc, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat),
          reinterpret_cast<void*>(0)
        );

        GLint texLoc = glGetAttribLocation(m_program, "aTexCoord");
        glEnableVertexAttribArray(texLoc);
        glVertexAttribPointer(
          texLoc, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat),
          reinterpret_cast<void*>(2 * sizeof(GLfloat))
        );

        ext.glBindVertexArrayOES(0);
      }
      m_isOpen = true;
    }

    bool GPUDriver::isOpen() const noexcept
    {
      return m_isOpen;
    }

    bool GPUDriver::addTexture(void* surface, const std::shared_ptr<void>& data)
    {
      EGLSurface eglSurface = static_cast<EGLSurface>(surface);
      if (eglSurface == EGL_NO_SURFACE)
      {
        logger::Warning("GPU: Invalid EGL surface.");
        return false;
      }

      if (!eglMakeCurrent(m_display, eglSurface, eglSurface, m_context))
      {
        logger::Warning("GPU: Failed to make EGL context current for rendering.");
        return false;
      }

      const auto dma = std::static_pointer_cast<hardware::DMA>(data);
      if (!dma || dma->memory().fd < 0)
      {
        logger::Warning("GPU: Frame has no valid DMA handler.");
        return false;
      }
      clearTexture();

      const VirtualMemory& virtualMemory = dma->memory();
      const EGLint attrs[] = {
        EGL_WIDTH,
        static_cast<EGLint>(virtualMemory.size.width),
        EGL_HEIGHT,
        static_cast<EGLint>(virtualMemory.size.height),
        EGL_LINUX_DRM_FOURCC_EXT,
        static_cast<EGLint>(DRM_FORMAT_R16),
        EGL_DMA_BUF_PLANE0_FD_EXT,
        virtualMemory.fd,
        EGL_DMA_BUF_PLANE0_OFFSET_EXT,
        static_cast<EGLint>(virtualMemory.offset),
        EGL_DMA_BUF_PLANE0_PITCH_EXT,
        static_cast<EGLint>(virtualMemory.stride),
        EGL_NONE
      };

      m_image = ext.eglCreateImageKHR(
        m_display, EGL_NO_CONTEXT, EGL_LINUX_DMA_BUF_EXT, nullptr, attrs
      );

      if (m_image == EGL_NO_IMAGE_KHR)
      {
        logger::Warning("GPU: Failed to create EGL image from DMA buffer.");
        clearTexture();
        return false;
      }

      glGenTextures(1, &m_texture);
      glBindTexture(GL_TEXTURE_2D, m_texture);
      glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

      ext.glEGLImageTarget(GL_TEXTURE_2D, m_image);

      if (glGetError() != GL_NO_ERROR)
      {
        logger::Warning("GPU: Failed to bind EGL image to texture.");
        clearTexture();
        return false;
      }

      glBindTexture(GL_TEXTURE_2D, 0);

      m_width = static_cast<GLsizei>(virtualMemory.size.width);
      m_height = static_cast<GLsizei>(virtualMemory.size.height);
      return true;
    }

    void GPUDriver::draw()
    {
      glViewport(0, 0, m_width, m_height);
      glClearColor(0.f, 0.f, 0.f, 1.f);
      glClear(GL_COLOR_BUFFER_BIT);

      glUseProgram(m_program);
      ext.glBindVertexArrayOES(m_vertexArray);

      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, m_texture);
      glUniform1i(glGetUniformLocation(m_program, "uTexRaw"), 0);
      glUniform2f(
        glGetUniformLocation(m_program, "uTextureSize"),
        static_cast<GLfloat>(m_width), static_cast<GLfloat>(m_height)
      );
      glUniform2f(
        glGetUniformLocation(m_program, "uTexelSize"),
        1.0f / static_cast<GLfloat>(m_width), 1.0f / static_cast<GLfloat>(m_height)
      );

      glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
      ext.glBindVertexArrayOES(0);
    }

    void GPUDriver::clearTexture()
    {
      if (m_image != EGL_NO_IMAGE_KHR)
      {
        ext.eglDestroyImageKHR(m_display, m_image);
        m_image = EGL_NO_IMAGE_KHR;
      }

      if (m_texture)
      {
        glDeleteTextures(1, &m_texture);
        m_texture = 0;
      }
      m_width = 0;
      m_height = 0;
    }

    void GPUDriver::close()
    {
      clearTexture();

      if (m_program)
      {
        glDeleteProgram(m_program);
        m_program = 0;
      }
      if (m_vertexArray)
      {
        ext.glDeleteVertexArraysOES(1, &m_vertexArray);
        m_vertexArray = 0;
      }
      if (m_vertexBuffer)
      {
        glDeleteBuffers(1, &m_vertexBuffer);
        m_vertexBuffer = 0;
      }

      if (m_display != EGL_NO_DISPLAY && m_context != EGL_NO_CONTEXT)
      {
        eglMakeCurrent(m_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        eglDestroyContext(m_display, m_context);
        m_context = EGL_NO_CONTEXT;
      }

      m_isOpen = false;
    }
  } // namespace drivers

  // LCOV_EXCL_STOP

  GPU::GPU() noexcept : m_driver(std::make_unique<drivers::GPUDriver>())
  {
  }

  GPU::~GPU() = default;

  types::Status GPU::open(void* display)
  {
    if (m_driver->isOpen())
      return logger::Info("GPU: Display is already open.");

    m_driver->open(display);
    if (!m_driver->isOpen())
    {
      m_driver->close();
      return types::Status::FAIL;
    }
    return logger::Info("GPU: Display opened successfully.");
  }

  types::Status GPU::render(void* surface, const std::shared_ptr<void>& data)
  {
    if (!m_driver->isOpen())
      return logger::Warning("GPU: Display is not open. Call open first.");

    if (!m_driver->addTexture(surface, data))
      return types::Status::FAIL;

    m_driver->draw();
    return types::Status::SUCCESS;
  }

  types::Status GPU::close()
  {
    if (!m_driver->isOpen())
      return logger::Info("GPU: Display is already closed.");

    m_driver->close();
    return logger::Info("GPU: Display closed successfully.");
  }

  void GPU::setDriver(std::unique_ptr<drivers::IGPUDriver> driver)
  {
    m_driver = std::move(driver);
  }
} // namespace hardware
