/******************************************************************************
 * Copyright 2023 AutoX. All Rights Reserved.
 *****************************************************************************/
//
// Created by tongdayang on 6/20/24.
//

#ifndef POINTVIEW_POINT_DATA_H
#define POINTVIEW_POINT_DATA_H

#include <QPushButton>

#include "point_table.h"
#include "pointcloud.h"
#include "utils/common/device_context.h"

namespace autox {
namespace pointview {
class PointData : public PointTable {
  Q_OBJECT
 public:
  using PointInfoCallback =
      std::function<bool(size_t idx, std::vector<double>&)>;
  PointData(std::shared_ptr<DeviceContext> device_context,
            QStringList head = {});
  void update(PointCloudT::Ptr point_cloud);
  void setPointInfoCallback(PointInfoCallback cb) { point_info_cb_ = cb; };

 private slots:
  void on_refresh_buttonClicked();
  void on_openButtonClicked();

 private:
  PointCloudT::Ptr crt_point_cloud_;
  std::shared_ptr<QPushButton> refresh_button_;
  PointInfoCallback point_info_cb_;
  PropertyPairs properties_;
  std::shared_ptr<DeviceContext> device_context_;
  std::shared_ptr<QPushButton> open_button_;
};
}  // namespace pointview
}  // namespace autox

#endif  // POINTVIEW_POINT_DATA_H
