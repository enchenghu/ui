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

namespace autox {
namespace drivers {
namespace xlidar {

constexpr int kLaserNum = 256;
constexpr int kSectionNum = 2;
constexpr int kSinCosTableResolution = 1000;
constexpr unsigned int kRotationMaxUnits = 360 * kSinCosTableResolution + 1;
constexpr double kDegreeToRadian = M_PI / 180.;

struct Point {
  uint8_t laser_id;
  uint8_t return_id;
  uint8_t intensity;
  uint8_t elongation;
  uint16_t azimuth_code;
  double timestamp;
  float encoder;
  float distance;
  int section;
  //
  float x;
  float y;
  float z;
  float azimuth;
  float elevation;
};

struct PointCloud {
  double timestamp;
  size_t udp_packet_number;
  uint8_t version;
  uint8_t max_return_num;
  std::vector<uint64_t> udp_sequences[2];
  std::vector<Point> points;
  size_t begin_index_in_pacp;
  size_t end_index_in_pacp;
  long int offset_in_pcap;
};

class XLidarDriver {
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
  XLidarDriver();
  ~XLidarDriver();
  void config(float min_distance, float max_distance, float hfov_start,
              float hfov_end) {
    min_distance_ = min_distance;
    max_distance_ = max_distance;
    hfov_start_ = hfov_start;
    hfov_end_ = hfov_end;
  }
  bool SetMultiThreadMode(int thread_mum);
  bool ParseLidarPacket(const uint8_t* data, size_t len);
  void SetPointCloudCallback(PointCloudCallbackType cb);
  void ResetDriver();
  void SetCurPacketIndex(size_t index) { cur_packet_index_ = index; }
  void SetCurPacketOffset(long int index) { cur_packet_offset_ = index; }

  bool SetIntrinsics(const std::vector<double>& elevation_offset,
                     const std::vector<double>& azimuth_offset,
                     const std::vector<double>& distance_offset,
                     const std::vector<double>& origin_x_offset,
                     const std::vector<double>& origin_y_offset,
                     const std::vector<double>& origin_z_offset);
  bool GetIntrinsics(std::vector<double>& elevation_offset,
                     std::vector<double>& azimuth_offset,
                     std::vector<double>& distance_offset,
                     std::vector<double>& origin_x_offset,
                     std::vector<double>& origin_y_offset,
                     std::vector<double>& origin_z_offset);

 private:
  void ClearPointcloud(std::shared_ptr<PointCloud> point_cloud);
  void ClearPacket(Packet& packet);
  bool ParsePacket(const uint8_t* data, size_t len,
                   std::shared_ptr<PointCloud> point_cloud);
  bool ParsePacket4_1(const uint8_t* data, size_t len,
                      std::shared_ptr<PointCloud> point_cloud);
  bool ParsePacket4_2(const uint8_t* data, size_t len,
                      std::shared_ptr<PointCloud> point_cloud);
  float CosLookupDegree(const float angle_in_deg);
  float SinLookupDegree(const float angle_in_deg);
  bool CheckAngle(float cur_azimuth, int section);
  bool CheckValid(float azimuth, float distance);
  // for multipe threads
  void ParserLoop();
  void AssemblerLoop();

 private:
  // lidar intrinsics
  double elevation_offset_[kLaserNum];
  double azimuth_offset_[kLaserNum];
  double distance_offset_[kLaserNum];
  double origin_x_offset_[kLaserNum];
  double origin_y_offset_[kLaserNum];
  double origin_z_offset_[kLaserNum];
  float elevation_offset_cos_[kLaserNum];
  float elevation_offset_sin_[kLaserNum];
  float azimuth_offset_cos_[kLaserNum];
  float azimuth_offset_sin_[kLaserNum];
  double azimuth_unit_ = 0.012;
  // time
  double laser_time_offset_[kLaserNum];
  // sin/cos table
  double sin_lookup_table_[kRotationMaxUnits];
  double cos_lookup_table_[kRotationMaxUnits];
  // used to check if a pointcloud frame assembled
  float start_angle_{180};
  float last_azimuth_[kSectionNum];
  int azimuth_sign_{1};  // 1 means clockwise, -1 means counterclockwise
  float accumulate_azimuth_{0};
  float max_distance_{330};
  float min_distance_{0.1};
  float hfov_start_{0};
  float hfov_end_{360};
  //
  std::shared_ptr<PointCloud> point_cloud_;
  PointCloudCallbackType point_cloud_callback_;
  bool first_check_flag_[kSectionNum];
  // multipe threads for packet parse
  bool use_multiple_threads_{false};
  int parser_thread_number_{0};
  std::unique_ptr<std::thread> assembler_thread_;
  std::vector<std::unique_ptr<std::thread>> parser_threads_;
  bool exit_{false};
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
};

}  // namespace xlidar
}  // namespace drivers
}  // namespace autox
