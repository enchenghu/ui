/*
 * Copyright (C) 2019 Arbe - http://www.arberobotics.com/ 
 * 
 * 
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions 
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright 
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the   
 *    distribution.
 *
 *    Neither the name of Arbe nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
*/

#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "ros/ros.h"
#include <string.h>
#include "Utils.h"
#include "Raf_Api.h"
#include "Accumulator.h"
#include "time.h"
#include <unistd.h>
#include <termios.h>
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
//#include <phidget22.h>
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
#include "std_msgs/Float64.h"
#include <std_msgs/UInt8MultiArray.h>
#include <std_msgs/UInt32MultiArray.h>
#include "std_msgs/MultiArrayLayout.h"
#include "std_msgs/MultiArrayDimension.h"
#include <std_msgs/String.h>

#include <pcl/io/pcd_io.h>
#include <pcl/point_types.h>
#include <pcl/filters/voxel_grid.h>
#include <pcl/point_cloud.h>
#include <pcl_conversions/pcl_conversions.h>
#include <pcl/common/transforms.h>
#include <sensor_msgs/PointCloud2.h>
#include <visualization_msgs/Marker.h>

#define MAX_BUFFER_LENGTH       40000000
#define CTRL_BUFFER_LENGTH      20000000 //20M
#define ETH_LOGS_BUFFER_LEN	4096

uint8_t ctrl_buffer[CTRL_BUFFER_LENGTH];
uint8_t ctrl_server_reply[MAX_BUFFER_LENGTH];
uint8_t data_buffer[MAX_BUFFER_LENGTH];
uint8_t data_server_reply[2*MAX_BUFFER_LENGTH];//ask Dror
char EthLogBuffer[ETH_LOGS_BUFFER_LEN];

int arg_radar_id;
std::string arg_radar_ip;
int arg_ctrl_port;
int arg_data_port;
int arg_logs_port;
double arg_radar_yaw_angle;
double arg_radar_pitch_angle;
double arg_radar_x_offset;
double arg_radar_y_offset;
double arg_radar_z_offset;

int radar_id;
double radar_yaw_angle;
double radar_pitch_angle;
double radar_x_offset;
double radar_y_offset;
double radar_z_offset;
float radar_base_freq;//Ghz

float MinDoppler = -100;
float MaxDoppler = 100;
uint8_t noise_level_db = 60;
int marker_text_size = 1;
double ego_velocity = 0; // Ego Velocity (signed double)
int selectedAzimuthBin = 0; // For Range/Doppler map display
bool tx_started = false;

// For FPS calculation
uint64_t usCurrentTime = 0;
uint64_t usPreviousTime = 0;

struct  EIGEN_ALIGN16 ArbePointXYZRGBGeneric
{
	PCL_ADD_POINT4D;
	PCL_ADD_RGB;
	float range;
	float azimuth;
	float elevation;
	float doppler;
	float power;
	float range_bin;
	float azimuth_bin;
	float elevation_bin;
	float doppler_bin;
	float power_value;
	float phase_value;
	uint32_t timestamp_sec;
	uint32_t timestamp_nsec;
	float snr;
	uint32_t radar_id;
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW     // make sure our new allocators are aligned
};

POINT_CLOUD_REGISTER_POINT_STRUCT (ArbePointXYZRGBGeneric,           // here we assume a XYZRGB + "detectionData" (as fields)
				(float, x, x)
				(float, y, y)
				(float, z, z)
				(float, rgb, rgb)
				(float, range, range)
				(float, azimuth, azimuth)
				(float, elevation, elevation)
				(float, doppler, doppler)
				(float, power, power)
				(float, range_bin, range_bin)
				(float, azimuth_bin, azimuth_bin)
				(float, elevation_bin, elevation_bin)
				(float, doppler_bin, doppler_bin)
				(float, power_value, power_value)
				(float, phase_value, phase_value)
				(uint32_t, timestamp_sec, timestamp_sec)
				(uint32_t, timestamp_nsec, timestamp_nsec)
				(float, snr, snr)
				(uint32_t, radar_id, radar_id)
)

pcl::PointCloud<ArbePointXYZRGBGeneric> cloud;
sensor_msgs::PointCloud2 output;

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

