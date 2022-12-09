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
#include "types_fmcw.hpp"
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

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

#include <pcl/io/pcd_io.h>
#include <pcl/point_types.h>
#include <pcl/filters/voxel_grid.h>
#include <pcl/point_cloud.h>
#include <pcl_conversions/pcl_conversions.h>
#include <pcl/common/transforms.h>
#include <sensor_msgs/PointCloud2.h>

#include "plot_tracer.h"
#include <fstream>
#include <sstream>
#include <pc_data.h>
#include "bst_msg_queue.h"
#include  <cmath>

using namespace fmcw_types;
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


class viewpanel: public QTabWidget
{
Q_OBJECT
public:
	viewpanel( QTabWidget* parent = 0 );
	virtual ~viewpanel();
	void setView( view_vals_t &view_vals );
	void printView();
    static viewpanel* Instance();
	void register_pointcloud_displays(int radar_num);
	void registerPointcloudRviz();


public Q_SLOTS:
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

private Q_SLOTS:
	void setLoadFileType( void );
	void screen_record( void );
	void setMinDoppler( int min_doppler_slider_value );
	void setMaxDoppler( int max_doppler_slider_value );
	void setMinColorCoding( int min_slider_value );
	void setMaxColorCoding( int max_slider_value );
	void setMinHeight( int min_height_slider_value );
	void setMaxHeight( int max_height_slider_value );
	void recording_control( void );
	void closeEvent(QCloseEvent *event);
	void start_save_task();
	void saveDataThead();
	void showSpeedometer( QGridLayout * layout);
	void showTurnRate( QGridLayout * layout);
	void loadLidarFile();
	void setSaveFolder();
	void udpConnect();
	void udpClose();
	void updateFFTdata();
	void updateADCdata();
	void showdBFFT();
	void singleFFT();
	void singleADC();
	void resetFFT();
	void resetADC();
	void updateState();
	void printErrorLog();
	void slotTextTcpChanged();
	void recvSerialInfo();
	void recvSerialInfoTest();
	void sendMotorConnectCmd();
	void sendMotorOpenCmd();
	void readMotorPid();


protected:
    static void TaskFunc(void *arg);
    static void TaskFuncUdpRecv(void *arg);
    static void TaskFuncUdpParse(void *arg);
private:
	void udpRecvLoop();
	void udpParseLoop();
	void CreatDebugWindow();
	void CreatUIWindow();
	void CreatADCWindow();
	void CreatMotorWindow();
	void CreatCtlPanel();
	int lidarConnect();
	int motorConnect();
	int motorSerialConnect();
	int motorSerialConnectTest();
	int serialClose(QSerialPort* );
	void releaseSerial();
	void checkMotorConnected();


	void CreatConnect();
	void Save2filecsv(std::vector<uint8_t> &, bool );
	void parseFFTData(std::vector<uint8_t> &data);
	void parseADCData(std::vector<uint8_t> &data);
	void setLED(QLabel* label, int color);
	void setLEDColor(QLabel* label, int color);
	std::string tohex(uint32_t a);
	double fft2dBm(double x);
	void load_settings();
	void save_settings();
	void init_pubs( void );
	void init_queue();
	void setReadOnlyLineEdit(QLineEdit* line);
	void setCheckBoxUnvaild(QCheckBox* line);
    QTimer* timer_;
	QTimer* timer_adc;  
	bool udpStop_;
	void saveData();
	int ctrl_sock;
	int motor_ctrl_sock;
	int udpRecvSocketFd_;
	std::string lidar_ip;
	int lidar_ctrl_port;
	int lidar_UDP_port;
	int motor_port;
	QSerialPort *m_serialPort;
	QSerialPort *m_serialPort_test;


	double distance_offset;
	double power_offset;

	QString lidar_ctrl_port_;
	QString lidar_UDP_port_;
	QString lidar_ip_;
	QString distance_offset_;
	QString power_offset_;

	rviz::VisualizationManager* manager_;
	rviz::RenderPanel* render_panel_;
	rviz::SelectionPanel* selection_panel_;
	rviz::ToolManager* tool_panel_;

	rviz::Display* grid_;
	rviz::Display* stationary_pointcloud_[MAX_RADARS];
	rviz::Display* fs_polygon_display_[MAX_RADARS] = {};
	rviz::Display* pointcloud_[MAX_RADARS] = {};
	rviz::Display* pointcloud_fmcw;

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
	QLineEdit *power_Offset_edit;
	QString save_folder_;
	QLineEdit* regAddr_line ;
	QLineEdit* regVal_line ;
	QLineEdit* regRead_line;
	QLineEdit* adcRead0_line;
	QLineEdit* adcRead1_line;

	QLineEdit* motorPidReadLine[4];

	QLineEdit* motorPidSetLine;
	QLineEdit* motorPidCycleSetLine;
	QLineEdit* motorPidPSetLine;
	QLineEdit* motorPidISetLine;
	QLineEdit* motorPidDSetLine;

	QLineEdit* motorWorkModeReadLine;
	QLineEdit* motorWorkModeAngleSetLine;
	QLineEdit* motorWorkModeSpeedSetLine;
	QLineEdit* motorWorkModeLocSetLine;

	QLineEdit* motorShowCycleSetLine;
	QLineEdit* motorDevReadLine;
	QLineEdit* motorShowItemsLine;
	QLineEdit* motorSoftVersionLine;
	QLineEdit* motorHardVersionLine;
	QLineEdit* motorConnectPortLine;

	QComboBox* motorWorkModeCombo;
	QComboBox* motorSerialCombo;


	QPushButton*  motorConnectBtn;
	QPushButton*  motorSwitchBtn;
	QPushButton*  motorWorkModeSetBtn;
	QPushButton*  motorWorkModeReadBtn;
	QPushButton*  motorPidSetBtn;
	QPushButton*  motorPidReadBtn;
	QPushButton*  motorShowCycleSetBtn;
	QPushButton*  motorShowItemsReadBtn;
	QPushButton*  motorSoftVersionReadBtn;
	QPushButton*  motorHardVersionReadBtn;
	QPushButton*  motorDevReadBtn;
	QPushButton*  motorChartResetBtn;
	QPushButton*  motorChartSingleBtn;
	QStringList   m_serialPortName;
	
	QPushButton*  saveBtn;
	QLabel* devLabel0_state;
	QLabel* devLabel1_state;
	QLabel* devLabel2_state;
	QTextEdit* errorLogText;

	fftMsg fftBuff[MAX_BUFF_LEN];
	adcMsg adcBuff[MAX_BUFF_LEN];
	udp_ADC_FFT_Msg udpFABuff[MAX_BUFF_LEN];
	bstMsgQueue<fftMsg*> fftMsg_free_buf_queue;
	bstMsgQueue<fftMsg*> fftMsg_done_buf_queue;
	bstMsgQueue<adcMsg*> adcMsg_free_buf_queue;
	bstMsgQueue<adcMsg*> adcMsg_done_buf_queue;
	bstMsgQueue<udp_ADC_FFT_Msg*> udpMsg_free_buf_queue;
	bstMsgQueue<udp_ADC_FFT_Msg*> udpMsg_done_buf_queue;
	
	std::string loadFileType_;
	QString  loadLidarFile_;
	bool ifConnected;
	bool ifStarted;
	bool ifConnectedMotor;
	bool ifOpenMotor;

	showModel ifShowdB_;
	bool ifSave;
	QPushButton *lidar_connect_button;
	QPushButton *setSaveBtn;
	QPushButton *regBtnWrite;
	QPushButton *regBtnRead;
	QPushButton * settingADCSavebutton;
	QPushButton * settingADCConfigbutton;
	QPushButton * singelFFTBtn_;
	QPushButton * resetFFTBtn_;
	QPushButton * singelADCBtn_;
	QPushButton * resetADCBtn_;
	QPushButton * mFFTShowdBBtn;

	std::vector<QPushButton* > ctlWriteBtn_;
	std::vector<QPushButton* > ctlReadBtn_;
	std::vector<QLineEdit* > ctlReadLine_;
	std::vector<double> power_index;
	vx_task bst_task[BST_MAX_TASK_NUM];
	vx_task_create_params_t bst_params;
	std::shared_ptr<fmcw_types::fmcwPoints> fmcwPointsData_;
	fmcw_types::fmcwPoint curPcPoint;
	ChartFFT* pFFTchart[2];
	ChartFFT* pADCchart[2];
	ChartFFT* pMotorchart;
	QVector<QCheckBox*> checkShowV;

	QVector<double> x_FFT;
	QVector<double> x_adc0;
	QVector<double> x_adc1;
	QVector<double> y_adc0;
	QVector<double> y_adc1;
	QVector<double> y_FFT;
	QVector<double> x_FFT_1;
	QVector<double> y_FFT_1;
	myTracer* plotTracer;
	ros::Publisher  fmcw_pcl_pub;
	motorCmdMsg motorMsgSend_;
	motorCmdMsg1 motorMsgSend1_;
	motorPidMsg motorMsgPid_;



};


#endif // viewpanel_H
