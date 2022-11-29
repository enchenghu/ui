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
#define DEBUG_UI 0
#define SIGN_LIMIT_NUM 32767
#define SIGN_OFFSET_NUM 65536

static pcData_t g_msg;
static udpMsg g_udpMsg;
extern void radar_quit();
extern void rosbag_start_recording();
extern void rosbag_stop_recording();

extern int terminating;
extern std::string ColoringType;
extern std::string RangeType;
extern std::string mode;

extern float MinDoppler;
extern float MaxDoppler;
extern int rosbag_recording;
extern int grid_cell_size;
extern float MinHeight;
extern float MaxHeight;
int DetectionMemoryTime = 0;
extern int marker_text_size;

QDockWidget *dock;
QDockWidget *dock_param;

QLabel* min_height_label;
QLabel* max_height_label;
QLabel* cc_min_label;
QLabel* cc_max_label;
QLabel* cc_label;
QLabel* mm_doppler_min_label;
QLabel* mm_doppler_max_label;
QLabel* mm_doppler_label;

QPushButton *record_button;
QPushButton *screen_record_button;
QPushButton *radar_pause_button;
QcGaugeWidget* SpeedometerWidget;
static bool radarIdCombo_ready = false;
static int connected_radars = 0;

bool screen_recording = 0;
bool radar_playing = 0;
bool pointcloud_topic_initialized[MAX_RADARS] = {};

QcGaugeWidget * mSpeedGauge;
QcNeedleItem *mSpeedNeedle;
QcNeedleItem *mGpsSpeedNeedle;

QcGaugeWidget * mTurnRateGauge;
QcNeedleItem *mTurnRateNeedle;
QcNeedleItem *mImuTurnRateNeedle;
static float intrinsic_mat[3][3] = {{1526.97,0,934.05},
									{0,1533.03,537.37},
									{0,0,1}};


/* Constructor for the viewpanel. */
viewpanel::viewpanel(QTabWidget* parent )
	: QTabWidget( parent ), ifConnected(false), ifSave(false), \
	save_folder_(QString(".")), udpStop_(true), ifShowdB_(FFT_ORI),\
	power_offset(0.0), distance_offset(0.0)
{
	init_queue();
	memset(&cmdMsg_, 0, sizeof(cmdMsg_));
	cmdMsg_.mHead.usPrefix = 0xeeff;
	cmdMsg_.mHead.usType = 0x10;
	cmdMsg_.mHead.usPayloadCrc = 0x00;
	cmdMsg_.mHead.unLength = 12;

	power_index = {0.2, 12, 20, 70, 290, 346, 347, 
				   397, 415, 500, 510, 560, 625, 1000, 
				   1130, 1450, 1580, 1600};
	load_settings();
	CreatUIWindow();
	CreatDebugWindow();
	CreatADCWindow();
	CreatConnect();

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

	/* Create the radar pointcloud fixed frame. */
	manager_->setFixedFrame("image_radar");

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

	resize(QDesktopWidget().availableGeometry(this).size() * 0.85);
}

// Destructor.
viewpanel::~viewpanel()
{
	delete manager_;
}

void viewpanel::init_queue()
{
	x_FFT.clear();
	x_FFT_1.clear();
	x_adc0.clear();
	x_adc1.clear();
	for(int i = 0; i< 8192;i++) 
	{
		x_FFT.append(i);
		x_FFT_1.append(-8191 + i);
		x_adc0.append(i);
		x_adc1.append(i);
	}
	fmcwPointsData_ = std::make_shared<fmcw_types::fmcwPoints>();
    fftMsg_free_buf_queue.setParam("fftMsg_free_buf_queue", MAX_BUFF_LEN);
    fftMsg_done_buf_queue.setParam("fftMsg_done_buf_queue", MAX_BUFF_LEN);
    adcMsg_free_buf_queue.setParam("adcMsg_free_buf_queue", MAX_BUFF_LEN);
    adcMsg_done_buf_queue.setParam("adcMsg_done_buf_queue", MAX_BUFF_LEN);
	udpMsg_free_buf_queue.setParam("udpMsg_free_buf_queue", MAX_BUFF_LEN);
    udpMsg_done_buf_queue.setParam("udpMsg_done_buf_queue", MAX_BUFF_LEN); 
    for (int loop = 0; loop < 4; loop++) {
        fftMsg *fbuf0 = &fftBuff[loop];
        adcMsg *fbuf1 = &adcBuff[loop];
        udp_ADC_FFT_Msg *fbuf2 = &udpFABuff[loop];
        fftMsg_free_buf_queue.put(fbuf0);
        adcMsg_free_buf_queue.put(fbuf1);
        udpMsg_free_buf_queue.put(fbuf2);
    }

}

void viewpanel::loadLidarFile(void){

	setLoadFileType();
	if(loadFileType_ ==  "Config file") {
		loadLidarFile_  =  QFileDialog::getOpenFileName(
												viewpanel::Instance(),
												"Choose config file",
												QDir::currentPath(),
												"Configration files (*.conf)",0,QFileDialog::DontUseNativeDialog);
	} else if (loadFileType_ ==  "DAC file") {
		loadLidarFile_  =  QFileDialog::getOpenFileName(
												viewpanel::Instance(),
												"Choose DAC data file",
												QDir::currentPath(),
												"DAC data  files (*.bin)",0,QFileDialog::DontUseNativeDialog);
	} else if (loadFileType_ ==  "Cali file"){
		loadLidarFile_  =  QFileDialog::getOpenFileName(
												viewpanel::Instance(),
												"Choose  calibration file",
												QDir::currentPath(),
												"Calibration files (*.cali)",0,QFileDialog::DontUseNativeDialog);
	} else {

	}

	if( !loadLidarFile_.isNull() )
	{
		qDebug() << "selected file path : " << loadLidarFile_.toUtf8();
	}	
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




void viewpanel::setLoadFileType( void ){
	QString str = loadDataCombo->currentText();
	loadFileType_.assign(str.toStdString());
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
}



void viewpanel::startControl(void){
}

void viewpanel::connectControl(void){
	if(!ifStarted){
		if(lidarConnect() < 0){
			QMessageBox msgBox;
			msgBox.setText("connect to the lidar failed!");
			msgBox.exec();
		}else {
			lidar_connect_button->setStyleSheet("color: green");
			lidar_connect_button->setText("&Disconnect");
			ifStarted = true;
		}
	}else {
		lidar_connect_button->setStyleSheet("color: black");
		lidar_connect_button->setText("&Connect");
		ifStarted = false;
		::close(ctrl_sock);
	}
}

void viewpanel::showdBFFT(void){
	if(ifShowdB_ == FFT_ORI){
		mFFTShowdBBtn->setStyleSheet("color: green");
		mFFTShowdBBtn->setText("&Show ori");
		ifShowdB_ = FFT_DB;
		power_offset = power_Offset_edit->text().toDouble();
	}else if(ifShowdB_ == FFT_DB){
		mFFTShowdBBtn->setStyleSheet("color: black");
		mFFTShowdBBtn->setText("&Show dB");
		ifShowdB_ = FFT_ORI;
	}
	for(int i = 0 ; i < 2; i++){
		pFFTchart[i]->setShowType(ifShowdB_);
		pFFTchart[i]->setIfScale(true);
	}
}

void viewpanel::configPower(void){

	QString str = PowerCombo->currentText();
	cmdMsg_.mCommandVal[0] = str.toDouble() * 100;
	cmdMsg_.mHead.usCommand = commandType::POWER_WRITE;
	if(::write(ctrl_sock, &cmdMsg_, sizeof(commandMsg)) < 0){
		QMessageBox msgBox;
		msgBox.setText("config power failed!");
		msgBox.exec();
		return;
	}
	QMessageBox msgBox;
	msgBox.setText("config power success!");
	msgBox.exec();
}

void viewpanel::readPower(void){
	cmdMsg_.mHead.usCommand = commandType::POWER_READ;
	if(::write(ctrl_sock, &cmdMsg_, sizeof(commandMsg)) < 0){
		QMessageBox msgBox;
		msgBox.setText("read power failed!");
		msgBox.exec();
		return;
	}
	commandMsg cmdMsg;
	bool ifread = true;
	int ret = 0;
	while(1){
		memset(&cmdMsg, 0, sizeof(cmdMsg));
		ret = ::recv(ctrl_sock, &cmdMsg, sizeof(commandMsg), MSG_WAITALL);
		if (ret <= 0 && ifread){
			if (ret < 0 && errno == EAGAIN){
				QMessageBox msgBox;
				msgBox.setText("read Power timeout!");
				msgBox.exec();	
				return;			
			}
			usleep(100*1000);
			ifread = false;
			continue;
		} else {
			if (cmdMsg.mHead.usCommand == commandType::POWER_READ){
				double power = cmdMsg.mCommandVal[0] / 100.0;
				ctlReadLine_[0]->setText(QString::number(power));
				break;
			} else {
				QMessageBox msgBox;
				msgBox.setText("read power failed!");
				msgBox.exec();
				return;
			}
		}
	}
	QMessageBox msgBox;
	msgBox.setText("read power success!");
	msgBox.exec();
	return;	
}

void viewpanel::configCFAR(void){
	QString str = CFARCombo->currentText();
	cmdMsg_.mCommandVal[0] = str.toInt();
	cmdMsg_.mHead.usCommand = commandType::CFAR_WRITE;
	if(::write(ctrl_sock, &cmdMsg_, sizeof(commandMsg)) < 0){
		QMessageBox msgBox;
		msgBox.setText("config CFAR failed!");
		msgBox.exec();
		return;
	}
	QMessageBox msgBox;
	msgBox.setText("config CFAR success!");
	msgBox.exec();
}

void viewpanel::readCFAR(void){
	cmdMsg_.mHead.usCommand = commandType::CFAR_READ;
	if(::write(ctrl_sock, &cmdMsg_, sizeof(commandMsg)) < 0){
		ctlReadBtn_[1]->setStyleSheet("color: black");
		QMessageBox msgBox;
		msgBox.setText("read CFAR failed!");
		msgBox.exec();
		return;
	}
	commandMsg cmdMsg;
	bool ifread = true;
	int ret = 0;
	while(1){
		memset(&cmdMsg, 0, sizeof(cmdMsg));
		ret = ::recv(ctrl_sock, &cmdMsg, sizeof(commandMsg), MSG_WAITALL);
		if(ret <= 0 && ifread){
			if(ret < 0 && errno == EAGAIN){
				QMessageBox msgBox;
				msgBox.setText("read CFAR timeout!");
				msgBox.exec();	
				return;			
			}
			usleep(100*1000);
			ifread = false;
			continue;
		} else {
			if (cmdMsg.mHead.usCommand == commandType::CFAR_READ){
				ctlReadLine_[1]->setText(QString::number(cmdMsg.mCommandVal[0]));
				break;
			} else {
				QMessageBox msgBox;
				msgBox.setText("read CFAR failed!");
				msgBox.exec();
				return;				
			}
		}
	}
	QMessageBox msgBox;
	msgBox.setText("read CFAR success!");
	msgBox.exec();
}

void viewpanel::config3DFT(void){
	QString str = m3DFTCombo->currentText();
	cmdMsg_.mCommandVal[0] = str.toInt();
	cmdMsg_.mHead.usCommand = commandType::DFT3_WRITE;
	if(::write(ctrl_sock, &cmdMsg_, sizeof(commandMsg)) < 0){
		QMessageBox msgBox;
		msgBox.setText("config 3DFT failed!");
		msgBox.exec();
		return;
	}
	QMessageBox msgBox;
	msgBox.setText("config 3DFT success!");
	msgBox.exec();
}

void viewpanel::read3DFT(void){
	cmdMsg_.mHead.usCommand = commandType::DFT3_READ;
	if(::write(ctrl_sock, &cmdMsg_, sizeof(commandMsg)) < 0){
		ctlReadBtn_[2]->setStyleSheet("color: black");
		QMessageBox msgBox;
		msgBox.setText("read 3DFT failed!");
		msgBox.exec();
		return;
	}
	commandMsg cmdMsg;
	bool ifread = true;
	int ret = 0;
	while(1) {
		memset(&cmdMsg, 0, sizeof(cmdMsg));
		ret = ::recv(ctrl_sock, &cmdMsg, sizeof(commandMsg),MSG_WAITALL);
		if(ret <= 0 && ifread){
			if(ret < 0 && errno == EAGAIN){
				QMessageBox msgBox;
				msgBox.setText("read 3DFT timeout!");
				msgBox.exec();	
				return;			
			}
			usleep(100*1000);
			ifread = false;
			continue;
		}else{
			if(cmdMsg.mHead.usCommand == commandType::DFT3_READ){
				ctlReadLine_[2]->setText(QString::number(cmdMsg.mCommandVal[0]));
				break;
			} else {
				QMessageBox msgBox;
				msgBox.setText("read 3DFT failed!");
				msgBox.exec();
				return;				
			}
		}
	}
	QMessageBox msgBox;
	msgBox.setText("read 3DFT success!");
	msgBox.exec();
}

void viewpanel::configDiff(void){
	QString str = DiffCombo->currentText();
	cmdMsg_.mCommandVal[0] = str.toInt();
	cmdMsg_.mHead.usCommand = commandType::DIFF_WRITE;
	if(::write(ctrl_sock, &cmdMsg_, sizeof(commandMsg)) < 0){
		QMessageBox msgBox;
		msgBox.setText("config Diff failed!");
		msgBox.exec();
		return;
	}
	QMessageBox msgBox;
	msgBox.setText("config Diff success!");
	msgBox.exec();
}
void viewpanel::configReg(void){
	QString strAddr = regAddr_line->text();
	QString strValue = regVal_line->text();
	//unsigned int x;
	std::stringstream ss;
	ss << std::hex << strAddr.toStdString();
	ss >> cmdMsg_.mCommandVal[0];


	std::stringstream tt;
	tt << std::hex << strValue.toStdString();
	tt >> cmdMsg_.mCommandVal[1];

	if(cmdMsg_.mCommandVal[0] >= 0xffffffff || cmdMsg_.mCommandVal[0] < 0x0 ||
	cmdMsg_.mCommandVal[1] >= 0xffffffff || cmdMsg_.mCommandVal[1] < 0x0 ){
		QMessageBox msgBox;
		msgBox.setText("error!!register addr or value is Invalid!");
		msgBox.exec();
		return;		
	}


	//cmdMsg_.mCommandVal[1] = strValue.toInt();
	cmdMsg_.mHead.usCommand = commandType::REG_WRITE;
	std::cout << "cmdMsg_.regAddr is " << cmdMsg_.mCommandVal[0] << " cmdMsg_.regVal " << cmdMsg_.mCommandVal[1] << std::endl;
	if(::write(ctrl_sock, &cmdMsg_, sizeof(commandMsg)) < 0){
		QMessageBox msgBox;
		msgBox.setText("config register failed!");
		msgBox.exec();
		return;
	}
	QMessageBox msgBox;
	msgBox.setText("write register success!");
	msgBox.exec();
}
double viewpanel::fft2dBm(double x){
	double inputV = x / pow(2, 17);
	double res = 10 * log10(pow(inputV, 2) / 100) - 1.1;
	return res; 
}

std::string viewpanel::tohex(uint32_t a){
	std::string res;
	std::string temp;
	int zerolen;
	if(a == 0) return std::string("00000000");
	uint32_t y = 0;
	while(a > 0)//大于0的数
	{
		y = a % 16;  //求余
		if(y < 10)   //小于10的余数
		 res =char('0'+ y) + res;  //利用字符的ascll码在字符串前面拼接
		else
		 res = char('A'- 10 + y) + res;  //大于9的余数用ABCDE表示
		a = a / 16; 
	} 
	zerolen = 8 - res.size();
	while (zerolen > 0)
	{
		temp += "0";
		zerolen--;
	}
	res = temp + res;
	return res; 	
}
void viewpanel::readReg(void){
	cmdMsg_.mHead.usCommand = commandType::REG_READ;
	QString strAddr = regAddr_line->text();
	std::stringstream ss;
	ss << std::hex << strAddr.toStdString();
	ss >> cmdMsg_.mCommandVal[0];

	if(::write(ctrl_sock, &cmdMsg_, sizeof(commandMsg)) < 0){
		regBtnRead->setStyleSheet("color: black");	
		QMessageBox msgBox;
		msgBox.setText("read Reg failed!");
		msgBox.exec();
		return;
	}
	commandMsg cmdMsg;
	bool ifread = true;
	int ret = 0;
	while(1){
		memset(&cmdMsg, 0, sizeof(cmdMsg));
		ret = ::recv(ctrl_sock, &cmdMsg, sizeof(commandMsg), MSG_WAITALL);
		if(ret <= 0 && ifread){
			if(ret < 0 && errno == EAGAIN){
				QMessageBox msgBox;
				msgBox.setText("read Reg timeout!");
				msgBox.exec();	
				return;			
			}
			usleep(100*1000);
			ifread = false;
			continue;
		}else{
			if(cmdMsg.mHead.usCommand == commandType::REG_READ){
				std::string tmp = tohex(cmdMsg.mCommandVal[1]);
				//regRead_line->setText(QString::number(cmdMsg.mCommandVal[1]));
				regRead_line->setText(QString::fromStdString(tmp));
				break;
			} else {
				QMessageBox msgBox;
				msgBox.setText("read Reg failed!");
				msgBox.exec();
				return;				
			}
		}
	}
	QMessageBox msgBox;
	msgBox.setText("read Reg success!");
	msgBox.exec();
}

void viewpanel::readDiff(void){
	cmdMsg_.mHead.usCommand = commandType::DIFF_READ;
	if(::write(ctrl_sock, &cmdMsg_, sizeof(commandMsg)) < 0){
		ctlReadBtn_[3]->setStyleSheet("color: black");
		QMessageBox msgBox;
		msgBox.setText("read Diff failed!");
		msgBox.exec();
		return;
	}
	commandMsg cmdMsg;
	bool ifread = true;
	int ret = 0;
	while(1){
		memset(&cmdMsg, 0, sizeof(cmdMsg));
		ret = ::recv(ctrl_sock, &cmdMsg, sizeof(commandMsg), MSG_WAITALL);
		if(ret <= 0 && ifread){
			if(ret < 0 && errno == EAGAIN){
				QMessageBox msgBox;
				msgBox.setText("read Diff timeout!");
				msgBox.exec();	
				return;			
			}
			usleep(100*1000);
			ifread = false;
			continue;
		}else{
			if(cmdMsg.mHead.usCommand == commandType::DIFF_READ){
				std::string tmp = tohex(cmdMsg.mCommandVal[0]);
				ctlReadLine_[3]->setText(QString::fromStdString(tmp));
				break;
			} else {
				QMessageBox msgBox;
				msgBox.setText("read Diff failed!");
				msgBox.exec();
				return;				
			}
		}
	}
	QMessageBox msgBox;
	msgBox.setText("read diff success!");
	msgBox.exec();
}

void viewpanel::printView(  )
{
	cmdMsg_.mHead.usCommand = commandType::FFT_ADC_READ_STOP;
	::write(ctrl_sock, &cmdMsg_, sizeof(commandMsg));
	::close(udpRecvSocketFd_);
	::close(ctrl_sock);
	udpStop_ = true;
	ROS_INFO("Like this view? [D,Y,P,F] = [%f,%f,%f,%s]",
			manager_->getViewManager()->getCurrent()->subProp("Distance")->getValue().toFloat(),
			manager_->getViewManager()->getCurrent()->subProp("Yaw")->getValue().toFloat(),
			manager_->getViewManager()->getCurrent()->subProp("Pitch")->getValue().toFloat(),
			manager_->getViewManager()->getCurrent()->subProp("Focal Point")->getValue().toString().toStdString().c_str());
	save_settings();
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

void viewpanel::closeEvent(QCloseEvent *event)
{
	//print_cam_widg_location();
	save_settings();
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

void viewpanel::registerPointcloudRviz()
{
	std::string pointcloud_topic = "/fmcw/rviz/pointcloud";
	//std::string stationary_pointcloud_topic = "/fmcw/rviz/stationary_pointcloud";
	ROS_INFO("Registering new pointcloud topic: %s",pointcloud_topic.c_str());	
	//ROS_INFO("Registering new pointcloud topic: %s",stationary_pointcloud_topic.c_str());

	pointcloud_fmcw = manager_->createDisplay( "rviz/PointCloud2", "PointCloud2", true );
	ROS_ASSERT(pointcloud_fmcw!=NULL);
	pointcloud_fmcw->subProp("Topic")->setValue( pointcloud_topic.c_str() );
	pointcloud_fmcw->subProp("Style")->setValue("Spheres");
	pointcloud_fmcw->subProp("Size (Pixels)")->setValue("3");
	pointcloud_fmcw->subProp("Size (m)")->setValue("0.3");

	//pointcloud_fmcw->subProp("Decay Time")->setValue((float)DetectionMemoryTime / 1000);

	pointcloud_fmcw->subProp("Color Transformer")->setValue("RGB8");
	pointcloud_fmcw->subProp("Invert Rainbow")->setValue("false");
	pointcloud_fmcw->subProp("Position Transformer")->setValue("XYZ");
	pointcloud_fmcw->subProp("Use Fixed Frame")->setValue("true");
	pointcloud_fmcw->subProp( "Axis" )->setValue( "Z" );
}

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


void viewpanel::init_pubs( void )
{
	ros::NodeHandlePtr node_ptr = boost::make_shared<ros::NodeHandle>();

	//arbe_gui_commands_pub = node_ptr->advertise<std_msgs::String>("arbe/settings/gui_commands", 1);

	fmcw_pcl_pub = node_ptr->advertise<sensor_msgs::PointCloud2>("/arbe/rviz/pointcloud_", 1);

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

viewpanel* viewpanel::m_pInstance = NULL;

viewpanel* viewpanel::Instance()
{
    if (!m_pInstance)
        m_pInstance = new viewpanel();

    return m_pInstance;
}


void viewpanel::CreatADCWindow()
{
	QWidget* multiWidget_new = new QWidget();

	QGroupBox *chartADCBox0 = new QGroupBox(tr("ADC chart 0-0:"));
	QGridLayout* chartADCLayout0 = new QGridLayout ;
	QGroupBox *chartADCBox1 = new QGroupBox(tr("ADC chart 0-1:"));
	QGridLayout* chartADCLayout1 = new QGridLayout ;

#if 0
    OSC_chart *label_OSC_0 = new OSC_chart(this);
    label_OSC_0->set_chart(10,20,this->width() /  2 -20,this->height()  / 2-20);
    label_OSC_0->Add_Line_Data(0, 100);
    label_OSC_0->View_Chart(1000);
#endif
	pADCchart[0] = new ChartFFT(this);
	pADCchart[1] = new ChartFFT(this);
	pADCchart[0]->setShowType(ADC_ORI);
	pADCchart[1]->setShowType(ADC_ORI);
	chartADCLayout0->addWidget(pADCchart[0]->setChart(0, 8192, -32768, 32768), 0 , 0);
	chartADCBox0->setLayout(chartADCLayout0);
#if 0
    OSC_chart *label_OSC_1 = new OSC_chart(this);
    label_OSC_1->set_chart(10,20,this->width() /  2 - 20,this->height() / 2 - 20);
    label_OSC_1->Add_Line_Data(0, 100);
    //label_OSC_1->View_Chart(10000);
#endif
	chartADCLayout1->addWidget(pADCchart[1]->setChart(0, 8192, -32768, 32768), 0, 0);
	chartADCBox1->setLayout(chartADCLayout1);

	QVBoxLayout* adcCharts= new QVBoxLayout ;
	adcCharts->addWidget(chartADCBox0);
	adcCharts->addWidget(chartADCBox1);
	
	QGridLayout* adcSettingLayout = new QGridLayout ;
	QGroupBox* adcSettingBox = new QGroupBox(tr("ADC control:"));
	QGridLayout* adcSettingBoxLayout = new QGridLayout ;
	singelADCBtn_ = new QPushButton("&Single");
	resetADCBtn_ = new QPushButton("&Reset");
	adcSettingBoxLayout->addWidget(singelADCBtn_, 0, 0);//, Qt::AlignTop);
	adcSettingBoxLayout->addWidget(resetADCBtn_, 0, 1);//, Qt::AlignTop);
	adcSettingBox->setLayout(adcSettingBoxLayout);
	adcSettingLayout->addWidget(adcSettingBox);

	QGridLayout* main_show= new QGridLayout ;
	main_show->setColumnStretch(0, 9);
	main_show->setColumnStretch(1, 1);
	main_show->addLayout(adcCharts, 0, 0 );
	main_show->addLayout(adcSettingLayout, 0, 1);

	multiWidget_new->setLayout(main_show);
	this->addTab(multiWidget_new,  "ADC Debug Mainwindow");
}


void viewpanel::CreatDebugWindow()
{
	QWidget* multiWidget_new = new QWidget();
	QGroupBox *chartADCBox = new QGroupBox(tr("FFT  chart 0:"));
	QGridLayout* chartADCLayout = new QGridLayout ;
	QGroupBox *chartFFTBox = new QGroupBox(tr("FFT  chart 1:"));
	QGridLayout* chartFFTLayout = new QGridLayout ;

	std::cout << "this->width() is "  << this->width() << " this->height() is " << this->height() << std::endl;
#if 0
    OSC_chart *label_OSC_0 = new OSC_chart(this);
    label_OSC_0->set_chart(10,20,this->width() /  2 -20,this->height()  / 2-20);
    label_OSC_0->Add_Line_Data(0, 100);
    label_OSC_0->View_Chart(1000);
#endif
	pFFTchart[0] = new ChartFFT(this);
	pFFTchart[1] = new ChartFFT(this);
	if(pFFTchart[0]) chartADCLayout->addWidget(pFFTchart[0]->setChart(0, 8192, 0, 256 * 4096), 0 , 0);
	chartADCBox->setLayout(chartADCLayout);
#if 0
    OSC_chart *label_OSC_1 = new OSC_chart(this);
    label_OSC_1->set_chart(10,20,this->width() /  2 - 20,this->height() / 2 - 20);
    label_OSC_1->Add_Line_Data(0, 100);
    //label_OSC_1->View_Chart(10000);
#endif
	if(pFFTchart[1]) chartFFTLayout->addWidget(pFFTchart[1]->setChart(-8191, 0, 0, 256 * 4096), 0, 0);
	chartFFTBox->setLayout(chartFFTLayout);

	QGridLayout* main_show= new QGridLayout ;
	main_show->setColumnStretch(0, 9);
	main_show->setColumnStretch(1, 1);

	QVBoxLayout* charts= new QVBoxLayout ;
	charts->addWidget(chartADCBox);
	charts->addWidget(chartFFTBox);

	QHBoxLayout* configs = new QHBoxLayout ;
	QGroupBox *addrConfigsBox = new QGroupBox(tr("Configs:"));
	QGroupBox *settingBox = new QGroupBox(tr("Settings:"));
	QGroupBox *addrBox = new QGroupBox(tr("addr config:"));
	QGroupBox *settingADCBox = new QGroupBox(tr("FFT control:"));
	QGroupBox *settingFFTBox = new QGroupBox(tr("FFT:"));

	QVBoxLayout* addrConfigLayout = new QVBoxLayout;
	QVBoxLayout* settingLayout = new QVBoxLayout;
	QGridLayout* settingADCLayout = new QGridLayout;
	QGridLayout* settingFFTLayout = new QGridLayout;

	QPushButton * writeAddrbutton = new QPushButton("&Write");
	QPushButton * readAddrbutton = new QPushButton("&Read");
	mFFTShowdBBtn = new QPushButton("&Show dB");

	QPushButton * settingFFTSavebutton = new QPushButton("&Save");
	QPushButton * settingFFTConfigbutton = new QPushButton("&Config");
	singelFFTBtn_ = new QPushButton("&Single");
	resetFFTBtn_ = new QPushButton("&Reset");

	QLabel* power_Offset_label = new QLabel("Power Offset/dB" );
	power_Offset_edit = new QLineEdit();	
	power_Offset_edit->setText(power_offset_);

	settingADCLayout->addWidget(power_Offset_label, 1, 0);
	settingADCLayout->addWidget(power_Offset_edit, 1, 1);

	settingADCLayout->addWidget(mFFTShowdBBtn, 2, 0);
	settingADCLayout->addWidget(resetFFTBtn_, 2, 1);
	settingADCLayout->addWidget(singelFFTBtn_, 3, 0);

	settingADCBox->setLayout(settingADCLayout);
	settingFFTLayout->addWidget(settingFFTSavebutton, 0, 0);
	settingFFTLayout->addWidget(settingFFTConfigbutton, 0, 1);
	settingFFTBox->setLayout(settingFFTLayout);

	settingLayout->addWidget(settingADCBox);
	settingLayout->addWidget(settingFFTBox);
	settingBox->setLayout(settingLayout);
	addrConfigLayout->addWidget(settingBox);
	addrConfigsBox->setLayout(addrConfigLayout);

	configs->addWidget(addrConfigsBox);
	main_show->addLayout(charts, 0, 0 );
	main_show->addLayout(configs, 0, 1);

	multiWidget_new->setLayout(main_show);

	this->addTab(multiWidget_new,  "FFT Debug Mainwindow");

}

void viewpanel::CreatConnect()
{
	connect(lidar_connect_button, SIGNAL(clicked()), this, SLOT( connectControl( void )));
	connect(ctlWriteBtn_[0], SIGNAL(clicked()), this, SLOT( configPower( void )));
	connect(ctlWriteBtn_[1], SIGNAL(clicked()), this, SLOT( configCFAR( void )));
	connect(ctlWriteBtn_[2], SIGNAL(clicked()), this, SLOT( config3DFT( void )));
	connect(ctlWriteBtn_[3], SIGNAL(clicked()), this, SLOT( configDiff( void )));
	connect(regBtnWrite, SIGNAL(clicked()), this, SLOT( configReg( void )));
	connect(saveBtn, SIGNAL(clicked()), this, SLOT( saveDataThead( void )));
	connect(setSaveBtn, SIGNAL(clicked()), this, SLOT( setSaveFolder( void )));
	connect(settingADCSavebutton, SIGNAL(clicked()), this, SLOT( udpConnect( void )));
	connect(settingADCConfigbutton, SIGNAL(clicked()), this, SLOT( udpClose( void )));
	connect(ctlReadBtn_[0], SIGNAL(clicked()), this, SLOT( readPower( void )));
	connect(ctlReadBtn_[1], SIGNAL(clicked()), this, SLOT( readCFAR( void )));
	connect(ctlReadBtn_[2], SIGNAL(clicked()), this, SLOT( read3DFT( void )));
	connect(ctlReadBtn_[3], SIGNAL(clicked()), this, SLOT( readDiff( void )));
	connect(regBtnRead, SIGNAL(clicked()), this, SLOT( readReg( void )));
	connect(mFFTShowdBBtn, SIGNAL(clicked()), this, SLOT( showdBFFT( void )));

	connect(singelFFTBtn_, SIGNAL(clicked()), this, SLOT( singleFFT( void )));
	connect(resetFFTBtn_, SIGNAL(clicked()), this, SLOT( resetFFT( void )));
	connect(singelADCBtn_, SIGNAL(clicked()), this, SLOT( singleADC( void )));
	connect(resetADCBtn_, SIGNAL(clicked()), this, SLOT( resetADC( void )));
	connect(errorLogText,SIGNAL(textChanged()),SLOT(slotTextTcpChanged()));
    timer_  = new QTimer(this);
    //timer_->setInterval(50);
    connect(timer_, SIGNAL(timeout()), this, SLOT(updateFFTdata(void)));
    timer_->start(100);

    timer_adc  = new QTimer(this);
    //timer_->setInterval(50);
    connect(timer_adc, SIGNAL(timeout()), this, SLOT(updateADCdata(void)));
    timer_adc->start(100);

    QTimer* timer_state  = new QTimer(this);
    //timer_->setInterval(50);
    connect(timer_state, SIGNAL(timeout()), this, SLOT(updateState(void)));
    connect(timer_state, SIGNAL(timeout()), this, SLOT(printErrorLog(void)));
    timer_state->start(500);

}

void viewpanel::CreatUIWindow()
{
	const QSize button_size = QSize(200, 30);
	const QSize view_button_side = QSize(60, 25);
	const QSize slider_size = QSize(200, 20);

	QWidget* multiWidget = new QWidget();
	QDockWidget* ctrlDock = new QDockWidget();
	QWidget* ctrlDockWidget = new QWidget();

	ctrlDock->setFeatures(QDockWidget::DockWidgetClosable );
	//dock->setWidget(multiWidget);
	ctrlDock->setMaximumHeight(250);

	//QDockWidget去掉标题栏
	//QWidget * titleBarWidget= ctrlDock->titleBarWidget();
	//QWidget *pEmptyWidget = new QWidget();
	//ctrlDock->setTitleBarWidget(pEmptyWidget);//设置一个空的widget作为标题栏
	//delete titleBarWidget;//删除原标题栏

	QGridLayout* controls = new QGridLayout ;
	QGridLayout* mainLayout = new QGridLayout ;
	//mainLayout->setRowStretch(0, 1);
	//mainLayout->setRowStretch(1, 5);

	QGroupBox *controlsBox = new QGroupBox(tr("Basic Controls:"));
	QGridLayout* controls_layout = new QGridLayout;
	QGroupBox *fileBox  = new QGroupBox(tr("File Operations:"));
	QVBoxLayout* fileLayout = new QVBoxLayout;
	QGroupBox *savefileBox  = new QGroupBox(tr("Save:"));
	QGridLayout* saveLayout = new QGridLayout;
	QLabel* saveDatalabel = new QLabel( "Select" );
	QComboBox*saveDataCombo = new QComboBox;
	saveBtn = new QPushButton("Save PC", this);
	saveDataCombo->addItem(tr("point cloud "));
	saveDataCombo->addItem(tr("data stream 1"));
	saveDataCombo->addItem(tr("data stream 2"));
	saveDataCombo->addItem(tr("data stream 3"));
	saveDataCombo->addItem(tr("data stream 4"));
	saveDataCombo->setCurrentIndex(0);
	saveLayout->addWidget(saveDatalabel, 0, 0, Qt::AlignLeft);
	saveLayout->addWidget(saveDataCombo, 0, 1, Qt::AlignLeft);
	saveLayout->addWidget(saveBtn, 0, 2, Qt::AlignLeft);

	//saveLayout->setColumnStretch(0, 1);
	//saveLayout->setColumnStretch(1, 2);
	//saveLayout->setColumnStretch(2, 2);

	savefileBox->setLayout(saveLayout);
	fileLayout->addWidget(savefileBox);

	QGroupBox *loadfileBox  = new QGroupBox(tr("Load:"));
	QGridLayout* loadLayout = new QGridLayout;
	QLabel* loadDatalabel = new QLabel( "Select" );
	loadDataCombo = new QComboBox;
	QPushButton*  loadBtn = new QPushButton("Load", this);
	loadDataCombo->addItem(tr("Config file"));
	loadDataCombo->addItem(tr("DAC file"));
	loadDataCombo->addItem(tr("Cali file"));
	//saveDataCombo->setCurrentIndex(0);
	loadLayout->addWidget(loadDatalabel, 0, 0, Qt::AlignLeft);
	loadLayout->addWidget(loadDataCombo, 0, 1, Qt::AlignLeft);
	loadLayout->addWidget(loadBtn, 0, 2, Qt::AlignLeft);

	loadfileBox->setLayout(loadLayout);
	fileLayout->addWidget(loadfileBox);

	fileBox->setLayout(fileLayout);
    connect( loadDataCombo, SIGNAL( currentTextChanged(QString)), this, SLOT( setLoadFileType( void )));
	connect( loadBtn, SIGNAL( clicked()), this, SLOT( loadLidarFile( void )));
	lidar_connect_button = new QPushButton("&Connect", this);
	//lidar_disconnect_button = new QPushButton("Disconnect", this);
	setSaveBtn = new QPushButton("&Set save folder", this);
	//lidar_stop_button = new QPushButton("Stop", this);
	//lidarIdCombo =  new QComboBox;

	QLabel* lidar_IP_label = new QLabel( "IP addr" );
	QLabel* lidar_port_label = new QLabel( "TCP Port" );
	QLabel* lidar_udp_port_label = new QLabel( "UDP Port" );
	QLabel* distanceOffset_label = new QLabel( "distance Offset" );

	ip_edit =  new QLineEdit();
	port_edit =  new QLineEdit();
	udp_port_edit =  new QLineEdit();
	distance_Offset_edit = new QLineEdit();

	ip_edit->setPlaceholderText("input ip addr");
	ip_edit->setText(lidar_ip_);
	port_edit->setPlaceholderText("input tcp port");
	port_edit->setText(lidar_ctrl_port_);
	udp_port_edit->setPlaceholderText("input udp port");
	udp_port_edit->setText(lidar_UDP_port_);
	distance_Offset_edit->setPlaceholderText("input distance offset ");
	distance_Offset_edit->setText(distance_offset_);

	controls_layout->addWidget( lidar_IP_label, 0, 0, Qt::AlignLeft);
	controls_layout->addWidget( ip_edit, 0, 1, Qt::AlignLeft);
	controls_layout->addWidget( lidar_port_label, 1, 0, Qt::AlignLeft);
	controls_layout->addWidget( port_edit, 1, 1, Qt::AlignLeft);

	controls_layout->addWidget( lidar_udp_port_label, 2, 0, Qt::AlignLeft);
	controls_layout->addWidget( udp_port_edit, 2, 1, Qt::AlignLeft);

	controls_layout->addWidget( lidar_connect_button, 3, 0, Qt::AlignLeft);
	controls_layout->addWidget( setSaveBtn, 3, 1, Qt::AlignLeft);

	QLabel* CFAR_label = new QLabel( "CFAR" );
	QLabel* m3DFT_label = new QLabel( "3DFT" );
	QLabel* Power_label = new QLabel( "Power/mW" );
	QLabel* diff_label = new QLabel( "Diff" );
	QLabel* regAddr_label = new QLabel( "Register Addr" );
	QLabel* regVal_label = new QLabel( "Register value" );
	QLabel* adc_label0 = new QLabel( "ADC overvoltage" );
	QLabel* adc_label1 = new QLabel( "ADC attenuation" );

	regAddr_line = new QLineEdit;
	regVal_line = new QLineEdit;
	regRead_line = new QLineEdit;
	adcRead0_line = new QLineEdit;
	adcRead1_line = new QLineEdit;
	setReadOnlyLineEdit(regRead_line);
	setReadOnlyLineEdit(adcRead0_line);
	setReadOnlyLineEdit(adcRead1_line);

	regBtnWrite = new QPushButton("&Write", this);
	regBtnRead = new QPushButton("&Read", this);
	settingADCSavebutton = new QPushButton("FFT-ADC &Start");
	settingADCConfigbutton = new QPushButton("FFT-ADC &Stop");
	CFARCombo = new QComboBox;
	m3DFTCombo = new QComboBox;
	PowerCombo = new QComboBox;
	DiffCombo = new QComboBox;

	for (int i = 0; i < 8; i++){
		CFARCombo->addItem(QString::number(i));
	}

	for (int i = 0; i < power_index.size(); i++){
		PowerCombo->addItem(QString::number(power_index[i]));
	}

	m3DFTCombo->addItem(tr("0"));
	m3DFTCombo->addItem(tr("1"));

	DiffCombo->addItem(tr("0"));
	DiffCombo->addItem(tr("1"));

	controls_layout->addWidget( Power_label, 0, 2, Qt::AlignLeft);	
	controls_layout->addWidget( CFAR_label, 1, 2, Qt::AlignLeft);			
	controls_layout->addWidget( m3DFT_label, 2, 2, Qt::AlignLeft);			
	controls_layout->addWidget( diff_label, 3, 2, Qt::AlignLeft);	

	controls_layout->addWidget( PowerCombo, 0, 3, Qt::AlignLeft);	
	controls_layout->addWidget( CFARCombo, 1, 3, Qt::AlignLeft);			
	controls_layout->addWidget( m3DFTCombo, 2, 3, Qt::AlignLeft);			
	controls_layout->addWidget( DiffCombo, 3, 3, Qt::AlignLeft);	

	for(int i = 0; i < 4; i++){
		ctlWriteBtn_.emplace_back(new QPushButton("&Cfg", this));
		ctlReadBtn_.emplace_back(new QPushButton("&Read", this));
		ctlReadLine_.emplace_back(new QLineEdit);
		setReadOnlyLineEdit(ctlReadLine_[i]);
		controls_layout->addWidget( ctlWriteBtn_[i], i, 4, Qt::AlignLeft);	
		controls_layout->addWidget( ctlReadLine_[i], i, 5, Qt::AlignLeft);			
		controls_layout->addWidget( ctlReadBtn_[i], i, 6, Qt::AlignLeft);			
	}
	controls_layout->addWidget( regAddr_label, 0, 7, Qt::AlignLeft);
	controls_layout->addWidget( distanceOffset_label, 1, 7, Qt::AlignLeft);
	controls_layout->addWidget( regAddr_line, 0, 8, Qt::AlignLeft);	
	controls_layout->addWidget( distance_Offset_edit, 1, 8, Qt::AlignLeft);	
	controls_layout->addWidget( regVal_label, 0, 9, Qt::AlignLeft);	
	controls_layout->addWidget( regVal_line, 0, 10, Qt::AlignLeft);	
	controls_layout->addWidget( regBtnWrite, 0, 11, Qt::AlignLeft);	
	controls_layout->addWidget( regRead_line, 0, 12, Qt::AlignLeft);	
	controls_layout->addWidget( regBtnRead, 0, 13, Qt::AlignLeft);	
	controls_layout->addWidget( settingADCSavebutton, 2, 7, Qt::AlignLeft);
	controls_layout->addWidget( settingADCConfigbutton, 2, 8, Qt::AlignLeft);
	controls_layout->addWidget( saveBtn, 3, 7, Qt::AlignLeft);
	controlsBox->setLayout(controls_layout);


	QGroupBox *stateShowBox  = new QGroupBox(tr("State Conditon:"));
	QGridLayout* stateShowBoxLayout = new QGridLayout;
	QGroupBox *errorLogBox = new QGroupBox(tr("Error Log:"));
	QGridLayout* errorLogBoxLayout = new QGridLayout;
	QGroupBox *stateDisplayBox = new QGroupBox(tr("State Show:"));
	QGridLayout* stateDisplayBoxLayout = new QGridLayout;
	QLabel* devLabel0 = new QLabel("dev0 state");
	devLabel0_state = new QLabel("dev0");
	QLabel* devLabel1 = new QLabel("dev1 state");
	devLabel1_state = new QLabel("dev1");
	QLabel* devLabel2 = new QLabel("dev2 state");
	devLabel2_state = new QLabel("dev2");
	errorLogText = new QTextEdit(this);
	errorLogText->setReadOnly(true);
	setLED(devLabel0_state, 1);
	setLED(devLabel1_state, 2);	
	setLED(devLabel2_state, 3);	
	stateDisplayBoxLayout->addWidget(adc_label0, 0, 0, Qt::AlignLeft | Qt::AlignTop);
	stateDisplayBoxLayout->addWidget(adcRead0_line, 0, 1, Qt::AlignLeft | Qt::AlignTop);
	stateDisplayBoxLayout->addWidget(adc_label1, 1, 0, Qt::AlignLeft| Qt::AlignTop);
	stateDisplayBoxLayout->addWidget(adcRead1_line, 1, 1, Qt::AlignLeft| Qt::AlignTop);
	stateDisplayBoxLayout->addWidget(devLabel0, 0, 2, Qt::AlignLeft | Qt::AlignTop);
	stateDisplayBoxLayout->addWidget(devLabel0_state, 0, 3, Qt::AlignLeft | Qt::AlignTop);
	stateDisplayBoxLayout->addWidget(devLabel1, 1, 2, Qt::AlignLeft| Qt::AlignTop);
	stateDisplayBoxLayout->addWidget(devLabel1_state, 1, 3, Qt::AlignLeft| Qt::AlignTop);
	stateDisplayBoxLayout->addWidget(devLabel2, 2, 2, Qt::AlignLeft| Qt::AlignTop);
	stateDisplayBoxLayout->addWidget(devLabel2_state, 2, 3, Qt::AlignLeft| Qt::AlignTop);
	stateDisplayBox->setLayout(stateDisplayBoxLayout);
	errorLogBoxLayout->addWidget(errorLogText, 0, 0, Qt::AlignLeft | Qt::AlignTop);
	errorLogBox->setLayout(errorLogBoxLayout);
	stateShowBoxLayout->addWidget(stateDisplayBox, 0, 0, Qt::AlignLeft | Qt::AlignTop);
	stateShowBoxLayout->addWidget(errorLogBox, 0, 1, Qt::AlignLeft | Qt::AlignTop);
	stateShowBox->setLayout(stateShowBoxLayout);

	render_panel_ = new rviz::RenderPanel();
	selection_panel_ = new rviz::SelectionPanel();
	controls->addWidget(controlsBox, 0, 0, Qt::AlignLeft);
	controls->addWidget(stateShowBox, 0, 1, Qt::AlignLeft);
	//controls->addWidget(fileBox, 0, 2, Qt::AlignLeft);

	controls->setColumnStretch(0,4);
	for(int i = 1; i < 2;i++)
		controls->setColumnStretch(i,2);

	ctrlDockWidget->setLayout(controls);
	ctrlDock->setWidget(ctrlDockWidget);

#if 0
	QWidget* stateWidget = new QWidget();
	QGridLayout* stateLayout = new QGridLayout ;
	QLabel* devLabel0 = new QLabel("dev0 state");
	QLabel* devLabel0_state = new QLabel("dev0");
	QLabel* devLabel1 = new QLabel("dev1 state");
	QLabel* devLabel1_state = new QLabel("dev1");
	setLED(devLabel0_state, 1, 16);
	setLED(devLabel1_state, 2, 16);
	stateLayout->addWidget(devLabel0, 0, 0, Qt::AlignLeft);
	stateLayout->addWidget(devLabel0_state, 0, 1, Qt::AlignLeft);
	stateLayout->addWidget(devLabel1, 1, 0, Qt::AlignLeft);
	stateLayout->addWidget(devLabel1_state, 1, 1, Qt::AlignLeft);
	stateWidget->setLayout(stateLayout);
	QPalette pal(stateWidget->palette());
	pal.setColor(QPalette::Background, Qt::white);
	stateWidget->setAutoFillBackground(true);
	stateWidget->setPalette(pal);
	mainLayout->addWidget ( stateWidget, 1, 1, 5, 4, Qt::AlignLeft);
#endif
	mainLayout->addWidget ( ctrlDock, 0, 0);
	mainLayout->addWidget ( render_panel_, 1, 0, 5, 5);
	mainLayout->addWidget ( selection_panel_, 1, 0, 5, 1, Qt::AlignLeft);

	multiWidget->setLayout(mainLayout);
	this->addTab(multiWidget,  "Lidar Ui Mainwindow");
}

void viewpanel::setSaveFolder()
{

	save_folder_ =  QFileDialog::getExistingDirectory(this, "Choose folder for saving pc data", QDir::currentPath(),
		QFileDialog::DontUseNativeDialog | QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

	if( !save_folder_.isNull() )
	{
		qDebug() << "selected default recording folder : " << save_folder_.toUtf8();
	}
	else
	{
		save_folder_ = QString(".");
	}
}


void viewpanel::parseADCData(std::vector<uint8_t> &data)
{
	std::cout << "!!enter parseADCData! input point num is  "  <<  data.size() / 2 << std::endl;
#if 0
	std::string datPath;
	datPath = save_folder.toStdString() + "/data_index" + std::to_string(findex) +".dat";
	std::ofstream datfile; 
	datfile.open(datPath, std::ios::out | std::ios::binary); 
	for(int i = 0; i < data.size(); i++) {
		datfile << data[i];
	}
	datfile.close();
#endif
	int32_t cur_data  = 0;
	int index = 0;
	adcMsg* padc = NULL;
	adcMsg_free_buf_queue.get(padc);
	padc->dataADC0.clear();
	padc->dataADC1.clear();
	for(int i = 0; i < data.size(); i++) {	
		index += 1;
		if(i >= data.size() / 2) break;
		int flagNum = i % 2;
		cur_data += data[i] << (8 * (flagNum));
		if(flagNum){
			if(cur_data > SIGN_LIMIT_NUM){
				cur_data -= SIGN_OFFSET_NUM;
			}
			if(index % 4){
				padc->dataADC0.append(cur_data);
			}else{
				padc->dataADC1.append(cur_data);
			}
			cur_data = 0;
		}
	}
	adcMsg_done_buf_queue.put(padc);
	printf("adcMsg send finished\n");  //打印自己发送的信息
}


void viewpanel::parseFFTData(std::vector<uint8_t> &data)
{
	std::cout << "!!enter parseFFTData! input point num is  "  <<  data.size() / 4 << std::endl;
#if 0
	std::string datPath;
	datPath = save_folder.toStdString() + "/data_index" + std::to_string(findex) +".dat";
	std::ofstream datfile; 
	datfile.open(datPath, std::ios::out | std::ios::binary); 
	for(int i = 0; i < data.size(); i++) {
		datfile << data[i];
	}
	datfile.close();
#endif
	uint32_t cur_data  = 0;
	int index = 0;
	fftMsg* pfft = NULL;
	double power_offset_ = power_offset;
	fftMsg_free_buf_queue.get(pfft);
	pfft->dataFFT_0.clear();
	pfft->dataFFT_1.clear();
	pfft->dataFFTdB_0.clear();
	pfft->dataFFTdB_1.clear();
	for(int i = 0; i < data.size(); i++) {
		index += 1;
		if(index < 5)
			cur_data += data[i] << (8 * (index - 1));
		else if (index < 9)
			cur_data += data[i] << (8 * (index - 5));
		else if (index < 13)
			cur_data += data[i] << (8 * (index - 9));
		else if (index < 17)
			cur_data += data[i] << (8 * (index - 13));
		else if (index < 21)
			cur_data += data[i] << (8 * (index - 17));
		else if (index < 25)
			cur_data += data[i] << (8 * (index - 21));
		else if (index < 29)
			cur_data += data[i] << (8 * (index - 25));
		else if (index < 33)
			cur_data += data[i] << (8 * (index - 29));

		if(index % 4 == 0 && index < 33){
			if(i < data.size() / 2){
				pfft->dataFFT_0.append(cur_data);
				pfft->dataFFTdB_0.append(fft2dBm(cur_data) + power_offset_);
			} else{
				pfft->dataFFT_1.append(cur_data);	
				pfft->dataFFTdB_1.append(fft2dBm(cur_data) + power_offset_);
			}
			cur_data = 0;
		}
		if(index == 32) index = 0;

	}
	fftMsg_done_buf_queue.put(pfft);
	printf("fftMsg send finished\n");  //打印自己发送的信息
}

void viewpanel::Save2filecsv(std::vector<uint8_t> &data, bool ifsave)
{
	if(!ifsave) return;
	//memset(&curPcData, 0, sizeof(curPcData));
#if 0
	std::string datPath;
	datPath = save_folder.toStdString() + "/data_index" + std::to_string(findex) +".dat";
	std::ofstream datfile; 
	datfile.open(datPath, std::ios::out | std::ios::binary); 
	for(int i = 0; i < data.size(); i++) {
		datfile << data[i];
	}
	datfile.close();
#endif
	time_t rawtime;
	struct tm *ptminfo;
	time(&rawtime);
	ptminfo = localtime(&rawtime);
	printf("current: %02d-%02d-%02d %02d:%02d:%02d\n",
	ptminfo->tm_year + 1900, ptminfo->tm_mon + 1, ptminfo->tm_mday,
	ptminfo->tm_hour, ptminfo->tm_min, ptminfo->tm_sec);
#if 1
	std::string csvPath;
	csvPath = save_folder_.toStdString() + "/data_convert_" + 
	std::to_string(ptminfo->tm_year + 1900) + 
	"-" + std::to_string(ptminfo->tm_mon + 1) +
	"-" + std::to_string(ptminfo->tm_mday) +
	"-" + std::to_string(ptminfo->tm_hour) +
	"-" + std::to_string(ptminfo->tm_min) +
	"-" + std::to_string(ptminfo->tm_sec) +
	+".csv";
	ROS_INFO("csvPath is %s \n", csvPath.c_str());
	std::ofstream csvfile; 
	csvfile.open(csvPath, std::ios::out); 
	int32_t cur_data = 0;
	int index = 0;
	csvfile << "indensity_0" << "," << "indensity_1" << "," 
	<< "distance(m)" << "," << "speed(m/s)" << "," << "reserved" << "\n";	
	for(int i = 0; i < data.size(); i++) {
		index += 1;
		if(index < 5)
			cur_data += data[i] << (8 * (index - 1));
		else if (index < 9)
			cur_data += data[i] << (8 * (index - 5));
		else if (index < 12)
			cur_data += data[i] << (8 * (index - 9));
		else if (index < 16)
			cur_data += data[i] << (8 * (index - 12));

		if(index == 4 || index == 8){
			csvfile << cur_data << ",";	
			if(index == 4) 
				curPcPoint.indensity_0 = cur_data;
			else
				curPcPoint.indensity_1 = cur_data;
			cur_data = 0;
		}

		if(index == 11){
			curPcPoint.distance = (double)(cur_data / 65536.0) - distance_offset;
			csvfile << curPcPoint.distance << ",";	
			cur_data = 0;
		}

		if(index == 15){
			if(cur_data > 0x2000000){
				cur_data = cur_data - 0x4000000;
			}
			cur_data  =  cur_data / 2;
			curPcPoint.speed = (double)(cur_data / 65536.0);
			csvfile << curPcPoint.speed << ",";	
			cur_data = 0;
		}

		if(index == 16){
			curPcPoint.reserved = cur_data;
			csvfile << curPcPoint.reserved << "\n";	
			fmcwPointsData_->emplace_back(curPcPoint);
			memset(&curPcPoint, 0, sizeof(curPcPoint));
			cur_data = 0;
			index = 0;
		}
	}
	csvfile.close();
#endif
}


void viewpanel::saveData(){

	int ret = 0;
	bool ifsave = true;
	//while(!terminating && ifSave){
	distance_offset = distance_Offset_edit->text().toDouble();
	std::cout << " distance_offset is " << distance_offset << std::endl;

	cmdMsg_.mHead.usCommand = commandType::PC_READ;
	if(::write(ctrl_sock, &cmdMsg_, sizeof(commandMsg)) < 0){
		QMessageBox msgBox;
		msgBox.setText("pc data save failed!");
		msgBox.exec();
		return;
	}
	std::vector<uint8_t> mv;
	for(int i = 0; i < TCP_TIMES_PER_FRAME; i++){
		memset(&g_msg, 0, sizeof(g_msg));
		ret = ::recv(ctrl_sock, &g_msg, sizeof(g_msg), MSG_WAITALL);  //will be blocked until recv all data 
		if(ret < 0){
			ROS_INFO("saveData tcp recv failed, continue\n");
			sleep(1);
			i--;
			continue;
		}
		//std::cout << "receive byte is " << ret << std::endl;
		if(g_msg.cmdmsg.mHead.usCommand == commandType::PC_READ){
			//std::cout << "current index is " << i << ", tcp msg count is " << g_msg.cmdmsg.mCommandVal[1] << std::endl;
			if(g_msg.cmdmsg.mCommandVal[1] != i){
				std::cout << "!!!error!!! current index is " << i << ", tcp msg count is " << g_msg.cmdmsg.mCommandVal[1] << std::endl;
				ifsave  =false;
				QMessageBox msgBox;
				msgBox.setText("error!!TCP data loss, save task quit!");
				msgBox.exec();	
				break;			
			}
			mv.insert(mv.end(), g_msg.pcTcpData, g_msg.pcTcpData + TCP_PC_SIZE_SINGLE);
		}else {
			ROS_INFO("msg is %d, not pc data msg, continue\n", g_msg.cmdmsg.mHead.usCommand);
			cmdMsg_.mHead.usCommand = commandType::PC_READ;
			if(::write(ctrl_sock, &cmdMsg_, sizeof(commandMsg)) < 0){
				QMessageBox msgBox;
				msgBox.setText("pc data save failed!");
				msgBox.exec();
				return;
			}
			i--;
			continue;
		}		
	}
	Save2filecsv(mv, ifsave);
	//}
	saveBtn->setStyleSheet("color: black");
	saveBtn->setText("Save PC");
	if(ifsave){
		QMessageBox msgBox;
		msgBox.setText("save pc data successfully!");
		msgBox.exec();
	}
	ifSave = false;
	//vx_task_delete(&bst_task[0]);
}

// 该函数将label控件变成一个圆形指示灯，需要指定颜色color以及直径size
// color 0:grey 1:red 2:green 3:yellow
// size  单位是像素
void viewpanel::setLEDColor(QLabel* label, int color)
{
    QString background = "background-color:";
    switch (color) {
    case 0:
        // 灰色
        background += "rgb(190,190,190)";
        break;
    case 1:
        // 红色
        background += "rgb(255,0,0)";
        break;
    case 2:
        // 绿色
        background += "rgb(0,255,0)";
        break;
    case 3:
        // 黄色
        background += "rgb(255,255,0)";
        break;
    default:
        break;
    }
    const QString SheetStyle = background;
    label->setStyleSheet(SheetStyle);
}

void viewpanel::setLED(QLabel* label, int color)
{
    // 将label中的文字清空
	int size = 20;
    label->setText("");
    // 先设置矩形大小
    // 如果ui界面设置的label大小比最小宽度和高度小，矩形将被设置为最小宽度和最小高度；
    // 如果ui界面设置的label大小比最小宽度和高度大，矩形将被设置为最大宽度和最大高度；
    QString min_width = QString("min-width: %1px;").arg(size);              // 最小宽度：size
    QString min_height = QString("min-height: %1px;").arg(size);            // 最小高度：size
    QString max_width = QString("max-width: %1px;").arg(size);              // 最小宽度：size
    QString max_height = QString("max-height: %1px;").arg(size);            // 最小高度：size
    // 再设置边界形状及边框
    QString border_radius = QString("border-radius: %1px;").arg(size/2);    // 边框是圆角，半径为size/2
    QString border = QString("border:1px solid black;");                    // 边框为1px黑色
    // 最后设置背景颜色
    QString background = "background-color:";
    switch (color) {
    case 0:
        // 灰色
        background += "rgb(190,190,190)";
        break;
    case 1:
        // 红色
        background += "rgb(255,0,0)";
        break;
    case 2:
        // 绿色
        background += "rgb(0,255,0)";
        break;
    case 3:
        // 黄色
        background += "rgb(255,255,0)";
        break;
    default:
        break;
    }

    const QString SheetStyle = min_width + min_height + max_width + max_height + border_radius + border + background;
    label->setStyleSheet(SheetStyle);
}


void viewpanel::TaskFunc(void *arg){
    viewpanel *pSave = (viewpanel *)arg;

    if (pSave && pSave->TaskFunc) {
        pSave->saveData();
    }
}

void viewpanel::TaskFuncUdpRecv(void *arg){
    viewpanel *pSave = (viewpanel *)arg;

    if (pSave && pSave->TaskFuncUdpRecv) {
        pSave->udpRecvLoop();
    }
}

void viewpanel::TaskFuncUdpParse(void *arg){
    viewpanel *pSave = (viewpanel *)arg;
	std::cout << "enter TaskFuncUdpParse" << std::endl;
    if (pSave && pSave->TaskFuncUdpParse) {
        pSave->udpParseLoop();
    }
}

void viewpanel::start_save_task()
{

}
void viewpanel::saveDataThead()
{
	if(!ifSave && ifStarted){
		saveBtn->setStyleSheet("color: green");
		saveBtn->setText("Saving..");
		ifSave = true;
	}else {
		saveBtn->setStyleSheet("color: black");
		saveBtn->setText("Save PC");
		ifSave = false;
		return;		
	}
	//usleep(100*1000);
	saveData();
#if 0
    vx_task_set_default_create_params(&bst_params);
    bst_params.app_var = this;
    bst_params.task_mode = 0;
    bst_params.task_main = TaskFunc;
    vx_task_create(&bst_task[0], &bst_params);    
#endif

#if 0
	pthread_t save_thread_id;
	usleep(100*1000);
	if(pthread_create(&save_thread_id, NULL, saveData, NULL) < 0)
	{
		printf("create saveDataThead failed erron= %d/n",errno);
		QMessageBox msgBox;
		msgBox.setText("create saveDataThead failed!");
		msgBox.exec();
	};
#endif
}

void viewpanel::udpClose(){
	cmdMsg_.mHead.usCommand = commandType::FFT_ADC_READ_STOP;
	if(::write(ctrl_sock, &cmdMsg_, sizeof(commandMsg)) < 0){
		QMessageBox msgBox;
		msgBox.setText("UDP close failed!");
		msgBox.exec();
		return;
	}
	::close(udpRecvSocketFd_);
	udpStop_ = true;
	vx_task_delete(&bst_task[1]);
	QMessageBox msgBox;
	msgBox.setText("UDP close success!");
	msgBox.exec();
}

void viewpanel::singleFFT() {
	for(int i = 0 ; i < 2; i++){
		pFFTchart[i]->setSingleShow(true);
		pFFTchart[i]->setContineFlag(false);
	}
}

void viewpanel::singleADC() {
	for(int i = 0 ; i < 2; i++){
		pADCchart[i]->setSingleShow(true);
		pADCchart[i]->setContineFlag(false);
	}
}

void viewpanel::resetFFT() {
	power_offset = power_Offset_edit->text().toDouble();
	for(int i = 0 ; i < 2; i++){
		pFFTchart[i]->setSingleShow(false);
		pFFTchart[i]->setContineFlag(true);
		pFFTchart[i]->setIfScale(true);
	}
}

void viewpanel::resetADC() {
	for(int i = 0 ; i < 2; i++){
		pADCchart[i]->setSingleShow(false);
		pADCchart[i]->setContineFlag(true);
		pADCchart[i]->setIfScale(true);
	}
}

void viewpanel::updateFFTdata() {

#if DEBUG_UI	
	y_FFT.clear();
	y_FFT_1.clear();
	QVector<double> y_FFT_dB;
	QVector<double> y_FFT_1_dB;
	for(int i = 0; i< 8192; i++) 
	{
		double tmp = qrand() % 100000;
		double tmp_log = 10 * log10(tmp);
		y_FFT.append(tmp);
		y_FFT_dB.append(tmp_log);
		y_FFT_1.append(tmp);
		y_FFT_1_dB.append(tmp_log);
	}

	if(!ifShowdB_){
		pFFTchart[0]->setData(x_FFT, y_FFT);
		pFFTchart[1]->setData(x_FFT_1, y_FFT_1);

	} else {
		pFFTchart[0]->setData(x_FFT, y_FFT_dB);
		pFFTchart[1]->setData(x_FFT_1, y_FFT_1_dB);
	}
#else 
	if(!fftMsg_done_buf_queue.empty()){
		fftMsg* pfft = NULL;
		fftMsg_done_buf_queue.get(pfft);
		if(ifShowdB_ == FFT_ORI){
			pFFTchart[0]->setData(x_FFT, pfft->dataFFT_0);
			pFFTchart[1]->setData(x_FFT_1, pfft->dataFFT_1);

		} else if(ifShowdB_ == FFT_DB){
			pFFTchart[0]->setData(x_FFT, pfft->dataFFTdB_0);
			pFFTchart[1]->setData(x_FFT_1, pfft->dataFFTdB_1);
		}
		fftMsg_free_buf_queue.put(pfft);
	}
#endif

}

void viewpanel::updateState()
{
	static bool update = true;
	if(update){
		setLED(devLabel0_state, 1);
		setLED(devLabel1_state, 2);	
		setLED(devLabel2_state, 3);
		update = false;
	} else {
		setLED(devLabel0_state, 0);
		setLED(devLabel1_state, 0);	
		setLED(devLabel2_state, 0);		
		update = true;
	}	
}


void viewpanel::printErrorLog()
{
	std::string log = "FMCW Lidar host:\n";
	QString mlog = QString::fromStdString(log);
	errorLogText->insertPlainText(mlog);
}

void viewpanel::slotTextTcpChanged()
{
   errorLogText->moveCursor(QTextCursor::End);
}

void viewpanel::updateADCdata() {
	static long long frame_index = 0;
	x_adc0.clear();
	x_adc1.clear();
	for(int i = 0; i< 8192; i++) {
		x_adc0.append(i + 8192 * frame_index);
	}
	x_adc1 = x_adc0;

#if DEBUG_UI	
	y_adc0.clear();
	y_adc1.clear();
	for(int i = 0; i< 8192; i++) {
		double tmp = qrand() % 10000;
		y_adc0.append(tmp);
		y_adc1.append(tmp);
	}
	pADCchart[0]->setData(x_adc0, y_adc0);
	pADCchart[1]->setData(x_adc1, y_adc1);
#else 
	if(!adcMsg_done_buf_queue.empty()){
		adcMsg* padc = NULL;
		adcMsg_done_buf_queue.get(padc);
		pADCchart[0]->setData(x_adc0, padc->dataADC0);
		pADCchart[1]->setData(x_adc1, padc->dataADC1);
		adcMsg_free_buf_queue.put(padc);
	}
#endif
	//pADCchart[0]->setXChart(8192 * frame_index, 8192 * frame_index + 8191);
	//pADCchart[1]->setXChart(8192 * frame_index, 8192 * frame_index + 8191);
	frame_index++;
}

void viewpanel::udpConnect() {

	if(udpStop_){
		commandMsg cmdMsg;
		memset(&cmdMsg, 0, sizeof(commandMsg));
		cmdMsg.mHead.usCommand = commandType::FFT_ADC_READ_START;
		if(::write(ctrl_sock, &cmdMsg, sizeof(commandMsg)) < 0){
			QMessageBox msgBox;
			msgBox.setText("UDP Connect failed!");
			msgBox.exec();
			return;
		}		
		vx_task_set_default_create_params(&bst_params);
		bst_params.app_var = this;
		bst_params.task_mode = 0;
		bst_params.task_main = TaskFuncUdpRecv;
		vx_task_create(&bst_task[1], &bst_params);  

		vx_task_set_default_create_params(&bst_params);
		bst_params.app_var = this;
		bst_params.task_mode = 0;
		bst_params.task_main = TaskFuncUdpParse;
		vx_task_create(&bst_task[2], &bst_params);  
	}else{
		QMessageBox msgBox;
		msgBox.setText("UDP Connection is already working, please stop it first !");
		msgBox.exec();		
	}
}

void viewpanel::udpParseLoop()
{
	std::cout << "enter udpParseLoop main" << std::endl;
	while(!terminating)
	{
		ROS_INFO("============enter udpParseLoop");
		udp_ADC_FFT_Msg* pmsg = nullptr;
		if(udpMsg_done_buf_queue.get(pmsg) == 0){
			parseFFTData(pmsg->fftDataV);
			parseADCData(pmsg->adcDataV);
			udpMsg_free_buf_queue.put(pmsg);
		}else{
			std::cout << "warning!!udpMsg_done_buf_queue get timeout!!!" << std::endl;
		}
		if(udpStop_) break;
	}
	std::cout << "quit udpParseLoop" << std::endl;
}
void viewpanel::udpRecvLoop(){

	//int client_fd;
	lidar_UDP_port = udp_port_edit->text().toInt();
	struct sockaddr_in ser_addr;

	udpRecvSocketFd_ = socket(AF_INET, SOCK_DGRAM, 0);
	if(udpRecvSocketFd_ < 0)
	{
		QMessageBox msgBox;
		msgBox.setText("udp socket create fail!");
		msgBox.exec();
		return;
	}

	struct timeval timeout_recv = {3, 0};
	setsockopt(udpRecvSocketFd_, SOL_SOCKET, SO_RCVTIMEO, &timeout_recv, sizeof(timeout_recv)); //recv timeout

	memset(&ser_addr, 0, sizeof(ser_addr));
	ser_addr.sin_family = AF_INET;
	//ser_addr.sin_addr.s_addr = inet_addr(lidar_ip.c_str());
	ser_addr.sin_addr.s_addr = htonl(INADDR_ANY);  //注意网络序转换
	ser_addr.sin_port = htons(lidar_UDP_port);  //注意网络序转换
	std::cout << "lidar_UDP_port is " << lidar_UDP_port << std::endl;

    int ret = ::bind(udpRecvSocketFd_, (struct sockaddr*)&ser_addr, sizeof(ser_addr));
    if(ret < 0)
    {
		QMessageBox msgBox;
		msgBox.setText("udp socket bind fail!");
		msgBox.exec();
		return;
    }
	udpStop_ = false;
	socklen_t len;
	struct sockaddr_in src;
	//printf("ready recv udp msg!\n");
	len = sizeof(sockaddr);
	std::vector<uint8_t> fftDataV;
	std::vector<uint8_t> adcDataV;
	uint32_t last_frame_index = 0;
	bool ifLost  = false;
	while(!terminating && !udpStop_)
	{
		fftDataV.clear();
		adcDataV.clear();
		ifLost  = false;
		for(int i = 0; i < UDP_TIMES_PER_FRAME; i++){
			memset(&g_udpMsg, 0, sizeof(g_udpMsg));
			//printf("ready recv udp msg!\n");
			ret = recvfrom(udpRecvSocketFd_, &g_udpMsg, sizeof(g_udpMsg), MSG_WAITALL, (struct sockaddr*)&src, &len);  //接收来自server的信息
			//printf("recv udp msg! receive byte is %d, g_udpMsg: %c %c %c %c %c\n", ret, g_udpMsg.pcUdpData[11], \
			//g_udpMsg.pcUdpData[13], g_udpMsg.pcUdpData[15], g_udpMsg.pcUdpData[17], g_udpMsg.pcUdpData[19]);
			if(ret <= 0){
				if(udpStop_) {
					printf("fftMsg udp  quit!\n"); 
    				::close(udpRecvSocketFd_);
					return;
				}
				ROS_INFO("fft data recv failed, continue\n");
				usleep(100*1000);
				i--;
				continue;
			}
			if(g_udpMsg.mHead.usRollingCounter != i) {
				ifLost = true;
				std::cout << "!!!warnning!!! current index is " << i << ", udp msg usRollingCounter is " << g_udpMsg.mHead.usRollingCounter << std::endl;
				break;			
			}

			if(i == 0) last_frame_index = g_udpMsg.mHead.usFrameCounter;

			if(g_udpMsg.mHead.usFrameCounter != last_frame_index) {
				ifLost = true;
				std::cout << "!!!warnning!!! current usFrameCounter is " << g_udpMsg.mHead.usFrameCounter
				 << ", last_frame_index is " << last_frame_index << std::endl;	
				break;
			}

			if(i < UDP_TIMES_PER_FRAME / 2) {
				for(int j = 0; j < 16; j++){
					fftDataV.insert(fftDataV.end(), g_udpMsg.pcUdpData + 32 + 64 * j, g_udpMsg.pcUdpData + 64 * (j + 1));
					adcDataV.insert(adcDataV.end(), g_udpMsg.pcUdpData + 64 * j, g_udpMsg.pcUdpData + 64 * j + 32);
				}
			}
		}
		if(ifLost) {
			std::cout << "warning!!! lost data continue! "  << std::endl;
			continue;
		}
		std::cout << "!!recv udp pkg successfully! "  << std::endl;
		udp_ADC_FFT_Msg* pUdp = NULL;
		if(udpMsg_free_buf_queue.get(pUdp) == 0){
			pUdp->fftDataV = fftDataV;
			pUdp->adcDataV = adcDataV;
			udpMsg_done_buf_queue.put(pUdp);	
		}else{
			std::cout << "error!!! udpMsg_free_buf_queue timeout!! "  << std::endl;
			return;
		}
	
#if 0
		parseFFTData(fftDataV);
#endif
	}
	printf("fftMsg udp  quit!\n");  //打印自己发送的信息
 
    ::close(udpRecvSocketFd_);
}

int viewpanel::lidarConnect()
{
	int err;
	int one = 1;

	lidar_ip = ip_edit->text().toStdString();
	lidar_ctrl_port = port_edit->text().toInt();
	ROS_INFO("lidar_ip is %s, lidar_ctrl_port is %d", lidar_ip.c_str(), lidar_ctrl_port);


	struct sockaddr_in ctrl_serv_addr;
	struct sockaddr_in data_serv_addr;
	struct sockaddr_in logs_serv_addr;
	//pthread_t eth_logs_thread_id;

	// create TCP socket that will be used to control the rada
	if ((ctrl_sock=socket(AF_INET, SOCK_STREAM, 0))==-1){
		ROS_DEBUG("ERROR: Could not create socket!");
		return -1;
	}

	int nRecvBuf=320 * 1024;//设置为32K
	setsockopt(ctrl_sock, SOL_SOCKET,SO_RCVBUF,(const char*)&nRecvBuf,sizeof(int));

	int nSendBuf=320 * 1024;//设置为32K
	setsockopt(ctrl_sock, SOL_SOCKET, SO_SNDBUF,(const char*)&nSendBuf,sizeof(int));
	setsockopt(ctrl_sock, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));


#if 1
	struct timeval timeout_send = {2, 0};
	setsockopt(ctrl_sock, SOL_SOCKET, SO_SNDTIMEO, &timeout_send, sizeof(timeout_send)); //send timeout

	struct timeval timeout_recv = {2, 0};
	setsockopt(ctrl_sock, SOL_SOCKET, SO_RCVTIMEO, &timeout_recv, sizeof(timeout_recv)); //recv timeout

#endif
	memset(&ctrl_serv_addr, 0, sizeof(ctrl_serv_addr));
	ctrl_serv_addr.sin_family = AF_INET;
	ctrl_serv_addr.sin_addr.s_addr = inet_addr(lidar_ip.c_str());
	ctrl_serv_addr.sin_port = htons(lidar_ctrl_port);

	if(::connect(ctrl_sock, (struct sockaddr*)&ctrl_serv_addr, sizeof(ctrl_serv_addr))<0)
	{
		ROS_INFO("Failed to connect to lidar_ip %s", lidar_ip.c_str());
		return -2; /* Completely fail only if first radar didn't connect */
	}
	//fcntl(ctrl_sock, F_SETFL, O_NONBLOCK); /* Set the socket to non blocking mode */
	return 0;
}


void viewpanel::load_settings()
{
	QSettings settings(QCoreApplication::organizationName(),
		QCoreApplication::applicationName());

	lidar_ip_ = settings.value("IP Addr","127.0.0.1").toString();
	lidar_ctrl_port_ = settings.value("TCP Port","5000").toString();
	lidar_UDP_port_ = settings.value("UDP Port","8000").toString();
	distance_offset_ = settings.value("Distance Offset","0.0").toString();
	power_offset_ = settings.value("Power Offset","0.0").toString();
	save_folder_ = settings.value("Save Folder",".").toString();
}


void viewpanel::setReadOnlyLineEdit(QLineEdit* line)
{
	line->setReadOnly(true);
	QPalette palette = line->palette();
	palette.setBrush(QPalette::Base,palette.brush(QPalette::Disabled, QPalette::Base));
	line->setPalette(palette);
}

void viewpanel::save_settings(void )
{
	ROS_INFO("enter save_settings");
	ROS_DEBUG("enter save_settings");
	QSettings settings(QCoreApplication::organizationName(),
		QCoreApplication::applicationName());

	lidar_ip = ip_edit->text().toStdString();
	lidar_ctrl_port = port_edit->text().toInt();
	lidar_UDP_port = udp_port_edit->text().toInt();
	distance_offset = distance_Offset_edit->text().toInt();
	power_offset = power_Offset_edit->text().toInt();

	lidar_ip_ = QString::fromStdString(lidar_ip);
	lidar_ctrl_port_ = QString::number(lidar_ctrl_port);
	lidar_UDP_port_ = QString::number(lidar_UDP_port);
	distance_offset_ = QString::number(distance_offset);
	power_offset_ = QString::number(power_offset);

	settings.setValue("IP Addr", lidar_ip_);
	settings.setValue("TCP Port", lidar_ctrl_port_);
	settings.setValue("UDP Port", lidar_UDP_port_);
	settings.setValue("Distance Offset", distance_offset_);
	settings.setValue("Power Offset", power_offset_);
	settings.setValue("Save Folder", save_folder_);

}
