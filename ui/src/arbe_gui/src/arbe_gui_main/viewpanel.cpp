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


#include "geometry_msgs/PolygonStamped.h"
#include "geometry_msgs/Point32.h"
#include "mainwindow.h"
#include "Per_radar_settings.hpp"

#include "sensor_msgs/CompressedImage.h"
#include "sensor_msgs/Image.h"
#include <image_transport/image_transport.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <ros/ros.h>
#include <opencv2/highgui/highgui.hpp>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>
#include "std_msgs/UInt8MultiArray.h"
#include "std_msgs/UInt16.h"
#include "std_msgs/Int8.h"
#include "std_msgs/Float64.h"
#include "std_msgs/Float32.h"
#include "std_msgs/Float32MultiArray.h"
#include <std_msgs/UInt8.h>
#include "arbe_msgs/arbeBookmarkMsg.h"
#include "arbe_msgs/arbeSlamMsg.h"
#include "arbe_msgs/arbeRdInclination.h"
#include "arbe_msgs/arbeFusionClasses.h"
#include "common.hpp"
#include "Utils.h"
#include "rqt_gauges/my_plugin.h"
#include "rqt_gauges/qcgaugewidget.h"
#include "ctkrangeslider.h"
#include "viewpanel.h"
#include <vector>
//#define ENABLE_MESH_DISPLAY

#define CTRL_SOCKET 0
#define DEFAULT_AZIMUTH_BIN 0
extern void Ntc_Mode_Set();
extern void Cfar_Mode_Set();
extern void radar_set_params();
extern void radar_change_seq();
extern void radar_start_transmit();
extern void radar_stop_transmit();

extern void radar_launch_pointcloud_nodes();
extern void radar_quit();
extern void rosbag_start_recording();
extern void rosbag_stop_recording();
extern void pubGUIcontrols();
extern void render_3d_car(float x, float y, float yaw);
extern void update_basic_markers( void );

extern int terminating;
extern std::string ColoringType;
extern std::string RangeType;
extern std::string mode;

extern int radar_id;
extern float radar_x_offset;
extern float radar_y_offset;
extern float radar_z_offset;
extern float radar_yaw_angle;
extern float radar_pitch_angle;

//extern float ant_euler_beta;
//extern float ant_euler_gamma;

float cam_euler_fs_alpha_backoff = -2.2;
extern float cam_euler_alpha;
extern float cam_euler_beta;
extern float cam_euler_gamma;

extern int threshold4d;
extern int AsphaltRoughness;
extern int DynamicAzimuth;
extern int DynamicElevation;
extern bool ntc_3d_enabled;
extern bool ntc_4d_enabled;
extern bool cfar_3d_enabled;
extern bool cfar_4d_enabled;
extern bool phase_enabled;

extern float MinDoppler;
extern float MaxDoppler;
extern int radar_connected;
extern int rosbag_recording;
extern int grid_cell_size;
extern float frames_per_second;
extern int selectedAzimuthBin;
extern QString radar_region;
extern float MinHeight;
extern float MaxHeight;
int DetectionMemoryTime = 0;
extern int marker_text_size;
extern QRect camera_geometry;

QSlider* thickness_slider;
QSlider* cell_size_slider;
QComboBox* ColoringCombo;
QComboBox* RangeCombo;
QComboBox* ModeCombo;
QComboBox* RadarIdCombo;
QComboBox* 	lidarIdCombo;
QSlider* sensitivity_slider;
QSlider* asphalt_roughness_slider;
QSlider* dynamic_azimuth_slider;
QSlider* dynamic_elevation_slider;
QSlider* azimuth_bin_slider;
QSlider* min_height_slider;
QSlider* max_height_slider;
ctkRangeSlider* cc_slider;
ctkRangeSlider* mm_doppler_slider;

QSlider* radar_x_offset_slider;
//QSlider* radar_y_offset_slider;
QSlider* radar_z_offset_slider;
QSlider* radar_yaw_angle_slider;
QSlider* radar_pitch_angle_slider;

QSlider* cam_rel_alpha;
QSlider* cam_rel_beta;
QSlider* cam_rel_gamma;

QSlider* vis_text_phi_slider;

QDockWidget *dock;
QDockWidget *dock_param;

QLabel* sensitivity_label;
QLabel* dynamic_azimuth_label;
QLabel* dynamic_elevation_label;
QLabel* azimuth_bin_label;
QLabel* thickness_label;
QLabel* cell_size_label;
QLabel* radar_x_offset_label;
//QLabel* radar_y_offset_label;
QLabel* radar_z_offset_label;
QLabel* radar_yaw_angle_label;
QLabel* radar_pitch_angle_label;
QLabel* decay_time_label;
QLabel* view_label;
QLabel* min_height_label;
QLabel* max_height_label;
QLabel* sensitivity_hint_label_l;
QLabel* sensitivity_hint_label_h;
QLabel* cc_min_label;
QLabel* cc_max_label;
QLabel* cc_label;
QLabel* mm_doppler_min_label;
QLabel* mm_doppler_max_label;
QLabel* mm_doppler_label;
QLabel* asphalt_roughness_label;
QLabel* vis_text_phi_label;


QLabel* cam_alpha_label;
QLabel* cam_beta_label;
QLabel* cam_gamma_label;
QLabel* overlay_text_label;
QLabel* camera_calibration_file;

QLabel* lidar_connect_label;
QLabel* lidar_disconnect_label;
QLabel* lidar_start_label;
QLabel* lidar_stop_label;

QPushButton *radar_connect_button;
QPushButton *record_button;
QPushButton *screen_record_button;
QPushButton *radar_start_stop_button;
QPushButton *radar_pause_button;
QPushButton *car_view_button;
QPushButton *top_car_view_button;
QPushButton *top_view_button;
QPushButton *side_view_button;

QPushButton *lidar_connect_button;
QPushButton *lidar_disconnect_button;
QPushButton *lidar_start_button;
QPushButton *lidar_stop_button;

QWidget* CameraWidget[MAX_RADARS];
QWidget* FreeSpaceWidget;
QcGaugeWidget* SpeedometerWidget;
QPushButton *cam_calib_btn;
QPushButton *btn_calc_ant_height_tilt;
QRadioButton *radar_text_radio;
QPushButton *bookmark_button;
QTextEdit*  para_show_1;
QTextEdit*  para_show_2;
QTextEdit*  para_show_3;
QTextEdit*  para_show_4;
QLabel* para_show_1_label;
QLabel* para_show_2_label;
QLabel* para_show_3_label;
QLabel* para_show_4_label;

static ros::Subscriber rd_inclination_sub;
static ros::Subscriber raw_cam_sub;
static ros::Subscriber gige_cam_sub;
static ros::Subscriber capture_for_training_sub;
static ros::Subscriber fs_display_sub;
static ros::Subscriber slam_objs_sub;
static ros::Subscriber bookmark_sub;
static ros::Subscriber cmnd_line_view_sub;
static ros::Subscriber cmnd_line_color_code_sub;
static ros::Subscriber cmnd_line_screen_record_sub;
static ros::Subscriber cmnd_line_record_sub;
static ros::Subscriber cmnd_line_euler_alpha_sub;
static ros::Subscriber cmnd_line_euler_beta_sub;
static ros::Subscriber cmnd_line_euler_gamma_sub;
static ros::Subscriber isOmegaCalibrated_sub;
static ros::Subscriber arbe_status_sub;
static ros::Subscriber arbe_targets_sub;
static ros::Subscriber arbe_radar_calibration_sub;
static ros::Subscriber arbe_ant_height_tilt_sub_arr[MAX_RADARS];
static std::vector<ros::Subscriber> arbe_targets_subs;
static bool radarIdCombo_ready = false;
static int connected_radars = 0;

//static ros::Subscriber disp_perspective_sub;
//static ros::Subscriber disp_colorcode_sub;
static  geometry_msgs::PolygonStamped FS_display_polygon;
static ros::Publisher  arbe_capture_pub;
static ros::Publisher  img_for_dltrain_pub;
static ros::Publisher  bookmark_pub;
static ros::Publisher  radarHnP_pub;
static ros::Publisher AsphaltRoughness_pub;
static ros::Publisher FloatingTextPhi_pub;

static bool dl_training_view_active = false;
static ros::Publisher  egoVel_pub;
static ros::Publisher  turnRate_pub;
static ros::Publisher  arbe_gui_commands_pub;
static ros::Publisher  pub_calc_ant_height_tilt[MAX_RADARS];

bool screen_recording = 0;
bool radar_playing = 0;

bool pointcloud_topic_initialized[MAX_RADARS] = {};
int num_of_radars = 1; // first radar pointcloud topic always set
std::vector<int> camera_id;

QcGaugeWidget * mSpeedGauge;
QcNeedleItem *mSpeedNeedle;
QcNeedleItem *mGpsSpeedNeedle;

QcGaugeWidget * mTurnRateGauge;
QcNeedleItem *mTurnRateNeedle;
QcNeedleItem *mImuTurnRateNeedle;

char cv_camera_launch_command[100];
bool isUSBcamera = false;

static float intrinsic_mat[3][3] = {{1526.97,0,934.05},
									{0,1533.03,537.37},
									{0,0,1}};


void imu_egoYaw_cb(float yaw)
{
	mImuTurnRateNeedle->setCurrentValue(yaw * 180 / 3.1415);
}

void gps_egoVel_cb(float gps_speed)
{
	float speed_kph = gps_speed * 3.6;
	mGpsSpeedNeedle->setCurrentValue(speed_kph);
}


void pub_egoVel_cb(float egoVel,float w_z)
{
//	std_msgs::Float64 msg;
//	msg.data = (float)((uint16_t)(egoVel * 3.6)); // convert to km/h
//	egoVel_pub.publish(msg);
	float speed_kph = egoVel * 3.6;
	mSpeedNeedle->setCurrentValue(speed_kph);
	QcLabelItem *label = mSpeedNeedle->label();
	label->setText(QString::number( (uint16_t)speed_kph ));

	float turnRate = w_z * 180.0 / 3.1415;
	mTurnRateNeedle->setCurrentValue(turnRate);
	QcLabelItem *label_w = mTurnRateNeedle->label();
	label_w->setText(QString::number( (int16_t)turnRate ));
}
void pub_radar_height_and_pitch(int index)
{
	if(index > -1)
	{
		ros::NodeHandlePtr node_ptr = boost::make_shared<ros::NodeHandle>();
		arbe_msgs::arbeRdInclination msg;
		radarHnP_pub = node_ptr->advertise<arbe_msgs::arbeRdInclination>("/arbe/settings/radar_height_and_pitch/"+std::to_string(index),1,true);
		msg.header.stamp = ros::Time::now();
		msg.ant_tilt = radar_pitch_angle;
		msg.ant_height = radar_z_offset;
		radarHnP_pub.publish(msg);
	}
}

/* Constructor for the viewpanel. */
viewpanel::viewpanel( QWidget* parent )
	: QWidget( parent )
{
	const QSize button_size = QSize(200, 30);
	const QSize view_button_side = QSize(60, 25);
	const QSize slider_size = QSize(200, 20);

#if ONLY_SHOW_UI
	init_pubs();
#endif
	/* Construct and lay out labels and slider controls. */

#if 0
	thickness_label = new QLabel( "Point Thickness (40cm)" );
	thickness_slider = new QSlider( Qt::Horizontal );
	thickness_slider->setMinimum( 1 );
	thickness_slider->setMaximum( 200 );
	//thickness_slider->setFixedSize(slider_size);
	//int default_cell_size = grid_cell_size;
	cell_size_label = new QLabel( "Grid Cell Size(" + QString::number( grid_cell_size ) + "m)" );
	cell_size_slider = new QSlider( Qt::Horizontal );
	cell_size_slider->setMinimum( 1 );
	cell_size_slider->setMaximum( 100 );

	decay_time_label = new QLabel( "Points Decay Time("+QString::number( DetectionMemoryTime )+"msec)" );
	QSlider* decay_time_slider = new QSlider( Qt::Horizontal );
	decay_time_slider->setMinimum( 0 );
	decay_time_slider->setMaximum( 500 );

	radar_x_offset_label = new QLabel( "X Offset("+QString::number( radar_x_offset * 100)+"cm)" );
	radar_x_offset_slider = new QSlider( Qt::Horizontal );
	radar_x_offset_slider->setMinimum( -500 );
	radar_x_offset_slider->setMaximum( 500 );

#endif
#if ONLY_SHOW_UI
	setradarXOffset( radar_x_offset * 100);
#endif

//	radar_y_offset_label = new QLabel( "Y Offset("+QString::number( radar_y_offset * 100)+"cm)" );
//	radar_y_offset_slider = new QSlider( Qt::Horizontal );
//	radar_y_offset_slider->setMinimum( -500 );
//	radar_y_offset_slider->setMaximum( 500 );
//	setradarYOffset( radar_y_offset * 100);


#if 0
	radar_z_offset_label = new QLabel( "Z Offset("+QString::number( radar_z_offset * 100)+"cm)" );
	radar_z_offset_slider = new QSlider( Qt::Horizontal );
	radar_z_offset_slider->setMinimum( -500 );
	radar_z_offset_slider->setMaximum( 500 );
	setradarZOffset( radar_z_offset * 100);
	std::cout << " until here " <<  __LINE__ << std::endl;

	radar_yaw_angle_label = new QLabel( "Yaw Angle ("+QString::number( (int)(radar_yaw_angle/3.1415*180) )+" deg)" );
	radar_yaw_angle_slider = new QSlider( Qt::Horizontal );
	radar_yaw_angle_slider->setMinimum( -180 );
	radar_yaw_angle_slider->setMaximum( 180 );
	setRadarYawAngle(radar_yaw_angle/*/3.1415*180*/);
	std::cout << " until here " <<  __LINE__ << std::endl;

	radar_pitch_angle_label = new QLabel( "Pitch ("+QString::number( (int)(radar_pitch_angle/3.1415*180/10) )+" deg)" );
	radar_pitch_angle_slider = new QSlider( Qt::Horizontal );
	radar_pitch_angle_slider->setMinimum( -100 );
	radar_pitch_angle_slider->setMaximum( 100 );
	setRadarPitchAngle(radar_pitch_angle/*/3.1415*180*/);
	std::cout << " until here " <<  __LINE__ << std::endl;


#endif

	QLabel* coloring_label = new QLabel( "Color By" );
	ColoringCombo = new QComboBox;
	ColoringCombo->addItem(tr("Doppler"));
	ColoringCombo->addItem(tr("Doppler Gradient"));
	ColoringCombo->addItem(tr("Amplitude"));
	ColoringCombo->addItem(tr("Amplitude-Flat"));
	ColoringCombo->addItem(tr("Elevation"));
	ColoringCombo->addItem(tr("Range/Doppler"));
#if 0
	azimuth_bin_label = new QLabel( "Azimuth Bin ("+QString::number( DEFAULT_AZIMUTH_BIN )+")" );
	azimuth_bin_slider = new QSlider( Qt::Horizontal );
	azimuth_bin_slider->setMinimum( -64 );
	azimuth_bin_slider->setMaximum( 64 );

	QLabel* range_label = new QLabel( "Range" );
	RangeCombo = new QComboBox;
	/* No Short Range mode in Japan */
	ROS_DEBUG("radar region = %s\n",radar_region.toStdString().c_str());
	if (radar_region != "Japan")
		RangeCombo->addItem(tr("Short"));
	RangeCombo->addItem(tr("Mid"));
	RangeCombo->addItem(tr("Long"));
	if (radar_region != "Japan")
	RangeCombo->addItem(tr("Ultra-Long"));

	QLabel* mode_label = new QLabel( "Mode" );
	ModeCombo = new QComboBox;
	ModeCombo->addItem(tr("3d"));
	ModeCombo->addItem(tr("4d"));

	min_height_label = new QLabel( "Min Height ("+QString::number( (float)MinHeight ,'f',2 )+"m)" );
	min_height_slider = new QSlider( Qt::Horizontal );
	min_height_slider->setMinimum( -1000 );
	min_height_slider->setMaximum( 200 );

	max_height_label = new QLabel( "Max Height ("+QString::number( (float)MaxHeight,'f',2 )+"m)" );
	max_height_slider = new QSlider( Qt::Horizontal );
	max_height_slider->setMinimum( 0 );
	max_height_slider->setMaximum( 2000 );

	sensitivity_label = new QLabel( "Sensitivity ("+QString::number( threshold4d )+")" );
	sensitivity_hint_label_l = new QLabel( "(Low)");
        sensitivity_hint_label_l->setWindowOpacity(0.5);
	sensitivity_hint_label_l->setAlignment(Qt::AlignLeft | Qt::AlignBottom);
	sensitivity_hint_label_h = new QLabel( "(High)");
        sensitivity_hint_label_h->setWindowOpacity(0.5);
	sensitivity_hint_label_h->setAlignment(Qt::AlignRight | Qt::AlignBottom);

	mm_doppler_label = new QLabel( "Doppler [m/s]");
	mm_doppler_slider = new ctkRangeSlider(Qt::Horizontal);
	mm_doppler_slider->setMinimum( -1000 );
	mm_doppler_slider->setMaximum( 1000 );

	mm_doppler_min_label = new QLabel( QString::number( MinDoppler ,'f',1 ));
		mm_doppler_min_label->setWindowOpacity(0.5);
	mm_doppler_min_label->setAlignment(Qt::AlignLeft | Qt::AlignBottom);
	mm_doppler_max_label = new QLabel( QString::number( MaxDoppler ,'f',1 ));
		mm_doppler_max_label->setWindowOpacity(0.5);
	mm_doppler_max_label->setAlignment(Qt::AlignRight | Qt::AlignBottom);
	mm_doppler_slider->setStyleSheet("QSlider::groove:horizontal { "
					  "border: 1px solid #999999; "
					  "height: 3px; "
					  "background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #B1B1B1, stop:1 #c4c4c4); "
					  "background-color: rgba(255, 255, 255, 10);"
					  "margin: 30px; "
					  "} "
					  "QSlider::handle:horizontal { "
					  "background-color: white; "
					  "border: 1px solid #999999; "
					  "width: 10px; "
					  "height: 40px; "
					  "margin: -6px 1px; "
					  "} ");

	cc_label = new QLabel( "Color coding");
	cc_slider = new ctkRangeSlider(Qt::Horizontal);
	cc_slider->setMinimum(-100);
	cc_slider->setMaximum(100);
	cc_min_label = new QLabel( "0");
		cc_min_label->setWindowOpacity(0.5);
	cc_min_label->setAlignment(Qt::AlignLeft | Qt::AlignBottom);
	cc_max_label = new QLabel( "8");
		cc_max_label->setWindowOpacity(0.5);
	cc_max_label->setAlignment(Qt::AlignRight | Qt::AlignBottom);
	cc_slider->setStyleSheet("QSlider::groove:horizontal { "
					  "border: 1px solid #999999; "
					  "height: 3px; "
					  "background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #B1B1B1, stop:1 #c4c4c4); "
					  "background-color: rgba(255, 255, 255, 10);"
					  "margin: 30px; "
					  "} "
					  "QSlider::handle:horizontal { "
					  "background-color: white; "
					  "border: 1px solid #999999; "
					  "width: 10px; "
					  "height: 40px; "
					  "margin: -6px 1px; "
					  "} ");
	cc_slider->setFixedSize(slider_size);


	sensitivity_slider = new QSlider( Qt::Horizontal );
	sensitivity_slider->setStyleSheet("QSlider::groove:horizontal { "
                      "border: 1px solid #999999; "
                      "height: 3px; "
                      "background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #B1B1B1, stop:1 #c4c4c4); "
                      "background-color: rgba(255, 255, 255, 10);"
                      "margin: 45px; "
                      "} "
                      "QSlider::handle:horizontal { "
                      "background-color: white; "	
                      "border: 1px solid #999999; "
                      "width: 10px; "
                      "height: 40px; "
                      "margin: -6px 1px; "
                      "} ");

	sensitivity_slider->setMinimum( -10 );
	sensitivity_slider->setMaximum( 50 );
        sensitivity_slider->setWindowOpacity(0.5);
	sensitivity_slider->setFixedSize(slider_size);
	sensitivity_slider->setInvertedAppearance(true);


	asphalt_roughness_label = new QLabel( "Asphalt roughness" );
	asphalt_roughness_slider = new QSlider( Qt::Horizontal );
	asphalt_roughness_slider->setMinimum( -5 );
	asphalt_roughness_slider->setMaximum( 5 );

	vis_text_phi_label = new QLabel( "Floating text phi" );
	vis_text_phi_slider = new QSlider( Qt::Horizontal );
	vis_text_phi_slider->setMinimum( 0 );
	vis_text_phi_slider->setMaximum( 360 );


	dynamic_azimuth_label = new QLabel( "Dynamic Azimuth ("+QString::number( DynamicAzimuth )+")" );
	dynamic_azimuth_slider = new QSlider( Qt::Horizontal );
	dynamic_azimuth_slider->setMinimum( 1 );
	dynamic_azimuth_slider->setMaximum( 50 );

	dynamic_elevation_label = new QLabel( "Dynamic Elevation ("+QString::number( DynamicElevation )+")");
	dynamic_elevation_slider = new QSlider( Qt::Horizontal );
	dynamic_elevation_slider->setMinimum( 1 );
	dynamic_elevation_slider->setMaximum( 50 );

#endif

	//view_label = new QLabel( "View Angle:");

/* dock plane*/

	dock = new QDockWidget(tr("Lidar Ui Mainwindow"), this);
#if 0
	delete dock->titleBarWidget(); 
	QWidget *widget_empty = new QWidget();
	dock->setTitleBarWidget(widget_empty);
#endif
	//dock_param = new QDockWidget(tr("para show Panel"), this);
	QWidget* multiWidget = new QWidget();
	//QVBoxLayout* controls = new QVBoxLayout ;
	QGridLayout* controls = new QGridLayout ;
    //controls->setColumnStretch(0, 1);  
    //controls->setColumnStretch(1, 3); 
	QGroupBox *controlsBox = new QGroupBox(tr("Basic Controls:"));
	QGridLayout* controls_layout = new QGridLayout;
	//QVBoxLayout* views = new QVBoxLayout;
	//views_layout->addWidget( view_label, 0, 6);
/*  view box */
	QGroupBox *viewsBox = new QGroupBox(tr("Data indicators:"));
	QGridLayout* views_layout = new QGridLayout;
#if 0
	car_view_button = new QPushButton("Car", this);
	car_view_button->setFixedSize(view_button_side);
	top_car_view_button = new QPushButton("Top Car", this);
	top_car_view_button->setFixedSize(view_button_side);
	top_view_button = new QPushButton("Top", this);
	top_view_button->setFixedSize(view_button_side);
	side_view_button = new QPushButton("3rd prs.", this);
	side_view_button->setFixedSize(view_button_side);
#endif
std::vector<QLabel* > paraLabel;
std::vector<QTextEdit* > paraText;
for(int i = 0 ; i < 21 ; i++){
	paraLabel.emplace_back(new QLabel("para  "+QString::number( i )));
	paraText.emplace_back(new QTextEdit);
	paraText[i]->setReadOnly(true);
}

#if 1
for(int j = 0;  j < 3; j++){
	for(int k = 0 ; k  < 7;  k++){
		views_layout->addWidget(paraLabel[j * 7 + k],  j,  2 * k);
		views_layout->addWidget(paraText[j  * 7 + k],  j,  1 + 2 * k);
	}
}
#endif

#if 0
	views_layout->addWidget( top_view_button, 0, 0);
	views_layout->addWidget( top_car_view_button, 0, 1);
	views_layout->addWidget( car_view_button, 1, 0);
	views_layout->addWidget( side_view_button, 1, 1);
#endif
	viewsBox->setLayout(views_layout);

#if 0
	cam_alpha_label = new QLabel( "a ("+QString::number( cam_euler_alpha,'f',1 )+")" );
	cam_rel_alpha = new QSlider( Qt::Horizontal );
	cam_rel_alpha->setMinimum( -150 );
	cam_rel_alpha->setMaximum( 150 );
	cam_rel_alpha->setValue(cam_euler_alpha*10);
	cam_beta_label = new QLabel( "b ("+QString::number( cam_euler_beta,'f',1 )+")" );
	cam_rel_beta = new QSlider( Qt::Horizontal );
	cam_rel_beta->setMinimum( -150 );
	cam_rel_beta->setMaximum( 150 );
	cam_rel_beta->setValue(cam_euler_beta*10);
	cam_gamma_label = new QLabel( "g ("+QString::number( cam_euler_gamma,'f',1 )+")" );
	cam_rel_gamma = new QSlider( Qt::Horizontal );
	cam_rel_gamma->setMinimum( -150 );
	cam_rel_gamma->setMaximum( 150 );
	cam_rel_gamma->setValue(cam_euler_gamma*10);
#endif

#if 0
	QGroupBox *radar_calibration = new QGroupBox(tr("Radar Calibration:"));
	QGridLayout* radar_calibration_layout = new QGridLayout;
	cam_calib_btn = new QPushButton("Camera calib", this);
	cam_calib_btn->setFixedSize(QSize(100,25));
	btn_calc_ant_height_tilt = new QPushButton("Ant. calib", this);
	btn_calc_ant_height_tilt->setFixedSize(QSize(100,25));
	radar_text_radio = new QRadioButton("Disp. text", this);
	QLabel* radar_id_label = new QLabel( "Radar ID" );
	RadarIdCombo = new QComboBox;
//	radar_text_radio->setFixedSize(QSize(100,30));
	radar_calibration_layout->addWidget( radar_id_label, 0, 0 );
	radar_calibration_layout->addWidget( RadarIdCombo, 0, 1);
//	radar_calibration_layout->addWidget( radar_yaw_angle_label, 0, 2);
//	radar_calibration_layout->addWidget( radar_yaw_angle_slider, 0, 3);
	radar_calibration_layout->addWidget( radar_pitch_angle_label, 1, 0);
	radar_calibration_layout->addWidget( radar_pitch_angle_slider, 1, 1);
//	radar_calibration_layout->addWidget( radar_x_offset_label, 1, 2 );
//	radar_calibration_layout->addWidget( radar_x_offset_slider, 1, 3 );
//	radar_calibration_layout->addWidget( radar_y_offset_label, 2, 0 );
//	radar_calibration_layout->addWidget( radar_y_offset_slider, 2, 1 );
	radar_calibration_layout->addWidget( radar_z_offset_label, 2, 0 );
	radar_calibration_layout->addWidget( radar_z_offset_slider, 2, 1 );
	radar_calibration_layout->addWidget( cam_alpha_label, 0, 2);
	radar_calibration_layout->addWidget( cam_rel_alpha, 0, 3);
	radar_calibration_layout->addWidget( cam_beta_label, 1, 2);
	radar_calibration_layout->addWidget( cam_rel_beta, 1, 3);
	radar_calibration_layout->addWidget( cam_gamma_label, 2, 2);
	radar_calibration_layout->addWidget( cam_rel_gamma, 2, 3);
	radar_calibration_layout->addWidget( cam_calib_btn, 0, 4);
    radar_calibration_layout->addWidget(btn_calc_ant_height_tilt, 2 , 4);
	radar_calibration_layout->addWidget( radar_text_radio, 1, 4);
	radar_calibration->setLayout(radar_calibration_layout);
#endif
	//lidar_connect_label = new QLabel( "Connect");
	lidar_connect_button = new QPushButton("Connect", this);
	//lidar_disconnect_label = new QLabel( "Disconnect");
	lidar_disconnect_button = new QPushButton("Disconnect", this);
	lidar_start_button = new QPushButton("Start", this);
	lidar_stop_button = new QPushButton("Stop", this);
	lidarIdCombo =  new QComboBox;
	QLabel* lidar_id_label = new QLabel( "Lidar ID" );

	QLabel* mode_label = new QLabel( "Mode" );
	ModeCombo = new QComboBox;
	ModeCombo->addItem(tr("3d"));
	ModeCombo->addItem(tr("4d"));

	QLabel* range_label = new QLabel( "Range" );
	RangeCombo = new QComboBox;
	RangeCombo->addItem(tr("Short"));
	RangeCombo->addItem(tr("Mid"));
	RangeCombo->addItem(tr("Long"));
	RangeCombo->addItem(tr("Ultra-Long"));

	controls_layout->addWidget( lidar_id_label, 0, 0 );
	controls_layout->addWidget( lidarIdCombo, 0, 1 );
	controls_layout->addWidget( lidar_connect_button, 1, 0 );
	controls_layout->addWidget( lidar_disconnect_button, 1, 1 );
	controls_layout->addWidget( lidar_start_button, 2, 0 );
	controls_layout->addWidget( lidar_stop_button, 2, 1 );

	controls_layout->addWidget( coloring_label, 0, 2 );
	controls_layout->addWidget( ColoringCombo, 0, 3);
	controls_layout->addWidget( mode_label, 1, 2 );
	controls_layout->addWidget( ModeCombo, 1, 3);
	controls_layout->addWidget( range_label, 2, 2 );
	controls_layout->addWidget( RangeCombo, 2, 3 );

#if 0
	controls_layout->addWidget( thickness_label, 0, 0 );
	controls_layout->addWidget( thickness_slider, 0, 1 );
	controls_layout->addWidget( cell_size_label, 0, 2 );
	controls_layout->addWidget( cell_size_slider, 0, 3 );
	//controls_layout->addWidget( min_height_label, 1, 4);
	//controls_layout->addWidget( min_height_slider, 1, 5);
	//controls_layout->addWidget( max_height_label, 1, 6);
	//controls_layout->addWidget( max_height_slider, 1, 7);
	controls_layout->addWidget( mode_label, 2, 0 );

	controls_layout->addWidget( asphalt_roughness_label, 2, 4 );
	controls_layout->addWidget( asphalt_roughness_slider, 2, 5 );
	controls_layout->addWidget( vis_text_phi_label, 2, 4 );
	controls_layout->addWidget( vis_text_phi_slider, 2, 5 );
	vis_text_phi_label->setVisible(false);
	vis_text_phi_slider->setVisible(false);
	vis_text_phi_slider->setEnabled(false);


	controls_layout->addWidget( ModeCombo, 2, 1 );
	controls_layout->addWidget( range_label, 2, 2 );
	controls_layout->addWidget( RangeCombo, 2, 3 );
	controls_layout->addWidget( coloring_label, 0, 4 );
	controls_layout->addWidget( ColoringCombo, 0, 5);
	controls_layout->addWidget( azimuth_bin_label, 1, 4 );
	controls_layout->addWidget( azimuth_bin_slider, 1, 5 );
	controls_layout->addWidget( sensitivity_label, 3, 0);
	controls_layout->addWidget( sensitivity_hint_label_l, 3, 1);
	controls_layout->addWidget( sensitivity_hint_label_h, 3, 1);
	controls_layout->addWidget( sensitivity_slider, 3, 1);
	controls_layout->addWidget( dynamic_azimuth_label, 3, 2);
	controls_layout->addWidget( dynamic_azimuth_slider, 3, 3);
	controls_layout->addWidget( dynamic_elevation_label, 3, 4);
	controls_layout->addWidget( dynamic_elevation_slider, 3, 5);
	controls_layout->addWidget( cc_label, 1, 2);
	controls_layout->addWidget( cc_min_label, 1, 3);
	controls_layout->addWidget( cc_max_label, 1, 3);
	controls_layout->addWidget( cc_slider, 1, 3);
	controls_layout->addWidget( mm_doppler_label, 1, 0);
	controls_layout->addWidget( mm_doppler_min_label, 1, 1);
	controls_layout->addWidget( mm_doppler_max_label, 1, 1);
	controls_layout->addWidget( mm_doppler_slider, 1, 1);
#endif
	controlsBox->setLayout(controls_layout);

	//controls->addLayout ( controls_layout );

	//controls->addWidget ( viewsBox, 0, 1 );
//	controls->addWidget ( cam_euler_angs );
//controls->addWidget ( radar_calibration );

//	cam_euler_angs->show();
	//radar_calibration->show(); ///?

	//QHBoxLayout* controls_views = new QHBoxLayout;
	//controls_views->addLayout(controls);
	//multiWidget->setMaximumHeight(140);

	QPushButton *config_button = new QPushButton("config&ure", this);
	radar_connect_button = new QPushButton("Radar &Connect", this);
	record_button = new QPushButton("Rec&ord", this);
	screen_record_button = new QPushButton("Screen R&ecord", this);
	radar_start_stop_button = new QPushButton("Start &Tx", this);
	radar_pause_button = new QPushButton("&>>", this);
	bookmark_button = new QPushButton("&Bookmark", this);

	config_button->setFixedSize(button_size);
	screen_record_button->setFixedSize(button_size);
	radar_connect_button->setFixedSize(button_size);
	record_button->setFixedSize(button_size);
	radar_start_stop_button->setFixedSize(button_size);
	radar_pause_button->setFixedSize(button_size);
	bookmark_button->setFixedSize(button_size);
	std::cout << " until here " <<  __LINE__ << std::endl;


	/* Construct and lay out render panel. */
	render_panel_ = new rviz::RenderPanel();
	/* Construct and lay out selection panel. */
	selection_panel_ = new rviz::SelectionPanel();
	controls->addWidget ( controlsBox, 0, 0, 1, 2, Qt::AlignLeft);
	controls->addWidget ( render_panel_, 1,  0,  5,  5);
	controls->addWidget ( selection_panel_, 1,  0, 5, 1, Qt::AlignRight);

	//QGridLayout* radar_layout = new QGridLayout;
    QHBoxLayout* buttons_layout = new QHBoxLayout();
 	buttons_layout->addWidget( config_button );
	buttons_layout->addWidget( radar_start_stop_button );
	buttons_layout->addWidget( radar_connect_button );
	buttons_layout->addWidget( record_button );
	buttons_layout->addWidget( screen_record_button );
	buttons_layout->addWidget( bookmark_button );
	buttons_layout->addWidget( radar_pause_button );
	controls->addLayout(buttons_layout, 6, 0);
	multiWidget->setLayout(controls);

	//multiWidget->setMaximumHeight(140);
	dock->setFeatures(QDockWidget::DockWidgetClosable );
	dock->setWidget(multiWidget);
	/**/
	QDockWidget* dockDebug = new QDockWidget(tr("Lidar Debug window"), this);
    dockDebug->setFeatures(QDockWidget::DockWidgetClosable );
	//this->tabifyDockWidget(dock,  dockDebug);

	QVBoxLayout* main_layout = new QVBoxLayout;
	main_layout->addWidget( dock);

#if 0
	radar_layout->addWidget( render_panel_ ,0,0,5,5);
	radar_layout->addWidget( selection_panel_ ,0,4,5,1,Qt::AlignRight);
#endif

	overlay_text_label = new QLabel;
	overlay_text_label->setAutoFillBackground(true);
	overlay_text_label->setIndent(10);
	QPalette p( overlay_text_label->palette());
	p.setColor( QPalette::Window, QColor(Qt::black));
	overlay_text_label->setPalette(p);

	/* Hide the selection panel on start */
	//selection_panel_->hide();
	std::cout << " until here " <<  __LINE__ << std::endl;

#if 0
	showSpeedometer(radar_layout);
	showTurnRate(radar_layout);
#endif
	//main_layout->addLayout(radar_layout);
	//main_layout->addLayout(buttons_layout);
	std::cout << " until here " <<  __LINE__ << std::endl;

	/* Set the top-level layout for this viewpanel widget. */
	setLayout( main_layout );

	/* Initialize the main RViz classes */
	manager_ = new rviz::VisualizationManager( render_panel_ );

	render_panel_->setBackgroundColor( Ogre::ColourValue(0,0,0,0.3));
	render_panel_->initialize( manager_->getSceneManager(), manager_ );
	selection_panel_->initialize( manager_ );
	manager_->initialize();
	manager_->startUpdate();
	manager_->getToolManager()->addTool("rviz_plugin_arbe_points_publisher/ArbePointsPublisher");

	/* Create a Grid display. */
	grid_ = manager_->createDisplay( "rviz/Grid", "adjustable grid", true );
	ROS_ASSERT( grid_ != NULL );

	//grid_->cc( "Alpha" )->setValue( "0.5" );
	grid_->subProp( "Line Style" )->setValue( "Lines" );
	grid_->subProp( "Plane" )->setValue( "XY" );
	grid_->subProp( "Cell Size" )->setValue( grid_cell_size ); 
	grid_->subProp( "Plane Cell Count" )->setValue( "1000" );
	grid_->subProp( "Offset" )->setValue( "X: -100 Y: 0 Z: 0" );
	grid_->subProp( "Reference Frame" )->setValue( "<Fixed Frame>" );

	manager_->getViewManager()->setCurrentViewControllerType("rviz/XYOrbit");
	manager_->getViewManager()->getCurrent()->subProp("Invert Z Axis")->setValue("false");
	follower_view_ = false;
	std::cout << " until here " <<  __LINE__ << std::endl;

#if ONLY_SHOW_UI
	setTopCarView();
#endif
	/* Create the radar pointcloud fixed frame. */
	manager_->setFixedFrame("image_radar");

//	SlamArray_[0] = manager_->createDisplay( "rviz/MarkerArray", "MarkerArray", true );
//	ROS_ASSERT( SlamArray_[0] != NULL );
//	SlamArray_[0]->subProp("Marker Topic")->setValue("/arbe/rviz/objects_0");

	Car_ = manager_->createDisplay( "rviz/Marker", "Marker", true );
	ROS_ASSERT( Car_ != NULL );
	Car_->subProp("Marker Topic")->setValue("/arbe/rviz/car_marker");

	FloatingText_ = manager_->createDisplay( "rviz/Marker", "Marker", true );
	ROS_ASSERT( FloatingText_ != NULL );
	FloatingText_->subProp("Marker Topic")->setValue("/arbe/rviz/floatingText_marker");

	Axes_ = manager_->createDisplay( "rviz/Axes", "Axes", true );
	ROS_ASSERT( Axes_ != NULL );
	Axes_->subProp("Reference Frame")->setValue("odom");
	Axes_->subProp("Length")->setValue("3");

	/* Set defaults */
#if 0
	thickness_slider->setValue( 40 );
	asphalt_roughness_slider->setValue( 0 );
	vis_text_phi_slider->setValue(180);

	cell_size_slider->setValue( grid_cell_size );
	radar_x_offset_slider->setValue( radar_x_offset * 100 );
//	radar_y_offset_slider->setValue( radar_y_offset * 100 );
	radar_z_offset_slider->setValue( radar_z_offset * 100 );
	radar_yaw_angle_slider->setValue((int)(radar_yaw_angle / 3.1415 * 180));
	radar_pitch_angle_slider->setValue((int)(radar_pitch_angle / 3.1415 * 180*10));
	decay_time_slider->setValue( DetectionMemoryTime );
	sensitivity_slider->setValue( threshold4d );
	dynamic_azimuth_slider->setValue( DynamicAzimuth );
	dynamic_elevation_slider->setValue( DynamicElevation );
	min_height_slider->setValue( MinHeight * 100 );
	max_height_slider->setValue( MaxHeight * 100 );
	azimuth_bin_slider->setValue( DEFAULT_AZIMUTH_BIN ); // default to boresight
	selectedAzimuthBin = DEFAULT_AZIMUTH_BIN;
	azimuth_bin_label->setEnabled(false);
	azimuth_bin_slider->setEnabled(false);
	mm_doppler_slider->setValues(MinDoppler * 10, MaxDoppler * 10);

#endif

	int index = ModeCombo->findText(mode.c_str());
	ModeCombo->setCurrentIndex(index);
	index = RangeCombo->findText(RangeType.c_str());
	RangeCombo->setCurrentIndex(index);
	index = ColoringCombo->findText(ColoringType.c_str());
	ColoringCombo->setCurrentIndex(index);
	std::cout << " until here " <<  __LINE__ << std::endl;

#if 0
	float cc_min, cc_max;
	Color_Coding_Min_Max::Instance()->get_converted_values(ColoringType, cc_min, cc_max);
	setMinColorCoding((int)cc_min);
	setMaxColorCoding((int)cc_max);
	cc_slider->setMinimumValue((int)cc_min);
	cc_slider->setMaximumValue((int)cc_max);
	cc_label->setText("Color coding ["+QString::fromStdString(Color_Coding_Min_Max::Instance()->get_units(ColoringType))+"]");
#endif

	resize(QDesktopWidget().availableGeometry(this).size() * 0.85);

#if 0
	/* Make signal/slot connections. */
	connect( thickness_slider, SIGNAL( valueChanged( int )), this, SLOT( setThickness( int )));
	connect( cell_size_slider, SIGNAL( valueChanged( int )), this, SLOT( setCellSize( int )));
	connect( radar_x_offset_slider, SIGNAL( valueChanged( int )), this, SLOT( setradarXOffset( int )));
//	connect( radar_y_offset_slider, SIGNAL( valueChanged( int )), this, SLOT( setradarYOffset( int )));
	connect( radar_z_offset_slider, SIGNAL( valueChanged( int )), this, SLOT( setradarZOffset( int )));

	connect( radar_yaw_angle_slider, SIGNAL( valueChanged( int )), this, SLOT( setRadarYawAngle( int )));
	connect( radar_pitch_angle_slider, SIGNAL( valueChanged( int )), this, SLOT( setRadarPitchAngle( int )));
	connect( cam_rel_alpha, SIGNAL( valueChanged( int )), this, SLOT( setCamEulerAlpha( int )));
	connect( cam_rel_beta, SIGNAL( valueChanged( int )), this, SLOT( setCamEulerBeta( int )));
	connect( cam_rel_gamma, SIGNAL( valueChanged( int )), this, SLOT( setCamEulerGamma( int )));

	connect( cam_calib_btn, SIGNAL( clicked()), this, SLOT( load_camera_calibration( void )));
	connect( btn_calc_ant_height_tilt, SIGNAL( clicked()), this, SLOT( calc_ant_height_tilt( void )));
	connect( radar_text_radio, SIGNAL( clicked()), this, SLOT( choose_which_radars_text( void )));

	//connect( RadarIdCombo, SIGNAL( currentIndexChanged(int )), this, SLOT( setPreRadarSliders( void )));

	//connect( decay_time_slider, SIGNAL( valueChanged( int )), this, SLOT( setPointDecayTime( int )));
        connect( ModeCombo, SIGNAL( currentTextChanged(QString)), this, SLOT( setMode( void )));
	connect( RangeCombo, SIGNAL( currentTextChanged(QString)), this, SLOT( setRange( void )));
	connect( ColoringCombo, SIGNAL( currentTextChanged(QString)), this, SLOT( setColoring( void )));
	connect( sensitivity_slider, SIGNAL( valueChanged( int )), this, SLOT( setThreshold4D( int )));
	connect( asphalt_roughness_slider, SIGNAL( valueChanged( int )), this, SLOT( setAsphaltRoughness( int )));
	connect( vis_text_phi_slider, SIGNAL( valueChanged( int )), this, SLOT( setFloatingTextPhi( int )));
	connect( dynamic_azimuth_slider, SIGNAL( valueChanged( int )), this, SLOT( setDynamicAzimuth( int )));
	connect( dynamic_elevation_slider, SIGNAL( valueChanged( int )), this, SLOT( setDynamicElevation( int )));
	connect( config_button, SIGNAL( clicked()), this, SLOT( show_configuration_dock( void )));
	connect( radar_connect_button, SIGNAL( clicked()), this, SLOT( radar_connect_control( void )));
	connect( record_button, SIGNAL( clicked()), this, SLOT( recording_control( void )));
	connect( screen_record_button, SIGNAL( clicked()), this, SLOT( screen_record( void )));
	connect( mm_doppler_slider, SIGNAL( maximumValueChanged( int )), this, SLOT( setMaxDoppler( int )));
	connect( mm_doppler_slider, SIGNAL( minimumValueChanged( int )), this, SLOT( setMinDoppler( int )));
	connect( min_height_slider, SIGNAL( valueChanged( int )), this, SLOT( setMinHeight( int )));
	connect( max_height_slider, SIGNAL( valueChanged( int )), this, SLOT( setMaxHeight( int )));
	connect( radar_start_stop_button, SIGNAL( clicked()), this, SLOT( radar_start_stop_control( void )));
	connect( radar_pause_button, SIGNAL( clicked()), this, SLOT( radar_pause_control( void )));
	connect( azimuth_bin_slider, SIGNAL( valueChanged( int )), this, SLOT( setAzimuthBin( int )));

	connect( top_view_button, SIGNAL( clicked()), this, SLOT( setTopView( void )));
	connect( top_car_view_button, SIGNAL( clicked()), this, SLOT( setTopCarView( void )));
	connect( car_view_button, SIGNAL( clicked()), this, SLOT( setCarView( void )));
	connect( side_view_button, SIGNAL( clicked()), this, SLOT( toggleFollowerView( void )));
	connect( bookmark_button, SIGNAL( clicked()), this, SLOT( bookmark_control( void )));

	connect( cc_slider, SIGNAL( maximumValueChanged( int )), this, SLOT( setMaxColorCoding( int )));
	connect( cc_slider, SIGNAL( minimumValueChanged( int )), this, SLOT( setMinColorCoding( int )));
#endif

	QShortcut *IncreaeSensitivity = new QShortcut(QKeySequence( Qt::Key_Plus), this);
	connect(IncreaeSensitivity, &QShortcut::activated, this, &viewpanel::IncreaseSensitivity);

	QShortcut *DecreaseSensitivity = new QShortcut(QKeySequence( Qt::Key_Minus), this);
	connect(DecreaseSensitivity, &QShortcut::activated, this, &viewpanel::DecreaseSensitivity);

	QShortcut *IncreaeDynamicAzimuth = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Plus), this);
	connect(IncreaeDynamicAzimuth, &QShortcut::activated, this, &viewpanel::IncreaeDynamicAzimuth);

	QShortcut *DecreaseDynamicAzimuth = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Minus), this);
	connect(DecreaseDynamicAzimuth, &QShortcut::activated, this, &viewpanel::DecreaseDynamicAzimuth);

	QShortcut *IncreaeAsphaltRoughness = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Plus), this);
	connect(IncreaeAsphaltRoughness, &QShortcut::activated, this, &viewpanel::IncreaeAsphaltRoughness);

	QShortcut *DecreaseAsphaltRoughness = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Minus), this);
	connect(DecreaseAsphaltRoughness, &QShortcut::activated, this, &viewpanel::DecreaseAsphaltRoughness);

	QShortcut *IncreaeDynamicElevation = new QShortcut(QKeySequence(Qt::ALT + Qt::Key_Plus), this);
	connect(IncreaeDynamicElevation, &QShortcut::activated, this, &viewpanel::IncreaeDynamicElevation);

	QShortcut *DecreaseDynamicElevation = new QShortcut(QKeySequence(Qt::ALT + Qt::Key_Minus), this);
	connect(DecreaseDynamicElevation, &QShortcut::activated, this, &viewpanel::DecreaseDynamicElevation);

	QShortcut *SetShortRnage = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_S), this);
	connect(SetShortRnage, &QShortcut::activated, this, &viewpanel::SetShortRnage);

	QShortcut *SetMidRnage = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_M), this);
	connect(SetMidRnage, &QShortcut::activated, this, &viewpanel::SetMidRnage);

	QShortcut *SetLongRnage = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_L), this);
	connect(SetLongRnage, &QShortcut::activated, this, &viewpanel::SetLongRnage);

	QShortcut *SetUltraLongRnage = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_U), this);
	connect(SetUltraLongRnage, &QShortcut::activated, this, &viewpanel::SetUltraLongRnage);

	QShortcut *Set3d = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_3), this);
	connect(Set3d, &QShortcut::activated, this, &viewpanel::Set3d);

	QShortcut *Set4d = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_4), this);
	connect(Set4d, &QShortcut::activated, this, &viewpanel::Set4d);

	QShortcut *ColorByElevation = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_E), this);
	connect(ColorByElevation, &QShortcut::activated, this, &viewpanel::ColorByElevation);

	QShortcut *ColorByDoppler = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_D), this);
	connect(ColorByDoppler, &QShortcut::activated, this, &viewpanel::ColorByDoppler);

	QShortcut *ColorByAmplitude = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_A), this);
	connect(ColorByAmplitude, &QShortcut::activated, this, &viewpanel::ColorByAmplitude);

#if ONLY_SHOW_UI
	init_subs();
#endif

	for(int nr = 0; nr < num_of_radars; nr++)
	{
		//RadarIdCombo->addItem(tr(std::to_string(nr).c_str()));//tr("0"));
	}
	radarIdCombo_ready = true;

#if ONLY_SHOW_UI

	render_3d_car(0,-2.5,0);

	follower_view_ = false;
	radar_text_radio->setChecked(true);

	choose_which_radars_text();
#endif
}

// Destructor.
viewpanel::~viewpanel()
{
	delete manager_;
}

void viewpanel::load_camera_calibration( void )
{
	load_camera_calib(false);
	return;
}

void viewpanel::calc_ant_height_tilt(void)
{

    uint32_t radar_id = RadarIdCombo->currentIndex();
    std_msgs::Bool msg;
    msg.data = true;
    pub_calc_ant_height_tilt[radar_id].publish(msg);
    return;
}

void viewpanel::choose_which_radars_text( void )
{
	if(radar_text_radio->isChecked() && radarIdCombo_ready)
	{
		Per_radar_settings::Instance()->radar_for_text = RadarIdCombo->currentIndex();
	}
	else
		Per_radar_settings::Instance()->radar_for_text = -1;

	pubGUIcontrols();
}


void viewpanel::IncreaseSensitivity()
{
	sensitivity_slider->setSliderPosition(sensitivity_slider->sliderPosition()+1);
}

void viewpanel::DecreaseSensitivity()
{
	sensitivity_slider->setSliderPosition(sensitivity_slider->sliderPosition()-1);
}

void viewpanel::IncreaeDynamicAzimuth()
{
	dynamic_azimuth_slider->setSliderPosition(dynamic_azimuth_slider->sliderPosition()+1);
}

void viewpanel::DecreaseDynamicAzimuth()
{
	dynamic_azimuth_slider->setSliderPosition(dynamic_azimuth_slider->sliderPosition()-1);
}

void viewpanel::IncreaeAsphaltRoughness()
{
	asphalt_roughness_slider->setSliderPosition(asphalt_roughness_slider->sliderPosition()+1);
}

void viewpanel::DecreaseAsphaltRoughness()
{
	asphalt_roughness_slider->setSliderPosition(asphalt_roughness_slider->sliderPosition()-1);
}

void viewpanel::IncreaeFloatingTextPhi()
{
	vis_text_phi_slider->setSliderPosition(vis_text_phi_slider->sliderPosition()+10);
}

void viewpanel::DecreaseFloatingTextPhi()
{
	vis_text_phi_slider->setSliderPosition(vis_text_phi_slider->sliderPosition()-10);
}


void viewpanel::IncreaeDynamicElevation()
{
	dynamic_elevation_slider->setSliderPosition(dynamic_elevation_slider->sliderPosition()+1);
}

void viewpanel::DecreaseDynamicElevation()
{
	dynamic_elevation_slider->setSliderPosition(dynamic_elevation_slider->sliderPosition()-1);
}

void viewpanel::SetShortRnage()
{
	int index = RangeCombo->findText("Short");
	if (index >= 0)
		RangeCombo->setCurrentIndex(index);
}

void viewpanel::SetMidRnage()
{
	int index = RangeCombo->findText("Mid");
	RangeCombo->setCurrentIndex(index);
}

void viewpanel::SetLongRnage()
{
	int index = RangeCombo->findText("Long");
	RangeCombo->setCurrentIndex(index);
}

void viewpanel::SetUltraLongRnage()
{
	int index = RangeCombo->findText("Ultra-Long");
	RangeCombo->setCurrentIndex(index);
}

void viewpanel::Set3d()
{
	int index = ModeCombo->findText("3d");
	ModeCombo->setCurrentIndex(index);
}

void viewpanel::Set4d()
{
	int index = ModeCombo->findText("4d");
	ModeCombo->setCurrentIndex(index);
}

void viewpanel::ColorByElevation()
{
	int index = ColoringCombo->findText("Elevation");
	ColoringCombo->setCurrentIndex(index);
}

void viewpanel::ColorByDoppler()
{
	int index = ColoringCombo->findText("Doppler");
	ColoringCombo->setCurrentIndex(index);
}

void viewpanel::ColorByAmplitude()
{
	int index = ColoringCombo->findText("Amplitude");
	ColoringCombo->setCurrentIndex(index);
}

void viewpanel::setThickness( int thickness_percent )
{
	for (int i=0; i < num_of_radars; i++)
	{
		if( pointcloud_[i] != NULL )
		{
			pointcloud_[i]->subProp( "Size (m)" )->setValue( thickness_percent / 100.0f );
			stationary_pointcloud_[i]->subProp( "Size (m)" )->setValue( thickness_percent / 100.0f );
	}
	}
	thickness_label->setText( "Point Thickness ("+QString::number( thickness_percent )+"cm)" );
}

void viewpanel::setCellSize( int cell_size_percent )
{
	if( grid_ != NULL )
	{
	        QStyleOptionSlider opt;
		grid_->subProp( "Cell Size" )->setValue( cell_size_percent );
		grid_cell_size = cell_size_percent;

		update_basic_markers();
	}
	cell_size_label->setText( "Grid Cell Size("+QString::number( cell_size_percent )+"m)" );
}

void viewpanel::setPointDecayTime( int decay_time_ms_slider_value )
{
	/* convert from (msec) to (sec) */
	DetectionMemoryTime = decay_time_ms_slider_value;
	for (int i=0; i < num_of_radars; i++)
	{
		stationary_pointcloud_[i]->subProp("Decay Time")->setValue((float)DetectionMemoryTime / 1000);
	}
	decay_time_label->setText( "Points Decay Time("+QString::number( decay_time_ms_slider_value )+"msec)" );
}

void viewpanel::setAzimuthBin( int azimuth_bin_slider_value )
{
	/* convert from (cm) to (m) */
	selectedAzimuthBin = azimuth_bin_slider_value;
	azimuth_bin_label->setText( "Azimuth Bin ("+QString::number( azimuth_bin_slider_value )+")" );
#if ONLY_SHOW_UI
	pubGUIcontrols();
#endif
}

void viewpanel::setradarXOffset( int radar_x_offset_slider_value )
{
	/* convert from (cm) to (m) */
	radar_x_offset = radar_x_offset_slider_value / 100.0;
	radar_x_offset_label->setText( "X Offset("+QString::number( radar_x_offset_slider_value )+"cm)" );
//	pub_radar_height_and_pitch();
#if ONLY_SHOW_UI
	pubGUIcontrols();
#endif
}

//void viewpanel::setradarYOffset( int radar_y_offset_slider_value )
//{
//	/* convert from (cm) to (m) */
//	radar_y_offset = radar_y_offset_slider_value / 100.0;
//	radar_y_offset_label->setText( "Y Offset("+QString::number( radar_y_offset_slider_value )+"cm)" );
//	pub_radar_height_and_pitch();

//	pubGUIcontrols();
//}

void viewpanel::setradarZOffset(int radar_z_offset_slider_value)
{
	/* convert from (cm) to (m) */
	radar_z_offset = radar_z_offset_slider_value / 100.0;
	radar_z_offset_label->setText( "Z Offset("+QString::number( radar_z_offset_slider_value )+"cm)" );

	int index = -1;
	if(radarIdCombo_ready && Per_radar_settings::Instance()->ant_pitch.size()>0)
	{
		index = RadarIdCombo->currentIndex();
		Per_radar_settings::Instance()->ant_height.at(index)=radar_z_offset;
		pub_radar_height_and_pitch(index);
	}
#if ONLY_SHOW_UI
	pubGUIcontrols();
#endif
}
	
void viewpanel::setRadarYawAngle( int radar_yaw_angle_slider_value )
{
	/* convert from (deg) to (rad) */
	radar_yaw_angle = radar_yaw_angle_slider_value * 3.1415 / 180.0;
	radar_yaw_angle_label->setText( "Yaw Angle ("+QString::number( radar_yaw_angle_slider_value )+" deg)" );
	set_euler_mat(radar_yaw_angle,0,0);
//	pub_radar_height_and_pitch();
#if ONLY_SHOW_UI
	pubGUIcontrols();
#endif
}

void viewpanel::setRadarPitchAngle( int radar_pitch_angle_slider_value )
{
	/* convert from (deg) to (rad) */
	radar_pitch_angle = radar_pitch_angle_slider_value * M_PI / 180.0 / 10;
	radar_pitch_angle_label->setText( "Pitch ("+QString::number( radar_pitch_angle_slider_value / 10.0 )+" deg)" );
	set_euler_mat(radar_pitch_angle,0,0);

	int index = -1;
	if(radarIdCombo_ready && Per_radar_settings::Instance()->ant_pitch.size()>0)
	{
		index = RadarIdCombo->currentIndex();
		Per_radar_settings::Instance()->ant_pitch.at(index)=radar_pitch_angle;
		pub_radar_height_and_pitch(index);
	}

#if ONLY_SHOW_UI
	pubGUIcontrols();
#endif
}

void viewpanel::setCamEulerAlpha( int cam_euler_alpha_slider_value )
{
	/* No need to convert deg ->rad */
	float val = cam_euler_alpha_slider_value / 10.0;
	cam_euler_alpha =  val;
	if (cam_euler_alpha_slider_value==0)
		cam_alpha_label->setText( "a (0)" );
	else
		cam_alpha_label->setText( "a ("+QString::number( val,'f',1 )+")" );

//	rotation_into_R_t(cam_euler_alpha,cam_euler_beta,cam_euler_gamma);
//	calc_cam_projection();
	rotation_into_R_t_and_calc_cam_projection(cam_euler_alpha,cam_euler_beta,cam_euler_gamma);
	if(radarIdCombo_ready && Per_radar_settings::Instance()->ant_pitch.size()>0)
	{
		Per_radar_settings::Instance()->cam_euler_a.at(RadarIdCombo->currentIndex())=cam_euler_alpha;
	}

#if ONLY_SHOW_UI
	pubGUIcontrols();
#endif
}


void viewpanel::setCamEulerBeta( int cam_euler_beta_slider_value )
{
	/* No need to convert deg ->rad */
	float val = cam_euler_beta_slider_value / 10.0;
	cam_euler_beta =  val;
	if (cam_euler_beta_slider_value==0)
		cam_beta_label->setText( "b (0)" );
	else
		cam_beta_label->setText( "b ("+QString::number( val,'f',1 )+")" );

//	rotation_into_R_t(cam_euler_alpha,cam_euler_beta,cam_euler_gamma);
//	calc_cam_projection();
	rotation_into_R_t_and_calc_cam_projection(cam_euler_alpha,cam_euler_beta,cam_euler_gamma);

	if(radarIdCombo_ready && Per_radar_settings::Instance()->ant_pitch.size()>0)
	{
		Per_radar_settings::Instance()->cam_euler_b.at(RadarIdCombo->currentIndex())=cam_euler_beta;
}

#if ONLY_SHOW_UI
	pubGUIcontrols();
#endif
}


void viewpanel::setCamEulerGamma( int cam_euler_gamma_slider_value )
{
	/* No need to convert deg ->rad */
	float val = cam_euler_gamma_slider_value / 10.0;
	cam_euler_gamma =  val;
	if (cam_euler_gamma_slider_value==0)
		cam_gamma_label->setText( "g (0)" );
	else
		cam_gamma_label->setText( "g ("+QString::number( val,'f',1 )+")" );

//	rotation_into_R_t(cam_euler_alpha,cam_euler_beta,cam_euler_gamma);
//	calc_cam_projection();
	rotation_into_R_t_and_calc_cam_projection(cam_euler_alpha,cam_euler_beta,cam_euler_gamma);
	if(radarIdCombo_ready && Per_radar_settings::Instance()->ant_pitch.size()>0)
	{
		Per_radar_settings::Instance()->cam_euler_g.at(RadarIdCombo->currentIndex())=cam_euler_gamma;
	}
#if ONLY_SHOW_UI
	pubGUIcontrols();
#endif

}
void viewpanel::setMinDoppler( int min_doppler_slider_value )
{
	MinDoppler = min_doppler_slider_value / 10.0;
	mm_doppler_min_label->setText( QString::number( MinDoppler ,'f',1) );
}

void viewpanel::setMaxDoppler( int max_doppler_slider_value )
{
	MaxDoppler = max_doppler_slider_value / 10.0;
	mm_doppler_max_label->setText( QString::number( MaxDoppler ,'f',1) );
}

void viewpanel::setMinColorCoding( int min_slider_value )
{
	Color_Coding_Min_Max::Instance()->set_min(ColoringType,min_slider_value/10.0);
	float cc_min, cc_max;
	Color_Coding_Min_Max::Instance()->get_values(ColoringType,cc_min, cc_max);
	cc_min_label->setText( QString::number( cc_min ,'f',1) );
#if ONLY_SHOW_UI
	pubGUIcontrols();
#endif
}

void viewpanel::setMaxColorCoding( int max_slider_value )
{
	Color_Coding_Min_Max::Instance()->set_max(ColoringType,max_slider_value/10.0);
	float cc_min, cc_max;
	Color_Coding_Min_Max::Instance()->get_values(ColoringType,cc_min, cc_max);
	cc_max_label->setText( QString::number( cc_max ,'f',1) );

#if ONLY_SHOW_UI
	pubGUIcontrols();
#endif

}

void viewpanel::setMinHeight( int min_height_slider_value )
{
	MinHeight = min_height_slider_value/100;
	min_height_label->setText( "Min Height ("+QString::number( (float)min_height_slider_value / 100,'f',2 )+")" );
}

void viewpanel::setMaxHeight( int max_height_slider_value )
{
	MaxHeight = max_height_slider_value/100;
	max_height_label->setText( "Max Height ("+QString::number( (float)max_height_slider_value / 100,'f',2 )+")" );
}

void viewpanel::setThreshold4D( int sensitivity_slider_value )
{
	threshold4d = sensitivity_slider_value;
	sensitivity_label->setText( "Sensitivity ("+QString::number( threshold4d )+")" );
//	radar_set_params();
	if(radarIdCombo_ready && Per_radar_settings::Instance()->threshold_4d.size()>0)
	{
		int ind = 0;
		ind = RadarIdCombo->currentIndex();
		Per_radar_settings::Instance()->threshold_4d.at(ind)=threshold4d;

	}

#if ONLY_SHOW_UI
	pubGUIcontrols();
#endif
	qApp->processEvents();
}

void viewpanel::setAsphaltRoughness( int AsphaltRoughness_slider_value )
{
	AsphaltRoughness = AsphaltRoughness_slider_value;
	ros::NodeHandlePtr node_ptr = boost::make_shared<ros::NodeHandle>();
	AsphaltRoughness_pub = node_ptr->advertise<std_msgs::Int8>("/arbe/settings/asphalt_roughness",1,true);
	std_msgs::Int8 msg;
	msg.data = AsphaltRoughness_slider_value;
	AsphaltRoughness_pub.publish(msg);
}

int floatingTextPhi = 180;
void viewpanel::setFloatingTextPhi( int floatingTextPhi_slider_value )
{
	floatingTextPhi = floatingTextPhi_slider_value;
	ros::NodeHandlePtr node_ptr = boost::make_shared<ros::NodeHandle>();
	FloatingTextPhi_pub = node_ptr->advertise<std_msgs::Float32>("/arbe/settings/floating_text_phi",1,true);
	std_msgs::Float32 msg;
	msg.data = floatingTextPhi * M_PI / 180;
	FloatingTextPhi_pub.publish(msg);
}


void viewpanel::setDynamicAzimuth( int DynamicAzimuth_slider_value )
{
	DynamicAzimuth = DynamicAzimuth_slider_value;
	dynamic_azimuth_label->setText( "Dynamic Azimuth ("+QString::number( DynamicAzimuth )+")" );
	radar_set_params();
	qApp->processEvents();
}

void viewpanel::setDynamicElevation( int DynamicElevation_slider_value )
{
	DynamicElevation = DynamicElevation_slider_value;
	dynamic_elevation_label->setText( "Dynamic Elevation ("+QString::number( DynamicElevation )+")" );
	radar_set_params();
	qApp->processEvents();
}

void viewpanel::setColoring( void )
{
	QString str = ColoringCombo->currentText();
	ColoringType.assign(str.toStdString());
	if ( strcmp(ColoringType.c_str(), "Range/Doppler") == 0 )
	{
		azimuth_bin_label->setEnabled(true);
		azimuth_bin_slider->setEnabled(true);
		manager_->getViewManager()->getCurrent()->subProp("Distance")->setValue("150");
	} else {
		azimuth_bin_label->setEnabled(false);
		azimuth_bin_slider->setEnabled(false);
	}
	dock->show();
	float cc_min, cc_max;
	Color_Coding_Min_Max::Instance()->get_converted_values(ColoringType, cc_min, cc_max);
	setMinColorCoding((int)cc_min);
	setMaxColorCoding((int)cc_max);
	cc_slider->setMinimumValue((int)cc_min);
	cc_slider->setMaximumValue((int)cc_max);

	cc_label->setText("Color coding ["+QString::fromStdString(Color_Coding_Min_Max::Instance()->get_units(ColoringType))+"]");

	pubGUIcontrols();
}

void viewpanel::setRange( void )
{
	QString str = RangeCombo->currentText();
	RangeType.assign(str.toStdString());
	radar_change_seq();

	if(fabs(manager_->getViewManager()->getCurrent()->subProp("Pitch")->getValue().toFloat() - 1.5398) < 0.0001)
		setTopView();
	qApp->processEvents();
}

void viewpanel::setMode( void )
{
	QString str = ModeCombo->currentText();
	mode.assign(str.toStdString());
        radar_change_seq();
	qApp->processEvents();
}

void viewpanel::setPreRadarSliders( void )
{
	int active_radar = RadarIdCombo->currentIndex();
	radar_id = active_radar;
	std::cout << "ant_pitch is  " << Per_radar_settings::Instance()->ant_pitch.empty() << std::endl;
	float val = Per_radar_settings::Instance()->ant_pitch[active_radar];
	radar_pitch_angle_slider->setValue((int)(round(val * 180.0 / M_PI * 10)));
	val = Per_radar_settings::Instance()->ant_height[active_radar];
	radar_z_offset_slider->setValue(val*100);
	val = Per_radar_settings::Instance()->cam_euler_a[active_radar];
	cam_rel_alpha->setValue(val*10);
	val = Per_radar_settings::Instance()->cam_euler_b[active_radar];
	cam_rel_beta->setValue(val*10);
	val = Per_radar_settings::Instance()->cam_euler_g[active_radar];
	cam_rel_gamma->setValue(val*10);
	val = Per_radar_settings::Instance()->threshold_4d[active_radar];
	sensitivity_slider->setValue(val);

	if(Per_radar_settings::Instance()->is_camera_calibrated[active_radar])
		cam_calib_btn->setStyleSheet("color: green");
	else
		cam_calib_btn->setStyleSheet("color: black");

	Per_radar_settings::Instance()->get_camera_intrinsic(active_radar,intrinsic_mat);

	radar_text_radio->setChecked(Per_radar_settings::Instance()->radar_for_text == RadarIdCombo->currentIndex());

}


void viewpanel::show_configuration_dock( void )
{
	dock->show();
}

void viewpanel::screen_record( void )
{
	int ret;

	if ( screen_recording == 0)
	{
		ROS_DEBUG("start screen recording.");
		ret = system("roslaunch arbe_phoenix_radar_driver screen_record.launch&");
		screen_record_button->setStyleSheet("color: red");
		screen_record_button->setText("R&ecording Screen");
		screen_recording = 1;
	} else {
		ROS_DEBUG("stop screen recording.");
		ret = system("rosnode kill screen_grab screen_recorder");
		screen_record_button->setStyleSheet("color: black");
		screen_record_button->setText("Screen R&ecord");
		screen_recording = 0;
	}
}

void viewpanel::radar_connect_control( void )
{
	int ret;
	if (radar_connected == 0)
	{
		/* Make sure visualization engine nodes are killed before launching */
		ret = system("rosnode list | grep arbe_visualization_engine | xargs rosnode kill");

		radar_launch_pointcloud_nodes();
	        radar_change_seq();
		}
	else
	{
		/* Stop Tx before siconnecting */
                radar_stop_transmit();
		radar_start_stop_button->setStyleSheet("color: black");
		radar_start_stop_button->setText("&Start Tx");
		radar_playing = 0;

		/* Stop pointcloud nodes */
		ret = system("rosnode list | grep arbe_pointcloud_receiver | xargs rosnode kill");

		/* Inidcate radar(s) disconnected */
		radar_connect_button->setStyleSheet("color: black");
		radar_connect_button->setText("Radar &Connect");
		radar_connected = 0;
		connected_radars = 0;
	}
}

void viewpanel::recording_control( void )
{
	if (rosbag_recording == 0)
	{
		rosbag_start_recording();
		if (rosbag_recording == 1)
		{
		record_button->setStyleSheet("color: red");
		record_button->setText("Rec&ording");
	}
	}
	else
	{
		rosbag_stop_recording();
		record_button->setStyleSheet("color: black");
		record_button->setText("Rec&ord");
	}

//	pubGUIcontrols();

}

void viewpanel::bookmark_control( void )
{
	uint16_t frame_num = get_frame_num();

	ROS_DEBUG("Bookmarked frame %d",frame_num);
	arbe_msgs::arbeBookmarkMsg msg;
	msg.frame_num = frame_num;
	msg.egoVel = get_host_vel();
	bookmark_pub.publish(msg);
//    pubGUIcontrols();

}

void viewpanel::radar_start_stop_control( void )
{
	if (radar_connected == 1)
	{
		if ( radar_playing == 0 )	
		{
                        setMode();
			for (int i=0; i < num_of_radars; i++)
			{
				pointcloud_[i]->subProp("Decay Time")->setValue((float)DetectionMemoryTime / 1000);
				stationary_pointcloud_[i]->subProp("Decay Time")->setValue((float)DetectionMemoryTime / 1000);
			}
                        radar_start_transmit();
			radar_start_stop_button->setStyleSheet("color: green");
			radar_start_stop_button->setText("&Stop Tx");
			radar_playing = 1;
		}
		else
                {
			for (int i=0; i < num_of_radars; i++)
			{
				pointcloud_[i]->subProp("Decay Time")->setValue((float)0);
				stationary_pointcloud_[i]->subProp("Decay Time")->setValue((float)0);
			}
                        radar_stop_transmit();
			radar_start_stop_button->setStyleSheet("color: black");
			radar_start_stop_button->setText("&Start Tx");
			radar_playing = 0;
		}
	} else 
	if ( radar_playing == 1 )
	{
		radar_start_stop_button->setStyleSheet("color: black");
		radar_start_stop_button->setText("&Start Tx");
		radar_playing = 0;
	} else
	{
		QMessageBox msgBox;
		msgBox.setText("Need to connect to the radar first.");
		msgBox.exec();
	}
}

int Pasued = 0;
void viewpanel::radar_pause_control( void )
{
	if (Pasued == false)
	{
		radar_pause_button->setStyleSheet("color: red");
		radar_pause_button->setText("&>");
		selection_panel_->show();
	}
	else
	{
		radar_pause_button->setStyleSheet("color: black");
		radar_pause_button->setText("&>>");
		selection_panel_->hide();
	}
	Pasued = 1 - Pasued;
}

bool viewpanel::get_3rd_prs( void )
{
    return follower_view_;
}

void viewpanel::printView(  )
{
	ROS_INFO("Like this view? [D,Y,P,F] = [%f,%f,%f,%s]",
			manager_->getViewManager()->getCurrent()->subProp("Distance")->getValue().toFloat(),
			manager_->getViewManager()->getCurrent()->subProp("Yaw")->getValue().toFloat(),
			manager_->getViewManager()->getCurrent()->subProp("Pitch")->getValue().toFloat(),
			manager_->getViewManager()->getCurrent()->subProp("Focal Point")->getValue().toString().toStdString().c_str());
}

void viewpanel::setView( view_vals_t &view_vals )
{
	manager_->getViewManager()->getCurrent()->subProp("Distance")->setValue(view_vals.distance);
	manager_->getViewManager()->getCurrent()->subProp("Yaw")->setValue(view_vals.yaw);
	manager_->getViewManager()->getCurrent()->subProp("Pitch")->setValue(view_vals.pitch);
	manager_->getViewManager()->getCurrent()->subProp("Focal Point")->setValue(view_vals.focal_point);
	marker_text_size = 1;
#if ONlY_SHOW_UI
	update_basic_markers();
#endif
}

void viewpanel::setTopView( void )
{
	float yaw = 3*M_PI_2;
	manager_->getViewManager()->getCurrent()->subProp("Yaw")->setValue(yaw);
	if(get_3rd_prs())
		return;
	if(get_localization_enable() && !get_3rd_prs())
	{
		float x, y;
		get_local_cart(x, y);
		calc_display_offsets(0,-2.5,0);
		setFocalPointAndHeading(x,y,0);
		return;
	}
	if(	strcmp(RangeType.c_str(), "Mid")==0)
	{
		manager_->getViewManager()->getCurrent()->subProp("Distance")->setValue("112");
		manager_->getViewManager()->getCurrent()->subProp("Pitch")->setValue("1.5398");
		manager_->getViewManager()->getCurrent()->subProp("Focal Point")->setValue("0;10;0");
        	display_offset_y_= 10;

		thickness_slider->setValue( 40 );
		cell_size_slider->setValue( 10 );
		marker_text_size = 3;
	}
	else if(strcmp(RangeType.c_str(), "Long")==0)
	{
		manager_->getViewManager()->getCurrent()->subProp("Distance")->setValue("265.499847");
		manager_->getViewManager()->getCurrent()->subProp("Pitch")->setValue("1.5398");
		manager_->getViewManager()->getCurrent()->subProp("Focal Point")->setValue("0;40;0");
        	display_offset_y_= 40;

		thickness_slider->setValue( 80 );
		cell_size_slider->setValue( 50 );
		marker_text_size = 5;
	}
	else if(strcmp(RangeType.c_str(), "Ultra-Long")==0)
	{
		manager_->getViewManager()->getCurrent()->subProp("Distance")->setValue("280.499847");
		manager_->getViewManager()->getCurrent()->subProp("Pitch")->setValue("1.5398");
		manager_->getViewManager()->getCurrent()->subProp("Focal Point")->setValue("0;45;0");
	        display_offset_y_= 45;

		thickness_slider->setValue( 80 );
		cell_size_slider->setValue( 50 );
		marker_text_size = 5.5;
	}
	else // "Short"
	{
		manager_->getViewManager()->getCurrent()->subProp("Distance")->setValue("80.665314");
		manager_->getViewManager()->getCurrent()->subProp("Pitch")->setValue("1.5398");
		manager_->getViewManager()->getCurrent()->subProp("Focal Point")->setValue("0;5;0");
		display_offset_y_= 5;

		thickness_slider->setValue( 40 );
		cell_size_slider->setValue( 10 );
		marker_text_size = 2;
	}
	update_basic_markers();

        pubGUIcontrols();
}

void viewpanel::setStandoffView( void )
{
	view_vals_t view_vals;
	view_vals.distance = 64.338554;
	view_vals.yaw = 4.094998;
	view_vals.pitch = 0.155;
	view_vals.focal_point="-15.981; 32.154; 3.29";

	setView(view_vals);
}

void viewpanel::set120mView( void )
{
	view_vals_t view_vals;
	view_vals.distance = 156.926270;
	view_vals.yaw = 4.713220;
	view_vals.pitch = 1.539800;
	view_vals.focal_point="-0.93111; 27.512; 0";

	setView(view_vals);
}


void viewpanel::setDiagView( void )
{
	view_vals_t view_vals;
	view_vals.distance = 151.205597;
	view_vals.yaw = 3.713210;
	view_vals.pitch = 0.939801;
	view_vals.focal_point="-8.8872; 53.601;  0";

	setView(view_vals);
}

void viewpanel::setBoutique2View( void )
{
	view_vals_t view_vals;
	view_vals.distance = 321;
	view_vals.yaw = 4.7;
	view_vals.pitch = 1.568;
	view_vals.focal_point="-88.9; -101;  0";

	setView(view_vals);
}

void viewpanel::setBoutique3View( void )
{
	view_vals_t view_vals;
	view_vals.distance = 478;
	view_vals.yaw = 4.7;
	view_vals.pitch = 1.568;
	view_vals.focal_point="-72.5; -212;  0";

	setView(view_vals);
}

void viewpanel::setBoutique1View( void )
{
	view_vals_t view_vals;
	view_vals.distance = 257;
	view_vals.yaw = 4.7;
	view_vals.pitch = 1.57;
	view_vals.focal_point="-161; 3.13;  0";

	setView(view_vals);
}

static bool inc_yaw_view = false;
static bool dec_yaw_view = false;
static float inc_yaw_val = 5 * M_PI / 180 / 20;

void viewpanel::rotate_view(void)
{
	if(inc_yaw_view || dec_yaw_view)
	{
		float yaw = manager_->getViewManager()->getCurrent()->subProp("Yaw")->getValue().toFloat();
		if (inc_yaw_view)
			yaw += inc_yaw_val;
		else
			yaw -= inc_yaw_val;
		manager_->getViewManager()->getCurrent()->subProp("Yaw")->setValue(yaw);
	}
}
void viewpanel::setFocalPointAndHeading( float x, float y, float h )
{
	if(recalc_display_offsets)
		calc_display_offsets(x,y,h);
	char fp[100];
	sprintf(fp,"%.5f;%.5f;0",x+display_offset_x_,y+display_offset_y_);
	rotate_view();

	manager_->getViewManager()->getCurrent()->subProp("Focal Point")->setValue(fp);
}

void viewpanel::setTopCarView( void )
{
	if(follower_view_)
	{
		inc_yaw_view = !inc_yaw_view;
		dec_yaw_view = false;
		if(inc_yaw_view)
			top_car_view_button->setText("Stop");
		else
			top_car_view_button->setText("Rot.Lt");
		car_view_button->setText("Rot.Rt");
		return;
	}
	else
	{
		view_vals_t view_vals;
		view_vals.distance = 40;
		view_vals.pitch = 0.25;
		view_vals.yaw = 4.73;
		view_vals.focal_point="0;0;0";
		setView(view_vals);
	}
}

void viewpanel::setCarView( void )
{
	if(follower_view_)
	{
		dec_yaw_view = !dec_yaw_view;
		inc_yaw_view = false;
		if(dec_yaw_view)
			car_view_button->setText("Stop");
		else
			car_view_button->setText("Rot.Rt");
		top_car_view_button->setText("Rot.Lt");
		return;
	}
	else
	{
		view_vals_t view_vals;
		view_vals.distance = 40;
		view_vals.yaw = 4.71322;
		view_vals.pitch = -0.154797;
		view_vals.focal_point="0;0;0";

		setView(view_vals);
	}
}

void viewpanel::setSideView( void )
{
	view_vals_t view_vals;
	view_vals.distance = 60;
	view_vals.yaw = 3.65;
	view_vals.pitch = -0.13;
	view_vals.focal_point="2.73;31.69;3.29";

	setView(view_vals);
}

bool viewpanel::getFollowerView( void )
{
    return follower_view_;
}

void viewpanel::toggleFollowerView( void )
{

    follower_view_ = !follower_view_;

    if (follower_view_)
    {
        side_view_button->setStyleSheet("color: red");
        side_view_button->setText("Dettach");

        top_view_button->setStyleSheet("color: red");
        top_view_button->setText("DynTop");

		top_car_view_button->setText("Rot.Lt");
		top_car_view_button->setStyleSheet("color: red");

		car_view_button->setText("Rot.Rt");
		car_view_button->setStyleSheet("color: red");

		if(get_localization_enable())
			recalc_display_offsets = true;
		else
		{
			calc_display_offsets(0,-2.5,0);
			recalc_display_offsets = false;
    }

	}
    else
    {
        side_view_button->setStyleSheet("color: black");
        side_view_button->setText("3rd prs.");

        top_view_button->setStyleSheet("color: black");
        top_view_button->setText("Top");

		top_car_view_button->setText("Top Car");
		top_car_view_button->setStyleSheet("color: black");

		car_view_button->setText("Car");
		car_view_button->setStyleSheet("color: black");

    }
}

void viewpanel::enableFollowerView( void )
{
	if (!follower_view_)
		toggleFollowerView();
	calc_display_offsets(0,-2.5,0);
	recalc_display_offsets = false;
}

void viewpanel::choose_asphalt_or_text(bool asphalt)
{
	asphalt_roughness_label->setVisible(asphalt);
	asphalt_roughness_slider->setVisible(asphalt);
	asphalt_roughness_slider->setEnabled(asphalt);
	vis_text_phi_label->setVisible(!asphalt);
	vis_text_phi_slider->setVisible(!asphalt);
	vis_text_phi_slider->setEnabled(!asphalt);
}

void viewpanel::calc_display_offsets(float x , float y, float h)
{
    float yaw = manager_->getViewManager()->getCurrent()->subProp("Yaw")->getValue().toFloat();

    std::string fp_str = manager_->getViewManager()->getCurrent()->subProp("Focal Point")->getValue().toString().toStdString();
    std::vector<float> fp;
    float val;

    std::stringstream ss(fp_str);
    while(ss >> val)
    {
        fp.push_back(val);
        if (ss.peek() == ';' or ss.peek() == ' ')
            ss.ignore();
    }


        display_offset_h_ = yaw;
	display_offset_x_ = fp[0] - x;
	display_offset_y_ = fp[1] - y;
	recalc_display_offsets = false;
    }

void viewpanel::closeEvent(QCloseEvent *event)
{
	//print_cam_widg_location();
	QMessageBox::StandardButton answer = QMessageBox::question(
		this,
		tr("Close the Window"),
		tr("Do you want to close the window?"),
		QMessageBox::Yes | QMessageBox::No);

	if(answer == QMessageBox::Yes){
		event->accept();
		close();
		radar_quit();
	}
	else
		event->ignore();
}

// **************************
/*
void on_disp_persp_callback(std_msgs::UInt16 msg);
void on_disp_colorc_callback(std_msgs::UInt16 msg);
void init_disp_sub(ros::NodeHandlePtr node_ptr)
{
	disp_perspective_sub = node_ptr->subscribe("/arbe/settings/perspective_t_tc", 1, on_disp_persp_callback);
	disp_colorcode_sub = node_ptr->subscribe("/arbe/settings/colorcode_d_dg_a_af_e", 1, on_disp_colorc_callback);
}

void on_disp_persp_callback(std_msgs::UInt16 msg)
{
	if(msg == 0) // top-mid
		setTopViewMid();
	else if (msg == 1)
		setTopCarView();
}

void on_disp_colorc_callback(std_msgs::UInt16 msg)
{

}
*/

void viewpanel::on_cmnd_line_view_callback(std_msgs::UInt8 msg)
{
	uint8_t view = msg.data;
	switch (view) {
	case VT_TOP:
		setTopView();
		break;
	case VT_TOP_CAR:
		setTopCarView();
		break;
	case VT_CAR:
		setCarView();
		break;
	case VT_SIDE:
		setSideView();
		break;
	case VT_STANDOFF:
		setStandoffView();
		break;
	case VT_120M:
		set120mView();
		break;
	case VT_DIAG:
		setDiagView();
		break;
	case VT_BOUTIQUE_1:
		setBoutique1View();
		break;
	case VT_BOUTIQUE_2:
		setBoutique2View();
		break;
	case VT_BOUTIQUE_3:
		setBoutique3View();
		break;
	default:
		break;
	}
}

void viewpanel::on_cmnd_line_colorcoding_callback(std_msgs::UInt8 msg)
{
	ColoringCombo->setCurrentIndex(msg.data);
}

void viewpanel::on_cmnd_line_screen_record(std_msgs::Bool msg)
{
	screen_record();
}

void viewpanel::on_cmnd_line_record(std_msgs::Bool msg)
{
    recording_control();
}

void viewpanel::on_cmnd_line_euler_alpha(std_msgs::Float32 msg)
{
    setCamEulerAlpha( (int)(msg.data*10));
}

void viewpanel::on_cmnd_line_euler_beta(std_msgs::Float32 msg)
{
    setCamEulerBeta( (int)(msg.data*10));
}
void viewpanel::on_cmnd_line_euler_gamma(std_msgs::Float32 msg)
{
    setCamEulerGamma( (int)(msg.data*10));
}
void viewpanel::on_omega_calibration(std_msgs::Float32 msg)
{
	if (msg.data < -1000)
		mTurnRateNeedle->setColor(Qt::red);
	else
    mTurnRateNeedle->setColor(Qt::white);
}

void road_inclination_callback(const arbe_msgs::arbeRdInclinationConstPtr & msg);

void viewpanel::on_status_mesage(std_msgs::String msg)
{
	connected_radars++;
	ROS_INFO("Status message from a radar: %s connected radars=%d", msg.data.c_str(),connected_radars);

	if (connected_radars == num_of_radars)
	{
		radar_connect_button->setStyleSheet("color: green");
	}
	else
	{
		radar_connect_button->setStyleSheet("color: Orange");
	}

	radar_connect_button->setText("Radar Dis&Connect");

	radar_set_params();
	sleep(1);
	Ntc_Mode_Set();
	Cfar_Mode_Set();

	radar_change_seq();

	radar_connected = 1;
}

static arbe_msgs::arbeNewPcMsg::ConstPtr PcFrame;

void viewpanel::register_pointcloud_displays(int radar_id)
{

	if ( ( pointcloud_topic_initialized[radar_id] != true ))
	{
		std::string pointcloud_topic = "/arbe/rviz/pointcloud_"+std::to_string(radar_id);
		std::string stationary_pointcloud_topic = "/arbe/rviz/stationary_pointcloud_"+std::to_string(radar_id);
		ROS_INFO("Registering new pointcloud topic: %s",pointcloud_topic.c_str());	
		ROS_INFO("Registering new pointcloud topic: %s",stationary_pointcloud_topic.c_str());

		pointcloud_[radar_id] = manager_->createDisplay( "rviz/PointCloud2", "PointCloud2", true );
		pointcloud_[radar_id]->subProp("Topic")->setValue( pointcloud_topic.c_str() );
		pointcloud_[radar_id]->subProp("Style")->setValue("Spheres");
		pointcloud_[radar_id]->subProp("Size (Pixels)")->setValue("3");
		pointcloud_[radar_id]->subProp("Size (m)")->setValue("0.3");
		pointcloud_[radar_id]->subProp("Decay Time")->setValue((float)DetectionMemoryTime / 1000);
		pointcloud_[radar_id]->subProp("Color Transformer")->setValue("RGB8");
		pointcloud_[radar_id]->subProp("Invert Rainbow")->setValue("false");
		pointcloud_[radar_id]->subProp("Position Transformer")->setValue("XYZ");
		pointcloud_[radar_id]->subProp("Use Fixed Frame")->setValue("true");
		pointcloud_[radar_id]->subProp( "Axis" )->setValue( "Z" );


		stationary_pointcloud_[radar_id] = manager_->createDisplay( "rviz/PointCloud2", "PointCloud2", true );
		stationary_pointcloud_[radar_id]->subProp("Topic")->setValue( stationary_pointcloud_topic.c_str() );
		stationary_pointcloud_[radar_id]->subProp("Style")->setValue("Spheres");
		stationary_pointcloud_[radar_id]->subProp("Size (Pixels)")->setValue("3");
		stationary_pointcloud_[radar_id]->subProp("Size (m)")->setValue("0.3");
	//	stationary_pointcloud_[radar_id]->subProp("Decay Time")->setValue((float)15500 / 1000);
		stationary_pointcloud_[radar_id]->subProp("Color Transformer")->setValue("RGB8");
		stationary_pointcloud_[radar_id]->subProp("Invert Rainbow")->setValue("false");
		stationary_pointcloud_[radar_id]->subProp("Position Transformer")->setValue("XYZ");
		stationary_pointcloud_[radar_id]->subProp("Use Fixed Frame")->setValue("true");
		stationary_pointcloud_[radar_id]->subProp( "Axis" )->setValue( "Z" );
		pointcloud_topic_initialized[radar_id] = true;
//		num_of_radars++;
	}	
}

void viewpanel::register_slam_displays(int radar_id)
{
	std::string topic = "/arbe/rviz/objects_"+std::to_string(radar_id);
	SlamArray_[radar_id] = manager_->createDisplay( "rviz/MarkerArray", "MarkerArray", true );
	SlamArray_[radar_id]->subProp("Marker Topic")->setValue(topic.c_str());
	SlamArray_[radar_id]->subProp("Decay Time")->setValue((float)DetectionMemoryTime / 1000);
}

void viewpanel::register_FS_displays(int radar_id)
{
	std::string topic = "/arbe/rviz/fs_poly_"+std::to_string(radar_id);

	fs_polygon_display_[radar_id] = manager_->createDisplay( "rviz/Polygon", "Polygon", true );
	fs_polygon_display_[radar_id]->subProp("Topic")->setValue(topic.c_str());
	fs_polygon_display_[radar_id]->subProp("Alpha")->setValue("0.1");
	fs_polygon_display_[radar_id]->subProp("Color")->setValue("25, 255, 0");
}


void on_ant_height_tilt_callback(const arbe_msgs::arbeRdInclination::ConstPtr &msg, uint32_t radar_id)
{

    int height_cm = (int)(msg->ant_height * 100);
	int tilt_deg_times_ten = (int)(round(msg->ant_tilt*10));

    if(radarIdCombo_ready == true && (radar_id == RadarIdCombo->currentIndex() ) )
    {
        radar_z_offset_slider->setValue(height_cm);
		radar_pitch_angle_slider->setValue(tilt_deg_times_ten);
		ROS_DEBUG("setting radar = %d. height = %d cm Tilt = %f degs", radar_id, height_cm, tilt_deg_times_ten/10.0);
    }
    else
    {
        // update only the params without the slider
		Per_radar_settings::Instance()->ant_pitch.at(radar_id) = tilt_deg_times_ten * M_PI / 180.0/10.0;
        Per_radar_settings::Instance()->ant_height.at(radar_id) = msg->ant_height;
        pub_radar_height_and_pitch(radar_id);
        pubGUIcontrols();
    }

}

void viewpanel::register_settings(int radar_id)
{
    ros::NodeHandlePtr node_ptr = boost::make_shared<ros::NodeHandle>("~");
    std::string topic = "/arbe/processed/ant_height_tilt/"+std::to_string(radar_id);
    arbe_ant_height_tilt_sub_arr[radar_id] = node_ptr->subscribe<arbe_msgs::arbeRdInclination>(topic,1,boost::bind(on_ant_height_tilt_callback, _1, radar_id));
}

void viewpanel::on_radar_calibration_callback(const arbe_msgs::arbeGUIsettings::ConstPtr& controls_data)
{
	if (controls_data->radar_id == 0)
	{
		radar_id = controls_data->radar_id;
		ROS_INFO("Set Radar ID to %d\n",controls_data->radar_id);

		ROS_INFO("Set Radar X offset to %lf\n",controls_data->radar_x_offset);
		radar_x_offset = controls_data->radar_x_offset;

//		ROS_INFO("Set Radar Y offset to %lf\n",controls_data->radar_y_offset);
//		radar_y_offset = controls_data->radar_y_offset;

		ROS_INFO("Set Radar Z offset to %lf\n",controls_data->radar_z_offset);
		radar_z_offset = controls_data->radar_z_offset;

		ROS_INFO("Set Radar Yaw Angle to %lf\n",controls_data->radar_yaw_angle * 180 / M_PI);
		radar_yaw_angle = controls_data->radar_yaw_angle;

		ROS_INFO("Set Radar Pitch Angle to %lf\n",controls_data->radar_pitch_angle * 180 / M_PI);
		radar_pitch_angle = controls_data->radar_pitch_angle;

		radar_x_offset_slider->setValue( radar_x_offset * 100 );
//		radar_y_offset_slider->setValue( radar_y_offset * 100 );
		radar_z_offset_slider->setValue( radar_z_offset * 100 );
		radar_yaw_angle_slider->setValue((int)(radar_yaw_angle * 180 / M_PI));
		radar_pitch_angle_slider->setValue((int)(radar_pitch_angle * 180 / M_PI*10));

		//pubGUIcontrols();
	}
	return;
}



void viewpanel::init_subs( void )
{
	ros::NodeHandlePtr node_ptr = boost::make_shared<ros::NodeHandle>("~");
	cmnd_line_view_sub = node_ptr->subscribe("/arbe/settings/cmnd_line_view",1,&viewpanel::on_cmnd_line_view_callback,this);
	cmnd_line_color_code_sub = node_ptr->subscribe("/arbe/settings/cmnd_line_color",1,&viewpanel::on_cmnd_line_colorcoding_callback,this);
	cmnd_line_screen_record_sub = node_ptr->subscribe("/arbe/settings/cmnd_line_screen_record",1,&viewpanel::on_cmnd_line_screen_record,this);
    cmnd_line_record_sub = node_ptr->subscribe("/arbe/settings/cmnd_line_record",1,&viewpanel::on_cmnd_line_record,this);
    cmnd_line_euler_alpha_sub = node_ptr->subscribe("/arbe/settings/cmnd_line_euler_alpha",1,&viewpanel::on_cmnd_line_euler_alpha,this);
    cmnd_line_euler_beta_sub = node_ptr->subscribe("/arbe/settings/cmnd_line_euler_beta",1,&viewpanel::on_cmnd_line_euler_beta,this);
    cmnd_line_euler_gamma_sub = node_ptr->subscribe("/arbe/settings/cmnd_line_euler_gamma",1,&viewpanel::on_cmnd_line_euler_gamma,this);
	isOmegaCalibrated_sub = node_ptr->subscribe("/arbe/settings/omega_calibration_value/0",1,&viewpanel::on_omega_calibration,this);
    rd_inclination_sub = node_ptr->subscribe("/arbe/processed/road_inclination",1,road_inclination_callback);
	arbe_status_sub = node_ptr->subscribe("/arbe/raw/status",10,&viewpanel::on_status_mesage,this);

//	int n_radars = 1;
	node_ptr->getParam("n_radars",num_of_radars);
	node_ptr->getParam("camera_id", camera_id);
    for(int nr = 0; nr < num_of_radars; nr++)
	{
		register_pointcloud_displays(nr);
		register_slam_displays(nr);
		register_FS_displays(nr);
        register_settings(nr);
}
	arbe_radar_calibration_sub = node_ptr->subscribe ("/arbe/settings/radar_calibration",1,&viewpanel::on_radar_calibration_callback,this);
    initCalcHeightTiltPub(node_ptr);

}

void init_pubs( void )
{
	ros::NodeHandlePtr node_ptr = boost::make_shared<ros::NodeHandle>();
	arbe_gui_commands_pub = node_ptr->advertise<std_msgs::String>("arbe/settings/gui_commands", 1);
}

void on_bookmark_receive_callback(arbe_msgs::arbeBookmarkMsg msg);
void init_speed_pub(ros::NodeHandlePtr node_ptr)
{
	egoVel_pub = node_ptr->advertise<std_msgs::Float64>("/arbe_speed",1); //TODO: REMOVE?
	turnRate_pub = node_ptr->advertise<std_msgs::Float64>("/arbe_turnrate",1); //TODO: REMOVE?
}
void init_bookmark_sub(ros::NodeHandlePtr node_ptr)
{
	bookmark_sub = node_ptr->subscribe("/arbe/settings/bookmarked_frames", 1, on_bookmark_receive_callback);
}
void init_bookmark_pub(ros::NodeHandlePtr node_ptr)
{
	bookmark_pub = node_ptr->advertise<arbe_msgs::arbeBookmarkMsg>("/arbe/settings/bookmarked_frames",1);
}
void init_bookmark_SubPub(ros::NodeHandlePtr node_ptr)
{
	init_bookmark_sub(node_ptr);
	init_bookmark_pub(node_ptr);
}

void on_bookmark_receive_callback(arbe_msgs::arbeBookmarkMsg msg)
{
	uint16_t bookmarked_frame_num = msg.frame_num;
	float ego_vel = msg.egoVel;
	ROS_DEBUG("Reached a bookmarked frame (%d)",bookmarked_frame_num);
}

void publish_image_for_training(const std_msgs::UInt16::ConstPtr& msg);
void SI_slam_on_cam(const sensor_msgs::CompressedImage::ConstPtr& image);

void init_capture_Sub(ros::NodeHandlePtr node_ptr)
{
	capture_for_training_sub = node_ptr->subscribe("/arbe/dl_training/pub_image_now", 1, publish_image_for_training);
}
void init_processed_image_Pub(ros::NodeHandlePtr node_ptr)
{
    arbe_capture_pub = node_ptr->advertise<sensor_msgs::CompressedImage>("/cv_camera/processed_image/compressed",1);
	img_for_dltrain_pub = node_ptr->advertise<sensor_msgs::CompressedImage>("/arbe/dl_training/thumbnail/periodic/compressed",1, true);
}

static bool FS_in_use = false;
void FS_disp_CB(const geometry_msgs::PolygonStamped::ConstPtr& FS_disp)
{
	if(!FS_in_use)
		FS_display_polygon = geometry_msgs::PolygonStamped(*FS_disp);
}
void initFSdisplaySub(ros::NodeHandlePtr node_ptr)
{
	fs_display_sub= node_ptr->subscribe("/arbe/processed/free_space/display_polygon", 1, FS_disp_CB);
}
void initImageSubPub(ros::NodeHandlePtr node_ptr)
{
    init_capture_Sub(node_ptr);
    init_processed_image_Pub(node_ptr);
    initFSdisplaySub(node_ptr);
}

void initCalcHeightTiltPub(ros::NodeHandlePtr node_ptr)
{
    for (uint32_t radar_ind = 0; radar_ind < num_of_radars; radar_ind++)
    {
        pub_calc_ant_height_tilt[radar_ind] = node_ptr->advertise<std_msgs::Bool>("/arbe/settings/do_calc_ant_height_tilt/"+std::to_string(radar_ind),1,true);
    }
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

// default camera calibration (Logitech L920)

/*static float prj[3][4] = {{1388,    67,    881,    0},
                   {24.5,    1215,    468.5,    0},
                   {0.0522,    0.0697,    0.99,    0}};
{{1134.1,    83.2,    951.1,    19.1},
                   {0,    1182.8,    451.1,    102.2},
                   {0,    0.1,    1.0,    0.0}};*/
static int n_rows = 1080;
static float R_T[3][4] = {{0,0,0,0},{0,0,0,0.08},{0,0,0,0.02}};
static float R_T_FS[3][4] = {{0,0,0,0},{0,0,0,0.08},{0,0,0,0.02}};
static float prj[3][4] = {{1526.97,0,934.05,18.68},
                          {0,1533.03,537.37,133.39},
                          {0,0,1,0.02}};
static float prj_FS[3][4] = {{1526.97,0,934.05,18.68},
						  {0,1533.03,537.37,133.39},
						  {0,0,1,0.02}};
/*{{1526.97,    84.64,   930.20,  18.68},
{0,1575.42,    396.24,    133.39},
{0.0,    0.0906,    0.9959,    0.02}};*/

void rotation_into_R_t(float alpha, float beta, float gamma, float fs_alpha_backoff)
{
    float rotx_alpha[3][3] = {{1,0,0},{0,1,0},{0,0,1}};
    float roty_beta[3][3] = {{1,0,0},{0,1,0},{0,0,1}};
    float rotx_gamma[3][3] = {{1,0,0},{0,1,0},{0,0,1}};
	//todo: split
	float rotx_fs_alpha[3][3] = {{1,0,0},{0,1,0},{0,0,1}};
	float fs_alpha = alpha+fs_alpha_backoff;
	rotx_fs_alpha[1][1] = cos(fs_alpha * M_PI / 180);
	rotx_fs_alpha[2][2] = rotx_fs_alpha[1][1];
	rotx_fs_alpha[2][1] = sin(fs_alpha * M_PI / 180);
	rotx_fs_alpha[1][2] = -rotx_fs_alpha[2][1];

	rotx_alpha[1][1] = cos(alpha * M_PI / 180);
    rotx_alpha[2][2] = rotx_alpha[1][1];
	rotx_alpha[2][1] = sin(alpha * M_PI / 180);
    rotx_alpha[1][2] = -rotx_alpha[2][1];

	roty_beta[0][0] = cos(beta * M_PI / 180);
    roty_beta[2][2] = roty_beta[0][0];
	roty_beta[0][2] = sin(beta * M_PI / 180);
    roty_beta[2][0] = -roty_beta[0][2];

	rotx_gamma[1][1] = cos(gamma * M_PI / 180);
    rotx_gamma[2][2] = rotx_gamma[1][1];
	rotx_gamma[2][1] = sin(gamma * M_PI / 180);
    rotx_gamma[1][2] = -rotx_gamma[2][1];

    for(uint8_t i = 0; i < 3; i++)
		for(uint8_t j = 0; j < 3; j++){
            R_T[i][j] = 0;
			R_T_FS[i][j] = 0;
		}

    for(uint8_t i = 0; i < 3; i++)
        for(uint8_t j = 0; j < 3; j++){
            for(uint8_t k = 0; k < 3; k++){
                for(uint8_t l = 0; l < 3; l++){
                    R_T[i][j] += rotx_gamma[i][k]*roty_beta[k][l]*rotx_alpha[l][j];
					R_T_FS[i][j] += rotx_gamma[i][k]*roty_beta[k][l]*rotx_fs_alpha[l][j];
                }
            }
        }

}
static ros::Publisher cam_projection_pub;
void calc_cam_projection(bool do_for_all_radars)
{
    std_msgs::Float32MultiArray msg;
    std::vector<float> data;

    memset(prj,0x0,12*sizeof(float));
    for(uint8_t i = 0; i < 3; i++)
        for(uint8_t j = 0; j < 4; j++){
            for(uint8_t k = 0; k < 3; k++){
                prj[i][j] += intrinsic_mat[i][k] * R_T[k][j];
				prj_FS[i][j] += intrinsic_mat[i][k] * R_T_FS[k][j];
            }
            data.push_back(prj[i][j]);
        }
    msg.data = data;
    ros::NodeHandlePtr node_ptr = boost::make_shared<ros::NodeHandle>();
	int index = -1;
	if(radarIdCombo_ready && Per_radar_settings::Instance()->ant_pitch.size()>0)
	{
		index = RadarIdCombo->currentIndex();
		int index0 = do_for_all_radars ? 0 : index;
		int index1 = do_for_all_radars ? Per_radar_settings::Instance()->ant_pitch.size() : index + 1;
		for (int ind = index0; ind < index1; ind++)
		{
			cam_projection_pub = node_ptr->advertise<std_msgs::Float32MultiArray>("/arbe/settings/camera_projection/"+std::to_string(ind),1,true);
    cam_projection_pub.publish(msg);
}
	}
}

void rotation_into_R_t_and_calc_cam_projection(float alpha, float beta, float gamma)
{
	rotation_into_R_t(alpha, beta, gamma, cam_euler_fs_alpha_backoff);
	calc_cam_projection(false);
}

int read_camera_calibration_from_file(const char * file_name, bool do_for_all_radars)
{
    // open file, return if fails
    static FILE * fp = NULL;
    fp = fopen(file_name,"r");
    if(fp == NULL)
        return -1;

    // read header line
    char junk[300];
    char * header = junk;
    size_t len = 0;
    ssize_t read;
    read = getline(&header, &len, fp);
    if(read == -1){
        fclose(fp);
        return -1;
    }

    // read intrinsic matrix
    for(uint8_t line = 0; line < 3; line++)
    {
        if(fscanf(fp,"%f,%f,%f\n",
                  &intrinsic_mat[line][0],&intrinsic_mat[line][1],&intrinsic_mat[line][2]) == EOF)
        {
            fclose(fp);
            return -1;
        }
    }

    // read translation header
    read = getline(&header, &len, fp);
    if(read == -1){
        fclose(fp);
        return -1;
    }

    // read translation
    if(fscanf(fp,"%f,%f,%f\n",&R_T[0][3],&R_T[1][3],&R_T[2][3]) == EOF)
    {
        fclose(fp);
        return -1;
    }

	if(radarIdCombo_ready && Per_radar_settings::Instance()->ant_pitch.size()>0)
	{
		int index = RadarIdCombo->currentIndex();
		int index0 = do_for_all_radars ? 0 : index;
		int index1 = do_for_all_radars ? Per_radar_settings::Instance()->ant_pitch.size() : index + 1;
		for (int ind = index0; ind < index1; ind++)
		{
			Per_radar_settings::Instance()->set_camera_intrinsic(ind,intrinsic_mat);
			Per_radar_settings::Instance()->is_camera_calibrated.at(ind) = true;
		}
		cam_calib_btn->setStyleSheet("color: green");
	}

	calc_cam_projection(do_for_all_radars);
    ROS_INFO("Camera calibration file was read successfully");
    return 0;
}

static cv::Point project(float X, float Y, float Z, float scale){
    float xzyw[4] = {X,-Z,Y,1};
    float out[3] = {0,0,0};

    for(uint8_t out_dim = 0; out_dim < 3; out_dim++){
        for(uint8_t inner_dim = 0; inner_dim < 4; inner_dim++){
            out[out_dim] += prj[out_dim][inner_dim] * xzyw[inner_dim] ;
        }
    }

    cv::Point pt = cv::Point(out[0]/out[2]*scale, out[1]/out[2]*scale);
    return pt;
}

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
    cv::Point pt;
    for (uint8_t i = 0; i < 8; i++){
        X = x + (primitive[pr_x[i]] * size_x * co + primitive[pr_y[i]] * size_y * so ) / 2;
        Y = depth + (primitive[pr_y[i]] * size_y * co - primitive[pr_x[i]] * size_x * so ) / 2;
        Z = (up + primitive[pr_z[i]] * size_z / 2);
        if(Y < 0)
            out = 8;
        pt = project(X, Y, Z, scale);
        if(pt.y > rows || pt.y < 0 || pt.x > cols || pt.x < 0)
            out++;
        points.push_back(pt);
        left = pt.x < left ? pt.x : left;
        top = pt.y < top ? pt.y : top;
    }

    tl.x = left ;
    tl.y = top - 20;

    if (out > 4)
        legit = false;
    else
        makeBoundingBox(points,polygons);
}

bool projectFS(geometry_msgs::PolygonStamped &fs_display, float scale,std::vector<cv::Point> &polygons, int rows, int cols)
{
//	legit=true;
	uint8_t out = 0;
	cv::Point pt;
//	for (uint8_t i = 0; i < 90; i++){//fs_display.polygon.points.size(); i++){
	for (uint16_t i = 0; i < fs_display.polygon.points.size(); i++){
		geometry_msgs::Point32 point = fs_display.polygon.points.at(i);

//		pt = project(point.x, point.y, point.z, scale);

		float xzyw[4] = {point.x,-point.z,point.y,1};
		float out[3] = {0,0,0};

		for(uint16_t out_dim = 0; out_dim < 3; out_dim++){
			for(uint16_t inner_dim = 0; inner_dim < 4; inner_dim++){
				out[out_dim] += prj/*_FS*/[out_dim][inner_dim] * xzyw[inner_dim] ;
			}
		}

		pt = cv::Point(out[0]/out[2]*scale, out[1]/out[2]*scale);
//		if(pt.y > rows || pt.y < 0 || pt.x > cols || pt.x < 0)
//			out++;
//		points.push_back(pt);
		polygons.push_back(pt);
	}

}


static cv_bridge::CvImagePtr cv_ptr_queue[30];
static ros::Time queue_stamps[30];
static double max_stamp = 0;
static uint16_t camera_read_ind = 0, camera_write_ind = 0;

bool read_queue_pp(ros::Time ref_time)
{
	if ((ref_time - queue_stamps[camera_read_ind]).toSec() > -0.01)
    {
		while((ref_time - queue_stamps[camera_read_ind]).toSec() > 0.05)
		{
        camera_read_ind++;
        camera_read_ind %= 30;
		}
		camera_read_ind++;
		camera_read_ind %= 30;
        return true;
    }
    return false;
}

void write_queue_pp(ros::Time ref_time, bool reset)
{
    camera_write_ind++;
	if (ref_time.toSec() < max_stamp || (ref_time.toSec() - max_stamp > 1) || reset)
    {
        camera_read_ind = camera_write_ind-1;
    }
	camera_write_ind %= 30;
    max_stamp = ref_time.toSec();
}

typedef struct
{
    ros::Time lastUpdateTime;
    float rd_ls_a = 0;
    float rd_ls_b = -1;
}sRdInclination;
static sRdInclination rd_inc;

void road_inclination_callback(const arbe_msgs::arbeRdInclinationConstPtr & msg)
{
    rd_inc.lastUpdateTime = msg->header.stamp;
    rd_inc.rd_ls_a = msg->ant_tilt;
    rd_inc.rd_ls_b = msg->ant_height;
}

static cv_bridge::CvImagePtr cv_ptr_training = NULL;
static bool time_to_capture = false;
static uint16_t corr_capture_num = 0;
void publish_image_for_training(const std_msgs::UInt16::ConstPtr& msg)
{
	time_to_capture = true;
	corr_capture_num = msg->data;
}

static sensor_msgs::CompressedImage thumb_msg;

void SI_slam_on_cam(const sensor_msgs::CompressedImage::ConstPtr& image)  //TODO: REMOVE
{
    if(get_slamOnCam())
    {
		ros::Time ref_time;
		if(get_slam_validity())
			ref_time.fromSec(get_slam_metadata().CorresPcTime/1000.0);
		else
			ref_time = image->header.stamp;//queue_stamps[camera_write_ind];
		bool is_read_pp;
		cv_bridge::CvImagePtr cv_ptr;
		if(!get_cameraBuffering() || (image->header.stamp - ref_time).toSec() < 0.05)
		{
			cv_ptr = cv_bridge::toCvCopy(image, sensor_msgs::image_encodings::BGR8);
			is_read_pp = true;
		}
		else
		{
			cv_ptr_queue[camera_write_ind] = cv_bridge::toCvCopy(image, sensor_msgs::image_encodings::BGR8);
			queue_stamps[camera_write_ind] = image->header.stamp;
			write_queue_pp(image->header.stamp, (image->header.stamp - ref_time).toSec() > 1);
			is_read_pp = read_queue_pp(ref_time);
			cv_ptr = cv_ptr_queue[camera_read_ind];
		}

		if(dl_training_view_active && !time_to_capture && !get_dl_training_playback())
			return;

		if(cv_ptr != NULL && time_to_capture)
		{
			thumb_msg.header.frame_id = "training_img";
			thumb_msg.header.stamp = ros::Time::now(); // time
			thumb_msg.header.seq = corr_capture_num; // user defined counter
			if(is_read_pp)
			cv_ptr->toCompressedImageMsg(thumb_msg);
			img_for_dltrain_pub.publish(thumb_msg);
			time_to_capture = false;
		}

        if(is_read_pp)
        {
        cv::Point pt2text;
        int cols = cv_ptr->image.cols;
        int rows = cv_ptr->image.rows;
            float scale = rows == 1200 ? 1.0 : rows / 1080.0;
        std::vector<std::vector<cv::Point> > polygons;
        bool draw;
        bool eco_mode = get_slamOnCam_ecoMode();
            bool color_by_class = get_colorObjByClass();
//		if(get_FS_OnCam())
		{
			FS_in_use = true;
			int n_pts = FS_display_polygon.polygon.points.size();
			if(n_pts>0 && get_FreeSpace_enabled())
			{
				cv::Mat layer = cv_ptr->image.clone();//= cv::Mat::zeros(cv_ptr->image.size(), CV_8UC3); //
				std::vector<cv::Point> fs_polygon;
				projectFS(FS_display_polygon, scale,fs_polygon, rows, cols);
//                    const cv::Point* elementPoints[1] = { &fs_polygon[0] };
//                    int numberOfPoints = (int)fs_polygon.size();
				//fillPoly (contourMask, elementPoints, &numberOfPoints, 1, Scalar (0, 0, 0), 8);

			//	cv::fillPoly(layer,elementPoints,&numberOfPoints,1,cv::Scalar(0,255,0), cv::FILLED );
//				cv::polylines(layer, fs_polygon, true, cv::Scalar(0,255,0),15);
				std::vector<std::vector<cv::Point> > fillContAll;
				  //fill the single contour
				  //(one could add multiple other similar contours to the vector)
				  fillContAll.push_back(fs_polygon);
				  cv::fillPoly( layer, fillContAll, cv::Scalar(90,255,50));
				  //cv::fillPoly( layer, fillContAll, cv::Scalar(255,215,5)); // B,G,R
				  cv::addWeighted(cv_ptr->image, 0.7, layer, 0.3, 0.0, cv_ptr->image);
			}
			FS_in_use = false;
		}
			if(get_slam_validity() && get_funnel_enabled())
			{
                uint8_t n_segments = 5;
				float vel = get_slam_metadata().HostVelocity, omega = get_slam_metadata().HostOmega;
				float DT = 1.0;
				if(vel*3.6>60)
				{
					DT -= vel*3.6 / 120;
					DT = DT < 0.3 ? 0.3 : DT;
				}
                float dt = DT / n_segments;
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
						cv::Point pt_r = project(cos(angle) * radius + circ_center + 1, y0, -1, scale);
						cv::Point pt_l = project(cos(angle) * radius + circ_center - 1, y0, -1, scale);
						for (uint8_t section = 0; section < n_segments; section++)
						{
							angle += delta_angle;
							double Y = sin(angle) * radius;
							double X = cos(angle) * radius + circ_center;
							double Z = rd_inc.rd_ls_a * Y + rd_inc.rd_ls_b;
							cv::Point pt2_r = project(X + 1, Y, Z, scale);
							cv::Point pt2_l = project(X - 1, Y, Z, scale);
							cv::line(cv_ptr->image, pt_r,pt2_r,cv::Scalar(0,0,255),4);
							cv::line(cv_ptr->image, pt_l,pt2_l,cv::Scalar(0,0,255),4);
							pt_r = pt2_r;
							pt_l = pt2_l;
						}
					}
					else
					{
						cv::Point pt_r = project( 1, y0, -1, scale);
						cv::Point pt_l = project(-1, y0, -1, scale);
						double Y = vel * DT;
						double Z = rd_inc.rd_ls_a * Y + rd_inc.rd_ls_b;
						cv::Point pt2_r = project( 1, Y, Z, scale);
						cv::Point pt2_l = project(-1, Y, Z, scale);

						cv::line(cv_ptr->image, pt_r,pt2_r,cv::Scalar(0,0,255),4);
						cv::line(cv_ptr->image, pt_l,pt2_l,cv::Scalar(0,0,255),4);

			}
				}
            }
			for(size_t i = 0; i < get_num_objects(); i++)
            {
                arbe_msgs::arbeTSlamObj object = get_object(i);
                int16_t cls2show = get_classes_to_show();
                if ( cls2show != -1 && cls2show != object.obj_class)
                    continue;
                std::string fc_txt="";
                float red,green,blue;
                get_class_color(object.obj_class, red, green, blue, fc_txt);

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
                if(get_distOnCam())
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

        }
        arbe_capture_pub.publish(cv_ptr->toCompressedImageMsg());
    }
    else
    {
        arbe_capture_pub.publish(image);
    }
}


void viewpanel::showSpeedometer( QGridLayout * layout) //QHBoxLayout* layout) //void )

{
	const QSize mSpeedGauge_size = QSize(150, 150);

	mSpeedGauge = new QcGaugeWidget;
	//this will add side color to gauge
	QcBackgroundItem *bkg1 = mSpeedGauge->addBackground(92);
	bkg1->clearrColors();
	bkg1->addColor(0.1,Qt::black);
	bkg1->addColor(1.0,Qt::white);
	QcBackgroundItem *bkg2 = mSpeedGauge->addBackground(88);
	bkg2->clearrColors();
	//this two line below will set background color of gauge
	bkg2->addColor(0.1,Qt::gray);
	bkg2->addColor(1.0,Qt::darkGray);

	mSpeedGauge->addArc(55);
	QcDegreesItem *degs = mSpeedGauge->addDegrees(65);
	degs->setValueRange(0,160);
	degs->setStep(20);

	//this will add color red-green color band
	/*QPair<QColor,float> pair;
	pair.first = Qt::red;
	pair.second = 100;
*/
	QPair<QColor,float> pair;
	QList<QPair<QColor, float> > colors;
	colors.clear();

	pair.first = Qt::green;
	pair.second = 10;
	colors.append(pair);

	pair.first = Qt::darkGreen;
	pair.second = 75;
	colors.append(pair);

	pair.first = Qt::red;
	pair.second = 100;
	colors.append(pair);

	QcColorBand * band = mSpeedGauge->addColorBand(50);
	band->setColors(colors);
	//	band->setDgereeRange(155,212);
//	setMaxDegree(120);//->
//	band->setMinDegree(90);
//	band->setColors(colors);

	// add value rage
	QcValuesItem *vals = mSpeedGauge->addValues(80);
	vals->setValueRange(0, 160);
	vals->setStep(20);
//	mSpeedGauge->addValues(80)->setValueRange(0, 160);

	//set gauge title
	mSpeedGauge->addLabel(70)->setText("Km/h");
	QcLabelItem *lab = mSpeedGauge->addLabel(40);
	lab->setText("0");
	mSpeedNeedle = mSpeedGauge->addNeedle(60);
	//add lable
	mSpeedNeedle->setLabel(lab);
	// set needle color
	mSpeedNeedle->setColor(Qt::white);
	//add range for your gauge => it should be just like the gauge range
	mSpeedNeedle->setValueRange(0,160);
	mSpeedGauge->addBackground(7);
	//add some shadow like glass
	mSpeedGauge->addGlass(88);
	mSpeedNeedle->setCurrentValue(0);

	mGpsSpeedNeedle = mSpeedGauge->addNeedle(60);
	//add lable
	// set needle color
	mGpsSpeedNeedle->setColor(Qt::blue);
	mGpsSpeedNeedle->setNeedle(QcNeedleItem::NeedleType::AttitudeMeterNeedle);

	//add range for your gauge => it should be just like the gauge range
	mGpsSpeedNeedle->setValueRange(0,160);
	//add some shadow like glass
	mGpsSpeedNeedle->setCurrentValue(160);



	//add this to your vertical layout
	layout->addWidget(mSpeedGauge,4,0,1,1);//layout->setProperty()
	mSpeedGauge->setFixedSize(mSpeedGauge_size);
}

void viewpanel::showTurnRate( QGridLayout * layout) //QHBoxLayout* layout) //void )

{
	const QSize mTurnRateGauge_size = QSize(150, 150);

	mTurnRateGauge = new QcGaugeWidget;
	//this will add side color to gauge
	QcBackgroundItem *bkg1 = mTurnRateGauge->addBackground(92);
	bkg1->clearrColors();
	bkg1->addColor(0.1,Qt::black);
	bkg1->addColor(1.0,Qt::white);
	QcBackgroundItem *bkg2 = mTurnRateGauge->addBackground(88);
	bkg2->clearrColors();
	//this two line below will set background color of gauge
	bkg2->addColor(0.1,Qt::gray);
	bkg2->addColor(1.0,Qt::darkGray);

	mTurnRateGauge->addArc(55);
	QcDegreesItem *degs = mTurnRateGauge->addDegrees(65);
	degs->setValueRange(-135,135);
	degs->setStep(45);

	//this will add color red-green color band
	/*QPair<QColor,float> pair;
	pair.first = Qt::red;
	pair.second = 100;
*/
	QPair<QColor,float> pair;
	QList<QPair<QColor, float> > colors;
	colors.clear();

	pair.first = Qt::red;
	pair.second = 100.0/3;
	colors.append(pair);

	pair.first = Qt::green;
	pair.second = 200.0/3;
	colors.append(pair);

	pair.first = Qt::red;
	pair.second = 100.0;
	colors.append(pair);

	QcColorBand * band = mTurnRateGauge->addColorBand(50);
	band->setColors(colors);

	// add value rage
	QcValuesItem *vals = mTurnRateGauge->addValues(80);
	vals->setValueRange(-135,135);
	vals->setStep(45);
//	mTurnRateGauge->addValues(80)->setValueRange(0, 160);

	//set gauge title
	mTurnRateGauge->addLabel(70)->setText("deg/s");
	QcLabelItem *lab = mTurnRateGauge->addLabel(40);
	lab->setText("0");
	mTurnRateNeedle = mTurnRateGauge->addNeedle(60);
	//add lable
	mTurnRateNeedle->setLabel(lab);
	// set needle color
    mTurnRateNeedle->setColor(Qt::red);
	//add range for your gauge => it should be just like the gauge range
	mTurnRateNeedle->setValueRange(-135,135);
	mTurnRateGauge->addBackground(7);
	//add some shadow like glass
	mTurnRateGauge->addGlass(88);
	mTurnRateNeedle->setCurrentValue(0);
	//add this to your vertical layout

	mImuTurnRateNeedle = mTurnRateGauge->addNeedle(60);
	//add lable
	// set needle color
	mImuTurnRateNeedle->setColor(Qt::blue);
	mImuTurnRateNeedle->setNeedle(QcNeedleItem::NeedleType::AttitudeMeterNeedle);

	//add range for your gauge => it should be just like the gauge range
	mImuTurnRateNeedle->setValueRange(-135,135);
	//add some shadow like glass
	mImuTurnRateNeedle->setCurrentValue(0);


	layout->addWidget(mTurnRateGauge,3,0,1,1);//layout->setProperty()
	mTurnRateGauge->setFixedSize(mTurnRateGauge_size);
}

void viewpanel::setCamera_sub_topic(bool flag)
{
	ros::NodeHandlePtr node_ptr = boost::make_shared<ros::NodeHandle>();
    int32_t ret;

	dl_training_view_active = flag;

	if(flag && get_dl_training_playback())
	{
		raw_cam_sub= node_ptr->subscribe("/arbe/dl_training/thumbnail/periodic/compressed", 1, SI_slam_on_cam);
	}
	else
	{
	}
	Camera_[0]->subProp("Image Topic")->setValue("/cv_camera_0/processed_image");
}

void viewpanel::print_cam_widg_location( void )
{
	ROS_INFO("Camera parameters: pos (%d,%d), dim [w/h] (%d,%d)",
			CameraWidget[0]->pos().x(), CameraWidget[0]->pos().y(),
			CameraWidget[0]->width(), CameraWidget[0]->height());
	ROS_INFO("Free-space parameters: pos (%d,%d), dim [w/h] (%d,%d)",
			FreeSpaceWidget->pos().x(), FreeSpaceWidget->pos().y(),
			FreeSpaceWidget->width(), FreeSpaceWidget->height());
}

void viewpanel::enableCamera( bool isUSB )
{
	int ret;

	/* Kill any previously left running cv_camera nodes */
	ret = system("killall pylon_camera_node&");
	ret = system("killall cv_camera_node&");
	ret = system("killall image_proc");

	//CameraWidget[0]->close();
	/* Create rqt_bag launch command */
	
	for (int nr = 0; nr < num_of_radars; nr++)
	{
		if(isUSB)
			sprintf(cv_camera_launch_command,"roslaunch arbe_phoenix_radar_driver camera.launch antenna_num:=%d video_device_num:=%d&",nr, get_video_dev_num());
		else
			sprintf(cv_camera_launch_command,"roslaunch arbe_phoenix_radar_driver pylon_camera_node.launch antenna_num:=%d device_id:=%d&", nr, camera_id[nr]);


		ROS_DEBUG("launch camera: %s\n",cv_camera_launch_command);

		ret = system(cv_camera_launch_command);
		if(!isUSB)
		{
			sprintf(cv_camera_launch_command,"ROS_NAMESPACE=pylon_camera_node_%d rosrun image_proc image_proc&",nr);
			ret = system(cv_camera_launch_command);
		}


		std::string pointcloud_topic = "/arbe/rviz/pointcloud_"+std::to_string(radar_id);
		std::string stationary_pointcloud_topic = "/arbe/rviz/stationary_pointcloud_"+std::to_string(radar_id);
		ROS_INFO("Registering new pointcloud topic: %s",pointcloud_topic.c_str());
		ROS_INFO("Registering new pointcloud topic: %s",stationary_pointcloud_topic.c_str());

		std::string title = "Arbe Camera View" + std::to_string(nr);
		Camera_[nr] = manager_->createDisplay( "rviz/Image", title.c_str(), true );

		std::string cstr_me = "/cv_camera_"+std::to_string(nr) +"/processed_image";
		Camera_[nr]->subProp("Image Topic")->setValue(cstr_me.c_str());
		printf("\r\n\r\n***topic inside viewpanel is %s\r\n\r\n\r\n",cstr_me.c_str());

		Camera_[nr]->subProp("Transport Hint")->setValue("compressed");

		CameraWidget[nr] = Camera_[nr]->getAssociatedWidget();
		Qt::WindowFlags flags = CameraWidget[nr]->windowFlags();
    flags |= Qt::WindowStaysOnTopHint;

		CameraWidget[nr]->setGeometry(camera_geometry);
		CameraWidget[nr]->setWindowFlags(flags);

    sleep(1);
		CameraWidget[nr]->show();

}
}

void viewpanel::update_text_metrics( QString * labelText )
{
	if ( overlay_text_label == NULL )
		return;
	overlay_text_label->setText(*labelText);
	qApp->processEvents();
}

void viewpanel::update_controls( gui_controls_t * gui_controls_data )
{
	if ( ModeCombo != NULL )
	ModeCombo->setCurrentIndex(gui_controls_data->mode_index);
	if ( RangeCombo != NULL )
	RangeCombo->setCurrentIndex(gui_controls_data->range_index);
	if ( sensitivity_label != NULL )
	sensitivity_label->setText( "Sensitivity ("+QString::number( gui_controls_data->threshold4d )+")" );
	if (dynamic_azimuth_label != NULL)
	dynamic_azimuth_label->setText( "Dynamic Azimuth ("+QString::number( gui_controls_data->DynamicAzimuth )+")" );
	if (dynamic_elevation_label != NULL)
	dynamic_elevation_label->setText( "Dynamic Elevation ("+QString::number( gui_controls_data->DynamicElevation )+")" );
	if (sensitivity_slider != NULL)
	sensitivity_slider->setValue(gui_controls_data->threshold4d);
	if (dynamic_azimuth_slider != NULL)
	dynamic_azimuth_slider->setValue(gui_controls_data->DynamicAzimuth);
	if (dynamic_elevation_slider != NULL)
	dynamic_elevation_slider->setValue(gui_controls_data->DynamicElevation);
	qApp->processEvents();
}

void viewpanel::get_controls_values( gui_controls_t * gui_controls_data )
{
	if ( ModeCombo == NULL )
		return;
	gui_controls_data->mode_index = ModeCombo->currentIndex();
	gui_controls_data->range_index = RangeCombo->currentIndex();
	gui_controls_data->threshold4d = threshold4d;
	gui_controls_data->DynamicAzimuth = DynamicAzimuth;
	gui_controls_data->DynamicElevation = DynamicElevation;
	gui_controls_data->isNtc3dOn = ntc_3d_enabled>0;
	gui_controls_data->isNtc4dOn = ntc_4d_enabled>0;
	gui_controls_data->isCfar3dOn = cfar_3d_enabled>0;
	gui_controls_data->isCfar4dOn = cfar_4d_enabled>0;
	gui_controls_data->isPhaseEnabled = phase_enabled>0;
}


void viewpanel::enableFreeSpaceView( void )
{
//	char cv_freeSpaceView_launch_command[60];
//	/* Create rqt_bag launch command */
//	sprintf(cv_freeSpaceView_launch_command,"roslaunch arbe_phoenix_radar_driver camera.launch&");
//	ROS_DEBUG("launch camera: %s\n",cv_freeSpaceView_launch_command);
//	int ret = system(cv_freeSpaceView_launch_command);

	/* Create a camera image display */
	FreeSpace_ = manager_->createDisplay( "rviz/Image", "Arbe Free-Space View", true );
	ROS_ASSERT( FreeSpace_ != NULL );

	//setCamera_sub_topic();
	FreeSpace_->subProp("Image Topic")->setValue("/arbe/processed/free_space/map/0");
	FreeSpace_->subProp("Transport Hint")->setValue("compressed");

    FreeSpaceWidget = FreeSpace_->getAssociatedWidget();
    Qt::WindowFlags flags = FreeSpaceWidget->windowFlags();
    flags |= Qt::WindowStaysOnTopHint;
    QPoint pos = FreeSpaceWidget->pos();
    pos.setX(1271);
    pos.setY(626);
    FreeSpaceWidget->move(pos);
    FreeSpaceWidget->setMinimumWidth(300);
    FreeSpaceWidget->setMinimumHeight(300);

    FreeSpaceWidget->setWindowFlags(flags);
    FreeSpaceWidget->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding);


    sleep(1);
    FreeSpaceWidget->show();
}

viewpanel* viewpanel::m_pInstance = NULL;

viewpanel* viewpanel::Instance()
{
	std::cout << " until here " <<  __LINE__ << std::endl;
    if (!m_pInstance)
        m_pInstance = new viewpanel;

    return m_pInstance;
}
