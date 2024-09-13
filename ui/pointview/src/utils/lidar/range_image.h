/******************************************************************************
 * Copyright 2023 AutoX. All Rights Reserved.
 *****************************************************************************/

#ifndef RANGEIMAGE_H
#define RANGEIMAGE_H

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
#include "utils/pointcloud/pointcloud.h"
#include "utils/property_tree.h"

namespace Ui {
class RangeImage;
}

namespace autox {
namespace pointview {

class RangeImage : public QMainWindow {
  Q_OBJECT

 public:
  using PointToPixelCallback = std::function<bool(size_t idx, int& x, int& y)>;
  RangeImage(std::shared_ptr<DeviceContext> device_context, int width,
             int height);
  ~RangeImage();
  bool InitFromConfig(std::shared_ptr<autox::pointview::Config> config);
  bool StoreToConfig(std::shared_ptr<autox::pointview::Config> config);
  void SetPointToPixelCallback(PointToPixelCallback cb);
  // update by using point cloud
  void Update(PointCloudT::Ptr cloud);
  // update by using image
  void Update(const QImage& image);

 public slots:
  void set_position(int x, int y);
  void set_select_status(bool status);
  bool SaveImage(QString file_name);

 signals:
  void PickRangeImagePoint(int x, int y);

 private:
  void ShowImage(const QImage& image);
  void SetRecordButtonState();
  bool eventFilter(QObject* object, QEvent* event);
  void UpdateImage(const QImage& image);
  void RecoverColor(int mode, QImage& img);
  void DrawRect(QPoint pos, QPainter& painter);

 protected:
  void closeEvent(QCloseEvent* event) override;
  void showEvent(QShowEvent* event) override;

 private slots:
  void on_openButtonClicked();
  void on_saveButtonClicked();
  void on_RecordButton_clicked();
  void on_enlargeButtonClicked();
  void on_shrinkButtonClicked();

 private:
  struct PixelData {
    QPoint pos;
    QColor color;
  };
  std::shared_ptr<DeviceContext> device_context_;
  std::string namespace_{"range_image"};
  std::shared_ptr<QPushButton> open_button_;
  PropertyPairs properties_;
  // todo: fix the segmentation fault for shared ptr on QWidget
  Ui::RangeImage* ui;
  QImage current_image_;
  QLabel* imageLabel_;
  QScrollArea* scrollArea_;
  // data
  int width_, height_;
  bool render_{false};
  bool auto_refresh_{true};
  PointToPixelCallback point2pixel_cb_;
  // record
  bool is_record_{false};
  QString record_dir_;
  int record_id_{0};
  // image scale control
  std::shared_ptr<QPushButton> enlarge_button_, shrink_button_;
  std::shared_ptr<QHBoxLayout> img_control_layout_;
  float scale_size_{1.0};
  float min_scale_{1.0};
  float max_scale_{4.0};
  float max_pixel_{32000.0};
  float real_scale_{1.0};
  // point color recover
  QColor last_color_;
  QPoint last_position_;
  bool need_recover_{false};
  // multi points
  bool select_status_{false};
  std::vector<PixelData> pixel_buffer_;
  // bound parameter
  int bound_width_{20};
  int half_bound_width_;
};
}  // namespace pointview
}  // namespace autox
#endif  // RANGEIMAGE_H
