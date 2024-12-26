#include "camera_image.h"
#include "ui_camera_image.h"
#include <glog/logging.h>
#include <QMessageBox>
#include "utils/utils.h"
#include <QTime>
struct PacketHeader {
  uint8_t sop[2];
  uint8_t major_version;
  uint8_t minor_version;
  uint16_t data_size;
  uint16_t row_num;
  uint16_t column_num;
  uint16_t row_id;
  uint16_t first_point_column_id;
  uint8_t last_packet;
  uint8_t reserve;
};

struct CameraPacket {
  PacketHeader header;
  uint8_t data[0];
};

using namespace std::chrono_literals;
namespace autox {
namespace pointview {
CameraImage::CameraImage(std::shared_ptr<DeviceContext> device_context, int width,
                       int height) :
  width_(width),
  height_(height),
  QMainWindow(device_context->getDisplayContext()->getParent()),
  open_button_(new QPushButton(this)),
  imageLabel_(new QLabel(this)),
  scrollArea_(new QScrollArea(this)),
  ui(new Ui::CameraImage)
{
  ui->setupUi(this);
  device_context_ = device_context;
  open_button_->setText("Open");
  properties_.push_back({"Camera Image", open_button_});
  device_context_->addProperties(properties_);
  connect(open_button_.get(), SIGNAL(clicked(bool)), this,
          SLOT(on_openButtonClicked()));
  resetBufferQueue();
  imageLabel_->setScaledContents(true);
  imageLabel_->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
  imageLabel_->setPixmap(QPixmap::fromImage(current_image_));
  scrollArea_->setBackgroundRole(QPalette::Dark);
  scrollArea_->setWidgetResizable(false);
  scrollArea_->setWidget(imageLabel_);
  scrollArea_->setMaximumWidth(width_);
  scrollArea_->setMinimumHeight(height_);
  ui->verticalLayout_2->addWidget(scrollArea_);
  connect(ui->SaveButton_2, SIGNAL(clicked(bool)), this,
          SLOT(on_saveButtonClicked()));
  connect(ui->AutoRefreshCheckbox_2, &QCheckBox::stateChanged, [this](int state) {
    auto_refresh_ = state > 0;
    device_context_->refreshPointCloud();
  });
  // udp input
  auto udp_cb = [this](const uint8_t* data, size_t len) {
    ParseCameraPacket(data, len);
  };
  udp_input_ = std::make_shared<autox::pointview::UdpInput>(device_context_);
  udp_input_->setUdpCallback(udp_cb);  
  // player thread
  player_thread_ = std::make_unique<std::thread>([this]() {
    while (!exit_) {
      if (udp_input_->isOpen()) {
        udp_input_->poll();
      } else {
        std::this_thread::sleep_for(10ms);
      }
    }
  });

  process_thread_ = std::make_unique<std::thread>([this]() {
    while (!exit_) {
      if (!pixel_frame_queue_done.empty()) {
        std::vector<std::vector<uint8_t>> data_v;
        if (pixel_frame_queue_done.get(data_v)) {
          LOG(INFO) << "warnning!! pixel_frame_queue_done is empty, get timeout";
          continue;
        }
        process(data_v);
        pixel_frame_queue_free.put(data_v);
      } else {
        std::this_thread::sleep_for(10ms);
      }
    }
  });
}

void CameraImage::set_status() {
  status_str_ = QTime::currentTime().toString();
  status_str_ = "update time: " + status_str_;
  ui->statusLabel_2->setText(status_str_);
  ui->sizeLabel_2->setText("width: " + QString::number(width_) + ", height: " + QString::number(height_));
}

void CameraImage::closeEvent(QCloseEvent* event) {
  QWidget::closeEvent(event);
  render_ = false;
}

void CameraImage::showEvent(QShowEvent* event) {
  QWidget::showEvent(event);
  render_ = true;
}

void CameraImage::Update()
{
  float speed_f = udp_input_->getUdpSpeed();
  QString speed = "speed: " + QString::number(speed_f) + "kB/s";
  ui->udpSpeedLabel_2->setText(speed);
  if(image_frame_queue_done.empty()) return;
  QImage new_image;
  image_frame_queue_done.get(new_image);
  ShowImage(new_image);
  image_frame_queue_free.put(new_image);
}

void CameraImage::process(std::vector<std::vector<uint8_t>> image_ori) 
{
  if(image_ori.size() > height_) {
    LOG(ERROR) << "image_ori.size() is " << image_ori.size() 
    << ", height_ is " << height_;
    return;
  }
  if(image_ori[0].size() > width_) {
    LOG(ERROR) << "image_ori[0].size() is" << image_ori[0].size() 
    << ", width_ is " << width_;
    return;
  }
  if(image_frame_queue_free.empty()){
    LOG(ERROR) << "image_frame_queue_free is empty";
    return;
  } 
  QImage new_image;
  image_frame_queue_free.get(new_image);
  new_image = QImage(width_, height_, QImage::Format_RGB32);
  new_image.fill(0);
  for(int i = 0; i < image_ori.size();i++) {
    for(int j = 0; j < image_ori[0].size(); j++) {
      uint8_t color_v = image_ori[i][j];
      new_image.setPixel(j, i, qRgb(color_v, color_v, color_v));
    }
  }
  image_frame_queue_done.put(new_image);
}

void CameraImage::ShowImage(const QImage& image) {
  if (image.isNull()) {
    return;
  }
  current_image_ = image;
  imageLabel_->setPixmap(QPixmap::fromImage(image));
  imageLabel_->adjustSize();
  scrollArea_->setVisible(true);
  set_status();
  // record
  if (is_record_) {
    QString fileName = record_dir_ + "/" + QString::number(record_id_) + ".png";
    SaveImage(fileName);
    if (SaveImage(fileName)) {
      LOG(INFO) << "successfully record id: " + std::to_string(record_id_);
    } else {
      LOG(ERROR) << "failedly record id: " + std::to_string(record_id_);
    }
    record_id_++;
  }
}

void CameraImage::openUdpPort(int port) 
{
  udp_receive_port_ = port;
  udp_input_->open(port);
}

void CameraImage::resetBufferQueue() {
  pixel_frame_queue_free.clear();
  pixel_frame_queue_done.clear();
  image_frame_queue_free.clear();
  image_frame_queue_done.clear();
  for (int i = 0; i < 2; i++) {
    pixel_frame_queue_free.put(std::vector<std::vector<uint8_t>>());
    image_frame_queue_free.put(QImage());
  }
}

void CameraImage::ParseCameraPacket(const uint8_t* data, size_t len) {
  // check packet
  if (!CheckPacket(data, len)) {
    return;
  }
  auto udp_pkt = reinterpret_cast<const CameraPacket*>(data);
  //camera image size
  height_ = udp_pkt->header.row_num;
  width_ = udp_pkt->header.column_num;
  //cur packet pixel info
  uint16_t cur_row_id = udp_pkt->header.row_id;
  uint16_t first_column_id = udp_pkt->header.first_point_column_id;
  uint16_t data_size = udp_pkt->header.data_size;
  uint8_t last_packet = udp_pkt->header.last_packet;
  if(data_size > width_ || cur_row_id >= height_ || 
    width_ < (data_size + first_column_id))
  {
    LOG(ERROR) << "SIZE ERROR! row_num: " << height_ << ", column_num: " << width_ \
    << ", data_size: " << data_size << ", cur_row_id: " << cur_row_id \
    << ", first_column_id: " << first_column_id;
    return;
  }
  std::vector<uint8_t> pixel_row(width_);
  for (int i = 0; i < data_size; i++) {
    pixel_row.at(i + first_column_id) = udp_pkt->data[i];
  }
  pixel_frame_.push_back(pixel_row);
  if(last_packet){
    if (!render_ || !auto_refresh_ || pixel_frame_.size() < 100) {
      pixel_frame_.clear();
      return;
    }
    LOG(INFO) << "recv last udp packet, will render one frame camera image";
    std::vector<std::vector<uint8_t>> data_v;
    if (pixel_frame_queue_free.empty()) {
      LOG(INFO) << "warnning!! pixel_frame_queue_free is empty, get timeout";
    } else {
      pixel_frame_queue_free.get(data_v);
      data_v = pixel_frame_;
      pixel_frame_queue_done.put(data_v);
    }
    pixel_frame_.clear();
  }
}

bool CameraImage::CheckPacket(const uint8_t* data, size_t len) {  // check packet
  if (len <= 16) {
    LOG(ERROR) << "udp receive len too short, len is " << len;
    return false;
  }
  // check head
  auto header = reinterpret_cast<const PacketHeader*>(data);
  if (header->sop[0] != 0xee || header->sop[1] != 0xff) {
    std::stringstream ss;
    ss << "Invalid log packet, head need be 0xeeff: " << std::hex
       << (0xFF & header->sop[0]) << (0xFF & header->sop[1]) << std::dec;
    LOG(ERROR) << ss.str();
    return false;
  }
  // check version
  if (header->major_version != 0x04 || header->minor_version != 0x20) {
    std::stringstream ss;
    ss << "Invalid log packet, version need be 4.32: " << std::hex
       << header->major_version << '.' << header->minor_version << std::dec;
    LOG(ERROR) << ss.str();
    return false;
  }
  // check data size
  if (header->data_size + 18 != len) {
    std::stringstream ss;
    ss << "Invalid log packet, packet size need be data size + 18, "
          "packet size:"
       << len << ", data size:" << header->data_size;
    LOG(ERROR) << ss.str();
    return false;
  }
  return true;
}

CameraImage::~CameraImage()
{
  exit_ = true;
  if (player_thread_->joinable()) {
    player_thread_->join();
  }
  if (process_thread_->joinable()) {
    process_thread_->join();
  }
  delete ui;
}

void CameraImage::on_openButtonClicked() {
  this->move(this->parentWidget()->geometry().center() - this->rect().center());
  this->show();
}
void CameraImage::on_RecordButton_2_clicked() {
  if (is_record_) {
    is_record_ = false;
  } else {
    record_dir_ = QFileDialog::getExistingDirectory(
        this, "Record image to directory", QDir::homePath() + "",
        QFileDialog::ShowDirsOnly | QFileDialog::DontUseNativeDialog);
    if (record_dir_.isNull()) {
      LOG(INFO) << "Do not select a target directory.";
      return;
    }
    LOG(INFO) << "directory name: " + record_dir_.toStdString();
    record_id_ = 0;
    is_record_ = true;
  }
  SetRecordButtonState();
}

void CameraImage::SetRecordButtonState() {
  if (is_record_) {
    ui->RecordButton_2->setStyleSheet(
        "QPushButton{border-image: url(:/pointview_icon/record-off.png);}");
  } else {
    ui->RecordButton_2->setStyleSheet(
        "QPushButton{border-image: "
        "url(:/pointview_icon/record-on-disable.png);}\n"
        "QPushButton:hover{border-image: "
        "url(:/pointview_icon/record-on.png);}");
  }
}

void CameraImage::on_saveButtonClicked() {
  QString fileName = GetSaveFileName("Save image to file",
                                     QDir::homePath() + "/Untitled.png",
                                     "PNG Files(*.png);;All Files(*.*)");
  if (fileName.isNull()) {
    LOG(INFO) << "Do not select a target file.";
    return;
  }
  QMessageBox msgBox;
  msgBox.setStyleSheet("QLabel{min-width: 200px;}");
  if (SaveImage(fileName)) {
    msgBox.setText("Success.");
    msgBox.setInformativeText("Save image in " + fileName +
                              " successfully.");
  } else {
    msgBox.setText("Failed.");
    msgBox.setInformativeText("Failed to save in " + fileName);
  }

  msgBox.setStandardButtons(QMessageBox::Ok);
  msgBox.exec();
}

bool CameraImage::SaveImage(QString file_name) {
  bool result = current_image_.save(file_name, "PNG");
  if (result) {
    LOG(INFO) << "successfully save to: " + file_name.toStdString();
  } else {
    LOG(ERROR) << "failed record to: " + file_name.toStdString();
  }
  return result;
}

}
}
