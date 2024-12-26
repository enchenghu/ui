#ifndef CAMERA_IMAGE_H
#define CAMERA_IMAGE_H

#include <QHBoxLayout>
#include <QLabel>
#include <QMainWindow>
#include <QPushButton>
#include <QScrollArea>
#include <QSpacerItem>
#include <QTimer>
#include <memory>

#include "utils/common/device_context.h"
#include "utils/config.h"
#include "utils/property_tree.h"
#include "utils/io/udp_input.h"
#include "utils/common/safe_queue.h"
namespace Ui {
class CameraImage;
}
namespace autox {
namespace pointview {
class CameraImage : public QMainWindow
{
  Q_OBJECT

 public:
  explicit CameraImage(std::shared_ptr<DeviceContext> device_context, int, int);
  ~CameraImage();
  void openUdpPort(int port = 2380);
  void Update();

 private slots:
  void on_openButtonClicked();
  void on_saveButtonClicked();
  void on_RecordButton_2_clicked();
 protected:
  void closeEvent(QCloseEvent* event) override;
  void showEvent(QShowEvent* event) override;
 private:
  Ui::CameraImage *ui;
  std::shared_ptr<DeviceContext> device_context_;
  PropertyPairs properties_;
  std::shared_ptr<QPushButton> open_button_;
  void process(std::vector<std::vector<uint8_t>>);
  QImage current_image_;
  QLabel* imageLabel_;
  QScrollArea* scrollArea_;
  QString status_str_;
  int width_, height_;
  bool render_{false};
  bool auto_refresh_{true};
  std::shared_ptr<UdpInput> udp_input_;
  std::unique_ptr<std::thread> player_thread_;
  std::unique_ptr<std::thread> process_thread_;
  void ParseCameraPacket(const uint8_t*, size_t);
  bool CheckPacket(const uint8_t*, size_t);
  int udp_receive_port_{2380};
  std::vector<std::vector<uint8_t>> pixel_frame_;
  bool exit_{false};
  SafeQueue<std::vector<std::vector<uint8_t>>> pixel_frame_queue_free;
  SafeQueue<std::vector<std::vector<uint8_t>>> pixel_frame_queue_done;
  SafeQueue<QImage> image_frame_queue_free;
  SafeQueue<QImage> image_frame_queue_done;  
  void resetBufferQueue();
  void ShowImage(const QImage&);
  bool SaveImage(QString);
  // record
  bool is_record_{false};
  QString record_dir_;
  int record_id_{0};
  void SetRecordButtonState();
  void set_status();

};

}
}

#endif // CAMERA_IMAGE_H
