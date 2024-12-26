

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
namespace autox {
namespace drivers {
namespace hesai {

struct PandarQt128Packet;
struct PandarQt128Block;
struct PandarPacket;


struct Point {
  uint8_t laser_id;
  uint8_t return_id;
  uint64_t timestamp;
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

class DriverQt128 {
 public:
  using PointCloudCallbackType =
      std::function<void(std::shared_ptr<PointCloud>&, long long)>;
  DriverQt128();
  ~DriverQt128();
  void setPointCloudCallback(PointCloudCallbackType cb) {
    point_cloud_callback_ = cb;
  };
  void resetDriver();
  void setCurPacketIndex(size_t index){cur_packet_index_ = index;}
  bool ParseLidarPacket(const uint8_t* data, size_t len);
  void resetFrameIndex() {index_f_single_thread_ = 0;}
  bool CheckAngle(const int cur_azimuth);
  float CosLookupDegree(const float angle_in_deg);
  float SinLookupDegree(const float angle_in_deg);
  bool setIntrinsics(const std::vector<double>& elevation,
                     const std::vector<double>& azimuth,
                     const std::vector<double>& distance);
  bool getIntrinsics(std::vector<double>& elevation,
                     std::vector<double>& azimuth,
                     std::vector<double>& distance);
                  
 private:
  bool IsValidQt128Packet(const uint8_t* data);
  void ParsePacket(const PandarQt128Packet* pkt);
  void CalcPointXYZIT(const PandarQt128Packet* pkt, const unsigned int blockid,
                      const unsigned int laser_num,
                      const uint64_t pkt_timestamp, const double distance_unit,
                      const double azimuth_in_deg, const uint8_t mode_flag);
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
  PointCloudCallbackType point_cloud_callback_;
  // point cloud buffer
  std::shared_ptr<PointCloud> point_cloud_;
  std::vector<std::shared_ptr<PointCloud>> point_cloud_buffer_;
  size_t max_point_cloud_buffer_size_{10};
  size_t point_cloud_buffer_index_{0};
  long long index_f_single_thread_{0};
  size_t cur_packet_index_{0}; 
  std::vector<double> elev_angle_map_;
  std::vector<double> horizatal_azimuth_offset_map_;
  std::vector<double> distance_offset_map_;          
};

}  // namespace autox::drivers::hesai
}
}
