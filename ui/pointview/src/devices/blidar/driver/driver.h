/******************************************************************************
 * Copyright 2023 AutoX. All Rights Reserved.
 *****************************************************************************/

#pragma once

#include <array>
#include <cmath>
#include <condition_variable>
#include <deque>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

constexpr int kSinCosTableResolution = 100;
constexpr unsigned int kRotationMaxUnits = 360 * kSinCosTableResolution + 1;
constexpr double kDegreeToRadian = M_PI / 180.;

constexpr int kLidarUnitNum = 4;
constexpr int kLidarBlockNum = 60;
constexpr int kMirrorNum = 3;

namespace autox {
namespace drivers {
namespace blidar {

struct Point {
  uint8_t laser_id;
  uint8_t return_id;
  uint8_t frame_id;
  uint8_t intensity;
  uint8_t elongation;
  double timestamp;
  uint16_t azimuth_encoder;
  uint16_t elevation_encoder;
  float azimuth;
  float elevation;
  float distance;
  float x;
  float y;
  float z;
};

struct PointCloud {
  double timestamp;
  size_t udp_packet_number;
  uint8_t version;
  uint8_t max_return_num;
  std::vector<uint64_t> udp_sequences;
  std::vector<Point> points;
  size_t begin_index_in_pacp;
  size_t end_index_in_pacp;
  long offset_in_pcap;
};

typedef struct LidarPacket_s LidarPacket;

class Driver {
 public:
  using PointCloudCallbackType =
      std::function<void(std::shared_ptr<PointCloud>, long long)>;
  Driver();
  ~Driver();

  bool ParseLidarPacket(const uint8_t* data, size_t len);
  void setPointCloudCallback(PointCloudCallbackType cb);
  void resetDriver();
  void resetDriverLoad();
  void setCurPacketIndex(size_t index) { cur_packet_index_ = index; }
  void setCurPacketOffset(long p) { cur_packet_offset_ = p; };
  bool setIntrinsics(const std::vector<double>& elevation,
                     const std::vector<double>& azimuth,
                     const std::vector<double>& distance);
  bool getIntrinsics(std::vector<double>& elevation,
                     std::vector<double>& azimuth,
                     std::vector<double>& distance);

 private:
  bool ParsePacket(const uint8_t* data, size_t len,
                   std::shared_ptr<PointCloud> point_cloud);
  bool ParsePacket5_1(const uint8_t* data, size_t len,
                      std::shared_ptr<PointCloud> point_cloud);
  bool parsePacket5_2(const uint8_t* data, size_t len,
                      std::shared_ptr<PointCloud> point_cloud);
  void CalcPointXYZIT(const LidarPacket* udp_pkt, unsigned int block_id,
                      uint8_t return_id, unsigned int laser_num,
                      uint64_t pkt_timestamp_ns, double distance_unit,
                      std::shared_ptr<PointCloud> point_cloud);
  float CosLookupDegree(const float angle_in_deg);
  float SinLookupDegree(const float angle_in_deg);

 private:
  // lidar intrinsics
  double elevation_offset_map_[kLidarUnitNum];
  double azimuth_offset_map_[kLidarUnitNum];
  double distance_offset_map_[kLidarUnitNum];
  // for mirror
  std::vector<double> start_frame_;
  std::vector<double> end_frame_;
  // time
  double block_64_offset_dual_[kLidarBlockNum];
  double block_64_offset_[kLidarBlockNum];
  double laser_64_offset_[kLidarUnitNum];
  // sin/cos table
  static std::array<double, kRotationMaxUnits> sin_lookup_table_;
  static std::array<double, kRotationMaxUnits> cos_lookup_table_;
  // filter
  float max_distance_{200};
  float min_distance_{0.01};
  //
  std::shared_ptr<PointCloud> point_cloud_;
  PointCloudCallbackType point_cloud_callback_;
  long long index_f_single_thread_{0};
  size_t cur_packet_index_{0};
  int last_packet_count_{0};
  long cur_packet_offset_{0};
};

}  // namespace blidar
}  // namespace drivers
}  // namespace autox
