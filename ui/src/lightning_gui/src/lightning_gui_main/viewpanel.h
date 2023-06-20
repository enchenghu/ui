
#ifndef viewpanel_H
#define viewpanel_H

#include <arpa/inet.h>
#include <fcntl.h>
#include <pc_data.h>
#include "qtCommon.hpp"
#include <pcl/common/transforms.h>
#include <pcl/filters/voxel_grid.h>
#include <pcl/io/pcd_io.h>
#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <pcl_conversions/pcl_conversions.h>
#include <pthread.h>
#include <sensor_msgs/PointCloud2.h>
#include <std_msgs/Bool.h>
#include <std_msgs/Float32.h>
#include <std_msgs/String.h>
#include <std_msgs/UInt8.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <tf2_geometry_msgs/tf2_geometry_msgs.h>

#include <QApplication>
#include <QCheckBox>
#include <QColor>
#include <QComboBox>
#include <QDebug>
#include <QDesktopWidget>
#include <QDockWidget>
#include <QFileDialog>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QMainWindow>
#include <QMessageBox>
#include <QPushButton>
#include <QShortcut>
#include <QSlider>
#include <QStyleOptionSlider>
#include <QTimer>
#include <QToolTip>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QWidget>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <cmath>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>

#include "bst_msg_queue.h"
#include "chartLightning.h"
#include "flidar_stat.h"
#include "plot_tracer.h"
#include "qcustomplot.h"
#include "rviz/display.h"
#include "rviz/panel.h"
#include "rviz/render_panel.h"
#include "rviz/selection_panel.h"
#include "rviz/tool_manager.h"
#include "rviz/view_manager.h"
#include "rviz/visualization_manager.h"
#include "types_fmcw.hpp"
#include <visualization_msgs/MarkerArray.h>
#include "vx_mutex.h"
#include "vx_queue.h"
#include "vx_task.h"
#include "systemMonitor.h"

using namespace fmcw_types;
namespace rviz {
class Display;
class RenderPanel;
class VisualizationManager;
}  // namespace rviz

typedef enum {
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
} eViewType;

typedef struct view_vals_t {
  QVariant distance;
  QVariant yaw;
  QVariant pitch;
  QVariant focal_point;
} view_vals_t;

class viewpanel : public QTabWidget {
  Q_OBJECT
 public:
  viewpanel(QTabWidget* parent = 0);
  virtual ~viewpanel();
  void setView(view_vals_t& view_vals);
  void printView();
  static viewpanel* Instance();

 public Q_SLOTS:
  void startControl(void);
  void connectControl(void);
  void configPower(void);
  void configADCDSA(void);

  void configCFAR(void);
  void config3DFT(void);
  void configDiff(void);
  void configReg(int);
  void readReg(int);
  void ctrlShowWindows(bool);
  void ctrlShowPcOffset(bool);
  void ctrlPcInfoWindows(bool);
  void ctrlPcPanel(bool);
  void resetViews(void);
  void increasePointSize();
  void decreasePointSize();
  void colorChange();

 private Q_SLOTS:
  void setLoadFileType(void);
  void closeEvent(QCloseEvent* event);
  void saveDataThead();
  void loadLidarFile();
  void loadAlgFile();
  void loadPCFile();
  void setSaveFolder();
  void ConfigFilterDialog();
  void save_SF_settings();
  void ConfigFilterWork();
  void FFT_ADC_UDP_Connect();
  void fftChannelChange();
  void udpPcConnect();
  void startPcUdpOnce();
  void startPcUdpContinuous();
  void udpFftAdcClose();
  void udpPcClose();
  void configAxesSize();
  void configCellSize();
  void configCellOffset();
  void config3dCellSize();

  void configPointSize();
  void pcShowBW();
  void pcRecord();
  void pcOneFramePure();
  void filterChange();
  void colorInfoChange();
  void colorBarChange( int );

  void pcChSelect();

  void updateFFTdata();
  void updateADCdata();
  void updateMotorChart();
  void showdBFFT();
  void singleFFT();
  void singleADC();
  void singleMotor();
  void resetFFT();
  void resetADC();
  void resetMotor();
  void updateState();
  void printErrorLog();
  void slotTextTcpChanged();
  void recvSerialInfo();
  void parseMotorInfo(uint8_t*);
  void recvSerialInfoTest();
  void sendItemsInfoTest();
  void sendMotorConnectCmd();
  void sendMotorConnectCmdM();
  void sendMotorWorkModeCmd();
  void sendMotorDisplayCycleCmd();
  void sendMotorOpenCmd();
  void sendMotorReset();
  void sendMotorPidCmd();
  void readMotorPid();
  void readMotorWorkMode();
  void readHardVersion();
  void readSoftVersion();
  void readMotorShowItems();
  void motorItemsShow(int);
  void showADCDataSim();

 protected:
  static void TaskFunc(void* arg);
  static void TaskFuncUdpFftAdcRecv(void* arg);
  static void TaskFuncUdpFftAdcParse(void* arg);
  static void TaskFuncPCRecv(void* arg);
  static void TaskFuncPCParse(void* arg);
  static void TaskFuncMotorRecv(void* arg);
  static void TaskFuncStateRecv(void* arg);

 private:
  void udpRecvFftAdcLoop();
  void udpParseFftAdcLoop();
  void udpRecvPCLoop();
  void udpRecvPCOnce();
  int udpRecvPCConnect();
  void recvMotorInfoloop();
  void recvStateInfoloop();
  void procEdfaInfo(uint8_t* data, uint8_t cmd_id);
  void procNllInfo(uint8_t* data, uint8_t cmd_id);
  void sendSerialBytes(const uint8_t* begin, int size);
  void CreatFFTWindow();
  void CreatPCWindow();
  void CreatADCWindow();
  void CreatMotorWindow();
  void CreatStateDetectWindow();
  int lidarConnect();
  int motorConnect();
  int stateConnect();
  void sendMotorCmd(const uint8_t* begin, int size);
  int motorSerialConnect();
  int motorSerialConnectTest();
  int serialClose(QSerialPort*);
  void releaseSerial();
  int checkMotorConnected();
  int configRegLidar();
  void motorInfoShow(uint8_t* ptr, int datalen);
  void pcParseLoop();
  void pcDataProc();
  void pcDataFilterPreProc(udpPcMsgOneFrame*);
  void registerPointcloudRviz();
  void startPcTask();
  void startMotorTask();
  void startStateDectTask();
  void CreatConnect();
  void saveData();
  void Save2filecsv(std::vector<uint8_t>&, bool);
  void parseFFTData(std::vector<uint8_t>& data);
  void parseADCData(std::vector<uint8_t>& data);
  void simulateADCData();
  std::string tohex(uint32_t a);
  inline double fft2dBm(double x);
  void load_settings();
  void load_SF_settings();
  void save_settings();
  void init_queue();
  void readMotorItemsFile();
  void prepare_basic_markers( void );
  QTimer* timer_;
  QTimer* timer_adc;
  bool udpFftAdcStop_;
  bool udpPCStop_;
  bool udpPCSingle_;
  bool udpPCContinu_;
  bool showBlack;
  bool saveadc_;
  int ctrl_sock;
  int motor_ctrl_sock;
  int state_ctrl_sock;
  int udpFftAdcSocketFd_;
  int udpRecvPCSocketFd_;
  std::vector<uint8_t> dataSimulateADC;
  std::string lidar_ip;
  std::string reg_addr_;
  std::string reg_value_;
	pcData_t g_msg;
	udpMsg g_udpMsg;

  QByteArray motorBuffAll;
  int lidar_ctrl_port;
  int lidar_UDP_port;
  int lidar_UDP_pc_port;
  int motor_port;
  int state_port;
  QSerialPort* m_serialPort;
  QSerialPort* m_serialPort_test;
  bool m_serialPort_test_open;
  double distance_offset[LIGHTNING_MAX_LINES];
  double power_offset;
  double power_min;
  double rotation_offset;
  double speed_critical;
  double leftAngle_offset;
  double rightAngle_offset;
  double cell_size;
  double cell_offset;
  double cell_3d_size;

  double point_size;
  double color_base;
  double axes_size;
  double distance_min;
  double distance_max;
  double indensity_min;
  double indensity_max;
  double speed_min;
  double speed_max;
  float byteSpeed_;
  QString lidar_ctrl_port_;
  QString lidar_UDP_port_;
  QString lidar_UDP_PC_port_;
  QString lidar_ip_;
  QString motor_port_;
  QString m_reg_addr_[LIGHTNING_REG_NUM];
  QString m_reg_value_[LIGHTNING_REG_NUM];
  QString distance_offset_[LIGHTNING_MAX_LINES];
  QString power_offset_;
  QString power_min_;
  UDP_PC_package_st pcDataRaw_;
  std::vector<UDP_PC_package_st> pcDataOneFrame_;
  rviz::VisualizationManager* manager_;
  rviz::RenderPanel* render_panel_;
  rviz::SelectionPanel* selection_panel_;
  rviz::ToolManager* tool_panel_;
  ros::Publisher  lightning_info_markers;
  visualization_msgs::Marker detections_per_frame_marker;
  rviz::Display* grid_;
  rviz::Display* pointcloud_fmcw;
  rviz::Display* Car_;
  rviz::Display* FloatingText_;
  rviz::Display* FreeSpace_;
  rviz::Display* Imu_;
  rviz::Display* Axes_;
  rviz::Display* MeshDisplay_;
  rviz::Panel* Detections_Info_;
  QCloseEvent* event;
  rviz::Display* Speedometer_;
  pcl::PointCloud<FmcwPointXYZRGBGeneric> cloud;
  pcl::PointCloud<FmcwPointXYZRGBGeneric> cloud_360;
  sensor_msgs::PointCloud2 output;
  static viewpanel* m_pInstance;
  commandMsg cmdMsg_;
  std::map<int, std::string> motorItemMap;
  QComboBox* loadDataCombo;
  QComboBox* m3DFTCombo;
  QComboBox* CFARCombo;
  QComboBox* PowerCombo;
  QComboBox* ADC_DSA_Combo;
  QComboBox* filter_Combo;

  QDoubleSpinBox* rotation_spin;

  QComboBox* DiffCombo;
  QComboBox* colorCombo;
  QComboBox* filterCombo;
  QComboBox* savePCCombo;
  QLineEdit* ip_edit;
  QLineEdit* port_edit;
  QLineEdit* udp_port_edit;
  QLineEdit* udp_pc_port_edit;
  QLineEdit* distance_Offset_edit[4];
	std::vector<QLineEdit*> distanceOffsetEditV;
	std::vector<QLineEdit*> showInfoEditV;
	std::vector<QLineEdit*> rangeSegmentEditV;
	std::vector<QLineEdit*> sfParaSpeedEditV;
	std::vector<QLineEdit*> sfParaIntenEditV;
	std::vector<QLineEdit*> sfParaRangeEditV;
	std::vector<QLineEdit*> sfParaRadiusEditV;

  QLineEdit* rotate_angle_edit;
  QLineEdit* speed_critical_edit;
  QLineEdit* maxPcValue_edit;
  QLineEdit* intervalPcValue_edit;
  QLineEdit* thresholdValue_edit;

  QLineEdit* left_angle_edit;
  QLineEdit* right_angle_edit;
  QLineEdit* color_base_edit;
  QLineEdit* point_size_edit;
  QLineEdit* cell_size_edit;
  QLineEdit* cell_offset_edit;
  QLineEdit* cell_3d_size_edit;

  QLineEdit* point_size_interval_edit;
  QLineEdit* axes_size_edit;
  QLineEdit* power_Offset_edit;
  QLineEdit* power_Min_edit;
  QString save_folder_;
  QLineEdit* regAddr_line[LIGHTNING_REG_NUM];
  QLineEdit* regVal_line[LIGHTNING_REG_NUM];
  QLineEdit* regRead_line[LIGHTNING_REG_NUM];
  QLineEdit* adcRead0_line;
  QLineEdit* adcRead1_line;
  QLineEdit* motorPidReadLine[5];
  QLineEdit* motorPidCSetLine;
  QLineEdit* motorPidPSetLine;
  QLineEdit* motorPidISetLine;
  QLineEdit* motorPidDSetLine;
  QLineEdit* motorPidMaxLine;
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
  QComboBox* motorIDCombo;
  QComboBox* motorPidReadCombo;
  QPushButton* motorConnectBtnTcp;
  QPushButton* motorConnectBtnSerial;
  QPushButton* motorSwitchBtn;
  QPushButton* motorResetBtn;
  QPushButton* motorWorkModeSetBtn;
  QPushButton* motorWorkModeReadBtn;
  QPushButton* motorPidSetBtn;
  QPushButton* motorPidReadBtn;
  QPushButton* motorShowCycleSetBtn;
  QPushButton* motorShowItemsReadBtn;
  QPushButton* motorSoftVersionReadBtn;
  QPushButton* motorHardVersionReadBtn;
  QPushButton* motorDevReadBtn;
  QPushButton* motorChartResetBtn;
  QPushButton* motorChartSingleBtn;
  QStringList m_serialPortName;

  std::vector<QLineEdit*> edfaStateLinesV;
  std::vector<QLabel*> edfaWarnLEDV;
  std::vector<QLineEdit*> edfaDeviceInfoLinesV;
  QLabel* netStateLED;

  QPushButton* saveBtn;
  QLabel* devLabel0_state;
  QLabel* devLabel1_state;
  QLabel* devLabel2_state;
  QTextEdit* errorLogText;

  fftMsg fftBuff[MAX_BUFF_LEN];
  adcMsg adcBuff[MAX_BUFF_LEN];
  udp_ADC_FFT_Msg udpFABuff[MAX_BUFF_LEN];
  udpPcMsgOneFrame udpPCBuff[MAX_BUFF_LEN];
  udpPcMsgOneFrame360 udpPCBuff_last;
  motorMaxBuff motorBuff[MAX_BUFF_LEN];
  stateMaxBuff stateBuff[MAX_BUFF_LEN];

  bstMsgQueue<fftMsg*> fftMsg_free_buf_queue;
  bstMsgQueue<fftMsg*> fftMsg_done_buf_queue;
  bstMsgQueue<adcMsg*> adcMsg_free_buf_queue;
  bstMsgQueue<adcMsg*> adcMsg_done_buf_queue;
  bstMsgQueue<udp_ADC_FFT_Msg*> udpMsg_free_buf_queue;
  bstMsgQueue<udp_ADC_FFT_Msg*> udpMsg_done_buf_queue;
  bstMsgQueue<udpPcMsgOneFrame*> udpPcMsg_free_buf_queue;
  bstMsgQueue<udpPcMsgOneFrame*> udpPcMsg_done_buf_queue;
  bstMsgQueue<motorMaxBuff*> motorMsg_free_buf_queue;
  bstMsgQueue<motorMaxBuff*> motorMsg_done_buf_queue;
  bstMsgQueue<stateMaxBuff*> stateMsg_free_buf_queue;
  bstMsgQueue<stateMaxBuff*> stateMsg_done_buf_queue;

  std::string loadFileType_;
  QString loadLidarFile_;
  QString loadPCFile_;
  bool ifConnected;
  bool ifStarted;
  bool ifConnectedMotorTcp;
  bool ifConnectedStateTcp;
  bool ifConnectedMotorSerial;
  bool ifOpenMotor;
  showModel ifShowdB_;
  bool ifSave;
  bool oneFramePure;
  QPushButton* lidar_connect_button;
  QPushButton* setSaveBtn;
  QPushButton* pcSwitchBtn;
  QPushButton* pcBWBtn;
  QPushButton* pcRecordBtn;
  QPushButton* pcProcBtn;
  QPushButton* pcOnceBtn;
  QPushButton* pcResetBtn;
  QPushButton* loadAlgBtn;
  QPushButton* loadPCRawBtn;
  QPushButton* regBtnWrite[LIGHTNING_REG_NUM];
  QPushButton* regBtnRead[LIGHTNING_REG_NUM];
  QPushButton* settingADCSavebutton;
  QPushButton* settingADCConfigbutton;
  QComboBox* fftChCombo;
  QPushButton* singelFFTBtn_;
  QPushButton* resetFFTBtn_;
  QPushButton* singelADCBtn_;
  QPushButton* resetADCBtn_;
  QPushButton* mFFTShowdBBtn;
  QDockWidget* ctrlDock;
  QWidget* ctrlDockWidget;
  QWidget* fullScreenWidget;
  QDockWidget* pcOffsetDock;
  QWidget* pcOffsetDockWidget;
  QSlider* colorSlider;
  std::vector<QPushButton*> ctlWriteBtn_;
  std::vector<QPushButton*> ctlReadBtn_;
  std::vector<QLineEdit*> ctlReadLine_;
  QLineEdit* byteSpeedLine;
  std::vector<int> power_index;
  vx_task bst_task[BST_MAX_TASK_NUM];
  vx_task_create_params_t bst_params;
  std::shared_ptr<fmcw_types::fmcwPoints> fmcwPointsData_;
  fmcw_types::fmcwPoint curPcPoint;
  ChartLighting* pFFTchart[2];
  ChartLighting* pADCchart[2];
  ChartLighting* pMotorchart;
  QVector<QCheckBox*> checkShowV;
  QVector<QCheckBox*> checkPCShowV;
  QVector<QCheckBox*> checkSfWorkV;

  QCheckBox* selectAll;
  QCheckBox* selectNone;
  QVector<double> x_FFT;
  QVector<double> x_adc0;
  QVector<double> x_adc1;
  QVector<double> y_adc0;
  QVector<double> y_adc1;
  QVector<double> y_FFT;
  QVector<double> x_FFT_1;
  QVector<double> y_FFT_1;
  myTracer* plotTracer;
  ros::Publisher fmcw_pcl_pub;
  ros::Publisher fmcw_pc_datapath_pub;

  motorCmdMsg motorMsgSend_;
  motorCmdMsg1 motorMsgSend1_;
  motorPidMsg motorMsgPid_;
  motorShowMsg motorMsgShow_;
  motorWorkModeMsg motorMsgWorkMode_;
  motorShowCycleMsg motorMsgShowCycle_;
  motorPidSetMsg motorMsgPidSet_;
  double vertical_bin;
  double speed_bin;
  double horizontal_bin;
  double vertical_offset;
  double distance_bin;
  std::string point_size_;
  std::string cell_size_;
  std::vector<std::string> wordsAddr;  // 声明一个字符串向量
  std::vector<std::string> wordsVal;   // 声明一个字符串向量
	udpPcMsgOneFrame360 oneFrame360;
	std::vector<std::vector<int>> shSpeedVV;
	std::vector<std::vector<int>> shIntenVV;

	std::vector<std::vector<pc_radius_meta>> pcRadius2DVV;

	std::vector<int> shRangeV;
  std::vector<int> rangeSegV;

  std::vector<double> maxPcValueSpeedV_;
  double maxPcValueRange_;
  std::vector<double> maxPcValueIntenV_;

  std::vector<double> minPcValueSpeedV_;
  double minPcValueRange_;
  std::vector<double> minPcValueIntenV_;

  std::vector<double> intervalSpeedV_;
  double intervalRange_;
  std::vector<double> intervalIntenV_;

  double radius_sf;
  int th_radius;
  int width_radius;
  
  std::vector<int> thresholdSpeedV_;
  std::vector<int> thresholdRangeV_;
  std::vector<int> thresholdIntenV_;

  std::vector<QString> sfPara;
  std::map<uint32_t, int> intenNumMap;
  std::map<uint32_t, int> reflectNumMap;

  SystemMonitor* systemMonitor_m;
  int modeFilter_;
  bool getMaxIntensityNum;
  bool getMaxReflectNum;
  QString lastPCRawPath_; 
  QString lastAlgPath_; 

};

#endif  // viewpanel_H
