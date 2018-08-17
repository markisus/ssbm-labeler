#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QDebug>
#include <QImage>
#include "video_seeker_image_provider.h"

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
  //write_test_image(video_path);

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
