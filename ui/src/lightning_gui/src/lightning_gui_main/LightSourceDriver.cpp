#include "LightSourceDriver.h"
LightSourceDriver::LightSourceDriver(QWidget* parent):BaseNode(TASK_NUMS)
{
    std::vector<MsgPtr_> queueBuffTask0;
    for(int i = 0; i < 4; i++)
    {
        queueBuffTask0.emplace_back(std::make_shared<linearityData>());
    }
    initTaskQueue(queueBuffTask0, TASK0);
    initComponent();
    creatUI();
    creatConnection();
}

void LightSourceDriver::connectControl()
{
    startTask(TASK0);
    startTask(TASK1);
/*     usleep(500 * 1000);
    startTask(TASK1, Task1Func); */
}

void LightSourceDriver::saveControl()
{
}

void LightSourceDriver::handleLoopTask0(void)
{
    long index = 0;
    while (true)
    {
        MsgPtr_ msg = getFreeMsg(TASK0);
        if(msg){
            linearityData* pdata = (linearityData*)msg.get();
            pdata->num = index++;
            dispatchMsg(msg, TASK0);
        }else{
            break;
        }
        usleep(500 * 1000);
    }

}

void LightSourceDriver::handleLoopTask1()
{
    long index = 0;
    while (true)
    {
        MsgPtr_ msg = getDoneMsg(TASK0);
        if(msg){
            linearityData* pdata = (linearityData*)msg.get();
            printf("recv data is %d\n", pdata->num);
            releaseMsg(msg, TASK0);
        }else{
            break;
        }
    }

}

void LightSourceDriver::initComponent()
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
	ip_edit =  new QLineEdit();
	//ip_edit->setFixedSize(70,25);
	port_edit =  new QLineEdit();
	//port_edit->setFixedSize(70,25);
	connect_button = new QPushButton("&设备连接", this);
	setButtonStyle(connect_button);
	save_button = new QPushButton("&保存", this);
	setButtonStyle(save_button);
	netStateLED = new QLabel;
	setLED(netStateLED, C_RED);
}

void LightSourceDriver::creatCtrlUI()
{
	ctrlBoxLayout->addWidget(new QLabel( "设备IP地址:" ), 0, 0, Qt::AlignRight | Qt::AlignTop);
	ctrlBoxLayout->addWidget(ip_edit, 0, 1, Qt::AlignLeft | Qt::AlignTop);
	ctrlBoxLayout->addWidget(new QLabel( "端口号:" ), 1, 0, Qt::AlignRight | Qt::AlignTop);
	ctrlBoxLayout->addWidget(port_edit, 1, 1, Qt::AlignLeft | Qt::AlignTop);
	ctrlBoxLayout->addWidget(connect_button, 2, 0, Qt::AlignRight | Qt::AlignTop);
	ctrlBoxLayout->addWidget(save_button, 2, 1, Qt::AlignRight | Qt::AlignTop);
    for(int i = 0; i < 4; i++) {
        ctrlBoxLayout->setColumnStretch(i, 1);
        ctrlBoxLayout->setRowStretch(i, 1);
    }
}

void LightSourceDriver::creatStateUI()
{
	stateBoxLayout->addWidget(new QLabel("网络连接状态"), 0, 0, Qt::AlignRight | Qt::AlignVCenter);			
	stateBoxLayout->addWidget(netStateLED, 0, 1, Qt::AlignLeft | Qt::AlignVCenter);	
    for(int i = 0; i < 4; i++) {
        stateBoxLayout->setColumnStretch(i, 1);
        stateBoxLayout->setRowStretch(i, 1);
    }
}

void LightSourceDriver::creatUI()
{
    creatCtrlUI();
    creatStateUI();
    linearityBoxLayout->addWidget(linearityChart->setChart(0, 1000, 0, 1), 0 , 0);
    linearityBox->setLayout(linearityBoxLayout);
    correctionBoxLayout->addWidget(correctionChart->setChart(0, 100, -200, 200), 0 , 0);
    correctionBox->setLayout(correctionBoxLayout);
    stateBox->setLayout(stateBoxLayout);
    ctrlBox->setLayout(ctrlBoxLayout);

    chartLayout->addWidget(linearityBox);
    chartLayout->addWidget(correctionBox);
    configLayout->addWidget(ctrlBox);
    configLayout->addWidget(stateBox);

	mainLayout->setColumnStretch(0, 2);
	mainLayout->setColumnStretch(1, 8);
    mainLayout->addLayout(configLayout, 0, 0);
    mainLayout->addLayout(chartLayout, 0, 1);
    this->setLayout(mainLayout);
}

void LightSourceDriver::creatConnection()
{
	connect(connect_button, SIGNAL(clicked()), this, SLOT(connectControl( void ))); 
	connect(save_button, SIGNAL(clicked()), this, SLOT(saveControl( void )));    
}
