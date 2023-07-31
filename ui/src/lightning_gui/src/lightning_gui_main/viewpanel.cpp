
#include "geometry_msgs/PolygonStamped.h"
#include "geometry_msgs/Point32.h"
#include "mainwindow.h"
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
#include "rqt_gauges/my_plugin.h"
#include "rqt_gauges/qcgaugewidget.h"
#include "viewpanel.h"
#include <vector>
#include <netinet/tcp.h>

#define CTRL_SOCKET 0
#define DEFAULT_AZIMUTH_BIN 0
#define DEBUG_UI 0
#define SIGN_LIMIT_NUM 32767
#define SIGN_OFFSET_NUM 65536
#define SINGELE_PC_SAVE 0


extern int terminating;

QStringList motorDataString = {
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

QStringList sfString = {
	"速度滤波",
	"距离滤波",
	"强度滤波",
	"半径滤波"
};
static QStringList regAddrList = {
	"0xa00a0014",
	"0xa00a0018",
	"0xa00a0004",
	"0xa0070010",
	"0",
	"0"
};

static QStringList regValueList = {
	"0xFFFF00B4",
	"0x0A6CFC8F",
	"0x91EC0001",
	"0x40001183",
	"0",
	"0"
};
static std::vector<std::string> cfarAddr = {"A0070060", "A0070064", "A0070068", "A007006C"};
static std::string softVersionName = "motor_xlidar_B_appA_0_3_2022_12_14";
static std::string hardVersionName = "50120-0126-V0.2_20221115";

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
										
static std::vector<std::vector<uint8_t>> chanColorV = {{0xFF, 0x33, 0x00}, {0xFF, 0xFF, 0x00}, {0x00, 0xFF, 0x33}, {0xFF, 0x66, 0xFF}};

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

uint16_t FloatSum(float data)
{
	uint8_t dataU8[4];
	memcpy(dataU8, &data, 4);
	return dataU8[0] + dataU8[1] + dataU8[2] + dataU8[3];
}

inline float calcFpsAndTransSpeed(int n)
{
	static auto last = std::chrono::steady_clock::now();
	static long byteNum = 0;
	std::chrono::duration<double> elapsed;
	byteNum += n;
	auto current = std::chrono::steady_clock::now();
	elapsed = current - last;
	if(elapsed.count() * 1000  > 3 * 1000){
		last = current;
		float res = (float) (byteNum * 1000) / (elapsed.count() * 1000);
		byteNum = 0;
		return res;
	}
	return -1;
}

inline int tcpSocketCheck(int sock)
{
	struct tcp_info info; 
	int len = sizeof(info); 
	getsockopt(sock, IPPROTO_TCP, TCP_INFO, &info, (socklen_t *)&len);
	if((info.tcpi_state == TCP_ESTABLISHED))
		return 0;
	else 
		return -1;
}

bool check_file_exist(const std::string &path) {
#ifdef _MSC_VER  
	bool ret = 0 == _access(path.c_str(), 0);
#else
	bool ret = 0 == access(path.c_str(), F_OK);
#endif
	if (!ret) {
		//LOG(INFO) << path << " not exist";
	}
	return ret;
}

size_t GetFileSize(const std::string &filepath) {
	FILE *fd = fopen(filepath.c_str(), "rb");
	if (fd == NULL) {
		//LOG(ERROR) << "Failed to open file " << filepath;
		return 0;
	}
	// Determine size of the file
	fseek(fd, 0, SEEK_END);
	size_t file_length = static_cast<size_t>(ftell(fd));
	fseek(fd, 0, SEEK_SET);
	fclose(fd);

	return file_length;
}
static uint8_t *encode_cali_data = nullptr;
static long file_size_g = 0;
uint8_t * LoadDat(const char *cali_file_path)
{
	if (NULL == cali_file_path) {
		//LOG(ERROR) << "Bad cali_file_path " LOG_END;
		return nullptr;
	}
	if (!check_file_exist(cali_file_path)) {
		//LOG(ERROR) << "Cali file " << cali_file_path << " not exist " LOG_END;
		return nullptr;
	}
	auto file_size = GetFileSize(cali_file_path);
    std::cout << "file_size is " << file_size << std::endl;
	if (file_size < 1000) {
		//LOG(ERROR) << "Bad cali file size " << file_size LOG_END;
		return nullptr;
	}

	FILE *file = fopen(cali_file_path, "rb");
	uint8_t *encode_cali_data = (uint8_t *)malloc(file_size);
    file_size_g = file_size;
	fseek(file, 0, SEEK_SET);
	fread(encode_cali_data, file_size, 1, file);
    return encode_cali_data;
}

/* Constructor for the viewpanel. */
viewpanel::viewpanel(QTabWidget* parent )
	: QTabWidget( parent ), ifConnected(false), ifSave(false), \
	save_folder_(QString(".")), udpFftAdcStop_(true), showBlack(true), ifShowdB_(FFT_DB),\
	power_offset(0.0),ifConnectedMotorSerial(false), ifConnectedMotorTcp(false),\
	ifOpenMotor(false), udpPCStop_(true), udpPCContinu_(true), udpPCSingle_(false),\
	ifStarted(false),saveadc_(false), oneFramePure(true), ifConnectedStateTcp(false), ctrl_sock(-1), modeFilter_(0), \
	th_radius(1), radius_sf(0.1), width_radius(0), scale_text_size_(1)
{
	init_queue();
	memset(&cmdMsg_, 0, sizeof(cmdMsg_));
	memset(&motorMsgSend_, 0, sizeof(motorMsgSend_));
	cmdMsg_.mHead.usPrefix = 0xeeff;
	cmdMsg_.mHead.usType = 0x10;
	cmdMsg_.mHead.usPayloadCrc = 0x00;
	cmdMsg_.mHead.unLength = 12;
	motorMsgSend_.header.mHead = 0xaa55;
	motorMsgSend1_.header.mHead = 0xaa55;
	motorMsgPid_.header.mHead = 0xaa55;
	motorMsgShow_.header.mHead = 0xaa55;
	motorMsgWorkMode_.header.mHead = 0xaa55;
	motorMsgShowCycle_.header.mHead = 0xaa55;
	distance_min  = 0.0;
	distance_max = 0.0;
	indensity_min = 0.0;
	indensity_max = 0.0;
	speed_min = 0.0;
	speed_max = 0.0;

	power_index = {0, 1, 10, 50, 100, 200, 500, 1000, 2000};
	load_settings();
	CreatPCWindow();
	CreatFFTWindow();
	CreatADCWindow();
	CreatMotorWindow();
	CreatStateDetectWindow();
/* 	systemMonitor_m  = new SystemMonitor();
	this->addTab(systemMonitor_m, "test"); */
	CreatConnect();
	registerPointcloudRviz();
	resize(QDesktopWidget().availableGeometry(this).size() * 0.85);
	std::cout << "R: " << R_V_g.size() << " G: " << G_V_g.size() <<  " B: " << B_V_g.size() << std::endl;
}

// Destructor.
viewpanel::~viewpanel()
{
	if(!udpFftAdcStop_) udpFftAdcClose();
	if(!udpPCStop_) udpPcClose();
	delete manager_;
	save_settings();
	::close(udpFftAdcSocketFd_);
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
	for(int i = 0; i< 12288;i++) 
	{
		x_FFT.append(i);
		x_FFT_1.append(-12287 + i);
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
	motorMsg_free_buf_queue.setParam("motorMsg_free_buf_queue", MAX_BUFF_LEN);
	motorMsg_done_buf_queue.setParam("motorMsg_done_buf_queue", MAX_BUFF_LEN);
	stateMsg_free_buf_queue.setParam("stateMsg_free_buf_queue", MAX_BUFF_LEN);
	stateMsg_done_buf_queue.setParam("stateMsg_done_buf_queue", MAX_BUFF_LEN);


    for (int loop = 0; loop < 4; loop++) {
        fftMsg *fbuf0 = &fftBuff[loop];
        adcMsg *fbuf1 = &adcBuff[loop];
        udp_ADC_FFT_Msg *fbuf2 = &udpFABuff[loop];
        udpPcMsgOneFrame *fbuf3 = &udpPCBuff[loop];
		motorMaxBuff *fbuf4 = &motorBuff[loop];
		stateMaxBuff *fbuf5 = &stateBuff[loop];
        fftMsg_free_buf_queue.put(fbuf0);
        adcMsg_free_buf_queue.put(fbuf1);
        udpMsg_free_buf_queue.put(fbuf2);
        udpPcMsg_free_buf_queue.put(fbuf3);
		motorMsg_free_buf_queue.put(fbuf4);
		stateMsg_free_buf_queue.put(fbuf5);
    }
	vertical_bin = 1 / 256.0; 
	speed_bin = 1 / 128.0; 
	horizontal_bin = 360.0 / 65536.0; 
	distance_bin = 1 / 32768.0; 
	vertical_offset = -2.5;
	udpPCBuff_last.pcDataOneFrame.clear();
	udpPCBuff_last.frameCounter.clear();
	motorSerialConnectTest();
	readMotorItemsFile();
	//simulateADCData();
}

void viewpanel::loadPCFile(void){

	QSettings settings(QCoreApplication::organizationName(),
	QCoreApplication::applicationName());

	lastPCRawPath_ = settings.value("lastPCRawPath_",".").toString();

	loadPCFile_  =  QFileDialog::getOpenFileName(
											this,
											"Choose pc raw file",
											lastPCRawPath_,
											"pc raw files (*.bin)",0,QFileDialog::DontUseNativeDialog);	

	if(loadPCFile_.isNull()) return; 	
	std::string temp = loadPCFile_.toStdString();
	lastPCRawPath_ = QString::fromStdString(temp.substr(0, temp.find_last_of('/')));
	settings.setValue("lastPCRawPath_", lastPCRawPath_);
	qDebug() << "loadPCFile_ is " << loadPCFile_;
	std_msgs::String status_message;
	status_message.data = loadPCFile_.toStdString();
	fmcw_pc_datapath_pub.publish(status_message);
	QMessageBox msgBox;
	msgBox.setText("load pc raw file success!");
	msgBox.exec();
	//ros::spinOnce();

}
void viewpanel::loadAlgFile(void){
	QSettings settings(QCoreApplication::organizationName(),
	QCoreApplication::applicationName());

	lastAlgPath_ = settings.value("lastAlgPath_",".").toString();

	loadLidarFile_  =  QFileDialog::getOpenFileName(
											this,
											"Choose register config file",
											lastAlgPath_,
											"register config files (*.csv)",0,QFileDialog::DontUseNativeDialog);	

	if(loadLidarFile_.isNull()) return; 	

	std::string temp = loadLidarFile_.toStdString();
	lastAlgPath_ = QString::fromStdString(temp.substr(0, temp.find_last_of('/')));
	settings.setValue("lastAlgPath_", lastAlgPath_);

	qDebug() << "loadLidarFile_ is " << loadLidarFile_;
    std::ifstream csv_data(loadLidarFile_.toStdString(), std::ios::in);
    std::string line;

    if (!csv_data.is_open())
    {
        std::cout << "Error: opening file fail" << std::endl;
        std::exit(1);
    }
    std::istringstream sin;         //将整行字符串line读入到字符串istringstream中
    std::vector<std::string> words; //声明一个字符串向量
    std::string word;
	wordsVal.clear();
	wordsAddr.clear();

    // 读取标题行
    std::getline(csv_data, line);
	bool readNext = false;
    // 读取数据
    while (std::getline(csv_data, line))
    {
        sin.clear();
        sin.str(line);
        words.clear();
        while (std::getline(sin, word, ',')) //将字符串流sin中的字符读到field字符串中，以逗号为分隔符
        {
            //words.push_back(word); //将每一格中的数据逐个push
			if(readNext) {
				std::cout << word << std::endl;
				readNext = false;
				wordsVal.push_back(word);
			}
			std::string temp;
			temp.assign(word, 0, 3);
			if(temp == "A00") {
				std::cout << word << " ";
				readNext = true;
				wordsAddr.push_back(word);
			}
            // std::cout << atol(word.c_str());
        }
        std::cout << std::endl;
        // do something。。。
    }
    csv_data.close();

#if 1
	for(int i = 0; i < wordsAddr.size(); i++){
		std::string strAddr = wordsAddr[i];
		std::string strValue = wordsVal[i];
		std::cout << "strAddr: " << strAddr << " strValue: "  << strValue << std::endl;
		//unsigned int x;
		std::stringstream ss;
		ss << std::hex << strAddr;
		ss >> cmdMsg_.mCommandVal[0];
		std::stringstream tt;
		tt << std::hex << strValue;
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
		usleep(1000);
	}
	QMessageBox msgBox;
	msgBox.setText("write register success!");
	msgBox.exec();

#endif
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

void viewpanel::setLoadFileType( void ){
	QString str = loadDataCombo->currentText();
	loadFileType_.assign(str.toStdString());
}

void viewpanel::startControl(void){
}

void viewpanel::connectControl(void){
	if(!ifStarted){
		if(lidarConnect() < 0 || stateConnect() < 0){
			QMessageBox msgBox;
			msgBox.setText("connect to the lidar failed!");
			msgBox.exec();
		}else {
			//configRegLidar();
			lidar_connect_button->setStyleSheet("QPushButton{background-color:rgba(127, 255, 0, 100);}");
			lidar_connect_button->setText("&Disconnect");
			ifStarted = true;
			startStateDectTask();
			commandMsg cmdMsg;
			memset(&cmdMsg, 0, sizeof(commandMsg));
			cmdMsg.mHead.usCommand = commandType::FFT_ADC_READ_SETCH;
			cmdMsg.mCommandVal[0] = fftChCombo->currentText().toInt() - 1;
			if(::write(ctrl_sock, &cmdMsg, sizeof(commandMsg)) < 0){
				QMessageBox msgBox;
				msgBox.setText("fftChannelChange failed!");
				msgBox.exec();
				return;
			}
			for(int i = 0; i < edfaWarnLEDV.size(); i++){
				setLED(edfaWarnLEDV[i], C_GREEN);
			}
		}
	}else {
		lidar_connect_button->setStyleSheet("QPushButton{background-color:rgba(192, 192, 192, 100);}");
		lidar_connect_button->setText("&Connect");
		ifStarted = false;
		ifConnectedStateTcp = false;
		if(!udpFftAdcStop_) udpFftAdcClose();
		if(!udpPCStop_) udpPcClose();
		::close(ctrl_sock);
		::close(state_ctrl_sock);
		vx_task_delete(&bst_task[TASK_SYSTEM_DATA_RECV]);
	}
}

void viewpanel::showdBFFT(void){
	if(ifShowdB_ == FFT_ORI){
		mFFTShowdBBtn->setStyleSheet("color: black");
		mFFTShowdBBtn->setText("&Show ori");
		ifShowdB_ = FFT_DB;
		power_offset = power_Offset_edit->text().toDouble();
	}else if(ifShowdB_ == FFT_DB){
		mFFTShowdBBtn->setStyleSheet("color: green");
		mFFTShowdBBtn->setText("&Show dB");
		ifShowdB_ = FFT_ORI;
	}
	for(int i = 0 ; i < 2; i++){
		pFFTchart[i]->setShowType(ifShowdB_);
		pFFTchart[i]->setIfScale(true);
	}
}

void viewpanel::configPower(void)
{
	QString str = PowerCombo->currentText();
	if(str.toDouble() < 0.0 || str.toDouble() > 50000.0){
		QMessageBox msgBox;
		msgBox.setText("input power invaild!");
		msgBox.exec();
		return;		
	}
	cmdMsg_.mCommandVal[0] = (uint32_t)(str.toInt());
	if(cmdMsg_.mCommandVal[0] > 2000) cmdMsg_.mCommandVal[0] = 2000;
	cmdMsg_.mHead.usCommand = commandType::POWER_WRITE;
	if(::write(ctrl_sock, &cmdMsg_, sizeof(commandMsg)) < 0){
		QMessageBox msgBox;
		msgBox.setText("config power failed!");
		msgBox.exec();
		return;
	}
	showInfoEditV[1]->setText(str);
	//showInfoEditV[1]->setStyleSheet("QLineEdit{color:rgb(255, 255, 0);}");
}
void viewpanel::configADCDSA(void)
{
	QString str = ADC_DSA_Combo->currentText();
	if(str == "clear")
		cmdMsg_.mCommandVal[0] = 0xFF;
	else{
		if(str.toDouble() < 0.0 || str.toDouble() > 28){
			QMessageBox msgBox;
			msgBox.setText("input ADC-DSA invaild!");
			msgBox.exec();
			return;		
		}
		cmdMsg_.mCommandVal[0] = (uint32_t)(str.toInt());
	}
	cmdMsg_.mHead.usCommand = commandType::ADC_DSA_SET;
	if(::write(ctrl_sock, &cmdMsg_, sizeof(commandMsg)) < 0){
		QMessageBox msgBox;
		msgBox.setText("config ADC-DSA failed!");
		msgBox.exec();
		return;
	}
}

void viewpanel::colorChange(void)
{
	static int i = 0;
	switch (i++ % 4)
	{
	case 0:
		colorCombo->setCurrentIndex(2);
		break;
	case 1:
		colorCombo->setCurrentIndex(0);
		break;
	case 2:
		colorCombo->setCurrentIndex(1);
		break;
	case 3:
		colorCombo->setCurrentIndex(3);
		break;
	default:
		break;
	}
}

void viewpanel::recvMotorInfoloop()
{
	uint8_t motorInfoHead[3]; //motor id + cmd + datalen
	int ret;
	uint16_t 	mHead; //0xaa55
	int dataLen = 0;
	//ret = ::send(motor_ctrl_sock, &dataLen, sizeof(int), 0);
	//printf("send motorMsg , ret is %d!\n", ret);
	ROS_INFO("====enter recvMotorInfoloop ");
	ifConnectedMotorTcp = true;
	while(!terminating){
		if(!ifConnectedMotorTcp) break;
		memset(&mHead, 0, 2);
		memset(motorInfoHead, 0, 3);
		ret = ::recv(motor_ctrl_sock, &mHead, 2, MSG_WAITALL);
		if (ret <= 0){
			printf("read MotorInfo timeout!\n");
			continue;
		} 
		if(mHead != 0xaa55) return;
		ROS_INFO("====recv motor info ");
		ret = ::recv(motor_ctrl_sock, motorInfoHead, 3, MSG_WAITALL);
		if (ret <= 0){
			printf("read MotorInfo timeout!\n");
			continue;
		} 
		dataLen = motorInfoHead[2] + 3; // + count + crc
		motorMaxBuff *ptr_msg = nullptr;
		if(motorMsg_free_buf_queue.get(ptr_msg)) continue;
		ret = ::recv(motor_ctrl_sock, ptr_msg->data + 5, dataLen, MSG_WAITALL);
		if (ret <= 0){
			printf("read MotorInfo timeout!\n");
			motorMsg_free_buf_queue.put(ptr_msg);
			continue;
		} 
		memcpy(ptr_msg->data, &mHead, 2);
		memcpy(ptr_msg->data + 2, motorInfoHead, 3);
		motorMsg_done_buf_queue.put(ptr_msg);
		ROS_INFO("====send motor msg ");
		//parseMotorInfo(recvBuffBody);
		//ROS_INFO("====finish parse motor info ");
	}
}

void viewpanel::recvStateInfoloop()
{
	uint8_t stateInfoHead[9]; //motor id + cmd + datalen
	int ret;
	uint16_t 	mHead; //0xaa55
	int dataLen = 0;
	//ret = ::send(motor_ctrl_sock, &dataLen, sizeof(int), 0);
	//printf("send motorMsg , ret is %d!\n", ret);
	ROS_INFO("====enter recvStateInfoloop ");
	//ifConnected = true;
	while(!terminating){
		if(!ifConnectedStateTcp) break;
		memset(&mHead, 0, 2);
		memset(stateInfoHead, 0, 9);
		ret = ::recv(state_ctrl_sock, &mHead, 2, MSG_WAITALL);
		if (ret <= 0){
			printf("read state mHead timeout!\n");
			if(ret < 0) break;
			continue;
		} 
		if(mHead != 0x55aa) continue;;
		ROS_INFO("====recv state info ");
		ret = ::recv(state_ctrl_sock, stateInfoHead, 9, MSG_WAITALL);
		if (ret <= 0){
			printf("read state stateInfoHead timeout!\n");
			if(ret < 0) break;
			continue;
		} 
		dataLen = stateInfoHead[8] + 1; // + count + crc
		ROS_INFO("====recv state dataLen %d ", dataLen);
		stateMaxBuff *ptr_msg = nullptr;
		if(stateMsg_free_buf_queue.get(ptr_msg)) continue;
		ret = ::recv(state_ctrl_sock, ptr_msg->data + 11, dataLen, MSG_WAITALL);
		if (ret <= 0){
			printf("read state data timeout!\n");
			if(ret < 0) break;
			continue;
		} 
		memcpy(ptr_msg->data, &mHead, 2);
		memcpy(ptr_msg->data + 2, stateInfoHead, 9);
		stateMsg_done_buf_queue.put(ptr_msg);
		ROS_INFO("====send state msg ");
		//parseMotorInfo(recvBuffBody);
	}
	ROS_INFO("====finish recvStateInfoloop ");
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
}
inline double viewpanel::fft2dBm(double x){
	double inputV = x / pow(2, 17);
	double res = 10 * log10(pow(inputV, 2) / 100) - 1.1;
	if (res < -power_min) res = -power_min;
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
	}
	else
		event->ignore();
}

void viewpanel::registerPointcloudRviz()
{
	/* Initialize the main RViz classes */
	manager_ = new rviz::VisualizationManager( render_panel_ );

	//render_panel_->setBackgroundColor( Ogre::ColourValue(238,238,236,0.3));
	render_panel_->setBackgroundColor(Ogre::ColourValue(0,0,0,0.3));

	render_panel_->initialize( manager_->getSceneManager(), manager_ );
	selection_panel_->initialize( manager_ );
	selection_panel_->setFixedSize(300, 600);
	manager_->initialize();
	manager_->startUpdate();
	/* Create a Grid display. */
	grid_ = manager_->createDisplay( "rviz/Grid", "adjustable grid", true );
	ROS_ASSERT( grid_ != NULL );

	//grid_->cc( "Alpha" )->setValue( "0.5" );
	grid_->subProp( "Line Style" )->setValue( "Lines" );
	grid_->subProp( "Plane" )->setValue( "XY" );
	grid_->subProp( "Cell Size" )->setValue(cell_size); 
	grid_->subProp( "Normal Cell Count" )->setValue(cell_3d_size); 
	grid_->subProp( "Plane Cell Count" )->setValue( "1000" );
	double offset = cell_size * 0.5 * cell_3d_size;
	grid_->subProp( "Offset" )->setValue( "0;0;" + QString::number(offset) );
	grid_->subProp( "Reference Frame" )->setValue( "<Fixed Frame>" );

	manager_->getViewManager()->setCurrentViewControllerType("rviz/XYOrbit");
	manager_->getViewManager()->getCurrent()->subProp("Invert Z Axis")->setValue("false");

	/* Create the radar pointcloud fixed frame. */
	manager_->setFixedFrame("image_lidar");

	//Car_ = manager_->createDisplay( "rviz/Marker", "Marker", true );
	//ROS_ASSERT( Car_ != NULL );
	FloatingText_ = manager_->createDisplay( "rviz/Marker", "Marker", true );
	ROS_ASSERT( FloatingText_ != NULL );
	FloatingText_->subProp("Marker Topic")->setValue("/lightning/rviz/floatingText_marker");

	Axes_ = manager_->createDisplay( "rviz/Axes", "Axes", true );
	ROS_ASSERT( Axes_ != NULL );
	Axes_->subProp("Reference Frame")->setValue("odom");
	Axes_->subProp("Length")->setValue("0.2");
	Axes_->subProp("Radius")->setValue("0.03");
	Axes_->subProp("Alpha")->setValue("0.8");

	std::string pointcloud_topic = "/fmcw/rviz/pointcloud";
	ros::NodeHandle fmcw_pcl("~");// = boost::make_shared<ros::NodeHandle>();

	fmcw_pcl_pub = fmcw_pcl.advertise<sensor_msgs::PointCloud2>(pointcloud_topic, 1);
	fmcw_pc_datapath_pub = fmcw_pcl.advertise<std_msgs::String>("/fmcw/pc_raw_data", 10);

	lightning_info_markers = fmcw_pcl.advertise<visualization_msgs::Marker>("/lightning/rviz/floatingText_marker", 1);

	//std::string stationary_pointcloud_topic = "/fmcw/rviz/stationary_pointcloud";
	ROS_INFO("Registering new pointcloud topic: %s",pointcloud_topic.c_str());	
	//ROS_INFO("Registering new pointcloud topic: %s",stationary_pointcloud_topic.c_str());

	pointcloud_fmcw = manager_->createDisplay( "rviz/PointCloud2", "PointCloud2", true );
	ROS_ASSERT(pointcloud_fmcw!=NULL);
	pointcloud_fmcw->subProp("Topic")->setValue( pointcloud_topic.c_str() );
	pointcloud_fmcw->subProp("Style")->setValue("Spheres");
	pointcloud_fmcw->subProp("Size (Pixels)")->setValue("3");
	pointcloud_fmcw->subProp("Size (m)")->setValue(point_size);
#if 1
	pointcloud_fmcw->subProp("Color Transformer")->setValue("RGB8");
	pointcloud_fmcw->subProp("Invert Rainbow")->setValue("false");
	pointcloud_fmcw->subProp("Position Transformer")->setValue("XYZ");
	pointcloud_fmcw->subProp("Use Fixed Frame")->setValue("true");
	pointcloud_fmcw->subProp( "Axis" )->setValue( "Z" );
	prepare_basic_markers();
#endif
}

void viewpanel::prepare_basic_markers( void )
{
	/* Prepare the markers that show the number of detections per frame */
	detections_per_frame_marker.header.frame_id = "image_lidar";
	detections_per_frame_marker.ns = "lidar_detections_per_frame_marker";
	detections_per_frame_marker.type = visualization_msgs::Marker::TEXT_VIEW_FACING;
	detections_per_frame_marker.action = visualization_msgs::Marker::ADD;
	detections_per_frame_marker.pose.position.z = 0;
	detections_per_frame_marker.pose.orientation.x = 0.0;
	detections_per_frame_marker.pose.orientation.y = 0.0;
	detections_per_frame_marker.pose.orientation.z = 0.0;
	detections_per_frame_marker.pose.orientation.w = 1.0;
	detections_per_frame_marker.scale.x = scale_text_size_;
	detections_per_frame_marker.scale.y = scale_text_size_;
	detections_per_frame_marker.scale.z = scale_text_size_;
	detections_per_frame_marker.color.r = 1.0f;
	detections_per_frame_marker.color.g = 1.0f;
	detections_per_frame_marker.color.b = 1.0f;
	detections_per_frame_marker.color.a = 1.0;

	configShowCellScale();
}


void viewpanel::ctrlShowPcOffset(bool show)
{
	pcOffsetDockWidget->setVisible(show);
}
void viewpanel::ctrlShowWindows(bool show)
{
	ctrlDockWidget->setVisible(show);
}

void viewpanel::ctrlPcInfoWindows(bool show)
{
	fullScreenWidget->setVisible(show);
}

void viewpanel::ctrlPcPanel(bool show)
{
	selection_panel_->setVisible(show);
}
void viewpanel::resetViews()
{
	view_vals_t view_vals;
	view_vals.distance = 10;
	view_vals.yaw = 1.56;
	view_vals.pitch = 0.33;
	view_vals.focal_point="0;0;0";
	setView(view_vals);
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
	for(const auto & it : motorItemMap){
		QCheckBox* newBox = new QCheckBox(QString::fromStdString(it.second), this);
		setCheckBoxUnvaild(newBox, false);
		checkShowV.push_back(newBox);
	}
	for(int i = 0; i < checkShowV.size(); i++)
		chartChooseBoxLayout->addWidget(checkShowV[i], i, 0, Qt::AlignLeft);

	chartChooseBoxLayout->addWidget(motorChartResetBtn, 0, 1, Qt::AlignLeft);
	chartChooseBoxLayout->addWidget(motorChartSingleBtn, 1, 1, Qt::AlignLeft);

	chartChooseBox->setLayout(chartChooseBoxLayout);

	pMotorchart = new ChartLighting(this, MOTOR_ORI, checkShowV.size());
	chartMotorBoxLayout->addWidget(pMotorchart->setChart(0, 200, -220, 220), 0 , 0);
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
	

	motorConnectBtnTcp = new QPushButton("&Tcp Connect");
	motorConnectBtnSerial = new QPushButton("&Serial Connect");

	motorSwitchBtn = new QPushButton("&Open");
	motorResetBtn = new QPushButton("&reset");
	motorWorkModeSetBtn = new QPushButton("&Set");
	motorPidSetBtn = new QPushButton("&Write");
	motorShowCycleSetBtn = new QPushButton("&Set");
	motorWorkModeCombo  = new QComboBox(this);
	motorSerialCombo = new QComboBox(this);

	motorPidReadCombo = new QComboBox(this);
	motorPidReadCombo->addItem(QString("current"));
	motorPidReadCombo->addItem(QString("speed"));
	motorPidReadCombo->addItem(QString("position"));
	motorPidReadCombo->addItem(QString("timer"));

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

	motorIDCombo = new QComboBox(this);
	//motorIDCombo->setEditable(true);
	motorIDCombo->setFixedSize(70, 25);
	motorIDCombo->addItem(QString("0"));
	motorIDCombo->addItem(QString("1"));
	motorIDCombo->addItem(QString("2"));
	motorIDCombo->addItem(QString("3"));
	motorIDCombo->setCurrentIndex(1);

	motorConnectPortLine = new QLineEdit(this);
	motorConnectPortLine->setText(motor_port_);
	motorWorkModeAngleSetLine = new QLineEdit(this);
	motorWorkModeSpeedSetLine = new QLineEdit(this);
	motorWorkModeLocSetLine = new QLineEdit(this);

	motorPidMaxLine = new QLineEdit(this);
	motorPidCSetLine = new QLineEdit(this);
	motorPidPSetLine = new QLineEdit(this);
	motorPidISetLine = new QLineEdit(this);
	motorPidDSetLine = new QLineEdit(this);

	motorShowCycleSetLine = new QLineEdit(this);

	QLabel* tcpPortLabel = new QLabel("TCP Port:" );
	QLabel* motor_id = new QLabel("Motor ID:" );

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

	motorControlBoxLayout->addWidget(tcpPortLabel, 0, 0, Qt::AlignRight);
	motorControlBoxLayout->addWidget(motorConnectPortLine, 0, 1, Qt::AlignLeft);
	motorControlBoxLayout->addWidget(serialLabel, 1, 0, Qt::AlignRight);
	motorControlBoxLayout->addWidget(motorSerialCombo, 1, 1, Qt::AlignLeft);
	motorControlBoxLayout->addWidget(motorConnectBtnTcp, 0, 2, Qt::AlignLeft);
	motorControlBoxLayout->addWidget(motorConnectBtnSerial, 1, 2, Qt::AlignLeft);
	motorControlBoxLayout->addWidget(motorSwitchBtn, 1, 3, Qt::AlignLeft);
	motorControlBoxLayout->addWidget(motorResetBtn, 1, 4, Qt::AlignLeft);
	motorControlBoxLayout->addWidget(motor_id, 0, 3, Qt::AlignRight);
	motorControlBoxLayout->addWidget(motorIDCombo, 0, 4, Qt::AlignLeft);


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
	motorControlBoxLayout->addWidget(workModeBox, 0, 5, Qt::AlignLeft);


	QGroupBox* pidBox = new QGroupBox(tr("PID:"));
	QGridLayout* pidBoxLayout = new QGridLayout ;

	QGroupBox* pidReadBox = new QGroupBox(tr("PID:"));
	QGridLayout* pidReadBoxLayout = new QGridLayout ;

	QGroupBox* stateReadBox = new QGroupBox();
	QGridLayout* stateReadBoxLayout = new QGridLayout ;

/* 	pidBoxLayout->addWidget(pidCLabel, 0, 0, Qt::AlignLeft | Qt::AlignTop);
	pidBoxLayout->addWidget(motorPidCycleSetLine, 0, 1, Qt::AlignLeft | Qt::AlignTop);
	pidBoxLayout->addWidget(pidPLabel, 0, 2, Qt::AlignLeft | Qt::AlignTop);
	pidBoxLayout->addWidget(motorPidPSetLine, 0, 3, Qt::AlignLeft | Qt::AlignTop);
	pidBoxLayout->addWidget(pidILabel, 0, 4, Qt::AlignLeft | Qt::AlignTop);
	pidBoxLayout->addWidget(motorPidISetLine, 0, 5, Qt::AlignLeft | Qt::AlignTop);
	pidBoxLayout->addWidget(pidDLabel, 0, 6, Qt::AlignLeft | Qt::AlignTop);
	pidBoxLayout->addWidget(motorPidDSetLine, 0, 7, Qt::AlignLeft | Qt::AlignTop);
	pidBoxLayout->addWidget(motorPidSetBtn, 0, 8, Qt::AlignLeft | Qt::AlignTop); */
	//pidBox->setLayout(pidBoxLayout);
	//motorControlBoxLayout->addWidget(pidBox, 1, 5, Qt::AlignLeft | Qt::AlignTop);

	QGroupBox* showSetBox = new QGroupBox;
	QGridLayout*showSetBoxLayout = new QGridLayout ;
	showSetBoxLayout->addWidget(showCycleLabel, 0, 0, Qt::AlignLeft | Qt::AlignTop);
	showSetBoxLayout->addWidget(motorShowCycleSetLine, 0, 1, Qt::AlignLeft | Qt::AlignTop);
	showSetBoxLayout->addWidget(motorShowCycleSetBtn, 0, 2, Qt::AlignLeft | Qt::AlignTop);
	showSetBox->setLayout(showSetBoxLayout);
	motorControlBoxLayout->addWidget(showSetBox, 1, 5, Qt::AlignLeft | Qt::AlignTop);
	
	motorControlBoxLayout->setColumnStretch(0, 1);
	motorControlBoxLayout->setColumnStretch(1, 1);
	motorControlBoxLayout->setColumnStretch(2, 1);
	motorControlBoxLayout->setColumnStretch(3, 1);
	motorControlBoxLayout->setColumnStretch(4, 1);
	motorControlBoxLayout->setColumnStretch(5, 10);

	motorControlBox->setLayout(motorControlBoxLayout);

	motorCharts->addWidget(motorControlBox, 1, 0);
	motorCharts->setRowStretch(0, 8);
	motorCharts->setRowStretch(1, 2);

	QLabel* workModeReadLabel = new QLabel("Work Mode:" );
	QLabel* displayReadLabel = new QLabel("Valid Items:" );
	QLabel* softReadLabel = new QLabel("SoftVersion:" );
	QLabel* hardReadLabel = new QLabel("HardVersion:" );
	QLabel* devLabel = new QLabel("Device Type:" );

	for(int i = 0; i < 5;i++){
		motorPidReadLine[i] =  new QLineEdit();
		setReadOnlyLineEdit(motorPidReadLine[i]);
		//motorPidReadLine[i]->setFixedSize(140, 25);
	}
	motorPidReadCombo->setFixedSize(100, 25);
	QLabel* pidCLabelR = new QLabel("Cycle:" );
	//pidCLabelR->setFixedSize(70, 25);
	QLabel* pidPLabelR = new QLabel("P:" );
	//pidPLabelR->setFixedSize(40, 25);
	QLabel* pidILabelR = new QLabel("I:" );
	//pidILabelR->setFixedSize(40, 25);
	QLabel* pidDLabelR = new QLabel("D:" );
	//pidDLabelR->setFixedSize(40, 25);

	pidReadBoxLayout->addWidget(new QLabel("mode:" ), 0, 0, Qt::AlignRight );
	pidReadBoxLayout->addWidget(motorPidReadCombo, 0, 1, Qt::AlignLeft );


	pidReadBoxLayout->addWidget(pidPLabelR, 1, 0, Qt::AlignRight );
	pidReadBoxLayout->addWidget(pidILabelR, 2, 0, Qt::AlignRight );
	pidReadBoxLayout->addWidget(pidDLabelR, 3, 0, Qt::AlignRight );
	pidReadBoxLayout->addWidget(pidCLabelR, 4, 0, Qt::AlignRight );
	pidReadBoxLayout->addWidget(new QLabel("max value:" ), 5, 0, Qt::AlignRight );


	for(int i = 0; i < 5;i++){
		pidReadBoxLayout->addWidget(motorPidReadLine[i], i + 1, 1, Qt::AlignLeft | Qt::AlignTop);
	}

	pidReadBoxLayout->addWidget(motorPidPSetLine, 1, 2, Qt::AlignLeft | Qt::AlignTop);
	pidReadBoxLayout->addWidget(motorPidISetLine, 2, 2, Qt::AlignLeft | Qt::AlignTop);
	pidReadBoxLayout->addWidget(motorPidDSetLine, 3, 2, Qt::AlignLeft | Qt::AlignTop);
	pidReadBoxLayout->addWidget(motorPidMaxLine, 5, 2, Qt::AlignLeft | Qt::AlignTop);
	pidReadBoxLayout->addWidget(motorPidCSetLine, 4, 2, Qt::AlignLeft | Qt::AlignTop);

	motorPidReadBtn = new QPushButton("&Read");
	//motorPidReadBtn->setFixedSize(120, 25);
	pidReadBoxLayout->addWidget(motorPidReadBtn, 6, 1, Qt::AlignLeft | Qt::AlignTop);
	pidReadBoxLayout->addWidget(motorPidSetBtn, 6, 2, Qt::AlignLeft | Qt::AlignTop);

	
	pidReadBoxLayout->setColumnStretch(0, 1);
	pidReadBoxLayout->setColumnStretch(1, 4);
	pidReadBoxLayout->setColumnStretch(2, 4);

	pidReadBox->setLayout(pidReadBoxLayout);

	motorWorkModeReadLine =  new QLineEdit(this);
	motorShowItemsLine =  new QLineEdit(this);
	//motorDevReadLine = new QLineEdit(this);
	motorSoftVersionLine = new QLineEdit(this);
	motorHardVersionLine = new QLineEdit(this);

	motorWorkModeReadBtn = new QPushButton("&Read");
	motorSoftVersionReadBtn = new QPushButton("&Read");
	motorHardVersionReadBtn = new QPushButton("&Read");
	motorDevReadBtn = new QPushButton("&Read");
	motorShowItemsReadBtn = new QPushButton("&Read");
	setReadOnlyLineEdit(motorWorkModeReadLine);
	setReadOnlyLineEdit(motorShowItemsLine);
	//setReadOnlyLineEdit(motorDevReadLine);
	setReadOnlyLineEdit(motorSoftVersionLine);
	setReadOnlyLineEdit(motorHardVersionLine);

	stateReadBoxLayout->addWidget(workModeReadLabel, 1, 0, Qt::AlignLeft | Qt::AlignTop);
	stateReadBoxLayout->addWidget(motorWorkModeReadLine, 1, 1);
	stateReadBoxLayout->addWidget(motorWorkModeReadBtn, 1, 2, Qt::AlignLeft | Qt::AlignTop);

	stateReadBoxLayout->addWidget(softReadLabel, 2, 0, Qt::AlignLeft | Qt::AlignTop);
	stateReadBoxLayout->addWidget(motorSoftVersionLine, 2, 1);
	stateReadBoxLayout->addWidget(motorSoftVersionReadBtn, 2, 2, Qt::AlignLeft | Qt::AlignTop);

	stateReadBoxLayout->addWidget(hardReadLabel, 3, 0, Qt::AlignLeft | Qt::AlignTop);
	stateReadBoxLayout->addWidget(motorHardVersionLine, 3, 1);
	stateReadBoxLayout->addWidget(motorHardVersionReadBtn, 3, 2, Qt::AlignLeft | Qt::AlignTop);

	stateReadBoxLayout->addWidget(displayReadLabel, 4, 0, Qt::AlignLeft | Qt::AlignTop);
	stateReadBoxLayout->addWidget(motorShowItemsLine, 4, 1);
	stateReadBoxLayout->addWidget(motorShowItemsReadBtn, 4, 2, Qt::AlignLeft | Qt::AlignTop);
	stateReadBoxLayout->setColumnStretch(0, 1);
	stateReadBoxLayout->setColumnStretch(1, 5);
	stateReadBoxLayout->setColumnStretch(2, 1);


	stateReadBox->setLayout(stateReadBoxLayout);

	motorStateBoxLayout->addWidget(stateReadBox, 0, 0, Qt::AlignTop);

	motorStateBoxLayout->addWidget(pidReadBox, 1, 0, Qt::AlignTop);

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
	this->addTab(multiWidget_new,  "Motor Debug");
}

void viewpanel::CreatStateDetectWindow()
{
	QTabWidget* stateDetectTab = new QTabWidget();

	QWidget* multiWidget0 = new QWidget();
	QGridLayout* showLayout0 = new QGridLayout() ;

	QGroupBox* edfaBox = new QGroupBox(tr("EDFA:"));
	QGridLayout* edfaBoxLayout = new QGridLayout ;

	QGroupBox* basicBox = new QGroupBox(tr("基本信息:"));
	QGridLayout* basicBoxLayout = new QGridLayout ;
	for(int i = 0; i < edfaDevName.size(); i++){
		edfaDeviceInfoLinesV.push_back(new QLineEdit);
		setReadOnlyLineEdit(edfaDeviceInfoLinesV[i]);
		QLabel* temp = new QLabel(edfaDevName[i].c_str());
		basicBoxLayout->addWidget(temp, i, 0, Qt::AlignRight | Qt::AlignTop);
		basicBoxLayout->addWidget(edfaDeviceInfoLinesV[i], i, 1, Qt::AlignLeft | Qt::AlignTop);
	}
	basicBox->setLayout(basicBoxLayout);
	edfaBoxLayout->addWidget(basicBox, 0, 0);

	QGroupBox* stateBox = new QGroupBox(tr("状态信息:"));
	QGridLayout* stateBoxLayout = new QGridLayout ;
	for(int i = 0; i < edfaStateName.size(); i++){
		edfaStateLinesV.push_back(new QLineEdit);
		setReadOnlyLineEdit(edfaStateLinesV[i]);
		QLabel* temp = new QLabel(edfaStateName[i].c_str());
		stateBoxLayout->addWidget(temp, i, 0, Qt::AlignRight | Qt::AlignTop);
		stateBoxLayout->addWidget(edfaStateLinesV[i], i, 1, Qt::AlignLeft | Qt::AlignTop);
	}
	stateBox->setLayout(stateBoxLayout);
	edfaBoxLayout->addWidget(stateBox, 1, 0);

	QGroupBox* warnBox = new QGroupBox(tr("报警信息:"));
	QGridLayout* warnBoxLayout = new QGridLayout ;
	for(int i = 0; i < edfaWarnName.size(); i++){
		edfaWarnLEDV.push_back(new QLabel);
		setLED(edfaWarnLEDV[i], C_GRAY);
		QLabel* temp = new QLabel(edfaWarnName[i].c_str());
		warnBoxLayout->addWidget(temp, i, 0, Qt::AlignRight | Qt::AlignTop);
		warnBoxLayout->addWidget(edfaWarnLEDV[i], i, 1, Qt::AlignLeft | Qt::AlignTop);
	}
	warnBox->setLayout(warnBoxLayout);
	edfaBoxLayout->addWidget(warnBox, 2, 0);
	edfaBoxLayout->setRowStretch(0, 1);
	edfaBoxLayout->setRowStretch(1, 4);
	edfaBoxLayout->setRowStretch(2, 4);
	edfaBox->setLayout(edfaBoxLayout);

	showLayout0->addWidget(edfaBox, 0, 0, Qt::AlignLeft);
	multiWidget0->setLayout(showLayout0);

	stateDetectTab->addTab(multiWidget0,  "光电系统");

	QWidget* multiWidget1 = new QWidget();
	QGridLayout* showLayout1 = new QGridLayout() ;
	multiWidget0->setLayout(showLayout1);
	stateDetectTab->addTab(multiWidget1,  "电机系统");

	QWidget* multiWidget2 = new QWidget();
	QGridLayout* showLayout2 = new QGridLayout() ;
	multiWidget2->setLayout(showLayout2);
	stateDetectTab->addTab(multiWidget2,  "硬件系统");

	QWidget* multiWidget3 = new QWidget();
	QGridLayout* showLayout3 = new QGridLayout() ;
	multiWidget2->setLayout(showLayout3);
	stateDetectTab->addTab(multiWidget3,  "接口外设");

	QWidget* multiWidget4 = new QWidget();
	QGridLayout* showLayout4 = new QGridLayout() ;
	multiWidget2->setLayout(showLayout4);
	stateDetectTab->addTab(multiWidget4,  "软件系统");

	this->addTab(stateDetectTab,  "System Monitor");

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
	pADCchart[0] = new ChartLighting(this, ADC_ORI);
	pADCchart[1] = new ChartLighting(this, ADC_ORI);
	chartADCLayout0->addWidget(pADCchart[0]->setChart(0, FFT_ADC_LENGTH, -32768, 32768), 0 , 0);
	chartADCBox0->setLayout(chartADCLayout0);
#if 0
    OSC_chart *label_OSC_1 = new OSC_chart(this);
    label_OSC_1->set_chart(10,20,this->width() /  2 - 20,this->height() / 2 - 20);
    label_OSC_1->Add_Line_Data(0, 100);
    //label_OSC_1->View_Chart(10000);
#endif
	chartADCLayout1->addWidget(pADCchart[1]->setChart(0, FFT_ADC_LENGTH, -32768, 32768), 0, 0);
	chartADCBox1->setLayout(chartADCLayout1);

	QVBoxLayout* adcCharts= new QVBoxLayout ;
	adcCharts->addWidget(chartADCBox0);
	adcCharts->addWidget(chartADCBox1);
	
	QGridLayout* adcSettingLayout = new QGridLayout ;
	QGroupBox* adcSettingBox = new QGroupBox(tr("ADC control:"));
	QGridLayout* adcSettingBoxLayout = new QGridLayout ;
	singelADCBtn_ = new QPushButton("&Single");
	singelADCBtn_->setFixedSize(100, 25);
	resetADCBtn_ = new QPushButton("&Reset");
	resetADCBtn_->setFixedSize(100, 25);
	
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
	this->addTab(multiWidget_new,  "ADC Debug");
}


void viewpanel::CreatFFTWindow()
{
	QWidget* multiWidget_new = new QWidget();
	QGroupBox *chartADCBox = new QGroupBox(tr("FFT  chart 0:"));
	QGridLayout* chartADCLayout = new QGridLayout ;
	QGroupBox *ChartLightingBox = new QGroupBox(tr("FFT  chart 1:"));
	QGridLayout* ChartLightingLayout = new QGridLayout ;

	std::cout << "this->width() is "  << this->width() << " this->height() is " << this->height() << std::endl;
#if 0
    OSC_chart *label_OSC_0 = new OSC_chart(this);
    label_OSC_0->set_chart(10,20,this->width() /  2 -20,this->height()  / 2-20);
    label_OSC_0->Add_Line_Data(0, 100);
    label_OSC_0->View_Chart(1000);
#endif
	pFFTchart[0] = new ChartLighting(this, FFT_DB);
	pFFTchart[1] = new ChartLighting(this, FFT_DB);
	if(pFFTchart[0]) chartADCLayout->addWidget(pFFTchart[0]->setChart(0, FFT_ADC_LENGTH, -200, 200), 0 , 0);
	chartADCBox->setLayout(chartADCLayout);
#if 0
    OSC_chart *label_OSC_1 = new OSC_chart(this);
    label_OSC_1->set_chart(10,20,this->width() /  2 - 20,this->height() / 2 - 20);
    label_OSC_1->Add_Line_Data(0, 100);
    //label_OSC_1->View_Chart(10000);
#endif
	if(pFFTchart[1]) ChartLightingLayout->addWidget(pFFTchart[1]->setChart( 1 - FFT_ADC_LENGTH, 0, -200, 200), 0, 0);
	ChartLightingBox->setLayout(ChartLightingLayout);

	QGridLayout* main_show= new QGridLayout ;
	main_show->setColumnStretch(0, 9);
	main_show->setColumnStretch(1, 1);

	QVBoxLayout* charts= new QVBoxLayout ;
	charts->addWidget(chartADCBox);
	charts->addWidget(ChartLightingBox);

	QHBoxLayout* configs = new QHBoxLayout ;
	QGroupBox *addrConfigsBox = new QGroupBox(tr("Settings:"));
	//QGroupBox *settingBox = new QGroupBox(tr("Settings:"));
/* 	QGroupBox *settingADCBox = new QGroupBox(tr("FFT control:"));
	QGroupBox *settingFFTBox = new QGroupBox(tr("FFT:")); */

	QGridLayout* addrConfigLayout = new QGridLayout;
	QGridLayout* settingLayout = new QGridLayout;
/* 	QGridLayout* settingADCLayout = new QGridLayout;
	QGridLayout* settingFFTLayout = new QGridLayout; */

	QPushButton * writeAddrbutton = new QPushButton("&Write");
	QPushButton * readAddrbutton = new QPushButton("&Read");
	mFFTShowdBBtn = new QPushButton("&Show Ori");

/* 	QPushButton * settingFFTSavebutton = new QPushButton("&Save");
	QPushButton * settingFFTConfigbutton = new QPushButton("&Config"); */
	singelFFTBtn_ = new QPushButton("&Single");
	singelFFTBtn_->setFixedSize(100, 25);

	resetFFTBtn_ = new QPushButton("&Reset");
	resetFFTBtn_->setFixedSize(100, 25);


	QLabel* power_Offset_label = new QLabel("Power Offset/dB" );
	power_Offset_edit = new QLineEdit();	
	QLabel* power_Min_label = new QLabel("Power Min/dB" );
	power_Min_edit = new QLineEdit();
	power_Offset_edit->setText(power_offset_);
	power_Min_edit->setText(power_min_);

	settingADCSavebutton = new QPushButton("&Start FFT-ADC");
	fftChCombo = new QComboBox();
	for (int i = 1; i < 5; i++){
		fftChCombo->addItem(QString::number(i));
	}
	addrConfigLayout->addWidget(settingADCSavebutton, 0, 0);
	addrConfigLayout->addWidget(fftChCombo, 0, 1);
	addrConfigLayout->addWidget(power_Offset_label, 2, 0);
	addrConfigLayout->addWidget(power_Offset_edit, 2, 1);
	addrConfigLayout->addWidget(power_Min_label, 3, 0);
	addrConfigLayout->addWidget(power_Min_edit, 3, 1);
	addrConfigLayout->addWidget(mFFTShowdBBtn, 4, 0);
	addrConfigLayout->addWidget(resetFFTBtn_, 1, 1);
	addrConfigLayout->addWidget(singelFFTBtn_, 1, 0);

/* 	settingADCBox->setLayout(settingADCLayout);
	settingFFTLayout->addWidget(settingFFTSavebutton, 0, 0);
	settingFFTLayout->addWidget(settingFFTConfigbutton, 0, 1);
	settingFFTBox->setLayout(settingFFTLayout); */
	//settingLayout->addWidget(settingADCBox);
	//settingLayout->addWidget(settingFFTBox);
	//settingBox->setLayout(settingLayout);
	//addrConfigLayout->addWidget(settingBox);
	addrConfigsBox->setLayout(addrConfigLayout);

	configs->addWidget(addrConfigsBox);
	main_show->addLayout(charts, 0, 0 );
	main_show->addLayout(configs, 0, 1);

	multiWidget_new->setLayout(main_show);

	this->addTab(multiWidget_new,  "FFT Debug");

}

void viewpanel::CreatConnect()
{
	connect(lidar_connect_button, SIGNAL(clicked()), this, SLOT( connectControl( void )));
	connect(ctlWriteBtn_[0], SIGNAL(clicked()), this, SLOT( configPower( void )));
	connect(ctlWriteBtn_[1], SIGNAL(clicked()), this, SLOT( configADCDSA( void )));
	connect(ctlWriteBtn_[2], SIGNAL(clicked()), this, SLOT( filterChange( void )));

	connect(saveBtn, SIGNAL(clicked()), this, SLOT( saveDataThead( void )));
	connect(setSaveBtn, SIGNAL(clicked()), this, SLOT( setSaveFolder( void )));
	connect(settingADCSavebutton, SIGNAL(clicked()), this, SLOT( FFT_ADC_UDP_Connect( void )));
	connect( fftChCombo, SIGNAL( currentTextChanged(QString)), this, SLOT( fftChannelChange( void )));
    connect( axes_size_edit, SIGNAL( textChanged(QString)), this, SLOT( configAxesSize( void )));
    connect( cell_size_edit, SIGNAL( textChanged(QString)), this, SLOT( configCellSize( void )));
	connect(selectShowCell, SIGNAL(clicked()), this, SLOT( configShowCellScale( void )));

    connect( cell_3d_size_edit, SIGNAL( textChanged(QString)), this, SLOT( config3dCellSize( void )));
    connect( cell_offset_edit, SIGNAL( textChanged(QString)), this, SLOT( configCellOffset( void )));


    connect( point_size_edit, SIGNAL( textChanged(QString)), this, SLOT( configPointSize( void )));
	connect( colorCombo, SIGNAL( currentTextChanged(QString)), this, SLOT( colorInfoChange( void )));

	QSignalMapper * configMapper;
	QSignalMapper * readMapper;
	configMapper = new QSignalMapper(this);
	readMapper = new QSignalMapper(this);
	for(int i = 0; i < LIGHTNING_REG_NUM; i++) {
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
	connect(pcBWBtn, SIGNAL(clicked()), this, SLOT( ConfigFilterDialog( void )));
	connect(pcRecordBtn, SIGNAL(clicked()), this, SLOT( pcRecord( void )));
	//connect(pcProcBtn, SIGNAL(clicked()), this, SLOT( pcOneFramePure( void )));
	connect(loadAlgBtn, SIGNAL(clicked()), this, SLOT( loadAlgFile( void )));
	connect(loadPCRawBtn, SIGNAL(clicked()), this, SLOT( loadPCFile( void )));
	connect(singelFFTBtn_, SIGNAL(clicked()), this, SLOT( singleFFT( void )));
	connect(resetFFTBtn_, SIGNAL(clicked()), this, SLOT( resetFFT( void )));
	connect(singelADCBtn_, SIGNAL(clicked()), this, SLOT( singleADC( void )));
	connect(resetADCBtn_, SIGNAL(clicked()), this, SLOT( resetADC( void )));
	connect(selectAll, SIGNAL(clicked()), this, SLOT( pcChSelect( void )));

#if 1
	connect(motorConnectBtnTcp, SIGNAL(clicked()), this, SLOT( sendMotorConnectCmdM( void )));
	connect(motorConnectBtnSerial, SIGNAL(clicked()), this, SLOT( sendMotorConnectCmd( void )));
	connect(motorSwitchBtn, SIGNAL(clicked()), this, SLOT( sendMotorOpenCmd( void )));
	connect(motorResetBtn, SIGNAL(clicked()), this, SLOT( sendMotorReset( void )));
	connect(motorPidReadBtn, SIGNAL(clicked()), this, SLOT( readMotorPid( void )));
	connect(motorWorkModeReadBtn, SIGNAL(clicked()), this, SLOT( readMotorWorkMode( void )));
	connect(motorShowItemsReadBtn, SIGNAL(clicked()), this, SLOT( readMotorShowItems( void )));
	connect(motorWorkModeSetBtn, SIGNAL(clicked()), this, SLOT( sendMotorWorkModeCmd( void )));
	connect(motorShowCycleSetBtn, SIGNAL(clicked()), this, SLOT( sendMotorDisplayCycleCmd( void )));
	connect(motorPidSetBtn, SIGNAL(clicked()), this, SLOT( sendMotorPidCmd( void )));
	connect(motorChartSingleBtn, SIGNAL(clicked()), this, SLOT( singleMotor( void )));
	connect(motorChartResetBtn, SIGNAL(clicked()), this, SLOT( resetMotor( void )));
	connect(motorSoftVersionReadBtn, SIGNAL(clicked()), this, SLOT( readSoftVersion( void )));
	connect(motorHardVersionReadBtn, SIGNAL(clicked()), this, SLOT( readHardVersion( void )));
#endif
	QSignalMapper * motorItemsMapper = new QSignalMapper(this);
	for(int i = 0; i < checkShowV.size(); i++) {
		connect(checkShowV[i], SIGNAL(clicked(bool)), motorItemsMapper, SLOT(map()));
		motorItemsMapper->setMapping(checkShowV[i], i);
	}
	connect(motorItemsMapper, SIGNAL(mapped(int)), this, SLOT(motorItemsShow(int)));
	connect(errorLogText,SIGNAL(textChanged()),SLOT(slotTextTcpChanged()));
    timer_  = new QTimer(this);
    connect(timer_, SIGNAL(timeout()), this, SLOT(updateFFTdata(void)));
    timer_->start(100);
    //timer_->setInterval(50);
    //QTimer* timer_adc_test  = new QTimer(this);
    //connect(timer_adc_test, SIGNAL(timeout()), this, SLOT(showADCDataSim(void)));
    //timer_adc_test->start(100);
    timer_adc  = new QTimer(this);
    connect(timer_adc, SIGNAL(timeout()), this, SLOT(updateADCdata(void)));
    timer_adc->start(100);
    QTimer* timer_state  = new QTimer(this);
    connect(timer_state, SIGNAL(timeout()), this, SLOT(updateState(void)));
    //connect(timer_state, SIGNAL(timeout()), this, SLOT(printErrorLog(void)));
    timer_state->start(100);
    //connect(timer_state, SIGNAL(timeout()), this, SLOT(recvSerialInfo(void)));
#if 0	
    QTimer* test_show_item  = new QTimer(this);
    connect(test_show_item, SIGNAL(timeout()), this, SLOT(sendItemsInfoTest(void)));
	test_show_item->start(100);
#endif

    QTimer* tcp_show_item  = new QTimer(this);
    connect(tcp_show_item, SIGNAL(timeout()), this, SLOT(updateMotorChart(void)));
	tcp_show_item->start(100);
}

void viewpanel::CreatPCWindow()
{
	const QSize button_size = QSize(200, 30);
	const QSize view_button_side = QSize(60, 25);
	const QSize slider_size = QSize(200, 20);

	QWidget* multiWidget = new QWidget();
	ctrlDock = new QDockWidget();
	ctrlDockWidget = new QWidget();
	fullScreenWidget = new QWidget();
	fullScreenWidget->setMaximumHeight(60);
	fullScreenWidget->setStyleSheet("background-color: rgb(0, 0, 0);");
	QGroupBox *fsBox = new QGroupBox();
	fsBox->setStyleSheet("QGroupBox{border:none}");
	QGridLayout* fsBoxLayout = new QGridLayout;
	QGridLayout* fsLayout = new QGridLayout;
	QLabel* fullShow0 = new QLabel("染色模式:");
	fullShow0->setStyleSheet("color:yellow;");
	QLabel* fullShow1 = new QLabel("功率/mW:");
	fullShow1->setStyleSheet("color:yellow;");
	QLabel* fullShow2 = new QLabel("单元格长度/m:");
	fullShow2->setStyleSheet("color:yellow;");
	fsBoxLayout->addWidget(fullShow0, 0, 0, Qt::AlignRight);
	fsBoxLayout->addWidget(fullShow1, 0, 2, Qt::AlignRight);
	fsBoxLayout->addWidget(fullShow2, 0, 4, Qt::AlignRight);
	showInfoEditV.push_back(new QLineEdit());
	fsBoxLayout->addWidget(showInfoEditV[0], 0, 1, Qt::AlignLeft);
	setReadOnlyLineEdit(showInfoEditV[0]);
	showInfoEditV.push_back(new QLineEdit());
	fsBoxLayout->addWidget(showInfoEditV[1], 0, 3, Qt::AlignLeft);
	setReadOnlyLineEdit(showInfoEditV[1]);
	showInfoEditV.push_back(new QLineEdit());
	fsBoxLayout->addWidget(showInfoEditV[2], 0, 5, Qt::AlignLeft);
	setReadOnlyLineEdit(showInfoEditV[2]);
	fsBox->setLayout(fsBoxLayout);
	fsLayout->addWidget(fsBox, 0, 0, Qt::AlignLeft | Qt::AlignTop);
	fullScreenWidget->setLayout(fsLayout);
	showInfoEditV[1]->setText("0");
	showInfoEditV[1]->setStyleSheet("QLineEdit{color:rgb(255, 255, 0);}");
	fullScreenWidget->hide();

	ctrlDock->setFeatures(QDockWidget::DockWidgetClosable );
	//dock->setWidget(multiWidget);
	ctrlDock->setMaximumHeight(250);

	//QDockWidget去掉标题栏
	//QWidget * titleBarWidget= ctrlDock->titleBarWidget();
	//QWidget *pEmptyWidget = new QWidget();
	//ctrlDock->setTitleBarWidget(pEmptyWidget);//设置一个空的widget作为标题栏
	//delete titleBarWidget;//删除原标题栏

	QGridLayout* controls = new QGridLayout;
	QGridLayout* mainLayout = new QGridLayout;
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
	saveBtn = new QPushButton("Save", this);
	setButtonStyle(saveBtn);
	saveBtn->setFixedSize(70,25);

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
	setButtonStyle(lidar_connect_button);
	//lidar_disconnect_button = new QPushButton("Disconnect", this);
	setSaveBtn = new QPushButton("&save folder", this);
	setSaveBtn->setFixedSize(70,25);
	setSaveBtn->setFont(QFont("微软雅黑", 10.5));
	setButtonStyle(setSaveBtn);
	pcSwitchBtn = new QPushButton("&Start PC", this);
	setButtonStyle(pcSwitchBtn);
	pcOnceBtn = new QPushButton("&PC Single", this);
	setButtonStyle(pcOnceBtn);
	pcResetBtn = new QPushButton("&PC Contin", this);
	setButtonStyle(pcResetBtn);
	pcBWBtn = new QPushButton("&统计滤波");
	pcBWBtn->setStyleSheet("QPushButton{background-color:rgba(192, 192, 192, 100);}"
	//"QPushButton:hover{background-color:rgba(0, 255, 0, 100);border:2px solid black;border-radius:10px;}"
	"QPushButton:pressed{background-color:rgba(127, 255, 0, 100);}");
	pcRecordBtn = new QPushButton("&Record", this);
	pcRecordBtn->setFixedSize(70,25);
	setButtonStyle(pcRecordBtn);
	pcProcBtn = new QPushButton("&Pure");
	setButtonStyle(pcProcBtn);
	QLabel* lidar_IP_label = new QLabel( "Lidar IP addr" );
	QLabel* lidar_port_label = new QLabel( "Lidar Ctrl Port" );
	QLabel* lidar_udp_port_label = new QLabel( "FFT-ADC Port" );
	QLabel* pcPort_label = new QLabel( "PointCloud port" );

	ip_edit =  new QLineEdit();
	ip_edit->setFixedSize(70,25);
	port_edit =  new QLineEdit();
	port_edit->setFixedSize(70,25);
	udp_pc_port_edit =  new QLineEdit();
	udp_pc_port_edit->setFixedSize(70,25);
	udp_port_edit =  new QLineEdit();
	udp_port_edit->setFixedSize(70,25);

	ip_edit->setPlaceholderText("input ip addr");
	ip_edit->setText(lidar_ip_);
	port_edit->setPlaceholderText("input tcp port");
	port_edit->setText(lidar_ctrl_port_);
	udp_port_edit->setPlaceholderText("input udp port");
	udp_port_edit->setText(lidar_UDP_port_);
	udp_pc_port_edit->setPlaceholderText("input udp pc port");
	udp_pc_port_edit->setText(lidar_UDP_PC_port_);

	controls_layout->addWidget( lidar_IP_label, 0, 0, Qt::AlignRight);
	controls_layout->addWidget( ip_edit, 0, 1, Qt::AlignLeft);
	controls_layout->addWidget( lidar_port_label, 1, 0, Qt::AlignRight);
	controls_layout->addWidget( port_edit, 1, 1, Qt::AlignLeft);

	controls_layout->addWidget( lidar_udp_port_label, 2, 0, Qt::AlignRight);
	controls_layout->addWidget( udp_port_edit, 2, 1, Qt::AlignLeft);
	controls_layout->addWidget( pcPort_label, 3, 0, Qt::AlignRight);	
	controls_layout->addWidget( udp_pc_port_edit, 3, 1, Qt::AlignLeft);	
	controls_layout->addWidget( lidar_connect_button, 4, 0, Qt::AlignHCenter);
	controls_layout->addWidget( setSaveBtn, 4, 2, Qt::AlignHCenter);

	//controls_layout->addWidget( new QLabel( "InvSincFIR" ), 4, 0, Qt::AlignRight);
	InvSincFIR_edit = new QLineEdit;
	InvSincFIR_edit->setFixedSize(80,25);
	setReadOnlyLineEdit(InvSincFIR_edit);
	//controls_layout->addWidget( InvSincFIR_edit, 4, 3, Qt::AlignRight);


	QLabel* CFAR_label = new QLabel( "CFAR" );
	QLabel* m3DFT_label = new QLabel( "3DFT" );
	QLabel* Power_label = new QLabel( "Power/mW" );
	QLabel* ADC_DSA_label = new QLabel( "ADC_DSA/dBm" );

	QLabel* diff_label = new QLabel( "Diff" );
	QLabel* regAddr_label = new QLabel( "Reg Addr" );
	QLabel* regVal_label = new QLabel( "Reg value" );

	QLabel* adc_label0 = new QLabel( "ADC OverVol" );
	QLabel* adc_label1 = new QLabel( "ADC Atten" );

	for (int i = 0; i < LIGHTNING_REG_NUM; i++){
		regAddr_line[i] = new QLineEdit;
		regAddr_line[i]->setText(m_reg_addr_[i]);
		regVal_line[i] = new QLineEdit;
		regVal_line[i]->setText(m_reg_value_[i]);
		regRead_line[i] = new QLineEdit;
		regBtnWrite[i] = new QPushButton("&Write", this);
		setButtonStyle(regBtnWrite[i]);
		regBtnRead[i] = new QPushButton("&Read", this);
		setButtonStyle(regBtnRead[i]);
		setReadOnlyLineEdit(regRead_line[i]);
	}
	loadAlgBtn = new QPushButton("&Load Config", this);
	loadPCRawBtn = new QPushButton("&Load PC", this);

	configInvBtn = new QPushButton("&config");
	readInvBtn = new QPushButton("&read");
	setButtonStyle(configInvBtn);
	setButtonStyle(readInvBtn);
	configInvBtn->setFixedSize(70,25);
	readInvBtn->setFixedSize(70,25);
	//controls_layout->addWidget( configInvBtn, 4, 2, Qt::AlignHCenter);
	//controls_layout->addWidget( readInvBtn, 4, 4, Qt::AlignLeft);

	//loadAlgBtn->setFixedSize(70,25);
	loadAlgBtn->setFont(QFont("微软雅黑", 10));
	loadPCRawBtn->setFont(QFont("微软雅黑", 10));
	setButtonStyle(loadAlgBtn);
	setButtonStyle(loadPCRawBtn);
	adcRead0_line = new QLineEdit;
	adcRead1_line = new QLineEdit;
	setReadOnlyLineEdit(adcRead0_line);
	setReadOnlyLineEdit(adcRead1_line);


	CFARCombo = new QComboBox;
	m3DFTCombo = new QComboBox;
	PowerCombo = new QComboBox;
	ADC_DSA_Combo =  new QComboBox;
	InvSincFIR_Combo =  new QComboBox;
	//controls_layout->addWidget( InvSincFIR_Combo, 4, 1, Qt::AlignLeft);

	filter_Combo =  new QComboBox;
	colorCombo = new QComboBox;
	savePCCombo = new QComboBox;
	savePCCombo->addItem(QString("all"));
	for (int i = 1; i < 5; i++){
		savePCCombo->addItem(QString::number(i));
	}
	ADC_DSA_Combo->addItem(QString::number(0));
	ADC_DSA_Combo->addItem(QString::number(27));
	ADC_DSA_Combo->addItem("clear");
	ADC_DSA_Combo->setCurrentIndex(1);

	InvSincFIR_Combo->addItem("disable");
	InvSincFIR_Combo->addItem("1st Nyquist Zone");
	InvSincFIR_Combo->addItem("2nd Nyquist Zone");
	InvSincFIR_Combo->setCurrentIndex(0);
	InvSincFIR_Combo->setFixedSize(70,25);


	filter_Combo->addItem("all DE");
	filter_Combo->addItem("all En");
	filter_Combo->addItem("Range En");
	filter_Combo->addItem("Speed En");
	filter_Combo->addItem("Intensity En");
	filter_Combo->addItem("Radius En");

	colorCombo->setFixedSize(90,25);
	colorCombo->addItem(tr("range"));
	colorCombo->addItem(tr("intensity"));
	colorCombo->addItem(tr("speed"));
	colorCombo->addItem(tr("channel"));
	colorCombo->addItem(tr("reflectivity"));

	PowerCombo->setEditable(true);
	ADC_DSA_Combo->setEditable(true);

	DiffCombo = new QComboBox;

	for (int i = 0; i < 8; i++){
		CFARCombo->addItem(QString::number(i));
	}

	for (int i = 0; i < power_index.size(); i++){
		PowerCombo->addItem(QString::number(power_index[i]));
	}
	PowerCombo->setCurrentIndex(0);
	m3DFTCombo->addItem(tr("0"));
	m3DFTCombo->addItem(tr("1"));
	DiffCombo->addItem(tr("0"));
	DiffCombo->addItem(tr("1"));
	controls_layout->addWidget( Power_label, 0, 2, Qt::AlignRight);	
	controls_layout->addWidget( ADC_DSA_label, 1, 2, Qt::AlignRight);	
	controls_layout->addWidget( new QLabel("Filter"), 2, 2, Qt::AlignRight);	


	//Power_label->setFixedSize(100,30);
	Power_label->setFont(QFont("微软雅黑", 10.5));
	ADC_DSA_label->setFont(QFont("微软雅黑", 9));

	controls_layout->addWidget( PowerCombo, 0, 3, Qt::AlignLeft);	
	controls_layout->addWidget( ADC_DSA_Combo, 1, 3, Qt::AlignLeft);	
	controls_layout->addWidget( filter_Combo, 2, 3, Qt::AlignLeft);	

	PowerCombo->setFixedSize(70,25);
	filter_Combo->setFixedSize(70,25);
	ADC_DSA_Combo->setFixedSize(70,25);

	for(int i = 0; i < 3; i++){
		ctlWriteBtn_.emplace_back(new QPushButton("&Cfg", this));
		controls_layout->addWidget( ctlWriteBtn_[i], i, 4, Qt::AlignLeft);	
		ctlWriteBtn_[i]->setFixedSize(70,30);
		setButtonStyle(ctlWriteBtn_[i]);
	}
	for(int i = 0; i < 4; i++){
		ctlReadLine_.emplace_back(new QLineEdit);
		setReadOnlyLineEdit(ctlReadLine_[i]); 
		ctlReadLine_[i]->setFixedSize(70,25);
		QString name = QString("EDFA ") + QString(edfaStateName[i + 1].c_str());
		controls_layout->addWidget( new QLabel(name), i + 2, 5, Qt::AlignRight);			
		controls_layout->addWidget( ctlReadLine_[i], i + 2, 6, Qt::AlignLeft);	
	}
	byteSpeedLine = new QLineEdit;
	netStateLED = new QLabel;
	setLED(netStateLED, C_RED);
	setReadOnlyLineEdit(byteSpeedLine); 
	byteSpeedLine->setFixedSize(70,25);
	controls_layout->addWidget( new QLabel("点云传输速度(KB/s)"), 1, 5, Qt::AlignRight);			
	controls_layout->addWidget( byteSpeedLine, 1, 6, Qt::AlignLeft);	
	controls_layout->addWidget( new QLabel("网络连接状态"), 0, 5, Qt::AlignRight);			
	controls_layout->addWidget( netStateLED, 0, 6, Qt::AlignLeft);	

	controls_layout->addWidget( regAddr_label, 0, 7, Qt::AlignRight);
	controls_layout->addWidget( regVal_label, 0, 9, Qt::AlignRight);	

	for(int i = 0; i < LIGHTNING_REG_NUM; i++){
		controls_layout->addWidget( regAddr_line[i], i, 8, Qt::AlignLeft);	
		controls_layout->addWidget( regVal_line[i], i, 10, Qt::AlignLeft);	
		controls_layout->addWidget( regBtnWrite[i], i, 11, Qt::AlignLeft);	
		controls_layout->addWidget( regRead_line[i], i, 12, Qt::AlignLeft);	
		controls_layout->addWidget( regBtnRead[i], i, 13, Qt::AlignLeft);	
	}
	controls_layout->addWidget( loadAlgBtn, 4, 3, Qt::AlignLeft);	
	controls_layout->addWidget( loadPCRawBtn, 4, 4, Qt::AlignLeft);	
	loadAlgBtn->setFixedSize(75,25);
	loadPCRawBtn->setFixedSize(70,25);

	QLabel* rotate_label = new QLabel( "rotate angle" );
	rotation_spin = new QDoubleSpinBox;
	//rotation_spin->setDecimals(3);
	rotation_spin->setMaximum(180);
	rotation_spin->setMinimum(-180);
	rotation_spin->setSingleStep(0.5);
	//rotation_spin->setSuffix("°");
	rotation_spin->setValue(rotation_offset);

	rotate_angle_edit = new QLineEdit;
	rotate_angle_edit->setFixedSize(70,25);
	rotate_angle_edit->setText(QString::number(rotation_offset));
	QLabel* left_label = new QLabel( "left angle" );
	left_angle_edit = new QLineEdit;
	left_angle_edit->setFixedSize(70,25);
	left_angle_edit->setText(QString::number(leftAngle_offset));

	QLabel* speed_critical_label = new QLabel( "critical speed" );
	speed_critical_edit = new QLineEdit;
	speed_critical_edit->setFixedSize(70,25);
	speed_critical_edit->setText(QString::number(speed_critical));

	QLabel* right_label = new QLabel( "right angle" );
	right_angle_edit = new QLineEdit;
	right_angle_edit->setFixedSize(70,25);
	right_angle_edit->setText(QString::number(rightAngle_offset));
	QLabel* color_base_label = new QLabel( "color interval" );
	color_base_edit = new QLineEdit;
	color_base_edit->setFixedSize(70,25);
	color_base_edit->setText(QString::number(color_base));
	QLabel* point_size_label = new QLabel( "point size" );
	point_size_edit = new QLineEdit;
	point_size_edit->setFixedSize(70,25);
	point_size_edit->setText(QString::number(point_size));
	QLabel* cell_size_label = new QLabel( "cell size" );
	QLabel* point_size_interval_label = new QLabel( "point interval" );
	cell_size_edit = new QLineEdit;
	point_size_interval_edit = new QLineEdit;
	cell_size_edit->setFixedSize(70,25);
	cell_3d_size_edit = new QLineEdit;
	cell_3d_size_edit->setText(QString::number(cell_3d_size));
	cell_3d_size_edit->setFixedSize(70,25);

	cell_offset_edit = new QLineEdit;
	cell_offset_edit->setText(QString::number(cell_offset));
	cell_offset_edit->setFixedSize(70,25);

	point_size_interval_edit->setFixedSize(70,25);
	cell_size_edit->setText(QString::number(cell_size));
	point_size_interval_edit->setText(QString::number(0.005));

	QLabel* axes_size_label = new QLabel( "axes size" );
	axes_size_edit = new QLineEdit;
	axes_size_edit->setFixedSize(70,25);
	axes_size_edit->setText(QString::number(axes_size));

	colorSlider = new QSlider( Qt::Horizontal );
	colorSlider->setMinimum( 1 );
	colorSlider->setMaximum( 100 );
	colorSlider->setValue( 10 );
	colorSlider->setTickPosition(QSlider::TicksBelow); 

	QLabel* color_by_label = new QLabel( "color by" );
	//QFrame* hframe = new QFrame(this);
	QFrame* vframe = new QFrame(this);
	//hframe->setFrameShape(QFrame::HLine);      // 设置水平方向
	//hframe->setStyleSheet("QFrame{background:red;min-height:5px}");
	vframe->setFrameShape(QFrame::VLine);      // 设置垂直方向
	vframe->setStyleSheet("QFrame{background:rgb(192,192,192);min-width:10px;border:0.0px solid black}");
	controls_layout->addWidget( vframe, 0, 14, LIGHTNING_REG_NUM, 1);

	controls_layout->addWidget( pcSwitchBtn, 0, 15, Qt::AlignRight);	
	controls_layout->addWidget( pcOnceBtn, 1, 15, Qt::AlignRight);	
	controls_layout->addWidget( pcResetBtn, 2, 15, Qt::AlignRight);	
	controls_layout->addWidget( new QLabel("点云采集"), 3, 2, Qt::AlignRight);
	controls_layout->addWidget( saveBtn, 3, 4, Qt::AlignRight);
	//controls_layout->addWidget( pcBWBtn, 3, 15, Qt::AlignRight);	

	controls_layout->addWidget( point_size_label, 0, 16, Qt::AlignRight);
	controls_layout->addWidget( point_size_edit, 0, 17, Qt::AlignLeft);	
	controls_layout->addWidget( cell_size_label, 1, 16, Qt::AlignRight);
	controls_layout->addWidget( cell_size_edit, 1, 17, Qt::AlignLeft);	

	controls_layout->addWidget( new QLabel("Z count"), 2, 16, Qt::AlignRight);	
	controls_layout->addWidget( cell_3d_size_edit, 2, 17, Qt::AlignLeft);	

	//controls_layout->addWidget( color_base_label, 2, 16, Qt::AlignRight);
	//controls_layout->addWidget( color_base_edit, 2, 17, Qt::AlignLeft);	
	controls_layout->addWidget( axes_size_label, 3, 16, Qt::AlignRight);	
	controls_layout->addWidget( axes_size_edit, 3, 17, Qt::AlignLeft);	
	controls_layout->addWidget( color_by_label, 4, 16, Qt::AlignRight);	
	controls_layout->addWidget( colorCombo, 4, 17, Qt::AlignLeft);
	controls_layout->addWidget( new QLabel("color bar"), 5, 16, Qt::AlignRight);
	controls_layout->addWidget( colorSlider, 5, 17, Qt::AlignLeft);	
	//controls_layout->addWidget( new QLabel("S Filter by"), 5, 16, Qt::AlignRight);	
	filterCombo = new QComboBox;
	filterCombo->setFixedSize(90,25);
	filterCombo->addItem(tr("range"));
	filterCombo->addItem(tr("speed"));
	filterCombo->addItem(tr("intensity"));
	filterCombo->addItem(tr("bypass"));
	filterCombo->setCurrentIndex(3);
	controls_layout->addWidget( savePCCombo, 3, 3, Qt::AlignLeft);	
	savePCCombo->setFixedSize(70, 25);	
	controls_layout->addWidget( pcBWBtn, 3, 15, Qt::AlignLeft);
	controls_layout->addWidget( pcRecordBtn, 4, 15, Qt::AlignLeft);
	controls_layout->addWidget( rotate_label, 1, 18, Qt::AlignRight);
	controls_layout->addWidget( rotation_spin, 1, 19, Qt::AlignLeft);	

	controls_layout->addWidget( point_size_interval_label, 0, 18, Qt::AlignRight);
	controls_layout->addWidget( point_size_interval_edit, 0, 19, Qt::AlignLeft);	
	controls_layout->addWidget( speed_critical_label, 2, 18, Qt::AlignRight);
	controls_layout->addWidget( speed_critical_edit, 2, 19, Qt::AlignLeft);	

/* 	controls_layout->addWidget( new QLabel("3d offset"), 3, 18, Qt::AlignRight);
	controls_layout->addWidget( cell_offset_edit, 3, 19, Qt::AlignLeft); */	

	selectAll = new QCheckBox("&Select Ch All/None");
	selectAll->setChecked(true);
	selectShowCell = new QCheckBox("&点云刻度");
	selectShowCell->setChecked(true);
	controls_layout->addWidget( selectShowCell, 3, 18, Qt::AlignLeft);	

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

	ctrlDockWidget->setLayout(controls);

	//ctrlDock->setWidget(ctrlDockWidget);


	pcOffsetDock = new QDockWidget();
	pcOffsetDockWidget = new QWidget();
	pcOffsetDock->setFeatures(QDockWidget::DockWidgetClosable );
	QGroupBox *pcOffsetBox = new QGroupBox(tr("Ch Offset&&Choose:"));
	QGridLayout* pcOffsetBoxLayout = new QGridLayout;	
	std::vector<QLabel*> distanceOffset_labelV;
	for(int i = 0; i < LIGHTNING_MAX_LINES; i++){
		std::string name = "CH" + std::to_string(i + 1);
		distanceOffset_labelV.emplace_back(new QLabel(name.c_str()));
		distanceOffsetEditV.emplace_back(new QLineEdit());
		distanceOffsetEditV[i]->setFixedSize(70, 25);
		checkPCShowV.push_back(new QCheckBox());
		pcOffsetBoxLayout->addWidget(distanceOffset_labelV[i], i, 0, Qt::AlignRight);
		pcOffsetBoxLayout->addWidget(distanceOffsetEditV[i], i, 1, Qt::AlignLeft);
		pcOffsetBoxLayout->addWidget(checkPCShowV[i], i, 2, Qt::AlignLeft);
		distanceOffsetEditV[i]->setText(distance_offset_[i]);
		checkPCShowV[i]->setChecked(true);
	}
	showInfoEditV[0]->setText(colorCombo->currentText());
	showInfoEditV[0]->setStyleSheet("QLineEdit{color:rgb(255, 255, 0);}");

	showInfoEditV[2]->setText(cell_size_edit->text());
	showInfoEditV[2]->setStyleSheet("QLineEdit{color:rgb(255, 255, 0);}");

	pcOffsetBox->setLayout(pcOffsetBoxLayout);
	QGridLayout* pcOffsetLayout = new QGridLayout;	
	pcOffsetLayout->addWidget(pcOffsetBox, 1, 0, Qt::AlignLeft);
	pcOffsetLayout->addWidget(selectAll, 0, 0, Qt::AlignLeft);
	pcOffsetDockWidget->setLayout(pcOffsetLayout);
	//pcOffsetDock->setWidget(pcOffsetDockWidget);

	QPalette pal(pcOffsetDockWidget->palette());
	pal.setColor(QPalette::Background, Qt::white);
	pcOffsetDockWidget->setAutoFillBackground(true);
	pcOffsetDockWidget->setPalette(pal);

	mainLayout->addWidget ( ctrlDockWidget, 0, 0);
	mainLayout->addWidget ( fullScreenWidget, 0, 0);
	mainLayout->addWidget ( render_panel_, 1, 0);
	mainLayout->addWidget ( selection_panel_, 1, 0, Qt::AlignLeft);
	mainLayout->addWidget ( pcOffsetDockWidget, 1, 0, Qt::AlignRight);

	multiWidget->setLayout(mainLayout);
	this->addTab(multiWidget,  "Lidar Control");
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

void viewpanel::ConfigFilterWork()
{
	rangeSegV.clear();
	maxPcValueSpeedV_.clear();
	maxPcValueIntenV_.clear();

	intervalSpeedV_.clear();
	intervalIntenV_.clear();

	thresholdSpeedV_.clear();
	thresholdRangeV_.clear();
	thresholdIntenV_.clear();

	int flage = 0;
	modeFilter_ = filterMode::BYPASS;
	for(int i = 0; i < rangeSegmentEditV.size(); i++){
		int data = rangeSegmentEditV[i]->text().toInt();
		if(data == 0) continue;
		rangeSegV.push_back(data);
	}

	for(int i = 0; i < sfParaSpeedEditV.size() / 3; i++){
		maxPcValueSpeedV_.push_back(sfParaSpeedEditV[i * 3]->text().toDouble());
		intervalSpeedV_.push_back(sfParaSpeedEditV[i * 3 + 1]->text().toDouble());
		thresholdSpeedV_.push_back(sfParaSpeedEditV[i * 3 + 2]->text().toInt());
	}

	for(int i = 0; i < sfParaIntenEditV.size() / 3; i++){
		maxPcValueIntenV_.push_back(sfParaIntenEditV[i * 3]->text().toDouble());
		intervalIntenV_.push_back(sfParaIntenEditV[i * 3 + 1]->text().toDouble());
		thresholdIntenV_.push_back(sfParaIntenEditV[i * 3 + 2]->text().toInt());
	}

	maxPcValueRange_ = sfParaRangeEditV[0]->text().toDouble();
	intervalRange_ = sfParaRangeEditV[1]->text().toDouble();
	for(int i =0; i < 3; i++){
		thresholdRangeV_.push_back(sfParaRangeEditV[i + 2]->text().toInt());
	}

	width_radius = sfParaRadiusEditV[0]->text().toInt();
	radius_sf = sfParaRadiusEditV[1]->text().toDouble();
	th_radius = sfParaRadiusEditV[2]->text().toInt();

	for(int i = 0; i < checkSfWorkV.size(); i++){
		if(checkSfWorkV[i]->isChecked()){
			flage = 0x1 << (i * 4);
			modeFilter_ += flage;
		}
	}
	save_SF_settings();
}

void viewpanel::ConfigFilterDialog()
{
	QDialog config_dialog(this);
	config_dialog.setWindowModality(Qt::WindowModal);
	config_dialog.setWindowTitle(tr("滤波设置"));
	config_dialog.setWindowFlags(Qt::Dialog | Qt::Desktop | Qt::WindowStaysOnTopHint);
	config_dialog.resize(500, 300);
	QGridLayout *mainLayout = new QGridLayout;

	QGroupBox *sfBox = new QGroupBox(tr("统计滤波:"));
	QGridLayout* sfBoxLayout = new QGridLayout;	
	sfBoxLayout->addWidget(new QLabel("分段距离/m:"), 1, 0, Qt::AlignHCenter| Qt::AlignTop);
	checkSfWorkV.clear();
	rangeSegmentEditV.clear();
	sfParaSpeedEditV.clear();
	sfParaRangeEditV.clear();
	sfParaIntenEditV.clear();
	sfParaRadiusEditV.clear();
	QPushButton *configBtn = new QPushButton("&Config");
	QPushButton *saveBtnsf = new QPushButton("&Save Config");
	setButtonStyle(configBtn);
	setButtonStyle(saveBtnsf);
	for(int i  = 0; i < sfString.size(); i++)
		checkSfWorkV.push_back(new QCheckBox(sfString[i]));
	for(int i = 0; i < 3; i++){
		rangeSegmentEditV.emplace_back(new QLineEdit);
		sfParaRadiusEditV.emplace_back(new QLineEdit);
	}
	for(int i = 0; i < 9; i++){
		sfParaSpeedEditV.emplace_back(new QLineEdit);
		sfParaIntenEditV.emplace_back(new QLineEdit);
	}
	for(int i = 0; i < 5; i++)
		sfParaRangeEditV.emplace_back(new QLineEdit);
	
	load_SF_settings();

	for(int i = 0; i < checkSfWorkV.size(); i++)
		sfBoxLayout->addWidget(checkSfWorkV[i], 0, i, Qt::AlignHCenter| Qt::AlignTop);
	for(int i = 0; i < rangeSegmentEditV.size(); i++)
		sfBoxLayout->addWidget(rangeSegmentEditV[i], 1, 1 + i, Qt::AlignHCenter| Qt::AlignTop);
	for(int i = 0; i < 3; i++){
		for(int j = 0; j < 3; j++){
			sfBoxLayout->addWidget(sfParaSpeedEditV[i * 3 + j], 3 + i, 1 + j, Qt::AlignHCenter| Qt::AlignTop);
			sfBoxLayout->addWidget(sfParaIntenEditV[i * 3 + j], 9 + i, 1 + j, Qt::AlignHCenter| Qt::AlignTop);
		}
	}
	for(int j = 0; j < 3; j++){
		sfBoxLayout->addWidget(sfParaRangeEditV[j], 6, 1 + j, Qt::AlignHCenter| Qt::AlignTop);
		sfBoxLayout->addWidget(sfParaRadiusEditV[j], 13, 1 + j, Qt::AlignHCenter| Qt::AlignTop);
	}
	sfBoxLayout->addWidget(sfParaRangeEditV[3], 7, 3, Qt::AlignHCenter| Qt::AlignTop);
	sfBoxLayout->addWidget(sfParaRangeEditV[4], 8, 3, Qt::AlignHCenter| Qt::AlignTop);

	sfBoxLayout->addWidget(new QLabel("最大值"), 2, 1, Qt::AlignHCenter | Qt::AlignTop);
	sfBoxLayout->addWidget(new QLabel("间隔"), 2, 2, Qt::AlignHCenter | Qt::AlignTop);
	sfBoxLayout->addWidget(new QLabel("阈值"), 2, 3, Qt::AlignHCenter | Qt::AlignTop);
	sfBoxLayout->addWidget(new QLabel("分段0速度"), 3, 0, Qt::AlignHCenter | Qt::AlignTop);
	sfBoxLayout->addWidget(new QLabel("分段1速度"), 4, 0, Qt::AlignHCenter | Qt::AlignTop);
	sfBoxLayout->addWidget(new QLabel("分段2速度"), 5, 0, Qt::AlignHCenter | Qt::AlignTop);
	
	sfBoxLayout->addWidget(new QLabel("分段0距离"), 6, 0, Qt::AlignHCenter | Qt::AlignTop);
	sfBoxLayout->addWidget(new QLabel("分段1距离"), 7, 0, Qt::AlignHCenter | Qt::AlignTop);
	sfBoxLayout->addWidget(new QLabel("分段2距离"), 8, 0, Qt::AlignHCenter | Qt::AlignTop);
	sfBoxLayout->addWidget(new QLabel("分段0强度"), 9, 0, Qt::AlignHCenter | Qt::AlignTop);
	sfBoxLayout->addWidget(new QLabel("分段1强度"), 10, 0, Qt::AlignHCenter | Qt::AlignTop);
	sfBoxLayout->addWidget(new QLabel("分段2强度"), 11, 0, Qt::AlignHCenter | Qt::AlignTop);

	sfBoxLayout->addWidget(new QLabel("搜索宽度"), 12, 1, Qt::AlignHCenter | Qt::AlignTop);
	sfBoxLayout->addWidget(new QLabel("半径/m"), 12, 2, Qt::AlignHCenter | Qt::AlignTop);
	sfBoxLayout->addWidget(new QLabel("阈值"), 12, 3, Qt::AlignHCenter | Qt::AlignTop);
	sfBoxLayout->addWidget(new QLabel("半径滤波参数"), 13, 0, Qt::AlignHCenter | Qt::AlignTop);

	sfBox->setLayout(sfBoxLayout);
	mainLayout->addWidget(sfBox, 0, 0, Qt::AlignLeft);
	mainLayout->addWidget(configBtn, 1, 0, Qt::AlignRight);
	mainLayout->addWidget(saveBtnsf, 1, 0, Qt::AlignLeft);
	config_dialog.setLayout(mainLayout);

	connect(configBtn, SIGNAL(clicked()), this, SLOT( ConfigFilterWork( void )));
	connect(saveBtnsf, SIGNAL(clicked()), this, SLOT( save_SF_settings( void )));

	config_dialog.exec();
}

void viewpanel::simulateADCData()
{
	std::string datPath;
	static int findex = 0;
	datPath = "/home/encheng/data/adc_data_index1.bin";
	uint8_t* ptr = LoadDat(datPath.c_str());
	for(int i = 0; i < file_size_g; i++)
		dataSimulateADC.push_back(ptr[i]);
#if 0
	std::ifstream datfile; 
	datfile.open(datPath, std::ios::in); //| std::ios::binary  && datfile.peek()!= EOF
	if (!datfile.is_open())
	{
		std::cout << "====================读取文件失败" << std::endl;
		return;
	}
	uint8_t c;
	while(!datfile.eof()){
		datfile >> c;
		dataSimulateADC.push_back(c);
	}
	datfile.close();
#endif
	std::cout << "====================读取文件ok, dataSimulateADC size is " <<  dataSimulateADC.size() <<  std::endl;
	//parseADCData(dataSimulateADC);
}
void viewpanel::showADCDataSim()
{
	parseADCData(dataSimulateADC);
	//std::cout << "==showADCDataSim loop" << std::endl;
}

void viewpanel::parseADCData(std::vector<uint8_t> &data)
{
	//std::cout << "!!enter parseADCData! input point num is  "  <<  data.size() / 2 << std::endl;
#if 1
	if(saveadc_){
		std::string datPath;
		static int findex = 0;
		datPath = save_folder_.toStdString() + "/adc_data_index" + std::to_string(findex++) +".bin";
		std::ofstream datfile; 
		datfile.open(datPath, std::ios::out | std::ios::binary); 
		for(int i = 0; i < data.size(); i++) {
			datfile << data[i];
		}
		datfile.close();
		saveadc_ = false;
	}

#endif
	int32_t cur_data  = 0;
	int index = 0;
	adcMsg* padc = NULL;
	if(!adcMsg_free_buf_queue.empty()){
		adcMsg_free_buf_queue.get(padc);
		padc->dataADC0.clear();
		padc->dataADC1.clear();
		for(int i = 0; i < data.size(); i++) {			
			if(i >= data.size() / 2) break;
			int flagNum = i % 2;
			cur_data += data[i] << (8 * (flagNum));
			if(flagNum){
				index += 1;
				if(cur_data > SIGN_LIMIT_NUM){
					cur_data -= SIGN_OFFSET_NUM;
				}
				if(index < 9){
					padc->dataADC0.append(cur_data);
				}else if( index < 17 ){
					padc->dataADC1.append(cur_data);
				}
				cur_data = 0;
				if(index == 16) index = 0;
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
	power_min = power_Min_edit->text().toDouble();
	for(int i = 0; i < data.size(); i++) {
		int flag = index / 4;
		cur_data += data[i] << (8 * (index - (flag * 4)));
		if(index % 4 == 3){
			if(i < data.size() / 2){
				pfft->dataFFT_0.append(cur_data);
				pfft->dataFFTdB_0.append(fft2dBm(cur_data) + power_offset_);
			} else{
				pfft->dataFFT_1.append(cur_data);	
				pfft->dataFFTdB_1.append(fft2dBm(cur_data) + power_offset_);
			}
			cur_data = 0;
		}
		if(index == 31) 
			index = 0;
		else 
			index += 1;
	}
	fftMsg_done_buf_queue.put(pfft);
}

void viewpanel::Save2filecsv(std::vector<uint8_t> &data, bool ifsave)
{
	if(!ifsave) return;
	static long findex = 0;
	int chanID = 0;
	QString str_power = PowerCombo->currentText();

	time_t rawtime;
	struct tm *ptminfo;
	time(&rawtime);
	ptminfo = localtime(&rawtime);
	printf("current: %02d-%02d-%02d %02d:%02d:%02d\n",
	ptminfo->tm_year + 1900, ptminfo->tm_mon + 1, ptminfo->tm_mday,
	ptminfo->tm_hour, ptminfo->tm_min, ptminfo->tm_sec);
	std::string time_str = 	std::to_string(ptminfo->tm_year + 1900) + \
		"-" + std::to_string(ptminfo->tm_mon + 1) + \
		"-" + std::to_string(ptminfo->tm_mday) + \
		"-" + std::to_string(ptminfo->tm_hour) + \
		"-" + std::to_string(ptminfo->tm_min) + \
		"-" + std::to_string(ptminfo->tm_sec) ;

#if 1
	std::string datPath;
	datPath = save_folder_.toStdString() + "/pc_raw_data_" + time_str +".bin";
	ROS_INFO("pc_raw_data path is %s \n", datPath.c_str());
	std::ofstream datfile; 
	datfile.open(datPath, std::ios::out | std::ios::binary); 
	for(int i = 0; i < data.size(); i++) {
		datfile << data[i];
	}
	datfile.close();
#endif

	for(int i = 0; i < LIGHTNING_MAX_LINES; i++){
		distance_offset[i] = distanceOffsetEditV[i]->text().toDouble();
	}
	std::vector<double> distanceOffset_m = {distance_offset[0], distance_offset[4], distance_offset[8], distance_offset[12]};
	if(savePCCombo->currentText() == "all")
		chanID = -1;
	else
		chanID = savePCCombo->currentText().toInt();
	std::string csvPath;
	if(chanID > 0){
		QString strValue = regVal_line[chanID - 1]->text();
		csvPath = save_folder_.toStdString() + "/SavePC_" + str_power.toStdString() + "mW" + \
		"_Ch" + std::to_string(chanID)  + "_" + "CFAR_" + \ 
		cfarAddr[chanID - 1] + "_" +  strValue.toStdString() + "_offset_" + \ 
		std::to_string(distanceOffset_m[chanID - 1]).substr(0, 6) + "_" + time_str + ".csv";

	} else {
		csvPath = save_folder_.toStdString() + "/SavePC_" + str_power.toStdString() + "mW" + \
		"_Ch_all_" + "offset_" + std::to_string(distanceOffset_m[0]).substr(0, 6) + "_" + \ 
		std::to_string(distanceOffset_m[1]).substr(0, 6) + "_" + std::to_string(distanceOffset_m[2]).substr(0, 6) + \ 
		"_"  + std::to_string(distanceOffset_m[3]).substr(0, 6) + "_" + time_str + ".csv";	
	}
	ROS_INFO("csvPath is %s \n", csvPath.c_str());
	std::ofstream csvfile; 
	csvfile.open(csvPath, std::ios::out); 
	int32_t cur_data = 0;
	double distance, speed, vAngle, hAngle;
	csvfile << "intensity" << "," << "distance(m)" << "," << \
				"speed(m/s)" << "," << "Vertical angle(degree)" << "," << "Horizontal angle(degree)"; 
	if(chanID == -1){
		for(int i = 0; i < 3; i++){
			csvfile << "," << "," << "intensity" << "," << "distance(m)" << \ 
					","  << "speed(m/s)" << "," << "Vertical angle(degree)" << "," << "Horizontal angle(degree)"; 
		}
	} 
	csvfile << "\n";
	int dataPointID = 1;
	int flag_nl = 0;	
	int index = 0;
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

		int flag = dataPointID % 4;
		if(index == 4 || index == 8){
			if(index == 4) {
				if(flag == chanID || (flag == (chanID - 4)) || chanID == -1) csvfile << cur_data << ",";	//intensity
			}
			else{
				if(chanID > 0) distance = cur_data * distance_bin - distanceOffset_m[chanID - 1]; //distance
				if(chanID < 0) distance = cur_data * distance_bin - distanceOffset_m[flag_nl]; //distance
				if(flag == chanID || (flag == (chanID - 4)) || chanID == -1) csvfile << distance << ",";	
			}
			cur_data = 0;
		}

		if(index == 10){
			if(cur_data > SIGN_LIMIT_NUM)
				cur_data -= SIGN_OFFSET_NUM;
			speed = cur_data / 128.0;
			if(flag == chanID || (flag == (chanID - 4)) || chanID == -1) csvfile << speed << ",";	 // speed
			cur_data = 0;
		}

		if(index == 12){
			int vindex = cur_data;
			if(vindex < 16 && vindex >= 0)	
				vAngle = fov_vertical[vindex];
			else
				vAngle = 666.666;
			//vAngle = cur_data / 256.0 - 2.5; //vertical
			if(flag == chanID || (flag == (chanID - 4)) || chanID == -1) csvfile << vAngle << ",";	
			cur_data = 0;
		}

		if(index == 14){
			hAngle = cur_data * horizontal_bin; //horizontal
			if(hAngle > 360.0) hAngle -= 360.0;
			if(flag == chanID || (flag == (chanID - 4)) || chanID == -1) {
				csvfile << hAngle;	
				flag_nl++;
				if((chanID == -1)){
					if(flag_nl < 4)
						csvfile << "," << ",";	
					if(flag_nl == 4){
						csvfile << "\n";
						flag_nl = 0;
					}
				}else{
					csvfile << "\n";
				}
			}
			cur_data = 0;
		}

		if(index == 16){
			index = 0;
			dataPointID  += 1;
		}
	}
	csvfile.close();
}


void viewpanel::saveData(){

	int ret = 0;
	bool ifsave = true;
	//while(!terminating && ifSave){
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
}

void viewpanel::TaskFunc(void *arg){
    viewpanel *pSave = (viewpanel *)arg;

    if (pSave && pSave->TaskFunc) {
        pSave->saveData();
    }
}

void viewpanel::TaskFuncUdpFftAdcRecv(void *arg){
    viewpanel *pSave = (viewpanel *)arg;

    if (pSave && pSave->TaskFuncUdpFftAdcRecv) {
        pSave->udpRecvFftAdcLoop();
    }
}

void viewpanel::TaskFuncUdpFftAdcParse(void *arg){
    viewpanel *pSave = (viewpanel *)arg;
    if (pSave && pSave->TaskFuncUdpFftAdcParse) {
        pSave->udpParseFftAdcLoop();
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

void viewpanel::TaskFuncMotorRecv(void *arg){
    viewpanel *pSave = (viewpanel *)arg;
	std::cout << "enter TaskFuncMotorRecv" << std::endl;
    if (pSave && pSave->TaskFuncMotorRecv) {
        pSave->recvMotorInfoloop();
    }
}

void viewpanel::TaskFuncStateRecv(void *arg){
    viewpanel *pSave = (viewpanel *)arg;
	std::cout << "enter TaskFuncStateRecv" << std::endl;
    if (pSave && pSave->TaskFuncStateRecv) {
        pSave->recvStateInfoloop();
    }
}

void viewpanel::saveDataThead()
{
	if(!ifStarted){
		QMessageBox msgBox;
		msgBox.setText("tcp is not connected, save failed!");
		msgBox.exec();
		return;		
	}
	if(!udpFftAdcStop_) udpFftAdcClose();
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

void viewpanel::pcChSelect()
{
	for(int i = 0; i < checkPCShowV.size(); i++){
		checkPCShowV[i]->setChecked(selectAll->isChecked());
	}
}
void viewpanel::pcOneFramePure()
{
	if(oneFramePure){
		pcProcBtn->setStyleSheet("QPushButton{background-color:rgba(192, 192, 192, 100);}");
		pcProcBtn->setText("&Pure");	
		oneFramePure = false;	
	} else {
		pcProcBtn->setStyleSheet("QPushButton{background-color:rgba(127, 255, 0, 100);}");
		pcProcBtn->setText("&360");	
		oneFramePure = true;			
	}

}
void viewpanel::pcRecord(){
	if(udpPCStop_){
		QMessageBox::warning(0, "提示", "pointCloud is stop, please start it",
                          QMessageBox::Ok | QMessageBox::Default, 	0);
		return;		
	}
	static bool recording = false;
	if(!recording){
		std::string pcRecordPath;
		std::string cmd;
		std::string node_name = " __name:=fmcw_record_node &";
		pcRecordPath = save_folder_.toStdString() + "/pointCloud_record" +".bag";
		std::string pointcloud_topic = " /fmcw/rviz/pointcloud";
		ROS_INFO("pcRecordPath is %s", pcRecordPath.c_str());
		cmd = "rosbag record -o " + pcRecordPath + pointcloud_topic + node_name;
		int ret = system(cmd.c_str());
		recording = true;
		//pcRecordBtn->setStyleSheet("color: red");
		pcRecordBtn->setStyleSheet("QPushButton{background-color:rgba(255, 0, 0, 100);}");
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
		//pcRecordBtn->setStyleSheet("color: black");
		pcRecordBtn->setStyleSheet("QPushButton{background-color:rgba(192, 192, 192, 100);}");
		pcRecordBtn->setText("&PointCloud Record");
	}
}


void viewpanel::colorInfoChange()
{
	showInfoEditV[0]->setText(colorCombo->currentText());
	QString mode = colorCombo->currentText();
	if(mode == "range"){
		colorSlider->setMinimum( 1 );
		colorSlider->setMaximum( 100 );
		colorSlider->setValue(10);
		colorSlider->setSingleStep(10);		
	} else if(mode == "intensity"){
		colorSlider->setMinimum( 1 );
		colorSlider->setMaximum( 80000 );
		colorSlider->setValue(25000);	
		colorSlider->setSingleStep(10000);		
	} else if(mode == "reflectivity"){
		colorSlider->setMinimum( 1 );
		colorSlider->setMaximum( 6000000 );
		colorSlider->setValue(700000);	
		colorSlider->setSingleStep(600000);	
	}
}

void viewpanel::colorBarChange(int value)
{

}

void viewpanel::filterChange()
{
	int cmd = filter_Combo->currentIndex();
	ROS_INFO("filter_Combo currentIndexis %d", cmd);
	cmdMsg_.mCommandVal[0] = cmd;
	cmdMsg_.mHead.usCommand = commandType::CtrlCmd_PC_Filter_enable;
	if(::write(ctrl_sock, &cmdMsg_, sizeof(commandMsg)) < 0){
		QMessageBox msgBox;
		msgBox.setText("config filterChange failed!");
		msgBox.exec();
		return;
	}

}
void viewpanel::pcShowBW(){
	if(showBlack){
		render_panel_->setBackgroundColor(Ogre::ColourValue(238,238,236,0.3));
		showBlack = false;
	}else{
		render_panel_->setBackgroundColor(Ogre::ColourValue(0,0,0,0.3));
		showBlack = true;		
	}
}

void viewpanel::udpFftAdcClose(){
	cmdMsg_.mHead.usCommand = commandType::FFT_ADC_READ_STOP;
	if(::write(ctrl_sock, &cmdMsg_, sizeof(commandMsg)) < 0){
		QMessageBox msgBox;
		msgBox.setText("UDP close failed!");
		msgBox.exec();
		return;
	}
	::close(udpFftAdcSocketFd_);
	udpFftAdcStop_ = true;
	vx_task_delete(&bst_task[TASK_FFT_ADC_DATA_RECV]);
	vx_task_delete(&bst_task[TASK_FFT_ADC_DATA_PARSE]);
	settingADCSavebutton->setStyleSheet("color: black");
	settingADCSavebutton->setText("&Start FFT-ADC");
}

void viewpanel::udpPcClose(){
	udpPCStop_ = true;
	byteSpeed_ = 0;
	commandMsg cmdMsg;
	memset(&cmdMsg, 0, sizeof(commandMsg));
	cmdMsg.mHead.usCommand = commandType::POINTCLOUD_UDP_STOP;
	if(::write(ctrl_sock, &cmdMsg, sizeof(commandMsg)) < 0){
		QMessageBox msgBox;
		msgBox.setText("Close PointCloud failed!");
		msgBox.exec();
		return;
	}
	::close(udpRecvPCSocketFd_);
	vx_task_delete(&bst_task[TASK_POINTCLOUD_DATA_RECV]);
	vx_task_delete(&bst_task[TASK_POINTCLOUD_DATA_PARSE]);
	pcSwitchBtn->setStyleSheet("QPushButton{background-color:rgba(192, 192, 192, 100);}");
	pcSwitchBtn->setText("&Start PC");
}


void viewpanel::configAxesSize(){
	axes_size = axes_size_edit->text().toDouble();
	Axes_->subProp("Length")->setValue(axes_size);

}

void viewpanel::configCellOffset(){

	cell_offset = cell_offset_edit->text().toDouble();
	QString offset = "0;0;" + QString::number(cell_offset);
	qDebug() << "====offset is " << offset;
	grid_->subProp( "Offset" )->setValue(offset);

}
void viewpanel::configShowCellScale()
{
	int cell_size_cur = cell_size;
	int text_cell;
	bool showCell = selectShowCell->isChecked();
	for(int i = 0; i < 2; i++){
		for(int j = -11; j < 10; j++){
			detections_per_frame_marker.id = i * 21 + j;
			detections_per_frame_marker.pose.position.x = (i % 2 == 0) ? 0 : (cell_size_cur * (j+1));
			detections_per_frame_marker.pose.position.y = (i % 2 == 0) ? (cell_size_cur * (j+1)) : 0;
			text_cell = cell_size_cur * (j+1);
			if(text_cell < 0) text_cell = -text_cell;
			if(showCell)
				detections_per_frame_marker.text = std::to_string(text_cell).c_str();
			else
				detections_per_frame_marker.text = "";
			lightning_info_markers.publish(detections_per_frame_marker);
		}
	}	
}

void viewpanel::configCellSize(){
	cell_size = cell_size_edit->text().toDouble();
	grid_->subProp( "Cell Size" )->setValue(cell_size); 
	double offset = cell_size * 0.5 * cell_3d_size;
	std::cout << "offset is " << offset << std::endl;
	grid_->subProp( "Offset" )->setValue( "0;0;" + QString::number(offset) );
	showInfoEditV[2]->setText(cell_size_edit->text());
	configShowCellScale();
}

void viewpanel::config3dCellSize(){
	cell_3d_size = cell_3d_size_edit->text().toDouble();
	grid_->subProp( "Normal Cell Count" )->setValue(cell_3d_size); 
	double offset = cell_size * 0.5 * cell_3d_size;
	std::cout << "offset is " << offset << std::endl;
	grid_->subProp( "Offset" )->setValue( "0;0;" + QString::number(offset) );
}

void viewpanel::increasePointSize(){
	point_size += point_size_interval_edit->text().toDouble();
	point_size_edit->setText(QString::number(point_size));
}

void viewpanel::increaseScaleSize()
{
	scale_text_size_ += 0.5;
	detections_per_frame_marker.scale.x = scale_text_size_;
	detections_per_frame_marker.scale.y = scale_text_size_;
	detections_per_frame_marker.scale.z = scale_text_size_;
	configShowCellScale();
}
void viewpanel::decreasePointSize(){
	point_size -= point_size_interval_edit->text().toDouble();;
	point_size_edit->setText(QString::number(point_size));
}

void viewpanel::decreaseScaleSize()
{
	scale_text_size_ -= 0.5;
	if(scale_text_size_ <= 0.0) scale_text_size_ = 0.5;
	detections_per_frame_marker.scale.x = scale_text_size_;
	detections_per_frame_marker.scale.y = scale_text_size_;
	detections_per_frame_marker.scale.z = scale_text_size_;
	configShowCellScale();
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
	saveadc_ = true;
	for(int i = 0 ; i < 2; i++){
		pADCchart[i]->setSingleShow(true);
		pADCchart[i]->setContineFlag(false);
	}
}

void viewpanel::singleMotor() {
	pMotorchart->setSingleShow(true);
	pMotorchart->setContineFlag(false);
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
void viewpanel::resetMotor() {
	pMotorchart->setSingleShow(false);
	pMotorchart->setContineFlag(true);
	pMotorchart->setIfScale(true);
}

void viewpanel::updateFFTdata() {

#if DEBUG_UI	
	y_FFT.clear();
	y_FFT_1.clear();
	QVector<double> y_FFT_dB;
	QVector<double> y_FFT_1_dB;
	y_FFT_dB.clear();
	y_FFT_1_dB.clear();
	power_min = power_Min_edit->text().toDouble();
	for(int i = 0; i< FFT_ADC_LENGTH; i++) 
	{
		double tmp = qrand() % 100000;
		double tmp_log = fft2dBm(tmp);
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
void viewpanel::procEdfaInfo(uint8_t* data, uint8_t cmd_id)
{
	if(cmd_id == sm_mCmd_base){
		flidar_sm_EDFA_base_st baseInfo;
		memcpy(&baseInfo, data, LEN_SM_EDFA_BASE);
		QString vstr = QString("v") + QString::number(baseInfo.hardVer[0]) +  QString(".") +  QString::number(baseInfo.hardVer[1]);
		edfaDeviceInfoLinesV[0]->setText(vstr);
		QString vstr_s = QString("v") + QString::number(baseInfo.softVer[0]) +  QString(".") +  QString::number(baseInfo.softVer[1]) \
		+  QString::number(baseInfo.softVer[2]);
		edfaDeviceInfoLinesV[1]->setText(vstr_s);
		
	} else if(cmd_id == sm_mCmd_stat){
		flidar_sm_EDFA_stat_st stateInfo;
		memcpy(&stateInfo, data, LEN_SM_EDFA_STAT);
		edfaStateLinesV[0]->setText(QString::number(stateInfo.lhtSrc));
		edfaStateLinesV[1]->setText(QString::number(stateInfo.powerIn));
		ctlReadLine_[0]->setText(QString::number(stateInfo.powerIn));
		edfaStateLinesV[2]->setText(QString::number(stateInfo.powerOut));
		ctlReadLine_[1]->setText(QString::number(stateInfo.powerOut));
		edfaStateLinesV[3]->setText(QString::number(stateInfo.mduTemp / 100.0));
		ctlReadLine_[2]->setText(QString::number(stateInfo.mduTemp / 100.0));
		edfaStateLinesV[4]->setText(QString::number(stateInfo.pump1thTemp / 100.0));
		ctlReadLine_[3]->setText(QString::number(stateInfo.pump1thTemp / 100.0));
		edfaStateLinesV[5]->setText(QString::number(stateInfo.pump1thCurr));
		edfaStateLinesV[6]->setText(QString::number(stateInfo.pump2thCurr));

	} else if(cmd_id == sm_mCmd_warn){
		flidar_sm_EDFA_warn_st warnInfo;
		memcpy(&warnInfo, data, LEN_SM_EDFA_WARN);
		uint8_t* ptr_o  = (uint8_t*)&warnInfo;
		for(int i = 0; i < sizeof(flidar_sm_EDFA_warn_st); i++) {
			if(*(ptr_o + i)  > 127){
				setLED(edfaWarnLEDV[i], C_RED);
			} else {
				setLED(edfaWarnLEDV[i], C_GREEN);
			}
		}
	}
}


void viewpanel::procNllInfo(uint8_t* data, uint8_t cmd_id)
{
	if(cmd_id == sm_mCmd_base){
/* 		flidar_sm_EDFA_base_st baseInfo;
		memcpy(&baseInfo, data, LEN_SM_EDFA_BASE);
		QString vstr = QString("v") + QString::number(baseInfo.hardVer[0]) +  QString(".") +  QString::number(baseInfo.hardVer[1]);
		edfaDeviceInfoLinesV[0]->setText(vstr);
		QString vstr_s = QString("v") + QString::number(baseInfo.softVer[0]) +  QString(".") +  QString::number(baseInfo.softVer[1]) \
		+  QString::number(baseInfo.softVer[2]);
		edfaDeviceInfoLinesV[1]->setText(vstr_s); */
	} else if(cmd_id == sm_mCmd_stat){
/* 		flidar_sm_EDFA_stat_st stateInfo;
		memcpy(&stateInfo, data, LEN_SM_EDFA_STAT);
		edfaStateLinesV[0]->setText(QString::number(stateInfo.lhtSrc));
		edfaStateLinesV[1]->setText(QString::number(stateInfo.powerIn));
		edfaStateLinesV[2]->setText(QString::number(stateInfo.powerOut));
		edfaStateLinesV[3]->setText(QString::number(stateInfo.mduTemp / 100.0));
		edfaStateLinesV[4]->setText(QString::number(stateInfo.pump1thTemp / 100.0));
		edfaStateLinesV[5]->setText(QString::number(stateInfo.pump1thCurr));
		edfaStateLinesV[6]->setText(QString::number(stateInfo.pump2thCurr)); */
	} else if(cmd_id == sm_mCmd_warn){
	
	}
}



void viewpanel::updateState()
{
	if(!stateMsg_done_buf_queue.empty()){
		stateMaxBuff* ptr_msg = nullptr;
		stateMsg_done_buf_queue.get(ptr_msg);
		//uint8_t* pdata = ptr_msg->data + 11;
		uint8_t devID = ptr_msg->data[8];
		uint8_t devCmd = ptr_msg->data[9];
		switch (devID)
		{
		case sm_mCode_EDFA:
			/* code */
			procEdfaInfo(ptr_msg->data + 11, devCmd);
			break;
		
		default:
			break;
		}
		stateMsg_free_buf_queue.put(ptr_msg);
	}

	if(tcpSocketCheck(ctrl_sock))
	{
		setLED(netStateLED, C_RED);
		if(ifStarted || ifConnectedStateTcp)
		{
			lidar_connect_button->setStyleSheet("QPushButton{background-color:rgba(192, 192, 192, 100);}");
			lidar_connect_button->setText("&Connect");
			ifStarted = false;
			ifConnectedStateTcp = false;
			::close(ctrl_sock);
		}
	} else {
		setLED(netStateLED, C_GREEN);
	};
	byteSpeedLine->setText(QString::number(byteSpeed_));
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
	if(ptr[0] != 0x55 || ptr[1] != 0xaa){
		qDebug() << "the msg is not motor info!";
		return;
	}
	uint8_t motor_id = ptr[2];
	uint8_t cmd_id = ptr[3];
	int datalen = ptr[4];
/* 	std::cout << " msg datalen is " << datalen << std::endl;
	printf("motor_id is %d, cmd_id is %d\n", motor_id, cmd_id); */

	switch (cmd_id)
	{
	case MOTOR_CONNECT_RET:
		if(ptr[5] == 0xFF){
			if(ifConnectedMotorSerial){
				motorConnectBtnSerial->setStyleSheet("color: green");
				motorConnectBtnSerial->setText("&Disconnect");
			} else {
				motorConnectBtnTcp->setStyleSheet("color: green");
				motorConnectBtnTcp->setText("&Disconnect");
				ifConnectedMotorTcp = true;
			}
		} else {
			QMessageBox msgBox;
			msgBox.setText("MOTOR CONNECT failed!");
			msgBox.exec();			
		}	
		break;
	case MOTOR_DISCONNECT_RET:
		if(ptr[5] == 0xFF){
/* 			motorConnectBtn->setStyleSheet("color: black");
			motorConnectBtn->setText("&Connect");
			ifConnectedMotor = false; */
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
		if(datalen == 20){
			for(int i = 0; i < 5; i++){
				int index = 4 * i + 5;
				motorPidReadLine[i]->setText(QString::number( UnsignedChar4ToFloat(&(ptr[index])) ,'f',3));
			}	
		}else {
			QMessageBox msgBox;
			msgBox.setText("MOTOR MOTOR_PID_READ failed!");
			msgBox.exec();					
		}
		break;

	case MOTOR_SHOW_ITEMS_READ_RET:
		{
			QString showName;
			for(int i = 0; i < datalen; i++){
				showName = showName + QString::number(ptr[5 + i]) +  QString(" ");
			}
			motorShowItemsLine->setText(showName);
		}
		break;
	case MOTOR_SOFT_VERSION_READ_RET:
		{
			QString showName;
			for(int i = 0; i < datalen; i++){
				showName = showName + QString(ptr[5 + i]);
			}
			motorSoftVersionLine->setText(showName);
		}
		break;

	case MOTOR_HARD_VERSION_READ_RET:
		{
			QString showName;
			for(int i = 0; i < datalen; i++){
				showName = showName + QString(ptr[5 + i]);
			}
			motorHardVersionLine->setText(showName);
		}
		break;

	case MOTOR_WORKMODE_SET_RET:
		if(ptr[5] != 0xFF){
			QMessageBox msgBox;
			msgBox.setText("MOTOR MOTOR_WORKMODE_SET failed!");
			msgBox.exec();
		}
		break;
	case MOTOR_SHOW_CYCLE_SET_RET:
		if(ptr[5] != 0xFF){
			QMessageBox msgBox;
			msgBox.setText("MOTOR MOTOR_SHOW_CYCLE_SET failed!");
			msgBox.exec();
		}
		break;
	case MOTOR_PID_SET_RET:
		if(ptr[5] != 0xFF){
			QMessageBox msgBox;
			msgBox.setText("MOTOR MOTOR_PID_SET_RET failed!");
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
		break;
	case MOTOR_ITEMS_INFO:
		motorInfoShow(ptr + 5, datalen);
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
	//qDebug() << "current recvSerialInfo is " << hexData;
	uint8_t* ptr = (uint8_t*)info.data();
	uint8_t* ptr_all;
	if(ptr[0] != 0x55 || ptr[1] != 0xaa){
		if(motorBuffAll.isEmpty()){
			return;
		} else {
			motorBuffAll.append(info);
			ptr_all = (uint8_t*)motorBuffAll.data();
			QByteArray hexData = motorBuffAll.toHex();
			qDebug() << "will process motorBuffAll : " << hexData;
			parseMotorInfo(ptr_all);
			motorBuffAll.clear();
		}		
	} else {
		if(ptr[3] == MOTOR_ITEMS_INFO){
			if(motorBuffAll.isEmpty()){
				motorBuffAll = info;
				return;
			}
			QByteArray hexData = motorBuffAll.toHex();
			qDebug() << "will process motorBuffAll : " << hexData;
			ptr_all = (uint8_t*)motorBuffAll.data();
			parseMotorInfo(ptr_all);
			motorBuffAll = info;
		} else {
			QByteArray hexData = info.toHex();
			qDebug() << "will process info : " << hexData;
			ptr_all = (uint8_t*)info.data();
			parseMotorInfo(ptr_all);			
		}
	}
}


void viewpanel::sendItemsInfoTest()
{
	if(!m_serialPort_test || !m_serialPort_test_open ) return;
	motorItemsShowMsg motorMsgSend;
	memset(&motorMsgSend, 0, sizeof(motorItemsShowMsg));
	motorMsgSend.header.mHead = 0xaa55;
	motorMsgSend.header.cmd = MOTOR_ITEMS_INFO;
	motorMsgSend.header.motor_index = motorIDCombo->currentText().toInt();
	motorMsgSend.header.dataLen = sizeof(ItemData) * ITEMS_NUM;
	for(int i = 0; i < ITEMS_NUM; i++){
		motorMsgSend.data[i].item_id = i;
		motorMsgSend.data[i].data = qrand() % 200;
	}
	int ret = m_serialPort_test->write((const char *)&motorMsgSend,sizeof(motorMsgSend));
	//ROS_INFO("sendItemsInfoTest send bytes are %d", ret);	
	//while ()
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
	//qDebug() << "info recvSerialInfoTest is " << hexData;
	uint8_t* ptr = (uint8_t*)info.data();
	int ret;
	float dataPid[5] = {5.555, 6.666, 7.777, 8.888, 9.999};
	uint8_t cmd_id = ptr[3];

	switch (cmd_id)
	{
	case MOTOR_CONNECT:
		motorMsgSend1_.header.motor_index = 0;
		motorMsgSend1_.header.cmd = motorCmdType::MOTOR_CONNECT_RET;
		motorMsgSend1_.header.dataLen = 0x01;
		motorMsgSend1_.data = 0xff;
		ret = m_serialPort_test->write((const char *)&motorMsgSend1_,sizeof(motorMsgSend1_));
		ROS_INFO("connect motor ok, ret is %d", ret);
		break;
	case MOTOR_DISCONNECT:
		ROS_INFO("disconnect motor ready");
		motorMsgSend1_.header.cmd = motorCmdType::MOTOR_DISCONNECT_RET;
		motorMsgSend1_.header.dataLen = 0x01;
		motorMsgSend1_.data = 0xff;
		ret = m_serialPort_test->write((const char *)&motorMsgSend1_,sizeof(motorMsgSend1_));
		ROS_INFO("disconnect motor ok, ret is %d", ret);
		break;
	case MOTOR_SOFT_VERSION_READ:
		{
			uint8_t data[softVersionName.size() + sizeof(MotorMsgHeader) + sizeof(MotorMsgTailer)];
			MotorMsgHeader	header;
			MotorMsgTailer tailer;
			header.mHead = 0xaa55;
			header.cmd = MOTOR_SOFT_VERSION_READ_RET;
			header.dataLen = softVersionName.size();
			header.motor_index = 0x01;
			memcpy(data, &header, sizeof(header));
			memcpy(data + sizeof(header), softVersionName.c_str(),softVersionName.size());
			memcpy(data + sizeof(header) + softVersionName.size(), &tailer, sizeof(tailer));
			ret = m_serialPort_test->write((const char *)data, softVersionName.size() + sizeof(MotorMsgHeader) + sizeof(MotorMsgTailer));
			//m_serialPort_test->flush();
			//ROS_INFO("disconnect motor ok, ret is %d", ret);
		}
		break;
	case MOTOR_HARD_VERSION_READ:
		{
			uint8_t data[hardVersionName.size() + sizeof(MotorMsgHeader) + sizeof(MotorMsgTailer)];
			MotorMsgHeader	header;
			MotorMsgTailer tailer;
			header.mHead = 0xaa55;
			header.cmd = MOTOR_HARD_VERSION_READ_RET;
			header.dataLen = hardVersionName.size();
			header.motor_index = 0x01;
			memcpy(data, &header, sizeof(header));
			memcpy(data + sizeof(header), hardVersionName.c_str(),hardVersionName.size());
			memcpy(data + sizeof(header) + hardVersionName.size(), &tailer, sizeof(tailer));
			ret = m_serialPort_test->write((const char *)data, hardVersionName.size() + sizeof(MotorMsgHeader) + sizeof(MotorMsgTailer));		
		}
		break;
	case MOTOR_OPEN:
		motorMsgSend1_.header.cmd = motorCmdType::MOTOR_OPEN_RET;
		motorMsgSend1_.header.dataLen = 0x01;
		motorMsgSend1_.data = 0xff;
		ret = m_serialPort_test->write((const char *)&motorMsgSend1_,sizeof(motorMsgSend1_));
		ROS_INFO("switch motor ok, ret is %d", ret);
		break;	
	case MOTOR_WORKMODE_READ:
		motorMsgSend1_.header.cmd = motorCmdType::MOTOR_WORKMODE_READ_RET;
		motorMsgSend1_.header.dataLen = 0x01;
		motorMsgSend1_.data = 0x02;
		ret = m_serialPort_test->write((const char *)&motorMsgSend1_,sizeof(motorMsgSend1_));
		ROS_INFO("MOTOR_WORKMODE_READ ok, ret is %d", ret);
		break;	
	case MOTOR_WORKMODE_SET:
		motorMsgSend1_.header.cmd = motorCmdType::MOTOR_WORKMODE_SET_RET;
		motorMsgSend1_.header.dataLen = 0x01;
		motorMsgSend1_.data = 0xff;
		ret = m_serialPort_test->write((const char *)&motorMsgSend1_,sizeof(motorMsgSend1_));
		ROS_INFO("MOTOR_WORKMODE_SET ok, ret is %d", ret);
		break;	
	case MOTOR_PID_SET:
		for(int i = 0; i < 5; i++){
			int index = 4 * i;
			motorPidReadLine[i]->setText(QString::number( UnsignedChar4ToFloat(&(ptr[6 + index])) ,'f',3));
		}	
		motorMsgSend1_.header.cmd = motorCmdType::MOTOR_PID_SET_RET;
		motorMsgSend1_.header.dataLen = 0x01;
		motorMsgSend1_.data = 0xff;
		ret = m_serialPort_test->write((const char *)&motorMsgSend1_,sizeof(motorMsgSend1_));
		ROS_INFO("MOTOR_PID_SET ok, ret is %d", ret);
		break;	
	case MOTOR_SHOW_CYCLE_SET:
		motorMsgSend1_.header.cmd = motorCmdType::MOTOR_SHOW_CYCLE_SET_RET;
		motorMsgSend1_.header.dataLen = 0x01;
		motorMsgSend1_.data = 0xff;
		ret = m_serialPort_test->write((const char *)&motorMsgSend1_,sizeof(motorMsgSend1_));
		ROS_INFO("MOTOR_SHOW_CYCLE_SET ok, ret is %d", ret);
		break;	
	case MOTOR_SHOW_ITEMS_READ:
		motorMsgShow_.header.cmd = motorCmdType::MOTOR_SHOW_ITEMS_READ_RET;
		motorMsgShow_.header.dataLen = 0x05;
		motorMsgShow_.data[0] = 0x01;
		motorMsgShow_.data[1] = 0x02;
		motorMsgShow_.data[2] = 0x04;
		motorMsgShow_.data[3] = 0x05;
		motorMsgShow_.data[4] = 0x06;
		ret = m_serialPort_test->write((const char *)&motorMsgShow_,sizeof(motorMsgShow_));
		ROS_INFO("MOTOR_SHOW_ITEMS_READ_RET ok, ret is %d", ret);
		break;	
	case MOTOR_PID_READ:
		motorMsgPid_.header.cmd = motorCmdType::MOTOR_PID_READ_RET;
		motorMsgPid_.header.dataLen = 20;
		for(int i = 0; i < 5; i++){
			int index = 4 * ( i );
			FloatToChar(dataPid[i], &(motorMsgPid_.data[index]));
		}
		ret = m_serialPort_test->write((const char *)&motorMsgPid_,sizeof(motorMsgPid_));
		ROS_INFO("MOTOR_PID para send ok, ret is %d", ret);
		break;	
	default:
		break;
	}
}

int viewpanel::checkMotorConnected()
{
	if(!ifConnectedMotorSerial && !ifConnectedMotorTcp){
		QMessageBox msgBox;
		msgBox.setText("please connect to the motor device firstly!");
		msgBox.exec();
		return -1;
	} 
	return 0;
}


void viewpanel::sendMotorPidCmd()
{
	if(checkMotorConnected()) return;
	motorMsgPidSet_.header.cmd = motorCmdType::MOTOR_PID_SET;
	motorMsgPidSet_.header.dataLen = 21;
	motorMsgPidSet_.header.motor_index = motorIDCombo->currentText().toInt();
	motorMsgPidSet_.header.mHead = 0xaa55;
	motorMsgPidSet_.tailer.count = 0x01;
	motorMsgPidSet_.cycle = motorPidCSetLine->text().toDouble();
	motorMsgPidSet_.maxVal = motorPidMaxLine->text().toDouble();
	motorMsgPidSet_.p = motorPidPSetLine->text().toDouble();
	motorMsgPidSet_.i = motorPidISetLine->text().toDouble();
	motorMsgPidSet_.d = motorPidDSetLine->text().toDouble();

	QString modeName = motorPidReadCombo->currentText();
	uint8_t mode = 0;
	if(modeName == "current"){
		mode = 0;
		ROS_INFO(L_GREEN"MODE IS current"NONE_COLOR);		
	} else if(modeName == "speed"){
		mode = 1;
		ROS_INFO(L_GREEN"MODE IS speed"NONE_COLOR);
	} else if(modeName == "position"){
		mode = 2;
		ROS_INFO(L_GREEN"MODE IS position"NONE_COLOR);
	} else if(modeName == "timer"){
		mode = 3;
		ROS_INFO(L_GREEN"MODE IS timer"NONE_COLOR);
	} else {
		ROS_INFO(L_RED"MODE IS invaild"NONE_COLOR);
		return;
	}
	motorMsgPidSet_.mode = mode;
	motorMsgPidSet_.tailer.crc = motorMsgPidSet_.header.cmd + motorMsgPidSet_.header.dataLen + \
	motorMsgPidSet_.header.motor_index + motorMsgPidSet_.tailer.count + \
	FloatSum(motorMsgPidSet_.cycle) + FloatSum(motorMsgPidSet_.p) + \
	FloatSum(motorMsgPidSet_.i) + FloatSum(motorMsgPidSet_.d) + FloatSum(motorMsgPidSet_.maxVal) + motorMsgPidSet_.mode;

	sendMotorCmd((uint8_t *)&motorMsgPidSet_, sizeof(motorMsgPidSet_));
}

void viewpanel::sendMotorWorkModeCmd()
{
	if(checkMotorConnected()) return;
	motorMsgWorkMode_.header.cmd = motorCmdType::MOTOR_WORKMODE_SET;
	motorMsgWorkMode_.header.dataLen = 0x0D;
	motorMsgWorkMode_.tailer.count = 0x01;
	motorMsgWorkMode_.header.motor_index = motorIDCombo->currentText().toInt();
	motorMsgWorkMode_.speed = motorWorkModeSpeedSetLine->text().toDouble();
	motorMsgWorkMode_.angle = motorWorkModeAngleSetLine->text().toDouble();
	motorMsgWorkMode_.location = motorWorkModeLocSetLine->text().toDouble();
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
	motorMsgWorkMode_.tailer.crc = motorMsgWorkMode_.header.cmd + motorMsgWorkMode_.header.dataLen + \
									motorMsgWorkMode_.header.motor_index + FloatSum(motorMsgWorkMode_.angle) + \
									FloatSum(motorMsgWorkMode_.speed) + FloatSum(motorMsgWorkMode_.location) + \
									motorMsgWorkMode_.mode + motorMsgWorkMode_.tailer.count;

	sendMotorCmd((uint8_t *)&motorMsgWorkMode_, sizeof(motorMsgWorkMode_));

}

void viewpanel::sendMotorDisplayCycleCmd()
{
	if(checkMotorConnected()) return;
	motorMsgShowCycle_.header.cmd = motorCmdType::MOTOR_SHOW_CYCLE_SET;
	motorMsgShowCycle_.header.dataLen = 0x02;
	motorMsgShowCycle_.data = motorShowCycleSetLine->text().toInt();
	motorMsgShowCycle_.header.motor_index = motorIDCombo->currentText().toInt();
	motorMsgShowCycle_.tailer.count = 0x01;
	motorMsgShowCycle_.tailer.crc = motorMsgShowCycle_.header.cmd + motorMsgShowCycle_.header.dataLen +\
									motorMsgShowCycle_.header.motor_index + (motorMsgShowCycle_.data & 0xff) + \
									(motorMsgShowCycle_.data >> 8) + motorMsgShowCycle_.tailer.count;
	sendMotorCmd((uint8_t *)&motorMsgShowCycle_, sizeof(motorMsgShowCycle_));
}

void viewpanel::readMotorShowItems()
{
	if(checkMotorConnected()) return;
	motorMsgSend_.header.cmd = motorCmdType::MOTOR_SHOW_ITEMS_READ;
	motorMsgSend_.header.dataLen = 0x00;
	motorMsgSend_.tailer.count = 0x01;
	motorMsgSend_.header.motor_index = motorIDCombo->currentText().toInt();
	motorMsgSend_.tailer.crc = motorMsgSend_.header.cmd + + motorMsgSend_.header.motor_index + \
								motorMsgSend_.header.dataLen + motorMsgSend_.tailer.count;
	sendMotorCmd((uint8_t *)&motorMsgSend_, sizeof(motorMsgSend_));
}
void viewpanel::motorItemsShow(int index)
{
	bool enable_show = checkShowV[index]->isChecked();
	pMotorchart->setGraphShow(index, enable_show);
}

void viewpanel::readMotorWorkMode()
{
	if(checkMotorConnected()) return;
	motorMsgSend_.header.cmd = motorCmdType::MOTOR_WORKMODE_READ;
	motorMsgSend_.header.dataLen = 0x00;
	motorMsgSend_.tailer.count = 0x01;
	motorMsgSend_.header.motor_index = motorIDCombo->currentText().toInt();
	motorMsgSend_.tailer.crc = motorMsgSend_.header.cmd + + motorMsgSend_.header.motor_index + \
								motorMsgSend_.header.dataLen + motorMsgSend_.tailer.count;
	sendMotorCmd((uint8_t *)&motorMsgSend_, sizeof(motorMsgSend_));
}

void viewpanel::readSoftVersion()
{
	if(checkMotorConnected()) return;
	motorMsgSend_.header.cmd = motorCmdType::MOTOR_SOFT_VERSION_READ;
	motorMsgSend_.header.dataLen = 0x00;
	motorMsgSend_.tailer.count = 0x01;
	motorMsgSend_.header.motor_index = motorIDCombo->currentText().toInt();
	motorMsgSend_.tailer.crc = motorMsgSend_.header.cmd + + motorMsgSend_.header.motor_index + \
								motorMsgSend_.header.dataLen + motorMsgSend_.tailer.count;
	sendMotorCmd((uint8_t *)&motorMsgSend_, sizeof(motorMsgSend_));
}
void viewpanel::readHardVersion()
{
	if(checkMotorConnected()) return;
	motorMsgSend_.header.cmd = motorCmdType::MOTOR_HARD_VERSION_READ;
	motorMsgSend_.header.dataLen = 0x00;
	motorMsgSend_.tailer.count = 0x01;
	motorMsgSend_.header.motor_index = motorIDCombo->currentText().toInt();
	motorMsgSend_.tailer.crc = motorMsgSend_.header.cmd + + motorMsgSend_.header.motor_index + \
								motorMsgSend_.header.dataLen + motorMsgSend_.tailer.count;
	sendMotorCmd((uint8_t *)&motorMsgSend_, sizeof(motorMsgSend_));
}

void viewpanel::readMotorPid()
{
	if(checkMotorConnected()) return;
	motorMsgSend1_.header.cmd = motorCmdType::MOTOR_PID_READ;
	//motorMsgSend1_.data = motorPidReadCombo;
	QString modeName = motorPidReadCombo->currentText();
	uint8_t mode = 0;
	if(modeName == "current"){
		mode = 0;
		ROS_INFO(L_GREEN"MODE IS current"NONE_COLOR);		
	} else if(modeName == "speed"){
		mode = 1;
		ROS_INFO(L_GREEN"MODE IS speed"NONE_COLOR);
	} else if(modeName == "position"){
		mode = 2;
		ROS_INFO(L_GREEN"MODE IS position"NONE_COLOR);
	} else if(modeName == "timer"){
		mode = 3;
		ROS_INFO(L_GREEN"MODE IS timer"NONE_COLOR);
	} else {
		ROS_INFO(L_RED"MODE IS invaild"NONE_COLOR);
		return;
	}
	motorMsgSend1_.data = mode;
	motorMsgSend1_.header.dataLen = 0x01;
	motorMsgSend1_.tailer.count = 0x01;
	motorMsgSend1_.header.motor_index = motorIDCombo->currentText().toInt();
	motorMsgSend1_.tailer.crc = motorMsgSend1_.header.cmd + motorMsgSend1_.header.dataLen + motorMsgSend1_.data + \
								motorMsgSend1_.tailer.count + motorMsgSend1_.header.motor_index;
							
	sendMotorCmd((uint8_t *)&motorMsgSend1_, sizeof(motorMsgSend1_));
}

void viewpanel::sendSerialBytes(const uint8_t *begin, int size)
{
	int count = 0;
	for(int i = 0; i < size; i++){
		//uint8_t *begin = (uint8_t *)&motorMsgSend1_;
		int ret = m_serialPort->write((const char *)begin + i,1);
		m_serialPort->flush();
		count += ret;
		usleep(20 * 1000);		
		qDebug("%x",begin[i]);
	}
	ROS_INFO("m_serialPort write bytes are %d, required bytes are %d", count, size);		
}

void viewpanel::sendMotorCmd(const uint8_t *begin, int size)
{
	if(ifConnectedMotorSerial){
		sendSerialBytes(begin, size);
	}
	int ret = 0;
	if(ifConnectedMotorTcp){
		ret = ::write(motor_ctrl_sock, begin, size);
		if(ret < 0){
			QMessageBox msgBox;
			msgBox.setText("sendMotorCmd failed!");
			msgBox.exec();
			return;
		}	
		ROS_INFO(L_GREEN"sendMotorCmd ok, ret is %d, size is %d"NONE_COLOR, ret, size);	
	}
}

void viewpanel::sendMotorOpenCmd()
{
	if(checkMotorConnected()) return;
	motorMsgSend1_.header.cmd = motorCmdType::MOTOR_OPEN;
	motorMsgSend1_.header.dataLen = 0x01;
	motorMsgSend1_.header.motor_index = motorIDCombo->currentText().toInt();
	motorMsgSend1_.header.mHead = 0xaa55;
	motorMsgSend1_.tailer.count = 0x01;
	if(!ifOpenMotor){
		motorMsgSend1_.data = 0x01;
	}else{
		motorMsgSend1_.data = 0x00;
	}
	motorMsgSend1_.tailer.crc = motorMsgSend1_.header.cmd + motorMsgSend1_.header.dataLen +  motorMsgSend1_.data + \
							motorMsgSend1_.header.motor_index + motorMsgSend1_.tailer.count;
	sendMotorCmd((uint8_t *)&motorMsgSend1_, sizeof(motorMsgSend1_));
}
void viewpanel::sendMotorReset()
{
	if(checkMotorConnected()) return;
	motorMsgSend_.header.cmd = motorCmdType::MOTOR_SYSTEM_RESET;
	motorMsgSend_.header.dataLen = 0x00;
	motorMsgSend_.header.motor_index = motorIDCombo->currentText().toInt();
	motorMsgSend_.header.mHead = 0xaa55;
	motorMsgSend_.tailer.count = 0x01;
	motorMsgSend_.tailer.crc = motorMsgSend_.header.cmd + motorMsgSend_.header.dataLen  + \
							motorMsgSend_.header.motor_index + motorMsgSend_.tailer.count;
	sendMotorCmd((uint8_t *)&motorMsgSend_, sizeof(motorMsgSend_));
}

void viewpanel::sendMotorConnectCmdM()
{
	if(!ifConnectedMotorTcp){
		if(motorConnect()){
			QMessageBox msgBox;
			msgBox.setText("connect to Motor failed!");
			msgBox.exec();
			return;
		} 
		startMotorTask(); 		
#if 1
		motorMsgSend_.header.cmd = motorCmdType::MOTOR_CONNECT;
		motorMsgSend_.header.motor_index = motorIDCombo->currentText().toInt();
		motorMsgSend_.header.dataLen = 0x00;
		motorMsgSend_.tailer.count = 0x01;	
		motorMsgSend_.tailer.crc = motorMsgSend_.header.cmd + motorMsgSend_.header.motor_index + \
									motorMsgSend_.header.dataLen + motorMsgSend_.tailer.count;
		if(::write(motor_ctrl_sock, &motorMsgSend_, sizeof(motorMsgSend_)) < 0){
			QMessageBox msgBox;
			msgBox.setText("send Motor ConnectCmdMsg failed!");
			msgBox.exec();
			return;
		}	
		ROS_INFO("send Motor ConnectCmdMsg ok");	
#endif
		motorConnectBtnTcp->setStyleSheet("color: green");
		motorConnectBtnTcp->setText("&Disconnect");
		motorConnectBtnSerial->setEnabled(false);
	}else{
		motorMsgSend_.header.cmd = motorCmdType::MOTOR_DISCONNECT;
		motorMsgSend_.header.dataLen = 0x00;
		motorMsgSend_.header.motor_index = motorIDCombo->currentText().toInt();
		motorMsgSend_.tailer.count = 0x01;
		motorMsgSend_.tailer.crc = motorMsgSend_.header.cmd + motorMsgSend_.header.motor_index + \
									motorMsgSend_.header.dataLen + motorMsgSend_.tailer.count;
		if(::write(motor_ctrl_sock, &motorMsgSend_, sizeof(motorMsgSend_)) < 0){
			QMessageBox msgBox;
			msgBox.setText("send Motor ConnectCmdMsg failed!");
			msgBox.exec();
			return;
		}	
		ifConnectedMotorTcp = false;
		::close(motor_ctrl_sock);
		motorConnectBtnTcp->setStyleSheet("color: black");
		motorConnectBtnTcp->setText("&Tcp Connect");
		motorConnectBtnSerial->setEnabled(true);
	}
}

void viewpanel::sendMotorConnectCmd()
{
	if(!ifConnectedMotorSerial){
		if(motorSerialConnect()){
			QMessageBox msgBox;
			msgBox.setText("connect to Motor failed!");
			msgBox.exec();
			return;
		} 

		motorMsgSend_.header.cmd = motorCmdType::MOTOR_CONNECT;
		motorMsgSend_.header.motor_index = motorIDCombo->currentText().toInt();
		motorMsgSend_.header.dataLen = 0x00;
		motorMsgSend_.tailer.count = 0x01;
		motorMsgSend_.tailer.crc = motorMsgSend_.header.cmd + motorMsgSend_.header.motor_index + \
									motorMsgSend_.header.dataLen + motorMsgSend_.tailer.count;
		motorMsgSend_.tailer.crc = 0x03;
		sendSerialBytes((uint8_t *)&motorMsgSend_, sizeof(motorMsgSend_));
		ifConnectedMotorSerial = true;
		motorConnectBtnSerial->setStyleSheet("color: green");
		motorConnectBtnSerial->setText("&Disconnect");
		motorConnectBtnTcp->setEnabled(false);
	}else{
		motorMsgSend_.header.cmd = motorCmdType::MOTOR_DISCONNECT;
		motorMsgSend_.header.motor_index = motorIDCombo->currentText().toInt();
		motorMsgSend_.header.dataLen = 0x00;
		motorMsgSend_.tailer.count = 0x01;
		motorMsgSend_.tailer.crc = motorMsgSend_.header.cmd + motorMsgSend_.header.motor_index + \
									motorMsgSend_.header.dataLen + motorMsgSend_.tailer.count;
		sendSerialBytes((uint8_t *)&motorMsgSend_, sizeof(motorMsgSend_));
		serialClose(m_serialPort);
		ifConnectedMotorSerial = false;
		motorConnectBtnSerial->setStyleSheet("color: black");
		motorConnectBtnSerial->setText("&Serial Connect");
		motorConnectBtnTcp->setEnabled(true);
	}
}

void viewpanel::updateMotorChart() {

	if(!motorMsg_done_buf_queue.empty()){
		motorMaxBuff* pMotor = NULL;
		motorMsg_done_buf_queue.get(pMotor);
		parseMotorInfo(pMotor->data);
		motorMsg_free_buf_queue.put(pMotor);
		ROS_INFO("motor chart update");  
	}
}


void viewpanel::updateADCdata() {

	static long long frame_index = 0;
	x_adc0.clear();
	x_adc1.clear();
	for(int i = 0; i< FFT_ADC_LENGTH; i++) {
		x_adc0.append(i + FFT_ADC_LENGTH * frame_index);
	}
	x_adc1 = x_adc0;

#if 0	
	y_adc0.clear();
	y_adc1.clear();
	for(int i = 0; i< FFT_ADC_LENGTH; i++) {
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
	//QVariant focal_point = {"0;0;0"};
	//manager_->getViewManager()->getCurrent()->subProp("Focal Point")->setValue("0;0;0");
}

void viewpanel::udpPcConnect() {

	if(!udpFftAdcStop_) udpFftAdcClose();
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
		pcSwitchBtn->setStyleSheet("QPushButton{background-color:rgba(127, 255, 0, 100);}");
		pcSwitchBtn->setText("&Close PC");
	}else{
		udpPcClose();
	}
}
void viewpanel::fftChannelChange() 
{
	if(!udpPCStop_) udpPcClose();
	if(!udpFftAdcStop_) udpFftAdcClose();
	usleep(100 * 1000);
	commandMsg cmdMsg;
	memset(&cmdMsg, 0, sizeof(commandMsg));
	cmdMsg.mHead.usCommand = commandType::FFT_ADC_READ_SETCH;
	cmdMsg.mCommandVal[0] = fftChCombo->currentText().toInt() - 1;
	if(::write(ctrl_sock, &cmdMsg, sizeof(commandMsg)) < 0){
		QMessageBox msgBox;
		msgBox.setText("fftChannelChange failed!");
		msgBox.exec();
		return;
	}	
	usleep(100 * 1000);
	FFT_ADC_UDP_Connect();
}

void viewpanel::FFT_ADC_UDP_Connect() {
	if(!udpPCStop_) udpPcClose();
	if(udpFftAdcStop_){
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
		bst_params.task_main = TaskFuncUdpFftAdcRecv;
		vx_task_create(&bst_task[TASK_FFT_ADC_DATA_RECV], &bst_params);  

		vx_task_set_default_create_params(&bst_params);
		bst_params.app_var = this;
		bst_params.task_mode = 0;
		bst_params.task_main = TaskFuncUdpFftAdcParse;
		vx_task_create(&bst_task[TASK_FFT_ADC_DATA_PARSE], &bst_params);  
		settingADCSavebutton->setStyleSheet("color: green");
		settingADCSavebutton->setText("&Stop FFT-ADC");
	}else{
		udpFftAdcClose();
		//QMessageBox msgBox;
		//msgBox.setText("UDP Connection is already working, please stop it first !");
		//msgBox.exec();		
	}
}

void viewpanel::startPcTask() 
{
	vx_task_set_default_create_params(&bst_params);
	bst_params.app_var = this;
	bst_params.task_mode = 0;
	bst_params.task_main = TaskFuncPCRecv;
	vx_task_create(&bst_task[TASK_POINTCLOUD_DATA_RECV], &bst_params);  

	vx_task_set_default_create_params(&bst_params);
	bst_params.app_var = this;
	bst_params.task_mode = 0;
	bst_params.task_main = TaskFuncPCParse;
	vx_task_create(&bst_task[TASK_POINTCLOUD_DATA_PARSE], &bst_params); 
}

void viewpanel::startMotorTask() 
{
	vx_task_set_default_create_params(&bst_params);
	bst_params.app_var = this;
	bst_params.task_mode = 0;
	bst_params.task_main = TaskFuncMotorRecv;
	vx_task_create(&bst_task[TASK_MOTOR_DATA_RECV], &bst_params);  
}

void viewpanel::startStateDectTask() 
{
	vx_task_set_default_create_params(&bst_params);
	bst_params.app_var = this;
	bst_params.task_mode = 0;
	bst_params.task_main = TaskFuncStateRecv;
	vx_task_create(&bst_task[TASK_SYSTEM_DATA_RECV], &bst_params);  
}


void viewpanel::radiusFilterProc()
{
	std::cout << "======RADIUS_F: oneFrame360.pcDataOneFrame size() is : " <<  oneFrame360.pcDataOneFrame.size() << std::endl;	
	int width = width_radius;
	double r_radius = radius_sf;
	int row = LIGHTNING_MAX_LINES; //max lines
	int col = oneFrame360.pcDataOneFrame.size() / LIGHTNING_MAX_LINES;
	int first_x, first_y, last_x, last_y;
	for(int i = 0; i < col; i++){
		for(int j = 0; j < row; j ++){
			if(oneFrame360.vaildV[i * row + j] == false) continue;
			int lineIndex =  oneFrame360.pcDataOneFrame[i * row + j].pcmVerticalIndex;
			double distance_m =  oneFrame360.pcDataOneFrame[i * row + j].pcmDistance * distance_bin - distance_offset[lineIndex];
			first_x = i - width;
			if(first_x < 0) first_x = 0;
			first_y = j - width;
			if(first_y < 0) first_y = 0;
			last_x = i + width + 1;
			if(last_x > col) last_x = col;
			last_y = j + width + 1;
			if(last_y > row) last_y = row;
			/* Traverse the selected rectangle */
			for(int x =  first_x; x < last_x; x++){
				for(int y = first_y; y < last_y; y++){
					int index =  oneFrame360.pcDataOneFrame[x * row + y].pcmVerticalIndex;
					double distance_c =  oneFrame360.pcDataOneFrame[x * row + y].pcmDistance * distance_bin - distance_offset[index];
					double distance_d  = distance_c - distance_m;	
					if(distance_d <= r_radius && distance_d >= -r_radius)
							oneFrame360.around_count[i * row + j]++;					
				}
			}
		}
	}
}
void viewpanel::pcDataFilterPreProc(udpPcMsgOneFrame* pmsg, int fMode)
{
	if(!pmsg || fMode == BYPASS) return;
	distance_min, distance_max, indensity_min, indensity_max, speed_min, speed_max   = 0.0;
	int pcFrameSize = pmsg->pcDataOneFrame.size();
	shSpeedVV.clear();
	shRangeV.clear();
	shIntenVV.clear();
	pcRadius2DVV.clear();
	intenNumMap.clear();
	reflectNumMap.clear();
	std::vector<int> hSpeedSize;
	std::vector<int> hIntenSize;
	std::vector<pc_radius_meta> pc_all_line;
	pc_all_line.clear();
	pc_all_line.resize(LIGHTNING_MAX_LINES);
	int hRangeSize = 0;
	minPcValueSpeedV_.clear();
	minPcValueRange_ = 0.0;
	bool start_copy = false;
	if(maxPcValueSpeedV_.empty() || intervalSpeedV_.empty() || thresholdSpeedV_.empty() || thresholdRangeV_.empty()) return;
	if(maxPcValueIntenV_.empty() || intervalIntenV_.empty() || thresholdIntenV_.empty()) return;

	for(int i = 0; i < maxPcValueSpeedV_.size(); i++){
		if(intervalSpeedV_[i] == 0.0) intervalSpeedV_[i] = 0.1;
		hSpeedSize.push_back((int)(maxPcValueSpeedV_[i] / intervalSpeedV_[i]) * 2);
		minPcValueSpeedV_.push_back((-maxPcValueSpeedV_[i]));
	}
	for(auto &it : hSpeedSize)
		shSpeedVV.emplace_back(it, 0);

	for(int i = 0; i < maxPcValueIntenV_.size(); i++){
		if(intervalIntenV_[i] < 10.0) intervalIntenV_[i] = 10.0;
		hIntenSize.push_back((int)(maxPcValueIntenV_[i] / intervalIntenV_[i]));
	}
	for(auto &it : hIntenSize)
		shIntenVV.emplace_back(it, 0);

	if(intervalRange_ == 0.0) intervalRange_ = 0.1;
	hRangeSize = maxPcValueRange_ / intervalRange_;
	shRangeV.resize(hRangeSize, 0);
	QString strColor = colorCombo->currentText();
	std::cout << "fMode: " << fMode << std::endl;

	if(fMode & filterMode::RADIUS_F){
		oneFrame360.pcDataOneFrame.clear();
		oneFrame360.vaildV.clear();
		oneFrame360.around_count.clear();
	}

	for(int j = 0; j < pcFrameSize; j++){
		for(int index = 0; index < UDP_PC_SIZE_SINGLE_V01; index++){
			double horizontal_m = pmsg->pcDataOneFrame[j].UDP_PC_payload[index].pcmHorizontal * horizontal_bin;
			if(horizontal_m > 360.0) horizontal_m -= 360.0;
			int lineIndex = pmsg->pcDataOneFrame[j].UDP_PC_payload[index].pcmVerticalIndex;
			double distance_m = pmsg->pcDataOneFrame[j].UDP_PC_payload[index].pcmDistance * distance_bin - distance_offset[lineIndex];
			if(fMode & filterMode::RADIUS_F){
				bool valid  = true;
				if( distance_m < 0.0) valid  = false;
				if(lineIndex == 0) start_copy  = true;
				if(start_copy){
					pc_radius_meta temp;
					temp.data = pmsg->pcDataOneFrame[j].UDP_PC_payload[index];
					temp.vaild = valid;
					temp.around_count = 0;
					pc_all_line[lineIndex] = temp;
					if(lineIndex == LIGHTNING_MAX_LINES - 1) {
						for(auto &it : pc_all_line) {
							oneFrame360.pcDataOneFrame.push_back(it.data);
							oneFrame360.vaildV.push_back(it.vaild);
							oneFrame360.around_count.push_back(it.around_count);
						}
					}
				}
			}
			if( distance_m < 0.0) continue;
			int locIndex = -1;
			for(int i = 0; i < rangeSegV.size(); i++){
				if(distance_m <= rangeSegV[i]) {
					locIndex = i;
					break;
				}
			}
			if(locIndex < 0 && rangeSegV.size() > 0) locIndex = rangeSegV.size() - 1;
			//if(locIndex < 0) continue;
			if(fMode & filterMode::RANGE_F){
				if(distance_m > maxPcValueRange_) continue;
				int index_r = (distance_m) / intervalRange_;
				if(index_r >= hRangeSize) index_r = hRangeSize - 1;
				shRangeV[index_r]++;
			}		
			double speed_m = pmsg->pcDataOneFrame[j].UDP_PC_payload[index].pcmSpeed * speed_bin;
			if(fMode & filterMode::SPEED_F){
				if(speed_m < minPcValueSpeedV_[locIndex] || speed_m > maxPcValueSpeedV_[locIndex])continue;
				int index_s = (speed_m - minPcValueSpeedV_[locIndex]) / intervalSpeedV_[locIndex];
				if(index_s >= shSpeedVV[locIndex].size()) index_s = shSpeedVV[locIndex].size() - 1;
				shSpeedVV[locIndex][index_s]++;
			}
			uint32_t  indensity_m = pmsg->pcDataOneFrame[j].UDP_PC_payload[index].pcmIndensity;
			uint32_t intenIndex = indensity_m / 3000;
			uint32_t  reflectiviy_m = (uint32_t) (indensity_m * distance_m * distance_m);
			uint32_t reIndex = reflectiviy_m / 15000;
			//intenNumMap[intenIndex]++;
			//reflectNumMap[reIndex]++;
			if(fMode & filterMode::INTEN_F){
				if(indensity_m > maxPcValueIntenV_[locIndex])continue;
				int index_s = (indensity_m) / intervalIntenV_[locIndex];
				if(index_s >= shIntenVV[locIndex].size()) index_s = shIntenVV[locIndex].size() - 1;
				shIntenVV[locIndex][index_s]++;
			}
			if(distance_min > distance_m )distance_min = distance_m;
			if(distance_max < distance_m )distance_max = distance_m;
			if(indensity_min > indensity_m )indensity_min = indensity_m;
			if(indensity_max < indensity_m )indensity_max = indensity_m;
			if(speed_min > speed_m )speed_min = speed_m;
			if(speed_max < speed_m )speed_max = speed_m;
		}
	}

/* 	getMaxIntensityNum = 1;
	if(strColor == "intensity"){
		auto func = [](std::pair<uint32_t, int> left, std::pair<uint32_t,int> right) {return left.second < right.second;};
		auto it_map = std::max_element(intenNumMap.begin(), intenNumMap.end(), func);
		std::cout << "most intensity is " << it_map->first << ", num is " << it_map->second << std::endl;	
	}
	getMaxReflectNum = 1;
	if(strColor == "reflectivity"){
		auto func = [](std::pair<uint32_t, int> left, std::pair<uint32_t,int> right) {return left.second < right.second;};
		auto it_map = std::max_element(reflectNumMap.begin(), reflectNumMap.end(), func);
		std::cout << "most reflect is " << it_map->first << ", num is " << it_map->second << std::endl;	
	} */
	if(fMode & filterMode::RADIUS_F) radiusFilterProc();
/* 	std::cout << "distance_min: " << distance_min << " distance_max: " << distance_max \
	<< " indensity_min: " << indensity_min << " indensity_max: " << indensity_max << " speed_min: " << speed_min \ 
	<< " speed_max: " << speed_max << std::endl;  */
}

void viewpanel::pcDataProc()
{
	udpPcMsgOneFrame* pmsg = nullptr;
	static long long frame_index = 0;
	if(udpPcMsg_done_buf_queue.get(pmsg)){
		return;
	}
	int pcFrameSize = 0;
	double horizontal_m = 0.0;
	bool begin_save = false;
	bool one_frame_360 = false;
	double horizontal_last = 0.0;
	bool oneFramePure_ = oneFramePure;
#if 1
	if(udpPCBuff_last.pcDataOneFrame.empty() && !oneFramePure){
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
	oneFrame360.pcDataOneFrame.clear();
	if(!oneFramePure_){
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
				ROS_INFO("warning! one_frame_360 is done in udpPCBuff_last");
				one_frame_360 = true;
				break;
			}
		}
		if(oneFrame360.pcDataOneFrame.empty()){
			ROS_INFO("oneFrame360.pcDataOneFrame is empty, can't find 0, frame index is %d", udpPCBuff_last.frameCounter[0]);
		}

		begin_save = false;
		oneFrame360.frameCounterCur = pmsg->pcDataOneFrame[0].UDP_PC_head.uphFrameCounter;
		if(!oneFrame360.pcDataOneFrame.empty())horizontal_last = oneFrame360.pcDataOneFrame[oneFrame360.pcDataOneFrame.size() - 1].pcmHorizontal * horizontal_bin;
		if(horizontal_last > 360.0) horizontal_last -= 360.0; 
	}
	pcFrameSize = pmsg->pcDataOneFrame.size();
	udpPCBuff_last.pcDataOneFrame.clear();
	udpPCBuff_last.frameCounter.clear();
	bool find_flag = true;
	if(!one_frame_360){
		for(int j = 0; j < pcFrameSize; j++)
		{
			for(int index = 0; index < UDP_PC_SIZE_SINGLE_V01; index++){	
				if(oneFramePure_) {
					oneFrame360.pcDataOneFrame.push_back(pmsg->pcDataOneFrame[j].UDP_PC_payload[index]);
					oneFrame360.frameCounter.push_back(pmsg->pcDataOneFrame[j].UDP_PC_head.uphFrameCounter);
					continue;
				}
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
	auto start = std::chrono::steady_clock::now();
	for(int i = 0; i < LIGHTNING_MAX_LINES; i++)
		distance_offset[i] = distanceOffsetEditV[i]->text().toDouble();
	rotation_offset = rotation_spin->value();
	speed_critical = speed_critical_edit->text().toDouble();
	leftAngle_offset = left_angle_edit->text().toDouble();
	rightAngle_offset = right_angle_edit->text().toDouble();
	//color_base = color_base_edit->text().toDouble();
	color_base = colorSlider->value();

	QString strColor = colorCombo->currentText();
	QString modeFilter = filterCombo->currentText();
	int modeFilterCur = modeFilter_;
	/*pc filter*/
	auto start1 = std::chrono::steady_clock::now();

	pcDataFilterPreProc(pmsg, modeFilterCur);

	auto end1 = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed_1 = end1 - start1;
	std::cout << "======time for pcDataFilterPreProc: " <<  elapsed_1.count() * 1000 << " ms" << std::endl;    

	udpPcMsg_free_buf_queue.put(pmsg);
	double distance_m, vertical_m, intensity_m, speed_m, reflectivity_m;
	int chan_id_m, index_rgb;
#if SINGELE_PC_SAVE
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
	int lineIndex = 0;
	int locIndex = -1;
	int thld = 10;
	if(modeFilterCur & filterMode::RADIUS_F){
		if(oneFrame360.around_count.size() != pcFrameSize) {
			std::cout << "!!!!!!RADIUS_F ERROR!!around_count is wrong! "  << std::endl;
		}
	}
	for(int j = 0; j < pcFrameSize; j++)
	{
		horizontal_m = oneFrame360.pcDataOneFrame[j].pcmHorizontal * horizontal_bin;
		if(horizontal_m > 360.0) horizontal_m -= 360.0;
		//if( horizontal_m < leftAngle_offset && horizontal_m > rightAngle_offset) continue;
		lineIndex = oneFrame360.pcDataOneFrame[j].pcmVerticalIndex;
		if(lineIndex > 15 || lineIndex < 0 ) continue;
		if(!checkPCShowV[lineIndex]->isChecked()) continue;
		distance_m = oneFrame360.pcDataOneFrame[j].pcmDistance * distance_bin - distance_offset[lineIndex];
		if(distance_m < 0.0) continue;
		if(modeFilterCur & filterMode::RADIUS_F){
			if(oneFrame360.around_count.empty()) continue;
			if(oneFrame360.around_count[j] < th_radius) continue;	
		}
		for(int i = 0; i < rangeSegV.size(); i++){
			if(distance_m <= rangeSegV[i]) {
				locIndex = i;
				break;
			}
		}
		if(locIndex < 0 && rangeSegV.size() > 0) locIndex = rangeSegV.size() - 1;

		speed_m = oneFrame360.pcDataOneFrame[j].pcmSpeed * speed_bin;

		if(modeFilterCur & filterMode::SPEED_F){
			if(locIndex < 0 || minPcValueSpeedV_.empty() || maxPcValueSpeedV_.empty() || \ 
			intervalSpeedV_.empty() || thresholdSpeedV_.empty() || shSpeedVV.empty()) continue;
			if(speed_m < minPcValueSpeedV_[locIndex] || speed_m > maxPcValueSpeedV_[locIndex]) continue;
			int index_s = (speed_m - minPcValueSpeedV_[locIndex]) / intervalSpeedV_[locIndex];
			if(index_s >= shSpeedVV[locIndex].size()) index_s = shSpeedVV[locIndex].size() - 1;
			if(shSpeedVV[locIndex][index_s] < thresholdSpeedV_[locIndex]) continue;
		}
		vertical_m = fov_vertical[lineIndex];
		chan_id_m = lineIndex / 4 + 1;
		if(modeFilterCur & filterMode::RANGE_F){
			if(locIndex < 0 || thresholdRangeV_.empty() || shRangeV.empty()) continue;
			if(distance_m > maxPcValueRange_) continue;
			int index_i = (int)((distance_m) / intervalRange_);
			if(index_i >= shRangeV.size()) index_i =  shRangeV.size() - 1;
			if(shRangeV[index_i] < thresholdRangeV_[locIndex]) continue;
		}
		intensity_m = oneFrame360.pcDataOneFrame[j].pcmIndensity;
		reflectivity_m = intensity_m * distance_m * distance_m;
		if(modeFilterCur & filterMode::INTEN_F){
			if(locIndex < 0 ||  maxPcValueIntenV_.empty() || intervalIntenV_.empty() || thresholdIntenV_.empty() || shIntenVV.empty()) continue;
			if(intensity_m > maxPcValueIntenV_[locIndex]) continue;
			int index_s = (intensity_m) / intervalIntenV_[locIndex];
			if(index_s >= shIntenVV[locIndex].size()) index_s = shIntenVV[locIndex].size() - 1;
			if(shIntenVV[locIndex][index_s] < thresholdIntenV_[locIndex]) continue;			
		}
#if SINGELE_PC_SAVE
		if(udpPCSingle_) {
			csvfile << oneFrame360.pcDataOneFrame[j].pcmIndensity << "," << distance_m << "," << speed_m << "," \
			<< vertical_m << ", " << horizontal_m << "," << oneFrame360.frameCounter[j] <<  "\n";
		}
#endif
		realSize++;
		cloud.points[j].vertical = vertical_m;
		cloud.points[j].horizontal = horizontal_m;
		cloud.points[j].distance = distance_m;
		cloud.points[j].indensity = intensity_m;
		cloud.points[j].speed = speed_m;
		cloud.points[j].chan_id = chan_id_m;
		cloud.points[j].line_id = lineIndex + 1;
		cloud.points[j].reflectivity = reflectivity_m;
		horizontal_m += rotation_offset;
		cloud.points[j].x = distance_m * cos(vertical_m * PI_FMCW / 180) * \
															cos(horizontal_m * PI_FMCW / 180);
		cloud.points[j].y = distance_m * cos(vertical_m * PI_FMCW / 180) * \
															sin(horizontal_m * PI_FMCW / 180) * (-1.0);
		cloud.points[j].z = distance_m * sin(vertical_m * PI_FMCW / 180) * (1.0);
		if(strColor == "range")
			index_rgb = distance_m / color_base * R_V_g.size();
		else if(strColor == "intensity")
			//index_rgb = (intensity_m - indensity_min) / (indensity_max - indensity_min) * R_V_g.size();
			index_rgb = (intensity_m) / (color_base) * R_V_g.size();
		else if(strColor == "reflectivity"){
			index_rgb = (reflectivity_m) / (color_base) * R_V_g.size();
		}
		else if(strColor == "speed"){
			uint8_t r, g, b = 0;
			if(speed_m < 0 && speed_m < -speed_critical){
				r = 255;
				g = 0;
				b = 0;
			} else if(speed_m > 0 && speed_m > speed_critical) {
				r = 13;
				g = 206;
				b = 235;
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
		} else if(strColor == "channel"){
			cloud.points[j].r = chanColorV[chan_id_m - 1][0];
			cloud.points[j].g = chanColorV[chan_id_m - 1][1];
			cloud.points[j].b = chanColorV[chan_id_m - 1][2];			
		}

		if(index_rgb > (R_V_g.size() - 1)) index_rgb = R_V_g.size() - 1;
		if(index_rgb < 0) index_rgb = 0;
		if(strColor != "speed" && strColor != "channel"){
			cloud.points[j].r = R_V_g[index_rgb];
			cloud.points[j].g = G_V_g[index_rgb];
			cloud.points[j].b = B_V_g[index_rgb];
		}
	}
	ROS_INFO("====PC Show Real Size is %d", realSize);
#if SINGELE_PC_SAVE
	csvfile.close();
#endif
	if(udpPCSingle_) udpPCSingle_ = false;
	output.header.stamp = ros::Time::now();
	pcl::toROSMsg(cloud,output);
	output.header.frame_id = "image_lidar";
	fmcw_pcl_pub.publish(output);
	cloud.clear();
	auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed = end - start;
	std::cout << "======time for pcDataProc of post process: " <<  elapsed.count() * 1000 << " ms" << std::endl;    
}

void viewpanel::pcParseLoop()
{
	std::cout << "enter pcParseLoop main" << std::endl;
    std::chrono::duration<double> elapsed;
	while(!terminating)
	{
		auto start = std::chrono::steady_clock::now();
		pcDataProc();
		auto end = std::chrono::steady_clock::now();
		elapsed = end - start;
		std::cout << "=======time for pcDataProc of all: " <<  elapsed.count() * 1000 << " ms" << std::endl;    
		if(udpPCStop_) break;
	}
	std::cout << "quit pcParseLoop" << std::endl;
}
void viewpanel::udpParseFftAdcLoop()
{
    std::chrono::duration<double> elapsed;
	while(!terminating)
	{
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
		std::cout << "time for parse FFT ADC data: " <<  elapsed.count() * 1000 << " ms" << std::endl;    
		if(udpFftAdcStop_) break;
	}
	std::cout << "quit udpParseFftAdcLoop" << std::endl;
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
		ret = recvfrom(udpRecvPCSocketFd_, &pcDataRaw_, sizeof(pcDataRaw_), MSG_WAITALL, (struct sockaddr*)&src_addr, &len);  //接收来自server的信息
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
	long bytesNum = 0;
	while(!terminating && !udpPCStop_)
	{
		auto start = std::chrono::steady_clock::now();
		pcDataOneFrame_.clear();
		ifLost  = false;
		bytesNum = 0;
		for(int i = 0; i < UDP_PC_TIMES_PER_FRAME; i++){
			memset(&pcDataRaw_, 0, sizeof(pcDataRaw_));
			ret = recvfrom(udpRecvPCSocketFd_, &pcDataRaw_, sizeof(pcDataRaw_), MSG_WAITALL, (struct sockaddr*)&src_addr, &len);  //接收来自server的信息
			if(ret <= 0){
				byteSpeed_ = 0;
				if(udpPCStop_) {
					ROS_INFO("pc udp recv thread quit!"); 
    				::close(udpRecvPCSocketFd_);
					return;
				}
				ROS_INFO("pc raw data recv failed, continue\n");
				usleep(100*1000);
				i--;
				continue;
			}
			bytesNum += ret;
			if(i == 0) head_frame_index = pcDataRaw_.UDP_PC_head.uphFrameCounter;
			if(pcDataRaw_.UDP_PC_head.uphFrameCounter!= head_frame_index) {
				std::cout << "!!!warnning!!! current usFrameCounter is " << pcDataRaw_.UDP_PC_head.uphFrameCounter \
				 << ", head_frame_index is " << head_frame_index << std::endl;	
				break;
			} 
			pcDataOneFrame_.emplace_back(pcDataRaw_);
		}
		auto end = std::chrono::steady_clock::now();
		elapsed = end - start;
		std::cout << "udp cost time for recv one frame pc raw data: " <<  elapsed.count() * 1000 << " ms" << std::endl;  
		float temp = calcFpsAndTransSpeed(bytesNum);
		if(temp > 0) byteSpeed_ =  temp / 1024.0;
		if(pcDataOneFrame_.size() > 2){
			udpPcMsgOneFrame* pUdp = NULL;
			if(udpPcMsg_free_buf_queue.get(pUdp)){
				byteSpeed_ = -1;
				ROS_INFO("!!!!error!!!!udpPcMsg_free_buf_queue is empty, continue\n");
			}else{
				pUdp->pcDataOneFrame = pcDataOneFrame_;
				udpPcMsg_done_buf_queue.put(pUdp);	
			}
		}
	}
	byteSpeed_ = 0;
	ROS_INFO("pc udp recv thread quit!");	
}

void viewpanel::udpRecvFftAdcLoop(){

	//int client_fd;
	lidar_UDP_port = udp_port_edit->text().toInt();
	struct sockaddr_in ser_addr;

	udpFftAdcSocketFd_ = socket(AF_INET, SOCK_DGRAM, 0);
	if(udpFftAdcSocketFd_ < 0)
	{
		QMessageBox msgBox;
		msgBox.setText("udp socket create fail!");
		msgBox.exec();
		return;
	}

	struct timeval timeout_recv = {3, 0};
	setsockopt(udpFftAdcSocketFd_, SOL_SOCKET, SO_RCVTIMEO, &timeout_recv, sizeof(timeout_recv)); //recv timeout

	memset(&ser_addr, 0, sizeof(ser_addr));
	ser_addr.sin_family = AF_INET;
	//ser_addr.sin_addr.s_addr = inet_addr(lidar_ip.c_str());
	ser_addr.sin_addr.s_addr = htonl(INADDR_ANY);  //注意网络序转换
	ser_addr.sin_port = htons(lidar_UDP_port);  //注意网络序转换
	std::cout << "lidar_UDP_port is " << lidar_UDP_port << std::endl;

    int ret = ::bind(udpFftAdcSocketFd_, (struct sockaddr*)&ser_addr, sizeof(ser_addr));
    if(ret < 0)
    {
		QMessageBox msgBox;
		msgBox.setText("udp socket bind fail!");
		msgBox.exec();
		return;
    }
	udpFftAdcStop_ = false;
	socklen_t len;
	struct sockaddr_in src;
	//printf("ready recv udp msg!\n");
	len = sizeof(sockaddr);
	std::vector<uint8_t> fftDataV;
	std::vector<uint8_t> adcDataV;
	uint32_t last_frame_index = 0;
	bool ifLost  = false;
    std::chrono::duration<double> elapsed;
	while(!terminating && !udpFftAdcStop_)
	{
		auto start = std::chrono::steady_clock::now();
		fftDataV.clear();
		adcDataV.clear();
		ifLost  = false;
		for(int i = 0; i < UDP_TIMES_PER_FRAME; i++){
			memset(&g_udpMsg, 0, sizeof(g_udpMsg));
			//printf("ready recv udp msg!\n");
			ret = recvfrom(udpFftAdcSocketFd_, &g_udpMsg, sizeof(g_udpMsg), MSG_WAITALL, (struct sockaddr*)&src, &len);  //接收来自server的信息
			if(ret <= 0){
				if(udpFftAdcStop_) {
					printf("fftMsg udp  quit!\n"); 
    				::close(udpFftAdcSocketFd_);
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
			if(i < UDP_TIMES_PER_FRAME) {
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
 
    ::close(udpFftAdcSocketFd_);
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
	QString nameSerialTest = QString("/dev/pts/666");
	m_serialPort_test->setPortName(nameSerialTest);

	if(!m_serialPort_test->open(QIODevice::ReadWrite))//用ReadWrite 的模式尝试打开串口
	{
		ROS_INFO(L_RED"open %s serial device failed"NONE_COLOR, nameSerialTest.toStdString().c_str());
		return -1;
	}
	//打开成功
	qDebug()<< nameSerialTest <<"open successfully!";
    m_serialPort_test->setBaudRate(QSerialPort::Baud115200,QSerialPort::AllDirections);//设置波特率和读写方向
    m_serialPort_test->setDataBits(QSerialPort::Data8);		//数据位为8位
    m_serialPort_test->setFlowControl(QSerialPort::NoFlowControl);//无流控制
    m_serialPort_test->setParity(QSerialPort::NoParity);	//无校验位
    m_serialPort_test->setStopBits(QSerialPort::OneStop); //一位停止位

	connect(m_serialPort_test,SIGNAL(readyRead()),this,SLOT(recvSerialInfoTest()));
	m_serialPort_test_open = true;
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
	ifConnectedMotorSerial = true;
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

	struct timeval timeout_recv = {5, 0};
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
	ifConnectedMotorTcp  = true;
	//fcntl(ctrl_sock, F_SETFL, O_NONBLOCK); /* Set the socket to non blocking mode */
	return 0;
}

int viewpanel::stateConnect()
{
	int one = 1;
	lidar_ip = ip_edit->text().toStdString();
	state_port = 5002;
	ROS_INFO(L_GREEN"lidar_ip is %s, state_port is %d"NONE_COLOR, lidar_ip.c_str(), state_port);
	struct sockaddr_in ctrl_serv_addr;
	if ((state_ctrl_sock=socket(AF_INET, SOCK_STREAM, 0))==-1){
		ROS_DEBUG("ERROR: Could not create socket!");
		return -1;
	}
	int nRecvBuf=320 * 1024;//设置为32K
	setsockopt(state_ctrl_sock, SOL_SOCKET,SO_RCVBUF,(const char*)&nRecvBuf,sizeof(int));

	int nSendBuf=320 * 1024;//设置为32K
	setsockopt(state_ctrl_sock, SOL_SOCKET, SO_SNDBUF,(const char*)&nSendBuf,sizeof(int));
	setsockopt(state_ctrl_sock, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));


#if 1
	struct timeval timeout_send = {2, 0};
	setsockopt(state_ctrl_sock, SOL_SOCKET, SO_SNDTIMEO, &timeout_send, sizeof(timeout_send)); //send timeout

	struct timeval timeout_recv = {4, 0};
	setsockopt(state_ctrl_sock, SOL_SOCKET, SO_RCVTIMEO, &timeout_recv, sizeof(timeout_recv)); //recv timeout

#endif
	memset(&ctrl_serv_addr, 0, sizeof(ctrl_serv_addr));
	ctrl_serv_addr.sin_family = AF_INET;
	ctrl_serv_addr.sin_addr.s_addr = inet_addr(lidar_ip.c_str());
	ctrl_serv_addr.sin_port = htons(state_port);

	if(::connect(state_ctrl_sock, (struct sockaddr*)&ctrl_serv_addr, sizeof(ctrl_serv_addr))<0)
	{
		ROS_INFO("Failed to connect to lidar_ip %s port: %d", lidar_ip.c_str(), state_port);
		return -2; /* Completely fail only if first radar didn't connect */
	}
	ifConnectedStateTcp  = true;
	//fcntl(ctrl_sock, F_SETFL, O_NONBLOCK); /* Set the socket to non blocking mode */
	return 0;
}



int viewpanel::lidarConnect()
{
	int err;
	int one = 1;

	lidar_ip = ip_edit->text().toStdString();
	lidar_ctrl_port = port_edit->text().toInt();
	ROS_INFO(L_GREEN"lidar_ip is %s, lidar_ctrl_port is %d"NONE_COLOR, lidar_ip.c_str(), lidar_ctrl_port);

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

	lidar_ip_ = settings.value("IP Addr","10.20.30.40").toString();
	for(int i = 0; i < LIGHTNING_REG_NUM; i++){
		m_reg_addr_[i] = settings.value("Reg Addr " + QString::number(i), regAddrList[i]).toString();
		m_reg_value_[i] = settings.value("Reg Value " + QString::number(i), regValueList[i]).toString();
	}
	lidar_UDP_PC_port_ = settings.value("UDP PC Port","8001").toString();
	lidar_ctrl_port_ = settings.value("TCP Port","5000").toString();
	lidar_UDP_port_ = settings.value("UDP Port","8000").toString();

	for(int i = 0; i < LIGHTNING_MAX_LINES; i++){
		distance_offset_[i] = settings.value("Distance Offset " + QString::number(i + 1), "13.672").toString();
	}

	rotation_offset = settings.value("rotate angle","45.0").toDouble();
	speed_critical = settings.value("critical speed","0.4").toDouble();
	leftAngle_offset = settings.value("left angle","306.0").toDouble();
	color_base = settings.value("color base","10.0").toDouble();
	point_size = settings.value("point size","0.03").toDouble();
	cell_size = settings.value("cell size","10.0").toDouble();
	cell_3d_size = settings.value("cell 3d size","2").toDouble();
	axes_size = settings.value("axes size","0.1").toDouble();
	rightAngle_offset = settings.value("right angle","125.0").toDouble();
	power_offset_ = settings.value("Power Offset","0.0").toString();
	power_min_ = settings.value("Power Min","120.0").toString();
	save_folder_ = settings.value("Save Folder",".").toString();
	motor_port_ = settings.value("TCP Motor Port","5001").toString();
}

void viewpanel::load_SF_settings()
{
	QSettings settings(QCoreApplication::organizationName(),
		QCoreApplication::applicationName());

	for(int i = 0; i < sfParaSpeedEditV.size(); i++){
		if(sfParaSpeedEditV[i]) sfParaSpeedEditV[i]->setText(settings.value("sf para " + QString::number(i)).toString()); 
	}

	for(int i = 0; i < sfParaIntenEditV.size(); i++){
		if(sfParaIntenEditV[i]) sfParaIntenEditV[i]->setText(settings.value("sf para inten " + QString::number(i)).toString()); 
	}

	for(int i = 0; i < sfParaRangeEditV.size(); i++){
		if(sfParaRangeEditV[i]) sfParaRangeEditV[i]->setText(settings.value("sf para range " + QString::number(i)).toString()); 
	}

	for(int i = 0; i < sfParaRadiusEditV.size(); i++){
		if(sfParaRadiusEditV[i]) sfParaRadiusEditV[i]->setText(settings.value("sf para radius  " + QString::number(i)).toString()); 
	}

	for(int i = 0; i < rangeSegmentEditV.size(); i++){
		if(rangeSegmentEditV[i]) rangeSegmentEditV[i]->setText(settings.value("range seg " + QString::number(i)).toString()); 
	}

	for(int i = 0; i < checkSfWorkV.size(); i++){
		if(checkSfWorkV[i]) checkSfWorkV[i]->setChecked(settings.value("sf mode " + QString::number(i), false).toBool()); 
	}
}

void viewpanel:: readMotorItemsFile()
{
	//qDebug() <<  QCoreApplication::applicationDirPath();
	std::string item_name;
	std::string motor_item_temp;
	//The server_host should be corresponding to the robot controller setup.
	motorItemMap.clear();
	std::string motor_item = "/lightning_gui/MotorItem";
	for(int i = 0 ; i < 10; i++){
		motor_item_temp = motor_item + std::to_string(i);
		bool res = ros::param::get(motor_item_temp, item_name); 
		if(res) motorItemMap.insert(std::pair<int, std::string>(i, item_name));
	}

	for(const auto & it : motorItemMap){
		std::cout << it.first << ", "<<  it.second << std::endl;
	}
#if 0
	std::ifstream file("./test0.txt", std::ios::in);
	if (! file.is_open()){ 
		std::cout << "=======================Error opening file" << std::endl;
	}
	//while (!in.eof() )
	std::string s;
	while (getline(file, s))
	{
		std::cout << s << std::endl;
	}
	file.close();
#endif
}

void viewpanel:: motorInfoShow(uint8_t *ptr, int datalen)
{
	if(!ptr || datalen < 5) return;
	QVector<double> x_pos;
	x_pos.clear();
	QVector<double> y_pos[MOTOR_ITEMS_NUM];
	for(int i = 0; i < MOTOR_ITEMS_NUM; i++)
		y_pos[i].clear();
	int item_index = 0;
	static int frame_index = 0;
	x_pos.append(frame_index);
	QVector<int> itemV;
	QVector<float> itemData;
	//itemV.clear();
	for(int i = 0; i < datalen; i = i + 5){
		item_index = ptr[i];
		itemV.append(item_index);
		if(frame_index == 0) {
			setCheckBoxUnvaild(checkShowV[item_index], true);
			checkShowV[item_index]->setChecked(true);
			pMotorchart->setGraphShow(item_index, true);
		}
		if(item_index > MOTOR_ITEMS_NUM - 1) {
			ROS_INFO("error!!! item_index > MOTOR_ITEMS_NUM - 1, item_index is %d", item_index);
		}
		double dataT = UnsignedChar4ToFloat(&(ptr[i + 1]));
		y_pos[item_index].append(dataT);
		pMotorchart->setData(x_pos, y_pos[item_index], item_index);
		//pMotorchart->setData(QVector<double>(10, 10), QVector<double>(10, 10), item_index);

	}
	if(frame_index == 0){
		bool find = false;
		for(int i = 0; i < checkShowV.size(); i++){
			find = false;
			for(int j = 0; j < itemV.size(); j++){
				if(i  == itemV[j]){	
					find = true;
					break;		
				}
			}
			if(!find)setCheckBoxUnvaild(checkShowV[i]);
		}
	}
	frame_index++;
}


void viewpanel::save_SF_settings(void )
{
	QSettings settings(QCoreApplication::organizationName(),
		QCoreApplication::applicationName());
	for(int i = 0; i < sfParaSpeedEditV.size(); i++){
		settings.setValue("sf para " + QString::number(i), sfParaSpeedEditV[i]->text());
	}

	for(int i = 0; i < sfParaIntenEditV.size(); i++){
		settings.setValue("sf para inten " + QString::number(i), sfParaIntenEditV[i]->text());
	}

	for(int i = 0; i < sfParaRangeEditV.size(); i++){
		settings.setValue("sf para range " + QString::number(i), sfParaRangeEditV[i]->text());
	}

	for(int i = 0; i < sfParaRadiusEditV.size(); i++){
		settings.setValue("sf para radius  " + QString::number(i), sfParaRadiusEditV[i]->text());
	}

	for(int i = 0; i < rangeSegmentEditV.size(); i++){
		settings.setValue("range seg " + QString::number(i), rangeSegmentEditV[i]->text());
	}
	for(int i = 0; i < checkSfWorkV.size(); i++){
		settings.setValue("sf mode " + QString::number(i), checkSfWorkV[i]->isChecked());
	}
}
void viewpanel::save_settings(void )
{
	ROS_INFO(L_GREEN"enter save_settings"NONE_COLOR);
	QSettings settings(QCoreApplication::organizationName(),
		QCoreApplication::applicationName());
	settings.setValue("IP Addr", ip_edit->text());
	settings.setValue("TCP Port", port_edit->text());
	settings.setValue("UDP Port", udp_port_edit->text());
	settings.setValue("UDP PC Port", udp_pc_port_edit->text());
	for(int i = 0; i < distanceOffsetEditV.size(); i++){
		settings.setValue("Distance Offset " + QString::number(i + 1), distanceOffsetEditV[i]->text());
	}

	settings.setValue("rotate angle", rotation_spin->text());
	settings.setValue("critical speed", speed_critical_edit->text());
	settings.setValue("left angle", left_angle_edit->text());
	settings.setValue("right angle", right_angle_edit->text());

	settings.setValue("cell size", cell_size_edit->text());
	settings.setValue("cell 3d size", cell_3d_size_edit->text());
	settings.setValue("point size", point_size_edit->text());
	settings.setValue("color base", color_base_edit->text());
	settings.setValue("axes size", axes_size_edit->text());

	settings.setValue("Power Offset", power_Offset_edit->text());
	settings.setValue("Power Min", power_Min_edit->text());
	settings.setValue("Save Folder", save_folder_);
	settings.setValue("TCP Motor Port", motorConnectPortLine->text());

	for(int i = 0; i < LIGHTNING_REG_NUM; i++){
		settings.setValue("Reg Addr " + QString::number(i), regAddr_line[i]->text());
		settings.setValue("Reg Value " + QString::number(i), regVal_line[i]->text());
	}
}
