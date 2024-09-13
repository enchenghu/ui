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

#include "hesai/functional_safety.h"

constexpr int kSinCosTableResolution = 100;
constexpr unsigned int kRotationMaxUnits = 360 * kSinCosTableResolution + 1;
constexpr double kDegreeToRadian = M_PI / 180.;

namespace autox {
namespace drivers {
namespace hesai {

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
  double timestamp;
  double ethernet_timestamp;
  size_t udp_packet_number;
  uint8_t return_mode;
  std::vector<uint64_t> udp_scan_sequence;
  std::vector<Point> points;
  size_t begin_index_in_pacp;
  size_t end_index_in_pacp;  
};

typedef struct Pandar128Packet_s Pandar128Packet;

class Pandar128Driver {
 public:
  using PointCloudCallbackType =
      std::function<void(std::shared_ptr<PointCloud>&, long long)>;
  Pandar128Driver();
  ~Pandar128Driver();

  bool ParseLidarPacket(const uint8_t* data, size_t len);
  void CalcBlockXYZIT(const Pandar128Packet* pkt, const int blockid,
                      double pkt_time_ns);
  bool CheckLidarHealth(const Pandar128Packet* pkt);
  bool CheckLidarRange(const int cur_azimuth) const;

  float CosLookupDegree(const float angle_in_deg);
  float SinLookupDegree(const float angle_in_deg);
  bool CheckAngle(const int cur_azimuth);

  void setPointCloudCallback(PointCloudCallbackType cb) {
    point_cloud_callback_ = cb;
  };
  void resetDriver();
  void resetFrameIndex();
  void setCurPacketIndex(size_t index){cur_packet_index_ = index;}

  bool setIntrinsics(const std::vector<double>& elevation,
                     const std::vector<double>& azimuth,
                     const std::vector<double>& distance);
  bool getIntrinsics(std::vector<double>& elevation,
                     std::vector<double>& azimuth,
                     std::vector<double>& distance);

 private:
  std::vector<double> elev_angle_map_;
  std::vector<double> horizatal_azimuth_offset_map_;
  std::vector<double> distance_offset_map_;
  std::vector<std::pair<int, int>> lidar_ranges_;  // first: start, second: end

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
  uint8_t return_mode_{0};

  FaultLevel fault_level_{FaultLevel::NF};
  std::string health_report_;
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

}  // namespace hesai
}  // namespace drivers
}  // namespace autox
