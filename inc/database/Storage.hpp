  /*
  * SPDX-License-Identifier: Apache-2.0
  * Copyright (c) 2026 Aleksa Markovic <aleksamarkovic96@outlook.com>
  */
  #pragma once

  #include "utility/Enums.hpp"
  #include "utility/Macros.hpp"

  #include <QNetworkAccessManager>

  #include <filesystem>

  namespace fs = std::filesystem;

  /**
   * @namespace db
   * @brief Database-related utilities and operations.
   */
  namespace db
  {
    /**
     * @class Storage
     * @brief Represents the cloud storage for managing authentication and data.
     */
    class Storage
    {
      public:
        /**
         * @brief Get the singleton instance of the Storage.
         * @return Reference to the Storage instance.
         */
        static Storage& instance();

        DISABLE_COPY_AND_MOVE(Storage);

        /**
         * @brief Initializes the storage with authentication and data paths.
         *
         * @param authPath Path to the authentication file, defaults to "./auth.json".
         * @param dataPath Path to the data directory, defaults to "./database".
         * @return Status::SUCCESS if the storage was initialized successfully,
         *         Status::FAIL if the storage could not be initialized.
         * @throws std::invalid_argument if the HTTP GET request fails.
         */
        types::Status initialize(const fs::path& = "./auth.json", const fs::path& = "./database");

        /**
         * @brief Deinitializes the local storage.
         *
         * @return Status::SUCCESS if the storage was deinitialized successfully.
         */
        types::Status deinitialize();

        /**
         * @brief Clears the storage, releasing any resources.
         *
         * @return Status::SUCCESS if the storage was cleared successfully.
         * @throws std::invalid_argument if the HTTP GET request fails.
         */
        types::Status clear();

        /**
         * @brief Uploads the storage container contents.
         *
         * @return Status::SUCCESS if the storage was synchronized successfully,
         *         Status::FAIL if the storage could not be synchronized.
         */
        types::Status sync();

        /**
         * @brief Gets the number of files in the storage container.
         *
         * @return The number of files.
         * @throws std::invalid_argument if the HTTP GET request fails.
         */
        size_t size();

      private:
        /**
         * @brief Constructs new Storage object.
         */
        Storage() noexcept = default;

        /**
         * @brief Destroys the Storage object.
         */
        ~Storage() noexcept = default;

        /**
         * @brief Downloads the storage container contents.
         *
         * @throws std::invalid_argument if the HTTP GET request fails.
         */
        void download();

        /**
         * @brief Performs an HTTP GET request.
         *
         * @param std::string The URL to fetch.
         * @param QByteArray The data to store the response.
         * @throws std::invalid_argument if the GET request fails.
         */
        void get(const std::string&, QByteArray&);

        /**
         * @brief Performs an HTTP PUT request.
         *
         * @param std::string The URL to send data to.
         * @param QByteArray The data to upload.
         */
        void put(const std::string&, const QByteArray&);

        /**
         * @brief Performs an HTTP DELETE request.
         *
         * @param std::string The URL to delete.
         */
        void del(const std::string&);

      private:
        QNetworkAccessManager m_manager;
        std::string m_url;
        std::string m_token;
        fs::path m_path;
    };
  } // namespace db
