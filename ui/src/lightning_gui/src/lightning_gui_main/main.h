
#include <ros/ros.h>
#include <string.h>
#include <pcl/point_cloud.h>
#include <pcl_conversions/pcl_conversions.h>
#include <pcl/common/transforms.h>
#include <sensor_msgs/PointCloud2.h>
#include <visualization_msgs/Marker.h>
#include <visualization_msgs/MarkerArray.h>
#include <tf2_geometry_msgs/tf2_geometry_msgs.h>
#include <std_msgs/UInt32MultiArray.h>
#include "Utils.h"
#include "Raf_Api.h"
#include "Accumulator.h"
#include "time.h"
#include <pthread.h>
#include <mutex>
#include <chrono>
#include <thread>
#include <algorithm>

#include "common.hpp"

#define EXTPECTED_FPS 15

typedef enum
{
	StateStopped = 0,
	StateConnected,
	StateConfigured,
	StateStarted,
	StateRecording
}SystemState;


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

void STD_CALL ctrl_callback(int32_t type, uint8_t* buffer, uint32_t size);
void STD_CALL data_callback(int32_t type, uint8_t* buffer, uint32_t size);
void radar_set_params( void );
void pubGUIcontrols();
uint64_t STD_CALL GetSystemTimeCb();
TArbeApiMailBox tArbeApiMailBox;

int radar_connected=0;

pcl::PointCloud<ArbePointXYZRGBGeneric> cloud;
pcl::PointCloud<ArbePointXYZRGBGeneric> stationary_pc;
sensor_msgs::PointCloud2 output;
sensor_msgs::PointCloud2 stationaty_output;

//std::vector<TSlamObj> ObjectsPerFrame;
bool PointcloudLastPacket = false;
bool StationaryPointcloudLastPacket = false;
//bool SlamLastPacket = false;
uint8_t  bin_file_buffer[70000];
std::string ctrlIP;
std::string dataIP;
int useSimulator;
int rosbag_playing = 1;
int unity_playing = 0;
int rosbag_recording = 0;
int threshold3d;
int threshold4d;
bool ntc_3d_enabled = false;
bool ntc_4d_enabled = true;
bool cfar_3d_enabled = true;
bool cfar_4d_enabled = false;
bool phase_enabled = false;
bool algo_thr_metadata_enabled = false;
int DynamicAzimuth;
int DynamicElevation;
float MinDoppler;
float MaxDoppler;
float MinHeight = -8;
float MaxHeight = 8;
bool tx_started = false;
int AsphaltRoughness;

std::string mode;
std::string RangeType;
std::string ColoringType;

int radar_id = 0;
float radar_yaw_angle = 0;
float radar_pitch_angle = 0;
float radar_x_offset = 0;
//float radar_y_offset = 0;
float radar_z_offset = 0;
std::vector<float> radar_yaw_angles;
int n_radars;

float cam_euler_alpha = 0;
float cam_euler_beta = 0;
float cam_euler_gamma = 0;

std::string RecordFileName;
std::string Region;
int recording = 0;
int terminating = 0;
FILE * file;
FILE * unity_bin_file;
SystemState RadarState;
//uint16_t host_vel; // Ego Velocity
double ego_velocity = 0; // Ego Velocity (signed double)
int countTargets = 0;
int grid_cell_size = 10;
TStartTxInfo startInfo;
TStopInfo stopInfo;
int mux = 0; //for alternating pointclouds
int selectedAzimuthBin; // For Range/Doppler map display
int marker_text_size = 1;
QString TestDescriptionString;

// For FPS calculation
uint64_t usCurrentTime = 0;
uint64_t usPreviousTime = 0;

//globally defined
ros::Publisher arbe_info_markers;
visualization_msgs::Marker ego_velocity_marker;
visualization_msgs::Marker zero_point_marker;
visualization_msgs::Marker grid_size_marker_pos;
visualization_msgs::Marker grid_size_marker_neg;
visualization_msgs::Marker imu_marker;
visualization_msgs::Marker gps_marker;
visualization_msgs::Marker dtections_per_frame_marker;

std::vector<ros::Subscriber> objects_subscribers;

ros::Subscriber imu_sub;
ros::Subscriber gps_sub;
ros::Subscriber objects_sub;
ros::Subscriber gui_controls_sub;
ros::Subscriber old_gui_controls_sub;
ros::Subscriber arbe_serial_sub;
ros::Subscriber old_arbe_serial_sub;
ros::Subscriber selected_points_sub;
ros::Subscriber ctl_reset_PC_aggregation;
ros::Subscriber arbe_debug_data_count_sub;

std::mutex g_buffer_mutex;
std::mutex g_text_mutex;
std::mutex g_objects_mutex;

// GPS/IMU Related/*
//PhidgetGPSHandle gps0;
//PhidgetReturnCode res;
//PhidgetSpatialHandle spatial0;*/

//union ArbeDetectionUnion
//{
//	struct s_arbe_detections_data
//	{
//		float power;
//		float elevation;
//		float azimuth;
//		float doppler;
//		float range;
//		int range_bin;
//		int elevation_bin;
//		int azimuth_bin;
//		int doppler_bin;
//		int power_value;
//	} fields;

//	uint8_t i_arbe_detections_data[40]; /* size of TargetGeneric * 2 */
//};

//ArbeDetectionUnion DetectionsData[20000];
//std::vector<ArbeDetectionUnion> DetectionsData;
int received_frame_size;
ros::NodeHandlePtr n;
