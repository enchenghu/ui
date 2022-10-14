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

#ifndef viewpanel_H
#define viewpanel_H

#include <sys/socket.h>
#include <sys/ioctl.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <iostream>
#include "vx_task.h"
#include "vx_queue.h"
#include "vx_mutex.h"

#include <QApplication>
#include <QWidget>
#include <QSlider>
#include <QComboBox>
#include <QColor>
#include <QSlider>
#include <QStyleOptionSlider>
#include <QToolTip>
#include <QLabel>
#include <QTimer>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QDockWidget>
#include <QTreeWidget>
#include <QMainWindow>
#include <QPushButton>
#include <QCheckBox>
#include <QMessageBox>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QDebug>
#include <QGroupBox>
#include "ctkrangeslider.h"
#include <QShortcut>

#include <std_msgs/UInt8.h>
#include <std_msgs/Bool.h>
#include <std_msgs/Float32.h>
#include <std_msgs/String.h>
#include "rviz/visualization_manager.h"
#include "rviz/render_panel.h"
#include "rviz/selection_panel.h"
#include "rviz/tool_manager.h"

#include "rviz/display.h"
#include "rviz/panel.h"
#include "rviz/view_manager.h"
#include <tf2_geometry_msgs/tf2_geometry_msgs.h>
#include <arbe_msgs/arbeGUIsettings.h>
#include <arbe_msgs/arbeNewPcMsg.h>
#include "qcustomplot.h"

#include "osc_chart.h"
#include "fft_charts.h"
#include <fstream>
#include <sstream>
#include <pc_data.h>
#include "bst_msg_queue.h"
#define MAX_RADARS 10
#define TCP_PC_SIZE_SINGLE 32000
#define UDP_PC_SIZE_SINGLE 1024
#define BUFF_LEN 1024
#define MAX_BUFF_LEN 8

#define TCP_TIMES_PER_FRAME 200
#define UDP_TIMES_PER_FRAME 256
#define BST_MAX_TASK_NUM		(16)


namespace rviz
{
	class Display;
	class RenderPanel;
	class VisualizationManager;
}

typedef enum
{
    VT_TOP,
    VT_TOP_CAR,
    VT_CAR,
    VT_SIDE,
    VT_STANDOFF,
    VT_120M,
    VT_DIAG,
    VT_BOUTIQUE_1,
    VT_BOUTIQUE_2,
    VT_BOUTIQUE_3
}eViewType;

typedef enum
{
    POWER_WRITE = 1, 
    CFAR_WRITE,
    DFT3_WRITE,
    DIFF_WRITE,
	REG_WRITE,
    POWER_READ,
    CFAR_READ,
    DFT3_READ,
    DIFF_READ,
	REG_READ,
	PC_READ,
	FFT_ADC_READ_START,
	FFT_ADC_READ_STOP
}commandType;

typedef struct API_Header
{
	uint16_t 	usPrefix; // 0xeeff
	uint16_t 	usType; // 0x10 version 1.0
	uint16_t 	usCommand; // command enum
	uint16_t 	usPayloadCrc;
	uint32_t 	unLength;
}API_Header;

typedef struct 
{
	uint16_t 	usPrefix; // 0xeeff
	uint16_t 	usType; // 0x10 version 1.0
	uint16_t 	usRollingCounter; 
	uint16_t 	usPayloadCrc;
	uint32_t 	usFrameCounter;
	uint32_t 	unLength;
}UDP_Header;

typedef struct 
{
	API_Header 	mHead; 
	uint32_t 	mCommandVal[2];
} commandMsg;


typedef struct 
{
	UDP_Header 	mHead; 
	uint8_t 	pcUdpData[UDP_PC_SIZE_SINGLE];
} udpMsg;

typedef struct 
{
	QVector<double> dataFFT_0;
	QVector<double> dataFFT_1;
}fftMsg;

typedef struct 
{
	commandMsg 	cmdmsg; 
	uint8_t 	pcTcpData[TCP_PC_SIZE_SINGLE];
} pcData_t;

typedef struct view_vals_t {
        QVariant distance;
        QVariant yaw;
        QVariant pitch;
        QVariant focal_point;
} view_vals_t;

typedef struct gui_controls_t {
	int mode_index;
	int range_index;
	int threshold3d;
	int threshold4d;
	int DynamicAzimuth;
	int DynamicElevation;
	bool isNtc3dOn;
	bool isNtc4dOn;
	bool isCfar3dOn;
	bool isCfar4dOn;
	bool isPhaseEnabled;
} gui_controls_t;

void init_bookmark_SubPub(ros::NodeHandlePtr node_ptr);
void initImageSubPub(ros::NodeHandlePtr node_ptr);
void init_speed_pub(ros::NodeHandlePtr node_ptr);
void init_pubs( void );
void rotation_into_R_t(float alpha, float beta, float gamma, float fs_alpha_backoff);
void calc_cam_projection(bool do_for_all_radars);
void initCalcHeightTiltPub(ros::NodeHandlePtr node_ptr);

void imu_egoYaw_cb(float yaw);
void gps_egoVel_cb(float gps_speed);
void pub_egoVel_cb(float egoVel, float w_z);

class viewpanel: public QTabWidget
{
Q_OBJECT
public:
	viewpanel( QTabWidget* parent = 0 );
	virtual ~viewpanel();
	void on_cmnd_line_view_callback(std_msgs::UInt8 msg );
	void on_cmnd_line_colorcoding_callback(std_msgs::UInt8 msg );
	void on_cmnd_line_screen_record(std_msgs::Bool msg);
    void on_cmnd_line_record(std_msgs::Bool msg);
    void on_cmnd_line_euler_alpha(std_msgs::Float32 msg);
    void on_cmnd_line_euler_beta(std_msgs::Float32 msg);
    void on_cmnd_line_euler_gamma(std_msgs::Float32 msg);
    void on_omega_calibration(std_msgs::Float32 msg);
    void init_subs( void );
	void print_cam_widg_location( void );
	void setView( view_vals_t &view_vals );
	void printView();
    bool get_3rd_prs( void );
    static viewpanel* Instance();
    //static viewpanel* Instance();
	void on_status_mesage(std_msgs::String msg);
	void register_pointcloud_displays(int radar_num);
	void register_slam_displays(int radar_num);
	void register_FS_displays(int radar_num);
    void register_settings(int radar_num);
	void on_radar_calibration_callback(const arbe_msgs::arbeGUIsettings::ConstPtr& controls_data);


public Q_SLOTS:
	void radar_start_stop_control( void );
	void startControl( void );
	void connectControl(void);
	void configPower(void);
	void configCFAR(void);
	void config3DFT(void);
	void configDiff(void);
	void configReg(void);

	void readPower(void);
	void readCFAR(void);
	void read3DFT(void);
	void readDiff(void);
	void readReg(void);
	
    void setCamera_sub_topic(bool flag );
	void enableCamera(bool isUSB );
	void enableFreeSpaceView( void );
	void showSpeedometer(QGridLayout * layout);
	void showTurnRate( QGridLayout * layout);
	static void update_text_metrics( QString * labelText );
	static void update_controls( gui_controls_t * gui_controls_data );
	static void get_controls_values( gui_controls_t * gui_controls_data );
	void rotate_view( void );
	void setFocalPointAndHeading(float x, float y , float h);
    void setPointDecayTime( int decay_time_ms_slider_value );
	void calc_display_offsets( float x, float y, float h );
	void choose_asphalt_or_text(bool asphalt);
	void enableFollowerView( void );


private Q_SLOTS:
    void setThickness( int thickness_percent );
    void setCellSize( int cell_size_percent );
	void setradarXOffset( int radar_x_offset_slider_value );
//	void setradarYOffset( int radar_y_offset_slider_value );
    void setradarZOffset(int radar_z_offset_slider_value);
	void setRadarYawAngle( int radar_yaw_angle_slider_value );
	void setRadarPitchAngle( int radar_pitch_angle_slider_value );
    //void setEuler_labelAndVal( int ten_times_val, uint8_t abg );
	void setCamEulerAlpha( int cam_euler_alpha_slider_value );
	void setCamEulerBeta( int cam_euler_beta_slider_value );
	void setCamEulerGamma( int cam_euler_gamma_slider_value );
	void setColoring( void );
	void setRange( void );
	void setMode( void );
	void setLoadFileType( void );
	void setPreRadarSliders( void );
	void setThreshold4D( int sensitivity_slider_value );
	void setAsphaltRoughness( int AsphaltRoughness_slider_value );
	void setFloatingTextPhi( int floatingTextPhi_slider_value );
	void setDynamicAzimuth( int DynamicAzimuth_slider_value );
	void setDynamicElevation( int DynamicElevation_slider_value );
	void show_configuration_dock( void );
	void screen_record( void );
	void setMinDoppler( int min_doppler_slider_value );
	void setMaxDoppler( int max_doppler_slider_value );
	void setMinColorCoding( int min_slider_value );
	void setMaxColorCoding( int max_slider_value );
	void setMinHeight( int min_height_slider_value );
	void setMaxHeight( int max_height_slider_value );
	void radar_connect_control( void );
	void recording_control( void );
	void closeEvent(QCloseEvent *event);
	void radar_pause_control( void );
	void setAzimuthBin( int azimuth_bin_slider_value );
	void setTopView( void );
	void setStandoffView( void );
	void set120mView( void );
	void setDiagView( void );
	void setBoutique1View( void );
	void setBoutique2View( void );
	void setBoutique3View( void );
    void setTopCarView( void );
	void setCarView( void );
	void setSideView( void );
    bool getFollowerView( void );
    void toggleFollowerView( void );
	void bookmark_control( void );
	void IncreaseSensitivity();
	void DecreaseSensitivity();
	void IncreaeDynamicAzimuth();
	void DecreaseDynamicAzimuth();
	void IncreaeAsphaltRoughness();
	void DecreaseAsphaltRoughness();
	void IncreaeFloatingTextPhi();
	void DecreaseFloatingTextPhi();
	void IncreaeDynamicElevation();
	void DecreaseDynamicElevation();
	void SetShortRnage();
	void SetMidRnage();
	void SetLongRnage();
	void SetUltraLongRnage();
	void Set3d();
	void Set4d();
	void ColorByElevation();
	void ColorByDoppler();
	void ColorByAmplitude();
	void load_camera_calibration();
	void choose_which_radars_text();
    void calc_ant_height_tilt();
	void start_save_task();
	void saveDataThead();

	void loadLidarFile();
	void setSaveFolder();
	void udpConnect();
	void udpClose();
	void updateFFTdata();

protected:
    static void TaskFunc(void *arg);
    static void TaskFuncUdp(void *arg);

private:
	void udpRecvLoop();
	void CreatDebugWindow();
	void CreatUIWindow();
	void CreatCtlPanel();
	int lidarConnect();
	void CreatConnect();
	void CreatFFTcharts();
	void CreatFFTcharts1();
	void Save2filecsv(std::vector<uint8_t> &, bool );
	void parseFFTData(std::vector<uint8_t> &data);
	std::string tohex(uint32_t a);
    QTimer* timer_;
	bool udpStop_;
	void saveData();
	int ctrl_sock;
	int udpRecvSocketFd_;
	std::string lidar_ip;
	int lidar_ctrl_port;
	int lidar_UDP_port;
	double distance_offset;
	rviz::VisualizationManager* manager_;
	rviz::RenderPanel* render_panel_;
	rviz::SelectionPanel* selection_panel_;
	rviz::ToolManager* tool_panel_;

	rviz::Display* grid_;
	rviz::Display* stationary_pointcloud_[MAX_RADARS];
	rviz::Display* fs_polygon_display_[MAX_RADARS] = {};
	rviz::Display* pointcloud_[MAX_RADARS] = {};
	rviz::Display* SlamArray_[MAX_RADARS] = {};
	rviz::Display* Car_;
	rviz::Display* FloatingText_;
	rviz::Display* Camera_[MAX_RADARS];
	rviz::Display* FreeSpace_;
	rviz::Display* Imu_;
	rviz::Display* Axes_;
	rviz::Display* MeshDisplay_;
	rviz::Panel* Detections_Info_;
	QCloseEvent *event;

	rviz::Display* Speedometer_;

	QGridLayout * mainRadarLayout;
    static viewpanel* m_pInstance;
	commandMsg cmdMsg_;

    double display_offset_x_;
    double display_offset_y_;
    double display_offset_h_;
    bool  follower_view_;
	bool recalc_display_offsets;
	QComboBox* loadDataCombo;
	QComboBox* m3DFTCombo;
	QComboBox* CFARCombo;
	QComboBox* PowerCombo;
	QComboBox* DiffCombo;
	QLineEdit *ip_edit;
	QLineEdit *port_edit;
	QLineEdit *udp_port_edit;
	QLineEdit *distance_Offset_edit;
	QString save_folder_;

	QLineEdit* regAddr_line ;
	QLineEdit* regVal_line ;
	QLineEdit* regRead_line;
	QPushButton*  saveBtn;

	fftMsg fftBuff[MAX_BUFF_LEN];
	bstMsgQueue<fftMsg*> fftMsg_free_buf_queue;
	bstMsgQueue<fftMsg*> fftMsg_done_buf_queue;

	std::string loadFileType_;
	QString  loadLidarFile_;
	bool ifConnected;
	bool ifStarted;
	bool ifSave;
	QPushButton *lidar_connect_button;
	QPushButton *setSaveBtn;
	QPushButton *regBtnWrite;
	QPushButton *regBtnRead;
	QPushButton * settingADCSavebutton;
	QPushButton * settingADCConfigbutton;
	std::vector<QPushButton* > ctlWriteBtn_;
	std::vector<QPushButton* > ctlReadBtn_;
	std::vector<QLineEdit* > ctlReadLine_;
	std::vector<double> power_index;
	vx_task bst_task[BST_MAX_TASK_NUM];
	vx_task_create_params_t bst_params;
	std::shared_ptr<autox_msgs::fmcwPoints> fmcwPointsData_;
	autox_msgs::fmcwPoint curPcPoint;
	Chart* fftChart;
	Chart* fftChart_1;
	QCustomPlot *pCustomPlot;
	QCustomPlot *pCustomPlot_1;
	QVector<double> x_FFT;
	QVector<double> y_FFT;
	QVector<double> x_FFT_1;
	QVector<double> y_FFT_1;

};

int read_camera_calibration_from_file(const char * file_name, bool do_for_all_radars);
void rotation_into_R_t_and_calc_cam_projection(float alpha, float beta, float gamma);

#endif // viewpanel_H
