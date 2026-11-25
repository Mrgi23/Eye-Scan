/*
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2026 Aleksa Markovic <aleksamarkovic96@outlook.com>
 */
#pragma once

#include "utility/Enums.hpp"
#include "utility/Macros.hpp"
#include "utility/Types.hpp"

#include <opencv2/core/mat.hpp>
#include <QDateTime>
#include <QString>
#include <QUuid>

#include <array>
#include <filesystem>
#include <map>

namespace fs = std::filesystem;

constexpr size_t MAX_CODES = 5;

/**
 * @namespace db
 * @brief Database-related utilities and operations.
 */
namespace db
{
  /**
   * @struct Info
   * @brief Represents the information of a person to be added to the database.
   *
   * @param std::optional<QUuid> The optional ID of the person.
   * @param std::optional<QString> The optional first name of the person.
   * @param std::optional<QString> The optional last name of the person.
   * @param cv::Mat The image of the person.
   * @param types::EyeCode The code of the person.
   */
  struct Info
  {
    std::optional<QUuid> id;
    std::optional<QString> firstName;
    std::optional<QString> lastName;
    cv::Mat img;
    types::EyeCode code;
  };

  /**
   * @struct Person
   * @brief Represents a person stored in the database.
   *
   * @param QUuid The unique ID of the person.
   * @param QString The first name of the person.
   * @param QString The last name of the person.
   * @param QDateTime The timestamp of the last modification.
   * @param std::array<types::EyeCode, MAX_CODES> The codes of the person.
   */
  struct Person
  {
    QUuid id;
    QString firstName;
    QString lastName;
    QDateTime modifiedAt;
    std::array<types::EyeCode, MAX_CODES> codes;
    size_t count;
  };

  /**
   * @class Database
   * @brief Represents the database for storing and managing people.
   */
  class Database
  {
    public:
      /**
       * @brief Get the singleton instance of the Database.
       *
       * @return Reference to the Database instance.
       */
      static Database& instance();

      DISABLE_COPY_AND_MOVE(Database);

      /**
       * @brief Initialize the database from the specified file path.
       *
       * @param fs::path The path to the database file, defaults to "./database".
       * @return Status::SUCCESS if the database was initialized successfully,
       *         Status::FAIL if the database could not be initialized.
       */
      types::Status initialize(const fs::path& = "./database");

      /**
       * @brief Deinitialize the database, removing all stored people.
       *
       * @return Status::SUCCESS after the database was deinitialized successfully,
       */
      types::Status deinitialize();

      /**
       * @brief Add a new person to the database.
       *
       * @param Info The information of the person to add.
       * @return Status::SUCCESS if the person was added successfully,
       *         Status::FAIL if the person could not be added.
       */
      virtual types::Status add(const Info&);

      /**
       * @brief Find a person in the database by their binary code.
       *
       * @param types::EyeCode The code to search for.
       * @param Person The found person.
       * @param double The distance of the found person.
       * @return Status::SUCCESS if the person was found,
       *         Status::FAIL if the person was not found.
       */
      virtual types::Status find(const types::EyeCode&, Person&, double&);

      /**
       * @brief Get the number of people in the database.
       *
       * @return The number of people in the database.
       */
      size_t size() const noexcept;

      /**
       * @brief Computes the distance between two eye codes.
       *
       * @param types::EyeCode The first eye code.
       * @param types::EyeCode The second eye code.
       * @return double The computed distance.
       */
      static double distance(const types::EyeCode&, const types::EyeCode&);

    protected:
      /**
       * @brief Constructs new Database object.
       */
      Database() noexcept = default;

      /**
       * @brief Destroys the Database object.
       */
      virtual ~Database() noexcept = default;

    private:
      /**
       * @brief Load a person from the database.
       * @param fs::path The path to the person's directory.
       *
       * @return An optional containing the Person if successfully loaded, or std::nullopt otherwise.
       */
      static std::optional<Person> loadPerson(const fs::path& dir);

      /**
       * @brief Save a person to the database.
       *
       * @param fs::path The path to the person's directory.
       * @param Person The person to save.
       * @param cv::Mat The image of the person to save.
       */
      static void savePerson(const fs::path&, const Person&, const cv::Mat&);

    private:
      fs::path m_path;
      std::map<QUuid, Person> m_people;
  };
} // namespace db
