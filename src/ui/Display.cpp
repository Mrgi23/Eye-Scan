/*
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2026 Aleksa Markovic <aleksamarkovic96@outlook.com>
 */
#include "ui/Display.hpp"
#include "utility/Logger.hpp"

#include <EGL/egl.h>
#include <QOpenGLContext>
#include <QScreen>

namespace ui
{
  Display::Display(QWindow* parent)
    : QOpenGLWindow(NoPartialUpdate, parent), m_gpu(std::make_unique<hardware::GPU>())
  {
    const QSize screen = QGuiApplication::primaryScreen()->size();
    fitDisplay(screen);
  }

  Display::~Display()
  {
    if (m_gpu)
    {
      m_gpu->close();
      m_gpu.reset();
    }
  }

  void Display::onNewFrame(std::shared_ptr<types::Frame> data)
  {
    const QSize size{
      static_cast<int>(data->size().width),
      static_cast<int>(data->size().height)
    };

    {
      QMutexLocker lock(&m_mutex);
      m_data = std::move(data);
    }

    if (!isVisible())
      fitDisplay(size);
    update();
  }

  void Display::setInterface(std::unique_ptr<hardware::IGPU> gpu)
  {
    m_gpu = std::move(gpu);
  }

  void Display::initializeGL()
  {
    initializeOpenGLFunctions();

    if (m_gpu && m_gpu->open(static_cast<void*>(eglGetCurrentDisplay())) != types::Status::SUCCESS)
    {
      m_gpu.reset();
      close();
    }
  }

  void Display::paintGL()
  {
    std::shared_ptr<types::Frame> data;
    {
      QMutexLocker lock(&m_mutex);
      data.swap(m_data);
    }

    if (!m_gpu || !data || !data->owner())
      return;

    m_gpu->render(static_cast<void*>(eglGetCurrentSurface(EGL_DRAW)), data->owner());
  }

  void Display::resizeGL(int w, int h)
  {
  }

  void Display::fitDisplay(const QSize& size)
  {
    const QSize screen = QGuiApplication::primaryScreen()->size();

    const float imgAspect = float(size.width()) / float(size.height());
    const float screenAspect = float(screen.width()) / float(screen.height());

    int width, height;
    if (imgAspect > screenAspect)
    {
      width  = screen.width();
      height = static_cast<int>(width / imgAspect);
    }
    else
    {
      height = screen.height();
      width  = static_cast<int>(height * imgAspect);
    }

    resize(width, height);
  }
} // namespace ui
