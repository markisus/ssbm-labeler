#include <QCryptographicHash>
#include <QDebug>
#include <QProcess>
#include <QSqlQuery>
#include <QStringList>
#include <QVariant>

#include "video_loader.h"

VideoLoader::VideoLoader(const std::string& target_directory,
                         const QSqlDatabase& db) :
    target_directory_(target_directory), db_(db) {
  db_.open();
}

std::string VideoLoader::DownloadVideo(const std::string& url) {
  QCryptographicHash hash_function(QCryptographicHash::Md4);
  hash_function.addData(url.c_str(), url.size());
  const std::string hashed_url = hash_function.result().toHex().toStdString();
  const std::string target_path = target_directory_ + "/" + hashed_url + ".mp4";

  qDebug() << "Going to try to download " << url.c_str() << " into " << target_path.c_str();

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

std::string VideoLoader::GetLocalPath(const std::string& url) {
  QSqlQuery query(db_);
  query.exec("select local_file_path from local_videos where url = :url");
  query.bindValue(":url", url.c_str());
  query.exec();

  while (query.next()) {
    qDebug() << "Found a local file path " << local_file_path;
    QString local_file_path = query.value(0).toString();
    return local_file_path.toStdString();
  }


  // If we didn't early return...
  qDebug() << "Need to download and install new video";
  std::string local_file_path = DownloadVideo(url);
  QSqlQuery insert_query(db_);
  insert_query.exec(
      "insert into local_videos (url, local_file_path) "
      "values (:url, :local_file_path)");
  insert_query.bindValue(":url", url.c_str());
  insert_query.bindValue(":local_file_path", local_file_path.c_str());
  insert_query.exec();

  return local_file_path;
};
