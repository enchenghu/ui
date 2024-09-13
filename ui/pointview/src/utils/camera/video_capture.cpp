#include "video_capture.h"

#include <QMessageBox>

#include "ui_video_capture.h"
#include "utils.h"

namespace autox {
namespace pointview {
VideoCapture::VideoCapture(std::shared_ptr<DeviceContext> device_context)
    : QMainWindow(device_context->getDisplayContext()->getParent()),
      ui(new Ui::VideoCaptureUi) {
  ui->setupUi(this);
  device_context_ = device_context;
  // property
  open_button_ = std::make_shared<QPushButton>();
  open_button_->setText("Open");
  properties_.push_back({"Video Capture", open_button_});
  connect(open_button_.get(), &QPushButton::clicked, [this]() { Open(); });
  device_context_->addProperties(properties_);

  device_context->registerStartRecorderCb([this]() { StartRecorder(); });
  device_context->registerStopRecorderCb([this]() { StopRecorder(); });
  // player thread
  player_thread_ = std::make_unique<std::thread>([this]() {
    while (!exit_) {
      if (capture_.isOpened()) {
        cv::Mat frame;
        if (capture_.read(frame)) {
          cv::Mat q_img;
          if (frame.channels() == 4) {
            cv::cvtColor(frame, q_img, cv::COLOR_BGRA2RGB);
          } else if (frame.channels() == 3) {
            cv::cvtColor(frame, q_img, cv::COLOR_BGR2RGB);
          }
          QImage img = QImage(q_img.data, q_img.cols, q_img.rows, q_img.step,
                              QImage::Format_RGB888);
          ui->ImageLabel->setPixmap(QPixmap::fromImage(img));
          ui->ImageLabel->adjustSize();
          std::lock_guard<std::mutex> lock(recoder_mutex_);
          if (need_recoder_) {
            writer_->write(frame);
          }
        } else {
          using namespace std::chrono_literals;
          std::this_thread::sleep_for(1000ms);
        }
      }
    }
  });
}

VideoCapture::~VideoCapture() {
  exit_ = true;
  if (player_thread_->joinable()) {
    player_thread_->join();
  }
  delete ui;
  capture_.release();
}

void VideoCapture::on_SelectFilePushButton_clicked() {
  QString fileName =
      GetSaveFileName("Save videeo to file", QDir::homePath() + "/Untitled.mp4",
                      "PNG Files(*.mp4);;All Files(*.*)");
  if (fileName.isNull()) {
    Debug("Do not select a target file.");
    return;
  }
  ui->SavePathLabel->setText(fileName);
  save_file_ = fileName.toStdString();
}

void VideoCapture::on_OpenPushButton_clicked() {
  int id = ui->CameraComboBox->currentIndex();
  capture_.open(id);
  if (capture_.isOpened()) {
    //    int width = capture_.get(cv::CAP_PROP_FRAME_WIDTH);
    //    int height = capture_.get(cv::CAP_PROP_FRAME_HEIGHT);
    //    ui->ImageLabel->setFixedSize(width, height);
  } else {
    QMessageBox::information(nullptr, "Video Capture",
                             "Open failed! Do not have this device.",
                             QMessageBox::Ok);
  }
}

void VideoCapture::StartRecorder() {
  if (!save_file_.empty() && capture_.isOpened()) {
    cv::Size frameSize(capture_.get(cv::CAP_PROP_FRAME_WIDTH),
                       capture_.get(cv::CAP_PROP_FRAME_HEIGHT));
    int fps = capture_.get(cv::CAP_PROP_FPS);
    std::lock_guard<std::mutex> lock(recoder_mutex_);
    writer_ = std::make_shared<cv::VideoWriter>(
        save_file_, cv::VideoWriter::fourcc('m', 'p', '4', 'v'), fps,
        frameSize);
    if (!writer_->isOpened()) {
      QMessageBox::information(nullptr, "Video Capture",
                               "Can not create video file.", QMessageBox::Ok);
      return;
    }
    need_recoder_ = true;
  }
}

void VideoCapture::StopRecorder() {
  std::lock_guard<std::mutex> lock(recoder_mutex_);
  need_recoder_ = false;
  if (writer_) {
    writer_->release();
    writer_.reset();
  }
}

void VideoCapture::Open() {
  this->move(this->parentWidget()->geometry().center() - this->rect().center());
  this->show();
}
}  // namespace pointview
}  // namespace autox