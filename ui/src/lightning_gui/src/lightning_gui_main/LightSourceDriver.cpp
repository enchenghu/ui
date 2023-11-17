#include "LightSourceDriver.h"

LightSourceDriver::LightSourceDriver(QWidget* parent)
{
    creatUI();
    creatConnection();
}

void LightSourceDriver::creatConnection()
{
    
}
void LightSourceDriver::creatUI()
{
    mainLayout = new QGridLayout();
    correctionBox = new QGroupBox(tr("矫正波形:")); 
    stateBox = new QGroupBox(tr("状态信息:")); 
    stateBoxLayout = new QGridLayout ;
    ctrlBox = new QGroupBox(tr("控制:")); 
    ctrlBoxLayout = new QGridLayout; 

    chartLayout = new QVBoxLayout;
    configLayout = new QVBoxLayout;

    linearityBox = new QGroupBox(tr("线性度:")); 
    linearityBoxLayout = new QGridLayout ;
    correctionBoxLayout = new QGridLayout ;

    linearityChart = new ChartLighting(this, Linearity);
    correctionChart = new ChartLighting(this, Correction);

    linearityBoxLayout->addWidget(linearityChart->setChart(0, 1000, 0, 1), 0 , 0);
    linearityBox->setLayout(linearityBoxLayout);

    correctionBoxLayout->addWidget(correctionChart->setChart(0, 100, -200, 200), 0 , 0);
    correctionBox->setLayout(correctionBoxLayout);

    stateBox->setLayout(stateBoxLayout);
    ctrlBox->setLayout(ctrlBoxLayout);

    chartLayout->addWidget(linearityBox);
    chartLayout->addWidget(correctionBox);
    configLayout->addWidget(stateBox);
    configLayout->addWidget(ctrlBox);

	mainLayout->setColumnStretch(0, 8);
	mainLayout->setColumnStretch(1, 2);
    mainLayout->addLayout(chartLayout, 0, 0);
    mainLayout->addLayout(configLayout, 0, 1);
    this->setLayout(mainLayout);
}