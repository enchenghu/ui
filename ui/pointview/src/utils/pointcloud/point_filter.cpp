
/******************************************************************************
 * Copyright 2023 AutoX. All Rights Reserved.
 *****************************************************************************/

#include "point_filter.h"

#include "utils.h"

namespace autox {
namespace pointview {

PointFilter::PointFilter(std::shared_ptr<DeviceContext> device_context,
                         PointFilterSetting setting)
    : QObject(device_context->getDisplayContext()->getParent()) {
  device_context_ = device_context;
  auto sub =
      device_context->getPropertyTree()->createPropertySubTree("point filter");
  // enable
  checkbox_enable_filter_ = std::make_shared<QCheckBox>();
  sub->addProperty("enable", checkbox_enable_filter_);
  connect(checkbox_enable_filter_.get(), &QCheckBox::stateChanged,
          [this](int state) {
            enbale_filter_ = state > 0;
            device_context_->refreshPointCloud();
          });
  // reverse mode and color mode
  checkbox_reverse_mode_ = std::make_shared<QCheckBox>();
  sub->addProperty("reverse mode", checkbox_reverse_mode_);
  connect(checkbox_reverse_mode_.get(), &QCheckBox::stateChanged,
          [this](int state) {
            reverse_mode_ = state > 0;
            device_context_->refreshPointCloud();
          });
  checkbox_color_mode_ = std::make_shared<QCheckBox>();
  sub->addProperty("color mode", checkbox_color_mode_);
  connect(checkbox_color_mode_.get(), &QCheckBox::stateChanged,
          [this](int state) {
            color_mode_ = state > 0;
            device_context_->refreshPointCloud();
          });
  // create default items for xyz
  std::vector<std::string> xyz_titles{"x", "y", "z"};
  for (size_t i = 0; i < xyz_titles.size(); i++) {
    auto item_sub =
        sub->createPropertySubTree(QString::fromStdString(xyz_titles[i]));

    // item
    xyz_items_.emplace_back();
    int idx = xyz_items_.size() - 1;
    auto& item = xyz_items_.back();
    item.title = xyz_titles[i];
    item.min_value = -500;
    item.max_value = 500;
    item.spinbox_min = std::make_shared<QDoubleSpinBox>();
    item.spinbox_min->setSuffix(QString::fromStdString("m"));
    item.spinbox_min->setRange(item.min_value, item.max_value * 10);
    item.spinbox_min->setValue(item.min_value);
    connect(item.spinbox_min.get(),
            static_cast<void (QDoubleSpinBox::*)(double)>(
                &QDoubleSpinBox::valueChanged),
            [idx, this](double v) {
              xyz_items_[idx].min_value = v;
              device_context_->refreshPointCloud();
            });
    item_sub->addProperty("min", item.spinbox_min);
    item.spinbox_max = std::make_shared<QDoubleSpinBox>();
    item.spinbox_max->setSuffix(QString::fromStdString("m"));
    item.spinbox_max->setRange(item.min_value, item.max_value * 10);
    item.spinbox_max->setValue(item.max_value);
    connect(item.spinbox_max.get(),
            static_cast<void (QDoubleSpinBox::*)(double)>(
                &QDoubleSpinBox::valueChanged),
            [idx, this](double v) {
              xyz_items_[idx].max_value = v;
              device_context_->refreshPointCloud();
            });
    item_sub->addProperty("max", item.spinbox_max);
  }
  // create items
  for (size_t i = 0; i < setting.titles_.size(); i++) {
    auto title = setting.titles_[i];
    auto type = setting.types_[title];
    if (type == PointFilterSetting::ItemType::IntItemType) {
      auto info = setting.int_items_[title];
      auto item_sub =
          sub->createPropertySubTree(QString::fromStdString(info.title));

      // item
      int_items_.emplace_back();
      int idx = int_items_.size() - 1;
      auto& item = int_items_.back();
      item.title = info.title;
      item.min_value = info.min_value;
      item.max_value = info.max_value;
      item.step = 1;
      item.cb = info.cb;
      item.spinbox_min = std::make_shared<QSpinBox>();
      item.spinbox_min->setSuffix(QString::fromStdString(info.suffix));
      item.spinbox_min->setRange(info.min_value, info.max_value * 10);
      item.spinbox_min->setValue(info.min_value);
      connect(item.spinbox_min.get(),
              static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
              [idx, this](int v) {
                int_items_[idx].min_value = v;
                if (v == int_items_[idx].max_value + 1) {
                  int_items_[idx].max_value = v;
                  int_items_[idx].spinbox_max->setValue(v);
                }
                device_context_->refreshPointCloud();
              });
      item_sub->addProperty("min", item.spinbox_min);
      item.spinbox_max = std::make_shared<QSpinBox>();
      item.spinbox_max->setSuffix(QString::fromStdString(info.suffix));
      item.spinbox_max->setRange(info.min_value, info.max_value * 10);
      item.spinbox_max->setValue(info.max_value);
      connect(item.spinbox_max.get(),
              static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
              [idx, this](int v) {
                int_items_[idx].max_value = v;
                if (v == int_items_[idx].min_value - 1) {
                  int_items_[idx].min_value = v;
                  int_items_[idx].spinbox_min->setValue(v);
                }
                device_context_->refreshPointCloud();
              });
      item_sub->addProperty("max", item.spinbox_max);
      if (info.use_step) {
        item.spinbox_step = std::make_shared<QSpinBox>();
        item.spinbox_step->setSuffix(QString::fromStdString(info.suffix));
        item.spinbox_step->setMinimum(1);
        item.spinbox_step->setValue(item.step);
        connect(item.spinbox_step.get(),
                static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
                [idx, this](int v) {
                  int_items_[idx].step = v;
                  device_context_->refreshPointCloud();
                });
        item_sub->addProperty("step", item.spinbox_step);
      }
    } else if (type == PointFilterSetting::ItemType::DoubleItemType) {
      auto info = setting.double_items_[title];
      auto item_sub =
          sub->createPropertySubTree(QString::fromStdString(info.title));

      // item
      double_items_.emplace_back();
      int idx = double_items_.size() - 1;
      auto& item = double_items_.back();
      item.title = info.title;
      item.min_value = info.min_value;
      item.max_value = info.max_value;
      item.cb = info.cb;
      item.spinbox_min = std::make_shared<QDoubleSpinBox>();
      item.spinbox_min->setSuffix(QString::fromStdString(info.suffix));
      item.spinbox_min->setRange(info.min_value, info.max_value * 10);
      item.spinbox_min->setValue(info.min_value);
      connect(item.spinbox_min.get(),
              static_cast<void (QDoubleSpinBox::*)(double)>(
                  &QDoubleSpinBox::valueChanged),
              [idx, this](double v) {
                double_items_[idx].min_value = v;
                device_context_->refreshPointCloud();
              });
      item_sub->addProperty("min", item.spinbox_min);
      item.spinbox_max = std::make_shared<QDoubleSpinBox>();
      item.spinbox_max->setSuffix(QString::fromStdString(info.suffix));
      item.spinbox_max->setRange(info.min_value, info.max_value * 10);
      item.spinbox_max->setValue(info.max_value);
      connect(item.spinbox_max.get(),
              static_cast<void (QDoubleSpinBox::*)(double)>(
                  &QDoubleSpinBox::valueChanged),
              [idx, this](double v) {
                double_items_[idx].max_value = v;
                device_context_->refreshPointCloud();
              });
      item_sub->addProperty("max", item.spinbox_max);
    }
  }
}

bool PointFilter::checkValid(size_t i, double x, double y, double z) {
  if (x < xyz_items_[0].min_value || x > xyz_items_[0].max_value) {
    return false;
  }
  if (y < xyz_items_[1].min_value || y > xyz_items_[1].max_value) {
    return false;
  }
  if (z < xyz_items_[2].min_value || z > xyz_items_[2].max_value) {
    return false;
  }
  int data;
  for (auto& item : int_items_) {
    if (item.cb(i, data)) {
      if (data < item.min_value || data > item.max_value) {
        return false;
      }
      if ((data - item.min_value) % item.step != 0) {
        return false;
      }
    } else {
      LOG(INFO) << "warnning, index overflow in point filter" ;
      return false;
    }
  }
  double data2;
  for (auto& item : double_items_) {
    if (item.cb(i, data2)) {
      if (data2 < item.min_value || data2 > item.max_value) {
        return false;
      }
    } else {
      LOG(INFO) << "warnning, index overflow in point filter" ;
      return false;
    }
  }
  return true;
}

bool PointFilter::initFromConfig(
    std::shared_ptr<autox::pointview::Config> config) {
  if (!config) {
    return false;
  }
  return true;
}

bool PointFilter::storeToConfig(
    std::shared_ptr<autox::pointview::Config> config) {
  if (config) {
    return false;
  }
  return true;
}

void PointFilter::update(PointCloudT::Ptr cloud) {
  if (!enbale_filter_) {
    return;
  }
  for (size_t i = 0; i < cloud->points.size(); i++) {
    auto& p = cloud->points[i];
    if (p.a == 0) {
      continue;
    }
    bool ok = checkValid(i, p.x, p.y, p.z);
    if ((reverse_mode_ && ok) || (!reverse_mode_ && !ok)) {
      if (color_mode_) {
        p.r = 255;
        p.g = 0;
        p.b = 0;
      } else {
        p.a = 0;
      }
    }
  }
}

}  // namespace pointview
}  // namespace autox
