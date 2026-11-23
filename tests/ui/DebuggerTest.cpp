/*
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2026 Aleksa Markovic <aleksamarkovic96@outlook.com>
 */
#include "utility/Logger.hpp"
#include "ui/Debugger.hpp"

#include <gtest/gtest.h>
#include <QString>
#include <QTest>

TEST(Debugger, SendMessages)
{
  ui::Debugger debugger;
  debugger.show();
  QTest::qWait(50);

  {
    logger::Info("Test info");
    QTest::qWait(50);

    QTextEdit* edit = debugger.findChild<QTextEdit*>();
    ASSERT_NE(edit, nullptr) << "Debugger does not contain a QTextEdit";

    QString content = edit->toPlainText();
    EXPECT_TRUE(content.contains("[INFO]"));
    EXPECT_TRUE(content.contains("Test info"));
  }

  {
    logger::Warning("Test warning");
    QTest::qWait(50);

    QTextEdit* edit = debugger.findChild<QTextEdit*>();
    ASSERT_NE(edit, nullptr) << "Debugger does not contain a QTextEdit";

    QString content = edit->toPlainText();
    EXPECT_TRUE(content.contains("[WARNING]"));
    EXPECT_TRUE(content.contains("Test warning"));
  }

  {
    EXPECT_THROW(logger::Error("Test error", types::Error::INVALID_ARGUMENT), std::invalid_argument);
    QTest::qWait(50);

    QTextEdit* edit = debugger.findChild<QTextEdit*>();
    ASSERT_NE(edit, nullptr) << "Debugger does not contain a QTextEdit";

    QString content = edit->toPlainText();
    EXPECT_TRUE(content.contains("[ERROR]"));
    EXPECT_TRUE(content.contains("Test error"));
  }
}
