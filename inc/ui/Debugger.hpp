/*
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2026 Aleksa Markovic <aleksamarkovic96@outlook.com>
 */
#pragma once

#include <QTextEdit>
#include <QWidget>

/**
 * @namespace ui
 * @brief User interface components for the application.
 */
namespace ui
{
  /**
   * @class Debugger
   * @brief A window for displaying log messages.
   */
  class Debugger : public QWidget
  {
    Q_OBJECT

    public:
      /**
       * @brief Construct new Debugger object.
       *
       * @param QWidget* The parent QWidget, default is nullptr.
       */
      explicit Debugger(QWidget* = nullptr);

      /**
       * @brief Destroy the Debugger object.
       */
      ~Debugger() override;

    private:
      QTextEdit* m_view = nullptr;
  };
} // namespace ui
