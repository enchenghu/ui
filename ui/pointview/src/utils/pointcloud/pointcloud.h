/******************************************************************************
 * Copyright 2023 AutoX. All Rights Reserved.
 *****************************************************************************/
//
// Created by tongdayang on 11/17/22.
//

#ifndef POINT_VIEW_POINTCLOUD_H
#define POINT_VIEW_POINTCLOUD_H

#include <pcl/point_cloud.h>
#include <pcl/point_types.h>

using PointT = pcl::PointXYZRGBA;
using PointCloudT = pcl::PointCloud<PointT>;

namespace autox {
namespace pointview {
template <typename RawPointT>
struct RawPointCloud {
  double timestamp;
  double ethernet_timestamp;
  size_t udp_packet_number;
  uint8_t return_mode;
  std::vector<uint64_t> udp_scan_sequence;
  std::vector<RawPointT> points;
};

}  // namespace pointview
}  // namespace autox

#endif  // POINT_VIEW_POINTCLOUD_H
