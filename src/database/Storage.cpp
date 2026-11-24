/*
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2026 Aleksa Markovic <aleksamarkovic96@outlook.com>
 */
#include "database/Storage.hpp"
#include "utility/Logger.hpp"

#include <nlohmann/json.hpp>
#include <QDomDocument>
#include <QEventLoop>
#include <QFile>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>

#include <fstream>
#include <unordered_set>

namespace db
{
  Storage& Storage::instance()
  {
    static Storage instance;
    return instance;
  }

  types::Status Storage::initialize(const fs::path& authPath, const fs::path& dataPath)
  {
    if (!m_path.empty())
      return logger::Info("Storage: Storage is already initialized.");

    if (!fs::exists(authPath))
      return logger::Warning(std::format("Storage: Auth file does not exist: {}.", authPath.string()));

    std::ifstream stream(authPath);
    nlohmann::json json;
    stream >> json;

    if (!json.contains("url") || !json.contains("sas"))
      return logger::Warning("Storage: Auth file is missing required fields.");

    m_url = json["url"].get<std::string>();
    m_token = json["sas"].get<std::string>();

    m_path = dataPath;

    download();
    return logger::Info("Storage: Storage downloaded successfully.");
  }

  types::Status Storage::deinitialize()
  {
    m_path.clear();
    m_url.clear();
    m_token.clear();
    return types::Status::SUCCESS;
  }

  types::Status Storage::clear()
  {
    if (m_url.empty())
    {
      return logger::Warning("Storage: Storage is not initialized. Call initialize first.");
    }

    QByteArray xml;
    get(std::format("{}?restype=container&comp=list&{}", m_url, m_token), xml);

    QDomDocument doc;
    doc.setContent(xml);

    const auto names = doc.elementsByTagName("Name");
    for (int i = 0; i < names.count(); ++i)
    {
      const QString blob = names.at(i).toElement().text();
      if (blob.startsWith("Eye-Scan/"))
        del(std::format("{}/{}?{}", m_url, blob.toStdString(), m_token));
    }

    deinitialize();
    return logger::Info("Storage: Storage cleared successfully.");
  }

  types::Status Storage::sync()
  {
    if (m_path.empty())
      return logger::Warning("Storage: Storage is not initialized. Call initialize first.");

    for (const auto& entry : fs::recursive_directory_iterator(m_path))
    {
      if (fs::is_regular_file(entry))
      {
        const fs::path relative = fs::relative(entry.path(), m_path);
        const QString blob = QString::fromStdString("Eye-Scan/" + relative.generic_string());
        QFile file(QString::fromStdString(entry.path().string()));

        file.open(QIODevice::ReadOnly);
        const QByteArray data = file.readAll();
        file.close();
        put(std::format("{}/{}?{}", m_url, blob.toStdString(), m_token), data);
      }
    }
    return logger::Info("Storage: Sync completed successfully.");
  }

  size_t Storage::size()
  {
    if (m_url.empty())
    {
      logger::Warning("Storage: Storage is not initialized. Call initialize first.");
      return 0;
    }

    QByteArray xml;
    get(std::format("{}?restype=container&comp=list&{}", m_url, m_token), xml);

    QDomDocument doc;
    doc.setContent(xml);

    std::unordered_set<std::string> uuids;
    const auto names = doc.elementsByTagName("Name");
    for (int i = 0; i < names.count(); ++i)
    {
      const QString blob = names.at(i).toElement().text();
      if (blob.startsWith("Eye-Scan/"))
      {
        const QString relative = blob.mid(QString("Eye-Scan/").length());
        const QString uuid = relative.section('/', 0, 0);
        uuids.insert(uuid.toStdString());
      }
    }
    return uuids.size();
  }

  void Storage::download()
  {
    fs::create_directories(m_path);

    QByteArray xml;
    get(std::format("{}?restype=container&comp=list&{}", m_url, m_token), xml);

    QDomDocument doc;
    doc.setContent(xml);

    const auto names = doc.elementsByTagName("Name");
    for (int i = 0; i < names.count(); ++i)
    {
      const QString blob = names.at(i).toElement().text();
      if (blob.startsWith("Eye-Scan/"))
      {
        const QString relative = blob.mid(QString("Eye-Scan/").length());
        const fs::path filePath = m_path / relative.toStdString();
        if (!fs::exists(filePath))
        {
          QByteArray data;
          get(std::format("{}/{}?{}", m_url, blob.toStdString(), m_token), data);

          fs::create_directories(filePath.parent_path());
          QFile file(QString::fromStdString(filePath.string()));

          file.open(QIODevice::WriteOnly);
          file.write(data);
          file.close();
        }
      }
    }
  }

  void Storage::get(const std::string& url, QByteArray& data)
  {
    const QNetworkRequest request(QUrl(QString::fromStdString(url)));
    QNetworkReply* const reply = m_manager.get(request);

    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    if (reply->error() != QNetworkReply::NoError)
    {
      const QString error = reply->errorString();
      reply->deleteLater();
      logger::Error(
        std::format("Storage: Failed to get URL: {} Error: {}", url, error.toStdString()),
        types::Error::INVALID_ARGUMENT
      );
    }

    data = reply->readAll();
    reply->deleteLater();
  }

  void Storage::put(const std::string& url, const QByteArray& data)
  {
    QNetworkRequest request(QUrl(QString::fromStdString(url)));
    request.setRawHeader("x-ms-blob-type", "BlockBlob");
    QNetworkReply* const reply = m_manager.put(request, data);

    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    reply->deleteLater();
  }

  void Storage::del(const std::string& url)
  {
    const QNetworkRequest request(QUrl(QString::fromStdString(url)));
    QNetworkReply* const reply = m_manager.deleteResource(request);

    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    reply->deleteLater();
  }
} // namespace db
