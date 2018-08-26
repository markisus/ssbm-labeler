#include <QCryptographicHash>
#include <QDebug>
#include <QProcess>
#include <QSqlQuery>
#include <QSqlError>
#include <QStringList>
#include <QVariant>

#include "video_loader.h"

VideoLoader::VideoLoader(const std::string& target_directory,
                         const QSqlDatabase& db) :
    target_directory_(target_directory), db_(db) {
  db_.open();
  InstallTableIfNotExists();
}

std::string VideoLoader::DownloadVideo(const std::string& url) {
  QCryptographicHash hash_function(QCryptographicHash::Md4);
  hash_function.addData(url.c_str(), url.size());
  const std::string hashed_url = hash_function.result().toHex().toStdString();
  const std::string target_path = target_directory_ + "/" + hashed_url + ".mp4";

  QProcess youtube_dl_process;
  QString program { "youtube-dl"};
  QStringList arguments;
  arguments << "-o" << target_path.c_str()
            << "-f" << "mp4"
            << url.c_str();
  youtube_dl_process.start(program, arguments);
  youtube_dl_process.waitForFinished();

  qDebug() << "Processed finished";
  qDebug() << youtube_dl_process.readAllStandardOutput();


  if (youtube_dl_process.exitCode() != QProcess::NormalExit) {
      qDebug() << "Abnormal exit";
      qDebug() << youtube_dl_process.readAllStandardError();
      exit(-1);
  }
  return target_path;
}

void VideoLoader::InstallTableIfNotExists() {
  QSqlQuery query(db_);
  if(!query.exec("create table if not exists local_videos "
                 "(url text primary key, local_file_path text)")) {
    qDebug() << "Could not create local_videos table";
    qDebug() << query.lastError();
  };
}

std::string VideoLoader::GetLocalPath(const std::string& url) {
  QSqlQuery query(db_);
  query.prepare("select local_file_path from local_videos where url = :url");
  query.bindValue(":url", url.c_str());
  if(!query.exec()) {
    qDebug() << "Local path query failed " << query.lastError();
    exit(-1);
  };

  while (query.next()) {
    QString local_file_path = query.value(0).toString();
    return local_file_path.toStdString();
  }

  // If we didn't early return...
  std::string local_file_path = DownloadVideo(url);
  QSqlQuery insert_query(db_);
  insert_query.prepare(
      "insert into local_videos (url, local_file_path) "
      "values (:url, :local_file_path)");
  insert_query.bindValue(":url", url.c_str());
  insert_query.bindValue(":local_file_path", local_file_path.c_str());
  if(!insert_query.exec()) {
    qDebug() << "Local path insert failed " << insert_query.lastError();
    qDebug() << insert_query.lastError();
  };

  return local_file_path;
};
