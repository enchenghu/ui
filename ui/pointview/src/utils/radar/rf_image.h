/******************************************************************************
 * Copyright 2023 AutoX. All Rights Reserved.
 *****************************************************************************/

#ifndef RFIMAGE_H
#define RFIMAGE_H

#include <QHBoxLayout>
#include <QLabel>
#include <QMainWindow>
#include <QPushButton>
#include <QScrollArea>
#include <QSpacerItem>
#include <QTimer>
#include <memory>

#include <opencv2/opencv.hpp>
#include <pcl/point_types.h>

#include "radar_image_viewer.h"
#include "utils/colormap.h"
#include "utils/common/device_context.h"
#include "utils/config.h"
#include "utils/pointcloud/point_manipulator.h"
#include "utils/pointcloud/pointcloud.h"
#include "utils/property_tree.h"
#include "devices/xradar_2d/driver/xradar_2d_driver.h"

namespace Ui {
class RfImage;
}

namespace autox {
namespace pointview {
namespace rf_image {
struct Point {
  uint16_t range_code;
  int16_t doppler_code;
  int8_t azimuth_code;
  uint8_t log_power;
  float doppler;
};
struct PointCloud {
  double timestamp;
  std::vector<Point> points;
};
class RfImage : public QMainWindow {
  Q_OBJECT

 public:
  using PointToPixelCallback = std::function<bool(size_t idx, int& x, int& y)>;
  RfImage(
      std::shared_ptr<DeviceContext> device_context,
      const std::shared_ptr<autox::pointview::PointManipulator>& manipulator);
  ~RfImage();
  bool IsInited() const { return inited_; };
  // update using image
  void Update(const drivers::xradar_2d::PointCloud& point_cloud);

  bool RequestRfByPointcloud(PointCloudT& rf_pointcloud);

 protected:
  void closeEvent(QCloseEvent* event) override;
  void showEvent(QShowEvent* event) override;

 private:
  bool Init(const drivers::xradar_2d::PointCloud& point_cloud);
  bool PointcloudToRawRf0_0(const drivers::xradar_2d::PointCloud& point_cloud);
  bool PointcloudToRawRf1_0(const drivers::xradar_2d::PointCloud& point_cloud);
  void ConvertRfToRgb();
  void UpdateViewers();

  cv::Mat GeneratePolarToCartesianRemapMat(const double range_resolution,
                                           const double azimuth_resolutio);

 private slots:
  void on_openButtonClicked();
  void OnColorMapRangeChanged(std::string chennel, int min, int max);
  void RfPointCloudTypeChange();

 private:
  std::shared_ptr<DeviceContext> device_context_;
  std::shared_ptr<QPushButton> open_button_;
  std::shared_ptr<QCheckBox> rf_point_cloud_checkbox_;
  std::shared_ptr<QComboBox> rf_type_combobox_;
  // todo: fix the segmentation fault for shared ptr on QWidget
  Ui::RfImage* ui;

  int range_bins_;
  int azimuth_bins_;
  int cart_pixel_width_;

  double base_range_resolution_;
  double base_azimuth_resolution_;

  cv::Mat current_power_rf_;
  cv::Mat current_doppler_rf_;

  // RGB data
  cv::Mat cv_current_power_polar_rgb_;
  cv::Mat cv_current_doppler_polar_rgb_;
  cv::Mat cv_current_power_cartesian_rgb_;
  cv::Mat cv_current_doppler_cartesian_rgb_;
  // only viewers
  QImage current_power_polar_rgb_;
  QImage current_doppler_polar_rgb_;
  QImage current_power_cartesian_rgb_;
  QImage current_doppler_cartesian_rgb_;

  RadarImageViewer* power_polar_viewer_;
  RadarImageViewer* doppler_polar_viewer_;
  RadarImageViewer* power_catesian_viewer_;
  RadarImageViewer* doppler_catesian_viewer_;

  std::shared_ptr<autox::pointview::PointManipulator> manipulator_;
  Colormap power_rf_colormap_;
  Colormap doppler_rf_colormap_;

  cv::Mat remap_mat_;
  cv::Mat cartesian_mask_;
  cv::Mat range_azimuth_mask_;
  // data
  bool render_{false};
  bool inited_;

  // member for vesion 1_0
  struct DetectTypeInfo {
    enum RemapType{
      CV_REMAP,
      POINT_ASSINE_REMAP
    };
    // on init
    int detact_type;
    int origin_range_bins;
    int origin_azimuth_bins;
    double origin_range_resolution;
    double origin_azimuth_resolution;
    RemapType remap_type;
    cv::Mat remap_mat;
    // on run
    cv::Mat origin_power_rf;
    cv::Mat origin_doppler_rf;
    cv::Mat remaped_power_rf;
    cv::Mat remaped_doppler_rf;

    bool GenerateRemapMat(const int target_range_bins,
                          const int target_azimuth_bins,
                          const double target_range_resolution,
                          const double target_azimuth_resolution);
    bool CvRemap();
  };
   std::vector<DetectTypeInfo> detect_type_infos_;
};

}  // namespace rf_image
}  // namespace pointview
}  // namespace autox
#endif  // RANGEIMAGE_H
