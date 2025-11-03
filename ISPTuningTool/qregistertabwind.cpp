#include "qregistertabwind.h"
#include "qcommondef.h"
#include "qobjectdefs.h"
#include "ui_qregistertabwind.h"

qregistertabwind::qregistertabwind(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::qregistertabwind)
{
    ui->setupUi(this);
    registerTable=std::make_shared<QTableListGroup>();//模块列表控件
    propertyTable=std::make_shared<QTableListGroup>();//寄存器属性列表控件
    labelSpinSliderCheckBoxGroup=std::make_shared<QLabelSpinSliderCheckBoxGroup>();//Register set控件
    textEditGroup=std::make_shared<QTextEditGroup>();//Communication log控件
    labelComboBox=std::make_shared<QLabelComboBox>();//Module 下拉列表控件
    labelEditGroup=std::make_shared<QLabelEditGroup>();//Search 控件
    pushButtonGroup=std::make_shared<QPushButtonGroup>();//read,load,save,通用组合控件
    init();
}

qregistertabwind::~qregistertabwind()
{
    delete ui;
}

void qregistertabwind::init()
{
    initConnect();
    registerTable->setHeader(QStringList()<<"NAME"<<"ADDRESS"<<"DATA");
    propertyTable->setHeader(QStringList()<<"Bit"<<"NAME"<<"HEX"<<"DEC"<<"Description");
    //设置控件尺寸、样式等
    labelEditGroup->setLabelValue("Search");
    labelEditGroup->setEditable(true);
    registerTable->horizontalHeader()->setSectionResizeMode(0,QHeaderView::ResizeMode::Stretch);
    registerTable->horizontalHeader()->setSectionResizeMode(1,QHeaderView::ResizeMode::Stretch);
    registerTable->horizontalHeader()->setSectionResizeMode(2,QHeaderView::ResizeMode::Interactive);

    propertyTable->horizontalHeader()->setSectionResizeMode(0,QHeaderView::ResizeMode::Interactive);
    propertyTable->horizontalHeader()->setSectionResizeMode(1,QHeaderView::ResizeMode::ResizeToContents);
    propertyTable->horizontalHeader()->setSectionResizeMode(2,QHeaderView::ResizeMode::Interactive);
    propertyTable->horizontalHeader()->setSectionResizeMode(3,QHeaderView::ResizeMode::ResizeToContents);
    propertyTable->horizontalHeader()->setSectionResizeMode(4,QHeaderView::ResizeMode::Stretch);

    //设置控件布局
    ui->registerTableLayout->addWidget(registerTable.get());
    ui->registerPropertyLayout->addWidget(propertyTable.get());
    ui->registerSetLayout->addWidget(labelSpinSliderCheckBoxGroup.get());
    ui->logLayout->addWidget(textEditGroup.get());
    labelComboBox->setComboBoxWidth(200);
    labelEditGroup->setEditWidth(200);
    labelEditGroup->setlabelWidth(50);
    ui->moduleLayout->addWidget(labelComboBox.get());
    ui->searchLayout->addWidget(labelEditGroup.get());
    ui->buttonLayout->addWidget(pushButtonGroup.get());
    labelSpinSliderCheckBoxGroup->setCurrentRange(0,255);
    registerTable->setMinimumWidth(400);
    labelSpinSliderCheckBoxGroup->setMinimumWidth(400);
    ui->horizontalLayout_3->setStretchFactor(ui->registerSetLayout,3);
    ui->horizontalLayout_3->setStretchFactor(ui->logLayout,4);
    ui->horizontalLayout_2->setStretchFactor(ui->registerTableLayout,3);
    ui->horizontalLayout_2->setStretchFactor(ui->registerPropertyLayout,4);
}

void qregistertabwind::initConnect()
{
    connect(registerTable.get(),&QTableListGroup::itemClicked,propertyTable.get(),&QTableListGroup::receiveitemClicked);//关联模块表格和寄存器表格，控件之间通讯
    connect(labelComboBox.get(),SIGNAL(signalValueChange(QString)),this,SLOT(updateTable(QString)));//关联Module combobox控件
    connect(labelEditGroup.get(),SIGNAL(signalValueChange(uint32_t)),this,SLOT(updateTable(uint32_t)));//关联Search 搜索控件
    connect(registerTable.get(),SIGNAL(itemClicked(QString)),labelSpinSliderCheckBoxGroup.get(),SLOT(recvValueChanged(QString)));//关联列表和registerset控件
    connect(propertyTable.get(),SIGNAL(itemClicked(QString)),labelSpinSliderCheckBoxGroup.get(),SLOT(recvValueChanged(QString)));//关联列表和registerset控件

    connect(labelSpinSliderCheckBoxGroup.get(),SIGNAL(signalValueChange(int,int)),registerTable.get(),SLOT(recvValueChanged(int,int)));//(需在appendRow后调用recordValue());自动关联registerset和列表中对应寄存器的值更新
    connect(labelSpinSliderCheckBoxGroup.get(),SIGNAL(signalValueChange(int,int)),propertyTable.get(),SLOT(recvValueChangedbyBitMask(int,int)));//(需在appendRow后调用recordValue());自动关联registerset和列表中对应寄存器的值更新
    connect(labelSpinSliderCheckBoxGroup.get(),SIGNAL(updateRegisterModule(int)),this,SLOT(updateRegisterMoudlebyAddress(int)));
    connect(labelSpinSliderCheckBoxGroup.get(),SIGNAL(signalValueChange(QString)),textEditGroup.get(),SLOT(recvValueChanged(QString)));//关联registerset和communication log控件


    connect(pushButtonGroup.get(),SIGNAL(readSignal()),this,SLOT(readMoudleRegisterValue()));
    connect(pushButtonGroup.get(),SIGNAL(loadSignal()),this,SLOT(loadMoudleRegisterValue()));
    connect(pushButtonGroup.get(),SIGNAL(saveSignal()),this,SLOT(saveMoudleRegisterValue()));
}

void qregistertabwind::recordAllModules(std::vector<Module> modules)
{
    QStringList modulestrlist;
    for(int i=0;i<modules.size();++i){
        modulestrlist<<QString::fromStdString(modules.at(i).moduleName);
    }
    labelComboBox->setValue("Module",modulestrlist);
    updateTable(labelComboBox->getValue(-1));
}

int qregistertabwind::getValueByRegisterMoudle(int addr)
{
    for(int i = 0; i < registerMoudle.size(); i++) {
        if(registerMoudle[i].first.address==addr)
            return registerMoudle[i].second;
    }
    return 0;
}

void qregistertabwind::setCheckState(bool isChecked)
{
    isCheckedFlag=isChecked;
}

void qregistertabwind::updateTable(QString str)
{
    //此处应请求数据库接口，展示模块相关的寄存器
    std::vector<Register> registerVector=DatabaseOrm::getRegistersByModuleName(str.toStdString());
    registerMoudle.clear();
    for(int i = 0; i < registerVector.size(); ++i)
        registerMoudle.push_back(std::make_pair(registerVector[i],0));
    std::vector<std::pair<Register, uint32_t>> registers;
    if(isCheckedFlag){
            readMoudleRegisterValue();
    }else{
        updateTableRegisterValue();
    }
    //刷新控件显示
}

void qregistertabwind::updateTable(uint32_t addr)
{
    if(addr==0){
        updateTable(labelComboBox->getValue(-1));
        return;
    }
    Module _module=DatabaseOrm::getModuleByRegisterAddress(addr);
    if (QString::fromStdString(_module.moduleName)!= labelComboBox->getValue(-1))
        return;
    registerTable->clearRows();
    Register _register=DatabaseOrm::getRegisterByAddress(addr);
    registerTable->appendRow(0,0,QString::fromStdString(_register.displayName),Qt::AlignLeft|Qt::AlignVCenter);
    registerTable->appendRow(0,1,QString("0x%1").arg(QString::number(_register.address,16).toUpper().rightJustified(ADDRESS_HEX_INFO,'0')));
    uint8_t value = 0;
    QUsbCommunication::getInstance().readBurst(addr, &value, ADDRESS_LENGTH, VALUE_LENGTH);
    registerTable->appendRow(0,2,QString("0x%1").arg(QString::number(value,16).toUpper().rightJustified(VALUE_HEX_INFO,'0')));
    registerTable->recordValue(_register,2,16);
}

void qregistertabwind::readMoudleRegisterValue()
{
    uint32_t adress = 0;
    uint8_t value = 0;
    for(int i = 0; i < registerMoudle.size(); i++) {
        adress = registerMoudle[i].first.address;
        value = 0;
        QUsbCommunication::getInstance().readBurst(adress, &value, ADDRESS_LENGTH, VALUE_LENGTH);
        registerMoudle[i].second = value;
    }
    updateTableRegisterValue();
}

void qregistertabwind::loadMoudleRegisterValue()
{
    std::vector<std::pair<uint32_t,uint32_t>> regValue;
    //加载文件
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("Text Files (*.txt)"));
    if(!fileName.isEmpty()){
        QFile file(fileName);
        if(file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream txtIn(&file);
            QString line = "";
            QStringList list = {""};
            uint32_t address = 0;
            uint32_t value = 0;
            while(!txtIn.atEnd()) {
                line = txtIn.readLine();
                list = line.split(',');
                address = getQStringToUint(list[0]);
                value = getQStringToUint(list[1]);
                regValue.push_back(std::make_pair(address,value));
            }
            file.close();
        }
    }
    if(regValue.size() != 0)
        writeSensorRegisterValue(regValue);
    readMoudleRegisterValue();
}

void qregistertabwind::saveMoudleRegisterValue()
{
    QString fileName = iniPathName() + "Save";
    QDir dir(fileName);
    if(!dir.exists())
        dir.mkpath(fileName);
    fileName += "/Register_" + labelComboBox->getValue(-1) + "_" + getCurrentTime() + ".txt";

    QString fileSavePath = QFileDialog::getSaveFileName(this,
                                                        tr("Save File"),
                                                        fileName,
                                                        tr("Text Files (*.txt)"));
    if (!fileSavePath.isEmpty()) {
        QFile file(fileSavePath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream txtOutput(&file);
            QString line = "";
            for (int i = 0; i < registerMoudle.size(); i++) {
                line = QString("0x%1").arg(QString::number(registerMoudle[i].first.address, 16)
                                               .toUpper()
                                               .rightJustified(ADDRESS_HEX_INFO, '0'))
                       + ","
                       + QString("0x%1").arg(QString::number(registerMoudle[i].second, 16)
                                                 .toUpper()
                                                 .rightJustified(VALUE_HEX_INFO, '0'))
                       + "\n";
                txtOutput << line;
            }
            file.close();
        }
    }
}

void qregistertabwind::updateRegisterMoudlebyAddress(int addr)
{
}

void qregistertabwind::updateTableRegisterValue()
{
    registerTable->clearRows();
    for(int i = 0; i < registerMoudle.size(); i++) {
        registerTable->appendRow(i,0,QString::fromStdString(registerMoudle[i].first.displayName),Qt::AlignLeft|Qt::AlignVCenter);
        registerTable->appendRow(i,1,QString("0x%1").arg(QString::number(registerMoudle[i].first.address,16).toUpper().rightJustified(ADDRESS_HEX_INFO,'0')));
        registerTable->appendRow(i,2,QString("0x%1").arg(QString::number(registerMoudle[i].second,16).toUpper().rightJustified(VALUE_HEX_INFO,'0')));
        registerTable->recordValue(registerMoudle[i].first,2,16);
        propertyTable->setValue(registerMoudle[i].first.address,registerMoudle[i].second);
    }
    labelSpinSliderCheckBoxGroup->updateValue();
}

void qregistertabwind::writeSensorRegisterValue(std::vector<std::pair<uint32_t,uint32_t>> &regValue)
{
    uint8_t value=0;
    for(int i = 0; i < regValue.size(); ++i){
        uint32_t address=regValue.at(i).first;
        value=(uint8_t)(regValue.at(i).second);
        QUsbCommunication::getInstance().writeBurst(address, &value, ADDRESS_LENGTH, VALUE_LENGTH);
    }
}
