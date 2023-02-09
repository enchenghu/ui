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
//#include "Per_radar_settings.hpp"

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
//#include "ctkrangeslider.h"
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

static QStringList motorDataString = {
	"speed",
	"angle",
	"speed P",
	"speed I",
	"IQ",
	"ID",
	"IQ tar",
	"ID tar",
	"time",
	"error time"
};
static QStringList regAddrList = {
	"0xa00a0014",
	"0xa00a0018",
	"0xa00a0004",
	"0xa0070010"
};

static QStringList regValueList = {
	"0xFFFF00B4",
	"0x0A6CFC8F",
	"0x91EC0001",
	"0X40001183"
};

//色表构建
static std::vector<unsigned char> R_V_g = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
											0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
											0, 20, 40, 60, 80, 100, 120, 140, 160, 180, 200, 220, 240,
											255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255 };

static std::vector<unsigned char> G_V_g = { 0, 20, 40, 60, 80, 100, 120, 140, 160, 180, 200, 220, 240,
											255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
											255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255 ,
											240, 220, 200, 180, 160, 140, 120, 100, 80, 60, 40, 20, 0};

static std::vector<unsigned char> B_V_g = { 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
											240, 220, 200, 180, 160, 140, 120, 100, 80, 60, 40, 20, 0,
											0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ,
											0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

float UnsignedChar4ToFloat(unsigned char *strBuf) 
{
	float fNum;
	char *p = (char *)&fNum;
	for (int i = 0; i < 4; i++) {
		*p = strBuf[i];
		p++;
	}
	return fNum;
}

void FloatToChar(float fNum, unsigned char *strBuf) 
{
	int i = 0;
	unsigned char nTmp;
	char *p = (char *)&fNum;
	for (i = 0; i < 4; i++) {
		strBuf[i] = *p;
		p++;
	}
}
/* Constructor for the viewpanel. */
viewpanel::viewpanel(QTabWidget* parent )
	: QTabWidget( parent ), ifConnected(false), ifSave(false), \
	save_folder_(QString(".")), udpStop_(true), ifShowdB_(FFT_ORI),\
	power_offset(0.0), distance_offset(0.0),ifConnectedMotor(false),\
	ifOpenMotor(false), udpPCStop_(true), udpPCContinu_(true), udpPCSingle_(false),\
	ifStarted(false)
{
	init_queue();
	memset(&cmdMsg_, 0, sizeof(cmdMsg_));
	memset(&motorMsgSend_, 0, sizeof(motorMsgSend_));
	cmdMsg_.mHead.usPrefix = 0xeeff;
	cmdMsg_.mHead.usType = 0x10;
	cmdMsg_.mHead.usPayloadCrc = 0x00;
	cmdMsg_.mHead.unLength = 12;
	motorMsgSend_.mHead = 0x55aa;
	motorMsgSend1_.mHead = 0x55aa;
	motorMsgPid_.mHead = 0x55aa;
	motorMsgShow_.mHead = 0x55aa;
	motorMsgWorkMode_.mHead = 0x55aa;
	motorMsgShowCycle_.mHead = 0x55aa;
	distance_min  = 0.0;
	distance_max = 0.0;
	indensity_min = 0.0;
	indensity_max = 0.0;
	speed_min = 0.0;
	speed_max = 0.0;

	power_index = {0, 12, 20, 70, 290, 346, 347, 
				   397, 415, 500, 510, 560, 625, 1000, 
				   1130, 1450, 1580, 1600, 3000, 20000};
	load_settings();
	CreatUIWindow();
	CreatDebugWindow();
	CreatADCWindow();
	CreatMotorWindow();
	CreatConnect();

	registerPointcloudRviz();
	resize(QDesktopWidget().availableGeometry(this).size() * 0.85);
	std::cout << "R: " << R_V_g.size() << " G: " << G_V_g.size() <<  " B: " << B_V_g.size() << std::endl;
}

// Destructor.
viewpanel::~viewpanel()
{
	if(!udpStop_) udpClose();
	if(!udpPCStop_) udpPcClose();
	delete manager_;
	save_settings();
	::close(udpRecvSocketFd_);
	::close(udpRecvPCSocketFd_);
	::close(ctrl_sock);
	serialClose(m_serialPort);	
	serialClose(m_serialPort_test);	
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
	udpPcMsg_free_buf_queue.setParam("udpPcMsg_free_buf_queue", MAX_BUFF_LEN);
    udpPcMsg_done_buf_queue.setParam("udpPcMsg_done_buf_queue", MAX_BUFF_LEN); 
    for (int loop = 0; loop < 4; loop++) {
        fftMsg *fbuf0 = &fftBuff[loop];
        adcMsg *fbuf1 = &adcBuff[loop];
        udp_ADC_FFT_Msg *fbuf2 = &udpFABuff[loop];
        udpPcMsgOneFrame *fbuf3 = &udpPCBuff[loop];
        fftMsg_free_buf_queue.put(fbuf0);
        adcMsg_free_buf_queue.put(fbuf1);
        udpMsg_free_buf_queue.put(fbuf2);
        udpPcMsg_free_buf_queue.put(fbuf3);

    }
	vertical_bin = 1 / 256.0; 
	speed_bin = 1 / 128.0; 
	horizontal_bin = 360.0 / 32000.0 * 2; 
	distance_bin = 1 / 65536.0; 
	vertical_offset = -2.5;
	udpPCBuff_last.pcDataOneFrame.clear();
	udpPCBuff_last.frameCounter.clear();
	motorSerialConnectTest();
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
		if (rosbag_recording == 1)
		{
		record_button->setStyleSheet("color: red");
		record_button->setText("Rec&ording");
	}
	}
	else
	{
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
			//configRegLidar();
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
	if(str.toDouble() < 0.0 || str.toDouble() > 50000.0){
		QMessageBox msgBox;
		msgBox.setText("input power invaild!");
		msgBox.exec();
		return;		
	}
	cmdMsg_.mCommandVal[0] = str.toInt();
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

int viewpanel::configRegLidar(void)
{
	std::string addr = "0xa00a0004";
	std::string value = "0x91ec0001";

	std::stringstream ss;
	ss << std::hex << addr;
	ss >> cmdMsg_.mCommandVal[0];

	std::stringstream tt;
	tt << std::hex << value;
	tt >> cmdMsg_.mCommandVal[1];

	cmdMsg_.mHead.usCommand = commandType::REG_WRITE;
	std::cout << "cmdMsg_.regAddr is " << cmdMsg_.mCommandVal[0] << " cmdMsg_.regVal " << cmdMsg_.mCommandVal[1] << std::endl;
	if(::write(ctrl_sock, &cmdMsg_, sizeof(commandMsg)) < 0){
		QMessageBox msgBox;
		msgBox.setText("config lidar register failed!");
		msgBox.exec();
		return -1;
	}

	return 0;

}
void viewpanel::configReg(int index){
	QString strAddr = regAddr_line[index]->text();
	QString strValue = regVal_line[index]->text();
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
void viewpanel::readReg(int index){
	cmdMsg_.mHead.usCommand = commandType::REG_READ;
	QString strAddr = regAddr_line[index]->text();
	std::stringstream ss;
	ss << std::hex << strAddr.toStdString();
	ss >> cmdMsg_.mCommandVal[0];

	if(::write(ctrl_sock, &cmdMsg_, sizeof(commandMsg)) < 0){
		regBtnRead[index]->setStyleSheet("color: black");	
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
				regRead_line[index]->setText(QString::fromStdString(tmp));
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
	/* Initialize the main RViz classes */
	manager_ = new rviz::VisualizationManager( render_panel_ );

	//render_panel_->setBackgroundColor( Ogre::ColourValue(238,238,236,0.3));
	render_panel_->setBackgroundColor(Ogre::ColourValue(238,238,236,0.3));

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
	manager_->setFixedFrame("image_lidar");

	//Car_ = manager_->createDisplay( "rviz/Marker", "Marker", true );
	//ROS_ASSERT( Car_ != NULL );
	//Car_->subProp("Marker Topic")->setValue("/arbe/rviz/car_marker");

	FloatingText_ = manager_->createDisplay( "rviz/Marker", "Marker", true );
	ROS_ASSERT( FloatingText_ != NULL );
	FloatingText_->subProp("Marker Topic")->setValue("/arbe/rviz/floatingText_marker");

	Axes_ = manager_->createDisplay( "rviz/Axes", "Axes", true );
	ROS_ASSERT( Axes_ != NULL );
	Axes_->subProp("Reference Frame")->setValue("odom");
	Axes_->subProp("Length")->setValue("0.2");
	Axes_->subProp("Radius")->setValue("0.03");
	Axes_->subProp("Alpha")->setValue("0.8");

	std::string pointcloud_topic = "/fmcw/rviz/pointcloud";
	ros::NodeHandle fmcw_pcl("~");// = boost::make_shared<ros::NodeHandle>();

	fmcw_pcl_pub = fmcw_pcl.advertise<sensor_msgs::PointCloud2>(pointcloud_topic, 1);

	//std::string stationary_pointcloud_topic = "/fmcw/rviz/stationary_pointcloud";
	ROS_INFO("Registering new pointcloud topic: %s",pointcloud_topic.c_str());	
	//ROS_INFO("Registering new pointcloud topic: %s",stationary_pointcloud_topic.c_str());

	pointcloud_fmcw = manager_->createDisplay( "rviz/PointCloud2", "PointCloud2", true );
	ROS_ASSERT(pointcloud_fmcw!=NULL);
	pointcloud_fmcw->subProp("Topic")->setValue( pointcloud_topic.c_str() );
	pointcloud_fmcw->subProp("Style")->setValue("Spheres");
	pointcloud_fmcw->subProp("Size (Pixels)")->setValue("3");
	pointcloud_fmcw->subProp("Size (m)")->setValue("0.015");

	//pointcloud_fmcw->subProp("Decay Time")->setValue((float)DetectionMemoryTime / 1000);

#if 1
	pointcloud_fmcw->subProp("Color Transformer")->setValue("RGB8");
	pointcloud_fmcw->subProp("Invert Rainbow")->setValue("false");
	pointcloud_fmcw->subProp("Position Transformer")->setValue("XYZ");
	pointcloud_fmcw->subProp("Use Fixed Frame")->setValue("true");
	pointcloud_fmcw->subProp( "Axis" )->setValue( "Z" );
#endif
}

void viewpanel::ctrlShowWindows(bool show)
{
	ctrlDock->setVisible(show);
}

void viewpanel::ctrlPcPanel(bool show)
{
	selection_panel_->setVisible(show);
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


void viewpanel::CreatMotorWindow()
{
	QWidget* multiWidget_new = new QWidget();
	QGridLayout* motorCharts= new QGridLayout;

	QGroupBox *chartMotorBox = new QGroupBox(tr("Motor chart"));
	QGroupBox *chartChooseBox = new QGroupBox(tr("choose to show"));

	QGridLayout* chartMotorBoxLayout = new QGridLayout ;
	QGridLayout* chartChooseBoxLayout = new QGridLayout ;
	motorChartResetBtn = new QPushButton("&Reset Chart");
	motorChartSingleBtn = new QPushButton("&Single");
	for(int i = 0; i < motorDataString.size(); i++){
		QCheckBox* newBox = new QCheckBox(motorDataString[i], this);
		checkShowV.push_back(newBox);
		chartChooseBoxLayout->addWidget(checkShowV[i], i, 0, Qt::AlignLeft);
		//checkShowV.push_back(tmp);
	}
	chartChooseBoxLayout->addWidget(motorChartResetBtn, 0, 1, Qt::AlignLeft);
	chartChooseBoxLayout->addWidget(motorChartSingleBtn, 1, 1, Qt::AlignLeft);

	chartChooseBox->setLayout(chartChooseBoxLayout);

	pMotorchart = new ChartFFT(this);
	pMotorchart->setShowType(ADC_ORI);

	chartMotorBoxLayout->addWidget(pMotorchart->setChart(0, 8192, -32768, 32768), 0 , 0);
	chartMotorBoxLayout->addWidget(chartChooseBox, 0 , 1);
	
	chartMotorBoxLayout->setColumnStretch(0, 9);
	chartMotorBoxLayout->setColumnStretch(1, 1);

	chartMotorBox->setLayout(chartMotorBoxLayout);

	motorCharts->addWidget(chartMotorBox, 0, 0);
	//adcCharts->addWidget(chartADCBox1);
	
	QGridLayout* motorDebugLayout = new QGridLayout ;
	QGroupBox* motorDebugBox = new QGroupBox(tr("Motor Debug:"));
	QGridLayout* motorDebugBoxLayout = new QGridLayout ;

	QGroupBox* motorControlBox = new QGroupBox(tr("Motor Control:"));
	QGridLayout* motorControlBoxLayout = new QGridLayout ;

	QGroupBox* motorStateBox = new QGroupBox(tr("Motor State:"));
	QGridLayout* motorStateBoxLayout = new QGridLayout ;
	

	motorConnectBtn = new QPushButton("&Connect");
	motorSwitchBtn = new QPushButton("&Open");
	motorWorkModeSetBtn = new QPushButton("&Set");
	motorPidSetBtn = new QPushButton("&Set");
	motorShowCycleSetBtn = new QPushButton("&Set");
	motorWorkModeCombo  = new QComboBox(this);
	motorSerialCombo = new QComboBox(this);
	motorSerialCombo->setEditable(true);
	foreach(const QSerialPortInfo &info,QSerialPortInfo::availablePorts())
	{
		m_serialPortName << info.portName();
		qDebug()<<"serialPortName:"<<info.portName();
		motorSerialCombo->addItem(info.systemLocation());
	}
	motorWorkModeCombo->addItem(QString("speed"));
	motorWorkModeCombo->addItem(QString("location"));
	motorWorkModeCombo->addItem(QString("phase-lock"));

	motorSerialCombo->addItem(QString("/dev/pts/3"));
	motorConnectPortLine = new QLineEdit(this);
	motorWorkModeAngleSetLine = new QLineEdit(this);
	motorWorkModeSpeedSetLine = new QLineEdit(this);
	motorWorkModeLocSetLine = new QLineEdit(this);

	motorPidCycleSetLine = new QLineEdit(this);
	motorPidPSetLine = new QLineEdit(this);
	motorPidISetLine = new QLineEdit(this);
	motorPidDSetLine = new QLineEdit(this);

	motorShowCycleSetLine = new QLineEdit(this);

	QLabel* tcpPortLabel = new QLabel("TCP Port:" );
	QLabel* serialLabel = new QLabel("Serial Device:" );

	QLabel* workModeLabel = new QLabel("Work Mode:" );
	QLabel* speedLabel = new QLabel("Rotating Speed:" );
	QLabel* angleLabel = new QLabel("Motor Angle:" );
	QLabel* locLabel = new QLabel("Motor Location:" );
	QLabel* pidCLabel = new QLabel("Pid Cycle:" );
	QLabel* pidPLabel = new QLabel("Pid P:" );
	QLabel* pidILabel = new QLabel("Pid I:" );
	QLabel* pidDLabel = new QLabel("Pid D:" );
	QLabel* showCycleLabel = new QLabel("DisPlay Cycle:" );

#if 0
	tcpPortLabel ->setAlignment(Qt::AlignHCenter);
	workModeLabel ->setAlignment(Qt::AlignHCenter);
	speedLabel ->setAlignment(Qt::AlignHCenter);
	angleLabel ->setAlignment(Qt::AlignHCenter);
	locLabel ->setAlignment(Qt::AlignHCenter);
	pidCLabel ->setAlignment(Qt::AlignHCenter);
	pidPLabel ->setAlignment(Qt::AlignHCenter);
	pidILabel ->setAlignment(Qt::AlignHCenter);
	pidDLabel ->setAlignment(Qt::AlignHCenter);
	showCycleLabel ->setAlignment(Qt::AlignHCenter);
#endif

	motorControlBoxLayout->addWidget(tcpPortLabel, 0, 0, Qt::AlignLeft);
	motorControlBoxLayout->addWidget(motorConnectPortLine, 0, 1, Qt::AlignLeft);
	motorControlBoxLayout->addWidget(serialLabel, 1, 0, Qt::AlignLeft);
	motorControlBoxLayout->addWidget(motorSerialCombo, 1, 1, Qt::AlignLeft);
	motorControlBoxLayout->addWidget(motorConnectBtn, 2, 0, Qt::AlignLeft);
	motorControlBoxLayout->addWidget(motorSwitchBtn, 2, 1, Qt::AlignLeft);

	QGroupBox* workModeBox = new QGroupBox(tr("Work Mode:"));
	QGridLayout* workModeBoxLayout = new QGridLayout ;
	workModeBoxLayout->addWidget(workModeLabel, 0, 0, Qt::AlignLeft | Qt::AlignTop);
	workModeBoxLayout->addWidget(motorWorkModeCombo, 0, 1, Qt::AlignLeft | Qt::AlignTop);
	workModeBoxLayout->addWidget(speedLabel, 0, 2, Qt::AlignLeft | Qt::AlignTop);
	workModeBoxLayout->addWidget(motorWorkModeSpeedSetLine, 0, 3, Qt::AlignLeft | Qt::AlignTop);
	workModeBoxLayout->addWidget(angleLabel, 0, 4, Qt::AlignLeft | Qt::AlignTop);
	workModeBoxLayout->addWidget(motorWorkModeAngleSetLine, 0, 5, Qt::AlignLeft | Qt::AlignTop);
	workModeBoxLayout->addWidget(locLabel, 0, 6, Qt::AlignLeft | Qt::AlignTop);
	workModeBoxLayout->addWidget(motorWorkModeLocSetLine, 0, 7, Qt::AlignLeft | Qt::AlignTop);
	workModeBoxLayout->addWidget(motorWorkModeSetBtn, 0, 8, Qt::AlignLeft | Qt::AlignTop);
	workModeBox->setLayout(workModeBoxLayout);
	motorControlBoxLayout->addWidget(workModeBox, 0, 2, Qt::AlignLeft | Qt::AlignTop);


	QGroupBox* pidBox = new QGroupBox(tr("PID:"));
	QGridLayout* pidBoxLayout = new QGridLayout ;

	QGroupBox* pidReadBox = new QGroupBox(tr("PID:"));
	QGridLayout* pidReadBoxLayout = new QGridLayout ;

	QGroupBox* stateReadBox = new QGroupBox();
	QGridLayout* stateReadBoxLayout = new QGridLayout ;

	pidBoxLayout->addWidget(pidCLabel, 0, 0, Qt::AlignLeft | Qt::AlignTop);
	pidBoxLayout->addWidget(motorPidCycleSetLine, 0, 1, Qt::AlignLeft | Qt::AlignTop);
	pidBoxLayout->addWidget(pidPLabel, 0, 2, Qt::AlignLeft | Qt::AlignTop);
	pidBoxLayout->addWidget(motorPidPSetLine, 0, 3, Qt::AlignLeft | Qt::AlignTop);
	pidBoxLayout->addWidget(pidILabel, 0, 4, Qt::AlignLeft | Qt::AlignTop);
	pidBoxLayout->addWidget(motorPidISetLine, 0, 5, Qt::AlignLeft | Qt::AlignTop);
	pidBoxLayout->addWidget(pidDLabel, 0, 6, Qt::AlignLeft | Qt::AlignTop);
	pidBoxLayout->addWidget(motorPidDSetLine, 0, 7, Qt::AlignLeft | Qt::AlignTop);
	pidBoxLayout->addWidget(motorPidSetBtn, 0, 8, Qt::AlignLeft | Qt::AlignTop);
	pidBox->setLayout(pidBoxLayout);
	motorControlBoxLayout->addWidget(pidBox, 1, 2, Qt::AlignLeft | Qt::AlignTop);

	QGroupBox* showSetBox = new QGroupBox;
	QGridLayout*showSetBoxLayout = new QGridLayout ;
	showSetBoxLayout->addWidget(showCycleLabel, 0, 0, Qt::AlignLeft | Qt::AlignTop);
	showSetBoxLayout->addWidget(motorShowCycleSetLine, 0, 1, Qt::AlignLeft | Qt::AlignTop);
	showSetBoxLayout->addWidget(motorShowCycleSetBtn, 0, 2, Qt::AlignLeft | Qt::AlignTop);
	showSetBox->setLayout(showSetBoxLayout);
	motorControlBoxLayout->addWidget(showSetBox, 2, 2, Qt::AlignLeft | Qt::AlignTop);
	motorControlBoxLayout->setColumnStretch(0, 1);
	motorControlBoxLayout->setColumnStretch(1, 1);
	motorControlBoxLayout->setColumnStretch(2, 9);

	motorControlBox->setLayout(motorControlBoxLayout);

	motorCharts->addWidget(motorControlBox, 1, 0);
	motorCharts->setRowStretch(0, 8);
	motorCharts->setRowStretch(1, 2);

	QLabel* workModeReadLabel = new QLabel("Motor Work Mode:" );
	QLabel* displayReadLabel = new QLabel("Valid Display Items:" );
	QLabel* softReadLabel = new QLabel("Software Version:" );
	QLabel* hardReadLabel = new QLabel("Hardware Version:" );
	QLabel* devLabel = new QLabel("Device Type:" );

	for(int i = 0; i < 4;i++){
		motorPidReadLine[i] =  new QLineEdit(this);
		setReadOnlyLineEdit(motorPidReadLine[i]);
	}
	QLabel* pidCLabelR = new QLabel("Cycle:" );
	QLabel* pidPLabelR = new QLabel("P:" );
	QLabel* pidILabelR = new QLabel("I:" );
	QLabel* pidDLabelR = new QLabel("D:" );
	pidReadBoxLayout->addWidget(pidCLabelR, 0, 0, Qt::AlignLeft | Qt::AlignTop);
	pidReadBoxLayout->addWidget(pidPLabelR, 1, 0, Qt::AlignLeft | Qt::AlignTop);
	pidReadBoxLayout->addWidget(pidILabelR, 2, 0, Qt::AlignLeft | Qt::AlignTop);
	pidReadBoxLayout->addWidget(pidDLabelR, 3, 0, Qt::AlignLeft | Qt::AlignTop);

	for(int i = 0; i < 4;i++){
		pidReadBoxLayout->addWidget(motorPidReadLine[i], i, 1, Qt::AlignLeft | Qt::AlignTop);
	}
	motorPidReadBtn = new QPushButton("&Read");
	pidReadBoxLayout->addWidget(motorPidReadBtn, 4, 1, Qt::AlignLeft | Qt::AlignTop);
	pidReadBox->setLayout(pidReadBoxLayout);

	motorWorkModeReadLine =  new QLineEdit(this);
	motorShowItemsLine =  new QLineEdit(this);
	motorDevReadLine = new QLineEdit(this);
	motorSoftVersionLine = new QLineEdit(this);
	motorHardVersionLine = new QLineEdit(this);

	motorWorkModeReadBtn = new QPushButton("&Read");
	motorSoftVersionReadBtn = new QPushButton("&Read");
	motorHardVersionReadBtn = new QPushButton("&Read");
	motorDevReadBtn = new QPushButton("&Read");
	motorShowItemsReadBtn = new QPushButton("&Read");
	setReadOnlyLineEdit(motorWorkModeReadLine);
	setReadOnlyLineEdit(motorShowItemsLine);
	setReadOnlyLineEdit(motorDevReadLine);
	setReadOnlyLineEdit(motorSoftVersionLine);
	setReadOnlyLineEdit(motorHardVersionLine);

	stateReadBoxLayout->addWidget(workModeReadLabel, 1, 0, Qt::AlignLeft | Qt::AlignTop);
	stateReadBoxLayout->addWidget(motorWorkModeReadLine, 1, 1, Qt::AlignLeft | Qt::AlignTop);
	stateReadBoxLayout->addWidget(motorWorkModeReadBtn, 1, 2, Qt::AlignLeft | Qt::AlignTop);


	stateReadBoxLayout->addWidget(devLabel, 0, 0, Qt::AlignLeft | Qt::AlignTop);
	stateReadBoxLayout->addWidget(motorDevReadLine, 0, 1, Qt::AlignLeft | Qt::AlignTop);
	stateReadBoxLayout->addWidget(motorDevReadBtn, 0, 2, Qt::AlignLeft | Qt::AlignTop);

	stateReadBoxLayout->addWidget(softReadLabel, 2, 0, Qt::AlignLeft | Qt::AlignTop);
	stateReadBoxLayout->addWidget(motorSoftVersionLine, 2, 1, Qt::AlignLeft | Qt::AlignTop);
	stateReadBoxLayout->addWidget(motorSoftVersionReadBtn, 2, 2, Qt::AlignLeft | Qt::AlignTop);

	stateReadBoxLayout->addWidget(hardReadLabel, 3, 0, Qt::AlignLeft | Qt::AlignTop);
	stateReadBoxLayout->addWidget(motorHardVersionLine, 3, 1, Qt::AlignLeft | Qt::AlignTop);
	stateReadBoxLayout->addWidget(motorHardVersionReadBtn, 3, 2, Qt::AlignLeft | Qt::AlignTop);

	stateReadBoxLayout->addWidget(displayReadLabel, 4, 0, Qt::AlignLeft | Qt::AlignTop);
	stateReadBoxLayout->addWidget(motorShowItemsLine, 4, 1, Qt::AlignLeft | Qt::AlignTop);
	stateReadBoxLayout->addWidget(motorShowItemsReadBtn, 4, 2, Qt::AlignLeft | Qt::AlignTop);

	stateReadBox->setLayout(stateReadBoxLayout);

	motorStateBoxLayout->addWidget(stateReadBox, 0, 0, Qt::AlignLeft);

	motorStateBoxLayout->addWidget(pidReadBox, 1, 0, Qt::AlignLeft);

	motorStateBox->setLayout(motorStateBoxLayout);


	//adcSettingBoxLayout->addWidget(singelADCBtn_, 0, 0);//, Qt::AlignTop);
	//adcSettingBoxLayout->addWidget(resetADCBtn_, 0, 1);//, Qt::AlignTop);
	//adcSettingBox->setLayout(adcSettingBoxLayout);

	motorDebugLayout->addWidget(motorStateBox);

	QGridLayout* main_show= new QGridLayout ;
	main_show->setColumnStretch(0, 8);
	main_show->setColumnStretch(1, 2);
	main_show->addLayout(motorCharts, 0, 0 );
	main_show->addLayout(motorDebugLayout, 0, 1);

	multiWidget_new->setLayout(main_show);
	this->addTab(multiWidget_new,  "Motor Debug Mainwindow");
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
	resetADCBtn_ = new QPushButton("&Continue");
	
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
	resetFFTBtn_ = new QPushButton("&Continue");

	QLabel* power_Offset_label = new QLabel("Power Offset/dB" );
	power_Offset_edit = new QLineEdit();	
	power_Offset_edit->setText(power_offset_);

	settingADCSavebutton = new QPushButton("&Start FFT-ADC");
	settingADCLayout->addWidget(settingADCSavebutton, 0, 0);

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
	connect(saveBtn, SIGNAL(clicked()), this, SLOT( saveDataThead( void )));
	connect(setSaveBtn, SIGNAL(clicked()), this, SLOT( setSaveFolder( void )));
	connect(settingADCSavebutton, SIGNAL(clicked()), this, SLOT( udpConnect( void )));
	connect(ctlReadBtn_[0], SIGNAL(clicked()), this, SLOT( readPower( void )));
	connect(ctlReadBtn_[1], SIGNAL(clicked()), this, SLOT( readCFAR( void )));
	connect(ctlReadBtn_[2], SIGNAL(clicked()), this, SLOT( read3DFT( void )));
	connect(ctlReadBtn_[3], SIGNAL(clicked()), this, SLOT( readDiff( void )));
    connect( axes_size_edit, SIGNAL( textChanged(QString)), this, SLOT( configAxesSize( void )));
    connect( cell_size_edit, SIGNAL( textChanged(QString)), this, SLOT( configCellSize( void )));
    connect( point_size_edit, SIGNAL( textChanged(QString)), this, SLOT( configPointSize( void )));


	QSignalMapper * configMapper;
	QSignalMapper * readMapper;
	configMapper = new QSignalMapper(this);
	readMapper = new QSignalMapper(this);
	for(int i = 0; i < 4; i++)
	{
		connect(regBtnWrite[i], SIGNAL(clicked(bool)), configMapper, SLOT(map()));//这个map(）是QSignalMapper类的槽函数，不需要我们定义
		connect(regBtnRead[i], SIGNAL(clicked(bool)), readMapper, SLOT(map()));
		configMapper->setMapping(regBtnWrite[i], i);//这个i就是我们传给槽函数的值，可以是字符串，其他等等。
		readMapper->setMapping(regBtnRead[i], i);
	}

	connect(configMapper, SIGNAL(mapped(int)), this, SLOT(configReg(int)));
	connect(readMapper, SIGNAL(mapped(int)), this, SLOT(readReg(int)));

	connect(mFFTShowdBBtn, SIGNAL(clicked()), this, SLOT( showdBFFT( void )));
	connect(pcSwitchBtn, SIGNAL(clicked()), this, SLOT( udpPcConnect( void )));
	connect(pcOnceBtn, SIGNAL(clicked()), this, SLOT( startPcUdpOnce( void )));
	connect(pcResetBtn, SIGNAL(clicked()), this, SLOT( startPcUdpContinuous( void )));

	connect(pcBWBtn, SIGNAL(clicked()), this, SLOT( pcShowBW( void )));
	connect(pcRecordBtn, SIGNAL(clicked()), this, SLOT( pcRecord( void )));

	connect(singelFFTBtn_, SIGNAL(clicked()), this, SLOT( singleFFT( void )));
	connect(resetFFTBtn_, SIGNAL(clicked()), this, SLOT( resetFFT( void )));
	connect(singelADCBtn_, SIGNAL(clicked()), this, SLOT( singleADC( void )));
	connect(resetADCBtn_, SIGNAL(clicked()), this, SLOT( resetADC( void )));


	connect(motorConnectBtn, SIGNAL(clicked()), this, SLOT( sendMotorConnectCmd( void )));
	connect(motorSwitchBtn, SIGNAL(clicked()), this, SLOT( sendMotorOpenCmd( void )));
	connect(motorPidReadBtn, SIGNAL(clicked()), this, SLOT( readMotorPid( void )));
	connect(motorWorkModeReadBtn, SIGNAL(clicked()), this, SLOT( readMotorWorkMode( void )));
	connect(motorShowItemsReadBtn, SIGNAL(clicked()), this, SLOT( readMotorShowItems( void )));
	connect(motorWorkModeSetBtn, SIGNAL(clicked()), this, SLOT( sendMotorWorkModeCmd( void )));
	connect(motorShowCycleSetBtn, SIGNAL(clicked()), this, SLOT( sendMotorDisplayCycleCmd( void )));
	connect(motorPidSetBtn, SIGNAL(clicked()), this, SLOT( sendMotorPidCmd( void )));

	connect(errorLogText,SIGNAL(textChanged()),SLOT(slotTextTcpChanged()));
    timer_  = new QTimer(this);
    //timer_->setInterval(50);
    connect(timer_, SIGNAL(timeout()), this, SLOT(updateFFTdata(void)));
    timer_->start(10);

    timer_adc  = new QTimer(this);
    //timer_->setInterval(50);
    connect(timer_adc, SIGNAL(timeout()), this, SLOT(updateADCdata(void)));
    timer_adc->start(10);

    QTimer* timer_state  = new QTimer(this);
    connect(timer_state, SIGNAL(timeout()), this, SLOT(updateState(void)));
    connect(timer_state, SIGNAL(timeout()), this, SLOT(printErrorLog(void)));
    //connect(timer_state, SIGNAL(timeout()), this, SLOT(recvSerialInfo(void)));

    timer_state->start(300);

}

void viewpanel::CreatUIWindow()
{
	const QSize button_size = QSize(200, 30);
	const QSize view_button_side = QSize(60, 25);
	const QSize slider_size = QSize(200, 20);

	QWidget* multiWidget = new QWidget();
	ctrlDock = new QDockWidget();
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
	pcSwitchBtn = new QPushButton("&Start PC", this);
	pcOnceBtn = new QPushButton("&PC Single", this);
	pcResetBtn = new QPushButton("&PC Contin", this);
	pcBWBtn = new QPushButton("&Bg Color", this);
	pcRecordBtn = new QPushButton("&PointCloud Record", this);

	//lidar_stop_button = new QPushButton("Stop", this);
	//lidarIdCombo =  new QComboBox;

	QLabel* lidar_IP_label = new QLabel( "IP addr" );
	QLabel* lidar_port_label = new QLabel( "TCP Port" );
	QLabel* lidar_udp_port_label = new QLabel( "UDP Port" );
	QLabel* distanceOffset_label = new QLabel( "distance Offset" );
	QLabel* pcPort_label = new QLabel( "PointCloud port" );

	ip_edit =  new QLineEdit();
	port_edit =  new QLineEdit();
	udp_pc_port_edit =  new QLineEdit();
	udp_port_edit =  new QLineEdit();

	distance_Offset_edit = new QLineEdit();

	ip_edit->setPlaceholderText("input ip addr");
	ip_edit->setText(lidar_ip_);
	port_edit->setPlaceholderText("input tcp port");
	port_edit->setText(lidar_ctrl_port_);
	udp_port_edit->setPlaceholderText("input udp port");
	udp_port_edit->setText(lidar_UDP_port_);
	udp_pc_port_edit->setPlaceholderText("input udp pc port");
	udp_pc_port_edit->setText(lidar_UDP_PC_port_);
	distance_Offset_edit->setPlaceholderText("input distance offset ");
	distance_Offset_edit->setText(distance_offset_);

	controls_layout->addWidget( lidar_IP_label, 0, 0, Qt::AlignRight);
	controls_layout->addWidget( ip_edit, 0, 1, Qt::AlignLeft);
	controls_layout->addWidget( lidar_port_label, 1, 0, Qt::AlignRight);
	controls_layout->addWidget( port_edit, 1, 1, Qt::AlignLeft);

	controls_layout->addWidget( lidar_udp_port_label, 2, 0, Qt::AlignRight);
	controls_layout->addWidget( udp_port_edit, 2, 1, Qt::AlignLeft);
	controls_layout->addWidget( pcPort_label, 3, 0, Qt::AlignRight);	
	controls_layout->addWidget( udp_pc_port_edit, 3, 1, Qt::AlignLeft);	
	controls_layout->addWidget( lidar_connect_button, 4, 0, Qt::AlignLeft);
	controls_layout->addWidget( setSaveBtn, 4, 1, Qt::AlignLeft);

	QLabel* CFAR_label = new QLabel( "CFAR" );
	QLabel* m3DFT_label = new QLabel( "3DFT" );
	QLabel* Power_label = new QLabel( "Power/mW" );
	QLabel* diff_label = new QLabel( "Diff" );
	QLabel* regAddr_label = new QLabel( "Reg Addr" );
	QLabel* regVal_label = new QLabel( "Reg value" );

	QLabel* adc_label0 = new QLabel( "ADC OverVol" );
	QLabel* adc_label1 = new QLabel( "ADC Atten" );

	for (int i = 0; i < 4; i++){
		regAddr_line[i] = new QLineEdit;
		regAddr_line[i]->setText(m_reg_addr_[i]);
		regVal_line[i] = new QLineEdit;
		regVal_line[i]->setText(m_reg_value_[i]);
		regRead_line[i] = new QLineEdit;
		regBtnWrite[i] = new QPushButton("&Write", this);
		regBtnRead[i] = new QPushButton("&Read", this);
		setReadOnlyLineEdit(regRead_line[i]);
	}

	adcRead0_line = new QLineEdit;
	adcRead1_line = new QLineEdit;
	setReadOnlyLineEdit(adcRead0_line);
	setReadOnlyLineEdit(adcRead1_line);


	CFARCombo = new QComboBox;
	m3DFTCombo = new QComboBox;
	PowerCombo = new QComboBox;
	colorCombo = new QComboBox;
	colorCombo->addItem(tr("range"));
	colorCombo->addItem(tr("intensity"));
	colorCombo->addItem(tr("speed"));

	PowerCombo->setEditable(true);
	DiffCombo = new QComboBox;

	for (int i = 0; i < 8; i++){
		CFARCombo->addItem(QString::number(i));
	}

	for (int i = 0; i < power_index.size(); i++){
		PowerCombo->addItem(QString::number(power_index[i]));
	}
	PowerCombo->setCurrentIndex(18);

	m3DFTCombo->addItem(tr("0"));
	m3DFTCombo->addItem(tr("1"));

	DiffCombo->addItem(tr("0"));
	DiffCombo->addItem(tr("1"));

	controls_layout->addWidget( Power_label, 0, 2, Qt::AlignRight);	
	controls_layout->addWidget( CFAR_label, 1, 2, Qt::AlignRight);			
	controls_layout->addWidget( m3DFT_label, 2, 2, Qt::AlignRight);			
	controls_layout->addWidget( diff_label, 3, 2, Qt::AlignRight);	

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
	controls_layout->addWidget( regAddr_label, 0, 7, Qt::AlignRight);
	controls_layout->addWidget( regVal_label, 0, 9, Qt::AlignRight);	

	for(int i = 0; i < 4; i++){
		controls_layout->addWidget( regAddr_line[i], i, 8, Qt::AlignLeft);	
		controls_layout->addWidget( regVal_line[i], i, 10, Qt::AlignLeft);	
		controls_layout->addWidget( regBtnWrite[i], i, 11, Qt::AlignLeft);	
		controls_layout->addWidget( regRead_line[i], i, 12, Qt::AlignLeft);	
		controls_layout->addWidget( regBtnRead[i], i, 13, Qt::AlignLeft);	
	}

	//controls_layout->addWidget( settingADCSavebutton, 4, 2, Qt::AlignLeft);

	QLabel* rotate_label = new QLabel( "rotate angle" );
	rotate_angle_edit = new QLineEdit;
	rotate_angle_edit->setText(QString::number(rotation_offset));
	QLabel* left_label = new QLabel( "left angle" );
	left_angle_edit = new QLineEdit;
	left_angle_edit->setText(QString::number(leftAngle_offset));
	QLabel* right_label = new QLabel( "right angle" );
	right_angle_edit = new QLineEdit;
	QLabel* color_base_label = new QLabel( "color base" );
	color_base_edit = new QLineEdit;
	color_base_edit->setText(QString::number(color_base));
	QLabel* point_size_label = new QLabel( "point size" );
	point_size_edit = new QLineEdit;
	point_size_edit->setText(QString::number(point_size));
	QLabel* cell_size_label = new QLabel( "cell size" );
	cell_size_edit = new QLineEdit;
	cell_size_edit->setText(QString::number(cell_size));

	QLabel* axes_size_label = new QLabel( "axes size" );
	axes_size_edit = new QLineEdit;
	axes_size_edit->setText(QString::number(axes_size));

	QLabel* color_by_label = new QLabel( "color by" );
	right_angle_edit->setText(QString::number(rightAngle_offset));

	//QFrame* hframe = new QFrame(this);
	QFrame* vframe = new QFrame(this);
	//hframe->setFrameShape(QFrame::HLine);      // 设置水平方向
	//hframe->setStyleSheet("QFrame{background:red;min-height:5px}");
	vframe->setFrameShape(QFrame::VLine);      // 设置垂直方向
	vframe->setStyleSheet("QFrame{background:rgb(192,192,192);min-width:10px;border:0.0px solid black}");
	controls_layout->addWidget( vframe, 0, 14, 5, 1);

	controls_layout->addWidget( pcSwitchBtn, 0, 15, Qt::AlignRight);	
	controls_layout->addWidget( pcOnceBtn, 1, 15, Qt::AlignRight);	
	controls_layout->addWidget( pcResetBtn, 2, 15, Qt::AlignRight);	
	controls_layout->addWidget( saveBtn, 3, 15, Qt::AlignRight);
	controls_layout->addWidget( pcBWBtn, 4, 15, Qt::AlignRight);	
	controls_layout->addWidget( pcRecordBtn, 4, 19, Qt::AlignRight);

	controls_layout->addWidget( point_size_label, 0, 16, Qt::AlignRight);
	controls_layout->addWidget( point_size_edit, 0, 17, Qt::AlignLeft);	
	controls_layout->addWidget( cell_size_label, 1, 16, Qt::AlignRight);
	controls_layout->addWidget( cell_size_edit, 1, 17, Qt::AlignLeft);	
	controls_layout->addWidget( color_base_label, 2, 16, Qt::AlignRight);
	controls_layout->addWidget( color_base_edit, 2, 17, Qt::AlignLeft);	
	controls_layout->addWidget( axes_size_label, 3, 16, Qt::AlignRight);	
	controls_layout->addWidget( axes_size_edit, 3, 17, Qt::AlignLeft);	
	controls_layout->addWidget( color_by_label, 4, 16, Qt::AlignRight);	
	controls_layout->addWidget( colorCombo, 4, 17, Qt::AlignLeft);		
	
	controls_layout->addWidget( distanceOffset_label, 0, 18, Qt::AlignRight);
	controls_layout->addWidget( distance_Offset_edit, 0, 19, Qt::AlignLeft);	
	controls_layout->addWidget( rotate_label, 1, 18, Qt::AlignRight);
	controls_layout->addWidget( rotate_angle_edit, 1, 19, Qt::AlignLeft);	
	controls_layout->addWidget( left_label, 2, 18, Qt::AlignRight);
	controls_layout->addWidget( left_angle_edit, 2, 19, Qt::AlignLeft);	
	controls_layout->addWidget( right_label, 3, 18, Qt::AlignRight);
	controls_layout->addWidget( right_angle_edit, 3, 19, Qt::AlignLeft);	

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
#if 0
	controls->addWidget(stateShowBox, 0, 1, Qt::AlignLeft);
	//controls->addWidget(fileBox, 0, 2, Qt::AlignLeft);
	controls->setColumnStretch(0,6);
	for(int i = 1; i < 2;i++)
		controls->setColumnStretch(i,2);
#endif
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
	//std::cout << "!!enter parseADCData! input point num is  "  <<  data.size() / 2 << std::endl;
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
	if(!adcMsg_free_buf_queue.empty()){
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
		//ROS_INFO("adcMsg send finished");  
	}else{
		ROS_INFO("warning!!adcMsg_free_buf_queue is empty, will discard adc data");
	}

}


void viewpanel::parseFFTData(std::vector<uint8_t> &data)
{
	//std::cout << "!!enter parseFFTData! input point num is  "  <<  data.size() / 4 << std::endl;
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
	if(fftMsg_free_buf_queue.empty()) {
		ROS_INFO("warning!!fftMsg_free_buf_queue is empty, will discard fft data");
		return;
	}
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
	//ROS_INFO("fftMsg send finished");  //打印自己发送的信息
}

void viewpanel::Save2filecsv(std::vector<uint8_t> &data, bool ifsave)
{
	if(!ifsave) return;
	//memset(&curPcData, 0, sizeof(curPcData));
	static long findex = 0;
#if 1
	std::string datPath;
	datPath = save_folder_.toStdString() + "/data_test_raw_index" + std::to_string(findex++) +".bin";
	ROS_INFO("datPath is %s \n", datPath.c_str());
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
	double distance;
	double speed;
	double vAngle;
	double hAngle;

	int index = 0;
	csvfile << "intensity" << "," << "distance(m)" << "," 
	<< "speed(m/s)" << "," << "Vertical angle(degree)" << "," << "Horizontal angle(degree)" << "\n";	
	for(int i = 0; i < data.size(); i++) {
		index += 1;
		if(index < 5)
			cur_data += data[i] << (8 * (index - 1));
		else if (index < 9)
			cur_data += data[i] << (8 * (index - 5));
		else if (index < 11)
			cur_data += data[i] << (8 * (index - 9));
		else if (index < 13)
			cur_data += data[i] << (8 * (index - 11));
		else if (index < 15)
			cur_data += data[i] << (8 * (index - 13));

		if(index == 4 || index == 8){
			if(index == 4) {
				csvfile << cur_data << ",";	//intensity
			}
			else{
				distance = cur_data / 65536.0 - distance_offset; //distance
				csvfile << distance << ",";	
			}
			cur_data = 0;
		}

		if(index == 10){
			if(cur_data > SIGN_LIMIT_NUM)
				cur_data -= SIGN_OFFSET_NUM;
			speed = cur_data / 128.0;
			csvfile << speed << ",";	 // speed
			cur_data = 0;
		}

		if(index == 12){
			vAngle = cur_data / 256.0 - 2.5; //vertical
			csvfile << vAngle << ",";	
			cur_data = 0;
		}

		if(index == 14){
			hAngle = cur_data * 720.0 / 32000.0; //horizontal
			if(hAngle > 360.0) hAngle -= 360.0;
			csvfile << hAngle << "\n";	
			cur_data = 0;
		}

		if(index == 16){
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

	cmdMsg_.mHead.usCommand = commandType::POINTCLOUD_TCP_READ;
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
		if(g_msg.cmdmsg.mHead.usCommand == commandType::POINTCLOUD_TCP_READ){
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
			cmdMsg_.mHead.usCommand = commandType::POINTCLOUD_TCP_READ;
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

void viewpanel::TaskFuncPCRecv(void *arg){
    viewpanel *pSave = (viewpanel *)arg;
	std::cout << "enter TaskFuncPCRecv" << std::endl;
    if (pSave && pSave->TaskFuncPCRecv) {
        pSave->udpRecvPCLoop();
    }
}

void viewpanel::TaskFuncPCParse(void *arg){
    viewpanel *pSave = (viewpanel *)arg;
	std::cout << "enter TaskFuncPCParse" << std::endl;
    if (pSave && pSave->TaskFuncPCParse) {
        pSave->pcParseLoop();
    }
}

void viewpanel::start_save_task()
{

}
void viewpanel::saveDataThead()
{
	if(!ifStarted){
		QMessageBox msgBox;
		msgBox.setText("tcp is not connected, save failed!");
		msgBox.exec();
		return;		
	}
	if(!udpStop_) udpClose();
	if(!udpPCStop_) udpPcClose();
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

void viewpanel::pcRecord(){
	if(udpPCStop_){
		QMessageBox msgBox;
		msgBox.setText("pointCloud udp is not connected!");
		msgBox.exec();
		return;		
	}
	static bool recording = false;
	if(!recording){
		std::string pcRecordPath;
		std::string cmd;
		std::string node_name = " __name:=fmcw_record_node &";
		pcRecordPath = save_folder_.toStdString() + "/pointCloud_record" +".bag";
		std::string pointcloud_topic = " /fmcw/rviz/pointcloud";
		ROS_INFO("pcRecordPath is %s \n", pcRecordPath.c_str());
		cmd = "rosbag record -o " + pcRecordPath + pointcloud_topic + node_name;
		int ret = system(cmd.c_str());
		recording = true;
		pcRecordBtn->setStyleSheet("color: red");
		pcRecordBtn->setText("&Recording");
	} else {
		ros::V_string v_nodes;
		ros::master::getNodes(v_nodes);
		std::string node_name = std::string("/fmcw_record_node");
		auto it = std::find(v_nodes.begin(), v_nodes.end(), node_name.c_str());
		if (it != v_nodes.end()){
			std::string cmd_str = "rosnode kill " + node_name;
			int ret = system(cmd_str.c_str());
			std::cout << "## stop rosbag record cmd: " << cmd_str << std::endl;
		}else{
			QMessageBox msgBox;
			msgBox.setText("stop rosbag record failed!");
			msgBox.exec();
			return;
		}
		recording = false;
		pcRecordBtn->setStyleSheet("color: black");
		pcRecordBtn->setText("&PointCloud Record");
	}
}

void viewpanel::pcShowBW(){
	static bool showBlack = true;
	if(!showBlack){
		render_panel_->setBackgroundColor(Ogre::ColourValue(238,238,236,0.3));
		showBlack = true;
	}else{
		render_panel_->setBackgroundColor(Ogre::ColourValue(0,0,0,0.3));
		showBlack = false;		
	}
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
	vx_task_delete(&bst_task[2]);
	settingADCSavebutton->setStyleSheet("color: black");
	settingADCSavebutton->setText("&Start FFT-ADC");
}

void viewpanel::udpPcClose(){
	::close(udpRecvPCSocketFd_);
	udpPCStop_ = true;
	vx_task_delete(&bst_task[3]);
	vx_task_delete(&bst_task[4]);
	commandMsg cmdMsg;
	memset(&cmdMsg, 0, sizeof(commandMsg));
	cmdMsg.mHead.usCommand = commandType::POINTCLOUD_UDP_STOP;
	if(::write(ctrl_sock, &cmdMsg, sizeof(commandMsg)) < 0){
		QMessageBox msgBox;
		msgBox.setText("Close PointCloud failed!");
		msgBox.exec();
		return;
	}
	pcSwitchBtn->setStyleSheet("color: black");
	pcSwitchBtn->setText("&Start PC");
}


void viewpanel::configAxesSize(){
	axes_size = axes_size_edit->text().toDouble();
	Axes_->subProp("Length")->setValue(axes_size);

}
void viewpanel::configCellSize(){
	cell_size = cell_size_edit->text().toDouble();
	grid_->subProp( "Cell Size" )->setValue(cell_size); 
}
void viewpanel::configPointSize(){
	point_size = point_size_edit->text().toDouble();
	pointcloud_fmcw->subProp("Size (m)")->setValue(point_size);
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
		ROS_INFO("fftMsg update");  
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


void viewpanel::parseMotorInfo(uint8_t* ptr)
{
	if(ptr == nullptr){
		QMessageBox msgBox;
		msgBox.setText("MOTOR info is empty!");
		msgBox.exec();	
		return;		
	}
	if(ptr[0] != 0xaa || ptr[1] != 0x55){
		qDebug() << "the msg is not motor info!";
		return;
	}
	uint8_t motor_id = ptr[2];
	uint8_t cmd_id = ptr[3];
	int datalen = ptr[4];
	std::cout << "the motor_id is " << motor_id  << ", cmd_id is " << cmd_id << ", datalen is " << datalen << std::endl;

	switch (ptr[3])
	{
	case MOTOR_CONNECT_RET:
		if(ptr[5] == 0xFF){
			motorConnectBtn->setStyleSheet("color: green");
			motorConnectBtn->setText("&Disconnect");
			ifConnectedMotor = true;
		} else {
			QMessageBox msgBox;
			msgBox.setText("MOTOR CONNECT failed!");
			msgBox.exec();			
		}	
		break;
	case MOTOR_DISCONNECT_RET:
		if(ptr[5] == 0xFF){
			motorConnectBtn->setStyleSheet("color: black");
			motorConnectBtn->setText("&Connect");
			ifConnectedMotor = false;
			releaseSerial();
		} else {
			QMessageBox msgBox;
			msgBox.setText("MOTOR DISCONNECT failed!");
			msgBox.exec();			
		}
		break;
	case MOTOR_OPEN_RET:
		if(ptr[5] == 0xFF){
			if(ifOpenMotor){
				motorSwitchBtn->setStyleSheet("color: black");
				motorSwitchBtn->setText("&Open");
				ifOpenMotor = false;
			}else {
				motorSwitchBtn->setStyleSheet("color: green");
				motorSwitchBtn->setText("&Close");
				ifOpenMotor = true;				
			}
		} else {
			QMessageBox msgBox;
			msgBox.setText("MOTOR SWITCH failed!");
			msgBox.exec();			
		}	
		break;	
	case MOTOR_PID_READ_RET:
		if(ptr[4] == 16){
			for(int i = 0; i < 4; i++){
				int index = 4 * (i+1);
				motorPidReadLine[i]->setText(QString::number( UnsignedChar4ToFloat(&(ptr[index])) ,'f',3));
			}	
			QMessageBox msgBox;
			msgBox.setText("MOTOR MOTOR_PID_READ successfully!");
			msgBox.exec();	
		}else {
			QMessageBox msgBox;
			msgBox.setText("MOTOR MOTOR_PID_READ failed!");
			msgBox.exec();					
		}
		break;

	case MOTOR_SHOW_ITEMS_READ_RET:
		{
			QString showName;
			for(int i = 0; i < ptr[3]; i++){
				showName = showName + QString::number(ptr[4 + i]) +  QString(" ");
			}
			motorShowItemsLine->setText(showName);
			QMessageBox msgBox;
			msgBox.setText("MOTOR MOTOR_PID_READ successfully!");
			msgBox.exec();
		}
		break;
	case MOTOR_WORKMODE_SET_RET:
		if(ptr[5] == 0xFF){
			QMessageBox msgBox;
			msgBox.setText("MOTOR MOTOR_WORKMODE_SET successfully!");
			msgBox.exec();
		}
		break;
	case MOTOR_SHOW_CYCLE_SET_RET:
		if(ptr[5] == 0xFF){
			QMessageBox msgBox;
			msgBox.setText("MOTOR MOTOR_SHOW_CYCLE_SET successfully!");
			msgBox.exec();
		}
		break;
	case MOTOR_PID_SET_RET:
		if(ptr[5] == 0xFF){
			QMessageBox msgBox;
			msgBox.setText("MOTOR MOTOR_PID_SET_RET successfully!");
			msgBox.exec();
		} else if(ptr[5] == 0x7F) {
			QMessageBox msgBox;
			msgBox.setText("MOTOR MOTOR_PID_SET_RET failed!");
			msgBox.exec();
		}
		break;
	case MOTOR_WORKMODE_READ_RET:
		if(ptr[5] == 0x00){
			motorWorkModeReadLine->setText(QString("speed"));
		}else if(ptr[5] == 0x01){
			motorWorkModeReadLine->setText(QString("location"));
		}else if(ptr[5] == 0x02){
			motorWorkModeReadLine->setText(QString("Phase-lock"));
		}else {
			QMessageBox msgBox;
			msgBox.setText("MOTOR MOTOR_PID_READ failed!");
			msgBox.exec();	
			return;				
		}
		{
			QMessageBox msgBox;
			msgBox.setText("MOTOR MOTOR_PID_READ successfully!");
			msgBox.exec();	
		}
		break;
	case MOTOR_ITEMS_INFO:
		break;
		
	default:
		break;
	}
}
void viewpanel::recvSerialInfo()
{
	//m_serialPort->waitForReadyRead(10);
	QByteArray info = m_serialPort->readAll();
	if(info.isEmpty()){
		qDebug() << "info recvSerialInfo  is empty";
		return;
	}
	QByteArray hexData = info.toHex();
	qDebug() << "info recvSerialInfo is " << hexData;
	uint8_t* ptr = (uint8_t*)info.data();
	parseMotorInfo(ptr);
}

void viewpanel::recvSerialInfoTest()
{
	//m_serialPort_test->waitForReadyRead(10);
	QByteArray info = m_serialPort_test->readAll();
	if(info.isEmpty()){
		qDebug() << "info recvSerialInfoTest  is empty";
		return;
	}
	QByteArray hexData = info.toHex();
	qDebug() << "info recvSerialInfoTest is " << hexData;
	uint8_t* ptr = (uint8_t*)info.data();
	int ret;
	float dataPid[4] = {6.666, 7.777, 8.888, 9.999};

	switch (ptr[2])
	{
	case MOTOR_CONNECT:
		motorMsgSend1_.cmd = motorCmdType::MOTOR_CONNECT_RET;
		motorMsgSend1_.dataLen = 0x01;
		motorMsgSend1_.data = 0xff;
		ret = m_serialPort_test->write((const char *)&motorMsgSend1_,sizeof(motorMsgSend1_));
		ROS_INFO("connect motor ok, ret is %d", ret);
		break;
	case MOTOR_DISCONNECT:
		ROS_INFO("disconnect motor ready");
		motorMsgSend1_.cmd = motorCmdType::MOTOR_DISCONNECT_RET;
		motorMsgSend1_.dataLen = 0x01;
		motorMsgSend1_.data = 0xff;
		ret = m_serialPort_test->write((const char *)&motorMsgSend1_,sizeof(motorMsgSend1_));
		ROS_INFO("disconnect motor ok, ret is %d", ret);
		break;
	case MOTOR_OPEN:
		motorMsgSend1_.cmd = motorCmdType::MOTOR_OPEN_RET;
		motorMsgSend1_.dataLen = 0x01;
		motorMsgSend1_.data = 0xff;
		ret = m_serialPort_test->write((const char *)&motorMsgSend1_,sizeof(motorMsgSend1_));
		ROS_INFO("switch motor ok, ret is %d", ret);
		break;	
	case MOTOR_WORKMODE_READ:
		motorMsgSend1_.cmd = motorCmdType::MOTOR_WORKMODE_READ_RET;
		motorMsgSend1_.dataLen = 0x01;
		motorMsgSend1_.data = 0x02;
		ret = m_serialPort_test->write((const char *)&motorMsgSend1_,sizeof(motorMsgSend1_));
		ROS_INFO("MOTOR_WORKMODE_READ ok, ret is %d", ret);
		break;	
	case MOTOR_WORKMODE_SET:
		motorMsgSend1_.cmd = motorCmdType::MOTOR_WORKMODE_SET_RET;
		motorMsgSend1_.dataLen = 0x01;
		motorMsgSend1_.data = 0xff;
		ret = m_serialPort_test->write((const char *)&motorMsgSend1_,sizeof(motorMsgSend1_));
		ROS_INFO("MOTOR_WORKMODE_SET ok, ret is %d", ret);
		break;	
	case MOTOR_PID_SET:
		for(int i = 0; i < 4; i++){
			int index = 4 * (i+1);
			motorPidReadLine[i]->setText(QString::number( UnsignedChar4ToFloat(&(ptr[index])) ,'f',3));
		}	
		motorMsgSend1_.cmd = motorCmdType::MOTOR_PID_SET_RET;
		motorMsgSend1_.dataLen = 0x01;
		motorMsgSend1_.data = 0xff;
		ret = m_serialPort_test->write((const char *)&motorMsgSend1_,sizeof(motorMsgSend1_));
		ROS_INFO("MOTOR_PID_SET ok, ret is %d", ret);
		break;	
	case MOTOR_SHOW_CYCLE_SET:
		motorMsgSend1_.cmd = motorCmdType::MOTOR_SHOW_CYCLE_SET_RET;
		motorMsgSend1_.dataLen = 0x01;
		motorMsgSend1_.data = 0xff;
		ret = m_serialPort_test->write((const char *)&motorMsgSend1_,sizeof(motorMsgSend1_));
		ROS_INFO("MOTOR_SHOW_CYCLE_SET ok, ret is %d", ret);
		break;	
	case MOTOR_SHOW_ITEMS_READ:
		motorMsgShow_.cmd = motorCmdType::MOTOR_SHOW_ITEMS_READ_RET;
		motorMsgShow_.dataLen = 0x05;
		motorMsgShow_.data[0] = 0x01;
		motorMsgShow_.data[1] = 0x02;
		motorMsgShow_.data[2] = 0x04;
		motorMsgShow_.data[3] = 0x05;
		motorMsgShow_.data[4] = 0x06;
		ret = m_serialPort_test->write((const char *)&motorMsgShow_,sizeof(motorMsgShow_));
		ROS_INFO("MOTOR_SHOW_ITEMS_READ_RET ok, ret is %d", ret);
		break;	
	case MOTOR_PID_READ:
		motorMsgPid_.cmd = motorCmdType::MOTOR_PID_READ_RET;
		motorMsgPid_.dataLen = 0x10;
		for(int i = 0; i < 4; i++){
			int index = 4 * ( i );
			FloatToChar(dataPid[i], &(motorMsgPid_.data[index]));
		}
		motorMsgPid_.count = 0x01;
		motorMsgPid_.crc = 0;
		ret = m_serialPort_test->write((const char *)&motorMsgPid_,sizeof(motorMsgPid_));
		ROS_INFO("MOTOR_PID para send ok, ret is %d", ret);
		break;	
	default:
		break;
	}
}

int viewpanel::checkMotorConnected()
{
	if(!ifConnectedMotor){
		QMessageBox msgBox;
		msgBox.setText("please connect to the motor serial device firstly!");
		msgBox.exec();
		return -1;
	} 

	return 0;
}


void viewpanel::sendMotorPidCmd()
{
	if(checkMotorConnected()) return;
	motorMsgPidSet_.cmd = motorCmdType::MOTOR_PID_SET;
	motorMsgPidSet_.dataLen = 0x10;
	motorMsgPidSet_.count = 0x01;
	motorMsgPidSet_.cycle = motorPidCycleSetLine->text().toDouble();
	motorMsgPidSet_.p = motorPidPSetLine->text().toDouble();
	motorMsgPidSet_.i = motorPidISetLine->text().toDouble();
	motorMsgPidSet_.d = motorPidDSetLine->text().toDouble();
	motorMsgPidSet_.crc = 0;

#if 0
	motorMsgPidSet_.crc = motorMsgPidSet_.cmd + motorMsgPidSet_.dataLen + \
	(motorMsgWorkMode_.speed & 0xff) + (motorMsgWorkMode_.speed >> 8) + \
	(motorMsgWorkMode_.angle & 0xff) + (motorMsgWorkMode_.angle >> 8) + \
	(motorMsgWorkMode_.location & 0xff) + (motorMsgWorkMode_.location >> 8) + \
	motorMsgWorkMode_.mode + motorMsgWorkMode_.count;
#endif
	int ret = m_serialPort->write((const char *)&motorMsgPidSet_,sizeof(motorMsgPidSet_));
	ROS_INFO("MOTOR_WORKMODE_SET m_serialPort->write is %d", ret);
}

void viewpanel::sendMotorWorkModeCmd()
{
	if(checkMotorConnected()) return;
	motorMsgWorkMode_.cmd = motorCmdType::MOTOR_WORKMODE_SET;
	motorMsgWorkMode_.dataLen = 0x07;
	motorMsgWorkMode_.count = 0x01;
	motorMsgWorkMode_.speed = motorWorkModeSpeedSetLine->text().toInt();
	motorMsgWorkMode_.angle = motorWorkModeAngleSetLine->text().toInt();
	motorMsgWorkMode_.location = motorWorkModeLocSetLine->text().toInt();
	QString modeName = motorWorkModeCombo->currentText();
	uint16_t mode = 0;
	if(modeName == "location"){
		mode = 1;
		ROS_INFO("MODE IS location");
	}else if(modeName == "phase-lock"){
		mode = 2;
		ROS_INFO("MODE IS phase-lock");
	}else{
		ROS_INFO("MODE IS speed");
	}
	motorMsgWorkMode_.mode = mode;
	motorMsgWorkMode_.crc = motorMsgWorkMode_.cmd + motorMsgWorkMode_.dataLen + \
	(motorMsgWorkMode_.speed & 0xff) + (motorMsgWorkMode_.speed >> 8) + \
	(motorMsgWorkMode_.angle & 0xff) + (motorMsgWorkMode_.angle >> 8) + \
	(motorMsgWorkMode_.location & 0xff) + (motorMsgWorkMode_.location >> 8) + \
	motorMsgWorkMode_.mode + motorMsgWorkMode_.count;
	int ret = m_serialPort->write((const char *)&motorMsgWorkMode_,sizeof(motorMsgWorkMode_));
	ROS_INFO("MOTOR_WORKMODE_SET m_serialPort->write is %d", ret);
}

void viewpanel::sendMotorDisplayCycleCmd()
{
	if(checkMotorConnected()) return;
	motorMsgShowCycle_.cmd = motorCmdType::MOTOR_SHOW_CYCLE_SET;
	motorMsgShowCycle_.dataLen = 0x02;
	motorMsgShowCycle_.data = motorShowCycleSetLine->text().toInt();
	motorMsgShowCycle_.count = 0x01;
	motorMsgShowCycle_.crc = motorMsgShowCycle_.cmd + motorMsgShowCycle_.dataLen +\
	(motorMsgShowCycle_.data & 0xff) + (motorMsgShowCycle_.data >> 8) + \
	 motorMsgShowCycle_.count;
	int ret = m_serialPort->write((const char *)&motorMsgShowCycle_,sizeof(motorMsgShowCycle_));
	ROS_INFO("MOTOR_SHOW_CYCLE_SET m_serialPort->write is %d", ret);
}

void viewpanel::readMotorShowItems()
{
	if(checkMotorConnected()) return;
	motorMsgSend_.cmd = motorCmdType::MOTOR_SHOW_ITEMS_READ;
	motorMsgSend_.dataLen = 0x00;
	motorMsgSend_.count = 0x01;
	motorMsgSend_.crc = motorMsgSend_.cmd + motorMsgSend_.dataLen + motorMsgSend_.count;
	int ret = m_serialPort->write((const char *)&motorMsgSend_,sizeof(motorMsgSend_));
	ROS_INFO("MOTOR_SHOW_ITEMS_READ m_serialPort->write is %d", ret);
}

void viewpanel::readMotorWorkMode()
{
	if(checkMotorConnected()) return;
	motorMsgSend_.cmd = motorCmdType::MOTOR_WORKMODE_READ;
	motorMsgSend_.dataLen = 0x00;
	motorMsgSend_.count = 0x01;
	motorMsgSend_.crc = motorMsgSend_.cmd + motorMsgSend_.dataLen + motorMsgSend_.count;
	int ret = m_serialPort->write((const char *)&motorMsgSend_,sizeof(motorMsgSend_));
	ROS_INFO("MOTOR_WORKMODE_READ m_serialPort->write is %d", ret);
}

void viewpanel::readMotorPid()
{
	if(checkMotorConnected()) return;
	motorMsgSend_.cmd = motorCmdType::MOTOR_PID_READ;
	motorMsgSend_.dataLen = 0x00;
	motorMsgSend_.count = 0x01;
	motorMsgSend_.crc = motorMsgSend_.cmd + motorMsgSend_.dataLen + motorMsgSend_.count;
	int ret = m_serialPort->write((const char *)&motorMsgSend_,sizeof(motorMsgSend_));
	ROS_INFO("m_serialPort->write is %d", ret);
}

void viewpanel::sendMotorOpenCmd()
{
	if(!ifOpenMotor){
		if(checkMotorConnected()) return;
		motorMsgSend1_.cmd = motorCmdType::MOTOR_OPEN;
		motorMsgSend1_.dataLen = 0x01;
		motorMsgSend1_.data = 0x01;
		motorMsgSend1_.count = 0x01;
		motorMsgSend1_.crc = motorMsgSend1_.cmd + motorMsgSend1_.dataLen +  motorMsgSend1_.data + motorMsgSend1_.count;
		int ret = m_serialPort->write((const char *)&motorMsgSend1_,sizeof(motorMsgSend1_));
		ROS_INFO("m_serialPort->write is %d", ret);	
	}else{
		motorMsgSend1_.cmd = motorCmdType::MOTOR_OPEN;
		motorMsgSend1_.dataLen = 0x01;
		motorMsgSend1_.data = 0x00;
		motorMsgSend1_.count = 0x01;
		motorMsgSend1_.crc = motorMsgSend1_.cmd + motorMsgSend1_.dataLen +  motorMsgSend1_.data + motorMsgSend1_.count;
		int ret = m_serialPort->write((const char *)&motorMsgSend1_,sizeof(motorMsgSend1_));
		ROS_INFO("m_serialPort->write is %d", ret);	
		//releaseSerial();
	}
}

void viewpanel::sendMotorConnectCmd()
{
	if(!ifConnectedMotor){
		if(motorSerialConnect()){
			QMessageBox msgBox;
			msgBox.setText("connect to the serial device failed!");
			msgBox.exec();
			return;
		} 
		motorMsgSend_.cmd = motorCmdType::MOTOR_CONNECT;
		motorMsgSend_.dataLen = 0x00;
		motorMsgSend_.count = 0x01;
		motorMsgSend_.crc = motorMsgSend_.cmd + motorMsgSend_.dataLen + motorMsgSend_.count;

		int ret = m_serialPort->write((const char *)&motorMsgSend_,sizeof(motorMsgSend_));
		ROS_INFO("MOTOR_CONNECT write is %d", ret);	
	}else{
		motorMsgSend_.cmd = motorCmdType::MOTOR_DISCONNECT;
		motorMsgSend_.dataLen = 0x00;
		motorMsgSend_.count = 0x01;
		motorMsgSend_.crc = motorMsgSend_.cmd + motorMsgSend_.dataLen + motorMsgSend_.count;
		int ret = m_serialPort->write((const char *)&motorMsgSend_,sizeof(motorMsgSend_));
		ROS_INFO("MOTOR_DISCONNECT write is %d", ret);
	}
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
		ROS_INFO("adcMsg update");  
	}
#endif
	//pADCchart[0]->setXChart(8192 * frame_index, 8192 * frame_index + 8191);
	//pADCchart[1]->setXChart(8192 * frame_index, 8192 * frame_index + 8191);
	frame_index++;
}

void viewpanel::startPcUdpOnce() {

#if 0
	commandMsg cmdMsg;
	memset(&cmdMsg, 0, sizeof(commandMsg));
	cmdMsg.mHead.usCommand = commandType::POINTCLOUD_UDP_START;
	if(::write(ctrl_sock, &cmdMsg, sizeof(commandMsg)) < 0){
		QMessageBox msgBox;
		msgBox.setText("startPcUdpOnce failed! Disconnect the target");
		msgBox.exec();
		return;
	}	
	udpRecvPCOnce();	
	pcDataProc();
#else
	udpPCContinu_ = false;
	udpPCSingle_ = true;
#endif
}

void viewpanel::startPcUdpContinuous() {
	udpPCContinu_ = true;
}

void viewpanel::udpPcConnect() {

	if(!udpStop_) udpClose();
	if(udpPCStop_){
#if 1
		commandMsg cmdMsg;
		memset(&cmdMsg, 0, sizeof(commandMsg));
		cmdMsg.mHead.usCommand = commandType::POINTCLOUD_UDP_START;
		if(::write(ctrl_sock, &cmdMsg, sizeof(commandMsg)) < 0){
			QMessageBox msgBox;
			msgBox.setText("UDP Connect failed!");
			msgBox.exec();
			return;
		}	
#endif
		if(udpRecvPCConnect()) return;
#if 1	
		startPcTask();
#endif
		pcSwitchBtn->setStyleSheet("color: green");
		pcSwitchBtn->setText("&Close PC");
	}else{
		udpPcClose();
	}
}

void viewpanel::udpConnect() {
	if(!udpPCStop_) udpPcClose();
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
		settingADCSavebutton->setStyleSheet("color: green");
		settingADCSavebutton->setText("&Stop FFT-ADC");
	}else{
		udpClose();
		//QMessageBox msgBox;
		//msgBox.setText("UDP Connection is already working, please stop it first !");
		//msgBox.exec();		
	}
}

void viewpanel::startPcTask() {
	vx_task_set_default_create_params(&bst_params);
	bst_params.app_var = this;
	bst_params.task_mode = 0;
	bst_params.task_main = TaskFuncPCRecv;
	vx_task_create(&bst_task[3], &bst_params);  

	vx_task_set_default_create_params(&bst_params);
	bst_params.app_var = this;
	bst_params.task_mode = 0;
	bst_params.task_main = TaskFuncPCParse;
	vx_task_create(&bst_task[4], &bst_params); 
}

void viewpanel::pcDataFindMaxMin(udpPcMsgOneFrame* pmsg)
{
	if(!pmsg) return;
	distance_min  = 0.0;
	distance_max = 0.0;
	indensity_min = 0.0;
	indensity_max = 0.0;
	speed_min = 0.0;
	speed_max = 0.0;
	int pcFrameSize = pmsg->pcDataOneFrame.size();
	for(int j = 0; j < pcFrameSize; j++)
	{
		for(int index = 0; index < UDP_PC_SIZE_SINGLE_V01; index++)
		{
			double horizontal_m = pmsg->pcDataOneFrame[j].UDP_PC_payload[index].pcmHorizontal * horizontal_bin;
			if(horizontal_m > 360.0) horizontal_m -= 360.0;
			if( horizontal_m < leftAngle_offset && horizontal_m > rightAngle_offset) continue;
			double distance_m = pmsg->pcDataOneFrame[j].UDP_PC_payload[index].pcmDistance * distance_bin - distance_offset;
			if( distance_m < 0.0 || distance_m > 101.0) continue;
			int speed_m = pmsg->pcDataOneFrame[j].UDP_PC_payload[index].pcmSpeed * speed_bin;
			if( speed_m < -60.0 || speed_m > 60.0) continue;
			int  indensity_m = pmsg->pcDataOneFrame[j].UDP_PC_payload[index].pcmIndensity;
			if(distance_min > distance_m )distance_min = distance_m;
			if(distance_max < distance_m )distance_max = distance_m;
			if(indensity_min > indensity_m )indensity_min = indensity_m;
			if(indensity_max < indensity_m )indensity_max = indensity_m;
			if(speed_min > speed_m )speed_min = speed_m;
			if(speed_max < speed_m )speed_max = speed_m;
		}
	}
	std::cout << "distance_min: " << distance_min << " distance_max: " << distance_max \
	<< " indensity_min: " << indensity_min << " indensity_max: " << indensity_max << " speed_min: " << speed_min \ 
	<< " speed_max: " << speed_max << std::endl; 
}

void viewpanel::pcDataProc()
{

	udpPcMsgOneFrame* pmsg = nullptr;
	static long long frame_index = 0;
	auto start = std::chrono::steady_clock::now();
	if(udpPcMsg_done_buf_queue.get(pmsg)){
		return;
	}

	int pcFrameSize = 0;
	double horizontal_m = 0.0;
	bool begin_save = false;
	bool one_frame_360 = false;
	double horizontal_last = 0.0;
#if 1
	if(udpPCBuff_last.pcDataOneFrame.empty()){
		pcFrameSize = pmsg->pcDataOneFrame.size(); 
		udpPCBuff_last.frameCounterLast = pmsg->pcDataOneFrame[0].UDP_PC_head.uphFrameCounter;
		for(int j = 0; j < pcFrameSize; j++)
		{
			for(int index = 0; index < UDP_PC_SIZE_SINGLE_V01; index++){	
				udpPCBuff_last.pcDataOneFrame.push_back(pmsg->pcDataOneFrame[j].UDP_PC_payload[index]);
				udpPCBuff_last.frameCounter.push_back(pmsg->pcDataOneFrame[j].UDP_PC_head.uphFrameCounter);
			}
		}
		udpPcMsg_free_buf_queue.put(pmsg);
		if(udpPcMsg_done_buf_queue.get(pmsg)){
			return;
		}
	}
	pcFrameSize = udpPCBuff_last.pcDataOneFrame.size();
	udpPcMsgOneFrame360 oneFrame360;
	oneFrame360.pcDataOneFrame.clear();
	oneFrame360.frameCounterLast = udpPCBuff_last.frameCounterLast;
	for(int j = 0; j < pcFrameSize; j++)
	{
		horizontal_m = udpPCBuff_last.pcDataOneFrame[j].pcmHorizontal * horizontal_bin;
		if(horizontal_m > 360.0) horizontal_m -= 360.0;
		if(horizontal_m >= 0.0 && horizontal_m < 1.0) {
			begin_save = true;
		}
		if(!begin_save) continue;
		oneFrame360.pcDataOneFrame.push_back(udpPCBuff_last.pcDataOneFrame[j]);
		oneFrame360.frameCounter.push_back(udpPCBuff_last.frameCounter[j]);

		if(begin_save && horizontal_m > 359.5 && horizontal_m < 360.0){
			std::cout << "warnning, one_frame_360 is done in udpPCBuff_last" << horizontal_m << std::endl;
			one_frame_360 = true;
			break;
		}
	}
	//frame_index++;

	if(oneFrame360.pcDataOneFrame.empty()){
		std::cout << "=============oneFrame360.pcDataOneFrame is empty, can't find 0 , frame index is " << udpPCBuff_last.frameCounter[0]<< std::endl;
	}
	udpPCBuff_last.pcDataOneFrame.clear();
	udpPCBuff_last.frameCounter.clear();
	begin_save = false;
	pcFrameSize = pmsg->pcDataOneFrame.size();
	oneFrame360.frameCounterCur = pmsg->pcDataOneFrame[0].UDP_PC_head.uphFrameCounter;
 	//auto last =  oneFrame360.pcDataOneFrame.end() - 1;
	if(!oneFrame360.pcDataOneFrame.empty())horizontal_last = oneFrame360.pcDataOneFrame[oneFrame360.pcDataOneFrame.size() - 1].pcmHorizontal * horizontal_bin;
	if(horizontal_last > 360.0) horizontal_last -= 360.0; 
	bool find_flag = true;
	if(!one_frame_360){
		for(int j = 0; j < pcFrameSize; j++)
		{
			for(int index = 0; index < UDP_PC_SIZE_SINGLE_V01; index++){	
				horizontal_m = pmsg->pcDataOneFrame[j].UDP_PC_payload[index].pcmHorizontal * horizontal_bin;
				if(horizontal_m > 360.0) horizontal_m -= 360.0;
				if((horizontal_last > horizontal_m) && find_flag) {
					continue;
				}else{
					find_flag = false;
				}
				if(horizontal_m > 359.5 && horizontal_m < 360.0){
					oneFrame360.pcDataOneFrame.push_back(pmsg->pcDataOneFrame[j].UDP_PC_payload[index]);
					oneFrame360.frameCounter.push_back(pmsg->pcDataOneFrame[j].UDP_PC_head.uphFrameCounter);
					one_frame_360 = true;
					continue;
				}				
				if(one_frame_360){
					udpPCBuff_last.pcDataOneFrame.push_back(pmsg->pcDataOneFrame[j].UDP_PC_payload[index]);
					udpPCBuff_last.frameCounter.push_back(pmsg->pcDataOneFrame[j].UDP_PC_head.uphFrameCounter);
					udpPCBuff_last.frameCounterLast = pmsg->pcDataOneFrame[j].UDP_PC_head.uphFrameCounter;
				}
				else{
					oneFrame360.frameCounter.push_back(pmsg->pcDataOneFrame[j].UDP_PC_head.uphFrameCounter);
					oneFrame360.pcDataOneFrame.push_back(pmsg->pcDataOneFrame[j].UDP_PC_payload[index]);
				}
			}
		}	
	}
#endif
	if(!udpPCContinu_ && !udpPCSingle_){
		udpPcMsg_free_buf_queue.put(pmsg);
		return;
	}
	distance_offset = distance_Offset_edit->text().toDouble();
	rotation_offset = rotate_angle_edit->text().toDouble();
	leftAngle_offset = left_angle_edit->text().toDouble();
	rightAngle_offset = right_angle_edit->text().toDouble();
	color_base = color_base_edit->text().toDouble();
	QString strColor = colorCombo->currentText();
	//std::cout << "rotation_offset: " << rotation_offset << ", leftAngle_offset: " << leftAngle_offset \
	<< ", rightAngle_offset: " << rightAngle_offset << std::endl; 
	pcDataFindMaxMin(pmsg);
	udpPcMsg_free_buf_queue.put(pmsg);
	double distance_m;
	double vertical_m;
	double intensity_m;
	double speed_m;
	int index_rgb;
#if 1
	time_t rawtime;
	struct tm *ptminfo;
	time(&rawtime);
	ptminfo = localtime(&rawtime);
	printf("current: %02d-%02d-%02d %02d:%02d:%02d\n",
	ptminfo->tm_year + 1900, ptminfo->tm_mon + 1, ptminfo->tm_mday,
	ptminfo->tm_hour, ptminfo->tm_min, ptminfo->tm_sec);
	std::string csvPath;
	csvPath = save_folder_.toStdString() + "/pc_test_data_convert_" + 
	std::to_string(ptminfo->tm_year + 1900) + 
	"-" + std::to_string(ptminfo->tm_mon + 1) +
	"-" + std::to_string(ptminfo->tm_mday) +
	"-" + std::to_string(ptminfo->tm_hour) +
	"-" + std::to_string(ptminfo->tm_min) +
	"-" + std::to_string(ptminfo->tm_sec) +
	+".csv";
	if(udpPCSingle_) ROS_INFO("will save udp pc data, csvPath is %s \n", csvPath.c_str());
	std::ofstream csvfile;
	if(udpPCSingle_) {
		csvfile.open(csvPath, std::ios::out); 
		csvfile << "intensity" << "," << "distance(m)" << "," 
		<< "speed(m/s)" << "," << "Vertical angle(degree)" << "," << "Horizontal angle(degree)" << "," << "frame count" \
		<<"\n";
	}
	
#endif
	pcFrameSize = oneFrame360.pcDataOneFrame.size();
	std::cout << "pcFrameSize is " << pcFrameSize << std::endl;
	cloud.points.resize(pcFrameSize);
	int realSize = 0;
	for(int j = 0; j < pcFrameSize; j++)
	{
		horizontal_m = oneFrame360.pcDataOneFrame[j].pcmHorizontal * horizontal_bin;
		if(horizontal_m > 360.0) horizontal_m -= 360.0;
		if( horizontal_m < leftAngle_offset && horizontal_m > rightAngle_offset) continue;
		realSize++;
		speed_m = oneFrame360.pcDataOneFrame[j].pcmSpeed * speed_bin;
		distance_m = oneFrame360.pcDataOneFrame[j].pcmDistance * distance_bin - distance_offset;
		vertical_m = oneFrame360.pcDataOneFrame[j].pcmVertical * vertical_bin + vertical_offset;
		intensity_m = oneFrame360.pcDataOneFrame[j].pcmIndensity;
		if(udpPCSingle_) {
			csvfile << oneFrame360.pcDataOneFrame[j].pcmIndensity << "," << distance_m << "," << speed_m << "," \
			<< vertical_m << ", " << horizontal_m << "," << oneFrame360.frameCounter[j] <<  "\n";
		}
		if(distance_m < 0.0) continue;
		cloud.points[j].vertical = vertical_m;
		cloud.points[j].horizontal = horizontal_m;
		cloud.points[j].distance = distance_m;
		cloud.points[j].indensity = intensity_m;
		cloud.points[j].speed = speed_m;
		horizontal_m += rotation_offset;
		cloud.points[j].x = distance_m * cos(vertical_m * PI_FMCW / 180) * \
															cos(horizontal_m * PI_FMCW / 180);
		cloud.points[j].y = distance_m * cos(vertical_m * PI_FMCW / 180) * \
															sin(horizontal_m * PI_FMCW / 180) * (-1.0);
		cloud.points[j].z = distance_m * sin(vertical_m * PI_FMCW / 180) * (1.0);
		if(strColor == "range")
			index_rgb = distance_m / color_base * R_V_g.size();
		else if(strColor == "intensity")
			index_rgb = (intensity_m - indensity_min) / (indensity_max - indensity_min) * R_V_g.size();
		else if(strColor == "speed"){
			uint8_t r, g, b = 0;
			if(speed_m < 0 && speed_m < - 0.4){
				r = 255;
				g = 0;
				b = 0;
			} else if(speed_m > 0 && speed_m > 0.4) {
				r = 0;
				g = 0;
				b = 240;
			}else {
				r = 192;
				g = 192;
				b = 192;					
			}
			cloud.points[j].r = r;
			cloud.points[j].g = g;
			cloud.points[j].b = b;
			//index_rgb = (speed_m + color_base) / (color_base * 2) * R_V_g.size();
			//index_rgb = R_V_g.size() - 1 - index_rgb;
		}

		if(index_rgb > (R_V_g.size() - 1)) index_rgb = R_V_g.size() - 1;
		if(index_rgb < 0) index_rgb = 0;
		if(strColor != "speed"){
			cloud.points[j].r = R_V_g[index_rgb];
			cloud.points[j].g = G_V_g[index_rgb];
			cloud.points[j].b = B_V_g[index_rgb];
		}
	}
	std::cout << "realSize is " << realSize << std::endl;
	csvfile.close();
	if(udpPCSingle_) udpPCSingle_ = false;
	output.header.stamp = ros::Time::now();
	pcl::toROSMsg(cloud,output);
	output.header.frame_id = "image_lidar";
	fmcw_pcl_pub.publish(output);
	cloud.clear();
	auto end = std::chrono::steady_clock::now();

}

void viewpanel::pcParseLoop()
{
	std::cout << "enter pcParseLoop main" << std::endl;
    std::chrono::duration<double> elapsed;
	while(!terminating)
	{
		//ROS_INFO("============enter udpParseLoop");
		auto start = std::chrono::steady_clock::now();
		pcDataProc();
		auto end = std::chrono::steady_clock::now();
		elapsed = end - start;
		std::cout << "time for pub one frame pc data: " <<  elapsed.count() * 1000 << " ms" << std::endl;    
		if(udpPCStop_) break;
	}
	std::cout << "quit pcParseLoop" << std::endl;
}
void viewpanel::udpParseLoop()
{
	std::cout << "enter udpParseLoop main" << std::endl;
    std::chrono::duration<double> elapsed;
	while(!terminating)
	{
		//ROS_INFO("============enter udpParseLoop");
		udp_ADC_FFT_Msg* pmsg = nullptr;
		auto start = std::chrono::steady_clock::now();
		if(udpMsg_done_buf_queue.get(pmsg) == 0){
			parseFFTData(pmsg->fftDataV);
			parseADCData(pmsg->adcDataV);
			udpMsg_free_buf_queue.put(pmsg);
		}else{
			std::cout << "warning!!udpMsg_done_buf_queue get timeout!!!" << std::endl;
		}
		auto end = std::chrono::steady_clock::now();
		elapsed = end - start;
		std::cout << "time for udpParseloop: " <<  elapsed.count() * 1000 << " ms" << std::endl;    
		if(udpStop_) break;
	}
	std::cout << "quit udpParseLoop" << std::endl;
}

int viewpanel::udpRecvPCConnect()
{
	lidar_UDP_pc_port = udp_pc_port_edit->text().toInt();

	udpRecvPCSocketFd_ = socket(AF_INET, SOCK_DGRAM, 0);
	if(udpRecvPCSocketFd_ < 0)
	{
		QMessageBox msgBox;
		msgBox.setText("udp pc socket create fail!");
		msgBox.exec();
		return -1;
	}

	struct timeval timeout_recv = {3, 0};
	setsockopt(udpRecvPCSocketFd_, SOL_SOCKET, SO_RCVTIMEO, &timeout_recv, sizeof(timeout_recv)); //recv timeout

	struct sockaddr_in ser_addr;
	struct sockaddr_in src_addr;
	socklen_t len;
	len = sizeof(sockaddr);
	memset(&ser_addr, 0, sizeof(ser_addr));
	memset(&src_addr, 0, sizeof(src_addr));
	ser_addr.sin_family = AF_INET;
	//ser_addr.sin_addr.s_addr = inet_addr(lidar_ip.c_str());
	ser_addr.sin_addr.s_addr = htonl(INADDR_ANY);  //注意网络序转换
	ser_addr.sin_port = htons(lidar_UDP_pc_port);  //注意网络序转换
	std::cout << "lidar_UDP_pc_port is " << lidar_UDP_pc_port << std::endl;

    int ret = ::bind(udpRecvPCSocketFd_, (struct sockaddr*)&ser_addr, sizeof(ser_addr));
    if(ret < 0)
    {
		QMessageBox msgBox;
		msgBox.setText("udp socket bind fail!");
		msgBox.exec();
		return -1;
    }
	udpPCStop_ = false;
	return 0;

}

void viewpanel::udpRecvPCOnce()
{
	uint32_t head_frame_index = 0;
	bool ifLost  = false;
    std::chrono::duration<double> elapsed;	
	int ret = 0;
	struct sockaddr_in src_addr;
	socklen_t len;
	len = sizeof(sockaddr);
	auto start = std::chrono::steady_clock::now();
	pcDataOneFrame_.clear();
	ifLost  = false;
	for(int i = 0; i < UDP_PC_TIMES_PER_FRAME; i++){
		memset(&pcDataRaw_, 0, sizeof(pcDataRaw_));
		//printf("ready recv udp msg!\n");
		ret = recvfrom(udpRecvPCSocketFd_, &pcDataRaw_, sizeof(pcDataRaw_), MSG_WAITALL, (struct sockaddr*)&src_addr, &len);  //接收来自server的信息
		//printf("recv udp msg! receive byte is %d, g_udpMsg: %c %c %c %c %c\n", ret, g_udpMsg.pcUdpData[11], \
		//g_udpMsg.pcUdpData[13], g_udpMsg.pcUdpData[15], g_udpMsg.pcUdpData[17], g_udpMsg.pcUdpData[19]);
		if(ret <= 0){
			if(udpPCStop_) {
				printf("pc raw udp  quit!\n"); 
				::close(udpRecvPCSocketFd_);
				return;
			}
			ROS_INFO("pc raw data recv failed, continue\n");
			usleep(100*1000);
			i--;
			continue;
		}

		if(i == 0) head_frame_index = pcDataRaw_.UDP_PC_head.uphFrameCounter;
		pcDataOneFrame_.emplace_back(pcDataRaw_);
		if(pcDataRaw_.UDP_PC_head.uphFrameCounter!= head_frame_index) {
			if(pcDataOneFrame_.size() > 2)pcDataOneFrame_.pop_back();
			ifLost = true;
			std::cout << "!!!warnning!!! current usFrameCounter is " << pcDataRaw_.UDP_PC_head.uphFrameCounter
				<< ", head_frame_index is " << head_frame_index << std::endl;	
			break;
		} 
	}
	auto end = std::chrono::steady_clock::now();
	elapsed = end - start;
	std::cout << "time for one frame udp: " <<  elapsed.count() * 1000 << " ms" << std::endl;  
	//std::cout << "!!recv udp pkg successfully! "  << std::endl;
	if(pcDataOneFrame_.size() > 2){
		udpPcMsgOneFrame* pUdp = NULL;
		if(udpPcMsg_free_buf_queue.get(pUdp) == 0){
			pUdp->pcDataOneFrame = pcDataOneFrame_;
			udpPcMsg_done_buf_queue.put(pUdp);	
		}else{
			std::cout << "error!!! udpPcMsg_free_buf_queue timeout!! "  << std::endl;
			return;
		}
	}else{
		QMessageBox msgBox;
		msgBox.setText("udpRecvPCOnce fail! pcDataOneFrame_ points are not enough!!");
		msgBox.exec();
		return;		
	}	
}

void viewpanel::udpRecvPCLoop()
{
	uint32_t head_frame_index = 0;
	bool ifLost  = false;
    std::chrono::duration<double> elapsed;	
	int ret = 0;
	struct sockaddr_in src_addr;
	socklen_t len;
	len = sizeof(sockaddr);
	while(!terminating && !udpPCStop_)
	{
		auto start = std::chrono::steady_clock::now();
		pcDataOneFrame_.clear();
		ifLost  = false;
		for(int i = 0; i < UDP_PC_TIMES_PER_FRAME; i++){
			memset(&pcDataRaw_, 0, sizeof(pcDataRaw_));
			//printf("ready recv udp msg!\n");
			ret = recvfrom(udpRecvPCSocketFd_, &pcDataRaw_, sizeof(pcDataRaw_), MSG_WAITALL, (struct sockaddr*)&src_addr, &len);  //接收来自server的信息
			//printf("recv udp msg! receive byte is %d, g_udpMsg: %c %c %c %c %c\n", ret, g_udpMsg.pcUdpData[11], \
			//g_udpMsg.pcUdpData[13], g_udpMsg.pcUdpData[15], g_udpMsg.pcUdpData[17], g_udpMsg.pcUdpData[19]);
			if(ret <= 0){
				if(udpPCStop_) {
					printf("pc raw udp  quit!\n"); 
    				::close(udpRecvPCSocketFd_);
					return;
				}
				ROS_INFO("pc raw data recv failed, continue\n");
				usleep(100*1000);
				i--;
				continue;
			}

			if(i == 0) head_frame_index = pcDataRaw_.UDP_PC_head.uphFrameCounter;
			pcDataOneFrame_.emplace_back(pcDataRaw_);
			if(pcDataRaw_.UDP_PC_head.uphFrameCounter!= head_frame_index) {
				if(pcDataOneFrame_.size() > 2)pcDataOneFrame_.pop_back();
				ifLost = true;
				std::cout << "!!!warnning!!! current usFrameCounter is " << pcDataRaw_.UDP_PC_head.uphFrameCounter
				 << ", head_frame_index is " << head_frame_index << std::endl;	
				break;
			} 
		}
		auto end = std::chrono::steady_clock::now();
		elapsed = end - start;
		std::cout << "time for one frame udp: " <<  elapsed.count() * 1000 << " ms" << std::endl;  
		//std::cout << "!!recv udp pkg successfully! "  << std::endl;
		if(pcDataOneFrame_.size() > 2){
			udpPcMsgOneFrame* pUdp = NULL;
			if(udpPcMsg_free_buf_queue.get(pUdp) == 0){
				pUdp->pcDataOneFrame = pcDataOneFrame_;
				udpPcMsg_done_buf_queue.put(pUdp);	
			}else{
				return;
			}
		}
	}
	printf("pc udp  quit!\n");	

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
    std::chrono::duration<double> elapsed;
	while(!terminating && !udpStop_)
	{
		auto start = std::chrono::steady_clock::now();
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
				ROS_INFO("fft data recv failed, ret is %d, continue\n", ret);
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
		auto end = std::chrono::steady_clock::now();
		elapsed = end - start;
		std::cout << "time for one frame udp: " <<  elapsed.count() * 1000 << " ms" << std::endl;  

		if(ifLost) {
			std::cout << "warning!!! lost data continue! "  << std::endl;
			continue;
		}
		//std::cout << "!!recv udp pkg successfully! "  << std::endl;
		udp_ADC_FFT_Msg* pUdp = NULL;
		if(udpMsg_free_buf_queue.empty()){
			std::cout << "warning!!! udpMsg_free_buf_queue is empty, will discard old udp msg "  << std::endl;
			udpMsg_done_buf_queue.get(pUdp);
			udpMsg_free_buf_queue.put(pUdp);
			udpMsg_free_buf_queue.get(pUdp);
			pUdp->fftDataV = fftDataV;
			pUdp->adcDataV = adcDataV;
			udpMsg_done_buf_queue.put(pUdp);	
			continue;			
		}
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

int viewpanel::motorSerialConnectTest()
{
	m_serialPort_test = new QSerialPort();//实例化串口类一个对象

	if(m_serialPort_test->isOpen())//如果串口已经打开了 先给他关闭了
	{
		m_serialPort_test->clear();
		m_serialPort_test->close();
	}

	//设置串口名字 假设我们上面已经成功获取到了 并且使用第一个
	//QString serialDevName = motorSerialCombo->currentText();
	m_serialPort_test->setPortName(QString("/dev/pts/2"));

	if(!m_serialPort_test->open(QIODevice::ReadWrite))//用ReadWrite 的模式尝试打开串口
	{
		qDebug()<< QString("/dev/pts/2") <<"open failed!";
		return -1;
	}
	//打开成功
	qDebug()<< QString("/dev/pts/4") <<"open successfully!";
    m_serialPort_test->setBaudRate(QSerialPort::Baud115200,QSerialPort::AllDirections);//设置波特率和读写方向
    m_serialPort_test->setDataBits(QSerialPort::Data8);		//数据位为8位
    m_serialPort_test->setFlowControl(QSerialPort::NoFlowControl);//无流控制
    m_serialPort_test->setParity(QSerialPort::NoParity);	//无校验位
    m_serialPort_test->setStopBits(QSerialPort::OneStop); //一位停止位

	//连接信号槽 当下位机发送数据QSerialPortInfo 会发送个 readyRead 信号,我们定义个槽void receiveInfo()解析数据
	connect(m_serialPort_test,SIGNAL(readyRead()),this,SLOT(recvSerialInfoTest()));
	return 0;
}

int viewpanel::serialClose(QSerialPort* serialPort)
{
	if(serialPort == nullptr) return -1;
	if(serialPort->isOpen())//如果串口已经打开了 先给他关闭了
	{
		serialPort->clear();
		serialPort->close();
		std::cout << "close serialPort" << std::endl;
	}
	return 0;
}

void viewpanel::releaseSerial()
{
	serialClose(m_serialPort);	
	delete m_serialPort;
	m_serialPort = nullptr;
	serialClose(m_serialPort_test);	
	delete m_serialPort_test;
	m_serialPort_test = nullptr;	
}

int viewpanel::motorSerialConnect()
{
	m_serialPort = new QSerialPort();//实例化串口类一个对象

	if(m_serialPort->isOpen())//如果串口已经打开了 先给他关闭了
	{
		m_serialPort->clear();
		m_serialPort->close();
	}

	//设置串口名字 假设我们上面已经成功获取到了 并且使用第一个
	QString serialDevName = motorSerialCombo->currentText();
	m_serialPort->setPortName(serialDevName);

	if(!m_serialPort->open(QIODevice::ReadWrite))//用ReadWrite 的模式尝试打开串口
	{
		qDebug()<< serialDevName <<"open failed!";
		return -1;
	}
	//打开成功
    m_serialPort->setBaudRate(QSerialPort::Baud115200,QSerialPort::AllDirections);//设置波特率和读写方向
    m_serialPort->setDataBits(QSerialPort::Data8);		//数据位为8位
    m_serialPort->setFlowControl(QSerialPort::NoFlowControl);//无流控制
    m_serialPort->setParity(QSerialPort::NoParity);	//无校验位
    m_serialPort->setStopBits(QSerialPort::OneStop); //一位停止位

	//连接信号槽 当下位机发送数据QSerialPortInfo 会发送个 readyRead 信号,我们定义个槽void receiveInfo()解析数据
	connect(m_serialPort,SIGNAL(readyRead()),this,SLOT(recvSerialInfo()));
	return 0;
}

int viewpanel::motorConnect()
{
	int one = 1;
	lidar_ip = ip_edit->text().toStdString();
	motor_port = motorConnectPortLine->text().toInt();
	ROS_INFO("lidar_ip is %s, motor_port is %d", lidar_ip.c_str(), motor_port);
	struct sockaddr_in ctrl_serv_addr;
	if ((motor_ctrl_sock=socket(AF_INET, SOCK_STREAM, 0))==-1){
		ROS_DEBUG("ERROR: Could not create socket!");
		return -1;
	}
	int nRecvBuf=320 * 1024;//设置为32K
	setsockopt(motor_ctrl_sock, SOL_SOCKET,SO_RCVBUF,(const char*)&nRecvBuf,sizeof(int));

	int nSendBuf=320 * 1024;//设置为32K
	setsockopt(motor_ctrl_sock, SOL_SOCKET, SO_SNDBUF,(const char*)&nSendBuf,sizeof(int));
	setsockopt(motor_ctrl_sock, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));


#if 1
	struct timeval timeout_send = {2, 0};
	setsockopt(motor_ctrl_sock, SOL_SOCKET, SO_SNDTIMEO, &timeout_send, sizeof(timeout_send)); //send timeout

	struct timeval timeout_recv = {2, 0};
	setsockopt(motor_ctrl_sock, SOL_SOCKET, SO_RCVTIMEO, &timeout_recv, sizeof(timeout_recv)); //recv timeout

#endif
	memset(&ctrl_serv_addr, 0, sizeof(ctrl_serv_addr));
	ctrl_serv_addr.sin_family = AF_INET;
	ctrl_serv_addr.sin_addr.s_addr = inet_addr(lidar_ip.c_str());
	ctrl_serv_addr.sin_port = htons(motor_port);

	if(::connect(motor_ctrl_sock, (struct sockaddr*)&ctrl_serv_addr, sizeof(ctrl_serv_addr))<0)
	{
		ROS_INFO("Failed to connect to lidar_ip %s port: %d", lidar_ip.c_str(), motor_port);
		return -2; /* Completely fail only if first radar didn't connect */
	}
	//fcntl(ctrl_sock, F_SETFL, O_NONBLOCK); /* Set the socket to non blocking mode */
	return 0;
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
	for(int i = 0; i < 4; i++){
		m_reg_addr_[i] = settings.value("Reg Addr " + QString::number(i), regAddrList[i]).toString();
		m_reg_value_[i] = settings.value("Reg Value " + QString::number(i), regValueList[i]).toString();
	}
	lidar_UDP_PC_port_ = settings.value("UDP PC Port","8001").toString();
	lidar_ctrl_port_ = settings.value("TCP Port","5000").toString();
	lidar_UDP_port_ = settings.value("UDP Port","8000").toString();
	distance_offset_ = settings.value("Distance Offset","0.0").toString();
	rotation_offset = settings.value("rotate angle","45.0").toDouble();
	leftAngle_offset = settings.value("left angle","306.0").toDouble();
	color_base = settings.value("color base","10.0").toDouble();
	point_size = settings.value("point size","0.03").toDouble();
	cell_size = settings.value("cell size","10.0").toDouble();
	axes_size = settings.value("axes size","0.1").toDouble();
	rightAngle_offset = settings.value("right angle","125.0").toDouble();
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

void viewpanel::setCheckBoxUnvaild(QCheckBox* checkBox)
{
	checkBox->setCheckable(true);
	QPalette palette = checkBox->palette();
	palette.setBrush(QPalette::Base,palette.brush(QPalette::Disabled, QPalette::Base));
	checkBox->setPalette(palette);
	checkBox->setStyleSheet("QCheckBox::indicator {width: 20px; height: 20px;}");	
}

void viewpanel::save_settings(void )
{
	ROS_INFO("enter save_settings");
	QSettings settings(QCoreApplication::organizationName(),
		QCoreApplication::applicationName());
	settings.setValue("IP Addr", ip_edit->text());
	settings.setValue("TCP Port", port_edit->text());
	settings.setValue("UDP Port", udp_port_edit->text());
	settings.setValue("UDP PC Port", udp_pc_port_edit->text());
	settings.setValue("Distance Offset", distance_Offset_edit->text());
	settings.setValue("rotate angle", rotate_angle_edit->text());
	settings.setValue("left angle", left_angle_edit->text());
	settings.setValue("right angle", right_angle_edit->text());

	settings.setValue("cell size", cell_size_edit->text());
	settings.setValue("point size", point_size_edit->text());
	settings.setValue("color base", color_base_edit->text());
	settings.setValue("axes size", axes_size_edit->text());

	settings.setValue("Power Offset", power_Offset_edit->text());
	settings.setValue("Save Folder", save_folder_);
	for(int i = 0; i < 4; i++){
		settings.setValue("Reg Addr " + QString::number(i), regAddr_line[i]->text());
		settings.setValue("Reg Value " + QString::number(i), regVal_line[i]->text());
	}
}
