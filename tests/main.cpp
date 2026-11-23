/*
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2026 Aleksa Markovic <aleksamarkovic96@outlook.com>
 */
#include <gtest/gtest.h>
#include <QApplication>

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);

  if (!qEnvironmentVariableIsSet("QT_QPA_PLATFORM"))
    qputenv("QT_QPA_PLATFORM", "wayland");
  QApplication app(argc, argv);

  return RUN_ALL_TESTS();
}
