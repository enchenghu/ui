#ifndef _TYPES_FMCW_H
#define _TYPES_FMCW_H


#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "ros/ros.h"
#include <string.h>
#include "time.h"
#include <unistd.h>
#include <termios.h>
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <mutex>
#include <chrono>
#include <thread>
#include <algorithm>
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
        float distance;
        float horizontal;
        float vertical;
        float speed;
        float indensity;
        int chan_id;
        uint32_t timestamp_sec;
        uint32_t timestamp_nsec;
        EIGEN_MAKE_ALIGNED_OPERATOR_NEW     // make sure our new allocators are aligned
};


POINT_CLOUD_REGISTER_POINT_STRUCT (FmcwPointXYZRGBGeneric,           // here we assume a XYZRGB + "detectionData" (as fields)
                                (float, x, x)
                                (float, y, y)
                                (float, z, z)
                                (float, rgb, rgb)
                                (float, distance, distance)
                                (float, horizontal, horizontal)
                                (float, vertical, vertical)
                                (float, speed, speed)
                                (float, indensity, indensity)
                                (int, chan_id, chan_id)
                                (uint32_t, timestamp_sec, timestamp_sec)
                                (uint32_t, timestamp_nsec, timestamp_nsec)
)



#endif
