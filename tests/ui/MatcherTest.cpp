/*
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2026 Aleksa Markovic <aleksamarkovic96@outlook.com>
 */
#include "ui/Matcher.hpp"

#include <QTest>
#include <gtest/gtest.h>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include <filesystem>

namespace fs = std::filesystem;

TEST(Matcher, NewPerson)
{
  const fs::path path = fs::temp_directory_path() / "testing_matcher";
  fs::create_directories(path);
  const fs::path imagePath = path / "image.png";

  types::PersonView person{
    .id = "1",
    .firstName = "John",
    .lastName = "Doe",
    .modifiedAt = "2026-11-24",
    .count = 5,
    .index = 1,
    .image = imagePath.string(),
    .match = 0.95
  };

  ui::Matcher matcher;
  matcher.show();
  ASSERT_TRUE(QTest::qWaitForWindowExposed(&matcher, 50));

  {
    matcher.onNewPerson(person);
    QTest::qWait(50);

    const QLabel* label = matcher.findChild<QLabel*>("imageLabel");
    ASSERT_NE(label, nullptr);
    EXPECT_EQ(label->text(), "No Image");
  }

  {
    cv::imwrite(
      imagePath.string(), cv::Mat(3040, 4056, CV_16UC1, cv::Scalar(65535))
    );
    person.match = 0.95;
    matcher.onNewPerson(person);
    QTest::qWait(50);

    const QLabel* label = matcher.findChild<QLabel*>("matchLabel");
    ASSERT_NE(label, nullptr);
    EXPECT_EQ(label->text(), "95.00 %");

    person.match = 0.85;
    matcher.onNewPerson(person);
    QTest::qWait(50);

    label = matcher.findChild<QLabel*>("matchLabel");
    ASSERT_NE(label, nullptr);
    EXPECT_EQ(label->text(), "85.00 %");
  }

  {
    person.match = 0.75;
    matcher.onNewPerson(person);
    QTest::qWait(50);

    const QLabel* label = matcher.findChild<QLabel*>("idLabel");
    ASSERT_NE(label, nullptr);
    EXPECT_EQ(label->text(), QString::fromStdString(person.id));
  }

  {
    matcher.onNewPerson(person);
    QTest::qWait(50);

    const QLabel* label = matcher.findChild<QLabel*>("firstNameLabel");
    ASSERT_NE(label, nullptr);
    EXPECT_EQ(label->text(), QString::fromStdString(person.firstName));
  }

  {
    matcher.onNewPerson(person);
    QTest::qWait(50);

    const QLabel* label = matcher.findChild<QLabel*>("lastNameLabel");
    ASSERT_NE(label, nullptr);
    EXPECT_EQ(label->text(), QString::fromStdString(person.lastName));
  }

  {
    matcher.onNewPerson(person);
    QTest::qWait(50);

    const QLabel* label = matcher.findChild<QLabel*>("modifiedAtLabel");
    ASSERT_NE(label, nullptr);
    EXPECT_EQ(label->text(), QString::fromStdString(person.modifiedAt));
  }

  {
    matcher.onNewPerson(person);
    QTest::qWait(50);

    const QLabel* label = matcher.findChild<QLabel*>("countLabel");
    ASSERT_NE(label, nullptr);
    EXPECT_EQ(label->text(), QString::number(person.count));
  }

  {
    matcher.onNewPerson(person);
    QTest::qWait(50);

    const QLabel* label = matcher.findChild<QLabel*>("codeFileLabel");
    ASSERT_NE(label, nullptr);
    EXPECT_EQ(
      label->text(), QString::fromStdString(std::format("{}.bin", person.index))
    );
  }

  {
    matcher.onNewPerson(person);
    QTest::qWait(50);

    const QLabel* label = matcher.findChild<QLabel*>("imageFileLabel");
    ASSERT_NE(label, nullptr);
    EXPECT_EQ(
      label->text(), QString::fromStdString(std::format("{}.png", person.index))
    );
  }

  fs::remove_all(path);
}

TEST(Matcher, Clear)
{
  ui::Matcher matcher;
  matcher.show();

  matcher.clear();
  QTest::qWait(50);

  const QList<QLabel*> labels = matcher.findChildren<QLabel*>();
  for (const QLabel* label : labels)
  {
    ASSERT_NE(label, nullptr);

    const QString name = label->objectName();

    if (name == "imageLabel")
    {
      EXPECT_EQ(label->text(), "No Image");
    }
    else if (name == "matchLabel")
    {
      EXPECT_EQ(label->text(), "No Match");
    }
    else if (!name.isEmpty())
    {
      EXPECT_EQ(label->text(), "N/A");
    }
  }
}
