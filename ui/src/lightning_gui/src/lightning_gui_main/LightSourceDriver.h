#pragma once
#include "qtCommon.hpp"
#include "flidar_stat.h"
#include "chartLightning.h"
class LightSourceDriver : public QWidget
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
  	QLabel* netStateLED;

private:

    void creatUI();
    void creatCtrlUI();
    void creatStateUI();
    void creatConnection();
	void initComponent();


};