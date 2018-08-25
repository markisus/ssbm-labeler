#include <QCryptographicHash>
#include <QProcess>
#include <QSqlQuery>
#include <QStringList>
#include <QVariant>

#include "video_loader.h"

VideoLoader::VideoLoader(const std::string& target_directory,
                         const QSqlDatabase& db) : target_directory_(target_directory),
                                                   db_(db) {}

std::string VideoLoader::DownloadVideo(const std::string& url) {
  QCryptographicHash hash_function(QCryptographicHash::Md4);
  hash_function.addData(url.c_str(), url.size());
  const std::string hashed_url = hash_function.result().toHex().toStdString();
  const std::string target_path = target_directory_ + "/" + hashed_url + ".mp4";

  QProcess youtube_dl_process;
  QString program { "youtube-dl"};
  QStringList arguments;
  arguments << "-o" << target_path.c_str() << "-f" << "mp4";
  youtube_dl_process.start(program, arguments);
  youtube_dl_process.waitForFinished();

  return target_path;
}

std::string VideoLoader::GetLocalPath(const std::string& url) {
  QSqlQuery query;
  query.exec("select local_file_path from local_videos where url = :url");
  query.bindValue(":url", url.c_str());
  query.exec();

  while (query.next()) {
    QString local_file_path = query.value(0).toString();
    return local_file_path.toStdString();
  }


  // If we didn't early return...
  std::string local_file_path = DownloadVideo(url);
  QSqlQuery insert_query;
  insert_query.exec(
      "insert into local_videos (url, local_file_path) "
      "values (:url, :local_file_path)");
  insert_query.bindValue(":url", url.c_str());
  insert_query.bindValue(":local_file_path", local_file_path.c_str());
  insert_query.exec();

  return local_file_path;
};
