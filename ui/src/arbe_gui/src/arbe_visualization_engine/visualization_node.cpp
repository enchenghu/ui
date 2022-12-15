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

#include "sensor_msgs/CompressedImage.h"
#include "sensor_msgs/Image.h"
#include <image_transport/image_transport.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>
#include <ros/ros.h>
#include <QDebug>
#include "arbe_msgs/arbeNewPcMsg.h"
#include <arbe_msgs/arbeGUIsettings.h>
#include <arbe_msgs/arbeSlamDisplaySettings.h>
#include <arbe_msgs/arbeCameraInstallationParams.h>
#include <arbe_msgs/arbeSlamMsg.h>
#include <arbe_msgs/arbeRdInclination.h>
#include <std_msgs/Float32MultiArray.h>
#include <std_msgs/Float32.h>
#include <std_msgs/Bool.h>
#include <geometry_msgs/PolygonStamped.h>
#include <QSettings>
#include <visualization_msgs/MarkerArray.h>
#include <tf2_geometry_msgs/tf2_geometry_msgs.h>
#include "types.hpp"
#include "Slam_color.hpp"
#include "Pointcloud_coloring.hpp"
#include "vis_utils.hpp"
#include "arbe_msgs/arbeBoolWithTime.h"
#include "visualization_node.h"


static arbe_msgs::arbeNewPcMsg::ConstPtr PointCloudMsg;
static arbe_msgs::arbeNewPcMsg::ConstPtr StationaryPointCloudMsg;
static arbe_msgs::arbeSlamDisplaySettings slamDisplaySettings;

ros::Subscriber pc_frame_sub;
ros::Publisher arbe_pc_frame_pub;

ros::Subscriber gui_commands_sub;
ros::Subscriber targets_sub;
ros::Subscriber targets_legacy_sub;
ros::Subscriber stationary_targets_sub;
ros::Subscriber objects_sub;
ros::Subscriber master_slam_sub;
ros::Subscriber gui_controls_sub;
ros::Subscriber slam_active_sub;
ros::Subscriber fs_display_sub;
ros::Subscriber camera_params_sub;
ros::Subscriber FS_road_inclination_sub;
ros::Subscriber installation_error_fix_sub;
ros::Subscriber restore_defaults_sub;
ros::Subscriber disp_FS_on_pc_sub;
ros::Subscriber floating_text_angle_sub;
ros::Subscriber trigger_side_slam_sub;

ros::Publisher arbe_bin_detections_pub;
ros::Publisher arbe_slam_pub;
//ros::Publisher arbe_radar_calibration_pub;
ros::Publisher arbe_pcl_pub;
ros::Publisher stationary_pcl_pub;
ros::Publisher marker_pub;
ros::Publisher fs_poly_pub;
ros::Publisher  slam_enable_pub;
ros::Publisher  free_space_enable_pub;

static ros::Subscriber raw_cam_sub;
static ros::Subscriber gige_cam_sub;
static ros::Subscriber raw_cam_legacy_sub;
static ros::Subscriber gige_cam_legacy_sub;
static ros::Publisher  arbe_capture_pub;
static ros::Publisher  arbe_info_markers;

static  geometry_msgs::PolygonStamped FS_display_polygon;
static Eigen::Affine3f camera_transform = Eigen::Affine3f::Identity();
static Eigen::Affine3f pcl_transform = Eigen::Affine3f::Identity();
static arbe_msgs::arbeSlamMsg::ConstPtr slamMsg;
static arbe_msgs::arbeSlamMsg::ConstPtr masterSlamMsg;

static bool is_slam_frame_available = false;
static bool is_master_slam_frame_available = false;
static bool is_pc_frame_available = false;
static bool valid_stationary_pc = false;
static bool is_cam_frame_available = false;
static bool is_slam_active = false;
static bool floating_text_enabled = false;
static bool expunge_text = false;
static bool enable_gui = true;

static sRdInclination rd_inc;


visualization_msgs::MarkerArray slam_boxes;
visualization_msgs::MarkerArray slam_arrows;
visualization_msgs::MarkerArray slam_tags;
visualization_msgs::Marker dtections_per_frame_marker;


void calc_transform_matrix();
void set_colorObjByClass(bool flag);
int16_t get_classes_to_show();
void fs_display_handler();
void reset_fps_calc();
void one_color_set();

float get_egoVel()
{
	if(get_slam_valid())
		return slamMsg->meta_data.HostVelocity;
	return 0;
}
bool get_hostHeading(float &heading)
{
	heading = 0;
	if(get_slam_valid() && slamMsg->meta_data.HostHeadingUnc)
		heading = slamMsg->meta_data.HostHeading;
		return true;
	return false;
}

std::string ColoringType = "Elevation";

void *radar_ethernet_logger_thread(void* args);
void pointcloud_display_handler (bool also_stationary_cloud);
void targets_read_callback(const arbe_msgs::arbeNewPcMsg::ConstPtr& pcMsg, int pc_type);
void slam_read_callback(const arbe_msgs::arbeSlamMsg::ConstPtr& msg);

static bool disp_processed_pc = false;
bool get_disp_processed_pc()
{
    return disp_processed_pc;
}
static bool triggered_slam = true;
static bool set_disp_run_once = false;
void set_disp_processed_pc(bool flag)
{

	if ( (disp_processed_pc == flag) && set_disp_run_once )
		return;

	set_disp_run_once = true;
	disp_processed_pc = flag;

	ros::NodeHandle n("~");
	if(disp_processed_pc)
	{
		stationary_targets_sub = n.subscribe<arbe_msgs::arbeNewPcMsg>("/arbe/processed/stationary_pc/"+std::to_string(arg_radar_id), 1, boost::bind(targets_read_callback, _1, PCT_PROCESSED_STATIONARY));
		targets_sub            = n.subscribe<arbe_msgs::arbeNewPcMsg>("/arbe/processed/pointcloud/"+std::to_string(arg_radar_id), 1, boost::bind(targets_read_callback, _1, PCT_PROCESSED_DYN));
	}
	else
	{
		targets_sub = n.subscribe<arbe_msgs::arbeNewPcMsg>("/arbe/parsed/pointcloud/"+std::to_string(arg_radar_id), 1, boost::bind(targets_read_callback, _1, PCT_PARSED));
		if(arg_radar_id == 0)
			targets_legacy_sub = n.subscribe<arbe_msgs::arbeNewPcMsg>("/arbe/parsed/pointcloud", 1, boost::bind(targets_read_callback, _1, PCT_PARSED));

		stationary_targets_sub.shutdown();
		stationaty_output.header.stamp = ros::Time::now();
		pcl::toROSMsg(stationary_pc,stationaty_output);
		stationaty_output.header.frame_id = "image_radar";
		stationary_pcl_pub.publish(stationaty_output);
		stationary_pc.clear();
	}
}

void clear_slam_markers()
{
	slam_boxes.markers.clear();
	slam_arrows.markers.clear();
	slam_tags.markers.clear();
}

static bool disp_objects = false;
void set_disp_objects(bool flag)
{
	if (disp_objects == flag)
		return;

	disp_objects = flag;
	ros::NodeHandle n("~");
	if(disp_objects)
	{
		objects_sub = n.subscribe("/arbe/processed/objects/"+std::to_string(arg_radar_id), 1, slam_read_callback);
	}
	else
	{
		clear_slam_markers();
		objects_sub.shutdown();
		set_slam_valid(false);
		ego_velocity = 0;
		hostHeadingUnc = -1;
		hostHeading = 0;
	}
}

void gui_controls_callback(const arbe_msgs::arbeGUIsettings::ConstPtr& controls_data)
{
	ROS_DEBUG("Set coloring to %s",controls_data->ColoringType.c_str());
	ColoringType = controls_data->ColoringType;

	ROS_DEBUG("Set MinDoppler to %lf",controls_data->MinDoppler);
	MinDoppler = controls_data->MinDoppler;

	ROS_DEBUG("Set MaxDoppler to %lf",controls_data->MaxDoppler);
	MaxDoppler = controls_data->MaxDoppler;

	ROS_DEBUG("Set Color_Coding_Min_Max min:%lf max:%lf",controls_data->coloring_cc_min, controls_data->coloring_cc_max);
	Color_Coding_Min_Max::Instance()->set_min(controls_data->ColoringType, controls_data->coloring_cc_min);
	Color_Coding_Min_Max::Instance()->set_max(controls_data->ColoringType, controls_data->coloring_cc_max);

	ROS_DEBUG("Set discardOutOfElContext %d",controls_data->discardOutOfElContext);
	set_discard_out_of_el_context(controls_data->discardOutOfElContext);

	ROS_DEBUG("Set disp_processed_pc %d",controls_data->disp_processed_pc);
	set_disp_processed_pc(controls_data->disp_processed_pc);

	ROS_DEBUG("Set disp_objects %d",controls_data->disp_slam);
	set_disp_objects(controls_data->disp_slam);

	ROS_DEBUG("Set noise_level_db to %d",controls_data->noise_level_db);
	noise_level_db = controls_data->noise_level_db;

	ROS_DEBUG("Set selectedAzimuthBin to %d",controls_data->selectedAzimuthBin);
	selectedAzimuthBin = controls_data->selectedAzimuthBin;

	ROS_DEBUG("Set localization_active to %d",controls_data->selectedAzimuthBin);
	is_localization_active = controls_data->localization_active;

	ROS_DEBUG("Set aggregation_active to %d",controls_data->selectedAzimuthBin);
	is_aggregation_active = controls_data->aggregation_active;

	ROS_DEBUG("NUM RADARS in MSG %d",controls_data->per_radar.n_radars);

	marker_text_size = controls_data->marker_text_size;

	if(controls_data->per_radar.n_radars > arg_radar_id)
	{
		bool recalc = false;
		ROS_DEBUG("Set pitch to %f",controls_data->per_radar.ant_pitch[arg_radar_id]);
		if(radar_pitch_angle != controls_data->per_radar.ant_pitch[arg_radar_id])
		{
			radar_pitch_angle = controls_data->per_radar.ant_pitch[arg_radar_id];
			recalc = true;
		}

		ROS_DEBUG("Set pitch to %f",controls_data->per_radar.ant_pitch[arg_radar_id]);
		if(radar_z_offset != controls_data->per_radar.ant_height[arg_radar_id])
		{
			radar_z_offset = controls_data->per_radar.ant_height[arg_radar_id];
			recalc=true;
		}
		if(recalc)
		calc_transform_matrix();

		if(floating_text_enabled && (controls_data->per_radar.radar_for_text == -1 || is_localization_active))
			expunge_text = true;
		bool prev_floating_text = floating_text_enabled;
		floating_text_enabled = arg_radar_id == controls_data->per_radar.radar_for_text;
		if (!prev_floating_text && floating_text_enabled)
			reset_fps_calc();

	}

    //is_slam_active = controls_data->slam_is_active;
	if(ros::Time::now().toSec()-controls_data->header.stamp.toSec()<3) //ignore recorded cmd
		slamDisplaySettings = controls_data->slam_display;

	set_colorObjByClass(controls_data->color_obj_by_class);

//	printf("***************************************\r\n");
//	printf("*                                     *\r\n");
//	printf("*           GUI CONTROLS MSG          *\r\n");
//	printf("*                                     *\r\n");
//	printf("***************************************\r\n");
	return;
}


void slam_enable_callback(const arbe_msgs::arbeBoolWithTimePtr& enableSlamMsg)
{
	if(ros::Time::now().toSec()-enableSlamMsg->header.stamp.toSec()<3)
	{
		is_slam_active = enableSlamMsg->flag;
		if(is_slam_active == false)
		{
			clear_slam_markers();
		}
	}
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

static bool FS_in_use = false;
static bool transformed_FS_in_use = false;
static geometry_msgs::PolygonStamped transformed_FS_polygon;

void FS_disp_CB(const geometry_msgs::PolygonStamped::ConstPtr& FS_disp)
{
	if(!FS_in_use)
		FS_display_polygon = geometry_msgs::PolygonStamped(*FS_disp);
	if(!transformed_FS_in_use)
	{
		transformed_FS_polygon = geometry_msgs::PolygonStamped(FS_display_polygon);
		fs_display_handler();
	}

}

static bool fs_from_gui = true;
void remove_fs_polygon()
{
	transformed_FS_polygon.polygon.points.clear();
	fs_poly_pub.publish(transformed_FS_polygon);
}
void choose_fs_disp_callback(const std_msgs::Bool::ConstPtr & msg)
{
	fs_from_gui = msg->data;
	transformed_FS_in_use = !msg->data;
	if(transformed_FS_in_use)
	{
		remove_fs_polygon();
	}
}

static float prj[3][4] = {{1526.97,0,934.05,18.68},
						  {0,1533.03,537.37,133.39},
						  {0,0,1,0.02}};
void prj_receive_CB(const std_msgs::Float32MultiArray::ConstPtr &msg)
{
	for(uint8_t i = 0; i < 3; i++)
		for(uint8_t j = 0; j < 4; j++)
		{
			prj[i][j] = msg->data[4*i + j];
		}
}

bool projectFS(geometry_msgs::PolygonStamped &fs_display, float scale,std::vector<cv::Point> &polygons)
{
	cv::Point pt;
	for (uint16_t i = 0; i < fs_display.polygon.points.size(); i++){
		geometry_msgs::Point32 point = fs_display.polygon.points.at(i);

		float xzyw[4] = {point.x,-point.z,point.y,1};
		float out[3] = {0,0,0};

//		Eigen::Vector3f ptx(point.x,-point.z,point.y);
//		ptx = camera_transform * ptx;
//		pt = cv::Point(ptx[0]/ptx[2]*scale, ptx[1]/ptx[2]*scale);

		for(uint16_t out_dim = 0; out_dim < 3; out_dim++){
			for(uint16_t inner_dim = 0; inner_dim < 4; inner_dim++){
				out[out_dim] += prj[out_dim][inner_dim] * xzyw[inner_dim] ;
			}
		}

		pt = cv::Point(out[0]/out[2]*scale, out[1]/out[2]*scale);
		polygons.push_back(pt);
	}

}

static Eigen::Matrix3f intrinsic;
void calc_camera_intrinsic(const arbe_msgs::arbeCameraInstallationParams::Ptr& msg)
{
//	intrinsic << 1526.97,       0, 934.05,
//				 0,       1533.03, 537.37,
//				 0,             0,      1;
	intrinsic << msg->intrinsic[0],msg->intrinsic[3],msg->intrinsic[6],
				 msg->intrinsic[1],msg->intrinsic[4],msg->intrinsic[7],
				 msg->intrinsic[2],msg->intrinsic[5],msg->intrinsic[8];
}
static Eigen::Affine3f extrinsic = Eigen::Affine3f::Identity();
void calc_camera_extrinsic(const arbe_msgs::arbeCameraInstallationParams::Ptr& msg)
{
//	extrinsic.translation() << 0, 0.08, 0.02;
	extrinsic.translation() << msg->extrinsic_trans[0], msg->extrinsic_trans[1], msg->extrinsic_trans[2];

	extrinsic.rotate(Eigen::AngleAxisf(msg->euler_a_b_g[0], Eigen::Vector3f::UnitX()));
	extrinsic.rotate(Eigen::AngleAxisf(msg->euler_a_b_g[1], Eigen::Vector3f::UnitZ()));
	extrinsic.rotate(Eigen::AngleAxisf(msg->euler_a_b_g[2], Eigen::Vector3f::UnitX()));
}
arbe_msgs::arbeCameraInstallationParams::Ptr cam_transform_defaults()
{
	arbe_msgs::arbeCameraInstallationParams::Ptr cam_params(new arbe_msgs::arbeCameraInstallationParams());
	float intr[] = {1526.97,      0, 934.05,
					0,      1533.03, 537.37,
					0,            0,      1};
	std::vector<float> intrinsic_vec (intr, intr + sizeof(intr) / sizeof(float) );

	float trans[] = {0, 0.08, 0.02};
	std::vector<float> translation (trans, trans + sizeof(trans) / sizeof(float) );

	float eul[] = {0, 0, 0};
	std::vector<float> euler (eul, eul + sizeof(eul) / sizeof(float) );

	cam_params->intrinsic = intrinsic_vec;
	cam_params->extrinsic_trans = translation;
	cam_params->euler_a_b_g = euler;
	return cam_params;

}

void calc_camera_transform(arbe_msgs::arbeCameraInstallationParams::Ptr& msg)
{
	calc_camera_intrinsic(msg);
	calc_camera_extrinsic(msg);
	camera_transform = intrinsic * extrinsic;
}

void calc_camera_transform_CB(const arbe_msgs::arbeCameraInstallationParams::ConstPtr& msg)
{
	arbe_msgs::arbeCameraInstallationParams::Ptr cam_params(new arbe_msgs::arbeCameraInstallationParams());
	cam_params->intrinsic = msg->intrinsic;
	cam_params->extrinsic_trans = msg->extrinsic_trans;
	cam_params->euler_a_b_g = msg->euler_a_b_g;

	calc_camera_intrinsic(cam_params);
	calc_camera_extrinsic(cam_params);
	camera_transform = intrinsic * extrinsic;
}

static cv::Point project(float X, float Y, float Z, float scale, float &cam_z){
	float xzyw[4] = {X,-Z,Y,1};
	float out[3] = {0,0,0};

	for(uint8_t out_dim = 0; out_dim < 3; out_dim++){
		for(uint8_t inner_dim = 0; inner_dim < 4; inner_dim++){
			out[out_dim] += prj[out_dim][inner_dim] * xzyw[inner_dim] ;
		}
	}
	cam_z = out[2];

	cv::Point pt = cv::Point(out[0]/out[2]*scale, out[1]/out[2]*scale);
	return pt;
}

static std::vector<cv::Point> makeFace(cv::Point p1, cv::Point p2, cv::Point p3, cv::Point p4)
{
	std::vector<cv::Point> face;
	face.push_back(p1);
	face.push_back(p2);
	face.push_back(p3);
	face.push_back(p4);
	return face;
}

static std::vector<cv::Point> makeLine(cv::Point p1, cv::Point p2)
{
	std::vector<cv::Point> line;
	line.push_back(p1);
	line.push_back(p2);
	return line;
}

static void  makeBoundingBox(std::vector<cv::Point> &bb_points, std::vector<std::vector<cv::Point> > &polygons)
{
	polygons.push_back(makeFace(bb_points[0],bb_points[1],bb_points[2],bb_points[3])); // bottom face
	polygons.push_back(makeFace(bb_points[4],bb_points[5],bb_points[6],bb_points[7])); // top face

	polygons.push_back(makeLine(bb_points[0], bb_points[4]));
	polygons.push_back(makeLine(bb_points[1], bb_points[5]));
	polygons.push_back(makeLine(bb_points[2], bb_points[6]));
	polygons.push_back(makeLine(bb_points[3], bb_points[7]));
}

int8_t primitive[2] = {-1, 1} ;
uint8_t pr_x[8] = {0,1,1,0,0,1,1,0};
uint8_t pr_y[8] = {0,0,1,1,0,0,1,1};
uint8_t pr_z[8] = {0,0,0,0,1,1,1,1};

static void object2boundingBox(arbe_msgs::arbeTSlamObj object, cv::Point &tl, int rows, int cols, bool &legit,float scale,std::vector<std::vector<cv::Point> > &polygons)
{
	legit=true;
	uint8_t out = 0;
	float x=object.position.x;
	float depth=object.position.y;
	float up=object.position.z;

	float size_x=object.bounding_box.scale_x; // perp. to the direction of motion
	float size_y=object.bounding_box.scale_y; // in the direction of motion
	float size_z=object.bounding_box.scale_z;

	float X,Y,Z;

	float orientation = object.bounding_box.orientation;
	float co = cos(orientation);
	float so = sin(orientation);
	std::vector<cv::Point> points;
	float top = 1000000;
	float left = 1000000;
	float cam_z;
	cv::Point pt;
	for (uint8_t i = 0; i < 8; i++){
		X = x + (primitive[pr_x[i]] * size_x * co + primitive[pr_y[i]] * size_y * so ) / 2;
		Y = depth + (primitive[pr_y[i]] * size_y * co - primitive[pr_x[i]] * size_x * so ) / 2;
		Z = (up + primitive[pr_z[i]] * size_z / 2);
		if(Y < 0)
			out = 8;
		pt = project(X, Y, Z, scale, cam_z);
		if(pt.y > rows || pt.y < 0 || pt.x > cols || pt.x < 0)
			out++;
		points.push_back(pt);
		left = pt.x < left ? pt.x : left;
		top = pt.y < top ? pt.y : top;
	}

	tl.x = left ;
	tl.y = top;// - 20;

	if (out > 4)
		legit = false;
	else
		makeBoundingBox(points,polygons);
}

static sensor_msgs::CompressedImage::ConstPtr cam_image;

void SI_slam_on_cam_callback(const sensor_msgs::CompressedImage::ConstPtr& image)
{
//	printf("received new image\n");
    cam_image = sensor_msgs::CompressedImage::ConstPtr(image);
    is_cam_frame_available = true;

}


void hadnle_SI_slam_on_cam()
{
    sensor_msgs::CompressedImage::ConstPtr image_local = cam_image;

    if(is_cam_frame_available == false)
    {
        return;
    }
    is_cam_frame_available = false;
    if(slamDisplaySettings.slam_on_camera && get_slam_valid())
	{

		cv_bridge::CvImagePtr cv_ptr;
        cv_ptr = cv_bridge::toCvCopy(image_local, sensor_msgs::image_encodings::BGR8);

		cv::Point pt2text;
		int cols = cv_ptr->image.cols;
		int rows = cv_ptr->image.rows;
		float scale = ((rows == 1200) || (rows == 512)) ? 1.0 : rows / 1080.0;
		std::vector<std::vector<cv::Point> > polygons;
		bool draw;
		bool eco_mode = slamDisplaySettings.disp_slam_eco_mode;
		bool color_by_class = slamDisplaySettings.color_by_class;

		// FS on camera!!!!
		FS_in_use = true;
		int n_pts = FS_display_polygon.polygon.points.size();
		if(n_pts>0 && slamDisplaySettings.disp_FS_on_cam)
		{
			cv::Mat layer = cv_ptr->image.clone();//= cv::Mat::zeros(cv_ptr->image.size(), CV_8UC3); //
			std::vector<cv::Point> fs_polygon;
			projectFS(FS_display_polygon, scale,fs_polygon);

			std::vector<std::vector<cv::Point> > fillContAll;
			//fill the single contour
			//(one could add multiple other similar contours to the vector)
			fillContAll.push_back(fs_polygon);
			cv::fillPoly( layer, fillContAll, cv::Scalar(90,255,50));
			//cv::fillPoly( layer, fillContAll, cv::Scalar(255,215,5)); // B,G,R
			cv::addWeighted(cv_ptr->image, 0.7, layer, 0.3, 0.0, cv_ptr->image);
		}
		FS_in_use = false;


		arbe_msgs::arbeSlamMsg::ConstPtr localSlam = arbe_msgs::arbeSlamMsg::ConstPtr(slamMsg);
		uint16_t n_objects = localSlam->meta_data.NumberOfObjects;
		if(arg_radar_id == 0 && get_slam_valid() && slamDisplaySettings.disp_funnel)
		{
			uint8_t n_segments = 5;
			float vel = localSlam->meta_data.HostVelocity, omega = localSlam->meta_data.HostOmega;
			float DT = 1.0;
			if(vel*3.6>60)
			{
				DT -= vel*3.6 / 120;
				DT = DT < 0.3 ? 0.3 : DT;
			}
			float dt = DT / n_segments;
			float cam_z;
			if(vel > 5/3.6)
			{
				double y0 = 1 > vel * dt / 2 ? 1 : vel * dt / 2;
				if(fabs(omega)*180/3.1415 > 0.001)
				{
					double circ_center = vel / omega;
					double radius = sqrt(circ_center*circ_center + vel * dt * vel * dt / 4);
					omega = -omega;
					double delta_angle = atan(dt * omega) * 2;
					double angle = - delta_angle / 2;
					if (omega < 0)
						angle += M_PI ;
					cv::Point pt_r = project(cos(angle) * radius + circ_center + 1, y0, -1, scale,  cam_z);
					cv::Point pt_l = project(cos(angle) * radius + circ_center - 1, y0, -1, scale,  cam_z);
					for (uint8_t section = 0; section < n_segments; section++)
					{
						angle += delta_angle;
						double Y = sin(angle) * radius;
						double X = cos(angle) * radius + circ_center;
						double Z = rd_inc.rd_ls_a * Y + rd_inc.rd_ls_b;
						cv::Point pt2_r = project(X + 1, Y, Z, scale,cam_z);
						cv::Point pt2_l = project(X - 1, Y, Z, scale,cam_z);
						cv::line(cv_ptr->image, pt_r,pt2_r,cv::Scalar(0,0,255),4);
						cv::line(cv_ptr->image, pt_l,pt2_l,cv::Scalar(0,0,255),4);
						pt_r = pt2_r;
						pt_l = pt2_l;
					}
				}
				else
				{
					cv::Point pt_r = project( 1, y0, -1, scale,cam_z);
					cv::Point pt_l = project(-1, y0, -1, scale, cam_z);
					double Y = vel * DT;
					double Z = rd_inc.rd_ls_a * Y + rd_inc.rd_ls_b;
					cv::Point pt2_r = project( 1, Y, Z, scale, cam_z);
					cv::Point pt2_l = project(-1, Y, Z, scale, cam_z);

					cv::line(cv_ptr->image, pt_r,pt2_r,cv::Scalar(0,0,255),4);
					cv::line(cv_ptr->image, pt_l,pt2_l,cv::Scalar(0,0,255),4);
				}
			}
		}

		for(size_t i = 0; i < n_objects; i++)
		{
			arbe_msgs::arbeTSlamObj object = localSlam->ObjectsBuffer[i];
			int16_t cls2show = get_classes_to_show();
			if ( cls2show != -1 && cls2show != object.obj_class)
				continue;
			std::string fc_txt="";
			float red,green,blue;
			Slam_Color::Instance()->get_class_color(object.obj_class, red, green, blue, fc_txt);

			if(!eco_mode)
				polygons.clear();
			object2boundingBox(object ,pt2text,rows,cols,draw,scale,polygons);
			if(!eco_mode)
			{
				if(!draw)
					continue;
				if(!color_by_class)
					Slam_Color::Instance()->get_color(object.ID,red,green,blue);
				uint8_t r,g,b;
				r = (uint8_t)(255*red);
				g = (uint8_t)(255*green);
				b = (uint8_t)(255*blue);

				float renorm_depth= 3.0 /80 * Fixed2Float(object.position.y, 7) + 3;
				int thickness = round(15 / renorm_depth);
				thickness = thickness < 1 ? 1 : thickness;

				cv::polylines(cv_ptr->image, polygons, true, cv::Scalar(b,g,r),thickness);

				std::string txt = std::to_string(object.ID);
				if(slamDisplaySettings.disp_dist_on_cam)
				{
					float x = object.position.x;
					float y = object.position.y;
					uint16_t dis_to_obj = (uint16_t)(round(sqrt(x*x + y*y)));
					txt = std::to_string(dis_to_obj) + " m";
				}
				txt = txt + fc_txt;
				cv::putText(cv_ptr->image, txt, pt2text, cv::FONT_HERSHEY_SIMPLEX,
							(0.5 + 3 / renorm_depth ), cv::Scalar(b,g,r),3);
			}
		}
		if (eco_mode)
		{
			cv::polylines(cv_ptr->image, polygons, true, cv::Scalar(53,255,255),5);
		}
		arbe_capture_pub.publish(cv_ptr->toCompressedImageMsg());
	}
	else
	{
        arbe_capture_pub.publish(image_local);
	}
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

std::string radar_descriptor;

static Eigen::Affine3f * transform_p = &pcl_transform;
void calc_transform_matrix()
{
	// Eigen transforms ALWAYS apply rotation first, then translation
	pcl_transform = Eigen::Affine3f::Identity();

	pcl_transform.rotate(Eigen::AngleAxisf(radar_yaw_angle, Eigen::Vector3f::UnitZ()));
	pcl_transform.rotate(Eigen::AngleAxisf(radar_pitch_angle, Eigen::Vector3f::UnitX()));

	Eigen::Vector3f translation(arg_radar_x_offset, arg_radar_y_offset, radar_z_offset);
	pcl_transform.translation() = translation;
}

static Eigen::Affine3f slam_transform = pcl_transform;
void calc_slam_transform()
{
	slam_transform = Eigen::Affine3f::Identity();
	slam_transform.translation()[0] = local_cart_x;
	slam_transform.translation()[1] = local_cart_y;
	slam_transform.rotate(Eigen::AngleAxisf(hostHeading, Eigen::Vector3f::UnitZ()));
	slam_transform = slam_transform * pcl_transform;
}

static uint8_t one_color_frame = 0;

void fix_installation_callback(const std_msgs::Float32::ConstPtr &msg)
{
	radar_yaw_angle = (msg->data + arg_radar_yaw_angle) * M_PI/180;
	calc_transform_matrix();
	one_color_set();
}

void restore_defaults_callback(const std_msgs::Bool::ConstPtr &msg)
{
	radar_yaw_angle = arg_radar_yaw_angle * M_PI/180;
	calc_transform_matrix();
}

#define FPS_CALC_LENGTH 20
ros::Time t_vec[FPS_CALC_LENGTH];
size_t write_i_t = 0;
size_t read_i_t = 0;
uint8_t n_fps = 0;

void reset_fps_calc()
{
	write_i_t = 0;
	read_i_t = 0;
	n_fps = 0;
}

double calculate_fps( void )
{
	ros::Time Now = ros::Time::now();
	t_vec[write_i_t++] = Now;
	ros::Duration delta = Now - t_vec[read_i_t];

	n_fps += n_fps < FPS_CALC_LENGTH ? 1 : 0 ;
	write_i_t %= FPS_CALC_LENGTH;
	if (n_fps == FPS_CALC_LENGTH)
	{
		read_i_t++;
		read_i_t %= FPS_CALC_LENGTH;
	}
	return n_fps / (delta.toSec() + 1e-5);
}


uint32_t total_pts =0;

static bool clear_stationary_once = true;
uint32_t detections_number = 0;
uint32_t dynamic_n_detections;

void pointcloud_display_handler (bool also_stationary_cloud)
{
//	printf("TODO: verify sign of host heading in localization mode in SLAM display\r\n");
//	printf("TODO: localization when localization isn't valid\r\n");
//	printf("TODO: implement double buffer?\r\n");
//	printf("TODO: implement noise_level_db\r\n");
//	printf("TODO: implement noise level \r\n");
//	printf("TODO: implement pcl filtering when aggregation is active via pcl2\r\n");

	float z_min, z_max;
	bool discard_out_of_elevation = get_discard_out_of_el_context();
	Color_Coding_Min_Max::Instance()->get_values("Elevation",z_min, z_max);

	dynamic_n_detections = 0;

	detections_number = PointCloudMsg->PcHeader.number_of_points;

	size_t j =0;

	/* we use double buffer - switch frame */
	//mux = 1 - mux;
	//printf("TODO: implement double buffer?\r\n");
	cloud.points.resize(detections_number);

//	cloud.height = 1;

//	cloud.width = detections_number;

	float cc_min, cc_max, span;
	Color_Coding_Min_Max::Instance()->get_values(ColoringType,cc_min,cc_max);
	span = cc_max - cc_min;

	for(size_t i = 0; i < detections_number; i++)
	{
		cloud.points[j].range = ((PointCloudMsg->Points.range_bin[i] - PointCloudMsg->PcMetadata.range_offset)*PointCloudMsg->PcMetadata.PcResolution.range_resolution);
		cloud.points[j].doppler = PointCloudMsg->Points.doppler_signed_bin[i] * PointCloudMsg->PcMetadata.PcResolution.doppler_resolution;
		if ( (cloud.points[j].doppler > MaxDoppler) || (cloud.points[j].doppler < MinDoppler) )
		{
			/* Simple filter based on min/max doppler */
			continue;
		}
		cloud.points[j].elevation = PointCloudMsg->Points.elevation_signed_bin[i] * PointCloudMsg->PcMetadata.PcResolution.elevation_coefficient;
		cloud.points[j].azimuth = PointCloudMsg->Points.azimuth_signed_bin[i] * PointCloudMsg->PcMetadata.PcResolution.azimuth_coefficient;

		TaregtCartesian targetCartesian;
		targetCartesian.x = cloud.points[j].range * cloud.points[j].azimuth;//PointCloudMsg->Points.azimuth_signed_bin[i] * PointCloudMsg->PcMetadata.PcResolution.azimuth_coefficient;
		targetCartesian.z = cloud.points[j].range * cloud.points[j].elevation;//PointCloudMsg->Points.elevation_signed_bin[i] * PointCloudMsg->PcMetadata.PcResolution.elevation_coefficient;
		targetCartesian.y = sqrt(cloud.points[j].range*cloud.points[j].range - targetCartesian.x * targetCartesian.x - targetCartesian.z * targetCartesian.z);

		Eigen::Vector3f one_point(targetCartesian.x,targetCartesian.y,targetCartesian.z);
		one_point = *transform_p * one_point;
		targetCartesian.x = one_point[0];
		targetCartesian.z = one_point[2];
		targetCartesian.y = one_point[1];

//		float abs_z = targetCartesian.z + radar_z_offset;
		if (discard_out_of_elevation && (targetCartesian.z < z_min || targetCartesian.z > z_max))
			continue;

//		if ( strcmp(ColoringType.c_str(), "Amplitude-Flat") == 0 )
//			cloud.points[j].z = 0;
//		else
//			cloud.points[j].z = targetCartesian.z;

//		cloud.points[j].range =  range;
//		cloud.points[j].doppler = doppler;

		/* Perform power equalization for amplitude display */

		cloud.points[j].power = RAF_COM_CALC_CalcPowerEqulizer(RAF_COM_CALC_CalcPower(PointCloudMsg->Points.power[i]),
															   PointCloudMsg->Points.range_bin[i],
															   PointCloudMsg->PcMetadata.range_res_in_db,
															   PointCloudMsg->PcMetadata.range_offset);

		if ( strcmp(ColoringType.c_str(), "Range/Doppler") == 0 )
		{
			/* range doppler map */
			if ( PointCloudMsg->Points.azimuth_signed_bin[i] != selectedAzimuthBin )
				continue;
			cloud.points[j].x = cloud.points[j].doppler;
			cloud.points[j].y = cloud.points[j].range;
			cloud.points[j].z =  RAF_COM_CALC_CalcPower(PointCloudMsg->Points.power[i] - noise_level_db);
		}
		else
		{

			cloud.points[j].x = targetCartesian.x;
			cloud.points[j].y = targetCartesian.y;
			if ( strcmp(ColoringType.c_str(), "Amplitude-Flat") == 0 )
				cloud.points[j].z = 0;
			else
				cloud.points[j].z = targetCartesian.z;
		}


		//ROS_DEBUG("min-%f,max-%f",min_power_to_color,max_power_to_color);
		cloud.points[j].range_bin 	   = PointCloudMsg->Points.range_bin[i];
		cloud.points[j].elevation_bin = PointCloudMsg->Points.elevation_signed_bin[i];
		cloud.points[j].azimuth_bin   = PointCloudMsg->Points.azimuth_signed_bin[i];
		cloud.points[j].doppler_bin   = PointCloudMsg->Points.doppler_signed_bin[i];
		cloud.points[j].power_value   = PointCloudMsg->Points.power[i];
		cloud.points[j].timestamp_sec   = (uint32_t)(PointCloudMsg->PcHeader.time / 1000);
		cloud.points[j].timestamp_nsec   = (uint32_t)(PointCloudMsg->PcHeader.time % 1000) * 1000000;

		//noise_level_db = noise_level.toInt();
		cloud.points[j].snr = RAF_COM_CALC_CalcPower(PointCloudMsg->Points.power[i]) ;//- noise_level_db;
		//printf("TODO: implement noise_level_db\r\n");

		if ( !PointCloudMsg->Points.phase.empty() )
		{
			cloud.points[j].phase_value   = PointCloudMsg->Points.phase[i];
		}

		if(one_color_frame == 0)
			pointcloud_color(cloud, j,cc_min, cc_max, span, ColoringType, ego_velocity, radar_yaw_angle);
		else
		{
			pointcloud_one_color(cloud, j, 0, 183, 235);
		}
		j++;
	}
	detections_number = j;
	cloud.height = 1;
	cloud.width = detections_number;
	cloud.points.resize(cloud.width * cloud.height);
	dynamic_n_detections = j;


	/* A flat amplitude display intended to visualize the strong center of targets */
	if ( strcmp(ColoringType.c_str(), "Amplitude-Flat") == 0 )
	{
		pointcloud_filter_amplitude( &j );
		/* shrink cloud width after filter */
		cloud.width = j;
		cloud.height = 1;
		cloud.points.resize(cloud.width * cloud.height);
	}


	//pcl::transformPointCloud(cloud, cloud, slam_transform);

	output.header.stamp = ros::Time::now();
	pcl::toROSMsg(cloud,output);
	output.header.frame_id = "image_radar";
	arbe_pcl_pub.publish(output);
	cloud.clear();
}

uint32_t stationary_n_detections = 0;

void stationary_display_handler (bool also_stationary_cloud)
{
	stationary_n_detections = 0;
	if ( also_stationary_cloud)
	{
		bool Aggregation_active = get_aggregation();

		if(1)//(Aggregation_active == false) && get_localization_enable())
		{
			//printf("TODO: implement decay rime\r\n");
//			if(slamMsg->meta_data.HostHeadingUnc < 0)
//			{
//				viewpanel::Instance()->setPointDecayTime(0);
//			}
//			else
//			{
//				viewpanel::Instance()->setPointDecayTime(15000);
//			}
		}

		uint32_t stationary_detections_number = StationaryPointCloudMsg->PcHeader.number_of_points;
		uint32_t J;
		if(!Aggregation_active || get_reset_mapping() || (slamMsg->meta_data.HostHeadingUnc < 0) /*slamMsg->meta_data.is_reset_geo_location*/)
		{
			J = 0;
			stationary_pc.clear();

			stationary_pc.width = stationary_detections_number;
			stationary_pc.points.resize(stationary_detections_number);
			total_pts=0;
			set_reset_mapping(false);
		}
		else
		{
			J = stationary_pc.width;
			stationary_pc.width += stationary_detections_number;
			stationary_pc.points.resize(stationary_pc.points.size() + stationary_detections_number);
		}
		stationary_pc.height = 1;

		for(size_t i = 0; i < stationary_detections_number; i++)
		{


			stationary_pc.points[J].elevation = StationaryPointCloudMsg->Points.elevation_signed_bin[i] *
					StationaryPointCloudMsg->PcMetadata.PcResolution.elevation_coefficient;
			stationary_pc.points[J].azimuth = StationaryPointCloudMsg->Points.azimuth_signed_bin[i] *
					StationaryPointCloudMsg->PcMetadata.PcResolution.azimuth_coefficient;
			stationary_pc.points[J].range = ((StationaryPointCloudMsg->Points.range_bin[i] - StationaryPointCloudMsg->PcMetadata.range_offset)*
						   StationaryPointCloudMsg->PcMetadata.PcResolution.range_resolution);
			stationary_pc.points[J].doppler = StationaryPointCloudMsg->Points.doppler_signed_bin[i] * StationaryPointCloudMsg->PcMetadata.PcResolution.doppler_resolution;

			if ( (stationary_pc.points[J].doppler > MaxDoppler) || (stationary_pc.points[J].doppler < MinDoppler) ) //||
			{
				/* Simple filter based on min/max doppler */
				continue;
			}

			TaregtCartesian targetCartesian;
			targetCartesian.x = stationary_pc.points[J].range * stationary_pc.points[J].azimuth;//StationaryPointCloudMsg->Points.azimuth_signed_bin[i] * StationaryPointCloudMsg->PcMetadata.PcResolution.azimuth_coefficient;
			targetCartesian.z = stationary_pc.points[J].range * stationary_pc.points[J].elevation;//StationaryPointCloudMsg->Points.elevation_signed_bin[i] * StationaryPointCloudMsg->PcMetadata.PcResolution.elevation_coefficient;
			targetCartesian.y = sqrt(stationary_pc.points[J].range*stationary_pc.points[J].range - targetCartesian.x * targetCartesian.x - targetCartesian.z * targetCartesian.z);

			Eigen::Vector3f one_point(targetCartesian.x,targetCartesian.y,targetCartesian.z);
			one_point = *transform_p * one_point;
			targetCartesian.x = one_point[0];
			targetCartesian.z = one_point[2];
			targetCartesian.y = one_point[1];

			bool discard_out_of_elevation = get_discard_out_of_el_context();
			if (discard_out_of_elevation){

				float abs_z = targetCartesian.z + radar_z_offset;
				float z_min, z_max;
				Color_Coding_Min_Max::Instance()->get_values("Elevation",z_min, z_max);

				if(abs_z < z_min || abs_z > z_max)
					continue;
			}
			stationary_pc.points[J].x = targetCartesian.x;
			stationary_pc.points[J].y = targetCartesian.y;

			if ( strcmp(ColoringType.c_str(), "Amplitude-Flat") == 0 )
				stationary_pc.points[J].z = 0;
			else
				stationary_pc.points[J].z = targetCartesian.z;


			/* Perform power equalization for amplitude display */

			stationary_pc.points[J].power = RAF_COM_CALC_CalcPowerEqulizer(RAF_COM_CALC_CalcPower(StationaryPointCloudMsg->Points.power[i]),
																		   StationaryPointCloudMsg->Points.range_bin[i],
																		   StationaryPointCloudMsg->PcMetadata.range_res_in_db,
																		   StationaryPointCloudMsg->PcMetadata.range_offset);

			if ( strcmp(ColoringType.c_str(), "Range/Doppler") == 0 )
			{
				/* range doppler map */
				if ( StationaryPointCloudMsg->Points.azimuth_signed_bin[i] != selectedAzimuthBin )
					continue;
				stationary_pc.points[J].x = stationary_pc.points[J].doppler;
				stationary_pc.points[J].y = stationary_pc.points[J].range;
				stationary_pc.points[J].z =  RAF_COM_CALC_CalcPower(StationaryPointCloudMsg->Points.power[i] - noise_level_db);
			} else {
				//cloud[mux].points[j].x = targetCartesian.x;
				//cloud[mux].points[j].y = targetCartesian.y;
				if ( strcmp(ColoringType.c_str(), "Amplitude-Flat") == 0 )
					stationary_pc.points[J].z = 0;
				else
					stationary_pc.points[J].z = targetCartesian.z;
			}

			stationary_pc.points[J].range_bin 	   = StationaryPointCloudMsg->Points.range_bin[i];
			stationary_pc.points[J].elevation_bin = StationaryPointCloudMsg->Points.elevation_signed_bin[i];
			stationary_pc.points[J].azimuth_bin   = StationaryPointCloudMsg->Points.azimuth_signed_bin[i];
			stationary_pc.points[J].doppler_bin   = StationaryPointCloudMsg->Points.doppler_signed_bin[i];
			stationary_pc.points[J].power_value   = StationaryPointCloudMsg->Points.power[i];
			stationary_pc.points[J].timestamp_sec   = (uint32_t)(PointCloudMsg->PcHeader.time / 1000);
			stationary_pc.points[J].timestamp_nsec   = (uint32_t)(PointCloudMsg->PcHeader.time % 1000) * 1000000;

			//noise_level_db = noise_level.toInt();
			stationary_pc.points[J].snr = RAF_COM_CALC_CalcPower(StationaryPointCloudMsg->Points.power[i]);// - noise_level_db;
			//printf("TODO: implement noise level \r\n");

			if ( !StationaryPointCloudMsg->Points.phase.empty() )
			{
				stationary_pc.points[J].phase_value   = StationaryPointCloudMsg->Points.phase[i];
			}
			float cc_min, cc_max, span;
			Color_Coding_Min_Max::Instance()->get_values(ColoringType,cc_min,cc_max);
			span = cc_max - cc_min;

			if(one_color_frame == 0)
				pointcloud_color(stationary_pc, J,cc_min, cc_max, span, ColoringType, ego_velocity, radar_yaw_angle);
			else
			{
				pointcloud_one_color(stationary_pc, J, 0, 183, 235);
			}
			J++;

		}
		stationary_pc.width = J;
		stationary_pc.height = 1;
		stationary_pc.points.resize(stationary_pc.width * stationary_pc.height);

		total_pts+=StationaryPointCloudMsg->Points.power.size();
		//			g_buffer_mutex.unlock();

		//printf("TODO: implement pcl filtering when aggregation is active via pcl2\r\n");
		/*
		pcl::toPCLPointCloud2(stationary_pc, point_cloud2);

		if (Aggregation_active)
		{
			// Create the filtering object
			pcl::VoxelGrid<pcl::PCLPointCloud2> sor;
			sor.setInputCloud (point_cloud2_ptr);
			sor.setLeafSize (1.0f, 1.0f, 1.0f);
			sor.filter (*cloud_filtered);

			stationary_pc.clear();
			point_cloud2.data.clear();
			pcl::fromPCLPointCloud2(*cloud_filtered, stationary_pc);

			// sort PC according to time and drop the oldest
			if(stationary_pc.width > MAX_AGG_POINTS)
			{
				ROS_INFO("too many aggregateed points. removing old ones");
				std::sort(stationary_pc.points.begin(), stationary_pc.points.end(), compare_timestamp);
				stationary_pc.width = MAX_AGG_POINTS/2;
				stationary_pc.points.resize(stationary_pc.width * stationary_pc.height);
			}
		}
*/

		//pcl::transformPointCloud(stationary_pc, stationary_pc, slam_transform);

		stationaty_output.header.stamp = ros::Time::now();
		pcl::toROSMsg(stationary_pc,stationaty_output);
		stationaty_output.header.frame_id = "image_radar";
		stationary_pcl_pub.publish(stationaty_output);

//		point_cloud2.data.clear();
		J = stationary_pc.points.size();

		uint32_t stationary_detections = StationaryPointCloudMsg->PcHeader.number_of_points;
		detections_number += stationary_detections;
		stationary_n_detections = stationary_detections;

	}
	else if(!get_disp_processed_pc() && clear_stationary_once)
	{
		clear_stationary_once = false;
		set_reset_mapping(false);

		stationaty_output.header.stamp = ros::Time::now();
		pcl::toROSMsg(stationary_pc,stationaty_output);
		stationaty_output.header.frame_id = "image_radar";
		stationary_pcl_pub.publish(stationaty_output);

		stationary_pc.clear();
		stationary_pc.width = 0;
		stationary_pc.points.resize(0);
	}




}
void display_text(bool also_stationary_cloud)
{
	if(one_color_frame > 0)
		one_color_frame--;


	std::stringstream detections_text;
	detections_text.precision(2);
	if (floating_text_enabled && !is_localization_active)
	{
		//detections_text << "(" << "Radar " << arg_radar_id << ", Offset [" << ((float)(int)(arg_radar_x_offset*10))/10.0 << ", " << ((float)(int)(arg_radar_y_offset*10))/10.0 << ", " << ((float)(int)(radar_z_offset*10))/10.0 <<"], Yaw "<< ((float)(int)(radar_yaw_angle * 180 / M_PI *10))/10.0<< ")\n";
		detections_text << "(" << "Radar: " << arg_radar_name << ")\n";
		detections_text << "(" << "Frame " << PointCloudMsg->PcHeader.frame_counter << ", " << ( detections_number )<< " detections" ;
		if(also_stationary_cloud)
			detections_text << ", " <<stationary_n_detections << " stat. ," << dynamic_n_detections << " dyn.";
		detections_text << ")\n";

		detections_text << "(";
		if(also_stationary_cloud)
			detections_text << "Speed: " << (int)(ego_velocity * 3.6) << " Km/h, ";
		detections_text << ((float)(int)(calculate_fps()*10))/10.0 << " FPS)\n";

		dtections_per_frame_marker.scale.x = marker_text_size;
		dtections_per_frame_marker.scale.y = marker_text_size;
		dtections_per_frame_marker.scale.z = marker_text_size;
		dtections_per_frame_marker.text = detections_text.str();
		arbe_info_markers.publish(dtections_per_frame_marker);
	}
	else if (expunge_text)
	{
		expunge_text = false;
		dtections_per_frame_marker.text = "";
		arbe_info_markers.publish(dtections_per_frame_marker);
	}
}
std::mutex g_buffer_mutex;

void handle_pc_frame()
{
    if(is_pc_frame_available == true)
    {
//		g_buffer_mutex.lock();
		pointcloud_display_handler(valid_stationary_pc);
		is_pc_frame_available = false;
		stationary_display_handler(valid_stationary_pc);
		display_text(valid_stationary_pc);
		valid_stationary_pc = false;
	}

//		g_buffer_mutex.unlock();



}


void targets_read_callback(const arbe_msgs::arbeNewPcMsg::ConstPtr& pcMsg, int pc_type)
{
		if ( get_disp_processed_pc() ) //processed PC --> that is after dsp_node
		{
			transform_p = &slam_transform;
			if((pc_type == PCT_PROCESSED_STATIONARY)&&(valid_stationary_pc == false))
			{
				StationaryPointCloudMsg = arbe_msgs::arbeNewPcMsg::ConstPtr(pcMsg);
				valid_stationary_pc = true;
				return;
			}
			else if((pc_type == PCT_PROCESSED_DYN)&&(is_pc_frame_available == false))
			{
				PointCloudMsg = pcMsg;//arbe_msgs::arbeNewPcMsg::ConstPtr(pcMsg);
				is_pc_frame_available = true;
			}
		}
		else if((pc_type == PCT_PARSED)&&(is_pc_frame_available == false))
		{
			PointCloudMsg = pcMsg;//arbe_msgs::arbeNewPcMsg::ConstPtr(pcMsg);
			is_pc_frame_available = true;
			transform_p = &pcl_transform;
			clear_stationary_once = true;
		}
		//pointcloud_display_handler(valid_stationary_pc);
		//valid_stationary_pc = false;
}

int16_t classes_to_show = -1;
int16_t get_classes_to_show()
{
	return classes_to_show;
}
void set_classes_to_show(int16_t cls)
{
	classes_to_show = cls;
}

size_t get_num_objects()
{
	if (slamMsg)
		return slamMsg->ObjectsBuffer.size();
	else
		return 0;
}


arbe_msgs::arbeTSlamObj get_object(uint16_t i)
{
	return slamMsg->ObjectsBuffer[i];
}

static bool colorObjectsByClass = false;
bool get_colorObjByClass()
{
	return colorObjectsByClass;
}
void set_colorObjByClass(bool flag)
{
	colorObjectsByClass = flag;
}

arbe_msgs::arbeTSlamMetadata get_slam_metadata()
{
	return slamMsg->meta_data;
}

void fs_display_handler()
{
	Eigen::Affine3f local_transform;
	if ( get_disp_processed_pc() )
	{
		local_transform = slam_transform;
	}
	else
	{
		local_transform = pcl_transform;
	}

	transformed_FS_in_use = true;
	for(size_t p = 0; p < transformed_FS_polygon.polygon.points.size(); p++)
	{
		Eigen::Vector3f one_point(transformed_FS_polygon.polygon.points[p].x,transformed_FS_polygon.polygon.points[p].y,transformed_FS_polygon.polygon.points[p].z);
		one_point = local_transform * one_point;
		transformed_FS_polygon.polygon.points[p].x = one_point[0];
		transformed_FS_polygon.polygon.points[p].z = one_point[2];
		transformed_FS_polygon.polygon.points[p].y = one_point[1];
	}
	fs_poly_pub.publish(transformed_FS_polygon);
	transformed_FS_in_use = false;
}


void pub_slam_vis()
{
	marker_pub.publish(slam_arrows);
	marker_pub.publish(slam_tags);
	marker_pub.publish(slam_boxes);
}

void slam_display_handler()
{

	// Set our initial shape type to be a cube
	uint32_t shape = visualization_msgs::Marker::CUBE;
	uint32_t arrow_shape = visualization_msgs::Marker::ARROW;
	//visualization_msgs::Marker marker;

	tf2::Quaternion q_rot;

	int16_t cls2show = get_classes_to_show();
	uint16_t num_bb = 0;
	for(int i = 0; i < get_num_objects(); i++)//ObjectsPerFrame.size(); i++)
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

		Eigen::Vector3f objx(slam_obj.position.x,slam_obj.position.y,slam_obj.position.z);
		objx = slam_transform * objx;

//		float local_cart_x, local_cart_y, ego_heading;
		float orientation_f, ego_heading;

		if(is_localization_active)
		{
			orientation_f = orientation - hostHeading;
		}
		else
		{
			orientation_f = orientation;
		}

		slam_boxes.markers[marker_i].pose.position.x=objx[0];
		slam_boxes.markers[marker_i].pose.position.y=objx[1];
		slam_boxes.markers[marker_i].pose.position.z=objx[2];

		double r=0, p=0, y= M_PI_2 - orientation_f + radar_yaw_angle;  // Rotate the previous pose (yaw)
		q_rot.setRPY(r, p, y);
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
			Slam_Color::Instance()->get_class_color(slam_obj.obj_class, red, green, blue, fc_txt);
		}
		else
		{
			Slam_Color::Instance()->get_color(slam_obj.ID,red,green,blue);
		}
		slam_boxes.markers[marker_i].color.r = red;
		slam_boxes.markers[marker_i].color.g = green;
		slam_boxes.markers[marker_i].color.b = blue;
		slam_boxes.markers[marker_i].color.a = 1;

		std::string id_text = std::to_string(slam_obj.ID);
		slam_boxes.markers[marker_i].text = id_text;

        slam_boxes.markers[marker_i].lifetime = ros::Duration(0.05);
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
        slam_arrows.markers[marker_i].lifetime = ros::Duration(0.05);

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
		slam_tags.markers[marker_i].type = visualization_msgs::Marker::TEXT_VIEW_FACING;
		slam_tags.markers[marker_i].action = visualization_msgs::Marker::ADD;
		slam_tags.markers[marker_i].pose.position.z += slam_obj.bounding_box.scale_z / 2 + 5 * scale_text/8;
		slam_tags.markers[marker_i].scale.z = scale_text;
		slam_tags.markers[marker_i].color.r = 1.0f;
		slam_tags.markers[marker_i].color.g = 1.0f;
		slam_tags.markers[marker_i].color.b = 1.0f;
		slam_tags.markers[marker_i].color.a = 1.0;
        slam_tags.markers[marker_i].lifetime = ros::Duration(0.05);

		marker_i++;
	}
	pub_slam_vis();
}

void handle_slam_frame()
{
    if(is_slam_frame_available == false)
    {
		if((disp_objects == true))// && (is_slam_active == true))
        {
            pub_slam_vis(); // publish latest msg to slam objects are seen when PC is "paused"
        }

        return;
    }

    is_slam_frame_available = false;
	arbe_msgs::arbeSlamMsg::ConstPtr local_slamMsg;

	if(is_localization_active && arg_radar_id !=0)
		local_slamMsg = masterSlamMsg;
	else
		local_slamMsg = slamMsg;

    ego_velocity = local_slamMsg->meta_data.HostVelocity;
    hostHeadingUnc = local_slamMsg->meta_data.HostHeadingUnc;
    if(hostHeadingUnc > 0)
        hostHeading = local_slamMsg->meta_data.HostHeading;
    local_cart_x = local_slamMsg->meta_data.local_catr_x;
    local_cart_y = local_slamMsg->meta_data.local_catr_y;

    if(is_localization_active)
        calc_slam_transform();
    else
        slam_transform = pcl_transform;

    set_slam_valid(true);

    slam_display_handler();


}

void slam_read_callback(const arbe_msgs::arbeSlamMsg::ConstPtr& msg)
{
	slamMsg = arbe_msgs::arbeSlamMsg::ConstPtr(msg);
    is_slam_frame_available = true;
}

void master_slam_read_callback(const arbe_msgs::arbeSlamMsg::ConstPtr& msg)
{
	masterSlamMsg = arbe_msgs::arbeSlamMsg::ConstPtr(msg);
	is_master_slam_frame_available = true;
}


void road_inclination_callback(const arbe_msgs::arbeRdInclinationConstPtr & msg)
{
	rd_inc.lastUpdateTime = msg->header.stamp;
    rd_inc.rd_ls_a = msg->ant_tilt;
    rd_inc.rd_ls_b = msg->ant_height;
}

void *si_slam_cam_thread(void *args)
{
	ros::Rate loop_rate(45);

    while (ros::ok())
    {
        hadnle_SI_slam_on_cam();
        ros::spinOnce();
        loop_rate.sleep();
    }

}

void gui_message_callback(const std_msgs::String::ConstPtr& msg)
{
	ROS_INFO("I heard: [%s]", msg->data.c_str());
	std::string msg_str = msg->data;

	std::size_t pos=msg_str.find("goodbye");
	if (pos!=std::string::npos)
	{
		ROS_WARN("Received a goodbye command from the GUI node... stopping");
		int ret =0;
		ret = system("rosnode kill --all");
	}
}

void prepare_basic_markers( void )
{

	/* Prepare the markers that show the number of detections per frame */
	dtections_per_frame_marker.header.frame_id = "image_radar";
	dtections_per_frame_marker.ns = "radar_dtections_per_frame_marker";
	dtections_per_frame_marker.id = 10006;
	dtections_per_frame_marker.type = visualization_msgs::Marker::TEXT_VIEW_FACING;
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
}

void change_text_phi_callback(const std_msgs::Float32::ConstPtr & msg)
{
	dtections_per_frame_marker.pose.position.x = -10 * sin(msg->data);
	dtections_per_frame_marker.pose.position.y = 10 * cos(msg->data);
}

void one_color_set()
{
	one_color_frame = 10;
}

void front_side_slam_triggered_callback(const std_msgs::Bool::ConstPtr & trigger)
{
	//one_color_set();
}

void side_slam_triggered_callback(const std_msgs::Bool::ConstPtr & trigger)
{
	triggered_slam = trigger->data;
	set_disp_objects(trigger->data);
	if(trigger->data && fs_from_gui)
		choose_fs_disp_callback(trigger);
	else if(!trigger->data && fs_from_gui)
	{
		choose_fs_disp_callback(trigger);
		fs_from_gui = true;
	}
	if(!trigger->data)
	{
		clear_stationary_once = true;
		slam_transform = pcl_transform;
	}
	//one_color_set();
}

/* Main function */
int main(int argc, char **argv)
{
 	pthread_t si_slam_cam_thread_id;
	int ret;
	int one = 1;
	struct sockaddr_in data_serv_addr;

	ros::init(argc, argv, "arbe_visualization_engine");
	ros::NodeHandle n("~");// = boost::make_shared<ros::NodeHandle>();

	arg_radar_yaw_angle = 0.0;

	n.getParam("Radar_ID", arg_radar_id);
	n.getParam("Antenna_Name", arg_radar_name);
	n.getParam("Radar_Yaw_Angle", arg_radar_yaw_angle);
	n.getParam("Radar_Pitch_Angle", arg_radar_pitch_angle);
	n.getParam("Radar_Offset_x", arg_radar_x_offset);
	n.getParam("Radar_Offset_y", arg_radar_y_offset);
	n.getParam("Radar_Offset_z", arg_radar_z_offset);

	/* Create a ROS publisher for GUI settings send to the radar nodes */
//	arbe_radar_calibration_pub = n.advertise<arbe_msgs::arbeGUIsettings>("/arbe/settings/radar_calibration",1,true);

	radar_yaw_angle = -arg_radar_yaw_angle * M_PI/180;
	radar_pitch_angle = arg_radar_pitch_angle;
	radar_x_offset = arg_radar_x_offset;
	radar_y_offset = arg_radar_y_offset;
	radar_z_offset = arg_radar_z_offset;

	calc_transform_matrix();
	slam_transform = pcl_transform;
	ros::Subscriber gui_commands_sub;

	arbe_pcl_pub = n.advertise<sensor_msgs::PointCloud2>("/arbe/rviz/pointcloud_"+std::to_string(arg_radar_id), 1);
	stationary_pcl_pub = n.advertise<sensor_msgs::PointCloud2>("/arbe/rviz/stationary_pointcloud_"+std::to_string(arg_radar_id), 1);
    marker_pub = n.advertise<visualization_msgs::MarkerArray>("/arbe/rviz/objects_"+std::to_string(arg_radar_id),3);
	fs_poly_pub = n.advertise<geometry_msgs::PolygonStamped>("/arbe/rviz/fs_poly_"+std::to_string(arg_radar_id), 1);
	arbe_capture_pub = n.advertise<sensor_msgs::CompressedImage>("/cv_camera_"+std::to_string(arg_radar_id)+"/processed_image/compressed",1);
	arbe_info_markers = n.advertise<visualization_msgs::Marker>("/arbe/rviz/floatingText_marker", 1);


	gui_controls_sub = n.subscribe("/arbe/settings/gui_controls", 1, gui_controls_callback);
    slam_active_sub = n.subscribe("/arbe/settings/enable_slam", 1, slam_enable_callback);

	set_disp_processed_pc(false);

	objects_sub = n.subscribe("/arbe/processed/objects/"+std::to_string(arg_radar_id), 1, slam_read_callback);
    raw_cam_sub= n.subscribe("/cv_camera_"+std::to_string(arg_radar_id)+"/image_raw/compressed", 1, SI_slam_on_cam_callback);
	gige_cam_sub= n.subscribe("/pylon_camera_node_"+std::to_string(arg_radar_id)+"/image_color/compressed", 1, SI_slam_on_cam_callback);
	if(arg_radar_id == 0)
	{
        raw_cam_legacy_sub= n.subscribe("/cv_camera/image_raw/compressed", 1, SI_slam_on_cam_callback);
        gige_cam_legacy_sub= n.subscribe("/pylon_camera_node/image_color/compressed", 1, SI_slam_on_cam_callback);
	}
	else
		master_slam_sub = n.subscribe<arbe_msgs::arbeSlamMsg>("/arbe/processed/objects/0", 1, master_slam_read_callback);
//	camera_params_sub = n.subscribe("/arbe/settings/camera_params/"+std::to_string(arg_radar_id), 1, calc_camera_transform_CB);
	camera_params_sub = n.subscribe("/arbe/settings/camera_projection/"+std::to_string(arg_radar_id), 1, prj_receive_CB);
	gui_commands_sub = n.subscribe("/arbe/settings/gui_commands", 1, gui_message_callback);
	FS_road_inclination_sub = n.subscribe("/arbe/processed/road_inclination/"+std::to_string(arg_radar_id),1,road_inclination_callback);
	disp_FS_on_pc_sub = n.subscribe("/arbe/settings/disp_fs_on_pc",1,choose_fs_disp_callback);
	installation_error_fix_sub = n.subscribe("/arbe/settings/installation_error_ang/"+std::to_string(arg_radar_id),1,fix_installation_callback);
	restore_defaults_sub = n.subscribe("/arbe/settings/restore_defaults",1,restore_defaults_callback);
	floating_text_angle_sub = n.subscribe("/arbe/settings/floating_text_phi",10, change_text_phi_callback);

	if(fabs(sin(arg_radar_yaw_angle * M_PI / 180)) >= sin( 5 * M_PI / 180))
	{
		trigger_side_slam_sub = n.subscribe("/arbe/settings/trigger_side_looking_slam", 1, side_slam_triggered_callback);
	}
	else
	{
		trigger_side_slam_sub = n.subscribe("/arbe/settings/trigger_side_looking_slam", 1, front_side_slam_triggered_callback);
	}
	fs_display_sub= n.subscribe("/arbe/processed/free_space/display_polygon/"+std::to_string(arg_radar_id), 1, FS_disp_CB);

	arbe_msgs::arbeCameraInstallationParams::Ptr cam_params = cam_transform_defaults();
	calc_camera_transform(cam_params);

    // create a thread for handling the SLAM visualisation on Camera.
    ret = pthread_create(&si_slam_cam_thread_id,NULL,&si_slam_cam_thread,NULL);

	prepare_basic_markers();

	sleep(1);

	n.getParam("/arbe_gui/enable_gui", enable_gui);
	if (enable_gui == false)
	{
		slam_enable_pub = n.advertise<arbe_msgs::arbeBoolWithTime>("/arbe/settings/enable_slam",10,true);
		free_space_enable_pub = n.advertise<arbe_msgs::arbeBoolWithTime>("/arbe/free_space/enable",10,true);
		ROS_INFO("Arbe real-time AI is enabled");
		arbe_msgs::arbeBoolWithTime def_msg;
		def_msg.flag = true;
		def_msg.header.stamp = ros::Time::now();
		slam_enable_pub.publish(def_msg);
		def_msg.header.stamp = ros::Time::now();
		free_space_enable_pub.publish(def_msg);

		ROS_WARN("Silent gui - always enable slam");
		set_disp_processed_pc(true);
		set_disp_objects(true);
	}

	ros::Rate loop_rate(100);
	while (ros::ok())
	{

		// Process
        handle_pc_frame();
		handle_slam_frame();

		ros::spinOnce();
		loop_rate.sleep();
	}

}
