#pragma once
#include "qtCommon.hpp"
#include "flidar_stat.h"
#include "chartLightning.h"
#include "baseNode.h"

#define TASK_NUMS 1
typedef enum{
	TASK0,
	TASK1,
	TASK2
} LightSourceDriver_TASK_ID;

typedef struct 
{
	std::vector<int> data;
	int num;
} linearityData;


class LightSourceDriver : public QWidget, public BaseNode
{
    Q_OBJECT
public:
    LightSourceDriver(QWidget* parent = 0);
    virtual ~LightSourceDriver();

private:
	QGridLayout* mainLayout;
	QGroupBox* linearityBox;
	QGridLayout* linearityBoxLayout;
	QGroupBox* linearityCtrlBox;
	QGridLayout* linearityCtrlBoxLayout;
	QGroupBox* correctionCtrlBox;
	QGridLayout* correctionCtrlBoxLayout;
	QGroupBox* correctionBox;
	QGridLayout* correctionBoxLayout;
    QGroupBox* stateBox;
	QGridLayout* stateBoxLayout;
	QGroupBox* ctrlBox;
	QGridLayout* ctrlBoxLayout;
	QVBoxLayout* chartLayout;    
	QVBoxLayout* configLayout;    
    ChartLighting* linearityChart;
    ChartLighting* correctionChart;
	QLineEdit* ip_edit;
	QLineEdit* port_edit;
	QPushButton* connect_button;
	QPushButton* save_button;
	QPushButton* reset_button_l;
	QPushButton* reset_button_c;
	QPushButton* single_button_l;
	QPushButton* single_button_c;
  	QLabel* netStateLED;
	QString device_ip_port_;
	QString device_ip_;
private:
	int socket_id;
	bool connectionState;

 private Q_SLOTS:
	void startConnect(void);
	void saveControl(void);
	void startXXTask(void);

private:
    void creatUI();
    void creatCtrlUI();
    void creatStateUI();
    void creatConnection();
	void initComponent();
	int deviceConnect();
	void handleLoopTask0() override;
	void handleLoopTask1() override;
	void initTaskQueue() override;
	void loadSettings();
	void saveSettings();
};