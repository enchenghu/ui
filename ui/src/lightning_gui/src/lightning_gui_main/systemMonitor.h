#pragma once
#include "qtCommon.hpp"

class SystemMonitor : public QTabWidget
{
    Q_OBJECT
public:
    SystemMonitor(QTabWidget* parent = 0);
    virtual ~SystemMonitor(){}

private:
    QWidget* eoSysWidget;
	QGridLayout* eoSysLayout;

    QWidget* motorSysWidget;
	QGridLayout* motorSysLayout;

    QWidget* hardSysWidget;
	QGridLayout* hardSysLayout;

    QWidget* softSysWidget;
	QGridLayout* softSysLayout;

    QWidget* iPeriSysWidget;
	QGridLayout* iPeriSysLayout;

	QGroupBox* edfaBox;
	QGridLayout* edfaBoxLayout;
	QGroupBox* basicBox;
	QGridLayout* basicBoxLayout;
    QGroupBox* stateBox;
	QGridLayout* stateBoxLayout;
	QGroupBox* warnBox;
	QGridLayout* warnBoxLayout;
    std::vector<QLineEdit*> edfaStateLinesV;
    std::vector<QLabel*> edfaWarnLEDV;
    std::vector<QLineEdit*> edfaDeviceInfoLinesV;
    void creatEoSysTab();
    void creatMotorSysTab();
    void creatHardSysTab();
    void creatSoftSysTab();
    void creatIPeriTab();

};