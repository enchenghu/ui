#ifndef _COMMON_H
#define _COMMON_H


#include "Raf_Api.h"
#include "Accumulator.h"
#include "time.h"
#include <unistd.h>
#include <termios.h>
#include <pthread.h>
#include <mutex>
#include <chrono>
#include <thread>
#include <algorithm>


/* for rosbag record/play */
#include <iostream>
#include <rosbag/bag.h>
#include <rosbag/view.h>
#include <std_msgs/Int32.h>
#include <std_msgs/Int8.h>
#include <std_msgs/UInt8MultiArray.h>
#include "std_msgs/MultiArrayLayout.h"
#include "std_msgs/MultiArrayDimension.h"
#include "std_msgs/String.h"
#include <arbe_msgs/arbeTSlamObj.h>
#include <arbe_msgs/arbeSlamMsg.h>
#include <sensor_msgs/Imu.h>
#include <geometry_msgs/TwistWithCovarianceStamped.h>
#include "Utils.h"

#define MAX_BUFFER_LENGTH       40000000

//std::vector<TSlamObj> ObjectsPerFrame;

bool get_slam_validity();
arbe_msgs::arbeTSlamMetadata get_slam_metadata();
bool get_cameraBuffering();
void set_cameraBuffering(bool flag);
bool get_slamOnCam();
void set_slamOnCam(bool flag);
bool get_slamOnCam_ecoMode();
void set_slamOnCam_ecoMode(bool flag);
bool get_colorObjByClass();
void set_colorObjByClass(bool flag);
bool get_distOnCam();
void set_distOnCam(bool flag);

float get_dview_x();
void set_dview_x(float val);
float get_dview_y();
void set_dview_y(float val);

bool get_FreeSpace_enabled();
void set_FreeSpace_enabled(bool flag);
bool get_funnel_enabled();
void set_funnel_enabled(bool flag);

void set_discard_out_of_el_context(bool flag);
bool get_discard_out_of_el_context();
void set_dl_training_playback(bool flag);
bool get_dl_training_playback();

void imu_read_callback(const sensor_msgs::Imu::ConstPtr& msg);
void gps_read_callback(const geometry_msgs::TwistWithCovarianceStamped::ConstPtr& msg);
void slam_read_callback(const arbe_msgs::arbeSlamMsg::ConstPtr& array, int n_radar);
void set_aggregation(bool agg);
bool get_aggregation();
void set_localization_enable_disable(bool loc);
bool get_localization_enable();

arbe_msgs::arbeTSlamObj get_object(uint16_t i);
size_t get_num_objects();
void flag_used4display();
void flag_notused4display();
bool get_used4display();
uint16_t get_frame_num();
void set_frame_num(uint16_t fn);
//uint16_t get_host_vel_uint16();
float get_host_vel();
void set_host_vel(float vel);
bool get_SlamLastPacket();
void falsify_slamLastPacket();
void truesify_slamLastPacket();
void objects_mutex_lock();
void objects_mutex_unlock();
void get_color_global(uint16_t id, float &r, float &g, float &b);

/*
class Frame_objects{
protected:
    Frame_objects();
    static Frame_objects* m_pInstance;
    std::vector<TSlamObj> objects;
public:
    static Frame_objects* Instance();

    TSlamObj& object(uint index);
    //size_t size();
    //uint16_t get_frameCounter();
    void new_frame();
    void add_object(TSlamObj obj);
};
*/

class Slam_Color{
    protected:
        float color_a[64][3];
        Slam_Color(uint64_t seed = 42);
        static Slam_Color* m_pInstance;
    public:
        static Slam_Color* Instance();
        void get_color(uint16_t id, float &r, float &g, float &b);
        void get_color_ui8(uint16_t id, uint8_t &r, uint8_t &g, uint8_t &b);

};

class Color_Coding_Min_Max
{
protected:
    float dopp_cc_min = -3.0, dopp_cc_max = 3.0;
    float el_cc_min = 0, el_cc_max = 8.0;
    float dopp_grad_cc_min = -20.0, dopp_grad_cc_max = 20.0;
    float amp_cc_min = 80, amp_cc_max = 150;
    Color_Coding_Min_Max();
    static Color_Coding_Min_Max* m_pInstance;
public:
    static Color_Coding_Min_Max* Instance();

    void set_min(std::string ColoringType, float min);
    void set_max(std::string ColoringType, float max);
    void get_values(std::string ColoringType, float &min, float &max);
    void get_converted_values(std::string ColoringType, float &min, float &max);
    std::string get_units(std::string ColoringType);
};

void set_euler_mat(float alpha, float beta, float gamma);
void euler_rotate(TaregtCartesian &cart_detection);
void tiltTarget(TaregtCartesian &cart_detection);
void set_reset_mapping(bool flag);
bool get_reset_mapping();

int16_t get_classes_to_show();
void set_classes_to_show(int16_t cls);
void get_class_color(uint8_t cl, float &red, float &green, float &blue, std::string &fc_txt);
bool get_disp_processed_pc();
void set_disp_processed_pc(bool flag);
bool get_disp_objects();
void set_disp_objects(bool flag);

int load_camera_calib(bool do_for_all_radars );
void set_local_cart(const arbe_msgs::arbeSlamMsg::ConstPtr& msg);
void get_local_cart(float &x, float &y);
#endif
