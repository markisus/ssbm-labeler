#include <QDebug>
#include <QGuiApplication>
#include <QImage>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "video_loader.h"
#include "video_seeker_image_provider.h"

const char* SSBM_DB_FILENAME = "ssbm-labler.db";

void write_test_image(const std::string& video_path) {
    lius_tools::VideoSeeker seeker{video_path};
    QImage result {
      seeker.data(),
      seeker.width(),
      seeker.height(),
      QImage::Format_RGB32};
    result.save("result.png");
}

int main(int argc, char *argv[])
{
  QGuiApplication app(argc, argv);
  const std::string video_path = "C:/Users/marki/Videos/smash0.mp4";

  {
    // Need to intialize the app db with the correct tables
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(SSBM_DB_FILENAME);
    VideoLoader video_loader { app.applicationDirPath().toStdString(), db };
    std::string local_path =
        video_loader.GetLocalPath("https://www.youtube.com/watch?v=-OMZm1wyCVQ");
    qDebug() << "Local path to video " << local_path.c_str();
  }

  VideoSeekerImageProvider* video_seeker_image_provider =
  new VideoSeekerImageProvider { video_path };

  const double max_msec = video_seeker_image_provider->maxMsec();

  QQmlApplicationEngine engine;
  engine.addImageProvider("ssbm_video", video_seeker_image_provider);
  engine.rootContext()->setContextProperty("max_msec", max_msec);
  engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
  if (engine.rootObjects().isEmpty()) {
    return -1;
  }

  return app.exec();
}
