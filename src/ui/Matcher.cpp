/*
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2026 Aleksa Markovic <aleksamarkovic96@outlook.com>
 */
#include "ui/Matcher.hpp"

#include <opencv2/imgproc.hpp>
#include <QFont>
#include <QImage>
#include <QFormLayout>
#include <QMutexLocker>
#include <QPixmap>
#include <QVBoxLayout>

namespace ui
{
  Matcher::Matcher(QWidget* parent) : QWidget(parent)
  {
    m_imageLabel.setAlignment(Qt::AlignCenter);
    m_imageLabel.setMinimumSize(600, 450);
    m_imageLabel.setObjectName("imageLabel");

    auto* form = new QFormLayout();

    form->addRow("Match:", &m_matchLabel);
    m_matchLabel.setObjectName("matchLabel");

    form->addRow("ID:", &m_idLabel);
    m_idLabel.setObjectName("idLabel");

    form->addRow("First Name:", &m_firstNameLabel);
    m_firstNameLabel.setObjectName("firstNameLabel");

    form->addRow("Last Name:", &m_lastNameLabel);
    m_lastNameLabel.setObjectName("lastNameLabel");

    form->addRow("Modified At:", &m_modifiedAtLabel);
    m_modifiedAtLabel.setObjectName("modifiedAtLabel");

    form->addRow("Eye Codes:", &m_countLabel);
    m_countLabel.setObjectName("countLabel");

    form->addRow("Code File:", &m_codeFileLabel);
    m_codeFileLabel.setObjectName("codeFileLabel");

    form->addRow("Image File:", &m_imageFileLabel);
    m_imageFileLabel.setObjectName("imageFileLabel");

    auto* layout = new QVBoxLayout(this);
    layout->addWidget(&m_imageLabel);
    layout->addLayout(form);

    QPalette palette;
    palette.setColor(QPalette::Window, QColor("#000000"));
    palette.setColor(QPalette::WindowText, QColor("#FFFFFF"));
    setPalette(palette);
    setAutoFillBackground(true);

    QFont font;
    font.setPointSize(12);
    font.setBold(true);
    setFont(font);

    clear();
  }

  void Matcher::onNewPerson(const types::PersonView& person)
  {
    {
      QMutexLocker lock(&m_mutex);
      m_person = person;
    }
    updateView();
  }

  void Matcher::clear()
  {
    m_imageLabel.clear();
    m_imageLabel.setText("No Image");

    m_matchLabel.setText("No Match");

    m_idLabel.setText("N/A");
    m_firstNameLabel.setText("N/A");
    m_lastNameLabel.setText("N/A");
    m_modifiedAtLabel.setText("N/A");
    m_countLabel.setText("N/A");
    m_codeFileLabel.setText("N/A");
    m_imageFileLabel.setText("N/A");
  }

  void Matcher::updateView()
  {
    types::PersonView person;
    {
      QMutexLocker lock(&m_mutex);
      person = m_person;
    }

    if (!updateImage(QString::fromStdString(person.image)))
    {
      clear();
      return;
    }

    const double match = 100.0 * person.match;
    m_matchLabel.setText(QString("%1 %").arg(match, 0, 'f', 2));

    QPalette palette = m_matchLabel.palette();
    if (match >= 90.0)
      palette.setColor(QPalette::WindowText, QColor("#7EE787"));
    else if (match >= 80.0)
      palette.setColor(QPalette::WindowText, QColor("#FFFF00"));
    else
      palette.setColor(QPalette::WindowText, QColor("#FF0000"));
    m_matchLabel.setPalette(palette);

    m_idLabel.setText(QString::fromStdString(person.id));
    m_firstNameLabel.setText(QString::fromStdString(person.firstName));
    m_lastNameLabel.setText(QString::fromStdString(person.lastName));
    m_modifiedAtLabel.setText(QString::fromStdString(person.modifiedAt));
    m_countLabel.setText(QString::number(person.count));
    m_codeFileLabel.setText(QString::fromStdString(std::format("{}.bin", person.index)));
    m_imageFileLabel.setText(QString::fromStdString(std::format("{}.png", person.index)));
  }

  bool Matcher::updateImage(const QString& path)
  {
    QImage image(path);

    if (image.isNull())
    {
      m_imageLabel.clear();
      m_imageLabel.setText("No Image");
      return false;
    }

    const QPixmap pixmap = QPixmap::fromImage(image);
    m_imageLabel.setPixmap(
      pixmap.scaled(
        m_imageLabel.size(),
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation
      )
    );
    return true;
  }
} // namespace ui
