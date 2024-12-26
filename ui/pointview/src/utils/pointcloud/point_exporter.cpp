/******************************************************************************
 * Copyright 2023 AutoX. All Rights Reserved.
 *****************************************************************************/

#include "point_exporter.h"

#include <QMessageBox>

#include <pcl/io/pcd_io.h>

#include "utils.h"

namespace autox {
namespace pointview {
PointExporter::PointExporter(std::shared_ptr<DeviceContext> device_context,
                             QStringList header)
    : QObject(device_context->getDisplayContext()->getParent()) {
  device_context_ = device_context;
  header_ = header;
  auto sub = device_context_->getPropertyTree()->createPropertySubTree(
      "point exporter");
  // properties
  btn_export_csv_ = std::make_shared<QPushButton>();
  btn_export_csv_->setText("export");
  sub->addProperty("csv", btn_export_csv_);
  connect(btn_export_csv_.get(), &QPushButton::clicked,
          [this]() { exportCSV(); });
  btn_export_pcd_ = std::make_shared<QPushButton>();
  btn_export_pcd_->setText("export");
  sub->addProperty("pcd (xyz)", btn_export_pcd_);
  connect(btn_export_pcd_.get(), &QPushButton::clicked,
          [this]() { exportPCD(); });
}

void PointExporter::exportCSV() {
  if (device_context_->getPlayerState().is_playing) {
    QMessageBox::warning(nullptr, "PointExporter",
                         "please pause player before export!", QMessageBox::Ok);
    return;
  }
  if (cloud_->points.size() == 0) {
    QMessageBox::warning(nullptr, "PointExporter",
                         "failed to save CSV, no data.", QMessageBox::Ok);
    return;
  }
  // open file dialog
  QString filename =
      GetSaveFileName("Save point cloud", QDir::homePath() + "/Untitled.csv",
                      "CSV Files(*.csv);;All Files(*.*)");
  if (filename.isNull()) {
    LOG(INFO) << "Do not select a target file.";
    return;
  }
  // open file
  std::ofstream out_file(filename.toStdString());
  if (!out_file) {
    return;
  }
  // save head
  for (int i = 0; i < header_.size(); i++) {
    if (i == header_.size() - 1) {
      out_file << header_[i].toStdString();
    } else {
      out_file << header_[i].toStdString() << ",";
    }
  }
  // save points
  std::vector<double> data;
  for (size_t k = 0; k < cloud_->points.size(); k++) {
    auto& p = cloud_->points[k];
    if (p.a == 0) {
      continue;
    }
    if (!point_info_cb_(k, data)) {
      continue;
    }
    if (data.size() != header_.size()) {
      LOG(INFO) << "invalid row size:" << data.size();
      continue;
    }
    // save a row
    for (size_t i = 0; i < data.size(); i++) {
      if (i == data.size() - 1) {
        out_file << data[i];
      } else {
        out_file << data[i] << ",";
      }
    }
  }
  out_file.close();
  QMessageBox::information(nullptr, "PointExporter", "save CSV successfully!",
                           QMessageBox::Ok);
  LOG(INFO) << "save csv successfully :" << filename.toStdString();
}

void PointExporter::exportPCD() {
  if (device_context_->getPlayerState().is_playing) {
    QMessageBox::warning(nullptr, "PointExporter",
                         "please pause player before export!", QMessageBox::Ok);
    return;
  }
  if (cloud_->points.size() == 0) {
    QMessageBox::warning(nullptr, "PointExporter",
                         "failed to save PCD, no data.", QMessageBox::Ok);
    return;
  }
  // open file dialog
  QString filename =
      GetSaveFileName("Save point cloud", QDir::homePath() + "/Untitled.pcd",
                      "PCD Files(*.pcd);;All Files(*.*)");
  if (filename.isNull()) {
    LOG(INFO) << "Do not select a target file.";
    return;
  }
  // convert to pcl::PointCloud
  pcl::PointCloud<pcl::PointXYZ>::Ptr point_cloud(
      new pcl::PointCloud<pcl::PointXYZ>);
  for (size_t i = 0; i < cloud_->points.size(); i++) {
    auto& p = cloud_->points[i];
    if (p.a == 0) {
      continue;
    }
    pcl::PointXYZ p2;
    p2.x = p.x;
    p2.y = p.y;
    p2.z = p.z;
    point_cloud->push_back(p2);
  }
  // save pcl::PointCloud
  pcl::io::savePCDFileBinary(filename.toStdString(), *point_cloud);
  QMessageBox::information(nullptr, "PointExporter", "save PCD successfully!",
                           QMessageBox::Ok);
  LOG(INFO) << "save PCD successfully :" << filename.toStdString();
}

}  // namespace pointview
}  // namespace autox
