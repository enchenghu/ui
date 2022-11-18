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

#include "mainwindow.h"
#include "viewpanel.h"
#include "common.hpp"
#include <std_msgs/UInt8MultiArray.h>
#include <std_msgs/Bool.h>
#include <std_msgs/String.h>
#include <arbe_msgs/arbeSlamMsg.h>
#include "arbe_msgs/arbeNewPcMsg.h"
#include <geometry_msgs/TwistWithCovarianceStamped.h>
#include <sensor_msgs/Imu.h>
#include <arbe_msgs/arbeBoolWithTime.h>
#include <fcntl.h>
#include <termios.h>

#ifndef __has_include
  static_assert(false, "__has_include not supported");
#else
#  if __has_include(<filesystem>)
#    include <filesystem>
     namespace fs = std::filesystem;
#  elif __has_include(<experimental/filesystem>)
#    include <experimental/filesystem>
     namespace fs = std::experimental::filesystem;
#  elif __has_include(<boost/filesystem.hpp>)
#    include <boost/filesystem.hpp>
     namespace fs = boost::filesystem;
#  endif
#endif

using namespace std;

#define DEFAULT_DISP_FS_ON_PC true
#define DEFAULT_CAMERA_BUFFERIBG false
#define DEFAULT_SLAM_ENABLED false
#define DEFAULT_FS_ENABLED false
#define DEFAULT_DIST_ON_CAM false
#define DEFAULT_CLASSIFICATION_ENABLED true
#define DEFAULT_FREESPACE_ENABLED true
#define DEFAULT_CAM_GIGE true

extern void radar_quit();
extern int rosbag_playing;
extern int unity_playing;
extern FILE * unity_bin_file;
extern void pubGUIcontrols();
extern ros::NodeHandlePtr n;

#if 0
extern std::vector<ros::Subscriber> targets_subscribers;
extern std::vector<ros::Subscriber> stationary_targets_subscribers;
extern std::vector<ros::Subscriber> objects_subscribers;
#endif

extern ros::Subscriber gps_sub;
extern ros::Subscriber imu_sub;
extern ros::Subscriber objects_sub;
extern ros::Subscriber ctl_reset_PC_aggregation;
static ros::Subscriber cmnd_line_dipsDist_sub;
static ros::Subscriber cmnd_line_mapping_dview_x_sub;
static ros::Subscriber cmnd_line_mapping_dview_y_sub;
static ros::Subscriber cmnd_line_load_camera_file_sub;

ros::Publisher  resetOmegaCalib_pub;
ros::Publisher  imposeOmegaZero_pub;
ros::Publisher  restore_defaults_pub;
ros::Publisher  disp_FS_on_pc_pub;
ros::Publisher  enableSLAM_pub;
ros::Publisher  resetSLAM_pub;
ros::Publisher  reset_local_cartesian_pub;
ros::Publisher  enableClassification_pub;
ros::Publisher  enableFreeSpace_pub;
extern void *radar_logger_thread(void* args);
extern void imu_read_callback(const sensor_msgs::Imu::ConstPtr& msg);
extern void gps_read_callback(const geometry_msgs::TwistWithCovarianceStamped::ConstPtr& msg);
extern void slam_read_callback(const arbe_msgs::arbeSlamMsg::ConstPtr& array, int n_radar);
extern void slam_ctrl_callback(const std_msgs::Bool::ConstPtr &msg);
extern void targets_read_callback(const arbe_msgs::arbeNewPcMsg::ConstPtr& targetsArray, int n_radar);
extern void stationary_targets_read_callback(const arbe_msgs::arbeNewPcMsg::ConstPtr& targetsArray, int n_radar);
extern void radar_set_chamber_mode( bool enable );
extern void radar_read_debug_data( void );
extern void Ntc_Mode_Set( void );
extern void Cfar_Mode_Set( void );
extern void radar_configure_pc();
extern float radar_x_offset;
//extern float radar_y_offset;
extern float radar_z_offset;
extern float radar_yaw_angle;
extern float radar_pitch_angle;

viewpanel* viewPanel;
QString playback_file;
QString radar_region;
QString bias_4d;
QString radar_debug_port;
QString video_device;
QVariant chamber_mode_state;
QVariant logging_state;
QVariant ntc_3d_state;
QVariant ntc_4d_state;
QVariant cfar_3d_state;
QVariant cfar_4d_state;
QVariant output_thr_metadata_state;
QVariant phase_state;
QVariant EnableSlam_state;
QVariant camera_type;
QString camera_file;
extern QComboBox* RangeCombo;
extern QWidget* CameraWidget[MAX_RADARS];
QRect camera_geometry;
QString noise_level;

QString save_folder = ".";


extern float cam_euler_alpha;
extern float cam_euler_beta;
extern float cam_euler_gamma;
extern bool ntc_3d_enabled;
extern bool ntc_4d_enabled;
extern bool cfar_3d_enabled;
extern bool cfar_4d_enabled;
extern bool phase_enabled;
extern bool algo_thr_metadata_enabled;
extern int threshold4d;
extern int threshold3d;
bool slam_displayed = false;

extern int camera_x_pos;
extern int camera_y_pos;

bool logging_enabled = false;

static bool rec_all_pp = false;
static bool rec_camera = true;
static bool rec_all_debug = false;
static bool rec_dl_training = false;
static bool rec_fusion = false;
static bool rec_parsed = true;
static bool rec_raw_topics = true;
static bool rec_settings_topics = true;
static bool rec_legacy_pc = false;
static bool rec_nav_topics = true;
static bool rec_object_list = true;

static bool rec_rviz_pointcloud = false;
static int video_dev_num = 0;
extern QString TestDescriptionString;
static int16_t now_showing = -1;

bool get_rec_topics(bool &all_pp, bool &camera, bool &all_debug, bool &dl_training, bool &fusion, bool &parsed, bool &raw_topics, bool &settings_topics, bool &legacy_pc, bool &nav_topics, bool &object_list)
{
    all_pp = rec_all_pp ;
    camera = rec_camera ;
    all_debug = rec_all_debug ;
    dl_training = rec_dl_training ;
    fusion = rec_fusion ;
    parsed = rec_parsed ;
    raw_topics = rec_raw_topics ;
    settings_topics = rec_settings_topics ;
    legacy_pc = rec_legacy_pc ;
	nav_topics = rec_nav_topics;
	object_list = rec_object_list;

	return rec_all_pp || rec_camera || rec_all_debug || rec_dl_training || rec_fusion || rec_parsed ||
			rec_raw_topics || rec_settings_topics || rec_legacy_pc || rec_nav_topics || rec_object_list;
}

bool get_rec_all_pp()
{
    return rec_all_pp;
}

string get_save_folder()
{
	return save_folder.toStdString();
}

bool get_rec_rviz_pointcloud()
{
	return rec_rviz_pointcloud;
}

uint8_t get_video_dev_num()
{
	return video_dev_num;
}

void init_reset_omega_calib_pub(ros::NodeHandlePtr node_ptr)
{
    resetOmegaCalib_pub = node_ptr->advertise<arbe_msgs::arbeBoolWithTime>("/arbe/settings/reset_omega_calibration",1);
	imposeOmegaZero_pub = node_ptr->advertise<arbe_msgs::arbeBoolWithTime>("/arbe/settings/impose_zero_omega",1, true);
	restore_defaults_pub = node_ptr->advertise<std_msgs::Bool>("/arbe/settings/restore_defaults",1);
	disp_FS_on_pc_pub = node_ptr->advertise<std_msgs::Bool>("/arbe/settings/disp_fs_on_pc",1);
}

void init_reset_local_cartesian_pub(ros::NodeHandlePtr node_ptr)
{
    reset_local_cartesian_pub = node_ptr->advertise<std_msgs::Bool>("/arbe/settings/reset_geo_location",1);
}

void init_enableSlam_pub(ros::NodeHandlePtr node_ptr)
{
    enableSLAM_pub = node_ptr->advertise<arbe_msgs::arbeBoolWithTime>("/arbe/settings/enable_slam",1,true);
	ROS_INFO("Arbe real-time AI is enabled");
    arbe_msgs::arbeBoolWithTime def_msg;
    def_msg.flag = DEFAULT_SLAM_ENABLED;
    ros::Time rosTime;
    def_msg.header.stamp = rosTime.now();
	enableSLAM_pub.publish(def_msg);
}
void init_enableClassification_pub(ros::NodeHandlePtr node_ptr)
{
	enableClassification_pub = node_ptr->advertise<arbe_msgs::arbeBoolWithTime>("/arbe/settings/enable_classification",1,true);
    arbe_msgs::arbeBoolWithTime def_msg;
	def_msg.flag = DEFAULT_CLASSIFICATION_ENABLED;
    ros::Time rosTime;
    def_msg.header.stamp = rosTime.now();
	enableClassification_pub.publish(def_msg);
}
void init_resetSlam_pub(ros::NodeHandlePtr node_ptr)
{
    resetSLAM_pub = node_ptr->advertise<arbe_msgs::arbeBoolWithTime>("/arbe/settings/reset_slam",1);
	ROS_INFO("Arbe real-time AI reset");
}

void init_enableFreeSpace_pub(ros::NodeHandlePtr node_ptr)
{
    enableFreeSpace_pub = node_ptr->advertise<arbe_msgs::arbeBoolWithTime>("/arbe/free_space/enable",1,true);
    arbe_msgs::arbeBoolWithTime def_msg;
	def_msg.flag = DEFAULT_FREESPACE_ENABLED;
    ros::Time rosTime;
    def_msg.header.stamp = rosTime.now();
    enableFreeSpace_pub.publish(def_msg);
}

void MainWindow::find_serial_ports()
{
	std::vector<std::string> ports;
	fs::path kdr_path{"/proc/tty/drivers"};
	if (fs::exists(kdr_path))
	{
		std::ifstream ifile(kdr_path.generic_string());
		std::string line;
		std::vector<std::string> prefixes;
		while (std::getline(ifile, line))
		{
			std::vector<std::string> items;
			auto it = line.find_first_not_of(' ');
			while (it != std::string::npos)
			{

				auto it2 = line.substr(it).find_first_of(' ');
				if (it2 == std::string::npos)
				{
					items.push_back(line.substr(it));
					break;
				}
				it2 += it;
				items.push_back(line.substr(it, it2 - it));
				it = it2 + line.substr(it2).find_first_not_of(' ');
			}
			if (items.size() >= 5)
			{
				if (items[4] == "serial" && items[0].find("serial") != std::string::npos)
				{
					prefixes.emplace_back(items[1]);
				}
			}
		}
		ifile.close();
		for (auto& p: fs::directory_iterator("/dev"))
		{
			for (const auto& pf : prefixes)
			{
				auto dev_path = p.path().generic_string();
				if (dev_path.size() >= pf.size() && std::equal(dev_path.begin(), dev_path.begin() + pf.size(), pf.begin()))
				{
					ports.emplace_back(dev_path);
					//printf("dev_path %s\n",dev_path.c_str());
					std::size_t found=dev_path.find("ttyUSB");
					if (found!=std::string::npos)
						debug_port_combo->insertItem(0,tr(dev_path.c_str()));
				}
			}
		}
	}
	ROS_INFO("Created list of available serial ports");
}

void MainWindow::find_video_devices()
{
	std::vector<std::string> devices;
	for (auto& p: fs::directory_iterator("/dev"))
	{
		auto dev_path = p.path().generic_string();
		devices.emplace_back(dev_path);
		std::size_t found=dev_path.find("video");
		if (found == 5)
		{
			video_device_combo->insertItem(0,tr(dev_path.c_str()));
			//ROS_DEBUG("found %s \n",dev_path.c_str());
		}
	}
	ROS_INFO("Created list of available usb cameras");
}

MainWindow::MainWindow()
{
#if ONLY_SHOW_UI
	init_cmnd_line_subs();
#endif
	slamDispOnCam        = false;
	slamDispOnCamEcoMode = false;
	slamDispDistOnCam    = false;
	slamColorByClass     = false;
	FSDispOnCam          = false;
	cameraBuffering      = DEFAULT_CAMERA_BUFFERIBG;

	set_slamOnCam(slamDispOnCam);
	set_slamOnCam_ecoMode(slamDispOnCamEcoMode);
	set_distOnCam(slamDispDistOnCam);
	set_colorObjByClass(slamColorByClass);


	textEdit = new QPlainTextEdit;
	createActions();

	ROS_INFO("Arbe GUI subscribed to unprocessed PC");
	//objects_sub = n->subscribe("/arbe/processed/objects", 1, slam_read_callback);
	createMenus();
	ROS_INFO("createMenus success ");
#if ONLY_SHOW_UI
	init_reset_omega_calib_pub(n);
	init_reset_local_cartesian_pub(n);
	init_enableSlam_pub(n);
	init_enableClassification_pub(n);
	init_resetSlam_pub(n);
	init_enableFreeSpace_pub(n);
#endif
	readSettings();
	ROS_INFO("readSettings success ");
	viewPanel = viewpanel::Instance();//new viewpanel();
	ROS_INFO("viewpanel get success ");
	int width = 1024;
	int height = 300;
	viewPanel->setMinimumSize(width, height);

	//this->layout()->addWidget(viewPanel);
	///this->addDockWidget(Qt::AllDockWidgetAreas, viewPanel);

#if 0
	QDockWidget* dockDebug = new QDockWidget(tr("Lidar Debug window"), this);
    dockDebug->setFeatures(QDockWidget::DockWidgetClosable );
	this->tabifyDockWidget(viewPanel,  dockDebug);
#endif

	setCentralWidget(viewPanel);


#if ONLY_SHOW_UI
	setCurrentFile("");
	setUnifiedTitleAndToolBarOnMac(true);

	ChamberAct->setChecked(chamber_mode_state.toBool());
	ChamberModeControl();

	loggingAct->setChecked(logging_state.toBool());
	LoggingControl();

	GigEcameraAct->setChecked(camera_type.toBool());
	USBcameraAct->setChecked(!(camera_type.toBool()));
	EnableSlamAct->setChecked(EnableSlam_state.toBool());
	//enableSlam();

	enableCamera();

	EnableClassificationAct->setChecked(DEFAULT_CLASSIFICATION_ENABLED);
	enableClassification();

	EnableFreeSpaceAct->setChecked(DEFAULT_FREESPACE_ENABLED);
	enableFreeSpace();
	enableSlam();

	load_settings();
#endif
	//viewPanel->enableFreeSpaceView();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
	viewPanel->printView();
	//viewPanel->print_cam_widg_location();
	event->accept();
	writeSettings();
	int ret = system("pkill -f rqt_bag");
	close();
	radar_quit();
	ret = 	system("rosnode kill --all");
}

void MainWindow::init_cmnd_line_subs( void )
{
	ros::NodeHandlePtr node_ptr = boost::make_shared<ros::NodeHandle>();
	cmnd_line_dipsDist_sub = node_ptr->subscribe("/arbe/settings/cmnd_line_dispdist",1,&MainWindow::on_cmnd_line_distdisp_callback,this);
    cmnd_line_mapping_dview_x_sub = node_ptr->subscribe("/arbe/settings/cmnd_line_dview_x",1,&MainWindow::on_cmnd_dview_x_callback,this);
    cmnd_line_mapping_dview_y_sub = node_ptr->subscribe("/arbe/settings/cmnd_line_dview_y",1,&MainWindow::on_cmnd_dview_y_callback,this);
	cmnd_line_load_camera_file_sub = node_ptr->subscribe("/arbe/settings/cmnd_line_camera_file",1,&MainWindow::on_cmnd_camera_file_callback,this);
}

void MainWindow::on_cmnd_line_distdisp_callback(std_msgs::Bool msg)
{
	if((msg.data && !slamDispDistOnCam) || (!msg.data && slamDispDistOnCam))
		toggle_dist_on_cam();
}

void MainWindow::on_cmnd_dview_x_callback(std_msgs::Float32 msg)
{
    set_dview_x(msg.data);
}

void MainWindow::on_cmnd_dview_y_callback(std_msgs::Float32 msg)
{
    set_dview_y(msg.data);
}

void MainWindow::on_cmnd_camera_file_callback(std_msgs::String msg)
{
}


void MainWindow::slam_reconfigure( void )
{
	std::string command = "rosrun rqt_reconfigure rqt_reconfigure &";

	QApplication::setOverrideCursor(Qt::WaitCursor);
	int ret = system(command.c_str());
	sleep(1);
	QApplication::restoreOverrideCursor();

}

void MainWindow::open( void )
{
	/* Kill any previously left rqt_bag window */
	int ret = system("pkill -f rqt_bag");

	playback_file =  QFileDialog::getOpenFileName(
	  this,
	  "Open Recordings",
	  QDir::currentPath(),
	  "Rosbag files (*.bag);;Unity bin files (*.bin)",0,QFileDialog::DontUseNativeDialog);

	if( !playback_file.isNull() )
	{
		qDebug() << "selected file path : " << playback_file.toUtf8();
	}
        setCurrentFile(playback_file);

	if(playback_file.toStdString().find(".bag") != std::string::npos)
	{
		char rqt_bag_launch_command[300];
		/* Create rqt_bag launch command */
		sprintf(rqt_bag_launch_command,"roslaunch arbe_phoenix_radar_driver arbe_rqt_bag.launch bag:=%c%c%s%c%c&",
			'"','\047',(char *)playback_file.toStdString().c_str(),'\047','"');
		ROS_DEBUG("launch playback menu control: %s",rqt_bag_launch_command);
		//rosbag_playing = 1;
		unity_playing = 0;
		/* wait for rqt_bag to start displaying a wait cursor */
		QApplication::setOverrideCursor(Qt::WaitCursor);
		int ret = system(rqt_bag_launch_command);
		sleep(4); 
		QApplication::restoreOverrideCursor();
	} else if (playback_file.toStdString().find(".bin") != std::string::npos)
	{
		unity_bin_file = fopen(playback_file.toStdString().c_str(), "rb");
		unity_playing = 1;
		//rosbag_playing = 0;
	}
}

void MainWindow::setSaveFolder()
{

	save_folder =  QFileDialog::getExistingDirectory(this, "Choose folder for saving bags", QDir::currentPath(),
		QFileDialog::DontUseNativeDialog | QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

	if( !save_folder.isNull() )
	{
		qDebug() << "selected default recording folder : " << save_folder.toUtf8();
	}
	else
	{
		save_folder = QString(".");
	}
}

void MainWindow::toggle_camera_buffering( void )
{
	cameraBuffering = !cameraBuffering;
	allowCameraBufferingForSyncAct->setChecked(cameraBuffering);
	set_cameraBuffering(cameraBuffering);
}


void MainWindow::toggle_slam_on_cam( void )
{
	slamDispOnCam = !slamDispOnCam;
	toggleSlamOnImageAct->setChecked(slamDispOnCam);
	set_slamOnCam(slamDispOnCam);
    pubGUIcontrols();
}

void MainWindow::toggle_FS_on_cam( void )
{
	FSDispOnCam = !FSDispOnCam;
    toggleFSOnImageAct->setChecked(FSDispOnCam);
	set_FreeSpace_enabled(FSDispOnCam);
    pubGUIcontrols();
}

void MainWindow::toggle_funnel_on_cam( void )
{
	FunnelDispOnCam = !FunnelDispOnCam;
	toggleFunnelOnImageAct->setChecked(FunnelDispOnCam);
	set_funnel_enabled(FunnelDispOnCam);
	pubGUIcontrols();
}

void MainWindow::toggle_discard_out_of_el_context( void )
{
	discardOutOfElContext = !discardOutOfElContext;
	toggleDiscardOutOfContextAct->setChecked(discardOutOfElContext);
	set_discard_out_of_el_context(discardOutOfElContext);

	pubGUIcontrols();
}

void MainWindow::toggle_asphalt_text( void )
{
}


void MainWindow::toggle_dist_on_cam( void )
{
	slamDispDistOnCam = !slamDispDistOnCam;
	toggleDispDistOnImageAct->setChecked(slamDispDistOnCam);
	set_distOnCam(slamDispDistOnCam);
	pubGUIcontrols();
}

void MainWindow::toggle_slam_display_economy( void )
{
	slamDispOnCamEcoMode = !slamDispOnCamEcoMode;
	toggleSlamDisplayEconomyModeAct->setChecked(slamDispOnCamEcoMode);
	set_slamOnCam_ecoMode(slamDispOnCamEcoMode);
	pubGUIcontrols();
}

void MainWindow::toggle_slam_color_by_class( void )
{
    slamColorByClass = !slamColorByClass;
    toggleSlamColorByClassAct->setChecked(slamColorByClass);
    set_colorObjByClass(slamColorByClass);
	pubGUIcontrols();
}

void MainWindow::load_camera_calibration( void )
{
	}

void MainWindow::stopPlayback( void )
{
	if (unity_playing == 1)
	{
		fclose(unity_bin_file);
		unity_playing = 0;
	} else
	if (rosbag_playing == 1)
	{
		/* Kill any previously left rqt_bag window */
		int ret = system("pkill -f rqt_bag");
		//rosbag_playing = 0;
	}
}

char rosbag_convert_command[500];
char rosbag_convert_script_path[200];
void MainWindow::convertRosbag( void )
{
	sprintf(rosbag_convert_script_path,SRC_DIR);
	sprintf(rosbag_convert_script_path+strlen(rosbag_convert_script_path),"/scripts/rosbag_to_bin_and_mp4.py");
	QString selectedFilter;
	QStringList files;
	files = QFileDialog::getOpenFileNames(
		                this, tr("Select rosbag files to convert)"),
		                openFilesPath,
		                tr("Rosbag files (*.bag)"),
		                &selectedFilter,
		                QFileDialog::DontUseNativeDialog);

	QProgressDialog progress("rosbag conversion in progress", "Cancel", 0, files.count(), this);
	progress.setWindowModality(Qt::WindowModal);

	for (int i = 0; i < files.count(); i++)
	{
		progress.setValue(i);
		QCoreApplication::processEvents();
		openFilesPath = files[i];
		ROS_DEBUG("selected file %s",files[i].toStdString().c_str());
		sprintf(rosbag_convert_command,"%s",rosbag_convert_script_path);
		sprintf(rosbag_convert_command+strlen(rosbag_convert_command)," %s",
			files[i].toStdString().c_str());
		int ret = system(rosbag_convert_command);
		if (progress.wasCanceled())
		    break;
	}
	progress.setValue(files.count());
	QMessageBox msgBox;
	msgBox.setText("Coversion completed");
	msgBox.exec();
}

void MainWindow::launchGigECam( void )
{
	GigEcameraAct->setChecked(true);
	USBcameraAct->setChecked(false);
}
void MainWindow::launchUSBCam( void )
{
	USBcameraAct->setChecked(true);
	GigEcameraAct->setChecked(false);
}

void MainWindow::enableCamera( void )
{
}

void MainWindow::enableFreeSpaceTopView( void )
{
}

void MainWindow::radarReset( void )
{

}

void MainWindow::selectDetectionsType( void )
{
	if(DetectionsTypeAct->isChecked())
	{
        set_disp_processed_pc(true);
		ROS_INFO("Processed pointcloud display option was selected");
	}
	else
	{
		ROS_INFO("Unprocessed pointcloud display option was selected");
        set_disp_processed_pc(false);
	}
	pubGUIcontrols();
}

void MainWindow::selectSlam( void )
{

}

void MainWindow::resetOmegaCalib( void )
{
    arbe_msgs::arbeBoolWithTime msg;
    msg.flag = true;
    ros::Time rosTime;
    msg.header.stamp = rosTime.now();
	resetOmegaCalib_pub.publish(msg);
    std_msgs::Float32 tmp_msg;
	tmp_msg.data = -10000.0;
	ROS_INFO("Installation error calibration has been reset");
}

void MainWindow::imposeOmegaZero( void )
{
	arbe_msgs::arbeBoolWithTime msg;
	msg.flag = true;
	ros::Time rosTime;
	msg.header.stamp = rosTime.now();
	imposeOmegaZero_pub.publish(msg);
	std_msgs::Float32 tmp_msg;
	tmp_msg.data = -10000.0;
	ROS_INFO("Installation error calibration has been imposed");
}

void MainWindow::restoreDefaults( void )
{
	std_msgs::Bool msg;
	msg.data = true;
	restore_defaults_pub.publish(msg);
}

void MainWindow::dispFSOnPc( void )
{
	std_msgs::Bool msg;
	msg.data = dispFSPolyOnPcAct->isChecked();
	disp_FS_on_pc_pub.publish(msg);
}

void MainWindow::EnableAggregation( void )
{

}

void MainWindow::setDLTrainingPlayback( void )
{
	set_dl_training_playback(DLTrainingPlaybackAct->isChecked());
}

void MainWindow::setDLTrainingView( void )
{
    if(DLTrainingViewAct->isChecked())
    {
        // turn on DL view

//		for(size_t nr = 0; nr < targets_subscribers.size(); nr++)
//		{
//			targets_subscribers.at(nr) = n->subscribe<arbe_msgs::arbeNewPcMsg>(
//											  "/arbe/dl_training/current_pc_dyn/"+std::to_string(nr), 1, boost::bind(targets_read_callback, _1, (int)(nr)));
//			if(targets_subscribers.size() == stationary_targets_subscribers.size())
//				stationary_targets_subscribers.at(nr) = n->subscribe<arbe_msgs::arbeNewPcMsg>(
//												  "/arbe/dl_training/current_pc_stationary/"+std::to_string(nr), 1, boost::bind(stationary_targets_read_callback, _1, (int)(nr)));
//			else
//				stationary_targets_subscribers.push_back(n->subscribe<arbe_msgs::arbeNewPcMsg>(
//												  "/arbe/dl_training/current_pc_stationary/"+std::to_string(nr), 1, boost::bind(stationary_targets_read_callback, _1, (int)(nr))));
//		}

//		for(size_t nr = 0; nr < objects_subscribers.size(); nr++)
//		{
//			objects_subscribers.at(nr) = n->subscribe<arbe_msgs::arbeSlamMsg>(
//											  "/arbe/dl_training/objects/"+std::to_string(nr), 1, boost::bind(slam_read_callback, _1, nr));
//		}

		//objects_sub = n->subscribe("/arbe/dl_training/objects", 1, slam_read_callback);
		cmnd_line_load_camera_file_sub = n->subscribe("/arbe/dl_training/cmnd_line_camera_file", 1, &MainWindow::on_cmnd_camera_file_callback,this);
        DetectionsTypeAct->setDisabled(true);
        ShowSlamAct->setChecked(true);
        ShowSlamAct->setDisabled(true);
        DLTrainingChooseAllClassesAct->setEnabled(true);
        DLTrainingChooseClass0Act->setEnabled(true);
        DLTrainingChooseClass1Act->setEnabled(true);
        DLTrainingChooseClass2Act->setEnabled(true);
        DLTrainingChooseClass3Act->setEnabled(true);
        DLTrainingChooseClass4Act->setEnabled(true);
        set_classes_to_show(-1);
        now_showing = -1;
        DLTrainingChooseAllClassesAct->setChecked(true);
        DLTrainingChooseClass0Act->setChecked(false);
        DLTrainingChooseClass1Act->setChecked(false);
        DLTrainingChooseClass2Act->setChecked(false);
        DLTrainingChooseClass3Act->setChecked(false);
        DLTrainingChooseClass4Act->setChecked(false);


    }
    else
    {
		// turn off DL view
		cmnd_line_load_camera_file_sub = n->subscribe("/arbe/settings/cmnd_line_camera_file",1,&MainWindow::on_cmnd_camera_file_callback,this);

        DetectionsTypeAct->setEnabled(true);
        ShowSlamAct->setEnabled(true);
        selectDetectionsType();
        selectSlam();
        DLTrainingChooseAllClassesAct->setChecked(true);
        set_classes_to_show(0);
        DLTrainingChooseAllClassesAct->setDisabled(true);
        DLTrainingChooseClass0Act->setDisabled(true);
        DLTrainingChooseClass1Act->setDisabled(true);
        DLTrainingChooseClass2Act->setDisabled(true);
        DLTrainingChooseClass3Act->setDisabled(true);
        DLTrainingChooseClass4Act->setDisabled(true);
    }
}

void MainWindow::setClassesToShow( void)
{
    int16_t cls = -1;
    switch(now_showing)
    {
    case -1:
        DLTrainingChooseAllClassesAct->setChecked(false);
        break;
    case 0:
        DLTrainingChooseClass0Act->setChecked(false);
        break;
    case 1:
        DLTrainingChooseClass1Act->setChecked(false);
        break;
    case 2:
        DLTrainingChooseClass2Act->setChecked(false);
        break;
    case 3:
        DLTrainingChooseClass3Act->setChecked(false);
        break;
    case 4:
        DLTrainingChooseClass4Act->setChecked(false);
        break;
    }
    if(DLTrainingChooseAllClassesAct->isChecked())
    {
        cls = -1;
        now_showing = -1;
    }
    else if(DLTrainingChooseClass0Act->isChecked())
    {
        cls = 0;
        now_showing = 0;
    }
    else if(DLTrainingChooseClass1Act->isChecked())
    {
        cls = 1;
        now_showing = 1;
    }
    else if(DLTrainingChooseClass2Act->isChecked())
    {
        cls = 2;
        now_showing = 2;
    }
    else if(DLTrainingChooseClass3Act->isChecked())
    {
        cls = 3;
        now_showing = 3;
    }
    else if(DLTrainingChooseClass4Act->isChecked())
    {
        cls = 4;
        now_showing = 4;
    }
    else
    {
        switch(now_showing)
        {
        case -1:
            DLTrainingChooseAllClassesAct->setChecked(true);
            break;
        case 0:
            DLTrainingChooseClass0Act->setChecked(true);
            break;
        case 1:
            DLTrainingChooseClass1Act->setChecked(true);
            break;
        case 2:
            DLTrainingChooseClass2Act->setChecked(true);
            break;
        case 3:
            DLTrainingChooseClass3Act->setChecked(true);
            break;
        case 4:
            DLTrainingChooseClass4Act->setChecked(true);
            break;

        }
    }

    set_classes_to_show(cls);
}


void MainWindow::EnableGeoLocalization( void )
{
}

void MainWindow::ResetLocalCartesian( void )
{
	std_msgs::Bool msg;
	msg.data = true;
	reset_local_cartesian_pub.publish(msg);
    set_reset_mapping(true);
}

void MainWindow::enableSlam( void )
{
    arbe_msgs::arbeBoolWithTime msg;

	bool isEnableSlam = EnableSlamAct->isChecked();
	if(isEnableSlam)
	{
		ROS_INFO("Arbe real-time AI is enabled");
	}
	else
	{
		ROS_INFO("Arbe real-time AI is disabled");
	}

    msg.flag = isEnableSlam;
    msg.header.stamp = ros::Time::now();
	DetectionsTypeAct->setChecked(false); //isEnableSlam
	ShowSlamAct->setChecked(isEnableSlam);
	toggleDispDistOnImageAct->setChecked(isEnableSlam && get_distOnCam());
	enableSLAM_pub.publish(msg);
	selectDetectionsType();
	selectSlam();
}


void MainWindow::enableClassification( void )
{
    arbe_msgs::arbeBoolWithTime msg;
	bool isEnableClassification= EnableClassificationAct->isChecked();
	if(isEnableClassification)
	{
		ROS_INFO("Arbe real-time Fusion is enabled");
	}
	else
	{
		ROS_INFO("Arbe real-time Fusion is disabled");
	}

	msg.flag = isEnableClassification;
    msg.header.stamp = ros::Time::now();
	enableClassification_pub.publish(msg);
}

void MainWindow::enableFreeSpace( void )
{
    arbe_msgs::arbeBoolWithTime msg;
    bool isEnableFreeSpace= EnableFreeSpaceAct->isChecked();
	if(isEnableFreeSpace != FSDispOnCam)
		toggle_FS_on_cam();

    if(isEnableFreeSpace)
    {
        ROS_INFO("Arbe real-time Free-Space is enabled");
    }
    else
    {
        ROS_INFO("Arbe real-time Free-Space is disabled");
    }

    msg.flag = isEnableFreeSpace;
    set_FreeSpace_enabled(isEnableFreeSpace);
    msg.header.stamp = ros::Time::now();
    enableFreeSpace_pub.publish(msg);
}

void MainWindow::resetSlam( void )
{
    arbe_msgs::arbeBoolWithTime msg;
	ROS_INFO("Arbe real-time AI Reset");
    msg.flag = true;
    msg.header.stamp = ros::Time::now();
	resetSLAM_pub.publish(msg);
}

void MainWindow::LoggingControl( void )
{
	if(loggingAct->isChecked())
	{
		pthread_t radar_logger_thread_id;
		int ret;
		/* spawn the radar logger thread */
		ret=pthread_create(&radar_logger_thread_id,NULL,&radar_logger_thread,NULL);
        if (ros::console::set_logger_level(ROSCONSOLE_DEFAULT_NAME, ros::console::levels::Error))//Debug))
		{
			ros::console::notifyLoggerLevelsChanged();
		}
		logging_enabled = true;
		logging_state = true;
	} else {
        if (ros::console::set_logger_level(ROSCONSOLE_DEFAULT_NAME, ros::console::levels::Error))//Info))
		{
			ros::console::notifyLoggerLevelsChanged();
		}
		logging_enabled = false;
		logging_state = false;
	}
}

void MainWindow::RecCheckAll_Control( void )
{
	rec_all_pp = true;
	rec_camera = true;
	rec_all_debug = true;
	rec_dl_training = true;
	rec_fusion = true;
	rec_parsed = true;
	rec_raw_topics = true;
	rec_settings_topics = true;
	rec_legacy_pc = true;
	rec_nav_topics = true;
	rec_object_list = true;

	recAllProcessedTopicsAct->setChecked(true);
	recCameraAct->setChecked(true);
	recDebugAct->setChecked(true);
	recDLTrainingAct->setChecked(true);
	recFusionAct->setChecked(true);
	recParsedAct->setChecked(true);
	recRawTopicsAct->setChecked(true);
	recSettingsTopicsAct->setChecked(true);
	recNavTopicsAct->setChecked(true);
	recLegacyPcAct->setChecked(true);
	recObjectListAct->setChecked(true);
	recObjectListAct->setEnabled(false);
}

void MainWindow::RecUncheckAll_Control( void )
{
	rec_all_pp = false;
	rec_camera = false;
	rec_all_debug = false;
	rec_dl_training = false;
	rec_fusion = false;
	rec_parsed = false;
	rec_raw_topics = false;
	rec_settings_topics = false;
	rec_legacy_pc = false;
	rec_nav_topics = false;
	rec_object_list = false;

	recAllProcessedTopicsAct->setChecked(false);
	recCameraAct->setChecked(false);
	recDebugAct->setChecked(false);
	recDLTrainingAct->setChecked(false);
	recFusionAct->setChecked(false);
	recParsedAct->setChecked(false);
	recRawTopicsAct->setChecked(false);
	recSettingsTopicsAct->setChecked(false);
	recNavTopicsAct->setChecked(false);
	recLegacyPcAct->setChecked(false);

	recObjectListAct->setChecked(false);
	recObjectListAct->setEnabled(true);

}


void MainWindow::RecAllPostProcessing_Control( void )
{
    if(recAllProcessedTopicsAct->isChecked())
    {
        rec_all_pp = true;
		recObjectListAct->setEnabled(false);
	}
	else
{
        rec_all_pp = false;
		recObjectListAct->setEnabled(true);
    }
}

void MainWindow::RecCamera_Control( void )
	{
    if(recCameraAct->isChecked())
    {
        rec_camera = true;
    } else
    {
        rec_camera = false;
    }

		}
void MainWindow::RecDebug_Control( void )
{
    if(recDebugAct->isChecked())
    {
        rec_all_debug = true;
    } else
    {
        rec_all_debug = false;
	}

}
void MainWindow::RecDLTraining_Control( void )
{
    if(recDLTrainingAct->isChecked())
    {
        rec_dl_training = true;
    } else
    {
        rec_dl_training = false;
    }

}
void MainWindow::RecFusion_Control( void )
{
    if(recFusionAct->isChecked())
    {
        rec_fusion = true;
    } else
    {
        rec_fusion = false;
    }

}
void MainWindow::RecParsed_Control( void )
{
    if(recParsedAct->isChecked())
    {
        rec_parsed = true;
    } else
    {
        rec_parsed = false;
    }
}
void MainWindow::RecNav_Control( void )
{
    if(recNavTopicsAct->isChecked())
    {
        rec_nav_topics = true;
    } else
    {
        rec_nav_topics = false;
    }
}
void MainWindow::RecObjList_Control( void )
{
	if(recObjectListAct->isChecked())
	{
		rec_object_list = true;
	} else
	{
		rec_object_list = false;
	}
}
void MainWindow::RecRawTopics_Control( void )
{
    if(recRawTopicsAct->isChecked())
    {
        rec_raw_topics = true;
    } else
    {
        rec_raw_topics = false;
    }
}
void MainWindow::RecSettingsTopics_Control( void )
{
    if(recSettingsTopicsAct->isChecked())
    {
        rec_settings_topics = true;
    } else
    {
        rec_settings_topics = false;
    }
}

void MainWindow::RecLegacyPointcloud_Control( void )
{
    if(recLegacyPcAct->isChecked())
    {
        rec_legacy_pc = true;
    } else
    {
        rec_legacy_pc = false;
    }
}


void MainWindow::RecRvizPointcloudControl( void )
{
	if(recRvizPointcloudAct->isChecked())
	{
		QMessageBox::StandardButton answer = QMessageBox::question(
			this,
			tr("Record Processed Pointcloud"),
			tr("This will record the processed PointCloud2 messages published to Rviz in addition to the binary pointcloud received from the Radar. Continue?"),
		QMessageBox::Yes | QMessageBox::No);

		if(answer == QMessageBox::Yes) {
			rec_rviz_pointcloud = true;
		}
	} else {
		rec_rviz_pointcloud = false;
	}
}

void MainWindow::ChamberModeControl( void )
{
	if(ChamberAct->isChecked())
	{
		QMessageBox::StandardButton answer = QMessageBox::question(
			this,
			tr("Enable Chamber mode"),
			tr("The system enters the An-Echoic chamber mode. Should not be used in operation mode. Continue?"),
		QMessageBox::Yes | QMessageBox::No);

		if(answer == QMessageBox::Yes) {
			/* Enable Chamber Mode */
			radar_set_chamber_mode( true );
			chamber_mode_state = true;
		} else {
			chamber_mode_state = false;
			ChamberAct->setChecked(false);
		}
	} else {
		radar_set_chamber_mode( false );
		chamber_mode_state = false;
	}
}

void MainWindow::read_radar_debug_data( void )
{
	/* use RAF_API to read debug data */
	//radar_read_debug_data();
}

void MainWindow::ConfigDialog()
{

	QDialog config_dialog(this);
	config_dialog.setWindowModality(Qt::WindowModal); 
	config_dialog.setWindowTitle(tr("System Settings"));
	config_dialog.setWindowFlags(Qt::Dialog | Qt::Desktop | Qt::WindowStaysOnTopHint);
	config_dialog.resize(300, 300);

	load_settings();
	QPushButton *closeButton = new QPushButton(tr("Save Settings"));
	connect( closeButton, SIGNAL( clicked()), this, SLOT( save_settings()));

	QHBoxLayout *buttonsLayout = new QHBoxLayout;
	buttonsLayout->addStretch(1);
	buttonsLayout->addWidget(closeButton);

	QVBoxLayout *mainLayout = new QVBoxLayout;

	QGroupBox *configGroup = new QGroupBox(tr("Radar configuration"));

	QLabel *debug_port_label = new QLabel(tr("Uart Debug Port:"));
	QLabel *video_device_label = new QLabel(tr("Camera Video Device:"));
	QLabel *region_label = new QLabel(tr("Selected Region:"));
	QLabel *bias4d_label = new QLabel(tr("4D Bias:"));
	QLabel *noise_level_label = new QLabel(tr("Noise Level:"));
	ntc_3d_checkbox = new QCheckBox(tr("NTC (Noise Threshold Control) 3D"));
	ntc_3d_checkbox->setChecked(ntc_3d_state.toBool());
	ntc_3d_enabled = ntc_3d_state.toBool();

	ntc_4d_checkbox = new QCheckBox(tr("NTC (Noise Threshold Control) 4D"));
	ntc_4d_checkbox->setChecked(ntc_4d_state.toBool());
	ntc_4d_enabled = ntc_3d_state.toBool();

	cfar_3d_checkbox = new QCheckBox(tr("Azimuth CFAR 3D"));
	cfar_3d_checkbox->setChecked(cfar_3d_state.toBool());
	cfar_3d_enabled = cfar_3d_state.toBool();

	cfar_4d_checkbox = new QCheckBox(tr("Azimuth CFAR 4D"));
	cfar_4d_checkbox->setChecked(cfar_4d_state.toBool());
	cfar_4d_enabled = cfar_4d_state.toBool();

	enable_phase_checkbox = new QCheckBox(tr("Output Phase Data in Pointcloud"));
	enable_phase_checkbox->setChecked(phase_state.toBool());
	phase_enabled = phase_state.toBool();

	enable_thr_metadata_checkbox = new QCheckBox(tr("Output Algo Threshold Metadata"));
	enable_thr_metadata_checkbox->setChecked(output_thr_metadata_state.toBool());
	algo_thr_metadata_enabled = output_thr_metadata_state.toBool();

	radar_region_combo = new QComboBox;
	radar_region_combo->addItem(tr("World"));
	radar_region_combo->addItem(tr("Japan"));
	bias_4d_edit = new QLineEdit();
	noise_level_edit = new QLineEdit();


	debug_port_combo = new QComboBox();
	find_serial_ports();
	video_device_combo = new QComboBox();
	find_video_devices();

	int index = radar_region_combo->findText(radar_region);
	radar_region_combo->setCurrentIndex(index);
	bias_4d_edit->setText(bias_4d);
	bias_4d_edit->setValidator( new QIntValidator(-50, 50, this) );
	noise_level_edit->setText(noise_level);
	noise_level_edit->setValidator( new QIntValidator(0, 100, this) );

	index = debug_port_combo->findText(radar_debug_port);
	debug_port_combo->setCurrentIndex(index);
	index = video_device_combo->findText(video_device);
	video_device_combo->setCurrentIndex(index);

	connect( radar_region_combo, SIGNAL( currentTextChanged(QString)), this, SLOT( updateSettingsStrings( void )));
	connect( bias_4d_edit, SIGNAL( textChanged(QString)), this, SLOT( updateSettingsStrings( void )));
	connect( ntc_3d_checkbox, SIGNAL( clicked()), this, SLOT( updateSettingsStrings( void )));
	connect( ntc_4d_checkbox, SIGNAL( clicked()), this, SLOT( updateSettingsStrings( void )));
	connect( cfar_3d_checkbox, SIGNAL( clicked()), this, SLOT( updateSettingsStrings( void )));
	connect( cfar_4d_checkbox, SIGNAL( clicked()), this, SLOT( updateSettingsStrings( void )));
	connect( enable_phase_checkbox, SIGNAL( clicked()), this, SLOT( updateSettingsStrings( void )));
	connect( enable_thr_metadata_checkbox, SIGNAL( clicked()), this, SLOT( updateSettingsStrings( void )));

	connect( debug_port_combo, SIGNAL( currentTextChanged(QString)), this, SLOT( updateSettingsStrings( void )));
	connect( video_device_combo, SIGNAL( currentTextChanged(QString)), this, SLOT( updateSettingsStrings( void )));
	connect( noise_level_edit, SIGNAL( textChanged(QString)), this, SLOT( updateSettingsStrings( void )));

	QVBoxLayout *serverLayout = new QVBoxLayout;
	serverLayout->addWidget(debug_port_label);
	serverLayout->addWidget(debug_port_combo);
	serverLayout->addWidget(video_device_label);
	serverLayout->addWidget(video_device_combo);
	serverLayout->addWidget(region_label);
	serverLayout->addWidget(radar_region_combo);
	serverLayout->addWidget(bias4d_label);
	serverLayout->addWidget(bias_4d_edit);
	serverLayout->addWidget(noise_level_label);
	serverLayout->addWidget(noise_level_edit);
	serverLayout->addWidget(ntc_3d_checkbox);
	serverLayout->addWidget(ntc_4d_checkbox);
	serverLayout->addWidget(cfar_3d_checkbox);
	serverLayout->addWidget(cfar_4d_checkbox);
	serverLayout->addWidget(enable_phase_checkbox);
	serverLayout->addWidget(enable_thr_metadata_checkbox);

	QVBoxLayout *configLayout = new QVBoxLayout;
	configLayout->addLayout(serverLayout);
	configLayout->addLayout(buttonsLayout);
	configGroup->setLayout(configLayout);

	mainLayout->addWidget(configGroup);

	config_dialog.setLayout(mainLayout);
	config_dialog.exec();
}

void MainWindow::updateSettingsStrings( void )
{

}

void MainWindow::load_settings()
{
}

void MainWindow::save_settings(void )
{
}

void MainWindow::about()
{
	QMessageBox::about(this, tr("Program Info"),
	tr("<b>Autox</b> LIDAR control GUI Demo Version 0.1"));
}

void MainWindow::quit( void )
{
    writeSettings();
	int ret = system("pkill -f rqt_bag");
	//radar_quit();
}

void MainWindow::createActions()
{
	configAct = new QAction(tr("&Common Settings"), this);
	configAct->setStatusTip(tr("us Common Settings"));
	connect(configAct, SIGNAL(triggered()), this, SLOT(ConfigDialog()));

	controlAct = new QAction(tr("&Control Settings"), this);
	controlAct->setStatusTip(tr("us Control Settings"));
	connect(controlAct, SIGNAL(triggered()), this, SLOT(ConfigDialog()));

	debugAct = new QAction(tr("&Debug Settings"), this);
	debugAct->setStatusTip(tr("us Debug Settings"));
	connect(debugAct, SIGNAL(triggered()), this, SLOT(ConfigDialog()));
#if 1
	openAct = new QAction(tr("&Open Recording"), this);
	openAct->setStatusTip(tr("Play a recording"));
	connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

	setRecFolderAct = new QAction(tr("&Set Recording Folder"), this);
	setRecFolderAct->setStatusTip(tr("Set default path for recording"));
	connect(setRecFolderAct, SIGNAL(triggered()), this, SLOT(setSaveFolder()));

#if 0
	loadCamCalibAct = new QAction(tr("Load &camera calibration"), this);
	loadCamCalibAct->setStatusTip(tr("Load camera calibration from file"));
	connect(loadCamCalibAct, SIGNAL(triggered()), this, SLOT(load_camera_calibration()));
#endif
	closeAct = new QAction(tr("&Close Recording"), this);
	closeAct->setStatusTip(tr("Stop Playback"));
	connect(closeAct, SIGNAL(triggered()), this, SLOT(stopPlayback()));

	convertAct = new QAction(tr("Convert Rosba&g"), this);
	convertAct->setStatusTip(tr("Convert a rosbag recording to binary and mp4"));
	connect(convertAct, SIGNAL(triggered()), this, SLOT(convertRosbag()));

	TestDescriptionAct = new QAction(tr("Set Test Descri&ption"), this);
	TestDescriptionAct->setStatusTip(tr("Add a test drive description"));
	connect(TestDescriptionAct, SIGNAL(triggered()), this, SLOT(SetTestDescription()));

#endif
	exitAct = new QAction(tr("E&xit"), this);
	exitAct->setStatusTip(tr("Exit the application"));
	connect(exitAct, SIGNAL(triggered()), this, SLOT(quit()));

	aboutAct = new QAction(tr("&About this app"), this);
	aboutAct->setStatusTip(tr("Show the application's About box"));
	connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

	updateAct = new QAction(tr("&System update"), this);


	GigEcameraAct = new QAction(tr("Launch GigE Camera"), this);
	GigEcameraAct->setStatusTip(tr("Launch GigE Camera"));
	GigEcameraAct->setCheckable(true);
	GigEcameraAct->setChecked(true);
	connect(GigEcameraAct, SIGNAL(triggered()), this, SLOT(launchGigECam()));

	USBcameraAct = new QAction(tr("Launch USB Camera"), this);
	USBcameraAct->setStatusTip(tr("Launch USB Camera"));
	USBcameraAct->setCheckable(true);
	USBcameraAct->setChecked(false);
	connect(USBcameraAct, SIGNAL(triggered()), this, SLOT(launchUSBCam()));

	cameraAct = new QAction(tr("Start Ca&mera"), this);
	cameraAct->setStatusTip(tr("Enable camera display"));
	connect(cameraAct, SIGNAL(triggered()), this, SLOT(enableCamera()));

	freeSpaceTopVeiwAct = new QAction(tr("Start &Free Space View"), this);
	freeSpaceTopVeiwAct->setStatusTip(tr("Enable free space top view display"));
	connect(freeSpaceTopVeiwAct, SIGNAL(triggered()), this, SLOT(enableFreeSpaceTopView()));

	resetAct = new QAction(tr("System Rese&t"), this);
	resetAct->setStatusTip(tr("Trigger radar restart"));
	connect(resetAct, SIGNAL(triggered()), this, SLOT(radarReset()));

	loggingAct = new QAction(tr("Enable Radar &Logging"), this);
	loggingAct->setStatusTip(tr("Display the logging messages received from the radar terminal port"));
	loggingAct->setCheckable(true);
	loggingAct->setChecked(false);
	connect(loggingAct, SIGNAL(triggered()), this, SLOT(LoggingControl()));

	recCheckAllAct = new QAction(tr("&Check all suggested topics"), this);
	recCheckAllAct->setStatusTip(tr("Check all available recordingTopics"));
	recCheckAllAct->setCheckable(false);
	connect(recCheckAllAct, SIGNAL(triggered()), this, SLOT(RecCheckAll_Control()));

	recUncheckAllAct = new QAction(tr("&Uncheck all recording topics"), this);
	recUncheckAllAct->setStatusTip(tr("Uncheck all recording topics"));
	recUncheckAllAct->setCheckable(false);
	connect(recUncheckAllAct, SIGNAL(triggered()), this, SLOT(RecUncheckAll_Control()));


	recAllProcessedTopicsAct = new QAction(tr("Record all &processed topics"), this);
	recAllProcessedTopicsAct->setStatusTip(tr("When recording, record *all* post processing topics"));
	recAllProcessedTopicsAct->setCheckable(true);
	recAllProcessedTopicsAct->setChecked(rec_all_pp);
	connect(recAllProcessedTopicsAct, SIGNAL(triggered()), this, SLOT(RecAllPostProcessing_Control()));

	recCameraAct = new QAction(tr("Record &camera"), this);
	recCameraAct->setStatusTip(tr("When recording, record camera compressed image"));
	recCameraAct->setCheckable(true);
	recCameraAct->setChecked(rec_camera);
	connect(recCameraAct, SIGNAL(triggered()), this, SLOT(RecCamera_Control()));

	recDebugAct = new QAction(tr("Record all &debug topics"), this);
	recDebugAct->setStatusTip(tr("When recording, record *all* debugging topics"));
	recDebugAct->setCheckable(true);
	recDebugAct->setChecked(rec_all_debug);
	connect(recDebugAct, SIGNAL(triggered()), this, SLOT(RecDebug_Control()));

	recDLTrainingAct = new QAction(tr("Record all D&L topics"), this);
	recDLTrainingAct->setStatusTip(tr("When recording, record *all* deep learning training topics"));
	recDLTrainingAct->setCheckable(true);
	recDLTrainingAct->setChecked(rec_dl_training);
	connect(recDLTrainingAct, SIGNAL(triggered()), this, SLOT(RecDLTraining_Control()));

	recFusionAct = new QAction(tr("Record all &fusion topics"), this);
	recFusionAct->setStatusTip(tr("When recording, record *all* fusion topics"));
	recFusionAct->setCheckable(true);
	recFusionAct->setChecked(rec_fusion);
	connect(recFusionAct, SIGNAL(triggered()), this, SLOT(RecFusion_Control()));

	recParsedAct = new QAction(tr("Record all &parsed topics"), this);
	recParsedAct->setStatusTip(tr("When recording, record *all* parsed topics"));
	recParsedAct->setCheckable(true);
	recParsedAct->setChecked(rec_parsed);
	connect(recParsedAct, SIGNAL(triggered()), this, SLOT(RecParsed_Control()));

	recNavTopicsAct = new QAction(tr("Record all &nav topics"), this);
	recNavTopicsAct->setStatusTip(tr("When recording, record *all* nav topics (GPS and IMU)"));
	recNavTopicsAct->setCheckable(true);
	recNavTopicsAct->setChecked(rec_nav_topics);
	connect(recNavTopicsAct, SIGNAL(triggered()), this, SLOT(RecNav_Control()));

	recObjectListAct = new QAction(tr("Record &object list topic"), this);
	recObjectListAct->setStatusTip(tr("When recording, record object list even if all post processing is off"));
	recObjectListAct->setCheckable(true);
	recObjectListAct->setChecked(rec_object_list);
	connect(recObjectListAct, SIGNAL(triggered()), this, SLOT(RecObjList_Control()));

	recRawTopicsAct = new QAction(tr("Record all &raw topics"), this);
	recRawTopicsAct->setStatusTip(tr("When recording, record all raw topics *except for legacy PC*"));
	recRawTopicsAct->setCheckable(true);
	recRawTopicsAct->setChecked(rec_raw_topics);
	connect(recRawTopicsAct, SIGNAL(triggered()), this, SLOT(RecRawTopics_Control()));

	recSettingsTopicsAct = new QAction(tr("Record all &settings topics"), this);
	recSettingsTopicsAct->setStatusTip(tr("When recording, record *all* settings topics"));
	recSettingsTopicsAct->setCheckable(true);
	recSettingsTopicsAct->setChecked(rec_settings_topics);
	connect(recSettingsTopicsAct, SIGNAL(triggered()), this, SLOT(RecSettingsTopics_Control()));

	recLegacyPcAct = new QAction(tr("Record le&gacy pointcloud format"), this);
	recLegacyPcAct->setStatusTip(tr("When recording, record pointcloud in legacy format"));
	recLegacyPcAct->setCheckable(true);
	recLegacyPcAct->setChecked(rec_legacy_pc);
	connect(recLegacyPcAct, SIGNAL(triggered()), this, SLOT(RecLegacyPointcloud_Control()));

	recRvizPointcloudAct = new QAction(tr("Record R&viz PointCloud2 messages"), this);
	recRvizPointcloudAct->setStatusTip(tr("When recording, also the processed PointCloud2 messages"));
	recRvizPointcloudAct->setCheckable(true);
	recRvizPointcloudAct->setChecked(rec_rviz_pointcloud);
	connect(recRvizPointcloudAct, SIGNAL(triggered()), this, SLOT(RecRvizPointcloudControl()));

	DetectionsTypeAct = new QAction(tr("&Post Process Detections"), this);
	DetectionsTypeAct->setStatusTip(tr("Apply post processing to the radar detections"));
	DetectionsTypeAct->setCheckable(true);
	DetectionsTypeAct->setChecked(DEFAULT_SLAM_ENABLED);
	connect(DetectionsTypeAct, SIGNAL(triggered()), this, SLOT(selectDetectionsType()));

	ShowSlamAct = new QAction(tr("&Display Slam Objects"), this);
	ShowSlamAct->setStatusTip(tr("Trigger radar restart"));
	ShowSlamAct->setCheckable(true);
	ShowSlamAct->setChecked(DEFAULT_SLAM_ENABLED);
	connect(ShowSlamAct, SIGNAL(triggered()), this, SLOT(selectSlam()));

	EnableAggregationAct = new QAction(tr("&Enable PC aggregation"), this);
	EnableAggregationAct->setStatusTip(tr("Toggle point cloud aggregation enable/disable"));
	EnableAggregationAct->setDisabled(true);
	EnableAggregationAct->setCheckable(true);
	EnableAggregationAct->setChecked(false);
	connect(EnableAggregationAct, SIGNAL(triggered()), this, SLOT(EnableAggregation()));

	DLTrainingViewAct = new QAction(tr("&DL training view"), this);
	DLTrainingViewAct->setStatusTip(tr("Switch to DL training view"));
	DLTrainingViewAct->setCheckable(true);
	DLTrainingViewAct->setChecked(false);
	connect(DLTrainingViewAct, SIGNAL(triggered()), this, SLOT(setDLTrainingView()));

	DLTrainingPlaybackAct = new QAction(tr("DL training &playback"), this);
	DLTrainingPlaybackAct->setStatusTip(tr("DL training view in playback mode"));
	DLTrainingPlaybackAct->setCheckable(true);
	DLTrainingPlaybackAct->setChecked(false);
	connect(DLTrainingPlaybackAct, SIGNAL(triggered()), this, SLOT(setDLTrainingPlayback()));

	DLTrainingChooseAllClassesAct = new QAction(tr("Show &all target classes"), this);
	DLTrainingChooseAllClassesAct->setStatusTip(tr("Show all target classes"));
	DLTrainingChooseAllClassesAct->setCheckable(true);
	DLTrainingChooseAllClassesAct->setChecked(true);
	DLTrainingChooseAllClassesAct->setEnabled(false);
	connect(DLTrainingChooseAllClassesAct, SIGNAL(triggered()), this, SLOT(setClassesToShow()));

	DLTrainingChooseClass0Act = new QAction(tr("Show only target class &0"), this);
	DLTrainingChooseClass0Act->setStatusTip(tr("Show only target class 0"));
	DLTrainingChooseClass0Act->setCheckable(true);
	DLTrainingChooseClass0Act->setChecked(false);
	DLTrainingChooseClass0Act->setEnabled(false);
	connect(DLTrainingChooseClass0Act, SIGNAL(triggered()), this, SLOT(setClassesToShow()));

	DLTrainingChooseClass1Act = new QAction(tr("Show only target class &1"), this);
	DLTrainingChooseClass1Act->setStatusTip(tr("Show only target class 1"));
	DLTrainingChooseClass1Act->setCheckable(true);
	DLTrainingChooseClass1Act->setChecked(false);
	DLTrainingChooseClass1Act->setEnabled(false);
	connect(DLTrainingChooseClass1Act, SIGNAL(triggered()), this, SLOT(setClassesToShow()));

	DLTrainingChooseClass2Act = new QAction(tr("Show only target class &2"), this);
	DLTrainingChooseClass2Act->setStatusTip(tr("Show only target class 2"));
	DLTrainingChooseClass2Act->setCheckable(true);
	DLTrainingChooseClass2Act->setChecked(false);
	DLTrainingChooseClass2Act->setEnabled(false);
	connect(DLTrainingChooseClass2Act, SIGNAL(triggered()), this, SLOT(setClassesToShow()));

	DLTrainingChooseClass3Act = new QAction(tr("Show only target class &3"), this);
	DLTrainingChooseClass3Act->setStatusTip(tr("Show only target class 3"));
	DLTrainingChooseClass3Act->setCheckable(true);
	DLTrainingChooseClass3Act->setChecked(false);
	DLTrainingChooseClass3Act->setEnabled(false);
	connect(DLTrainingChooseClass3Act, SIGNAL(triggered()), this, SLOT(setClassesToShow()));

	DLTrainingChooseClass4Act = new QAction(tr("Show only target class &4"), this);
	DLTrainingChooseClass4Act->setStatusTip(tr("Show only target class 4"));
	DLTrainingChooseClass4Act->setCheckable(true);
	DLTrainingChooseClass4Act->setChecked(false);
	DLTrainingChooseClass4Act->setEnabled(false);
	connect(DLTrainingChooseClass4Act, SIGNAL(triggered()), this, SLOT(setClassesToShow()));

	EnableGeogAct = new QAction(tr("Enable &Geographic localization"), this);
	EnableGeogAct->setStatusTip(tr("Toggle geograpic localization enable/disable"));
	EnableGeogAct->setCheckable(true);
	EnableGeogAct->setChecked(false);
	connect(EnableGeogAct, SIGNAL(triggered()), this, SLOT(EnableGeoLocalization()));

	ResetLocalCartesianAct = new QAction(tr("&Reset local cartesian frame"), this);
	ResetLocalCartesianAct->setStatusTip(tr("Reset local cartesian frame and reset aggregation"));
	ResetLocalCartesianAct->setCheckable(false);
	connect(ResetLocalCartesianAct, SIGNAL(triggered()), this, SLOT(ResetLocalCartesian()));

	EnableSlamAct = new QAction(tr("&Enable real-time SLAM"), this);
	EnableSlamAct->setStatusTip(tr("Toggle slam enable/disable"));
	EnableSlamAct->setCheckable(true);
	EnableSlamAct->setChecked(DEFAULT_SLAM_ENABLED);
	connect(EnableSlamAct, SIGNAL(triggered()), this, SLOT(enableSlam()));

	EnableClassificationAct = new QAction(tr("Enable real-time &Classification"), this);
	EnableClassificationAct->setStatusTip(tr("Toggle classification enable/disable"));
	EnableClassificationAct->setCheckable(true);
	EnableClassificationAct->setChecked(DEFAULT_CLASSIFICATION_ENABLED);
	connect(EnableClassificationAct, SIGNAL(triggered()), this, SLOT(enableClassification()));

	EnableFreeSpaceAct = new QAction(tr("Enable real-time Free-&Space Mapping"), this);
	EnableFreeSpaceAct->setStatusTip(tr("Toggle free-space enable/disable"));
	EnableFreeSpaceAct->setCheckable(true);
	EnableFreeSpaceAct->setChecked(DEFAULT_FREESPACE_ENABLED);
	connect(EnableFreeSpaceAct, SIGNAL(triggered()), this, SLOT(enableFreeSpace()));

	ResetOmegaCalibAct = new QAction(tr("Reset &omega calibration"), this);
	ResetOmegaCalibAct->setStatusTip(tr("Reset installation error calibration"));
	connect(ResetOmegaCalibAct, SIGNAL(triggered()), this, SLOT(resetOmegaCalib()));

	ImposeOmegaZeroAct = new QAction(tr("Impose &zero omega"), this);
	ImposeOmegaZeroAct->setStatusTip(tr("Impose 0 omega. May violate max allowed installation error"));
	connect(ImposeOmegaZeroAct, SIGNAL(triggered()), this, SLOT(imposeOmegaZero()));

	restoreDefaultsAct = new QAction(tr("Restore defaults"), this);
	restoreDefaultsAct->setStatusTip(tr("Restore defaults"));
	connect(restoreDefaultsAct, SIGNAL(triggered()), this, SLOT(restoreDefaults()));

	dispFSPolyOnPcAct = new QAction(tr("Display free space polygon on top of pointcloud"), this);
	dispFSPolyOnPcAct->setStatusTip(tr("Display free space polygon on top of pointcloud"));
	dispFSPolyOnPcAct->setCheckable(true);
	dispFSPolyOnPcAct->setChecked(DEFAULT_DISP_FS_ON_PC);
	connect(dispFSPolyOnPcAct, SIGNAL(triggered()), this, SLOT(dispFSOnPc()));

	ResetSlamAct = new QAction(tr("&Reset real-time SLAM"), this);
	ResetSlamAct->setStatusTip(tr("Trigger radar restart"));
	connect(ResetSlamAct, SIGNAL(triggered()), this, SLOT(resetSlam()));

	slamReconfigureAct = new QAction(tr("Slam &Reconfigure"), this);
	slamReconfigureAct->setStatusTip(tr("Slam Params reconfigure"));
	connect(slamReconfigureAct, SIGNAL(triggered()), this, SLOT(slam_reconfigure()));

	ChamberAct = new QAction(tr("Enable &Chamber Mode"), this);
	ChamberAct->setStatusTip(tr("Set the radar into Anechoic chamber mode"));
	ChamberAct->setCheckable(true);
	ChamberAct->setChecked(false);
	connect(ChamberAct, SIGNAL(triggered()), this, SLOT(ChamberModeControl()));

	allowCameraBufferingForSyncAct = new QAction(tr("Allow camera &buffering for sync"), this);
	allowCameraBufferingForSyncAct->setStatusTip(tr("Allow camera buffering for radar/camera synchronization, useful especially offline"));
	allowCameraBufferingForSyncAct->setCheckable(true);
	allowCameraBufferingForSyncAct->setChecked(DEFAULT_CAMERA_BUFFERIBG);
	connect(allowCameraBufferingForSyncAct, SIGNAL(triggered()), this, SLOT(toggle_camera_buffering()));

	toggleSlamOnImageAct = new QAction(tr("Display slam on &camera"), this);
	toggleSlamOnImageAct->setStatusTip(tr("Display slam objects on camera"));
	toggleSlamOnImageAct->setCheckable(true);
	toggleSlamOnImageAct->setChecked(DEFAULT_SLAM_ENABLED);
	//Or use instead:
	//toggleSlamOnImageAct->setChecked(slamDispOnCam);
	connect(toggleSlamOnImageAct, SIGNAL(triggered()), this, SLOT(toggle_slam_on_cam()));

	toggleFSOnImageAct = new QAction(tr("Display &Free Space on camera"), this);
	toggleFSOnImageAct->setStatusTip(tr("Display Free Space on camera"));
	toggleFSOnImageAct->setCheckable(true);
	toggleFSOnImageAct->setChecked(FSDispOnCam);
	connect(toggleFSOnImageAct, SIGNAL(triggered()), this, SLOT(toggle_FS_on_cam()));

	toggleFunnelOnImageAct = new QAction(tr("Display funne&l on camera"), this);
	toggleFunnelOnImageAct->setStatusTip(tr("Display funnel on camera"));
	toggleFunnelOnImageAct->setCheckable(true);
	toggleFunnelOnImageAct->setChecked(true);
	connect(toggleFunnelOnImageAct, SIGNAL(triggered()), this, SLOT(toggle_funnel_on_cam()));

	toggleDiscardOutOfContextAct = new QAction(tr("Discard out-of-elevation detections"), this);
	toggleDiscardOutOfContextAct->setStatusTip(tr("Don't display pointcloud that lies outside the elevation colorcoding, otherwise saturate"));
	toggleDiscardOutOfContextAct->setCheckable(true);
	toggleDiscardOutOfContextAct->setChecked(discardOutOfElContext);
	connect(toggleDiscardOutOfContextAct, SIGNAL(triggered()), this, SLOT(toggle_discard_out_of_el_context()));

	toggleAsphaltTextSlidersAct = new QAction(tr("Asphalt Roughness over Floating Text"), this);
	toggleAsphaltTextSlidersAct->setStatusTip(tr("Toggle between Asphalt Roughness control and floating text angle control"));
	toggleAsphaltTextSlidersAct->setCheckable(true);
	toggleAsphaltTextSlidersAct->setChecked(true);
	connect(toggleAsphaltTextSlidersAct, SIGNAL(triggered()), this, SLOT(toggle_asphalt_text()));

	toggleDispDistOnImageAct = new QAction(tr("Display dis&tance to objects (else ID) on image"), this);
	toggleDispDistOnImageAct->setStatusTip(tr("Display distance to objects (else ID) on image"));
	toggleDispDistOnImageAct->setCheckable(true);
	toggleDispDistOnImageAct->setChecked(DEFAULT_DIST_ON_CAM);
	connect(toggleDispDistOnImageAct, SIGNAL(triggered()), this, SLOT(toggle_dist_on_cam()));


	toggleSlamDisplayEconomyModeAct = new QAction(tr("Slam on camera in Eco &mode"), this);
	toggleSlamDisplayEconomyModeAct->setStatusTip(tr("Economy mode -- one color, no id as text"));
	toggleSlamDisplayEconomyModeAct->setCheckable(true);
	toggleSlamDisplayEconomyModeAct->setChecked(slamDispOnCamEcoMode);
	connect(toggleSlamDisplayEconomyModeAct, SIGNAL(triggered()), this, SLOT(toggle_slam_display_economy()));

	toggleSlamColorByClassAct = new QAction(tr("&Color objects by class"), this);
	toggleSlamColorByClassAct->setStatusTip(tr("Color is set according to class label"));
	toggleSlamColorByClassAct->setCheckable(true);
	toggleSlamColorByClassAct->setChecked(slamDispOnCamEcoMode);
	connect(toggleSlamColorByClassAct, SIGNAL(triggered()), this, SLOT(toggle_slam_color_by_class()));

	captureRadarDebugDataAct = new QAction(tr("Capture radar debug data"), this);
	captureRadarDebugDataAct->setStatusTip(tr("Read the debug data from the radar"));
	connect(captureRadarDebugDataAct, SIGNAL(triggered()), this, SLOT(read_radar_debug_data()));
}

void MainWindow::createMenus()
{
	fileMenu = menuBar()->addMenu(tr("&File"));
	fileMenu->addAction(openAct);
	fileMenu->addAction(closeAct);
	fileMenu->addAction(setRecFolderAct);
	fileMenu->addSeparator();
	fileMenu->addAction(convertAct);
	fileMenu->addAction(TestDescriptionAct);
	fileMenu->addSeparator();
	fileMenu->addAction(exitAct);
	menuBar()->addSeparator();

#if  0
	aiMenu = menuBar()->addMenu(tr("&AI (beta)"));
	aiMenu->addAction(EnableSlamAct);
	aiMenu->addAction(EnableClassificationAct);
	aiMenu->addAction(EnableFreeSpaceAct);
	aiMenu->addSeparator();
	aiMenu->addAction(ResetSlamAct);
	aiMenu->addAction(ResetOmegaCalibAct);
	aiMenu->addAction(ImposeOmegaZeroAct);
	aiMenu->addAction(restoreDefaultsAct);
	aiMenu->addSeparator();
	aiMenu->addAction(EnableGeogAct);
	aiMenu->addAction(EnableAggregationAct);
	aiMenu->addAction(ResetLocalCartesianAct);
	aiMenu->addSeparator();
	aiMenu->addAction(slamReconfigureAct);
	menuBar()->addSeparator();
	viewMenu = menuBar()->addMenu(tr("&View"));
	viewMenu->addAction(ShowSlamAct);
	viewMenu->addAction(DetectionsTypeAct);
	viewMenu->addAction(dispFSPolyOnPcAct);
	viewMenu->addAction(freeSpaceTopVeiwAct);
    viewMenu->addSeparator();
	viewMenu->addAction(allowCameraBufferingForSyncAct);
    viewMenu->addAction(toggleSlamOnImageAct);
    viewMenu->addAction(toggleFSOnImageAct);
	viewMenu->addAction(toggleSlamDisplayEconomyModeAct);
    viewMenu->addAction(toggleSlamColorByClassAct);
    viewMenu->addAction(toggleDispDistOnImageAct);
	viewMenu->addAction(toggleFunnelOnImageAct);
	viewMenu->addSeparator();
	viewMenu->addAction(toggleDiscardOutOfContextAct);
	viewMenu->addAction(toggleAsphaltTextSlidersAct);
	viewMenu->addSeparator();
	viewMenu->addAction(DLTrainingViewAct);
    viewMenu->addAction(DLTrainingChooseAllClassesAct);
    viewMenu->addAction(DLTrainingChooseClass0Act);
    viewMenu->addAction(DLTrainingChooseClass1Act);
    viewMenu->addAction(DLTrainingChooseClass2Act);
    viewMenu->addAction(DLTrainingChooseClass3Act);
    viewMenu->addAction(DLTrainingChooseClass4Act);
	viewMenu->addSeparator();
	viewMenu->addAction(DLTrainingPlaybackAct);

	menuBar()->addSeparator();
	cameraMenu = menuBar()->addMenu(tr("&Camera"));
	cameraMenu->addAction(cameraAct);
	cameraMenu->addAction(loadCamCalibAct);
	cameraMenu->addAction(GigEcameraAct);
	cameraMenu->addAction(USBcameraAct);
#endif

	menuBar()->addSeparator();
	systemMenu = menuBar()->addMenu(tr("&Settings"));
#if 0
	systemMenu->addAction(resetAct);
	systemMenu->addSeparator();
	systemMenu->addAction(loggingAct);
	systemMenu->addAction(ChamberAct);
	systemMenu->addSeparator();
#endif
#if 1
	systemMenu->addAction(configAct);
	systemMenu->addSeparator();
	systemMenu->addAction(controlAct);
	systemMenu->addSeparator();
	systemMenu->addAction(debugAct);
#endif
	//systemMenu->addAction(captureRadarDebugDataAct);
	menuBar()->addSeparator();
#if 0
    recordingMenu = menuBar()->addMenu(tr("&Recording"));
	recordingMenu->addAction(recCheckAllAct);
	recordingMenu->addAction(recUncheckAllAct);
	recordingMenu->addSeparator();
    recordingMenu->addAction(recParsedAct);
	recordingMenu->addAction(recCameraAct);
	recordingMenu->addAction(recObjectListAct);
	recordingMenu->addAction(recSettingsTopicsAct);
    recordingMenu->addAction(recRawTopicsAct);
    recordingMenu->addAction(recAllProcessedTopicsAct);
    recordingMenu->addAction(recDebugAct);
    recordingMenu->addAction(recDLTrainingAct);
    recordingMenu->addAction(recFusionAct);
    recordingMenu->addAction(recNavTopicsAct);
    recordingMenu->addSeparator();
    recordingMenu->addAction(recLegacyPcAct);
	recordingMenu->addSeparator();
	recordingMenu->addAction(recRvizPointcloudAct);
	menuBar()->addSeparator();
#endif
	helpMenu = menuBar()->addMenu(tr("&Update"));
	helpMenu->addAction(aboutAct);
	helpMenu->addAction(updateAct);
}

void MainWindow::readSettings()
{
	QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
	const QByteArray geometry = settings.value("geometry", QByteArray()).toByteArray();
	if (geometry.isEmpty()) {
		const QRect availableGeometry = QApplication::desktop()->availableGeometry(this);
		resize(availableGeometry.width() / 3, availableGeometry.height() / 2);
		move((availableGeometry.width() - width()) / 2,
		     (availableGeometry.height() - height()) / 2);
	} else {
		restoreGeometry(geometry);
	}
	load_settings();	
}

void MainWindow::writeSettings()
{
	QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
	settings.setValue("geometry",saveGeometry());
    save_settings();
}


void MainWindow::setCurrentFile(const QString &fileName)
{
	curFile = fileName;
	textEdit->document()->setModified(false);
	setWindowModified(false);

	QString shownName = curFile;
	if (curFile.isEmpty())
		shownName = "No playback file selected";
	setWindowFilePath(shownName);
}

void MainWindow::SetTestDescription( void )
{
	QString text = TestDescriptionString;
	QInputDialog *TestDescriptionDialog = new QInputDialog(this);
	TestDescriptionDialog->setLabelText(tr("Test Description:"));
	TestDescriptionDialog->setWindowTitle(tr("Enter Test Drive Description"));
	TestDescriptionDialog->setTextValue(text);
	TestDescriptionDialog->resize(400,100);
	TestDescriptionDialog->move(QApplication::desktop()->screen()->rect().center() - TestDescriptionDialog->rect().center());
	if(TestDescriptionDialog->exec() == QDialog::Accepted)
		TestDescriptionString = TestDescriptionDialog->textValue();
}

