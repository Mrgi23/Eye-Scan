/*
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2026 Aleksa Markovic <aleksamarkovic96@outlook.com>
 */
#pragma once

#include "hardware/GPU.hpp"
#include "utility/Frame.hpp"

#include <QMutex>
#include <QOpenGLFunctions>
#include <QOpenGLWindow>
#include <QSize>

/**
 * @namespace ui
 * @brief User interface components for the application.
 */
namespace ui
{
  /**
   * @class Display
   * @brief A display window for rendering frames and rectangles using OpenGL.
   */
  class Display : public QOpenGLWindow, protected QOpenGLFunctions
  {
  public:
    /**
     * @brief Construct new Display object.
     *
     * @param QWindow* The parent QWindow, default is nullptr.
     */
    explicit Display(QWindow* = nullptr);

    /**
     * @brief Destroy the Display object.
     */
    virtual ~Display();

  public slots:
    /**
     * @brief Slot called when a new frame is available.
     *
     * @param std::shared_ptr<types::Frame> The shared pointer to the new frame.
     */
    void onNewFrame(std::shared_ptr<types::Frame>);

  protected:
    /**
     * @brief Sets the GPU driver for the Display object.
     *
     * @param std::unique_ptr<hardware::GPU> Unique pointer to the GPU object.
     */
    void setInterface(std::unique_ptr<hardware::IGPU>);

  private:
    /**
     * @brief Initialize the OpenGL context.
     */
    void initializeGL() override;

    /**
     * @brief Render the current frame.
     */
    void paintGL() override;

    /**
     * @brief Handle window resizing and adjust the OpenGL viewport accordingly.
     *
     * @param int The new width of the window.
     * @param int The new height of the window.
     */
    void resizeGL(int, int) override;

    /**
     * @brief Show the display window and adjust the image to fit within it.
     *
     * @param QSize The size of the image to fit within the display window, default
     * is 1x1.
     */
    void fitDisplay(const QSize& size = QSize{1, 1});

  private:
    std::unique_ptr<hardware::IGPU> m_gpu;

    QMutex m_mutex;
    std::shared_ptr<types::Frame> m_data;
  };
} // namespace ui
