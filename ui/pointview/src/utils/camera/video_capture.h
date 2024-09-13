#ifndef VIDEO_CAPTURE_H
#define VIDEO_CAPTURE_H

#include <QMainWindow>
#include <QPushButton>
#include <thread>

#include <opencv2/opencv.hpp>

#include "utils/common/device_context.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class VideoCaptureUi;
}
QT_END_NAMESPACE

namespace autox {
namespace pointview {
class VideoCapture : public QMainWindow {
  Q_OBJECT

 public:
  VideoCapture(std::shared_ptr<DeviceContext> device_context);
  ~VideoCapture();

 private slots:
  void on_SelectFilePushButton_clicked();
  void on_OpenPushButton_clicked();
  void Open();

 private:
  void StartRecorder();
  void StopRecorder();

 private:
  std::shared_ptr<DeviceContext> device_context_;
  std::shared_ptr<QPushButton> open_button_;
  PropertyPairs properties_;
  Ui::VideoCaptureUi* ui;
  cv::VideoCapture capture_;
  std::shared_ptr<cv::VideoWriter> writer_;
  std::mutex recoder_mutex_;
  bool need_recoder_{false};
  std::string save_file_;
  std::unique_ptr<std::thread> player_thread_;
  bool exit_{false};
};
}  // namespace pointview
}  // namespace autox
#endif  // VIDEO_CAPTURE_H
