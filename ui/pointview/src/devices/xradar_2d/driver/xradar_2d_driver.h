/******************************************************************************
 * Copyright 2023 AutoX Inc. All Rights Reserved.
 *****************************************************************************/

#pragma once

#include <array>
#include <cmath>
#include <deque>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace autox {
namespace drivers {
namespace xradar_2d {

struct Point {
  uint16_t range_code;
  int16_t doppler_code;
  int8_t azimuth_code;
  uint8_t log_power;
  double timestamp;
  float distance;
  int detect_type;
  //
  float x;
  float y;
  float z;
  float azimuth;
  float doppler;
};

struct PointCloud {
  double timestamp;
  size_t udp_packet_number;
  uint8_t major_version;
  uint8_t minor_version;
  std::vector<uint64_t> udp_sequences;
  std::vector<Point> points;
  size_t begin_index_in_pacp;
  size_t end_index_in_pacp;
  long int offset_in_pcap;
  std::vector<float> range_resolution;
  std::vector<float> azimuth_resolution;
  int dectect_number;
};
using PointCloudPtr = std::shared_ptr<PointCloud>;
struct Timestamp;

class XRadar2DDriver {
 public:
  using PointCloudCallbackType = std::function<void(PointCloudPtr, long long)>;
  XRadar2DDriver();
  ~XRadar2DDriver();
  void config(float min_distance, float max_distance, float hfov_start,
              float hfov_end) {
    min_distance_ = min_distance;
    max_distance_ = max_distance;
    hfov_start_ = hfov_start;
    hfov_end_ = hfov_end;
  }
  bool ParseRadarPacket(const uint8_t* data, size_t len);
  void SetPointCloudCallback(PointCloudCallbackType cb);
  void ResetDriver();
  void SetCurPacketIndex(size_t index) { cur_packet_index_ = index; }
  void SetCurPacketOffset(long int index) { cur_packet_offset_ = index; }

 private:
  void ClearPointcloud(PointCloudPtr point_cloud);
  bool ParsePacket(const uint8_t* data, size_t len);
  bool ParsePacket0_0(const uint8_t* data, size_t len);
  bool ParsePacket1_0(const uint8_t* data, size_t len);
  float CosLookupDegree(const float angle_in_deg);
  float SinLookupDegree(const float angle_in_deg);
  bool CheckValid(float azimuth, float distance);
  void InitBufferCloud(int size);
  double ComputeTimeStamp(Timestamp time);

 private:
  // sin/cos table
  std::vector<double> sin_lookup_table_;
  std::vector<double> cos_lookup_table_;
  // used to check if a pointcloud frame assembled
  float max_distance_{330};
  float min_distance_{0.1};
  float hfov_start_{0};
  float hfov_end_{360};
  //
  PointCloudCallbackType point_cloud_callback_;
  // parser buffer
  long long index_f_single_thread_{0};
  size_t cur_packet_index_{0};
  long int cur_packet_offset_{0};
  // for different range flag
  std::vector<PointCloudPtr> point_cloud_buffer_;
  bool init_buffer_{false};
  int udp_packet_number_{0};
  int dectect_number_{1};
};

}  // namespace xradar_2d
}  // namespace drivers
}  // namespace autox
