#ifndef _TYPES_FMCW_H
#define _TYPES_FMCW_H


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

struct  EIGEN_ALIGN16 FmcwPointXYZRGBGeneric
{
        PCL_ADD_POINT4D;
        PCL_ADD_RGB;
        float range;
        float azimuth;
        float elevation;
        float doppler;
        float power1;
        float power2;
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


POINT_CLOUD_REGISTER_POINT_STRUCT (FmcwPointXYZRGBGeneric,           // here we assume a XYZRGB + "detectionData" (as fields)
                                (float, x, x)
                                (float, y, y)
                                (float, z, z)
                                (float, rgb, rgb)
                                (float, range, range)
                                (float, azimuth, azimuth)
                                (float, elevation, elevation)
                                (float, doppler, doppler)
                                (float, power1, power1)
                                (float, power2, power2)
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



#endif
