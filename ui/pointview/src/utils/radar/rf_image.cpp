/******************************************************************************
 * Copyright 2023 AutoX. All Rights Reserved.
 *****************************************************************************/

#include "rf_image.h"

#include <QMessageBox>
#include <QMouseEvent>
#include <algorithm>
#include <iostream>

#include "ui_rf_image.h"
#include "utils.h"

namespace autox {
namespace pointview {
namespace rf_image {

RfImage::RfImage(
    std::shared_ptr<DeviceContext> device_context,
    const std::shared_ptr<autox::pointview::PointManipulator>& manipulator)
    : QMainWindow(device_context->getDisplayContext()->getParent()),
      ui(new Ui::RfImage),
      power_polar_viewer_(nullptr),
      doppler_polar_viewer_(nullptr),
      power_catesian_viewer_(nullptr),
      doppler_catesian_viewer_(nullptr),
      open_button_(new QPushButton(this)),
      rf_point_cloud_checkbox_(new QCheckBox(this)),
      rf_type_combobox_(new QComboBox(this)),
      manipulator_(manipulator),
      inited_(false) {
  device_context_ = device_context;
  ui->setupUi(this);

  // property
  auto sub =
      device_context->getPropertyTree()->createPropertySubTree("rf image");
  // open
  open_button_->setText("Open");
  sub->addProperty("Open", open_button_);
  connect(open_button_.get(), SIGNAL(clicked(bool)), this,
          SLOT(on_openButtonClicked()));
  // replace point cloud
  sub->addProperty("rf pointcloud", rf_point_cloud_checkbox_);
  connect(rf_point_cloud_checkbox_.get(), SIGNAL(stateChanged(int)), this,
          SLOT(RfPointCloudTypeChange()));
  // rf type
  rf_type_combobox_->addItem("power");
  rf_type_combobox_->addItem("doppler");
  sub->addProperty("rf type", rf_type_combobox_);
  connect(rf_type_combobox_.get(), SIGNAL(currentIndexChanged(int)), this,
          SLOT(RfPointCloudTypeChange()));

  // init color map
  power_rf_colormap_.setType("jet");
  power_rf_colormap_.setRange(1, 4);
  doppler_rf_colormap_.setType("bwr");
  doppler_rf_colormap_.setRange(-5, 5);

  connect(manipulator_.get(),
          SIGNAL(ColorMapRangeChanged(std::string, int, int)), this,
          SLOT(OnColorMapRangeChanged(std::string, int, int)));
}

RfImage::~RfImage() { delete ui; }

bool RfImage::Init(const drivers::xradar_2d::PointCloud& point_cloud) {
  if (std::any_of(point_cloud.range_resolution.begin(),
                  point_cloud.range_resolution.end(),
                  [](float x) { return x <= 0.0f; })) {
    LOG(ERROR) << "bad point cloud range_resolution.";
    return false;
  }
  if (std::any_of(point_cloud.azimuth_resolution.begin(),
                  point_cloud.azimuth_resolution.end(),
                  [](float x) { return x <= 0.0f; })) {
    LOG(ERROR) << "bad point cloud azimuth_resolution.";
    return false;
  }

  // assume the latest detect type has the most accurate resolution.
  base_range_resolution_ = point_cloud.range_resolution[0];
  base_azimuth_resolution_ =
      point_cloud.azimuth_resolution[point_cloud.dectect_number - 1] * M_PI /
      180.0;

  // determine range_bins_ and azimuth_bins_ by diffrent radar version.
  if (point_cloud.major_version == 0x00) {
    range_bins_ = 256;
    azimuth_bins_ = 160;
  } else if (point_cloud.major_version == 0x01) {
    constexpr int kEachDetectTypeRangeBins = 1024;
    constexpr int kEachDetectTypeAzimuthBins = 256;
    double max_range =
        kEachDetectTypeRangeBins * point_cloud.range_resolution[0];
    double max_azimuth =
        kEachDetectTypeAzimuthBins *
        point_cloud.azimuth_resolution[point_cloud.dectect_number - 1] * M_PI /
        180.0;

    range_bins_ = static_cast<int>(max_range / base_range_resolution_);
    azimuth_bins_ = static_cast<int>(max_azimuth / base_azimuth_resolution_);

    for (size_t i = 0; i < point_cloud.range_resolution.size(); i++) {
      DetectTypeInfo& type_info =
          detect_type_infos_.emplace_back(DetectTypeInfo());
      type_info.detact_type = static_cast<int>(i);
      type_info.origin_range_bins = kEachDetectTypeRangeBins;
      type_info.origin_azimuth_bins = kEachDetectTypeAzimuthBins;
      type_info.origin_range_resolution = point_cloud.range_resolution[i];
      type_info.origin_azimuth_resolution =
          point_cloud.azimuth_resolution[i] * M_PI / 180.0;
      type_info.remaped_power_rf =
          cv::Mat::zeros(azimuth_bins_, range_bins_, CV_32F);
      type_info.remaped_doppler_rf =
          cv::Mat::zeros(azimuth_bins_, range_bins_, CV_32F);
      type_info.origin_power_rf = cv::Mat::zeros(
          type_info.origin_azimuth_bins, type_info.origin_range_bins, CV_32F);
      type_info.origin_doppler_rf = cv::Mat::zeros(
          type_info.origin_azimuth_bins, type_info.origin_range_bins, CV_32F);
      if (type_info.origin_range_resolution > base_range_resolution_) {
        type_info.remap_type = RfImage::DetectTypeInfo::RemapType::CV_REMAP;
        type_info.GenerateRemapMat(range_bins_, azimuth_bins_,
                                   base_range_resolution_,
                                   base_azimuth_resolution_);
      } else {
        type_info.remap_type =
            RfImage::DetectTypeInfo::RemapType::POINT_ASSINE_REMAP;
      }
    }

  } else {
    LOG(ERROR) << "Not suport radar major version: "
               << point_cloud.major_version;
    return false;
  }
  cart_pixel_width_ = 2 * range_bins_;

  current_power_rf_ = cv::Mat::zeros(azimuth_bins_, range_bins_, CV_32F);
  current_doppler_rf_ = cv::Mat::zeros(azimuth_bins_, range_bins_, CV_32F);
  // init inner images=
  current_power_polar_rgb_ =
      QImage(range_bins_, azimuth_bins_, QImage::Format_RGB888);
  current_doppler_polar_rgb_ =
      QImage(range_bins_, azimuth_bins_, QImage::Format_RGB888);
  current_power_cartesian_rgb_ =
      QImage(cart_pixel_width_, cart_pixel_width_, QImage::Format_RGB888);
  current_doppler_cartesian_rgb_ =
      QImage(cart_pixel_width_, cart_pixel_width_, QImage::Format_RGB888);
  current_power_polar_rgb_.fill({0, 0, 0});
  current_doppler_polar_rgb_.fill({0, 0, 0});
  current_power_cartesian_rgb_.fill({0, 0, 0});
  current_doppler_cartesian_rgb_.fill({0, 0, 0});
  remap_mat_ = cv::Mat::zeros(cart_pixel_width_, cart_pixel_width_, CV_32FC2);
  cartesian_mask_ =
      cv::Mat::zeros(cart_pixel_width_, cart_pixel_width_, CV_8UC1);
  range_azimuth_mask_ =
      cv::Mat::zeros(cart_pixel_width_, cart_pixel_width_, CV_32FC2);

  power_polar_viewer_ =
      new RadarImageViewer(this, true, azimuth_bins_, range_bins_,
                           base_azimuth_resolution_, base_range_resolution_);
  doppler_polar_viewer_ =
      new RadarImageViewer(this, true, azimuth_bins_, range_bins_,
                           base_azimuth_resolution_, base_range_resolution_);
  power_catesian_viewer_ =
      new RadarImageViewer(this, false, cart_pixel_width_, cart_pixel_width_,
                           base_range_resolution_, base_range_resolution_);
  doppler_catesian_viewer_ =
      new RadarImageViewer(this, false, cart_pixel_width_, cart_pixel_width_,
                           base_range_resolution_, base_range_resolution_);

  ui->gridLayout_3->addWidget(power_polar_viewer_, 0, 1);
  power_polar_viewer_->InitAxis(ui->gridLayout_3);
  ui->gridLayout_4->addWidget(doppler_polar_viewer_, 0, 1);
  doppler_polar_viewer_->InitAxis(ui->gridLayout_4);
  ui->gridLayout_5->addWidget(power_catesian_viewer_, 0, 1);
  power_catesian_viewer_->InitAxis(ui->gridLayout_5);
  ui->gridLayout_6->addWidget(doppler_catesian_viewer_, 0, 1);
  doppler_catesian_viewer_->InitAxis(ui->gridLayout_6);

  remap_mat_ = GeneratePolarToCartesianRemapMat(base_range_resolution_,
                                                base_azimuth_resolution_);

  inited_ = true;
  return true;
}

void RfImage::Update(const drivers::xradar_2d::PointCloud& point_cloud) {
  if (!IsInited()) {
    if (!Init(point_cloud)) {
      return;
    }
  }
  if (point_cloud.major_version == 0x00) {
    PointcloudToRawRf0_0(point_cloud);
  } else if (point_cloud.major_version == 0x01) {
    PointcloudToRawRf1_0(point_cloud);
  }
  UpdateViewers();
  return;
}

bool RfImage::RequestRfByPointcloud(PointCloudT& rf_pointcloud) {
  if (!rf_point_cloud_checkbox_->isChecked()) {
    return false;
  }
  int index = rf_type_combobox_->currentIndex();
  rf_pointcloud.clear();
  for (int ri = 0; ri < cartesian_mask_.rows; ri++) {
    for (int ci = 0; ci < cartesian_mask_.cols; ci++) {
      if (!cartesian_mask_.at<uchar>(ri, ci)) {
        continue;
      }
      int r, g, b;
      const cv::Vec2f& r_a = range_azimuth_mask_.at<cv::Vec2f>(ri, ci);
      float x = r_a[0] * std::sin(r_a[1]);
      float y = r_a[0] * std::cos(r_a[1]);
      float z = 0;
      int a = 255;
      if (index == 0) {
        QColor color_power = current_power_cartesian_rgb_.pixelColor(ci, ri);
        color_power.getRgb(&r, &g, &b);
      } else {
        QColor color_doppler =
            current_doppler_cartesian_rgb_.pixelColor(ci, ri);
        color_doppler.getRgb(&r, &g, &b);
        if (r == 0 && g == 0 && b == 0) {
          a = 0;
        }
      }
      PointT rf_point;
      rf_point.x = x;
      rf_point.y = y;
      rf_point.z = z;
      rf_point.r = r;
      rf_point.g = g;
      rf_point.b = b;
      rf_point.a = a;
      rf_pointcloud.push_back(rf_point);
    }
  }
  return true;
}

void RfImage::closeEvent(QCloseEvent* event) {
  QWidget::closeEvent(event);
  render_ = false;
}

void RfImage::showEvent(QShowEvent* event) {
  QWidget::showEvent(event);
  if (inited_) {
    power_polar_viewer_->ResetFitView();
    doppler_polar_viewer_->ResetFitView();
    power_catesian_viewer_->ResetFitView();
    doppler_catesian_viewer_->ResetFitView();
  }
  render_ = true;
}

void RfImage::on_openButtonClicked() {
  this->move(this->parentWidget()->geometry().center() - this->rect().center());
  this->show();
}

void RfImage::ConvertRfToRgb() {
  double min_doppler;
  cv::minMaxLoc(current_doppler_rf_, &min_doppler, nullptr, nullptr, nullptr);
  float doppler_shift = std::abs(std::min(min_doppler, 0.0)) + 1.0f;
  cv::Mat rf_polar_doppler_value_shift =
      cv::Mat::zeros(azimuth_bins_, range_bins_, CV_32F);
  // draw polar image rgb
#pragma omp parallel for collapse(2)
  for (int ri = 0; ri < azimuth_bins_; ri++) {
    uchar* power_scan_line = current_power_polar_rgb_.scanLine(ri);
    uchar* doppler_scan_line = current_doppler_polar_rgb_.scanLine(ri);
    for (int ci = 0; ci < range_bins_; ci++) {
      unsigned char r, g, b;
      // power color
      float v = current_power_rf_.at<float>(ri, ci);
      power_rf_colormap_.get(static_cast<double>(v), r, g, b);
      QColor qc_power(r, g, b);
      current_power_polar_rgb_.setPixelColor(ci, ri, qc_power);
      power_scan_line[ci * 3] = r;
      power_scan_line[ci * 3 + 1] = g;
      power_scan_line[ci * 3 + 2] = b;
      // doppler color
      if (v == 0) {
        r = 0;
        g = 0;
        b = 0;
      } else {
        v = current_doppler_rf_.at<float>(ri, ci);
        doppler_rf_colormap_.get(static_cast<double>(v), r, g, b);
        rf_polar_doppler_value_shift.at<float>(ri, ci) = v + doppler_shift;
      }
      doppler_scan_line[ci * 3] = r;
      doppler_scan_line[ci * 3 + 1] = g;
      doppler_scan_line[ci * 3 + 2] = b;
    }
  }

  cv::Mat rf_cartesian_power =
      cv::Mat::zeros(cart_pixel_width_, cart_pixel_width_, CV_32F);
  cv::Mat rf_cartesian_doppler_value_shift =
      cv::Mat::zeros(cart_pixel_width_, cart_pixel_width_, CV_32F);
  cv::remap(current_power_rf_, rf_cartesian_power, remap_mat_, cv::Mat(),
            cv::INTER_LINEAR, cv::BORDER_CONSTANT, cv::Scalar(0.0, 0.0, 0.0));
  cv::remap(rf_polar_doppler_value_shift, rf_cartesian_doppler_value_shift,
            remap_mat_, cv::Mat(), cv::INTER_NEAREST, cv::BORDER_CONSTANT,
            cv::Scalar(0.0, 0.0, 0.0));

  // draw catesian image rgb
#pragma omp parallel for collapse(2)
  for (int ri = 0; ri < cart_pixel_width_; ri++) {
    uchar* power_scan_line = current_power_cartesian_rgb_.scanLine(ri);
    uchar* doppler_scan_line = current_doppler_cartesian_rgb_.scanLine(ri);
    for (int ci = 0; ci < cart_pixel_width_; ci++) {
      if (!cartesian_mask_.at<uchar>(ri, ci)) {
        continue;
      }
      unsigned char r, g, b;
      // power color
      float v = rf_cartesian_power.at<float>(ri, ci);
      power_rf_colormap_.get(static_cast<double>(v), r, g, b);
      power_scan_line[ci * 3] = r;
      power_scan_line[ci * 3 + 1] = g;
      power_scan_line[ci * 3 + 2] = b;
      // doppler color
      v = rf_cartesian_doppler_value_shift.at<float>(ri, ci);
      if (v == 0) {
        r = 0;
        g = 0;
        b = 0;
      } else {
        v -= doppler_shift;
        doppler_rf_colormap_.get(static_cast<double>(v), r, g, b);
      }
      doppler_scan_line[ci * 3] = r;
      doppler_scan_line[ci * 3 + 1] = g;
      doppler_scan_line[ci * 3 + 2] = b;
    }
  }
}

void RfImage::UpdateViewers() {
  ConvertRfToRgb();
  power_polar_viewer_->setQImage(current_power_polar_rgb_);
  doppler_polar_viewer_->setQImage(current_doppler_polar_rgb_);
  power_catesian_viewer_->setQImage(current_power_cartesian_rgb_);
  doppler_catesian_viewer_->setQImage(current_doppler_cartesian_rgb_);
}

cv::Mat RfImage::GeneratePolarToCartesianRemapMat(
    const double range_resolution, const double azimuth_resolution) {
  cv::Mat map_xy =
      cv::Mat::zeros(cart_pixel_width_, cart_pixel_width_, CV_64FC2);
#pragma omp parallel for collapse(2)
  for (int i = 0; i < map_xy.rows; ++i) {
    for (int j = 0; j < map_xy.cols; ++j) {
      double x = (cart_pixel_width_ / 2 - i) * range_resolution;
      double y = (j - cart_pixel_width_ / 2) * range_resolution;
      map_xy.at<cv::Vec2d>(i, j) = cv::Vec2d(x, y);
    }
  }
  cv::Mat range_angle =
      cv::Mat::zeros(cart_pixel_width_, cart_pixel_width_, CV_32FC2);
#pragma omp parallel for collapse(2)
  for (int i = 0; i < range_angle.rows; ++i) {
    for (int j = 0; j < range_angle.cols; ++j) {
      cv::Vec2d& x_y = map_xy.at<cv::Vec2d>(i, j);
      float r_v = cv::norm(x_y);
      float r = r_v / range_resolution;
      if (r < 0) r = 0;
      float theta_o = std::atan2(x_y[1], x_y[0]);
      if (theta_o < 0) theta_o += 2 * M_PI;
      range_azimuth_mask_.at<cv::Vec2f>(i, j) = {r_v, theta_o};
      double fix_theta = theta_o + azimuth_bins_ / 2 * azimuth_resolution;
      if (fix_theta < 0) {
        fix_theta += 2 * M_PI;
      } else if (fix_theta > 2 * M_PI) {
        fix_theta -= 2 * M_PI;
      }
      float theta = static_cast<float>(fix_theta / azimuth_resolution);
      range_angle.at<cv::Vec2f>(i, j) = {r, theta};

      if (theta >= 0 && theta <= static_cast<float>(azimuth_bins_) && r >= 0 &&
          r <= static_cast<float>(range_bins_)) {
        cartesian_mask_.at<uchar>(i, j) = 1;
      }
    }
  }
  return range_angle;
}

void RfImage::OnColorMapRangeChanged(std::string chennel, int min, int max) {
  if (chennel == "log_power") {
    power_rf_colormap_.setRange(min, max);
  } else if (chennel == "doppler") {
    // Doppler colormap range should be symmetrical with respect to 0,
    // as the zero value always be white.
    doppler_rf_colormap_.setRange(-std::abs(max), std::abs(max));
  }
  return;
}

void RfImage::RfPointCloudTypeChange() { device_context_->refreshPointCloud(); }

bool RfImage::PointcloudToRawRf0_0(
    const drivers::xradar_2d::PointCloud& point_cloud) {
  current_power_rf_.setTo(0.0);
  current_doppler_rf_.setTo(0.0);
  for (const auto& p : point_cloud.points) {
    int row = p.azimuth_code + azimuth_bins_ / 2;
    int col = p.range_code;
    if (row < 0 || row >= azimuth_bins_ || col < 0 || col >= range_bins_) {
      continue;
    }
    current_power_rf_.at<float>(row, col) = static_cast<float>(p.log_power);
    current_doppler_rf_.at<float>(row, col) = p.doppler;
  }
  return true;
}

bool RfImage::PointcloudToRawRf1_0(
    const drivers::xradar_2d::PointCloud& point_cloud) {
  for (auto& ti : detect_type_infos_) {
    ti.remaped_power_rf.setTo(0.0f);
    ti.remaped_doppler_rf.setTo(0.0f);
    if (ti.remap_type == RfImage::DetectTypeInfo::RemapType::CV_REMAP) {
      ti.origin_power_rf.setTo(0.0f);
      ti.origin_doppler_rf.setTo(0.0f);
    }
  }

  for (const auto& p : point_cloud.points) {
    size_t detact_type_idx = static_cast<size_t>(p.detect_type);
    DetectTypeInfo& infoi = detect_type_infos_[detact_type_idx];
    if (infoi.remap_type == RfImage::DetectTypeInfo::RemapType::CV_REMAP) {
      cv::Mat& origin_power_rf_mat = infoi.origin_power_rf;
      cv::Mat& origin_doppler_rf_mat = infoi.origin_doppler_rf;
      int row =
          static_cast<int>(p.azimuth_code) + infoi.origin_azimuth_bins / 2;
      int col = p.range_code;
      if (row < 0 || row >= infoi.origin_azimuth_bins || col < 0 ||
          col >= infoi.origin_range_bins) {
        continue;
      }
      origin_power_rf_mat.at<float>(row, col) = static_cast<float>(p.log_power);
      origin_doppler_rf_mat.at<float>(row, col) = p.doppler;
    } else if (infoi.remap_type ==
               RfImage::DetectTypeInfo::RemapType::POINT_ASSINE_REMAP) {
      cv::Mat& target_power_rf_mat = infoi.remaped_power_rf;
      cv::Mat& target_doppler_rf_mat = infoi.remaped_doppler_rf;
      int row = static_cast<int>(std::round(static_cast<float>(p.azimuth_code) *
                                            infoi.origin_azimuth_resolution /
                                            base_azimuth_resolution_)) +
                azimuth_bins_ / 2;
      int col = static_cast<int>(std::round(static_cast<float>(p.range_code) *
                                            infoi.origin_range_resolution /
                                            base_range_resolution_));
      if (row < 0 || row >= azimuth_bins_ || col < 0 || col >= range_bins_) {
        continue;
      }
      target_power_rf_mat.at<float>(row, col) = static_cast<float>(p.log_power);
      target_doppler_rf_mat.at<float>(row, col) = p.doppler;
    } else {
      return false;
    }
  }

  for (auto& ti : detect_type_infos_) {
    if (ti.remap_type == RfImage::DetectTypeInfo::RemapType::CV_REMAP) {
      ti.CvRemap();
    }
  }
  cv::waitKey(1);

  current_power_rf_.setTo(0.0f);
  current_doppler_rf_.setTo(0.0f);
  for (int ri = 0; ri < azimuth_bins_; ri++) {
    for (int ci = 0; ci < range_bins_; ci++) {
      std::vector<float> power_compare_vector;
      std::vector<float> doppler_compare_vector;
      for (auto& ti : detect_type_infos_) {
        power_compare_vector.push_back(ti.remaped_power_rf.at<float>(ri, ci));
        doppler_compare_vector.push_back(
            ti.remaped_doppler_rf.at<float>(ri, ci));
      }
      current_power_rf_.at<float>(ri, ci) = *std::max_element(
          power_compare_vector.begin(), power_compare_vector.end());
      current_doppler_rf_.at<float>(ri, ci) = *std::max_element(
          doppler_compare_vector.begin(), doppler_compare_vector.end());
    }
  }
  return true;
}

bool RfImage::DetectTypeInfo::GenerateRemapMat(
    const int target_range_bins, const int target_azimuth_bins,
    const double target_range_resolution,
    const double target_azimuth_resolution) {
  remap_mat = cv::Mat::zeros(target_azimuth_bins, target_range_bins, CV_32FC2);
  for (int ri = 0; ri < target_azimuth_bins; ri++) {
    for (int ci = 0; ci < target_range_bins; ci++) {
      double ra = ci * target_range_resolution;
      double az = (ri - target_azimuth_bins / 2) * target_azimuth_resolution;
      float col_on_origin = ra / origin_range_resolution;
      float row_on_origin = az / origin_azimuth_resolution +
                            static_cast<float>(origin_azimuth_bins / 2);
      remap_mat.at<cv::Vec2f>(ri, ci) = {col_on_origin, row_on_origin};
    }
  }
  return true;
}

bool RfImage::DetectTypeInfo::CvRemap() {
  cv::remap(origin_power_rf, remaped_power_rf, remap_mat, cv::Mat(),
            cv::INTER_LINEAR, cv::BORDER_CONSTANT, cv::Scalar(0.0, 0.0, 0.0));
  cv::remap(origin_doppler_rf, remaped_doppler_rf, remap_mat, cv::Mat(),
            cv::INTER_NEAREST, cv::BORDER_CONSTANT, cv::Scalar(0.0, 0.0, 0.0));
  return true;
}

}  // namespace rf_image
}  // namespace pointview
}  // namespace autox
