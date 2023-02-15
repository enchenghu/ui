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

#ifndef _VISUALIZATION_NODE_H
#define _VISUALIZATION_NODE_H

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
#define ETH_LOGS_BUFFER_LEN	1024

uint8_t ctrl_buffer[CTRL_BUFFER_LENGTH];
uint8_t ctrl_server_reply[MAX_BUFFER_LENGTH];
uint8_t data_buffer[MAX_BUFFER_LENGTH];
uint8_t data_server_reply[2*MAX_BUFFER_LENGTH];//ask Dror
char EthLogBuffer[ETH_LOGS_BUFFER_LEN];

int arg_radar_id;
std::string arg_radar_name;
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

float MinDoppler = -100;
float MaxDoppler = 100;
uint8_t noise_level_db = 60;
int marker_text_size = 1;
double ego_velocity = 0; // Ego Velocity (signed double)
int selectedAzimuthBin = 0; // For Range/Doppler map display
float hostHeading = 0;
float hostHeadingUnc = -1;
double local_cart_x = 0, local_cart_y = 0;
bool 	is_localization_active=false, is_aggregation_active=false;


// For FPS calculation
uint64_t usCurrentTime = 0;
uint64_t usPreviousTime = 0;

pcl::PointCloud<ArbePointXYZRGBGeneric> cloud;
pcl::PointCloud<ArbePointXYZRGBGeneric> stationary_pc;

sensor_msgs::PointCloud2 output;
sensor_msgs::PointCloud2 stationaty_output;


#endif
