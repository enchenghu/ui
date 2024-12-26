/******************************************************************************
 * Copyright 2023 AutoX. All Rights Reserved.
 *****************************************************************************/

#ifndef PLAYBACK_BUFFER_H
#define PLAYBACK_BUFFER_H

#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <thread>
#include <tuple>
#include <vector>

#include "utils/common/device_context.h"
#include "utils/common/safe_queue.h"
namespace autox {
namespace pointview {

class PlaybackBufferBase {
 public:
  virtual ~PlaybackBufferBase(){};
  virtual void setConstantFps(double constant_fps) = 0;
  virtual void enableConstantFps(bool use_constant_fps) = 0;
  virtual void enableLoop(bool loop) = 0;
  virtual void clear() = 0;
  virtual bool sync() = 0;
  virtual bool jump(size_t idx) = 0;
  virtual int getTotalFrames() = 0;
  virtual int getRTFrameNum() = 0;
  virtual void addRTFrameNum(int n = 1) = 0;
  virtual void consumeNextBuffer(bool delay = true) = 0;
  virtual void consumeNextRTBuffer(bool delay = true, bool jump = false) = 0;
  virtual void initSafeQueue() = 0;
};

template <typename T>
class PlaybackBuffer : public PlaybackBufferBase {
  using FrameCallbackType =
      std::function<void(std::shared_ptr<T>, int idx, double t)>;
  using FrameRTType = struct {
    size_t frame_index;
    double time_stamp;
    std::shared_ptr<T> framePtr;
  };

#define max_queue_buffer_num 4

 public:
  PlaybackBuffer(std::shared_ptr<DeviceContext> device_context)
      : device_context_(device_context) {
    initSafeQueue();
  };
  ~PlaybackBuffer(){};
  void setFrameCallback(FrameCallbackType cb) { frame_cb_ = cb; };
  void setProcCallback(std::function<bool(double)> cb) { proc_cb_ = cb; };
  void setConstantFps(double constant_fps) override {
    constant_fps_ = constant_fps;
    sync();
  }
  void enableConstantFps(bool use_constant_fps) override {
    use_constant_fps_ = use_constant_fps;
  }

  double getRateJump() const { return rate_jump_; }
  // add and clear buffer
  void addFrame(double t, std::shared_ptr<T> data) {
    buffer_.push_back({t, data});
  }

  void addFrame(int frame_index, double t, std::shared_ptr<T> data) {
    // buffer_.push_back({t, data});
    FrameRTType* ptr;
    if (rt_queue_free.get(ptr)) {
      rt_queue_done.get(ptr);
      rt_queue_free.put(ptr);
      rt_queue_free.get(ptr);
      printf(
          "addFrame: rt_queue_free get timeout, will drop an oldest frame\n");
      sync();
    }
    ptr->frame_index = frame_index;
    ptr->time_stamp = t;
    ptr->framePtr = data;
    rt_queue_done.put(ptr);
  }

  void addRTFrameNum(int n = 1) { RT_total_frames += n; }

  int getRTFrameNum() override { return RT_total_frames; };

  void clear() override {
    buffer_.clear();
    init_ = false;
    rt_buffer_.clear();
    RT_total_frames = 0;
    cur_buffer_idx_ = 0;
    initSafeQueue();
  }
  int getTotalFrames() override { return buffer_.size(); };
  void enableLoop(bool loop) override { is_loop_ = loop; }
  // jump and sync
  bool jump(size_t idx) override {
    if (idx >= RT_total_frames) {
      return false;
    }
    cur_buffer_idx_ = idx;
    sync();
    return true;
  }
  bool sync() override {
    double dt_sec = getBufferRelativeTime(cur_buffer_idx_);
    auto dt = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::duration<double>(dt_sec));
    real_start_ = std::chrono::steady_clock::now() - dt;
    return true;
  }
  // consume buffer
  void consumeNextRTBuffer(bool delay = true, bool jumpIf = false) override {
    using namespace std::chrono_literals;
    if (frame_cb_) {
      FrameRTType* ptr = nullptr;
      if (!rt_queue_done.get(ptr)) {
        if (delay) {
          if (!jumpIf) cur_buffer_idx_ = ptr->frame_index;
          if (cur_buffer_idx_ == 0) sync();
          auto cur = std::chrono::steady_clock::now();
          double real_dt =
              std::chrono::duration<double>(cur - real_start_).count();
          double buffer_dt = getBufferRelativeTime(cur_buffer_idx_);
          double sleep_xxxx = buffer_dt - real_dt;
          if (sleep_xxxx > 0.0) {
            if (sleep_xxxx > 1.0) {
              sleep_xxxx = std::min(sleep_xxxx, 1.0);
              sync();
            }
            std::this_thread::sleep_for(sleep_xxxx * 1s);
          } else {
            if (use_constant_fps_ && constant_fps_ < 1000) sync();
          }
        }
        if (jumpIf) {
          while (1) {
            if (ptr->frame_index != cur_buffer_idx_) {
              if (ptr->frame_index > cur_buffer_idx_)
                rate_jump_ = 0.0;
              else
                rate_jump_ = (double)ptr->frame_index / (double)cur_buffer_idx_;
              device_context_->updateJumpRate(rate_jump_);
              rt_queue_free.put(ptr);
              if (!rt_queue_done.get(ptr)) continue;
            } else {
              device_context_->updateJumpRate(1.0);
              break;
            }
          }
        }
        frame_cb_(ptr->framePtr, ptr->frame_index, ptr->time_stamp);
        rt_queue_free.put(ptr);
      } else {
        printf("rt_queue_done get timeout\n");
      }
    } else {
      std::this_thread::sleep_for(100ms);
      LOG(INFO) << " player_thread_ noing is doing" ;
    }
  };

  void consumeNextBuffer(bool delay = true) override {
    if (!init_) {
      jump(0);
      init_ = true;
    }
    using namespace std::chrono_literals;
    if (buffer_.size() == 0) {
      LOG(INFO) << "invalid palyback buffer when call consumeNextBuffer()"
                ;
      std::this_thread::sleep_for(100ms);
      return;
    }
    if (cur_buffer_idx_ < buffer_.size()) {
      if (delay) {
        auto cur = std::chrono::steady_clock::now();
        double real_dt =
            std::chrono::duration<double>(cur - real_start_).count();
        double buffer_dt = getBufferRelativeTime(cur_buffer_idx_);
        if (real_dt < buffer_dt) {
          auto sleep_dt = (buffer_dt - real_dt);
          // at least 1hz
          sleep_dt = std::min(sleep_dt, 1.0);
          std::this_thread::sleep_for(sleep_dt * 1s);
        }
      }
      if (frame_cb_) {
        frame_cb_(buffer_[cur_buffer_idx_].second, cur_buffer_idx_,
                  buffer_[cur_buffer_idx_].first);
        cur_buffer_idx_++;
      }
      if (is_loop_ && cur_buffer_idx_ == buffer_.size()) {
        // for the last data
        std::this_thread::sleep_for(500ms);
        jump(0);
      }
    } else {
      std::this_thread::sleep_for(100ms);
    }
  };

  void initSafeQueue() {
    rt_queue_free.clear();
    rt_queue_done.clear();
    rt_queue_free.setName("rt_queue_free");
    rt_queue_done.setName("rt_queue_done");
    for (int index = 0; index < max_queue_buffer_num; index++) {
      rt_queue_free.put(&frameBuffer[index]);
    }
  }

 private:
  double getBufferRelativeTime(size_t idx) {
    if (use_constant_fps_) {
      return double(idx) / constant_fps_;
    } else {
      return 0.0;
    }
  }

 private:
  FrameCallbackType frame_cb_;
  std::function<bool(double)> proc_cb_;
  std::vector<std::pair<double, std::shared_ptr<T>>> buffer_;
  std::map<int, std::pair<double, std::shared_ptr<T>>> rt_buffer_;
  bool init_ = false;
  size_t cur_buffer_idx_{0};
  std::chrono::time_point<std::chrono::steady_clock> real_start_;
  std::shared_ptr<DeviceContext> device_context_;
  bool is_loop_{true};
  bool use_constant_fps_{false};
  double constant_fps_{10};
  long RT_total_frames{0};
  std::mutex map_buffer_mutex_;
  SafeQueue<FrameRTType*> rt_queue_free;
  SafeQueue<FrameRTType*> rt_queue_done;
  FrameRTType frameBuffer[max_queue_buffer_num];
  double rate_jump_{0.0};
};

}  // namespace pointview
}  // namespace autox

#endif  // PLAYBACK_BUFFER_H
