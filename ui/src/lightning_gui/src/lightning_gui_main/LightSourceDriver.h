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
    virtual ~LightSourceDriver(){}

private:
	QGridLayout* mainLayout;

	QGroupBox* linearityBox;
	QGridLayout* linearityBoxLayout;

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
  	QLabel* netStateLED;


 private Q_SLOTS:
	void connectControl(void);
	void saveControl(void);

private:

    void creatUI();
    void creatCtrlUI();
    void creatStateUI();
    void creatConnection();
	void initComponent();
	void handleLoopTask0() override;
	void handleLoopTask1() override;
};