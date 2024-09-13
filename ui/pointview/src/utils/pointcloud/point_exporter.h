/******************************************************************************
 * Copyright 2023 AutoX. All Rights Reserved.
 *****************************************************************************/

#ifndef POINTVIEW_POINT_EXPORTER_H
#define POINTVIEW_POINT_EXPORTER_H

#include <QPushButton>
#include <QStringList>
#include <functional>
#include <memory>

#include "pointcloud.h"
#include "utils/common/device_context.h"
#include "utils/property_tree.h"

namespace autox {
namespace pointview {

class PointExporter : public QObject {
  Q_OBJECT
 public:
  using PointInfoCallback =
      std::function<bool(size_t idx, std::vector<double>&)>;
  PointExporter(std::shared_ptr<DeviceContext> device_context,
                QStringList header);
  ~PointExporter() = default;

  void setPointInfoCallback(PointInfoCallback cb) { point_info_cb_ = cb; };
  void update(PointCloudT::Ptr cloud) { cloud_ = cloud; };

 private:
  void exportCSV();
  void exportPCD();

 private:
  std::shared_ptr<DeviceContext> device_context_;
  QStringList header_;
  // item
  std::shared_ptr<QPushButton> btn_export_csv_;
  std::shared_ptr<QPushButton> btn_export_pcd_;
  // point info callback
  PointInfoCallback point_info_cb_;
  // data
  PointCloudT::Ptr cloud_;
};

}  // namespace pointview
}  // namespace autox

#endif  // POINTVIEW_POINT_EXPORTER_H
