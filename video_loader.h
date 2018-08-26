#ifndef VIDEO_LOADER_H
#define VIDEO_LOADER_H

#include <QSqlDatabase>
#include <string>
#include <vector>

class VideoLoader
{
 public:
  VideoLoader(const std::string& target_directory,
              const QSqlDatabase& db);

  std::string GetLocalPath(const std::string& url);
  
 private:
  void InstallTableIfNotExists();
  std::string DownloadVideo(const std::string& url);

  QSqlDatabase db_;
  std::string target_directory_;
};

#endif // VIDEO_LOADER_H
