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

#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QProgressDialog>
#include <QDebug>
#include <QString>
#include "main.h"
#include "mainwindow.h"
#include "viewpanel.h"
#include "common.hpp"
#include <arbe_msgs/arbeGUIsettings.h>
#include "Per_radar_settings.hpp"
#include "arbe_msgs/arbeNewPcMsg.h"
#include <arbe_msgs/arbeBuildNumber.h>
#include <arbe_msgs/arbeBoolWithTime.h>
#include "Utils.h"
#include "rqt_gauges/my_plugin.h"
#include "rqt_gauges/qcgaugewidget.h"
#include <iostream>
#include <fstream>
#include <sys/ioctl.h>

#include <pcl/io/pcd_io.h>
#include <pcl/point_types.h>
#include <pcl/filters/voxel_grid.h>

QString labelText;
QProgressDialog* pd;


static arbe_msgs::arbeNewPcMsg::ConstPtr PcFrame;
static arbe_msgs::arbeNewPcMsg::ConstPtr StationaryPcFrame;
static arbe_msgs::arbeSlamMsg::ConstPtr slamMsg;
static bool clear_stationary_once = true;
static float host_w_z; // Ego Velocity

extern bool radar_playing;
extern bool slam_displayed;
extern QString bias_4d;

#define MAX_AGG_POINTS  200000
#define BUILD_LENGTH 10
#define AGG_PC

static int slamBuild = -1;
static int arbe_guiBuild = -1;
static int arbe_api_commonBuild = -1;
static int arbe_msgsBuild = -1;
static int radarBuild = -1;
int  console_level = (int)ros::console::levels::Warn;
extern QString noise_level;
uint8_t noise_level_db;
bool enable_gui = true;
static int noise_level_int = -1;
static int bias_4d_int = -1;

std::vector<int> yaw_in_degrees, Ctrl_Port,Data_Port,Logs_Port;
std::vector<float> pitch_in_degrees, Radar_X_Offset, Radar_Y_Offset, Radar_Z_Offset, moment_length_in_m, radar_base_freq;
std::vector<std::string> radar_ip, camera_calib_file, Radar_Name;

ros::Publisher arbe_BuildNum_pub ;
ros::Publisher arbe_ctrl_pub;
ros::Publisher pcl_pub;
ros::Publisher stationary_pcl_pub;
ros::Publisher marker_pub;
ros::Publisher arbe_gui_commands_pub;
ros::Publisher trigger_side_slam_pub;
ros::Subscriber master_slam_sub;

/* Publish the GUI controls values can be used for playback */
ros::Publisher arbe_controls_pub ;
arbe_msgs::arbeGUIsettings controls_data;


void pubGUIcontrols()
{

	if (enable_gui == false)
		return;

	gui_controls_t gui_controls_data;

	arbe_controls_pub.publish(controls_data);
}


void timerCallback(const ros::TimerEvent& event)
{
	/* Publish the gui controls value every 500msec */
	pubGUIcontrols();
}

void pubBuildNum()
{
	arbe_msgs::arbeBuildNumber buildNumMsg;
	ros::Time rosTime;
	buildNumMsg.slam = slamBuild;
	buildNumMsg.arbe_gui = arbe_guiBuild;
	buildNumMsg.arbe_api_common = arbe_api_commonBuild;
	buildNumMsg.arbe_msgs = arbe_msgsBuild;
	buildNumMsg.radar = radarBuild;
	buildNumMsg.header.stamp = rosTime.now();
	arbe_BuildNum_pub.publish(buildNumMsg);
}

void getBuildNum()
{
	char ver[BUILD_LENGTH];
	char fileName[400];
	bool isOpen = false;
	sprintf(fileName,"%s/addons/arbe_gui_build.ver",SRC_DIR);
	std::ifstream arbe_gui_build(fileName);
	isOpen = arbe_gui_build.is_open();
	if(arbe_gui_build) {
		arbe_gui_build.getline(ver, BUILD_LENGTH);  // delim defaults to '\n'
		arbe_gui_build.close();
	}
	arbe_guiBuild = std::atoi(ver);

	sprintf(fileName,"%s/addons/slam_build.ver",SRC_DIR);
	std::ifstream slam_build(fileName);
	isOpen = slam_build.is_open();
	if(slam_build) {
		slam_build.getline(ver, BUILD_LENGTH);  // delim defaults to '\n'
		slam_build.close();
	}
	slamBuild = std::atoi(ver);

	sprintf(fileName,"%s/addons/arbe_msgs_build.ver",SRC_DIR);
	std::ifstream arbe_msgs_build(fileName);
	isOpen = arbe_msgs_build.is_open();
	if(arbe_msgs_build) {
		arbe_msgs_build.getline(ver, BUILD_LENGTH);  // delim defaults to '\n'
		arbe_msgs_build.close();
	}
	arbe_msgsBuild = std::atoi(ver);

	sprintf(fileName,"%s/addons/arbe_api_common_build.ver",SRC_DIR);
	std::ifstream arbe_api_common_build(fileName);
	isOpen = arbe_api_common_build.is_open();
	if(arbe_api_common_build) {
		arbe_api_common_build.getline(ver, BUILD_LENGTH);  // delim defaults to '\n'
		arbe_api_common_build.close();
	}
	arbe_api_commonBuild = std::atoi(ver);
//	printf("slamBuild %d, arbe_guiBuild %d, arbe_api_commonBuild %d, arbe_msgsBuild %d",slamBuild , arbe_guiBuild , arbe_api_commonBuild , arbe_msgsBuild );
	pubBuildNum();
}
/* 
   Color the pointcloud detections based on the selected coloring scheme
*/

const int n_colors_jet = 4;
static float color_jet[n_colors_jet][3] = { {0,0,255}, {0,255,0}, {255,255,0}, {255,0,0} };

float normalize(float x, float min, float span)
{
	return (x-min)/span;
}

void apply_color(float z, uint8_t &r, uint8_t &g, uint8_t &b)
{
	int idx1;        // |-- Our desired color will be between these two indexes in "color".
	int idx2;        // |
	float fractBetween = 0;  // Fraction between "idx1" and "idx2" where our value is.


	if(z <= 0) {  idx1 = idx2 = 0; }    // accounts for an input <=0
	else if(z >= 1)  {  idx1 = idx2 = n_colors_jet-1; }    // accounts for an input >=0
	else
	{
		z = z * (n_colors_jet-1);        // Will multiply value by 3.
		idx1  = uint16_t(z); // Our desired color will be after this index.
		idx2  = (idx1+1);                        	// ... and before this index (inclusive).
		fractBetween = z - float(idx1); // Distance between the two indexes (0-1).
	}
	r = (color_jet[idx2][0] - color_jet[idx1][0])*fractBetween + color_jet[idx1][0];
	g = (color_jet[idx2][1] - color_jet[idx1][1])*fractBetween + color_jet[idx1][1];
	b = (color_jet[idx2][2] - color_jet[idx1][2])*fractBetween + color_jet[idx1][2];
}


void pointcloud_color( pcl::PointCloud<ArbePointXYZRGBGeneric> &buffer, size_t j, float cc_min, float cc_max, float cc_span) // NEW FUNCTION!
{
	if (std::isnan(buffer.points[j].z)) buffer.points[j].z = 0;
	if ( strcmp(ColoringType.c_str(), "Doppler") == 0 )
	{
		/* Color by doppler */
		float rel_speed = (buffer.points[j].doppler + ego_velocity* cos(buffer.points[j].azimuth));
		if (rel_speed > cc_max) {
			buffer.points[j].r = 0;
			buffer.points[j].g = 0;
			buffer.points[j].b = 255;
		}
		else if (rel_speed < cc_min) {
			buffer.points[j].r = 255;
			buffer.points[j].g = 0;
			buffer.points[j].b = 0;
		}
		else {
			buffer.points[j].r = 0;
			buffer.points[j].g = 255;
			buffer.points[j].b = 0;
		}
	}
	else
	{
		float x, z;
		if ( strcmp(ColoringType.c_str(), "Doppler Gradient") == 0 )
		{
			x = (buffer.points[j].doppler * 10 +  ego_velocity* cos(buffer.points[j].azimuth)) ;
		}
		else if ( (strcmp(ColoringType.c_str(), "Amplitude") == 0)  || (strcmp(ColoringType.c_str(), "Amplitude-Flat") == 0 )
			|| (strcmp(ColoringType.c_str(), "Range/Doppler") == 0 ))
		{
			x = buffer.points[j].power;
		}
		else
		{
			x = buffer.points[j].z;
		}

		z = normalize(x, cc_min, cc_span);
		apply_color(z,buffer.points[j].r,buffer.points[j].g,buffer.points[j].b);
	}
	buffer.points[j].a = 0.2;
}

/* 
   Trigger the rosbag application to start recording the published pointcloud
*/
void rosbag_start_recording( void )
{
	bool rec_rviz_pointcloud = get_rec_rviz_pointcloud();
    char rosbag_record_command[700];
	std::string rviz_str;
	std::string processed_str, objects_str, camera_str, debug_str, dl_training_str, fusion_str, parsed_str, raw_str, settings_str, legacy_pc_str, nav_topics_str;
	std::string excluded_settings_str="";
	std::string RosbagdFileName = RecordFileName;

	bool all_postProcessing, camera, all_debug, dl_training, fusion, parsed, raw_topics, settings_topics, legacy_pc, nav_topics, object_list;
	bool record_something = get_rec_topics(all_postProcessing, camera, all_debug, dl_training, fusion, parsed,
										   raw_topics, settings_topics, legacy_pc, nav_topics, object_list);

	if (!record_something)
	{
		ROS_ERROR("No topics were chosen to record");
		return;
	}

	if (TestDescriptionString.size() > 0)
	{
	TestDescriptionString.replace(" ", "_");
		RosbagdFileName.append("_");
		RosbagdFileName.append(TestDescriptionString.toStdString().c_str());
	}

	ROS_DEBUG("start rosbag recording.");

    std::string regex_str = "";

	if (rec_rviz_pointcloud)
    {
		rviz_str = ("'(.*)/arbe/rviz/(.*)'");
        regex_str = " -e ";
    }
	else
		rviz_str = ("");


    if(all_postProcessing)
	{
        processed_str = ("'(.*)/arbe/processed/(.*)'");
        objects_str="";
        regex_str = " -e ";
	}
	else
	{
        processed_str = ("");
		if(object_list)
		{
			objects_str = "'/arbe/processed/objects(.*)'";
			regex_str = " -e ";
		}
		else
			objects_str = "";
	}

    if(camera)
    {
		camera_str = "'(.*)/cv_camera(.*)/image_raw/compressed' '(.*)/pylon_camera_node(.*)/image_color/compressed'";
		regex_str = " -e ";
    }
    else
    {
        camera_str="";
    }
    if(all_debug)
    {
        debug_str = "'(.*)/arbe/debug/(.*)'";
        regex_str = " -e ";
    }
    else
    {
        debug_str = "";
    }
    if(dl_training)
    {
		dl_training_str = "'/arbe/dl_training/(.*)'";
        regex_str = " -e ";
    }
    else
    {
        dl_training_str = "";
    }
    if(fusion)
    {
        fusion_str = "'/arbe/fusion/(.*)'";
        regex_str = " -e ";
    }
    else
    {
        fusion_str = "";
    }
    if(parsed)
    {
        parsed_str = "'/arbe/parsed/(.*)'";
        regex_str = " -e ";
    }
    else
    {
        parsed_str = "";
    }
    if(raw_topics)
    {
        raw_str = "'(.*)/arbe/raw/(.*)'";
        regex_str = " -e ";
    }
    else
    {
        raw_str = "";
    }
    if(settings_topics)
    {
        settings_str = "'(.*)/arbe/settings/(.*)'";
		excluded_settings_str = " -x '/arbe/settings/(.*)record'";
        regex_str = " -e ";
    }
    else
    {
        settings_str = "";
    }
    if(legacy_pc)
    {
		legacy_pc_str = "'(.*)/raw/radar_tx(.*)'";
		regex_str = " -e ";
    }
    else
    {
		legacy_pc_str = "-x '(.*)/raw/radar_tx(.*)'";
    }
    if(nav_topics)
    {
        nav_topics_str ="'(.*)/imu/(.*)'  '(.*)/imu_filter_node/(.*)' '(.*)/ublox1/(.*)' '(.*)/ublox2/(.*)'";
        regex_str = " -e ";
    }
    else
    {
        nav_topics_str = "";
    }

	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	sprintf(rosbag_record_command,"rosbag record --split --size=3072 -o  %s/%s  %s %s   %s   %s %s %s    %s %s %s   %s %s %s %s __name:=arbe_bag&",
			get_save_folder().c_str(),RosbagdFileName.c_str(),
			regex_str.c_str(),
			objects_str.c_str(),
			camera_str.c_str(),
			processed_str.c_str(), rviz_str.c_str(), debug_str.c_str(),
			parsed_str.c_str(),raw_str.c_str(), dl_training_str.c_str(),
			settings_str.c_str(),nav_topics_str.c_str(), excluded_settings_str.c_str(), legacy_pc_str.c_str());

	ROS_DEBUG("rosbag command: %s",rosbag_record_command);
	int ret = system(rosbag_record_command); 
	rosbag_recording = 1;
	pubBuildNum();
}

/*
   Send a provided data buffer to the radar over the TCP control socket
*/
uint32_t STD_CALL TransmitBuffer(int messageID, uint8_t* buffer, unsigned int len)
{
	ROS_INFO("TransmitBuffer: publish %d bytes",len);
	std_msgs::UInt8MultiArray ctrl_data;

	ctrl_data.data.clear();
	ctrl_data.data.insert(ctrl_data.data.begin(),buffer,buffer+len);
	arbe_ctrl_pub.publish(ctrl_data);
	return 0;
}

/*
   Stop rosbag recording
*/
void rosbag_stop_recording( void )
{
	int ret = system("rosnode kill /arbe_bag");
	rosbag_recording = 0;
}

/*
   Calculate elaped time between frames
*/
float calculate_fps( uint64_t timestamp_ms )
{
	float frames_per_second;
	usCurrentTime = timestamp_ms;
	frames_per_second = (1000.0)/(usCurrentTime-usPreviousTime) ; // delta time is in milliseconds
	usPreviousTime = usCurrentTime;
	return frames_per_second;
}

/* 
   This thread is used for playing back pointcloud recordings captured in native 
   binary format (unity)
*/
#define SIZE_OF_FILE_READ (1024 * 64) 
void *playback_thread(void* args)
{
	//ros::NodeHandlePtr n = boost::make_shared<ros::NodeHandle>();
	ros::Publisher arbe_binary_pcl_pub = n->advertise<std_msgs::UInt8MultiArray>("/arbe/raw/radar_tx",100);
	ros::Rate loop_rate(30);
	int recv_size;
	std_msgs::UInt8MultiArray pointcloud_data;

	while (ros::ok() && (!terminating))
	{
		if (unity_playing == 1)
		{
			int unity_bytes_read = 0;
			/* Read and display data */
			unity_bytes_read = fread(bin_file_buffer, (SIZE_OF_FILE_READ), 1, unity_bin_file);
			ROS_DEBUG("Read %d bytes from a unity recording file",SIZE_OF_FILE_READ);
			if ( unity_bytes_read > 0 )
			{
					/* Publish the binary data received from the socket
					   So it will can recorded using rosbag */
					pointcloud_data.data.clear();
					for (int i = 0; i < SIZE_OF_FILE_READ; i++)
					{
						pointcloud_data.data.push_back(bin_file_buffer[i]);
					}
					arbe_binary_pcl_pub.publish(pointcloud_data);
					usleep(100000);
			}
		}
		ros::spinOnce();
		loop_rate.sleep();
	}
}


void debug_data_read_callback(const std_msgs::UInt32MultiArray::ConstPtr &msg)
{
	/* adjust progress bar values */
	pd->setMaximum( msg->data[0] );
	pd->setValue( msg->data[1] );
}

/* Used for pointcloud sorting */
bool CompareRecords(const ArbePointXYZRGBGeneric& a, const ArbePointXYZRGBGeneric& b)
{
    if (a.azimuth < b.azimuth)
        return true;
    else if (a.azimuth > b.azimuth)
        return false;

    else if (a.range > b.range)
        return false;

    return false;
}

/* Filter the pointcloud for showing the strongest target in case 
   multiple targets are detected at the same location. we flaten the 
   view, so elevation is not counted */
void pointcloud_filter_amplitude ( size_t* array_size )
{
	/* Sort the array according to range, azimuth */
	std::sort (cloud.points.begin(), cloud.points.end(),CompareRecords);

	/* traverse through the array and pick from adjacent detections the one with the highest power. remove the rest */
	for (auto target_it = cloud.points.begin()+1;target_it <= cloud.points.end(); target_it++) 
	{
		if ( (target_it->range == (target_it-1)->range ) && (target_it->azimuth ==  (target_it-1)->azimuth) )
		{
			/* more then one target at same location! */
			if (target_it->power > (target_it-1)->power)
			{
				/* stronger target at the same location remove the weaker one */
				cloud.points.erase(target_it-1);
				*array_size = *array_size - 1;
			}
		}
	}
}

/* 
   This thread prepares the ros pointcloud mesaages and slam objects 
   and publish them to Rviz
*/
uint8_t gui_controls_publish_subrate = 0;
static uint32_t stationary_width = 0;
static float agg_y = 5;

pcl::PCLPointCloud2 point_cloud2;
pcl::PCLPointCloud2::Ptr point_cloud2_ptr(&point_cloud2);
pcl::PCLPointCloud2::Ptr cloud_filtered (new pcl::PCLPointCloud2 ());
uint32_t total_pts =0;

bool compare_timestamp(ArbePointXYZRGBGeneric a, ArbePointXYZRGBGeneric b)
{
    return ((double)a.timestamp_sec + ((double)a.timestamp_nsec)/1000000000) > ((double)b.timestamp_sec + ((double)b.timestamp_nsec)/1000000000);
}

static arbe_msgs::arbeTSlamMetadata slam_mdata = arbe_msgs::arbeTSlamMetadata();
static arbe_msgs::arbeNewPcMsg::ConstPtr NewPcFrame;

void radar_configure_pc()
{
	ROS_INFO("Configure Pointcloud - Phase Enabled = %d.\n\r",phase_enabled);
	if ( phase_enabled )
	{
    int phase_config =phase_enabled ? 0 : 1;

    TConfigPacketFormatInfo tConfigPacketFormatInfo;
    tConfigPacketFormatInfo.unPacketFormat =(uint8_t)phase_config + 2; // 3 for 80 Bit uncompressed(48Bit), 2 for 96 Bit uncompressed(56Bits - with phase)
    RAF_API_SysCfg_ConfigPacketFormat(&tArbeApiMailBox,&tConfigPacketFormatInfo);
		sleep(1);
	}
}

void radar_quit()
{
	int ret;
	ROS_WARN("received stop command 'q'... stop the GUI");
	if (rosbag_recording == 1) {
		rosbag_stop_recording();
	}
	ROS_WARN("trying to stop radar nodes");

	/* Publish the goodbye command to trigger a kill of radar nodes */
	std_msgs::String msg;
	std::stringstream ss;
	ss << "goodbye";
	msg.data = ss.str();
	ROS_INFO("%s", msg.data.c_str());
	arbe_gui_commands_pub.publish(msg);
	ret = system("rosnode kill screen_grab screen_recorder");
	ret = system("killall cv_camera_node");

	terminating = 1;
	exit(0);
}


/*
   provides the system time needed by the radar framework (RAF)
*/
unsigned long STD_CALL GetSystemTime()
{
	time_t tt;
	time( &tt );
	tt = tt + 8*3600;  // transform the time zone
	tm* t= gmtime( &tt );
	return tt;
}

/*
   Control An-echoic chamber mode
*/
void radar_set_chamber_mode( bool enable )
{
	TDebugOperationInfo debug_info;
	debug_info.eDebugOperation = AnechoicChamberRTS;
	if ( enable == true )
	{
		ROS_DEBUG("Enable Chamber mode\n\r");
		debug_info.aunParams[0] = 1;
	}
	else
	{
		ROS_DEBUG("Disable Chamber mode\n\r");
		debug_info.aunParams[0] = 0;
	}
	RAF_API_Dbg_TestOperation(&tArbeApiMailBox,&debug_info);
}

/*
   Read debug data from the radar
*/


static float heading = 0;
static bool prev_localization = false;
static bool side_slams_on = true;
#define VELOCITY_TH 0.25 // m/s


/* Callback for debug messages recieved from the radar over the serial port */
void serial_read_callback(const std_msgs::String::ConstPtr& msg)
{
	ROS_WARN("Serial msg: [%s]", msg->data.c_str());
	return;
}

void process_selected_points_callback(const sensor_msgs::PointCloud2ConstPtr& input)
{
	pcl::PointCloud<ArbePointXYZRGBGeneric> cloud;
	char FileName[40];
	const char RecordFileName[32] = "arbe-selected-points";
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	sprintf(FileName,"%s-%d%02d%02d-%02d%02d%02d.csv",
		RecordFileName, tm.tm_year + 1900,
		tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min,
		tm.tm_sec);

	pcl::fromROSMsg (*input, cloud);//cloud is the output

	//if(pcl::io::savePCDFileASCII (FileName, cloud)>=0)
	//{std::cerr << "Saved  " << FileName << std::endl;}

	std::ofstream csvfile(FileName);

	if (csvfile.is_open())
	{
		csvfile << "radar_id, x, y, z, range, azimuth, elevation, doppler, power, range_bin, azimuth_bin, elevation_bin"
			<< ", doppler_bin, power_value, phase_value, timestamp_sec, timestamp_nsec, snr" 
			<< "\t\n";

		char outFormat[2000] = "%d,%20.3f,%20.3f,%20.3f,%20.3f,%20.3f,%20.3f,%20.3f,%20.3f,%20.3f,%20.3f,%20.3f,%20.3f,%20.3f,%20.3f,%d,%d,%20.3f";
		char line[10000] = "";
		for (int i = 0; i < cloud.points.size(); i++)
		{
			sprintf(line, outFormat,
				cloud.points[i].radar_id,
				cloud.points[i].x, cloud.points[i].y, cloud.points[i].z, 
				cloud.points[i].range, 
				cloud.points[i].azimuth, 
				cloud.points[i].elevation, 
				cloud.points[i].doppler,
				cloud.points[i].power, 
				cloud.points[i].range_bin,
				cloud.points[i].azimuth_bin, 
				cloud.points[i].elevation_bin,
				cloud.points[i].doppler_bin,
				cloud.points[i].power_value, 
				cloud.points[i].phase_value,
				cloud.points[i].timestamp_sec,
				cloud.points[i].timestamp_nsec,
				cloud.points[i].snr);

			csvfile << line << "\t\n";
		}
		csvfile.close();
		std::cerr << "Saved  " << FileName << std::endl;
	}
	else
	{
		std::cerr << "Unable to open file: " << FileName;
	}
}

/*
   Start the radar
*/
void radar_start_transmit()
{
	radar_configure_pc();

	RAF_API_RdrCtrl_StartTx(&tArbeApiMailBox,&startInfo);
	rosbag_playing = 0; // to avoid playing a recording together with realtime data
	tx_started = true;
	pubGUIcontrols();
}


/*
   Stop the radar
*/
void radar_stop_transmit()
{
	rosbag_playing = 1;
	RAF_API_RdrCtrl_StopTx(&tArbeApiMailBox,&stopInfo);

	/* subscribe to to original(parsed) pointcloud for playing recordings */
	tx_started = false;
	pubGUIcontrols();
}




/* Main function */
int main(int argc, char **argv)
{
	pthread_t playback_thread_id;
	int ret;
	ros::init(argc, argv, "lidar_gui");
	ros::Time::init();
	ros::NodeHandlePtr pLidarNodeHandle = boost::make_shared<ros::NodeHandle>();
#if 1
	QApplication app(argc, argv);
	QCoreApplication::setOrganizationName("FMCW Lidar");
	QCoreApplication::setApplicationName("FMCW Lidar ROS GUI");
	QCoreApplication::setApplicationVersion(QT_VERSION_STR);
	QCommandLineParser parser;
	parser.setApplicationDescription(QCoreApplication::applicationName());
	parser.addHelpOption();
	parser.addVersionOption();
	parser.addPositionalArgument("file", "The file to open.");
	parser.process(app);
#endif
#if 1
	MainWindow mainWin;
	const QRect availableGeometry = QApplication::desktop()->availableGeometry(&mainWin);
	mainWin.resize(availableGeometry.width() / 2, availableGeometry.height() * 2 / 3);
	mainWin.move((availableGeometry.width() - mainWin.width()) / 2, (availableGeometry.height() - mainWin.height()) / 2);
	mainWin.show();
	return app.exec();
#endif

}
