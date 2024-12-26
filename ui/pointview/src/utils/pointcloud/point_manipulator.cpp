
/******************************************************************************
 * Copyright 2023 AutoX. All Rights Reserved.
 *****************************************************************************/

#include "point_manipulator.h"

namespace autox {
namespace pointview {

PointManipulator::PointManipulator(
    std::shared_ptr<DeviceContext> device_context,
    std::vector<ChannelSetting> channel_settings)
    : QObject(device_context->getDisplayContext()->getParent()),
      device_context_(device_context),
      viewer_(device_context->getDisplayContext()->getViewerPtr()) {
  // init data
  channel_settings_ = channel_settings;
  colormap_ = std::make_shared<Colormap>();
  // check colormap
  QStringList colormap_names{"gray",   "jet",     "bwr",
                             "plasma", "rainbow", "AutoxMaxRange"};
  for (int i = 0; i < colormap_names.size(); ++i) {
    colormap_idx_map_[colormap_names.at(i).toStdString()] = i;
  }
  QStringList channel_names;
  for (size_t i = 0; i < channel_settings_.size(); i++) {
    channel_names.append(
        QString::fromStdString(channel_settings_[i].channel_name));
    if (colormap_idx_map_.find(channel_settings_[i].default_colormap_name) ==
        colormap_idx_map_.end()) {
      LOG(INFO) << "Unsupported colormap: "
                << channel_settings_[i].default_colormap_name ;
      channel_settings_[i].default_colormap_name =
          colormap_names.at(0).toStdString();
    }
  }
  auto sub =
      device_context->getPropertyTree()->createPropertySubTree("Point Setting");
  // channel_selection
  channel_selection_ = std::make_shared<QComboBox>();
  channel_selection_->addItems(channel_names);
  connect(channel_selection_.get(), SIGNAL(currentIndexChanged(int)), this,
          SLOT(pointCloudChannelChanged(int)));
  sub->addProperty("color mode", channel_selection_);
  // range min/max
  range_min_spinbox_ = std::make_shared<QDoubleSpinBox>();
  range_max_spinbox_ = std::make_shared<QDoubleSpinBox>();
  range_min_spinbox_->setSingleStep(0.1);
  range_max_spinbox_->setSingleStep(0.1);
  range_min_spinbox_->setRange(-1000, 1000);
  range_max_spinbox_->setRange(-1000, 1000);
  connect(range_min_spinbox_.get(), SIGNAL(valueChanged(double)), this,
          SLOT(pointCloudRangeChanged()));
  connect(range_max_spinbox_.get(), SIGNAL(valueChanged(double)), this,
          SLOT(pointCloudRangeChanged()));
  sub->addProperty("min value", range_min_spinbox_);
  sub->addProperty("max value", range_max_spinbox_);
  // colormap_selection
  colormap_selection_ = std::make_shared<QComboBox>();
  colormap_selection_->addItems(colormap_names);
  connect(colormap_selection_.get(), SIGNAL(currentIndexChanged(int)), this,
          SLOT(pointCloudColormapChanged()));
  sub->addProperty("colormap", colormap_selection_);
  // colormap image
  colormap_label_ = std::make_shared<ResizeLabel>();
  sub->addProperty("colorbar", colormap_label_);
  connect(colormap_label_.get(), SIGNAL(Resize()), this,
          SLOT(pointCloudColormapChanged()));
  // gamma tree
  auto gamma_enable_sub = sub->createPropertySubTree("gamma");
  checkbox_enable_gamma_ = std::make_shared<QCheckBox>();
  connect(checkbox_enable_gamma_.get(), &QCheckBox::stateChanged,
          [this](int state) {
            colormap_->setUseGamma(state > 0);
          });
  gamma_enable_sub->addProperty("enable", checkbox_enable_gamma_);
  btn_gamma_file_choose_ = std::make_shared<QPushButton>("open");
  connect(btn_gamma_file_choose_.get(), SIGNAL(clicked(bool)), this,
        SLOT(openGammaFile()));
  gamma_enable_sub->addProperty("gamma file", btn_gamma_file_choose_);
  label_gamma_file_path_ = std::make_shared<QLabel>();
  gamma_enable_sub->addProperty("file path", label_gamma_file_path_);
  //
  spinbox_point_size_ = std::make_shared<QSpinBox>();
  spinbox_point_size_->setRange(1, 5);
  connect(spinbox_point_size_.get(),
          static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
          [this](int v) {
            point_size_ = v;
            device_context_->refreshPointCloud();
          });
  sub->addProperty("size", spinbox_point_size_);
  // hide button
  checkbox_hide_ = std::make_shared<QCheckBox>();
  connect(checkbox_hide_.get(), &QCheckBox::stateChanged, [this](int state) {
    is_hided_ = (state > 0);
    device_context_->refreshPointCloud();
  });
  sub->addProperty("hide", checkbox_hide_);
  // update widgets
  channel_selection_->setCurrentIndex(0);
  pointCloudChannelChanged(0);
}

bool PointManipulator::InitFromConfig(
    std::shared_ptr<autox::pointview::Config> config) {
  int mode_index = -1;
  config->getParameter(namespace_ + ".mode_index", mode_index);
  if (mode_index >= 0 && mode_index < channel_settings_.size()) {
    auto& setting = channel_settings_[mode_index];
    std::string colormap;
    config->getParameter(namespace_ + ".colormap", colormap);
    if (colormap_idx_map_.find(colormap) != colormap_idx_map_.end()) {
      setting.default_colormap_name = colormap;
    } else {
      LOG(INFO) << "Unsupported colormap: " << colormap ;
    }
    config->getParameter(namespace_ + ".min_value", setting.default_range_min);
    config->getParameter(namespace_ + ".max_value", setting.default_range_max);
    config->getParameter(namespace_ + ".point_size", point_size_);
    spinbox_point_size_->setValue(point_size_);
    config->getParameter(namespace_ + ".is_hided", is_hided_);
    checkbox_hide_->setChecked(is_hided_);
    channel_selection_->setCurrentIndex(mode_index);
    pointCloudChannelChanged(mode_index);
  }
  return true;
}

bool PointManipulator::StoreToConfig(
    std::shared_ptr<autox::pointview::Config> config) {
  config->setParameter(namespace_ + ".mode_index",
                       channel_selection_->currentIndex());
  config->setParameter(namespace_ + ".colormap",
                       colormap_selection_->currentText().toStdString());
  config->setParameter(namespace_ + ".min_value", range_min_spinbox_->value());
  config->setParameter(namespace_ + ".max_value", range_max_spinbox_->value());
  config->setParameter(namespace_ + ".point_size", point_size_);
  config->setParameter(namespace_ + ".is_hided", is_hided_);
  return true;
}

void PointManipulator::pointCloudChannelChanged(int index) {
  channel_idx_ = index;
  auto& setting = channel_settings_[index];
  colormap_->setRange(setting.default_range_min, setting.default_range_max);
  range_min_spinbox_->setValue(setting.default_range_min);
  range_max_spinbox_->setValue(setting.default_range_max);
  colormap_selection_->setCurrentIndex(
      colormap_idx_map_[setting.default_colormap_name]);
  device_context_->refreshPointCloud();
}

bool PointManipulator::readGammaFile(std::string filename) {
  return colormap_->readGammaFile(filename);
}

void PointManipulator::openGammaFile()
{
  QString gamma_file = GetOpenFileName("Open gamma File", last_gamma_open_dirpath_,
                                      "gamma File(*.csv);All Files(*.*)");
  if (gamma_file.isNull()) {
    LOG(INFO) << "Do not select a target file.";
    return;
  }
  last_gamma_open_dirpath_ = QFileInfo(gamma_file).dir().absolutePath();
  if(!readGammaFile(gamma_file.toStdString())) {
    label_gamma_file_path_->setText("");
    return;
  }
  label_gamma_file_path_->setText(gamma_file);
}

void PointManipulator::pointCloudRangeChanged() {
  if (range_min_spinbox_->value() < range_max_spinbox_->value()) {
    colormap_->setRange(range_min_spinbox_->value(),
                        range_max_spinbox_->value());
    emit ColorMapRangeChanged(channel_selection_->currentText().toStdString(),
                              range_min_spinbox_->value(),
                              range_max_spinbox_->value());
    device_context_->refreshPointCloud();
  }
}

void PointManipulator::pointCloudColormapChanged() {
  std::string crt_color_map = colormap_selection_->currentText().toStdString();
  colormap_->setType(crt_color_map);
  QImage img;
  img.load(
      QString::fromStdString(":/pointview_icon/" + crt_color_map + ".png"));
  colormap_label_->setPixmap(
      QPixmap::fromImage(img.scaled(colormap_label_->size())));
  if (crt_color_map == "AutoxMaxRange") {
    range_max_spinbox_->setEnabled(false);
    range_min_spinbox_->setEnabled(false);
  } else {
    range_max_spinbox_->setEnabled(true);
    range_min_spinbox_->setEnabled(true);
  }
  device_context_->refreshPointCloud();
}

void PointManipulator::update(const std::string& cloud_id) {
  auto v = (is_hided_ ? 0 : 1.0);
  // hide
  viewer_->setPointCloudRenderingProperties(
      pcl::visualization::PCL_VISUALIZER_OPACITY, v, cloud_id);
  // point size
  viewer_->setPointCloudRenderingProperties(
      pcl::visualization::PCL_VISUALIZER_POINT_SIZE, point_size_, cloud_id);
}

}  // namespace pointview
}  // namespace autox
