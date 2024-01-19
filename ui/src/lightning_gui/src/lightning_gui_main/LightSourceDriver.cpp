#include "LightSourceDriver.h"
LightSourceDriver::LightSourceDriver(QWidget* parent):BaseNode(), \
connectionState(false)
{
    initNode();
    loadSettings();
    initComponent();
    creatUI();
    creatConnection();
}

void LightSourceDriver::startXXTask()
{
    startTask(TASK0);
    startTask(TASK1);
}

void LightSourceDriver::saveControl()
{

}
void LightSourceDriver::initTaskQueue()
{
    std::vector<MsgPtr_> queueBuffTask0;
    for(int i = 0; i < 4; i++)
    {
        queueBuffTask0.emplace_back(std::make_shared<linearityData>());
    }
    BaseNode::initTaskQueue(queueBuffTask0, TASK0);
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
            LOGD("send data is %d", pdata->num);
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
            LOGI("recv data is %d", pdata->num);
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

    linearityCtrlBox = new QGroupBox(tr("控制:")); 
    linearityCtrlBoxLayout = new QGridLayout ;

    correctionCtrlBox = new QGroupBox(tr("控制:")); 
    correctionCtrlBoxLayout = new QGridLayout ;

    correctionBoxLayout = new QGridLayout ;
    linearityChart = new ChartLighting(this, Linearity);
    correctionChart = new ChartLighting(this, Correction);
	ip_edit =  new QLineEdit();
	ip_edit->setText(device_ip_);
	//ip_edit->setFixedSize(70,25);
	port_edit =  new QLineEdit();
	port_edit->setText(device_ip_port_);
	//port_edit->setFixedSize(70,25);
	connect_button = new QPushButton("设备连接", this);
	setButtonStyle(connect_button);
	save_button = new QPushButton("保存", this);
	setButtonStyle(save_button);

	reset_button_l = new QPushButton("reset", this);
	setButtonStyle(reset_button_l);

	reset_button_c = new QPushButton("reset", this);
	setButtonStyle(reset_button_c);

	single_button_l = new QPushButton("single", this);
	setButtonStyle(single_button_l);

	single_button_c = new QPushButton("single", this);
	setButtonStyle(single_button_c);

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
    linearityCtrlBoxLayout->addWidget(reset_button_l, 0, 0);
    linearityCtrlBoxLayout->addWidget(single_button_l, 1, 0);
    linearityCtrlBox->setLayout(linearityCtrlBoxLayout);
    linearityBoxLayout->addWidget(linearityCtrlBox, 0 , 1);
    linearityBoxLayout->setColumnStretch(0, 9);
    linearityBoxLayout->setColumnStretch(1, 1);
    linearityBox->setLayout(linearityBoxLayout);

    correctionBoxLayout->addWidget(correctionChart->setChart(0, 100, -200, 200), 0 , 0);
    correctionCtrlBoxLayout->addWidget(reset_button_c, 0, 0);
    correctionCtrlBoxLayout->addWidget(single_button_c, 1, 0);
    correctionCtrlBox->setLayout(correctionCtrlBoxLayout);
    correctionBoxLayout->addWidget(correctionCtrlBox, 0 , 1);
    correctionBoxLayout->setColumnStretch(0, 9);
    correctionBoxLayout->setColumnStretch(1, 1);
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
	connect(connect_button, SIGNAL(clicked()), this, SLOT(startConnect( void ))); 
	connect(save_button, SIGNAL(clicked()), this, SLOT(saveControl( void )));    
}


int LightSourceDriver::deviceConnect()
{
	int err;
	int one = 1;

	std::string device_ip = ip_edit->text().toStdString();
	int device_ip_ctrl_port = port_edit->text().toInt();
	LOGI("device_ip is %s, device_ctrl_port is %d", device_ip.c_str(), device_ip_ctrl_port);

	struct sockaddr_in ctrl_serv_addr;
	struct sockaddr_in data_serv_addr;
	struct sockaddr_in logs_serv_addr;

	if ((socket_id=socket(AF_INET, SOCK_STREAM, 0))==-1){
		LOGE("ERROR: Could not create socket!");
		return -1;
	}

	int nRecvBuf=320 * 1024;
	setsockopt(socket_id, SOL_SOCKET,SO_RCVBUF,(const char*)&nRecvBuf,sizeof(int));

	int nSendBuf=320 * 1024;
	setsockopt(socket_id, SOL_SOCKET, SO_SNDBUF,(const char*)&nSendBuf,sizeof(int));
	setsockopt(socket_id, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));

	struct timeval timeout_send = {2, 0};
	setsockopt(socket_id, SOL_SOCKET, SO_SNDTIMEO, &timeout_send, sizeof(timeout_send)); //send timeout
	struct timeval timeout_recv = {2, 0};
	setsockopt(socket_id, SOL_SOCKET, SO_RCVTIMEO, &timeout_recv, sizeof(timeout_recv)); //recv timeout

	memset(&ctrl_serv_addr, 0, sizeof(ctrl_serv_addr));
	ctrl_serv_addr.sin_family = AF_INET;
	ctrl_serv_addr.sin_addr.s_addr = inet_addr(device_ip.c_str());
	ctrl_serv_addr.sin_port = htons(device_ip_ctrl_port);

	if(::connect(socket_id, (struct sockaddr*)&ctrl_serv_addr, sizeof(ctrl_serv_addr))<0)
	{
		LOGE("Failed to connect to device_ip %s", device_ip.c_str());
		return -2; 
	}
	//fcntl(ctrl_sock, F_SETFL, O_NONBLOCK); /* Set the socket to non blocking mode */
	return 0;    
}

void LightSourceDriver::startConnect()
{
	if(connectionState == false){
		if(deviceConnect()){
            QMessageBox::warning(0, "提示", "设备网络未连接，请检查网络！", QMessageBox::Ok | QMessageBox::Default, 0);
		}else {
			connect_button->setStyleSheet("QPushButton{background-color:rgba(127, 255, 0, 100);}");
			connect_button->setText("设备断开");
			connectionState = true;
            setLED(netStateLED, C_GREEN);
/* 			commandMsg cmdMsg;
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
			} */
		}
	}else {
		connect_button->setStyleSheet("QPushButton{background-color:rgba(192, 192, 192, 100);}");
		connect_button->setText("设备连接");
		connectionState = false;
/* 		::close(ctrl_sock);
		::close(state_ctrl_sock);
		vx_task_delete(&bst_task[TASK_SYSTEM_DATA_RECV]); */
	}
}
void LightSourceDriver::loadSettings()
{
	QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
	device_ip_ = settings.value("device IP Addr","10.20.30.40").toString();
	device_ip_port_ = settings.value("device TCP Port","5000").toString();
}
void LightSourceDriver::saveSettings()
{
	QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
	settings.setValue("device IP Addr", ip_edit->text());
	settings.setValue("device TCP Port", port_edit->text());
}

LightSourceDriver::~LightSourceDriver()
{
    saveSettings();
}