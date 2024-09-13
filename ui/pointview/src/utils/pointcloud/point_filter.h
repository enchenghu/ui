/******************************************************************************
 * Copyright 2023 AutoX. All Rights Reserved.
 *****************************************************************************/

#ifndef POINTVIEW_POINT_FILTER_H
#define POINTVIEW_POINT_FILTER_H

#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QString>
#include <QWidget>
#include <memory>
#include <string>
#include <vector>

#include "pointcloud.h"
#include "utils/common/device_context.h"
#include "utils/config.h"

namespace autox {
namespace pointview {

class PointFilterSetting {
 public:
  using IntCallback = std::function<bool(size_t idx, int& data)>;
  using DoubleCallback = std::function<bool(size_t idx, double& data)>;
  struct IntItem {
    std::string title;
    std::string suffix;
    int min_value;
    int max_value;
    bool use_step;
    IntCallback cb;
  };
  struct DoubleItem {
    std::string title;
    std::string suffix;
    double min_value;
    double max_value;
    DoubleCallback cb;
  };
  enum class ItemType { IntItemType, DoubleItemType };

  PointFilterSetting() = default;
  ~PointFilterSetting() = default;
  // title,unit suffix,min,max,int callback,step
  void addIntItem(std::string title, std::string suffix, int min, int max,
                  IntCallback cb, bool use_step = false) {
    IntItem item;
    item.title = title;
    item.suffix = suffix;
    item.min_value = min;
    item.max_value = max;
    item.use_step = use_step;
    item.cb = cb;
    int_items_[title] = item;
    titles_.push_back(title);
    types_[title] = ItemType::IntItemType;
  }
  // title,unit suffix,min,max,double callback
  void addDoubleItem(std::string title, std::string suffix, double min,
                     double max, DoubleCallback cb) {
    DoubleItem item;
    item.title = title;
    item.suffix = suffix;
    item.min_value = min;
    item.max_value = max;
    item.cb = cb;
    double_items_[title] = item;
    titles_.push_back(title);
    types_[title] = ItemType::DoubleItemType;
  }
  std::vector<std::string> titles_;
  std::map<std::string, ItemType> types_;
  std::map<std::string, IntItem> int_items_;
  std::map<std::string, DoubleItem> double_items_;
};

class PointFilter : public QObject {
  Q_OBJECT

 public:
  using IntCallback = std::function<bool(size_t idx, int& data)>;
  using DoubleCallback = std::function<bool(size_t idx, double& data)>;
  struct IntItem {
    std::string title;
    int min_value;
    int max_value;
    int step;
    IntCallback cb;
    std::shared_ptr<QSpinBox> spinbox_min;
    std::shared_ptr<QSpinBox> spinbox_max;
    std::shared_ptr<QSpinBox> spinbox_step;
  };
  struct DoubleItem {
    std::string title;
    double min_value;
    double max_value;
    DoubleCallback cb;
    std::shared_ptr<QDoubleSpinBox> spinbox_min;
    std::shared_ptr<QDoubleSpinBox> spinbox_max;
  };
  PointFilter(std::shared_ptr<DeviceContext> device_context,
              PointFilterSetting setting);
  ~PointFilter() = default;

 public:
  bool initFromConfig(std::shared_ptr<autox::pointview::Config> config);
  bool storeToConfig(std::shared_ptr<autox::pointview::Config> config);
  void update(PointCloudT::Ptr cloud);

 private:
  bool checkValid(size_t i, double x, double y, double z);

 private:
  std::shared_ptr<DeviceContext> device_context_;
  // property namespace
  std::string namespace_{"point_filter"};
  // property widgets
  std::shared_ptr<QCheckBox> checkbox_enable_filter_;
  std::shared_ptr<QCheckBox> checkbox_reverse_mode_;
  std::shared_ptr<QCheckBox> checkbox_color_mode_;
  // property values
  bool enbale_filter_{false};
  bool reverse_mode_{false};
  bool color_mode_{false};
  // filter items
  std::vector<IntItem> int_items_;
  std::vector<DoubleItem> double_items_;
  std::vector<DoubleItem> xyz_items_;
};

}  // namespace pointview
}  // namespace autox

#endif  // POINTVIEW_POINT_FILTER_H
