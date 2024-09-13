/******************************************************************************
 * Copyright 2021 AutoX Inc. All Rights Reserved.
 *****************************************************************************/

#pragma once

#include <array>
#include <atomic>
#include <cmath>
#include <functional>
#include <memory>
#include <string>
#include <vector>

constexpr int kSinCosTableResolution = 100;
constexpr unsigned int kRotationMaxUnits = 360 * kSinCosTableResolution + 1;
constexpr double kDegreeToRadian = M_PI / 180.;

constexpr int kHsLidarL64UnitNum = 64;
// Block number 6 or 7
constexpr int kHsLidarL64BlockNumber_7 = 7;
// each block have  64 Unit
constexpr int kHsLidarL64BlockNumber_6 = 6;

namespace autox {
namespace drivers {
namespace hesai_qt64 {

struct Point {
  uint8_t laser_id;
  uint8_t return_id;
  double timestamp;
  float encoder;
  float azimuth;
  float elevation;
  float distance;
  float intensity;
  float x;
  float y;
  float z;
};

struct PointCloud {
  /* data */
  double timestamp;
  size_t udp_packet_number;
  uint8_t return_mode;
  std::vector<uint64_t> udp_scan_sequence;
  std::vector<Point> points;
  size_t begin_index_in_pacp;
  size_t end_index_in_pacp;  
};

typedef struct PandarQtPacket_s PandarQtPacket;

class DriverQt64 {
 public:
  using PointCloudCallbackType =
      std::function<void(std::shared_ptr<PointCloud>&, long long)>;
  DriverQt64();
  ~DriverQt64();

  bool ParseLidarPacket(const uint8_t* data, size_t len);
  void CalcPointXYZIT(const PandarQtPacket* block, const unsigned int blockid,
                      const unsigned int laser_num,
                      const uint64_t pkt_timestamp, const double distance_unit,
                      const double azimuth_in_deg);

  float CosLookupDegree(const float angle_in_deg);
  float SinLookupDegree(const float angle_in_deg);
  bool CheckAngle(const int cur_azimuth);

  void setPointCloudCallback(PointCloudCallbackType cb) {
    point_cloud_callback_ = cb;
  };
  void resetDriver();
  void resetFrameIndex();
  void setCurPacketIndex(size_t index){cur_packet_index_ = index;}

 private:
  std::vector<double> elev_angle_map_;
  std::vector<double> horizatal_azimuth_offset_map_;

  double min_laser_azimuth_offset_;
  double max_laser_azimuth_offset_;

  static std::array<double, kRotationMaxUnits> sin_lookup_table_;
  static std::array<double, kRotationMaxUnits> cos_lookup_table_;

  int start_angle_{180};
  int last_azimuth_{0};
  int azimuth_sign_{1};  // 1 means clockwise, -1 means counterclockwise
  int accumulate_azimuth_{0};
  float max_distance_{200};
  float min_distance_{0.5};
  std::atomic<bool> is_dual_return_{false};
  //
  double block_64_offset_dual_[kHsLidarL64BlockNumber_6];
  double block_64_offset_[kHsLidarL64BlockNumber_6];
  double laser_64_offset_[kHsLidarL64UnitNum];
  //
  std::shared_ptr<PointCloud> point_cloud_;
  PointCloudCallbackType point_cloud_callback_;
  // point cloud buffer
  std::vector<std::shared_ptr<PointCloud>> point_cloud_buffer_;
  size_t max_point_cloud_buffer_size_{10};
  size_t point_cloud_buffer_index_{0};
  long long index_f_single_thread_{0};
  size_t cur_packet_index_{0};
};

}  // namespace hesai_qt64
}  // namespace drivers
}  // namespace autox
