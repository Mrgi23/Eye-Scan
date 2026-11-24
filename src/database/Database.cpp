/*
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2026 Aleksa Markovic <aleksamarkovic96@outlook.com>
 */
#include "database/Database.hpp"
#include "utility/Logger.hpp"

#include <nlohmann/json.hpp>
#include <opencv2/imgcodecs.hpp>

#include <fstream>
#include <limits>

constexpr double MATCH_THRESHOLD = 0.35;

namespace db
{
  Database& Database::instance()
  {
    static Database instance;
    return instance;
  }

  types::Status Database::initialize(const fs::path& path)
  {
    if (!m_path.empty())
      return types::Status::SUCCESS;

    if (!fs::exists(path))
      return logger::Warning(std::format("Database: Database file does not exist: {}.", path.string()));

    m_path = fs::weakly_canonical(path);
    m_people.clear();

    for (const auto& dir : fs::directory_iterator(m_path))
    {
      if (fs::is_directory(dir))
      {
        const auto person = loadPerson(dir);
        if (person.has_value())
          m_people[person->id] = std::move(*person);
      }
    }
    return logger::Info("Database: Database loaded successfully.");
  }

  types::Status Database::deinitialize()
  {
    m_path.clear();
    m_people.clear();
    return logger::Info("Database: Database cleared successfully.");
  }

  types::Status Database::add(const Info& info)
  {
    if (m_path.empty())
      return logger::Warning("Database: Database not initialized. Call initialize first.");

    if (info.image.empty())
      return logger::Warning("Database: Image must be provided.");

    Person person;
    if (info.id.has_value())
    {
      if (m_people.find(*info.id) == m_people.end())
      {
        if (!info.firstName.has_value() || !info.lastName.has_value())
          return logger::Warning("Database: First name and last name must be provided for new person.");

        person.id = *info.id;
        person.firstName = *info.firstName;
        person.lastName = *info.lastName;
        person.count = 0;
      }
      else
      {
        person = m_people.at(*info.id);
        if (person.count == MAX_CODES)
          return logger::Warning("Database: Maximum number of codes reached for this person.");
      }
    }
    else
    {
      if (!info.firstName.has_value() || !info.lastName.has_value())
        return logger::Warning("Database: First name and last name must be provided for new person.");

      person.id = QUuid::createUuid();
      person.firstName = *info.firstName;
      person.lastName = *info.lastName;
      person.count = 0;
    }

    person.modifiedAt = QDateTime::currentDateTime();

    const fs::path dir = m_path / person.id.toString().toStdString();
    const fs::path image = dir / "images" / std::format("{}.png", person.count);
    person.images[person.count] = QString::fromStdString(image.string());
    person.codes[person.count++] = info.code;

    m_people[person.id] = std::move(person);
    savePerson(dir, m_people.at(person.id), info.image);
    return logger::Info("Database: Person added successfully.");
  }

  types::Status Database::find(const types::EyeCode& code, types::PersonView& person)
  {
    if (m_path.empty())
      return logger::Warning("Database: Database not initialized. Call initialize first.");

    const Person* bestPerson = nullptr;
    double bestDistance = std::numeric_limits<double>::max();
    size_t bestIndex = 0;
    for (const auto& candidate : m_people)
    {
      for (size_t i = 0; i < candidate.second.count; ++i)
      {
        const double distance = Database::distance(candidate.second.codes[i], code);
        if (distance < bestDistance)
        {
          bestDistance = distance;
          bestPerson = &candidate.second;
          bestIndex = i;
        }
      }
    }

    if (!bestPerson || bestDistance > MATCH_THRESHOLD)
      return logger::Warning("Database: Person not found.");

    person = types::PersonView{
      .id = bestPerson->id.toString().toStdString(),
      .firstName = bestPerson->firstName.toStdString(),
      .lastName = bestPerson->lastName.toStdString(),
      .modifiedAt = bestPerson->modifiedAt.toString(Qt::ISODate).toStdString(),
      .count = bestPerson->count,
      .index = bestIndex,
      .image = bestPerson->images[bestIndex].toStdString(),
      .match = 1.0 - bestDistance
    };
    return logger::Info("Database: Person found successfully.");
  }

  double Database::distance(const types::EyeCode& lhs, const types::EyeCode& rhs)
  {
    const std::bitset<BITSET_SIZE> valid = lhs.mask & rhs.mask;
    const std::bitset<BITSET_SIZE> diff = (lhs.data ^ rhs.data) & valid;

    const size_t validCount = valid.count();
    return validCount ? static_cast<double>(diff.count()) / validCount : 1.0;
  }

  size_t Database::size() const noexcept
  {
    return m_people.size();
  }

  std::optional<Person> Database::loadPerson(const fs::path& dir)
  {
    const auto metadataPath = dir / "metadata.json";
    const auto imagesDir = dir / "images";
    const auto codesDir = dir / "codes";
    if (!fs::exists(metadataPath) ||  !fs::exists(imagesDir) || !fs::exists(codesDir))
    {
      logger::Warning(std::format("Database: Person metadata, images or codes not found: {}.", dir.string()));
      return std::nullopt;
    }
    std::ifstream stream(metadataPath);
    nlohmann::json json;
    stream >> json;

    Person person;
    person.id = QUuid::fromString(json.at("id").get<std::string>());
    person.firstName = QString::fromStdString(json.at("firstName").get<std::string>());
    person.lastName = QString::fromStdString(json.at("lastName").get<std::string>());
    person.modifiedAt = QDateTime::fromString(QString::fromStdString(json.at("modifiedAt").get<std::string>()), Qt::ISODate);
    person.count = json.at("count").get<size_t>();

    auto load = [&stream](std::bitset<BITSET_SIZE>& bits)
    {
      for (size_t i = 0; i < BITSET_SIZE / 8; ++i)
      {
        uint8_t byte{};
        stream.read(reinterpret_cast<char*>(&byte), sizeof(byte));
        for (size_t bit = 0; bit < 8; ++bit)
          bits[i * 8 + bit] = (byte >> bit) & 1;
      }
    };

    for (size_t i = 0; i < person.count; ++i)
    {
      const auto imagePath = imagesDir / std::format("{}.png", i);
      if (fs::exists(imagePath))
        person.images[i] = QString::fromStdString(imagePath.string());

      const auto codePath = codesDir / std::format("{}.bin", i);
      if (fs::exists(codePath))
      {
        stream.close();
        stream.open(codePath, std::ios::binary);
        load(person.codes[i].data);
        load(person.codes[i].mask);
      }
    }
    return person;
  }

  void Database::savePerson(const fs::path& dir, const Person& person, const cv::Mat& image)
  {
    fs::create_directories(dir);

    const auto metadataPath = dir / "metadata.json";
    nlohmann::json json;
    json["id"] = person.id.toString().toStdString();
    json["firstName"] = person.firstName.toStdString();
    json["lastName"] = person.lastName.toStdString();
    json["modifiedAt"] = person.modifiedAt.toString(Qt::ISODate).toStdString();
    json["count"] = person.count;
    std::ofstream(metadataPath) << json.dump(2);

    const auto imageDir = dir / "images";
    fs::create_directories(imageDir);
    cv::imwrite(person.images[person.count - 1].toStdString(), image);

    const auto codesDir = dir / "codes";
    fs::create_directories(codesDir);

    const auto codePath = codesDir / std::format("{}.bin", person.count - 1);
    std::ofstream stream(codePath, std::ios::binary);
    auto save = [&stream](const std::bitset<BITSET_SIZE>& bits)
    {
      for (size_t i = 0; i < BITSET_SIZE / 8; ++i)
      {
        uint8_t byte = 0;
        for (size_t bit = 0; bit < 8; ++bit)
          byte |= (bits[i * 8 + bit] << bit);
        stream.write(reinterpret_cast<char*>(&byte), sizeof(byte));
      }
    };
    save(person.codes[person.count - 1].data);
    save(person.codes[person.count - 1].mask);
  }
} // namespace db
