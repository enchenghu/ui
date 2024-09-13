/******************************************************************************
 * Copyright 2023 AutoX Inc. All Rights Reserved.
 *****************************************************************************/

#pragma once

#include <array>
#include <cmath>
#include <condition_variable>
#include <deque>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include <pcap/pcap.h>

constexpr int kLaserNum = 256;

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
  uint8_t version;
  std::vector<uint64_t> udp_sequences;
  std::vector<Point> points;
  size_t begin_index_in_pacp;
  size_t end_index_in_pacp;
  long int offset_in_pcap;
  float range_resolution;
  float azimuth_resolution;
};

class XRadar2DDriver {
 public:
  struct Packet {
    // udp data
    uint8_t data[1500];
    size_t len;
    // parsed data
    bool parsed = false;
    bool valid = false;
    std::shared_ptr<PointCloud> point_cloud;
    size_t index_in_pcap;
    long int offset_in_pcap;
  };
  using PointCloudCallbackType =
      std::function<void(std::shared_ptr<PointCloud>, long long)>;
  XRadar2DDriver();
  ~XRadar2DDriver();
  void config(float min_distance, float max_distance, float hfov_start,
              float hfov_end) {
    min_distance_ = min_distance;
    max_distance_ = max_distance;
    hfov_start_ = hfov_start;
    hfov_end_ = hfov_end;
  }
  bool parseRadarPacket(const uint8_t* data, size_t len);
  void setPointCloudCallback(PointCloudCallbackType cb);
  void resetDriver();
  void setCurPacketIndex(size_t index) { cur_packet_index_ = index; }
  void setCurPacketOffset(long int index) { cur_packet_offset_ = index; }

 private:
  void clearPointcloud(std::shared_ptr<PointCloud> point_cloud);
  void clearPacket(Packet& packet);
  bool parsePacket(const uint8_t* data, size_t len,
                   std::shared_ptr<PointCloud> point_cloud);
  bool parsePacket0_0(const uint8_t* data, size_t len,
                      std::shared_ptr<PointCloud> point_cloud);
  float CosLookupDegree(const float angle_in_deg);
  float SinLookupDegree(const float angle_in_deg);
  bool checkValid(float azimuth, float distance);
  // for multipe threads
  void parserLoop();
  void assemblerLoop();

 private:
  // sin/cos table
  std::vector<double> sin_lookup_table_;
  std::vector<double> cos_lookup_table_;
  // used to check if a pointcloud frame assembled
  float start_angle_{180};
  int azimuth_sign_{1};  // 1 means clockwise, -1 means counterclockwise
  float accumulate_azimuth_{0};
  float max_distance_{330};
  float min_distance_{0.1};
  float hfov_start_{0};
  float hfov_end_{360};
  //
  std::shared_ptr<PointCloud> point_cloud_;
  PointCloudCallbackType point_cloud_callback_;
  // point cloud buffer
  std::vector<std::shared_ptr<PointCloud>> point_cloud_buffer_;
  size_t max_point_cloud_buffer_size_{10};
  size_t point_cloud_buffer_index_{0};
  // packet buffer
  std::vector<std::shared_ptr<Packet>> packet_buffer_;  // one frame data
  size_t max_packet_buffer_size_{20000};
  size_t packet_buffer_index_{0};
  size_t assembling_index_{0};
  size_t used_packet_count_{0};
  std::mutex packet_buffer_mutex_;
  std::condition_variable packet_buffer_not_full_;
  // parser buffer
  std::deque<std::shared_ptr<Packet>> parser_buffer_;
  std::mutex parser_buffer_mutex_;
  long long index_f_single_thread_{0};
  size_t cur_packet_index_{0};
  long int cur_packet_offset_{0};
  // for different range flag
  std::vector<bool> is_last_frame_;
};

}  // namespace xradar_2d
}  // namespace drivers
}  // namespace autox
