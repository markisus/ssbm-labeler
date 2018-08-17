// VideoSeekerImageProvider wraps a VideoSeeker as a QImageProvider
#include <video_seeker.h>
#include <QQuickImageProvider>
#include <QDebug>

class VideoSeekerImageProvider : public QQuickImageProvider {
 public:
  VideoSeekerImageProvider(const std::string& file_path) :
      QQuickImageProvider{QQuickImageProvider::Image},
      video_seeker_{file_path} {}

  int maxMsec() const {
    return (int)(video_seeker_.duration()*1000);
  }

  QImage requestImage(const QString& id,
                      QSize* size,
                      const QSize& requestedSize) override {
      const int msecs = id.toInt();
      const double secs = msecs/(double)1000;
      video_seeker_.Seek(secs);
    QImage result {
      video_seeker_.data(),
      video_seeker_.width(),
      video_seeker_.height(),
      QImage::Format_RGB32};
    return result;
  }
  
 private:
  lius_tools::VideoSeeker video_seeker_;
};
