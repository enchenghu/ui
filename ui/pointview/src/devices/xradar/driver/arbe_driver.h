/******************************************************************************
 * Copyright 2021 AutoX. All Rights Reserved.
 *****************************************************************************/
#pragma once

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <sys/socket.h>
#include <unistd.h>

#include <atomic>
#include <cstdio>
#include <deque>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include "driver/Raf_Api.h"
#include "driver/Utils.h"

namespace autox {
namespace drivers {
namespace arbe {

struct Point {
  double timestamp;
  float azimuth;
  float elevation;
  float range;
  float x;
  float y;
  float z;
  float doppler;
  float power;
};

struct RawPointCloud {
  double timestamp;
  size_t udp_packet_number;
  uint8_t radar_type;
  uint8_t is_4d;
  std::vector<Point> points;
};

enum RadarDataStatus {
  kNoDtataReady = 0,
  kRadarPointsReady,
  kRadarTracksReady,
  kSize
};

enum RadarRangingMode {
  UNKNONW_MODE = 0,
  SHORT_3D = 1,
  MID_3D = 2,
  LONG_3D = 3,
  SHORT_4D = 4,
  MID_4D = 5,
  LONG_4D = 6
};

class ArbeDriver {
 public:
  using PointCloudCallbackType =
      std::function<void(std::shared_ptr<RawPointCloud>&)>;
  ArbeDriver();
  ~ArbeDriver();
  bool connectRadarCtrl(const std::string& ip, int port);
  void startRadar();
  void stopRadar();
  void setThreshold(int power, int azimuth, int elevation,
                    int noise_level = 60);
  void setPointCloudCallback(PointCloudCallbackType cb);
  bool processUdpPacket(const uint8_t* data, size_t len);
  void clear();
  bool isIdle();

 private:
  void AccumulateData();
  void ConfigMailbox();

  void EnableNoiseControl();
  void ParsePointCloudPacket();
  void SetSeqType();
  void SetTime();
  void StartTransmit();
  ESeuqenceType ToSequenceType(RadarRangingMode mode);
  // both radar and hostmachine has the same data port
  std::string frame_id_;
  uint16_t data_port_;
  uint16_t ctrl_port_;
  std::string radar_ip_;
  bool udp_replay_;
  int radar_ctrl_sock_{-1};
  int radar_data_sock_{-1};
  RadarRangingMode mode_;
  std::unique_ptr<TArbeApiMailBox> mailbox_;

  // accumulate one frame data(several packets)
  std::deque<std::vector<uint8_t>> point_cloud_data_;
  std::mutex radar_point_cloud_mutex_;
  std::unique_ptr<std::thread> point_cloud_parse_thread_;
  bool exit_{false};
  //
  std::shared_ptr<RawPointCloud> pointcloud_;
  PointCloudCallbackType point_cloud_callback_;

  int power_threshold_ = 29;
  int azimuth_threshold_ = 10;
  int elevation_threshold_ = 10;
  int noise_level_ = 60;

  static constexpr int kBias = 4;
  // one point 6 byte, use 1400bytes for a packet
  static constexpr int kMaxBufferLength = 5000;
  static constexpr int kPollTimeout = 1;
};

}  // namespace arbe
}  // namespace drivers
}  // namespace autox
