#include "systemMonitor.h"


SystemMonitor::SystemMonitor(QTabWidget* parent)
{
    creatEDFATab();
    creatMotorSysTab();
    creatHardSysTab();
    creatIPeriTab();
    creatSoftSysTab();

}

void SystemMonitor::setReadOnlyLineEdit(QLineEdit* line)
{
	line->setReadOnly(true);
	QPalette palette = line->palette();
	palette.setBrush(QPalette::Base,palette.brush(QPalette::Disabled, QPalette::Base));
	line->setPalette(palette);
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

void SystemMonitor::creatEDFATab()
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
		setLED(edfaWarnLEDV[i], 0);
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

void SystemMonitor::setLED(QLabel* label, int color)
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
