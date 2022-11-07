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

void render_3d_car(float x, float y, float yaw);

void pubGUIcontrols()
{

	if (enable_gui == false)
		return;

	gui_controls_t gui_controls_data;
	
	viewpanel::get_controls_values( &gui_controls_data );
	
	controls_data.mode = gui_controls_data.mode_index;
	controls_data.range = gui_controls_data.range_index;
	controls_data.threshold4d = threshold4d;
	controls_data.threshold3d = threshold4d - bias_4d.toInt();
	controls_data.DynamicAzimuth = gui_controls_data.DynamicAzimuth;
	controls_data.DynamicElevation = gui_controls_data.DynamicElevation;

	controls_data.isNtc3dOn = ntc_3d_enabled>0;
	controls_data.isNtc4dOn = ntc_4d_enabled>0;
	controls_data.isCfar3dOn = cfar_3d_enabled>0;
	controls_data.isCfar4dOn = cfar_4d_enabled>0;
	controls_data.isPhaseEnabled = phase_enabled>0;
	controls_data.isMetaDataEnabled = algo_thr_metadata_enabled>0;

	controls_data.ColoringType = ColoringType;
	controls_data.MinDoppler = MinDoppler;
	controls_data.MaxDoppler = MaxDoppler;
	controls_data.discardOutOfElContext = get_discard_out_of_el_context();
	controls_data.disp_processed_pc = get_disp_processed_pc();
	controls_data.disp_slam = get_disp_objects();
	controls_data.noise_level_db = noise_level.toInt();
	controls_data.selectedAzimuthBin = selectedAzimuthBin;
	controls_data.tx_started = tx_started;
	controls_data.radar_id = radar_id;
	controls_data.radar_x_offset = radar_x_offset;
//	controls_data.radar_y_offset = radar_y_offset;
	controls_data.radar_z_offset = radar_z_offset;
	controls_data.radar_yaw_angle = radar_yaw_angle;
	controls_data.radar_pitch_angle = radar_pitch_angle;

	controls_data.localization_active = get_localization_enable();
	controls_data.aggregation_active = get_aggregation();

	controls_data.color_obj_by_class = get_colorObjByClass();


	//controls_data.cam_euler_alpha = cam_euler_alpha;
	//controls_data.cam_euler_beta = cam_euler_beta;
	//controls_data.cam_euler_gamma = cam_euler_gamma;
	//controls_data.header.stamp = rosTime.now();

	Color_Coding_Min_Max::Instance()->get_converted_values(ColoringType, controls_data.coloring_cc_min, controls_data.coloring_cc_max);
	controls_data.coloring_cc_min /= 10.0;
	controls_data.coloring_cc_max /= 10.0;
	if(Per_radar_settings::Instance()->ant_height.size() > 0)
		controls_data.per_radar = Per_radar_settings::Instance()->to_msg();

	controls_data.slam_display.color_by_class = get_colorObjByClass();
	controls_data.slam_display.disp_dist_on_cam = get_distOnCam();
	controls_data.slam_display.disp_FS_on_cam = get_FreeSpace_enabled();
	controls_data.slam_display.disp_funnel = get_funnel_enabled();
	controls_data.slam_display.disp_slam_eco_mode = get_slamOnCam_ecoMode();
	controls_data.slam_display.show_classification = true;
	controls_data.slam_display.slam_on_camera = get_slamOnCam();
	controls_data.header.stamp = ros::Time::now();
	controls_data.marker_text_size = marker_text_size;

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

void rosbag_controls_read_callback(const arbe_msgs::arbeGUIsettings::ConstPtr& controls_data)
{
	gui_controls_t gui_controls_data;

	if (rosbag_playing == 0)
		return;

//        cam_euler_alpha = controls_data->cam_euler_alpha;
//        cam_euler_beta = controls_data->cam_euler_beta;
//        cam_euler_gamma = controls_data->cam_euler_gamma;
//        cam_alpha_label->setText( "a ("+QString::number( cam_euler_alpha,'f',1 )+")" );
//        cam_beta_label->setText( "b ("+QString::number( cam_euler_beta,'f',1 )+")" );
//        cam_gamma_label->setText( "g ("+QString::number( cam_euler_gamma,'f',1 )+")" );
//        cam_rel_alpha->setValue(cam_euler_alpha*10);
//        cam_rel_beta->setValue(cam_euler_beta*10);
//        cam_rel_gamma->setValue(cam_euler_gamma*10);
//        rotation_into_R_t_and_calc_cam_projection(cam_euler_alpha,cam_euler_beta,cam_euler_gamma);

	gui_controls_data.mode_index = controls_data->mode;
	gui_controls_data.range_index = controls_data->range;
	gui_controls_data.threshold3d = controls_data->threshold3d;
	gui_controls_data.threshold4d = controls_data->threshold4d;
	gui_controls_data.DynamicAzimuth = controls_data->DynamicAzimuth;
	gui_controls_data.DynamicElevation = controls_data->DynamicElevation;
	viewpanel::update_controls( &gui_controls_data );

	return;
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

arbe_msgs::arbeTSlamObj get_object(uint16_t i)
{
	if (slamMsg)
		return slamMsg->ObjectsBuffer[i];
		}

size_t get_num_objects()
		{
	if (slamMsg)
		return slamMsg->ObjectsBuffer.size();
            else
		return 0;
            }

void slam_display_handler( int n_radar )
			{
	visualization_msgs::MarkerArray slam_boxes;
	visualization_msgs::MarkerArray slam_arrows;
	visualization_msgs::MarkerArray slam_tags;

	// Set our initial shape type to be a cube
	uint32_t shape = visualization_msgs::Marker::CUBE;
	uint32_t arrow_shape = visualization_msgs::Marker::ARROW;
	//visualization_msgs::Marker marker;

	tf2::Quaternion q_orig, q_rot, q_new;

	//ros::NodeHandlePtr n = boost::make_shared<ros::NodeHandle>();
		/* Prepare slam realted markers from the recived slam data */
		if( get_SlamLastPacket() == true && !get_used4display())
		{
			falsify_slamLastPacket(); //SlamLastPacket = false;
			//ROS_DEBUG("Slam -> Objects=%ld",get_num_objects());//ObjectsPerFrame.size());

            int16_t cls2show = get_classes_to_show();
            uint16_t num_bb = 0;
            for(int i = 0, marker_i = 0; i < get_num_objects(); i++)//ObjectsPerFrame.size(); i++)
            {
                if ( cls2show != -1 && cls2show != get_object(i).obj_class)
                    continue;
                num_bb++;
            }

            /* Prepare the markers array that bound detected slam objects */

			slam_boxes.markers.resize(get_num_objects());//ObjectsPerFrame.size());
			slam_arrows.markers.resize(get_num_objects());//ObjectsPerFrame.size());
			slam_tags.markers.resize(get_num_objects());//ObjectsPerFrame.size());

            for(int i = 0, marker_i = 0; i < get_num_objects(); i++)//ObjectsPerFrame.size(); i++)
			{
				arbe_msgs::arbeTSlamObj slam_obj =get_object(i);

                if ( cls2show != -1 && cls2show != slam_obj.obj_class)
                    continue;

                slam_boxes.markers[marker_i].header.frame_id = "image_radar";
                slam_boxes.markers[marker_i].header.stamp = ros::Time::now();
                slam_boxes.markers[marker_i].ns = "radar_object";
                slam_boxes.markers[marker_i].id = slam_obj.ID;
                if (slam_obj.ID==0) slam_boxes.markers[marker_i].id  += i;
                slam_boxes.markers[marker_i].type = shape;
                slam_boxes.markers[marker_i].action = visualization_msgs::Marker::ADD;

				float orientation = slam_obj.bounding_box.orientation;

				/* x,y,z position of a slam object */
				TaregtCartesian object_x;
				object_x.x=slam_obj.position.x;
				object_x.y=slam_obj.position.y;
				object_x.z=slam_obj.position.z;

				tiltTarget(object_x);

			{
				double tmp_x = object_x.x * cos(radar_yaw_angles[n_radar]) + object_x.y * sin(radar_yaw_angles[n_radar]);
				object_x.y = object_x.y * cos(radar_yaw_angles[n_radar]) - object_x.x * sin(radar_yaw_angles[n_radar]);
				object_x.x = tmp_x;
			}


				float local_cart_x, local_cart_y, ego_heading;
				float orientation_f;

				if(get_localization_enable())
				{
					arbe_msgs::arbeTSlamMetadata slam_mdata = get_slam_metadata();
					ego_heading = slam_mdata.HostHeading;
					local_cart_x = slam_mdata.local_catr_x;
					local_cart_y = slam_mdata.local_catr_y;
                    slam_boxes.markers[marker_i].pose.position.x = cos(ego_heading)*object_x.x - sin(ego_heading) * object_x.y + local_cart_x;
                    slam_boxes.markers[marker_i].pose.position.y = cos(ego_heading)*object_x.y + sin(ego_heading) * object_x.x + local_cart_y;
					orientation_f = orientation - ego_heading;
				}
				else
				{
                slam_boxes.markers[marker_i].pose.position.x = object_x.x;
                slam_boxes.markers[marker_i].pose.position.y = object_x.y;
                orientation_f = orientation;
				}

			slam_boxes.markers[marker_i].pose.position.z = object_x.z + radar_z_offset;

//			slam_boxes.markers[marker_i].pose.orientation.x = 0;
//			slam_boxes.markers[marker_i].pose.orientation.y = 0;
//			slam_boxes.markers[marker_i].pose.orientation.z = 0;
//			slam_boxes.markers[marker_i].pose.orientation.w = 1;

				// Get the original orientation of 'commanded_pose'
//			tf2::convert(slam_boxes.markers[marker_i].pose.orientation, q_orig);
			double r=0, p=0, y= M_PI_2 - orientation_f + radar_yaw_angles[n_radar];  // Rotate the previous pose (yaw)
				q_rot.setRPY(r, p, y);
//			q_new = q_rot*q_orig;  // Calculate the new orientation
//			q_new.normalize();
				// Stuff the new rotation back into the pose. This requires conversion into a msg type
			tf2::convert(q_rot, slam_boxes.markers[marker_i].pose.orientation);

				/* Set the scale of the object-- 1x1x1 here means 1m on a side */
                slam_boxes.markers[marker_i].scale.x = slam_obj.bounding_box.scale_y;
                slam_boxes.markers[marker_i].scale.y = slam_obj.bounding_box.scale_x;
                slam_boxes.markers[marker_i].scale.z = slam_obj.bounding_box.scale_z;

				/* Set the color of the object -- be sure to set alpha to something non-zero! */
				float red,green,blue;
                if(get_colorObjByClass())
                {
                    std::string fc_txt;
                    get_class_color(slam_obj.obj_class, red, green, blue, fc_txt);
                }
                else
				Slam_Color::Instance()->get_color(slam_obj.ID,red,green,blue);
                slam_boxes.markers[marker_i].color.r = red;
                slam_boxes.markers[marker_i].color.g = green;
                slam_boxes.markers[marker_i].color.b = blue;
                slam_boxes.markers[marker_i].color.a = 1;

				std::string id_text = std::to_string(slam_obj.ID);
                slam_boxes.markers[marker_i].text = id_text;

                slam_boxes.markers[marker_i].lifetime = ros::Duration(0.1);
                marker_i++;
			}
			/* Add arrows that show the direction of movement of a slam object */
            for(int i = 0, marker_i = 0; i < get_num_objects(); i++) //ObjectsPerFrame.size(); i++)
			{
				arbe_msgs::arbeTSlamObj slam_obj =get_object(i);
                if ( cls2show != -1 && cls2show != slam_obj.obj_class)
                    continue;

                slam_arrows.markers[marker_i] = slam_boxes.markers[marker_i];
                slam_arrows.markers[marker_i].type = arrow_shape;
				float y_vel = slam_obj.velocity.y_dot + ego_velocity;
				// normalized arrow sc = 3 @ 60 km/h + BB size
				float arrow_sc = sqrt(slam_obj.velocity.x_dot*slam_obj.velocity.x_dot + y_vel*y_vel) / 60 * 3.6 * 3;
				arrow_sc = arrow_sc > 3 ? 3 : arrow_sc;
				arrow_sc = arrow_sc < 0.1 ? 0 : arrow_sc;
                slam_arrows.markers[marker_i].scale.x = arrow_sc + slam_obj.bounding_box.scale_y / 2;

                slam_arrows.markers[marker_i].scale.y = 0.2;
                slam_arrows.markers[marker_i].scale.z = 0.2;
                slam_arrows.markers[marker_i].id = slam_obj.ID + 100;
			if (slam_obj.ID==0)
				slam_boxes.markers[marker_i].id  += i;
                slam_arrows.markers[marker_i].action = visualization_msgs::Marker::ADD;
                slam_arrows.markers[marker_i].color.r = 0.0f;
                slam_arrows.markers[marker_i].color.g = 0.0f;
                slam_arrows.markers[marker_i].color.b = 10.0f;
                slam_arrows.markers[marker_i].color.a = 1.0;
                marker_i++;
			}
            for(int i = 0, marker_i = 0; i < get_num_objects(); i++) //ObjectsPerFrame.size(); i++)
			{
				arbe_msgs::arbeTSlamObj slam_obj =get_object(i);
                if ( cls2show != -1 && cls2show != slam_obj.obj_class)
                    continue;

				float scale_text = 2;
                slam_tags.markers[marker_i] = slam_boxes.markers[marker_i];
                slam_tags.markers[marker_i].id = slam_obj.ID + 1000;
                if (slam_obj.ID==0) slam_boxes.markers[marker_i].id  += i;
                slam_tags.markers[marker_i].type = visualization_msgs::Marker::TEXT_VIEW_FACING;;
                slam_tags.markers[marker_i].action = visualization_msgs::Marker::ADD;
                slam_tags.markers[marker_i].pose.position.z += slam_obj.bounding_box.scale_z / 2 + 5 * scale_text/8;
                slam_tags.markers[marker_i].scale.z = scale_text;
                slam_tags.markers[marker_i].color.r = 1.0f;
                slam_tags.markers[marker_i].color.g = 1.0f;
                slam_tags.markers[marker_i].color.b = 1.0f;
                slam_tags.markers[marker_i].color.a = 1.0;
                marker_i++;
			}
		if(slam_displayed == true)
		{
			marker_pub.publish(slam_boxes);
			marker_pub.publish(slam_arrows);
			marker_pub.publish(slam_tags);
		}
	}
}

/* 
   Uses the RAF API for setting radar thresholds and ATC
*/
void Ntc_Mode_Set( void )
{
	TNtcInfo Ntc_Config;
	ROS_DEBUG("ntc: 3d %d 4d %d\n\r",ntc_3d_enabled, ntc_4d_enabled);

	if ( (ntc_4d_enabled == 0) && ( ntc_3d_enabled == 1) )
	{
		QMessageBox msgBox;
		msgBox.setText("NTC command not sent! Illegal NTC settings (3d:on, 4d:off)");
		msgBox.setWindowFlags(msgBox.windowFlags() | Qt::WindowStaysOnTopHint);
		msgBox.exec();
	}
	else
	{
		Ntc_Config.unNTCCoarseOn = ntc_3d_enabled;
		Ntc_Config.unNTCFineOn = ntc_4d_enabled;
		Ntc_Config.unSendMetadata = algo_thr_metadata_enabled;
		RAF_API_SysCfg_NTC(&tArbeApiMailBox,&Ntc_Config);
	}
}

void Cfar_Mode_Set( void )
{
	TCFARInfo Cfar_Config;
	ROS_DEBUG("cfar: 3d %d 4d %d\n\r",cfar_3d_enabled, cfar_4d_enabled);
	Cfar_Config.unCFARCoarseOn = cfar_3d_enabled;
	Cfar_Config.unCFARFineOn = cfar_4d_enabled;
	Cfar_Config.unSendMetadata = algo_thr_metadata_enabled;

	RAF_API_SysCfg_CFAR(&tArbeApiMailBox,&Cfar_Config);
}

/*
  Configure radar pointcloud parameters
*/
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


void radar_set_params()
{
	TSetThresholdsInfo radarThresholds;
	int threshold3d_snr = threshold3d;
	int threshold4d_snr = threshold4d;


	if ( enable_gui == false)
	{
		n->getParam("/arbe_gui/noise_level", noise_level_int);
		n->getParam("/arbe_gui/bias_4d", bias_4d_int);
		threshold4d_snr = threshold4d + noise_level_int;
		threshold3d_snr = threshold4d_snr - bias_4d_int;
		ROS_INFO("noise_level %d bias %d",noise_level_int, bias_4d_int);
	} else {
		threshold4d_snr = threshold4d + noise_level.toInt();
		threshold3d_snr = threshold4d_snr - bias_4d.toInt();
	}

	radarThresholds.opcode = SetStaticAndDynamicThresholds;
	radarThresholds.aunParams[0] = (uint32_t)((float)threshold3d_snr  / POW_TO_DB_RATIO);
	radarThresholds.aunParams[1] = (uint32_t)((float)threshold4d_snr / POW_TO_DB_RATIO);
	radarThresholds.aunParams[2] = (uint32_t)((float)DynamicAzimuth / POW_TO_DB_RATIO);
	radarThresholds.aunParams[3] = (uint32_t)((float)DynamicElevation / POW_TO_DB_RATIO);
	ROS_INFO("Set radar parameters th:3d=%d,4d=%d",threshold3d_snr,threshold4d_snr);
	RAF_API_RdrCtrl_SetThresholds(&tArbeApiMailBox, &radarThresholds );
	pubGUIcontrols();
}

void radar_change_seq()
{
	/* intialize defaults for min and max power to color */

	ROS_DEBUG("Change Seq.");

	TSelectActiveSeqInfo sequenceInfo;

	if ( (strcmp(mode.c_str(), "3d") == 0 ) && ( strcmp(RangeType.c_str(), "Short") == 0 ) ) {
		sequenceInfo.eSequenceType = CoarseShortSeq;
	}
	else if ( (strcmp(mode.c_str(), "3d") == 0 ) && ( strcmp(RangeType.c_str(), "Mid") == 0 ) ) {
		sequenceInfo.eSequenceType = CoarseMidSeq;
	}
	else if ( (strcmp(mode.c_str(), "3d") == 0 ) && ( strcmp(RangeType.c_str(), "Long") == 0 ) ) {
		sequenceInfo.eSequenceType = CoarseLongSeq;
	}
	else if ( (strcmp(mode.c_str(), "3d") == 0 ) && ( strcmp(RangeType.c_str(), "Ultra-Long") == 0 ) ) {
		sequenceInfo.eSequenceType = CoarseUltraLongSeq;
	}
	else if ( (strcmp(mode.c_str(), "4d") == 0 ) && ( strcmp(RangeType.c_str(), "Short") == 0 ) ) {
		sequenceInfo.eSequenceType = FineShortSeq;
	}
	else if ( (strcmp(mode.c_str(), "4d") == 0 ) && ( strcmp(RangeType.c_str(), "Mid") == 0 ) ) {
		sequenceInfo.eSequenceType = FineMidSeq;
	}
	else if ( (strcmp(mode.c_str(), "4d") == 0 ) && ( strcmp(RangeType.c_str(), "Long") == 0 ) ) {
		sequenceInfo.eSequenceType = FineLongSeq;
	}
	else if ( (strcmp(mode.c_str(), "4d") == 0 ) && ( strcmp(RangeType.c_str(), "Ultra-Long") == 0 ) ) {
		sequenceInfo.eSequenceType = FineUltraLongSeq;
	}
	else sequenceInfo.eSequenceType = FineMidSeq; // Default to 4d mid if not defined

	RAF_API_RdrCtrl_SetActiveSeq(&tArbeApiMailBox,&sequenceInfo);
	pubGUIcontrols();
}

void radar_launch_pointcloud_nodes()
{
	char buffer [512];
	for(int n_rdr = 0; n_rdr < n_radars; n_rdr++)
{

		sprintf(buffer,"roslaunch arbe_phoenix_radar_driver arbe_radar_nodes.launch \
		this_radar:=%d yaw_in_degrees:=%d pitch_in_degrees:=%f Radar_X_Offset:=%f Radar_Y_Offset:=%f Radar_Z_Offset:=%f \
		Radar_IP:=%s Ctrl_Port:=%d Data_Port:=%d Logs_Port:=%d Radar_base_freq:=%f Radar_Name:=%s console_level:=%d&",
				n_rdr,yaw_in_degrees[n_rdr],pitch_in_degrees[n_rdr],Radar_X_Offset[n_rdr],Radar_Y_Offset[n_rdr],Radar_Z_Offset[n_rdr],
				radar_ip[n_rdr].c_str(),Ctrl_Port[n_rdr],Data_Port[n_rdr],Logs_Port[n_rdr],radar_base_freq[n_rdr], Radar_Name[n_rdr].c_str(),console_level);
		int ret = system(buffer);
}
	sleep(1);
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

extern QPushButton *radar_start_stop_button;
void radar_read_debug_data( void )
{
	if ( radar_connected == 1 )
	{
		pd = new QProgressDialog( "Radar memory read operation in progress.", "", 0, 100 );
		pd->setWindowModality(Qt::WindowModal);
		pd->setWindowTitle( "Radar Debug Data Read" );
		pd->setCancelButton( NULL );
		pd->show();
		pd->setValue( 0 );
		if ( radar_playing == 1 )
		{
			ROS_DEBUG("Stop radar transmit");
			RAF_API_RdrCtrl_StopTx(&tArbeApiMailBox,&stopInfo);
			radar_start_stop_button->setStyleSheet("color: black");
			radar_start_stop_button->setText("&Start Tx");
		}
		TMemoryOperationInfo memory_info;
		memory_info.eMemoryOperation = ReadMemory;
		memory_info.eSpecialOpcode = DEBUG_INFO;
		memory_info.unAddress = 0xffffffff;
		memory_info.unValueOrTotalLength = 0xffffffff;
		RAF_API_Dbg_MemoryOperation(&tArbeApiMailBox,&memory_info);
	} else {
		QMessageBox msgBox;
		msgBox.setText("Radar is not connected");
		msgBox.exec();
	}
}

void update_3d_car(float x, float y, float yaw)
{
	tf2::Quaternion q_rot;

	zero_point_marker.pose.position.x = x;
	zero_point_marker.pose.position.y = y /*- 2.5*/;
	zero_point_marker.pose.position.z = 0;
	zero_point_marker.pose.orientation.x = 0.0;
	zero_point_marker.pose.orientation.y = 0.0;
	zero_point_marker.pose.orientation.z = 0.0;
	zero_point_marker.pose.orientation.w = 1.0;

	double r=0, p=0;
	q_rot.setRPY(r, p, M_PI + yaw);
	q_rot.normalize();
	// Stuff the new rotation back into the pose. This requires conversion into a msg type
	tf2::convert(q_rot, zero_point_marker.pose.orientation);

	arbe_info_markers.publish(zero_point_marker);

}

void render_3d_car(float x, float y, float yaw)
{
	tf2::Quaternion q_rot;

	/* Prepare the Car marker */
	uint32_t shape = visualization_msgs::Marker::MESH_RESOURCE;
	zero_point_marker.mesh_resource = "package://arbe_phoenix_radar_driver/src/arbe_gui_main/Car.dae";
	zero_point_marker.header.frame_id = "image_radar";
	zero_point_marker.header.stamp = ros::Time::now();
	zero_point_marker.ns = "radar_zero_point_marker";
	zero_point_marker.id = 10000;
	zero_point_marker.type = shape;
	zero_point_marker.action = visualization_msgs::Marker::ADD;

	zero_point_marker.scale.x = 1.0f;
	zero_point_marker.scale.y = 1.0f;
	zero_point_marker.scale.z = 1.0f;
	zero_point_marker.color.r = 1.0f;
	zero_point_marker.color.g = 1.0f;
	zero_point_marker.color.b = 1.0f;
	zero_point_marker.color.a = 1.0;
	zero_point_marker.lifetime = ros::Duration(0);

	update_3d_car(x, y, yaw);
}


/* Prepare the static markerts that are shown on the display grid */
void prepare_basic_markers( void )
{

	/* Prepare the markers that show the number of detections per frame */
	dtections_per_frame_marker.header.frame_id = "image_radar";
	dtections_per_frame_marker.ns = "radar_dtections_per_frame_marker";
	dtections_per_frame_marker.id = 10006;
	dtections_per_frame_marker.type = visualization_msgs::Marker::TEXT_VIEW_FACING;;
	dtections_per_frame_marker.action = visualization_msgs::Marker::ADD;
	dtections_per_frame_marker.pose.position.x = 0;
	dtections_per_frame_marker.pose.position.y = -10;
	dtections_per_frame_marker.pose.position.z = 1;
	dtections_per_frame_marker.pose.orientation.x = 0.0;
	dtections_per_frame_marker.pose.orientation.y = 0.0;
	dtections_per_frame_marker.pose.orientation.z = 0.0;
	dtections_per_frame_marker.pose.orientation.w = 1.0;
	dtections_per_frame_marker.scale.x = marker_text_size;
	dtections_per_frame_marker.scale.y = marker_text_size;
	dtections_per_frame_marker.scale.z = marker_text_size;
	dtections_per_frame_marker.color.r = 1.0f;
	dtections_per_frame_marker.color.g = 1.0f;
	dtections_per_frame_marker.color.b = 1.0f;
	dtections_per_frame_marker.color.a = 1.0;

	/* prepare the markers that show the current grid cell size */
	grid_size_marker_pos.header.frame_id = "image_radar";
	grid_size_marker_pos.ns = "radar_zero_point_marker";
	grid_size_marker_pos.id = 10007;
	grid_size_marker_pos.type = visualization_msgs::Marker::TEXT_VIEW_FACING;;
	grid_size_marker_pos.action = visualization_msgs::Marker::ADD;
	grid_size_marker_pos.pose.position.x = grid_cell_size;
	grid_size_marker_pos.pose.position.y = -0.5;
	grid_size_marker_pos.pose.position.z = 0;
	grid_size_marker_pos.pose.orientation.x = 0.0;
	grid_size_marker_pos.pose.orientation.y = 0.0;
	grid_size_marker_pos.pose.orientation.z = 0.0;
	grid_size_marker_pos.pose.orientation.w = 1.0;
	grid_size_marker_pos.scale.x = marker_text_size;
	grid_size_marker_pos.scale.y = marker_text_size;
	grid_size_marker_pos.scale.z = marker_text_size;
	grid_size_marker_pos.color.r = 1.0f;
	grid_size_marker_pos.color.g = 1.0f;
	grid_size_marker_pos.color.b = 1.0f;
	grid_size_marker_pos.color.a = 1.0;
	grid_size_marker_pos.lifetime = ros::Duration(1);

	grid_size_marker_neg.header.frame_id = "image_radar";
	grid_size_marker_neg.ns = "radar_zero_point_marker";
	grid_size_marker_neg.id = 10008;
	grid_size_marker_neg.type = visualization_msgs::Marker::TEXT_VIEW_FACING;;
	grid_size_marker_neg.action = visualization_msgs::Marker::ADD;
	grid_size_marker_neg.pose.position.x = -grid_cell_size;
	grid_size_marker_neg.pose.position.y = -0.5;
	grid_size_marker_neg.pose.position.z = 0;
	grid_size_marker_neg.pose.orientation.x = 0.0;
	grid_size_marker_neg.pose.orientation.y = 0.0;
	grid_size_marker_neg.pose.orientation.z = 0.0;
	grid_size_marker_neg.pose.orientation.w = 1.0;
	grid_size_marker_neg.scale.x = marker_text_size;
	grid_size_marker_neg.scale.y = marker_text_size;
	grid_size_marker_neg.scale.z = marker_text_size;
	grid_size_marker_neg.color.r = 1.0f;
	grid_size_marker_neg.color.g = 1.0f;
	grid_size_marker_neg.color.b = 1.0f;
	grid_size_marker_neg.color.a = 1.0;
	grid_size_marker_neg.lifetime = ros::Duration(1);
}

void update_basic_markers( void )
{
	/* Update basic markers */
	grid_size_marker_pos.scale.x = marker_text_size;
	grid_size_marker_pos.scale.y = marker_text_size;
	grid_size_marker_pos.scale.z = marker_text_size;
	grid_size_marker_neg.scale.x = marker_text_size;
	grid_size_marker_neg.scale.y = marker_text_size;
	grid_size_marker_neg.scale.z = marker_text_size;

	std::stringstream grid_size_text_pos;
	grid_size_text_pos << (uint16_t)grid_cell_size << "m";
	grid_size_marker_pos.text = grid_size_text_pos.str();
	grid_size_marker_pos.pose.position.x = grid_cell_size;
	arbe_info_markers.publish(grid_size_marker_pos);
	std::stringstream grid_size_text_neg;
	grid_size_text_neg << (int16_t)-grid_cell_size << "m";
	grid_size_marker_neg.text = grid_size_text_neg.str();
	grid_size_marker_neg.pose.position.x = -grid_cell_size;
	arbe_info_markers.publish(grid_size_marker_neg);
}

static float heading = 0;
static bool prev_localization = false;
static bool side_slams_on = true;
#define VELOCITY_TH 0.25 // m/s

void trigger_side_slam(float egovel)
{
	if(egovel < VELOCITY_TH && !side_slams_on)
	{
		side_slams_on = true;
		std_msgs::Bool trigger;
		trigger.data = true;
		trigger_side_slam_pub.publish(trigger);
	}
	else if(egovel >= VELOCITY_TH*1.1 && side_slams_on)
	{
		side_slams_on = false;
		std_msgs::Bool trigger;
		trigger.data = false;
		trigger_side_slam_pub.publish(trigger);
	}
}

void master_slam_read_callback(const arbe_msgs::arbeSlamMsg::ConstPtr& msg)
{
	trigger_side_slam(msg->meta_data.HostVelocity);
	if (enable_gui == true)
	{
		if(get_localization_enable())
		{
			prev_localization = true;
			float local_cart_x = msg->meta_data.local_catr_x;
			float local_cart_y = msg->meta_data.local_catr_y;
			float headingUnc = msg->meta_data.HostHeadingUnc;
			set_local_cart(msg);
			if(headingUnc > 0)
			{
				heading = msg->meta_data.HostHeading;
			}
			if(viewpanel::Instance()->get_3rd_prs())
				viewpanel::Instance()->setFocalPointAndHeading(local_cart_x, local_cart_y, heading);

			Eigen::Affine3f transform = Eigen::Affine3f::Identity();

			transform.rotate(Eigen::AngleAxisf(heading, Eigen::Vector3f::UnitZ()));

			Eigen::Vector3f car_center(0, -2.5, 0);

			car_center = transform * car_center;

			update_3d_car(car_center[0] + local_cart_x, car_center[1] + local_cart_y, heading);
		}
		else
		{
			if(prev_localization)
				update_3d_car(0, -2.5, 0);
			prev_localization = false;
			if(viewpanel::Instance()->get_3rd_prs())
			{
				viewpanel::Instance()->rotate_view();
			}
		}
		if (enable_gui == true)
			pub_egoVel_cb(msg->meta_data.HostVelocity,msg->meta_data.HostOmega);
	}
}


void slam_read_callback(const arbe_msgs::arbeSlamMsg::ConstPtr& array, int n_radar)
{
	int recv_size = 0;
	TSlamObj* SlamObject;
	TSlamHeader_V1_1 SlamHeader;

	g_objects_mutex.lock();
	slamMsg = arbe_msgs::arbeSlamMsg::ConstPtr(array);
	g_objects_mutex.unlock();

	set_host_vel(slamMsg->meta_data.HostVelocity);
	host_w_z = slamMsg->meta_data.HostOmega;
	if (enable_gui == true)
		pub_egoVel_cb(get_host_vel(),host_w_z);

	truesify_slamLastPacket();

	flag_notused4display();

	slam_display_handler(n_radar);
	}

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

int getkey(void)
{
	int readKey = 0;
	static struct termios oldTermios, newTermios;
	tcgetattr(STDIN_FILENO, &oldTermios);
	newTermios = oldTermios;
	cfmakeraw(&newTermios);
	tcsetattr(STDIN_FILENO, TCSANOW, &newTermios);
	readKey = getchar();
	tcsetattr(STDIN_FILENO, TCSANOW, &oldTermios);
	return readKey;
}

int _kbhit() {
    static const int STDIN = 0;
    static bool initialized = false;

    if (! initialized) {
        // Use termios to turn off line buffering
        termios term;
        tcgetattr(STDIN, &term);
        term.c_lflag &= ~ICANON;
        tcsetattr(STDIN, TCSANOW, &term);
        setbuf(stdin, NULL);
        initialized = true;
    }

    int bytesWaiting;
    ioctl(STDIN, FIONREAD, &bytesWaiting);
    return bytesWaiting;
}

/* Main function */
int main(int argc, char **argv)
{
	pthread_t playback_thread_id;
	int ret;
	ros::init(argc, argv, "lidar_gui");
	ros::Time::init();
	ros::NodeHandlePtr pLidarNodeHandle = boost::make_shared<ros::NodeHandle>();

#if  ONLY_SHOW_UI
	arbe_info_markers = n->advertise<visualization_msgs::Marker>("/arbe/rviz/car_marker", 1);
	n->getParam("/arbe_gui/console_level", console_level);
	ros::console::levels::Level CL = ros::console::levels::Warn;
	if(console_level<ros::console::levels::Count && console_level>=ros::console::levels::Debug)
		CL = (ros::console::levels::Level)console_level;
	if (ros::console::set_logger_level(ROSCONSOLE_DEFAULT_NAME, CL))//Debug))
	{
		ros::console::notifyLoggerLevelsChanged();
	}

	arbe_BuildNum_pub = n->advertise<arbe_msgs::arbeBuildNumber>("/arbe/settings/build_num",1,true);

	getBuildNum();

	arbe_ctrl_pub = n->advertise<std_msgs::UInt8MultiArray>("/arbe/raw/ctrl_status",10);
	arbe_gui_commands_pub = n->advertise<std_msgs::String>("/arbe/settings/gui_commands", 1);
	trigger_side_slam_pub = n->advertise<std_msgs::Bool>("/arbe/settings/trigger_side_looking_slam", 1);

	pcl_pub = n->advertise<sensor_msgs::PointCloud2>("/arbe/rviz/pointcloud_0",1);
	stationary_pcl_pub = n->advertise<sensor_msgs::PointCloud2>("/arbe/rviz/stationary_pointcloud_0",1);
	marker_pub = n->advertise<visualization_msgs::MarkerArray>("/arbe/rviz/objects", 1);

#endif

/*   gui  */

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

	QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());

#endif
#if 1
	MainWindow mainWin;

	const QRect availableGeometry = QApplication::desktop()->availableGeometry(&mainWin);
	mainWin.resize(availableGeometry.width() / 2, availableGeometry.height() * 2 / 3);
	mainWin.move((availableGeometry.width() - mainWin.width()) / 2, (availableGeometry.height() - mainWin.height()) / 2);
	mainWin.show();
	return app.exec();
#endif


#if 0
	ctrl_ip[0] = settings.value("radar ip").toString();
	ctrl_port[0] = settings.value("radar port").toString();
	data_port[0] = settings.value("data port").toString();
	logs_port[0] = settings.value("logs port").toString();

	ctrl_ip[1] = "10.20.30.41";
	ctrl_port[1] = "6002";
	data_port[1] = "6004";
	logs_port[1] = "6011";
#endif


#if ONLY_SHOW_UI   //gui test
	/* Radar parameters */
	n->getParam("/arbe_gui/Mode", mode);
	n->getParam("/arbe_gui/RangeType", RangeType);
	n->getParam("/arbe_gui/ColoringType", ColoringType);
	n->getParam("/arbe_gui/Threshold4d", threshold4d);
	n->getParam("/arbe_gui/DynamicAzimuth", DynamicAzimuth);
	n->getParam("/arbe_gui/DynamicElevation", DynamicElevation);
	n->getParam("/arbe_gui/MinDoppler", MinDoppler);
	n->getParam("/arbe_gui/MaxDoppler", MaxDoppler);
	n->getParam("/arbe_gui/RecordFileName", RecordFileName);

	n_radars = 1;
	n->getParam("/arbe_gui/n_radars", n_radars);
	Per_radar_settings::Instance(n_radars);
	Per_radar_settings::Instance()->resize(n_radars);
	for(int i =0 ; i<n_radars; i++)
	{
		Per_radar_settings::Instance()->threshold_4d.at(i) = threshold4d;
	}

//	float ang;
	std::vector<int> camera_id;

	bool resu= n->getParam("/arbe_gui/yaw_in_degrees", yaw_in_degrees);
	resu= n->getParam("/arbe_gui/pitch_in_degrees", pitch_in_degrees);
	resu= n->getParam("/arbe_gui/Radar_base_freq", radar_base_freq);
	resu= n->getParam("/arbe_gui/Radar_X_Offset", Radar_X_Offset);
	resu= n->getParam("/arbe_gui/Radar_Y_Offset", Radar_Y_Offset);
	resu= n->getParam("/arbe_gui/Radar_Z_Offset", Radar_Z_Offset);
	resu= n->getParam("/arbe_gui/Radar_Name", Radar_Name);
	resu= n->getParam("/arbe_gui/Radar_IP", radar_ip);
	resu= n->getParam("/arbe_gui/Ctrl_Port", Ctrl_Port);
	resu= n->getParam("/arbe_gui/Data_Port", Data_Port);
	resu= n->getParam("/arbe_gui/Logs_Port", Logs_Port);
	resu= n->getParam("/arbe_gui/moment_length_in_m", moment_length_in_m);
	resu= n->getParam("/arbe_gui/camera_id", camera_id);
	resu= n->getParam("/arbe_gui/camera_calib_file", camera_calib_file);
	if(n_radars!=yaw_in_degrees.size() || n_radars!=pitch_in_degrees.size() ||
			n_radars!=Radar_X_Offset.size() || n_radars!=Radar_Y_Offset.size() ||n_radars!=Radar_Z_Offset.size()
			|| n_radars!=Radar_Name.size() || n_radars!=radar_ip.size() || n_radars!=Ctrl_Port.size() || n_radars!=Data_Port.size()
			|| n_radars!=Logs_Port.size() || n_radars!=moment_length_in_m.size() ||
			n_radars!=camera_id.size() || n_radars!=camera_calib_file.size()|| n_radars!=radar_base_freq.size())
	{
		ROS_WARN("ERROR! Please Fix launch file params!");
		ret = 	system("rosnode kill --all");
		return -1;
	}
	int err = read_camera_calibration_from_file(camera_calib_file[0].c_str(), true);
	if(err <0)
		ROS_WARN("Warning! Please Fix camera calibration file!");
	else
		ROS_INFO("camera calibrated!");

	Per_radar_settings::Instance()->ant_height = Radar_Z_Offset;
//	Per_radar_settings::Instance()->ant_pitch = pitch_in_degrees;
	for(int ant =0; ant<pitch_in_degrees.size();ant++)
	{
		Per_radar_settings::Instance()->ant_pitch.at(ant) = pitch_in_degrees[ant] * M_PI/180.0 ;
	}


	for(int n_rdr = 0; n_rdr < n_radars; n_rdr++)
	{
		//		ang = 0;
		//		bool res = n->getParam("/arbe_gui/yaw_in_degrees_"+std::to_string(n_rdr), ang);
		radar_yaw_angles.push_back(yaw_in_degrees[n_rdr]*3.1415/180);

		//Launch SLAM+FS
		char buffer [512];
			sprintf(buffer,"roslaunch arbe_phoenix_radar_driver arbe_SLAM.launch \
					n_radars:=%d yaw_in_degrees:=%d this_radar:=%d moment_length_in_m:=%f pitch_in_degrees:=%f Radar_Z_Offset:=%f Radar_Y_Offset:=%f &",
					n_radars,yaw_in_degrees[n_rdr],n_rdr, moment_length_in_m[n_rdr], pitch_in_degrees[n_rdr], Radar_Z_Offset[n_rdr], Radar_Y_Offset[n_rdr]-Radar_Y_Offset[0]);
					ret = system(buffer);
		}

	radar_launch_pointcloud_nodes();
	char class_buffer [512];
	sprintf(class_buffer,"roslaunch arbe_phoenix_radar_driver arbe_classification.launch n_radars:=%d &",n_radars);
	ret = system(class_buffer);
#endif

#if 0
	ROS_INFO("Ctrl IP = %s", ctrl_ip[0].toStdString().c_str());
	ROS_INFO("Ctrl Port = %d", ctrl_port[0].toInt());
	ROS_INFO("Data Port = %d", data_port[0].toInt());
	ROS_INFO("Logs Port = %d", logs_port[0].toInt());
#endif


#if ONLY_SHOW_UI   //gui test

	/* Initialize API mailbox */
	tArbeApiMailBox.sendCmd = TransmitBuffer;
	tArbeApiMailBox.sysCfg_GetSystemTimeMs = GetSystemTime;

	/* spawn the unity playback thread */
	//ret=pthread_create(&playback_thread_id,NULL,&playback_thread,NULL);

	/* start the GUI controls subscriber */
	old_gui_controls_sub = n->subscribe("/arbe_gui_controls", 1, rosbag_controls_read_callback);
	gui_controls_sub = n->subscribe("/arbe/settings/gui_controls", 1, rosbag_controls_read_callback);

	/* start the serial readback subscriber */
	old_arbe_serial_sub = n->subscribe("/arbe_serial_console", 1, serial_read_callback);
	arbe_serial_sub = n->subscribe("/arbe/raw/serial_console", 1, serial_read_callback);

	/* Create a ROS subscriber for the input point cloud */
	selected_points_sub = n->subscribe ("/arbe/rviz/selected_points", 1, process_selected_points_callback);
	arbe_debug_data_count_sub = n->subscribe ("/arbe/raw/debug_progress", 1, debug_data_read_callback);

	/* Create a ROS publisher for GUI settings send to the radar nodes */
	arbe_controls_pub = n->advertise<arbe_msgs::arbeGUIsettings>("/arbe/settings/gui_controls",1,true);

	initImageSubPub(n);	 // initialize image sub pub for bounding boxes in display
	init_bookmark_SubPub(n); // initialize bookmarking sub pub
    init_speed_pub(n);	 // Initialize speed and trunrate publishers

	master_slam_sub = n->subscribe<arbe_msgs::arbeSlamMsg>("/arbe/processed/objects/0", 1, master_slam_read_callback);
	for(int nr = 0; nr < n_radars; nr++)
	{
//		targets_subscribers.push_back(n->subscribe<arbe_msgs::arbeNewPcMsg>(
//										  "/arbe/processed/pointcloud/"+std::to_string(nr), 1, boost::bind(targets_read_callback, _1, nr)));
//		objects_subscribers.push_back(n->subscribe<arbe_msgs::arbeSlamMsg>(
//										  "/arbe/processed/objects/"+std::to_string(nr), 1, boost::bind(slam_read_callback, _1, nr)));
	}

	/* Initialize Rviz markers (car, grid size etc.) */
	prepare_basic_markers();

	/* Create a houskeeping timer function. runs every 500msec */
	ros::Timer timer = n->createTimer(ros::Duration(0.5), timerCallback);
	n->getParam("/arbe_gui/enable_gui", enable_gui);
	if (enable_gui == true)
	{
		/* Prepare and launch the GUI main application */
		MainWindow mainWin;

		const QRect availableGeometry = QApplication::desktop()->availableGeometry(&mainWin);
		mainWin.resize(availableGeometry.width() / 2, availableGeometry.height() * 2 / 3);
		mainWin.move((availableGeometry.width() - mainWin.width()) / 2, (availableGeometry.height() - mainWin.height()) / 2);

		mainWin.show();
		return app.exec();
	} else
	{
		char readKey;
		char secondKey;
		sleep(5);

		/* Enable NTC and CFAR using defaults (off, on, on, off) */
		radar_set_params();
		radar_change_seq();

		Ntc_Mode_Set();
		Cfar_Mode_Set();
		sleep(1);

		/* Enable phase data in pointcloud */
		phase_enabled = true;

		radar_start_transmit();

		ros::Rate loop_rate(30);
		while (ros::ok() && (!terminating))
		{
			if (_kbhit())
			{
				readKey = getkey();
				switch(readKey)
				{
					case '/': 
						ROS_WARN("here is /");
						secondKey = getkey();
						switch(secondKey)
						{
							case 'q':
								radar_quit();
								break;
						}
					break;
				}
			}
			fflush(stdout);
			ros::spinOnce();
			loop_rate.sleep();
		}
	}
#endif
}
