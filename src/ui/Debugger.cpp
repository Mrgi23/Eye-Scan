/*
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2026 Aleksa Markovic <aleksamarkovic96@outlook.com>
 */
#include "ui/Debugger.hpp"
#include "utility/Logger.hpp"

#include <QColor>
#include <QMetaObject>
#include <QString>
#include <QTextEdit>
#include <QVBoxLayout>

namespace ui
{
  Debugger::Debugger(QWidget* parent) : QWidget(parent)
  {
    resize(400, 300);
    move(400, 400);

    m_view = new QTextEdit(this);
    m_view->setReadOnly(true);
    m_view->setStyleSheet("QTextEdit { "
                          "  background-color: black; "
                          "}");

    auto* layout = new QVBoxLayout(this);
    layout->addWidget(m_view);
    setLayout(layout);

    logger::SetLogCallback(
      [this](const std::string& message)
      {
        QString qMessage = QString::fromUtf8(message.data(), message.size());

        QMetaObject::invokeMethod(
          this,
          [this, qMessage]()
          {
            QColor color = QColor("#7EE787");
            if (qMessage.contains("[WARNING]"))
              color = Qt::yellow;
            else if (qMessage.contains("[ERROR]"))
              color = Qt::red;

            m_view->setTextColor(color);
            m_view->append(qMessage);
          },
          Qt::QueuedConnection
        );
      }
    );
  }

  Debugger::~Debugger()
  {
    logger::SetLogCallback(nullptr);
  }
} // namespace ui
