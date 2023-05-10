#include "systemMonitor.h"


SystemMonitor::SystemMonitor(QTabWidget* parent)
{
    creatEoSysTab();
    creatMotorSysTab();
    creatHardSysTab();
    creatIPeriTab();
    creatSoftSysTab();

}

void SystemMonitor::creatMotorSysTab()
{
	motorSysWidget = new QWidget();
	motorSysLayout = new QGridLayout();
	motorSysWidget->setLayout(motorSysLayout);
	this->addTab(motorSysWidget,  "电机系统");
}

void SystemMonitor::creatHardSysTab()
{
	hardSysWidget = new QWidget();
	hardSysLayout = new QGridLayout();
	hardSysWidget->setLayout(hardSysLayout);
	this->addTab(hardSysWidget,  "硬件系统");
}

void SystemMonitor::creatSoftSysTab()
{
	softSysWidget = new QWidget();
	softSysLayout = new QGridLayout();
	softSysWidget->setLayout(softSysLayout);
	this->addTab(softSysWidget,  "软件系统");
}

void SystemMonitor::creatIPeriTab()
{
	iPeriSysWidget = new QWidget();
	iPeriSysLayout = new QGridLayout();
	iPeriSysWidget->setLayout(iPeriSysLayout);
	this->addTab(iPeriSysWidget,  "接口外设");
}

void SystemMonitor::creatEoSysTab()
{
	eoSysWidget = new QWidget();
	eoSysLayout = new QGridLayout() ;
	edfaBox = new QGroupBox(tr("EDFA:"));
	edfaBoxLayout = new QGridLayout ;
	basicBox = new QGroupBox(tr("基本信息:"));
	basicBoxLayout = new QGridLayout ;
	for(int i = 0; i < edfaDevName.size(); i++){
		edfaDeviceInfoLinesV.push_back(new QLineEdit);
		setReadOnlyLineEdit(edfaDeviceInfoLinesV[i]);
		QLabel* temp = new QLabel(edfaDevName[i].c_str());
		basicBoxLayout->addWidget(temp, i, 0, Qt::AlignRight | Qt::AlignTop);
		basicBoxLayout->addWidget(edfaDeviceInfoLinesV[i], i, 1, Qt::AlignLeft | Qt::AlignTop);
	}
	basicBox->setLayout(basicBoxLayout);
	edfaBoxLayout->addWidget(basicBox, 0, 0);
	stateBox = new QGroupBox(tr("状态信息:"));
	stateBoxLayout = new QGridLayout ;
	for(int i = 0; i < edfaStateName.size(); i++){
		edfaStateLinesV.push_back(new QLineEdit);
		setReadOnlyLineEdit(edfaStateLinesV[i]);
		QLabel* temp = new QLabel(edfaStateName[i].c_str());
		stateBoxLayout->addWidget(temp, i, 0, Qt::AlignRight | Qt::AlignTop);
		stateBoxLayout->addWidget(edfaStateLinesV[i], i, 1, Qt::AlignLeft | Qt::AlignTop);
	}
	stateBox->setLayout(stateBoxLayout);
	edfaBoxLayout->addWidget(stateBox, 1, 0);
	warnBox = new QGroupBox(tr("报警信息:"));
	warnBoxLayout = new QGridLayout ;
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
	eoSysLayout->addWidget(edfaBox, 0, 0, Qt::AlignLeft);
	eoSysWidget->setLayout(eoSysLayout);
    this->addTab(eoSysWidget,  "光电系统");
}


