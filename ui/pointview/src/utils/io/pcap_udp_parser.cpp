
/******************************************************************************
 * Copyright 2023 AutoX. All Rights Reserved.
 *****************************************************************************/

#include "pcap_udp_parser.h"

#include <sys/stat.h>
#ifdef _WIN32
#include <Windows.h>
#include <Winsock2.h>
#else
#include <netinet/if_ether.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#endif

#include <string.h>

#include <iostream>

namespace autox {
namespace pointview {
using namespace std::chrono_literals;
PcapUdpParser::~PcapUdpParser() {
  if (isOpen()) {
    this->close();
  }
  if (outPcapIndexFile_.is_open()) {
    outPcapIndexFile_.close();
  }
  if (outOffsetFile_.is_open()) {
    outOffsetFile_.close();
  }
};

void PcapUdpParser::close() {
  if (pcap_handle_) {
    pcap_close(pcap_handle_);
    pcap_handle_ = nullptr;
  }
  pcap_file_ = "";
}

bool PcapUdpParser::isOpen() { return pcap_handle_ != nullptr; }

void PcapUdpParser::setFilter(std::string ip, uint16_t port) {
  filter_ = "";
  if (ip != "")  // using specific IP?
  {
    filter_ = filter_ + "src host " + ip + " && ";
  }
  if (port != 0) {
    filter_ = filter_ + "udp dst port " + std::to_string(port);
  }
}

std::string PcapUdpParser::getMdsum(const std::string& filename) {
  struct stat statbuf;
  int ret;
  ret = stat(filename.c_str(), &statbuf);
  if (ret != 0) return "";
  std::string pcap_file_name =
      filename.substr(filename.find_last_of('/') + 1, filename.size());
  return pcap_file_name + "_" + std::to_string(statbuf.st_size);
}

bool PcapUdpParser::open(const std::string& file) {
  if (isOpen()) {
    close();
  }
  writeFileEnd();
  pcap_offset_map_.clear();
  offset_index_map_.clear();
  pcap_index_map_.clear();
  pcap_file_ = file;
  pcap_file_real_ = getMdsum(pcap_file_);
  LOG(INFO) << "====pcap_file_real_ is " << pcap_file_real_;
  if (file_offset_maps_.find(pcap_file_real_) != file_offset_maps_.end() &&
      pcap_index_maps_.find(pcap_file_real_) != pcap_index_maps_.end() &&
      pcap_cnt_map_.find(pcap_file_real_) != pcap_cnt_map_.end()) {
    ifInited = true;
    cnt_ = pcap_cnt_map_[pcap_file_real_];
  } else {
    if (!pcap_mem_path.empty()) {
      LOG(INFO)
          << "====can't find file in offset map, now write memory file.....";
      ifInited = false;
      outOffsetFile_.open(pcap_mem_path + "/file_offset.csv", ios::app);
      outPcapIndexFile_.open(pcap_mem_path + "/pcap_index.csv", ios::app);
      outOffsetFile_ << pcap_file_real_ << ',';
      outPcapIndexFile_ << pcap_file_real_ << ',';
    } else {
      LOG(INFO) << "====can't find file in offset map, not save.....";
    }
  }

  pcap_handle_ = pcap_open_offline(pcap_file_.c_str(), err_buf_);
  if (pcap_handle_ == nullptr) {
    std::cerr << "pcap_open: " << err_buf_;
    return false;
  }
  pcap_hdr_pos_ = ftell(pcap_file(pcap_handle_));
  pcap_offset_map_[0] = pcap_hdr_pos_;
  pcap_index_map_[0] = 0;
  offset_index_map_[pcap_hdr_pos_] = 0;
  file_offset_maps_[pcap_file_real_][0] = pcap_hdr_pos_;
  file_offset_r_maps_[pcap_file_real_][pcap_hdr_pos_] = 0;
  pcap_index_maps_[pcap_file_real_][0] = 0;
  if (filter_ != "") {
    pcap_compile(pcap_handle_, &pcap_packet_filter_, filter_.c_str(), 1,
                 PCAP_NETMASK_UNKNOWN);
    pcap_setfilter(pcap_handle_, &pcap_packet_filter_);
  }
  eof_ = false;
  return true;
}

size_t PcapUdpParser::getPacketNumber() {
  if (pcap_handle_ == nullptr) {
    return 0;
  }
  fseek(pcap_file(pcap_handle_), pcap_hdr_pos_, SEEK_SET);
  size_t cnt = 0;
  pcap_pkthdr* header;
  const u_char* p;
  while (pcap_next_ex(pcap_handle_, &header, &p) >= 0) {
    cnt++;
  }
  fseek(pcap_file(pcap_handle_), pcap_hdr_pos_, SEEK_SET);
  return cnt;
}

bool PcapUdpParser::getPacket(uint8_t* data, size_t* len, double* timestamp,
                              int* port) {
  if (data == nullptr || len == nullptr) {
    LOG(INFO) << "[PcapUdpParser]getPacket Argument nullptr";
    eof_ = true;
    return false;
  }
  pcap_pkthdr* header;
  const u_char* p;
  int ret = pcap_next_ex(pcap_handle_, &header, &p);
  if (ret < 0) {
    if (ret == -2) {
      LOG(INFO) << "[PcapUdpParser] eof";
      eof_ = true;
    } else {
      LOG(INFO) << "[PcapUdpParser]pcap_next_ex error";
    }
    return false;
  }
  size_t ip_offset = sizeof(ether_header);
  auto ether = reinterpret_cast<const struct ether_header*>(p);
  if (ntohs(ether->ether_type) != ETHERTYPE_IP) {
    // for Linux cooked capture
    auto type = reinterpret_cast<const uint16_t*>(p + ip_offset);
    if (ntohs(*type) != ETHERTYPE_IP) {
      return false;
    }
    ip_offset = ip_offset + 2;
  }
  auto ip = reinterpret_cast<const struct ip*>(p + ip_offset);
  if (ip->ip_v != 4) {
    if(!ifInited) LOG(INFO) << "[PcapUdpParser] packet isn't not ipv4" ;
    return false;
  }
  if (ip->ip_p != IPPROTO_UDP) {
    if(!ifInited) LOG(INFO) << "[PcapUdpParser] packet isn't udp" ;
    return false;
  }
  size_t udp_offset = ip_offset + ip->ip_hl * 4;
  auto udp = reinterpret_cast<const udphdr*>(p + udp_offset);
  uint32_t udp_len = ntohs(udp->len) - 8;
  if (target_udp_len_ > 0 && (udp_len != target_udp_len_)) {
    // filter by length of udp packet.
    return false;
  }
  *len = udp_len;
  memcpy(data, p + udp_offset + 8, udp_len);
  if (timestamp) {
    *timestamp = header->ts.tv_sec + header->ts.tv_usec * 1e-6;
  }
  if (port) {
    *port = ntohs(udp->dest);
  }
  return true;
}

void PcapUdpParser::reset() {
  fseek(pcap_file(pcap_handle_), pcap_hdr_pos_, SEEK_SET);
  eof_ = false;
}

void PcapUdpParser::breakParseAllPackets(bool it) { breakloop_ = it; }

bool PcapUdpParser::parseAllPackets(std::function<bool(double)> progress_cb) {
  if (!ifInited) {
    cnt_ = getPacketNumber();
    if (outOffsetFile_.is_open()) {
      outOffsetFile_ << cnt_ << ',';
    };
    LOG(INFO) << "pcap file packet num is " << cnt_;
    pcap_cnt_map_[pcap_file_real_] = cnt_;
  }
  // parse pcap
  uint8_t data[65535];
  size_t len = 0;
  size_t cnt = 0;
  bool reset_jump = false;
  while (cnt < cnt_) {
    if (breakloop_) {
      break;
    }
    auto play_state = device_context_->getPlayerState();
    if (!play_state.need_playback_jumb) reset_jump = false;
    if (!play_state.is_playback) return false;
    if (play_state.is_playing || !ifInited || play_state.need_playback_jumb) {
      if (play_state.need_playback_jumb && !reset_jump) {
        auto jump_frame_idx = play_state.jump_frame_idx;
        cnt = findCntInMap(jump_frame_idx);
        if (cnt < 0) return false;
        if (reset_driver_cb_) reset_driver_cb_();
        reset_jump = true;
      }
      if (getPacket(data, &len)) {
        if (packet_index_cb_) packet_index_cb_(cnt + 1);
        if (packet_offset_cb_) {
          long int offset_cur = ftell(pcap_file(pcap_handle_));
          packet_offset_cb_(offset_cur);
        }
        if (udp_cb_)
          udp_cb_(data, len);  // driver_->parseLidarPacket(data, len);
      } else {
        if (eof()) {
          LOG(INFO)
              << "============warning!!! reading pcap file loop quit early";
          break;
        }
      }
      cnt++;
      // update progress
      if (progress_cb && !ifInited) {
        double v = 1.0 * (cnt + 1) / cnt_;
        if (!progress_cb(v)) {
          // cancel
          return false;
        }
      }
    } else {
      std::this_thread::sleep_for(50ms);
      continue;
    }
    if (cnt == cnt_) {
      reset();
      ifInited = true;
    }
  }
  reset();
  breakloop_ = false;
  return true;
}

int PcapUdpParser::findCntInMap(int jump_frame_idx) {
  if (file_offset_maps_[pcap_file_real_].find(jump_frame_idx) !=
      file_offset_maps_[pcap_file_real_].end()) {
    LOG(INFO) << "====find in file_offset_maps_, jump_frame_idx is "
              << jump_frame_idx << ", offset is "
              << file_offset_maps_[pcap_file_real_][jump_frame_idx];

    fseek(pcap_file(pcap_handle_),
          file_offset_maps_[pcap_file_real_][jump_frame_idx], SEEK_SET);

    if (pcap_index_maps_[pcap_file_real_].find(jump_frame_idx) !=
        pcap_index_maps_[pcap_file_real_].end()) {
      return pcap_index_maps_[pcap_file_real_][jump_frame_idx];
    } else {
      if (pcap_index_maps_[pcap_file_real_].size() != 1) {
        LOG(INFO) << "error!!can't find jump_frame_idx in pcap_index_map_, "
                     "jump_frame_idx is "
                  << jump_frame_idx;
      }
      return -2;
    }
  }
  LOG(INFO) << "error!!can't find jump_frame_idx in pcap_offset_map_, "
               "jump_frame_idx is "
            << jump_frame_idx;
  return -1;
}

bool PcapUdpParser::readPcapMemory(std::string folder) {
  pcap_mem_path = folder;
  return readPcapIndex(folder + "/pcap_index.csv") &&
         readFileOffset(folder + "/file_offset.csv");
}

bool PcapUdpParser::readPcapIndex(std::string filename) {
  std::ifstream csv_data(filename, std::ios::in);
  std::string line;
  if (!csv_data.is_open()) {
    LOG(ERROR) << "Error: opening pcap_index fail: " << filename;
    return false;
  }
  std::istringstream sin;
  std::string word;
  while (std::getline(csv_data, line)) {
    sin.clear();
    sin.str(line);
    bool read_file_name = true;
    std::string file_name;
    int index = 0;
    while (std::getline(sin, word, ',')) {
      if (read_file_name) {
        file_name = word;
        read_file_name = false;
        continue;
      }
      if (!file_name.empty())
        pcap_index_maps_[file_name][index++] = std::stoi(word);
      ;
    }
  }
  csv_data.close();
  return true;
}

bool PcapUdpParser::readFileOffset(std::string filename) {
  std::ifstream csv_data(filename, std::ios::in);
  std::string line;
  if (!csv_data.is_open()) {
    LOG(INFO) << "Error: opening file_offset fail: " << filename;
    return false;
  }
  std::istringstream sin;
  std::string word;
  while (std::getline(csv_data, line)) {
    sin.clear();
    sin.str(line);
    bool read_file_name = true;
    bool read_pkg_cnt = true;
    std::string file_name;
    int index = 0;
    while (std::getline(sin, word, ',')) {
      if (read_file_name) {
        file_name = word;
        LOG(INFO) << "file name is " << file_name;
        read_file_name = false;
        continue;
      }
      if (read_pkg_cnt) {
        if (!file_name.empty()) pcap_cnt_map_[file_name] = std::stoi(word);
        LOG(INFO) << "pkg cnt is " << std::stoi(word);
        read_pkg_cnt = false;
        continue;
      }
      if (!file_name.empty()) {
        long offset;
        std::stringstream ss;
        ss << word;
        ss >> offset;
        file_offset_maps_[file_name][index] = offset;
        file_offset_r_maps_[file_name][offset] = index++;
      }
    }
  }
  csv_data.close();
  return true;
}

}  // namespace pointview
}  // namespace autox
