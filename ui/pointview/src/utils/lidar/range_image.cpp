/******************************************************************************
 * Copyright 2023 AutoX. All Rights Reserved.
 *****************************************************************************/

#include "range_image.h"

#include <QMessageBox>
#include <QMouseEvent>
#include <QPainter>
#include <iostream>

#include "ui_range_image.h"
#include "utils.h"

namespace autox {
namespace pointview {

RangeImage::RangeImage(std::shared_ptr<DeviceContext> device_context, int width,
                       int height)
    : QMainWindow(device_context->getDisplayContext()->getParent()),
      ui(new Ui::RangeImage),
      width_(width),
      height_(height),
      imageLabel_(new QLabel(this)),
      scrollArea_(new QScrollArea(this)),
      open_button_(new QPushButton(this)),
      enlarge_button_(new QPushButton(this)),
      shrink_button_(new QPushButton(this)),
      img_control_layout_(new QHBoxLayout) {
  device_context_ = device_context;
  ui->setupUi(this);
  imageLabel_->setScaledContents(true);
  imageLabel_->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
  imageLabel_->setPixmap(QPixmap::fromImage(current_image_));
  scrollArea_->setBackgroundRole(QPalette::Dark);
  scrollArea_->setWidgetResizable(false);
  scrollArea_->setWidget(imageLabel_);
  scrollArea_->setMaximumWidth(width_);
  scrollArea_->setMinimumHeight(height_);
  ui->verticalLayout->addWidget(scrollArea_);

  connect(ui->SaveButton, SIGNAL(clicked(bool)), this,
          SLOT(on_saveButtonClicked()));
  connect(ui->AutoRefreshCheckbox, &QCheckBox::stateChanged, [this](int state) {
    auto_refresh_ = state > 0;
    device_context_->refreshPointCloud();
  });
  // property
  open_button_->setText("Open");
  properties_.push_back({"Range Image", open_button_});
  connect(open_button_.get(), SIGNAL(clicked(bool)), this,
          SLOT(on_openButtonClicked()));
  device_context_->addProperties(properties_);
  SetRecordButtonState();
  // image scale control button
  enlarge_button_->setText("Large(+)");
  connect(enlarge_button_.get(), SIGNAL(clicked(bool)), this,
          SLOT(on_enlargeButtonClicked()));
  shrink_button_->setText("Shrink(-)");
  connect(shrink_button_.get(), SIGNAL(clicked(bool)), this,
          SLOT(on_shrinkButtonClicked()));
  img_control_layout_->addWidget(shrink_button_.get());
  img_control_layout_->addWidget(enlarge_button_.get());
  ui->verticalLayout->addLayout(img_control_layout_.get());
  imageLabel_->installEventFilter(this);
  half_bound_width_ = (int)(bound_width_ / 2);
}

RangeImage::~RangeImage() { delete ui; }

void RangeImage::ShowImage(const QImage& image) {
  if (image.isNull()) {
    return;
  }
  auto img_size = image.size() * scale_size_;
  real_scale_ = scale_size_;
  if (img_size.height() > max_pixel_) {
    real_scale_ = std::min(real_scale_, max_pixel_ / image.height());
    max_scale_ = std::min(max_scale_, real_scale_);
  } else if (img_size.width() > max_pixel_) {
    real_scale_ = std::min(real_scale_, max_pixel_ / image.width());
    max_scale_ = std::min(max_scale_, real_scale_);
  }
  auto scale_image = image.scaled(image.size() * real_scale_);
  imageLabel_->setPixmap(QPixmap::fromImage(scale_image));
  imageLabel_->adjustSize();
  scrollArea_->setVisible(true);
}

void RangeImage::closeEvent(QCloseEvent* event) {
  QWidget::closeEvent(event);
  render_ = false;
}

void RangeImage::showEvent(QShowEvent* event) {
  QWidget::showEvent(event);
  render_ = true;
}

void RangeImage::on_openButtonClicked() {
  this->move(this->parentWidget()->geometry().center() - this->rect().center());
  this->show();
}

void RangeImage::on_saveButtonClicked() {
  QString fileName = GetSaveFileName("Save range image to file",
                                     QDir::homePath() + "/Untitled.png",
                                     "PNG Files(*.png);;All Files(*.*)");
  if (fileName.isNull()) {
    Debug("Do not select a target file.");
    return;
  }
  QMessageBox msgBox;
  msgBox.setStyleSheet("QLabel{min-width: 200px;}");
  if (SaveImage(fileName)) {
    msgBox.setText("Success.");
    msgBox.setInformativeText("Save range image in " + fileName +
                              " successfully.");
  } else {
    msgBox.setText("Failed.");
    msgBox.setInformativeText("Failed to save range image in " + fileName);
  }

  msgBox.setStandardButtons(QMessageBox::Ok);
  msgBox.exec();
}

bool RangeImage::InitFromConfig(
    std::shared_ptr<autox::pointview::Config> config) {
  return true;
}

bool RangeImage::StoreToConfig(
    std::shared_ptr<autox::pointview::Config> config) {
  return true;
}

void RangeImage::SetPointToPixelCallback(PointToPixelCallback cb) {
  point2pixel_cb_ = cb;
};

void RangeImage::Update(PointCloudT::Ptr cloud) {
  if (!point2pixel_cb_) {
    return;
  }
  if (!render_ || !auto_refresh_) {
    return;
  }
  QImage new_image = QImage(width_, height_, QImage::Format_RGB32);
  new_image.fill(0);
  int x, y;
  for (size_t i = 0; i < cloud->points.size(); i++) {
    auto& p = cloud->points[i];
    if (p.a == 0) {
      continue;
    }
    if (point2pixel_cb_(i, x, y)) {
      int color = qRgb(p.r, p.g, p.b);
      new_image.setPixel(x, y, color);
    }
  }
  // update range image
  UpdateImage(new_image);
}

void RangeImage::Update(const QImage& image) { UpdateImage(image); }

void RangeImage::on_RecordButton_clicked() {
  if (is_record_) {
    is_record_ = false;
  } else {
    record_dir_ = QFileDialog::getExistingDirectory(
        this, "Record range image to directory", QDir::homePath() + "",
        QFileDialog::ShowDirsOnly | QFileDialog::DontUseNativeDialog);
    if (record_dir_.isNull()) {
      Debug("Do not select a target directory.");
      return;
    }
    Debug("directory name: " + record_dir_.toStdString());
    record_id_ = 0;
    is_record_ = true;
  }
  SetRecordButtonState();
}

void RangeImage::SetRecordButtonState() {
  if (is_record_) {
    ui->RecordButton->setStyleSheet(
        "QPushButton{border-image: url(:/pointview_icon/record-off.png);}");
  } else {
    ui->RecordButton->setStyleSheet(
        "QPushButton{border-image: "
        "url(:/pointview_icon/record-on-disable.png);}\n"
        "QPushButton:hover{border-image: "
        "url(:/pointview_icon/record-on.png);}");
  }
}

void RangeImage::on_enlargeButtonClicked() {
  float old_scale = scale_size_;
  scale_size_ += 0.2;
  if (scale_size_ > max_scale_) scale_size_ = max_scale_;
  Debug("old size: " + std::to_string(old_scale) +
        ", new: " + std::to_string(scale_size_));
  if (scale_size_ != old_scale) {
    device_context_->refreshPointCloud();
    ShowImage(current_image_);
  }
}

void RangeImage::on_shrinkButtonClicked() {
  float old_scale = scale_size_;
  scale_size_ -= 0.2;
  if (scale_size_ < min_scale_) scale_size_ = min_scale_;
  Debug("old size: " + std::to_string(old_scale) +
        ", new: " + std::to_string(scale_size_));
  if (scale_size_ != old_scale) {
    device_context_->refreshPointCloud();
    ShowImage(current_image_);
  }
}

bool RangeImage::eventFilter(QObject* object, QEvent* event) {
  if (object == imageLabel_ && event->type() == QEvent::MouseButtonPress) {
    Debug("event filter");
    QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
    QPoint pos = mouseEvent->pos() / real_scale_;
    if (pos.x() < 10000 && pos.y() < 256) {
      set_position(pos.x(), pos.y());
      emit PickRangeImagePoint(pos.x(), pos.y());
    }
  }
  return false;
}

void RangeImage::UpdateImage(const QImage& image) {
  if (!render_ || !auto_refresh_) {
    return;
  }
  // update range image
  ShowImage(image);
  need_recover_ = false;
  // record
  if (is_record_) {
    QString fileName = record_dir_ + "/" + QString::number(record_id_) + ".png";
    SaveImage(fileName);
    if (SaveImage(fileName)) {
      Debug("successfully record id: " + std::to_string(record_id_));
    } else {
      Debug("failedly record id: " + std::to_string(record_id_));
    }
    record_id_++;
  }
  current_image_ = image;
}

void RangeImage::set_position(int x, int y) {
  Debug("new position: " + std::to_string(x) + ", " + std::to_string(y));
  QImage img = current_image_;
  QPoint pos(x, y);
  if (select_status_) {
    for (int i = 0; i < pixel_buffer_.size(); i++) {
      if (pos == pixel_buffer_[i].pos) {
        img.setPixelColor(pixel_buffer_[i].pos, pixel_buffer_[i].color);
        pixel_buffer_.erase(pixel_buffer_.begin() + i);
        break;
      }
    }
    pixel_buffer_.push_back({pos, img.pixelColor(pos)});
  } else {
    if (pos == last_position_) {
      return;
    }
    // recover
    RecoverColor(0, img);
    // save
    last_color_ = img.pixelColor(pos);
    last_position_ = pos;
    need_recover_ = true;
  }
  // set
  img.setPixel(pos, qRgb(0, 255, 0));
  ShowImage(img);
  current_image_ = img;
}

void RangeImage::set_select_status(bool status) {
  if (status != select_status_) {
    select_status_ = status;
    QImage img = current_image_;
    if (!status) {
      RecoverColor(1, img);
    } else {
      RecoverColor(0, img);
    }
    ShowImage(img);
    current_image_ = img;
  }
}

bool RangeImage::SaveImage(QString file_name) {
  if (!pixel_buffer_.empty()) {
    QImage img = current_image_;
    QPainter painter(&img);
    // draw rectangular for first point
    painter.setPen(QPen(Qt::blue, 1, Qt::SolidLine, Qt::RoundCap));
    DrawRect(pixel_buffer_[0].pos, painter);
    int low_bound = pixel_buffer_[0].pos.x();
    int high_bound = pixel_buffer_[0].pos.x();
    for (int i = 1; i < pixel_buffer_.size(); i++) {
      // Draw rectangular for point
      painter.setPen(QPen(Qt::green, 1, Qt::SolidLine, Qt::RoundCap));
      DrawRect(pixel_buffer_[i].pos, painter);
      // Draw connection line
      painter.setPen(QPen(Qt::red, 1, Qt::SolidLine, Qt::RoundCap));
      painter.drawLine(pixel_buffer_[i - 1].pos, pixel_buffer_[i].pos);
      // Get low and high bound
      if (pixel_buffer_[i].pos.x() > high_bound) {
        high_bound = pixel_buffer_[i].pos.x();
      }
      if (pixel_buffer_[i].pos.x() < low_bound) {
        low_bound = pixel_buffer_[i].pos.x();
      }
    }
    // Draw bound
    painter.setPen(QPen(Qt::red, 5, Qt::SolidLine, Qt::RoundCap));
    low_bound = low_bound - bound_width_ > half_bound_width_
                    ? low_bound - bound_width_
                    : half_bound_width_;
    painter.drawLine(QPoint(low_bound, 0), QPoint(low_bound, 255));
    high_bound = high_bound + bound_width_ < 10000 - half_bound_width_
                     ? high_bound + bound_width_
                     : 10000 - half_bound_width_;
    painter.drawLine(QPoint(high_bound, 0), QPoint(high_bound, 255));
    painter.end();
    return img.save(file_name, "PNG");
  }
  bool result = current_image_.save(file_name, "PNG");
  if (result) {
    Debug("successfully save to: " + file_name.toStdString());
  } else {
    Debug("failed record to: " + file_name.toStdString());
  }
  return result;
}

void RangeImage::RecoverColor(int mode, QImage& img) {
  // recover single point color
  if (mode == 0) {
    if (need_recover_) {
      img.setPixelColor(last_position_, last_color_);
      need_recover_ = false;
    }
  }
  // recover multi points color
  else if (mode == 1) {
    for (int i = 0; i < pixel_buffer_.size(); i++) {
      img.setPixelColor(pixel_buffer_[i].pos, pixel_buffer_[i].color);
    }
    pixel_buffer_.clear();
  }
}

void RangeImage::DrawRect(QPoint pos, QPainter& painter) {
  int x = pos.x();
  int y = pos.y();
  int x_l = 3;
  int y_l = 3;
  if (x > 2) {
    x = (x - 2);
    x_l = 5;
  }
  if (y > 2) {
    y = (y - 2);
    y_l = 5;
  }
  painter.drawRect(x, y, x_l, y_l);
}

}  // namespace pointview
}  // namespace autox
