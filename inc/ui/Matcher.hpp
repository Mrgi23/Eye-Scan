/*
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2026 Aleksa Markovic <aleksamarkovic96@outlook.com>
 */
#pragma once

#include "utility/Types.hpp"

#include <QLabel>
#include <QMutex>
#include <QString>
#include <QWidget>

/**
 * @namespace ui
 * @brief User interface components for the application.
 */
namespace ui
{
  class Matcher : public QWidget
  {
    Q_OBJECT

    public:
      /**
       * @brief Construct a new Matcher widget.
       *
       * @param QWidget* The parent QWidget, default is nullptr.
       */
      explicit Matcher(QWidget* = nullptr);

    public slots:
      void onNewPerson(const types::PersonView&);

      void clear();

    private:
      void updateView();
      bool updateImage(const QString&);

    private:
      QLabel m_imageLabel;
      QLabel m_matchLabel;
      QLabel m_idLabel;
      QLabel m_firstNameLabel;
      QLabel m_lastNameLabel;
      QLabel m_modifiedAtLabel;
      QLabel m_countLabel;
      QLabel m_codeFileLabel;
      QLabel m_imageFileLabel;

      QMutex m_mutex;
      types::PersonView m_person;
  };
} // namespace ui
