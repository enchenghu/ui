/******************************************************************************
 * Copyright 2021 AutoX. All Rights Reserved.
 *****************************************************************************/
#include "driver/arbe_driver.h"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <utility>

namespace autox {
namespace drivers {
namespace arbe {

ArbeDriver::ArbeDriver() {
  pointcloud_ = std::make_shared<RawPointCloud>();
  clear();
  point_cloud_parse_thread_ = std::unique_ptr<std::thread>(
      new std::thread([&]() { ArbeDriver::ParsePointCloudPacket(); }));
}

ArbeDriver::~ArbeDriver() {
  if (radar_ctrl_sock_ != -1) {
    close(radar_ctrl_sock_);
  }
  exit_ = true;
  // should stop connect first
  if (point_cloud_parse_thread_ && point_cloud_parse_thread_->joinable()) {
    point_cloud_parse_thread_->join();
  }
}

void ArbeDriver::setPointCloudCallback(PointCloudCallbackType cb) {
  point_cloud_callback_ = cb;
}

bool ArbeDriver::connectRadarCtrl(const std::string& ip, int port) {
  struct sockaddr_in data_serv_addr;
  // create TCP socket that will be used to receive data from the Radar
  if ((radar_ctrl_sock_ = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    std::cerr << "ERROR: Could not create TCP socket!" << std::endl;
    return false;
  }

  struct timeval timeout;
  timeout.tv_sec = 0;
  timeout.tv_usec = 1e5;
  int one = 1;
  setsockopt(radar_ctrl_sock_, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
  setsockopt(radar_ctrl_sock_, SOL_SOCKET, SO_SNDTIMEO, &timeout,
             sizeof(timeout));
  memset(&data_serv_addr, 0, sizeof(data_serv_addr));
  data_serv_addr.sin_family = AF_INET;
  data_serv_addr.sin_addr.s_addr = inet_addr(ip.c_str());
  data_serv_addr.sin_port = htons(port);
  if (connect(radar_ctrl_sock_, (const struct sockaddr*)&data_serv_addr,
              sizeof(data_serv_addr)) < 0) {
    std::cerr << "Failed to connect to the control socket" << std::endl;
    return false;
  }
  /* Set the socket to non blocking mode */
  fcntl(radar_ctrl_sock_, F_SETFL, O_NONBLOCK);
  std::cout << "radar connected" << std::endl;
  ConfigMailbox();
  return true;
}

void ArbeDriver::startRadar() {
  if (!mailbox_) {
    return;
  }
  std::cout << "startRadar" << std::endl;
  // SetSeqType();
  // SetTime();
  // EnableNoiseControl();
  TStartTxInfo startInfo;
  RAF_API_RdrCtrl_StartTx(&startInfo, mailbox_.get());
}

void ArbeDriver::stopRadar() {
  if (!mailbox_) {
    return;
  }
  std::cout << "stopRadar" << std::endl;
  TStopInfo stopInfo;
  RAF_API_RdrCtrl_StopTx(&stopInfo, mailbox_.get());
}

void ArbeDriver::setThreshold(int power, int azimuth, int elevation,
                              int noise_level) {
  if (!mailbox_) {
    return;
  }
  std::cout << "setThreshold" << std::endl;
  TSetThresholdsInfo radarThresholds;
  radarThresholds.opcode = SetStaticAndDynamicThresholds;
  radarThresholds.unParam1 =
      static_cast<uint32_t>(((power - kBias + noise_level) * 16) / 3);
  radarThresholds.unParam2 =
      static_cast<uint32_t>(((power + noise_level) * 16) / 3);
  radarThresholds.unParam3 = static_cast<uint32_t>((azimuth * 16) / 3);
  radarThresholds.unParam4 = static_cast<uint32_t>((elevation * 16) / 3);
  RAF_API_RdrCtrl_SetThresholds(&radarThresholds, mailbox_.get());
}

void ArbeDriver::ConfigMailbox() {
  mailbox_ = std::make_unique<TArbeApiMailBox>();
  mailbox_->radar_socket = radar_ctrl_sock_;
  mailbox_->unMessageNumber = 0;
}

void ArbeDriver::StartTransmit() {
  TStartTxInfo startInfo;
  RAF_API_RdrCtrl_StartTx(&startInfo, mailbox_.get());
}

void ArbeDriver::SetSeqType() {
  if (!mailbox_) {
    return;
  }
  TSelectActiveSeqInfo sequenceInfo;
  sequenceInfo.eSequenceType = ToSequenceType(mode_);
  RAF_API_RdrCtrl_SetActiveSeq(&sequenceInfo, mailbox_.get());
}

void ArbeDriver::SetTime() {
  // DO: use ptp time
  auto current_time = std::chrono::system_clock::now();
  auto duration_in_seconds =
      std::chrono::duration<double>(current_time.time_since_epoch());
  double time = duration_in_seconds.count();
  uint64_t mill_sec = static_cast<uint64_t>(time * 1000);
  uint8_t* time_ptr = reinterpret_cast<uint8_t*>(&mill_sec);
  uint32_t upper, lower;
  memcpy(&lower, time_ptr, 4);
  memcpy(&upper, time_ptr + 4, 4);
  TSetTimeInfo time_info;
  time_info.unInitateTimeMsb = upper;
  time_info.unInitateTimeLsb = lower;
  RAF_API_SysCfg_SetTime(&time_info, mailbox_.get());
}

// enable noise threshold control
void ArbeDriver::EnableNoiseControl() {
  TAtcInfo atcInfo;
  atcInfo.unEnableAtc = true;
  atcInfo.unsensitivity = 0;
  RAF_API_EXT_AtcMode(&atcInfo, mailbox_.get());
}

bool ArbeDriver::processUdpPacket(const uint8_t* data, size_t len) {
  auto mandatory_header = reinterpret_cast<const MandatoryHeader*>(data);
  // accumulate data
  if (mandatory_header->usPrefix == PACKET_PREFIX) {
    switch (mandatory_header->usType) {
      case PointCloud_Output_V1_1:
      case PointCloud_Output_xRadar_V1_1: {
        std::lock_guard<std::mutex> lock(radar_point_cloud_mutex_);
        std::vector<uint8_t> tmp_data(data, data + len);
        point_cloud_data_.push_back(std::move(tmp_data));
      } break;
      // Arbe do not ouput slam data for now
      case Slam_Output_V1_1:
        break;
      case RangeThreshold_output:
        std::cout << "range threshold" << std::endl;
        break;
      default:
        std::cout << "other type data " << mandatory_header->usType
                  << std::endl;
        break;
    }
  }
}

void ArbeDriver::ParsePointCloudPacket() {
  static constexpr size_t header_size = sizeof(TPointCloud_V1_1);
  bool is_meansurement_time_setted = false;
  while (!exit_) {
    std::vector<uint8_t> point_cloud_packet;
    {
      std::lock_guard<std::mutex> lock(radar_point_cloud_mutex_);
      if (!point_cloud_data_.empty()) {
        point_cloud_packet = point_cloud_data_.front();
        point_cloud_data_.pop_front();
      }
    }
    if (point_cloud_packet.empty()) {
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
      continue;
    }
    if (point_cloud_packet.size() < header_size) {
      std::cerr << "frame data is too short, header is incomplete" << std::endl;
      continue;
    }
    uint8_t* point_cloud_packet_pointer = point_cloud_packet.data();
    MandatoryHeader* mandatory_header =
        reinterpret_cast<MandatoryHeader*>(point_cloud_packet_pointer);
    if (mandatory_header->usType == PointCloud_Output_V1_1 ||
        mandatory_header->usType == PointCloud_Output_xRadar_V1_1) {
      TPointCloud_V1_1* header1_1 =
          reinterpret_cast<TPointCloud_V1_1*>(point_cloud_packet_pointer);
      uint64_t timestamp_ns;
      if (mandatory_header->usType == PointCloud_Output_xRadar_V1_1) {
        uint64_t timestamp_ms;
        timestamp_ms = static_cast<uint64_t>(header1_1->unTimeMsb);
        timestamp_ms = timestamp_ms << 32;
        timestamp_ms |= static_cast<uint64_t>(header1_1->unTimeLsb);
        timestamp_ms = timestamp_ms >> 16;
        timestamp_ns = timestamp_ms * 1e6;
      } else {
        timestamp_ns = static_cast<uint64_t>(header1_1->unTimeMsb) * 1e9 +
                       static_cast<uint64_t>(header1_1->unTimeLsb);
      }

      if (!is_meansurement_time_setted) {
        pointcloud_->timestamp = static_cast<double>(timestamp_ns / 1e9);
        is_meansurement_time_setted = true;
      }
      uint8_t targetsizeByte =
          RAF_COM_CALC_GetBytesLengthOfTraget(header1_1->ucTrgtFmtFrmType);
      size_t detections_number =
          (point_cloud_packet.size() - header_size) / targetsizeByte;
      float range_res_db = RAF_COM_CALC_ClacRangeResInDb(&header1_1->tMetadata);
      for (size_t i = 0; i < detections_number; i++) {
        Target48bit* targets = reinterpret_cast<Target48bit*>(
            point_cloud_packet_pointer + header_size + i * targetsizeByte);
        TaregtCartesian taregtCartesian;
        RAF_COM_Bin2cart(targets->Fields.range, targets->Fields.azimuth,
                         targets->Fields.elevation, &header1_1->tMetadata,
                         &taregtCartesian);
        // coordination transform,
        // arbe corrdinate: x left, y front, z up
        // abe coordinate: azimuth 0(front), negitive(right), positive(left)
        Point p;
        p.timestamp = static_cast<double>(timestamp_ns / 1e9);
        p.x = taregtCartesian.y;
        p.y = -taregtCartesian.x;
        p.z = taregtCartesian.z;
        p.range = RAF_COM_CALC_CalcRange(targets->Fields.range,
                                         &header1_1->tMetadata);
        p.azimuth = -RAF_COM_CALC_CalcAzimuth(targets->Fields.azimuth,
                                              targets->Fields.elevation,
                                              &header1_1->tMetadata);
        p.elevation = RAF_COM_CALC_CalcElevation(targets->Fields.elevation,
                                                 &header1_1->tMetadata);
        p.doppler = RAF_COM_CALC_CalcDoppler(targets->Fields.doppler,
                                             &header1_1->tMetadata);
        // check
        if (std::isnan(p.azimuth)) {
          continue;
        }
        /* In 4d (zoom) the number of range bins is multiplied by 4 */
        // 40*log10(4)
        p.power = RAF_COM_CALC_CalcPowerEqulizer(
            RAF_COM_CALC_CalcPower(targets->Fields.power),
            (targets->Fields.range / 4), (range_res_db + 24.08));
        pointcloud_->points.push_back(p);
      }
      pointcloud_->udp_packet_number++;
      if (header1_1->ucLastPacket == 1) {
        // last packet, ready to publish;
        point_cloud_callback_(pointcloud_);
        pointcloud_->points.clear();
        pointcloud_->udp_packet_number = 0;
        is_meansurement_time_setted = false;
      }
    }
  }
}

ESeuqenceType ArbeDriver::ToSequenceType(RadarRangingMode mode) {
  switch (mode) {
    case LONG_4D:
      return FineLongSeq;
    case LONG_3D:
      return CoarseLongSeq;
    case MID_4D:
      return FineMidSeq;
    case MID_3D:
      return CoarseMidSeq;
    case SHORT_4D:
      return FineShortSeq;
    case SHORT_3D:
      return CoarseShortSeq;
    default:
      return IdleSeq;
  }
}

void ArbeDriver::clear() { pointcloud_->points.clear(); }

bool ArbeDriver::isIdle() {
  std::lock_guard<std::mutex> lock(radar_point_cloud_mutex_);
  return point_cloud_data_.empty();
}

}  // namespace arbe
}  // namespace drivers
}  // namespace autox
