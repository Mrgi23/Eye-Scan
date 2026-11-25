/*
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2026 Aleksa Markovic <aleksamarkovic96@outlook.com>
 */
#include "database/Database.hpp"

#include <gtest/gtest.h>

TEST(Database, Init)
{
  db::Database& database = db::Database::instance();
  const fs::path path = fs::temp_directory_path() / QUuid::createUuid().toString().toStdString();
  fs::create_directories(path);

  {
    EXPECT_EQ(database.initialize(path / "do_not_exist"), types::Status::FAIL) << "Expected failure with a non-existent path";
  }

  {
    EXPECT_EQ(database.initialize(path), types::Status::SUCCESS) << "Expected to initialize successfully with a valid path";
    EXPECT_EQ(database.initialize(path / "do_not_exist"), types::Status::SUCCESS) << "Expected success after initial successful init";
    EXPECT_EQ(database.size(), 0) << "Database should not contain any entries.";
  }

  database.deinitialize();
  fs::remove_all(path);
}

TEST(Database, Add)
{
  db::Database& database = db::Database::instance();
  const fs::path path = fs::temp_directory_path() / QUuid::createUuid().toString().toStdString();
  fs::create_directories(path);

  {
    db::Info info;
    EXPECT_EQ(database.add(info), types::Status::FAIL) << "Expected failure when adding a person without initializing the database";

    database.initialize(path);
    EXPECT_EQ(database.add(info), types::Status::FAIL) << "Expected failure when adding a person without an image";

    database.deinitialize();
  }

  {
    database.initialize(path);

    db::Info info;
    info.image = cv::Mat::zeros(100, 100, CV_16UC1);
    info.code = {std::bitset<BITSET_SIZE>(), std::bitset<BITSET_SIZE>().set()};

    EXPECT_EQ(database.add(info), types::Status::FAIL) << "Expected failure when adding a new person without first and last name";

    info.firstName = "John";
    info.lastName = "Doe";
    EXPECT_EQ(database.add(info), types::Status::SUCCESS) << "Expected success when adding a new person with first and last name";
    EXPECT_EQ(database.size(), 1) << "Expected database size to be 1 after adding a new person";
  }

  {
    db::Info info;
    info.image = cv::Mat::zeros(100, 100, CV_16UC1);
    info.id = QUuid::createUuid();
    EXPECT_EQ(database.add(info), types::Status::FAIL) << "Expected failure when adding a new person without first and last name";

    info.firstName = "Jane";
    info.lastName = "Doe";
    info.code = {std::bitset<BITSET_SIZE>(), std::bitset<BITSET_SIZE>().set()};
    EXPECT_EQ(database.add(info), types::Status::SUCCESS) << "Expected success when adding a new person with first and last name";
    EXPECT_EQ(database.size(), 2) << "Expected database size to be 2 after adding another new person";

    db::Info newInfo;
    newInfo.id = info.id;
    newInfo.image = cv::Mat::zeros(100, 100, CV_16UC1);
    newInfo.code = {std::bitset<BITSET_SIZE>(), std::bitset<BITSET_SIZE>().set()};

    EXPECT_EQ(database.add(newInfo), types::Status::SUCCESS) << "Expected success when adding a code for an existing person";
    EXPECT_EQ(database.add(newInfo), types::Status::SUCCESS) << "Expected success when adding a code for an existing person";
    EXPECT_EQ(database.add(newInfo), types::Status::SUCCESS) << "Expected success when adding a code for an existing person";
    EXPECT_EQ(database.add(newInfo), types::Status::SUCCESS) << "Expected success when adding a code for an existing person";
    EXPECT_EQ(database.add(newInfo), types::Status::FAIL) << "Expected failure when adding a code beyond the maximum limit for an existing person";
    EXPECT_EQ(database.size(), 2) << "Expected database size to remain 2 after adding multiple codes for an existing person";

  }

  {
    database.deinitialize();
    EXPECT_EQ(database.size(), 0) << "Expected database to be empty after clearing";

    fs::create_directories(path / QUuid::createUuid().toString().toStdString());

    database.initialize(path);
    EXPECT_NE(database.size(), 0) << "Expected database to still load the previously added person";
  }

  database.deinitialize();
  fs::remove_all(path);
}

TEST(Database, Find)
{
  db::Database& database = db::Database::instance();
  const fs::path path = fs::temp_directory_path() / QUuid::createUuid().toString().toStdString();
  fs::create_directories(path);

  {
    types::PersonView personView;
    EXPECT_EQ(database.find({}, personView), types::Status::FAIL) << "Expected failure when finding a person without initializing the database";
    database.initialize(path);
  }

  {
    std::bitset<BITSET_SIZE> data;
    data[0] = 1;
    data[4] = 1;
    data[7] = 1;
    std::bitset<BITSET_SIZE> mask;
    mask.set();
    types::EyeCode code = {data, mask};

    db::Info info;
    info.firstName = "John";
    info.lastName = "Doe";
    info.image = cv::Mat::zeros(100, 100, CV_8UC3);
    info.code = code;
    database.add(info);

    types::EyeCode invalidCode;
    invalidCode.mask.set();
    for (size_t i = 0; i < BITSET_SIZE / 2; ++i)
      invalidCode.data.set(i);

    types::PersonView notFoundView;
    EXPECT_EQ(database.find(invalidCode, notFoundView), types::Status::FAIL) << "Expected failure when finding a non-existent person";

    types::PersonView foundView;
    ASSERT_EQ(database.find(code, foundView), types::Status::SUCCESS) << "Expected success when finding the previously added person";
    EXPECT_EQ(foundView.firstName, "John") << "Expected found person's first name to match";
    EXPECT_EQ(foundView.lastName, "Doe") << "Expected found person's last name to match";
  }

  database.deinitialize();
  fs::remove_all(path);
}
