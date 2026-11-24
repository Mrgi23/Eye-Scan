/*
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2026 Aleksa Markovic <aleksamarkovic96@outlook.com>
 */
#include "database/Storage.hpp"

#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#include <QUuid>

#include <fstream>

TEST(Storage, Init)
{
  db::Storage& storage = db::Storage::instance();
  const fs::path path = fs::temp_directory_path() / QUuid::createUuid().toString().toStdString();
  fs::create_directories(path);

  {
    EXPECT_EQ(storage.initialize("do_not_exist.json", path), types::Status::FAIL) << "Expected failure for non-existent file";

    nlohmann::json json;
    std::ofstream(path / "example.json") << json.dump(2);
    EXPECT_EQ(storage.initialize(path / "example.json", path), types::Status::FAIL) << "Expected failure for empty JSON file";

    json["url"] = "http://example.com/database";
    json["sas"] = "sas_token";
    std::ofstream(path / "example.json") << json.dump(2);
    EXPECT_THROW(storage.initialize(path / "example.json", path), std::invalid_argument) << "Expected exception for invalid storage url";
  }

  {
    storage.deinitialize();
    EXPECT_EQ(storage.initialize("../../database/auth.test.json", path), types::Status::SUCCESS) << "Expected to initialize successfully with a valid configuration";
    EXPECT_EQ(storage.initialize("do_not_exist.json", path), types::Status::SUCCESS) << "Expected success after initial successful init";
  }

  storage.deinitialize();
  fs::remove_all(path);
}

TEST(Storage, Sync)
{
  db::Storage& storage = db::Storage::instance();
  const fs::path path = fs::temp_directory_path() / QUuid::createUuid().toString().toStdString();
  fs::create_directories(path);

  fs::path authPath = "../../database/auth.test.json";

  {
    EXPECT_EQ(storage.sync(), types::Status::FAIL) << "Expected failure when syncing without initializing the storage";
    EXPECT_EQ(storage.size(), 0) << "Expected size to be 0 when storage is not initialized.";
  }

  {
    storage.initialize(authPath, path);

    fs::path subDir = path / QUuid::createUuid().toString().toStdString();
    fs::create_directories(subDir);
    std::ofstream(subDir / "example.json") << "{}";

    ASSERT_EQ(storage.sync(), types::Status::SUCCESS) << "Expected successful sync.";
    EXPECT_EQ(storage.size(), 1) << "Expected storage size to be 1 after adding one directory.";
  }

  storage.deinitialize();
  fs::remove_all(path);
}

TEST(Storage, Clear)
{
  db::Storage& storage = db::Storage::instance();
  const fs::path path = fs::temp_directory_path() / QUuid::createUuid().toString().toStdString();
  fs::create_directories(path);

  fs::path authPath = "../../database/auth.test.json";

  {
    EXPECT_EQ(storage.clear(), types::Status::FAIL) << "Expected failure when clearing without initializing the storage";
  }

  {
    storage.initialize(authPath, path);
    EXPECT_NE(storage.size(), 0) << "Expected storage not to be empty before clearing.";
    EXPECT_EQ(storage.clear(), types::Status::SUCCESS) << "Expected successful clear.";
    EXPECT_EQ(storage.size(), 0) << "Expected storage to be empty after clearing.";
  }

  storage.deinitialize();
  fs::remove_all(path);
}
