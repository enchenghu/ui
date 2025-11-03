#include "qlabelspinslidercheckboxgroup.h"
#include "Common/Others/qusbcommunication.h"
#include "Common/UI/qtabwndcomm.h"
#include "ui_qlabelspinslidercheckboxgroup.h"
#include <QDateTime>

QLabelSpinSliderCheckBoxGroup::QLabelSpinSliderCheckBoxGroup(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::QLabelSpinSliderCheckBoxGroup)
{
    ui->setupUi(this);
    spinSliderGroup  = new QSpinSliderGroup();
    bitCheckBoxGroup = new QBitCheckBoxGroup();
    labelEditGroup   = new QLabelEditGroup();
    readButton       = new QPushButton();
    tableListGroup   = new QTableListGroup();
    labelEditGroup2  = new QLabelEditGroup;

    init();
    initConnect();
}

QLabelSpinSliderCheckBoxGroup::~QLabelSpinSliderCheckBoxGroup()
{
    delete ui;
}

void QLabelSpinSliderCheckBoxGroup::init()
{
    ui->gridLayout->addWidget(labelEditGroup.get(),0,0);    
    ui->gridLayout->addWidget(spinSliderGroup.get(),0,1);
    ui->gridLayout->addWidget(bitCheckBoxGroup.get(),1,0,1,2);
    ui->gridLayout->addWidget(readButton.get(),2,1);
    readButton->setText("Read");
    labelEditGroup->setEditWidth(60);
    labelEditGroup->setlabelWidth(150);
    labelEditGroup2->setEditWidth(60);
    labelEditGroup2->setlabelWidth(40);
    readButton->adjustSize();
    ui->gridLayout->setAlignment(readButton.get(),Qt::AlignRight);
    spinSliderGroup->setValueRange(0,255);
    ui->gridLayout->setOriginCorner(Qt::TopLeftCorner);
    m_type=FloatIntType::Type_1_0x100;
}

void QLabelSpinSliderCheckBoxGroup::initConnect()
{
    connect(spinSliderGroup.get(), SIGNAL(signalValueChange(uint32_t, bool)), this, SLOT(recvValueChanged(uint32_t, bool)));
    connect(bitCheckBoxGroup.get(), SIGNAL(signalValueChange(uint32_t)), this, SLOT(recvValueChanged(uint32_t)));
    connect(labelEditGroup.get(), SIGNAL(signalValueChange(uint32_t)), this, SLOT(recvValueChanged(uint32_t)));
    connect(labelEditGroup2.get(), &QLabelEditGroup::floatValueChange, this,
        &QLabelSpinSliderCheckBoxGroup::recFloatValue);
    connect(readButton.get(), &QPushButton::clicked, this, &QLabelSpinSliderCheckBoxGroup::on_readButton_clicked);
}

void QLabelSpinSliderCheckBoxGroup::setValue(uint32_t value)
{
    float fccm = 0.0;
    if(m_type==FloatIntType::Type_1_0x200){
        if (value & 0x400) {
            fccm = (float)((int16_t)(value | 0xf800)) / 512;
        } else {
            fccm = (float)value / 512;
        }
        if (fccm > 0) {
            fccm = fccm + 0.0005;
        }
        if (fccm < 0) {
            fccm = fccm - 0.0005;
        }
    }else{
        if (value & 0x400) {
            fccm = (float)((int16_t)(value | 0xf800)) / 256;
        } else {
            fccm = (float)value / 256;
        }
        if (fccm > 0) {
            fccm = fccm + 0.0005;
        }
        if (fccm < 0) {
            fccm = fccm - 0.0005;
        }
    }
    // qDebug() << 1;
    labelEditGroup2->setValue(QString::number(fccm,'f',2));
    // qDebug() << 2;
    spinSliderGroup->setValue(value);
    // qDebug() << 3;
    labelEditGroup->setValue(QString("0x%1").arg(QString::number(value,16).toUpper().rightJustified(VALUE_HEX_INFO,'0')));
    // qDebug() << 4;
    bitCheckBoxGroup->setValue(value);
}

int QLabelSpinSliderCheckBoxGroup::getValue()
{
    return spinSliderGroup->getValue();
}


void QLabelSpinSliderCheckBoxGroup::updateValue()
{
    uint8_t value = 0;
    QUsbCommunication::getInstance().readBurst(currentRegister.address, &value, ADDRESS_LENGTH, VALUE_LENGTH);
    emit signalValueChange(currentRegister.address,value);//更新表格显示
    currentValue=value;
    setValue(value);
}

void QLabelSpinSliderCheckBoxGroup::setReadBtnVisible(bool isVisible)
{
    readButton->setVisible(isVisible);
}

void QLabelSpinSliderCheckBoxGroup::setTableListVisible(bool isVisible)
{
    tableListGroup->setVisible(isVisible);
}

void QLabelSpinSliderCheckBoxGroup::setTableHeader(QStringList strList)
{
    tableListGroup->setHeader(strList);
    tableListGroup->horizontalHeader()->setSectionResizeMode(0,QHeaderView::ResizeMode::Interactive);
    tableListGroup->horizontalHeader()->setSectionResizeMode(1,QHeaderView::ResizeMode::ResizeToContents);
    tableListGroup->horizontalHeader()->setSectionResizeMode(2,QHeaderView::ResizeMode::Stretch);
    tableListGroup->horizontalHeader()->setSectionResizeMode(3,QHeaderView::ResizeMode::Interactive);
    tableListGroup->horizontalHeader()->setSectionResizeMode(4,QHeaderView::ResizeMode::Interactive);
    tableListGroup->horizontalHeader()->setSectionResizeMode(5,QHeaderView::ResizeMode::Interactive);
    tableListGroup->horizontalHeader()->setSectionResizeMode(6,QHeaderView::ResizeMode::Stretch);
    tableListGroup->setSelectionBehavior(QAbstractItemView::SelectRows);
}

void QLabelSpinSliderCheckBoxGroup::appendTableRow(int row, int column, QString str, Qt::Alignment alignment)
{
    tableListGroup->appendRow(row,column,str,alignment);
}

int QLabelSpinSliderCheckBoxGroup::currentTableRow()
{
    return tableListGroup->rowCount();
}

void QLabelSpinSliderCheckBoxGroup::changeGroupLayout(int tableRow, int tableCol, int spinSliderRow, int spinSliderCol, int bitBoxRow, int bitBoxCol, int labelEditRow, int labelEditCol)
{
    ui->gridLayout->removeWidget(spinSliderGroup.get());
    ui->gridLayout->removeWidget(bitCheckBoxGroup.get());
    ui->gridLayout->removeWidget(labelEditGroup.get());
    ui->gridLayout->removeWidget(tableListGroup.get());

    ui->gridLayout->addWidget(tableListGroup.get(),tableRow,tableCol,1,4);
    tableListGroup->setMinimumHeight(50);
    tableListGroup->setMaximumHeight(150);

    labelEditGroup2->setLabelValue("Real");
    labelEditGroup2->setVisible(false);
    ui->gridLayout->addWidget(labelEditGroup2.get(),spinSliderRow,0);
    ui->gridLayout->addWidget(spinSliderGroup.get(),spinSliderRow,spinSliderCol+1);
    ui->gridLayout->addWidget(labelEditGroup.get(),labelEditRow,labelEditCol+1);
    ui->gridLayout->addWidget(bitCheckBoxGroup.get(),bitBoxRow,bitBoxCol+1);
    bitCheckBoxGroup->setMinimumWidth(600);
    bitCheckBoxGroup->setMinimumHeight(100);
    labelEditGroup->setLabelVisible(false);
}

void QLabelSpinSliderCheckBoxGroup::clearTableRows()
{
    tableListGroup->clearRows();
}

void QLabelSpinSliderCheckBoxGroup::setCurrentRegisterPair(std::pair<Register, Register> registerPair, int cValue)
{
    currentValue=cValue;
    currentRegisterPair=registerPair;
    labelEditGroup->setLabelValue("");
    setValue(cValue);
}

void QLabelSpinSliderCheckBoxGroup::setCurrentRange(uint32_t minVal, uint32_t maxVal)
{
    if(maxVal==1)
        bitCheckBoxGroup->setAllEnable(false);
    else
        bitCheckBoxGroup->setAllEnable(true);
    spinSliderGroup->setValueRange(minVal,maxVal);
    if(maxVal>255&&maxVal<=65535)
        bitCheckBoxGroup->setBits(16);
    if(maxVal<=255)
        bitCheckBoxGroup->setBits(8);
    if(maxVal>65535)
        bitCheckBoxGroup->setBits(32);
}

void QLabelSpinSliderCheckBoxGroup::setcurrentValue(uint32_t value)
{
    float fccm = 0.0;
    if(m_type==FloatIntType::Type_1_0x200){
        if (value & 0x400) {
            fccm = (float)((int16_t)(value | 0xf800)) / 512;
        } else {
            fccm = (float)value / 512;
        }
        if (fccm > 0) {
            fccm = fccm + 0.0005;
        }
        if (fccm < 0) {
            fccm = fccm - 0.0005;
        }
    }else{
        if (value & 0x400) {
            fccm = (float)((int16_t)(value | 0xf800)) / 256;
        } else {
            fccm = (float)value / 256;
        }
        if (fccm > 0) {
            fccm = fccm + 0.0005;
        }
        if (fccm < 0) {
            fccm = fccm - 0.0005;
        }
    }

    labelEditGroup2->setValue(QString::number(fccm,'f',2));
    currentValue=value;
    spinSliderGroup->setValue(value);
    labelEditGroup->setValue(QString("0x%1").arg(QString::number(value,16).toUpper().rightJustified(VALUE_HEX_INFO,'0')));
    bitCheckBoxGroup->setValue(value);
}

std::vector<CombinedRegister> QLabelSpinSliderCheckBoxGroup::getCombinedRegister()
{
    return combinedRegister;
}

void QLabelSpinSliderCheckBoxGroup::setCombinedRegister(std::vector<CombinedRegister> combineVector)
{
    for(auto c_reg:combineVector){
        std::vector<RegisterBitInfo> regBitInfo = DatabaseOrm::getRegisterBitInfoByCombinedRegisterId(c_reg.combinedId);
        if(regBitInfo.size()==1)
            bitCheckBoxGroup->setBits(8);
        else if(regBitInfo.size()==2)
            bitCheckBoxGroup->setBits(16);
        else if(regBitInfo.size()==4)
            bitCheckBoxGroup->setBits(32);

        bitCheckBoxGroup->setAllEnable(false);
        for(int i=0;i<regBitInfo.size();i++)
            bitCheckBoxGroup->setEnableCheckBox(regBitInfo.at(i).combinedBitMask);//计算有效bitmask
        spinSliderGroup->setValueRange(0,bitCheckBoxGroup->getCurrentEnableRange());//设置slider范围
        uint32_t value = 0;
        readCombinedRegisterValue(c_reg, value);
        setcurrentValue(value);//设置当前显示值
    }
    combinedRegister=combineVector;//赋值std::vector<CombinedRegister>
}

void QLabelSpinSliderCheckBoxGroup::setCombinedRegister2(std::vector<CombinedRegister> combineVector) 
{ 
    for(auto c_reg:combineVector){
        std::vector<RegisterBitInfo> regBitInfo = DatabaseOrm::getRegisterBitInfoByCombinedRegisterId(c_reg.combinedId);
        if(regBitInfo.size()==1)
            bitCheckBoxGroup->setBits(8);
        else if(regBitInfo.size()==2)
            bitCheckBoxGroup->setBits(16);
        else if(regBitInfo.size()==4)
            bitCheckBoxGroup->setBits(32);

        bitCheckBoxGroup->setAllEnable(false);
        for(int i=0;i<regBitInfo.size();i++)
            bitCheckBoxGroup->setEnableCheckBox(regBitInfo.at(i).combinedBitMask);//计算有效bitmask
        spinSliderGroup->setValueRange(0,bitCheckBoxGroup->getCurrentEnableRange());//设置slider范围
        uint32_t value = 0;
        readCombinedRegisterValue(c_reg, value);
        setcurrentValue(value);//设置当前显示值
    }
}

void QLabelSpinSliderCheckBoxGroup::clearRegister()
{
    combinedRegister.clear();
    currentRegisterPair=std::make_pair(Register(),Register());
    // recvValueChanged(QString::number(0,16));
}

void QLabelSpinSliderCheckBoxGroup::setRealEdit(bool flag)
{
    labelEditGroup2->setVisible(flag);
}

void QLabelSpinSliderCheckBoxGroup::recvValueChanged(uint32_t value)
{
    //只调整寄存器value
    emit ctrlTriggered(value);
    if(currentValue!=value){
        currentValue=value;
        setValue(value);
        if (combinedRegister.empty()){
            uint8_t valueu8;
            valueu8=(uint8_t)(value);
            int ret = QUsbCommunication::getInstance().writeBurst(currentRegister.address, &valueu8, ADDRESS_LENGTH,
                VALUE_LENGTH);
            if(ret==0){//读写成功
                emit updateRegisterModule(currentRegister.address);//更新RegisterModule
                emit signalValueChange(currentRegister.address,value);//更新表格显示
                QString str;
                str=QDateTime::currentDateTime().toString("[yyyy/MM/dd hh:mm:ss]")+"  ["+QString::fromStdString(currentModule.moduleName)+"]"+
                    "  "+QString::fromStdString(currentRegister.displayName)+";  Addr: "+QString("0x%1").arg(QString::number(currentRegister.address,16).toUpper())
                    +";  Val: "+QString("0x%1").arg(QString::number(this->getValue(),16).rightJustified(VALUE_HEX_INFO,'0').toUpper())+";   Write OK";
                emit signalValueChange(str);//更新log
            }else{
                QString str;
                str=QDateTime::currentDateTime().toString("[yyyy/MM/dd hh:mm:ss]")+"  ["+QString::fromStdString(currentModule.moduleName)+"]"+
                    "  "+QString::fromStdString(currentRegister.displayName)+";  Addr: "+QString("0x%1").arg(QString::number(currentRegister.address,16).toUpper())
                    +";  Val: "+QString("0x%1").arg(QString::number(this->getValue(),16).rightJustified(ADDRESS_HEX_INFO,'0').toUpper())+";   Write False";
                emit signalValueChange(str);
            }
        }
        //读写高低位寄存器pair
        if (!combinedRegister.empty()){
            for (int i = 0; i < combinedRegister.size(); i++)
                writeCombinedRegisterValue(combinedRegister.at(i), value);
            emit changeValueByHLBits(value);
        }
    }
}

void QLabelSpinSliderCheckBoxGroup::recvValueChanged(uint32_t value, bool flag)
{
    if(flag){
        recvValueChanged(value);
    }else{
        setValue(value);
    }
}

void QLabelSpinSliderCheckBoxGroup::recFloatValue(float val)
{
    uint32_t   value    = 0;
    float fTempccm = val;
    if (fTempccm > 0) {
        fTempccm += (float)0.0005;
    }
    if (fTempccm < 0) {
        fTempccm -= (float)0.0005;
    }
    uint16_t uccmDataH=0;
    uint16_t uccmDataL=0;
    if(m_type==FloatIntType::Type_1_0x200){
        if (fTempccm < 0) {
            uccmDataH = (((int16_t)(fTempccm * 512)) & 0xff00) >> 8; // 处理负值
            uccmDataL = ((int16_t)(fTempccm * 512)) & 0xff;
        } else {
            uccmDataH = (((int16_t)(fTempccm * 512) & 0x7ff) & 0xff00) >> 8; // 处理正值
            uccmDataL = ((int16_t)(fTempccm * 512) & 0x7ff) & 0xff;
        }
    }else{
        if(fTempccm<0){
            uccmDataH = (((int16_t)(fTempccm * 256)) & 0xff00) >> 8;
            uccmDataL = ((int16_t)(fTempccm * 256)) & 0xff;
        }else{
            uccmDataH = (((int16_t)(fTempccm * 256) & 0x7ff) & 0xff00) >> 8;
            uccmDataL = ((int16_t)(fTempccm * 256) & 0x7ff) & 0xff;
        }
    }
    QString lowBit;
    QString highBit;
    QString valuestr;
    lowBit   = lowBit.setNum((int)uccmDataL, 2).rightJustified(8, '0');
    highBit  = highBit.setNum((int)uccmDataH, 2).rightJustified(8, '0');
    valuestr = highBit + lowBit;
    value      = valuestr.toUInt(nullptr, 2);
    recvValueChanged(value);
}
float QLabelSpinSliderCheckBoxGroup::getCurrentRealVal()
{
    float val=0.0;
    val=labelEditGroup2->value().toFloat();
    return val;
}

void QLabelSpinSliderCheckBoxGroup::setSpinSliderEnable(bool flag)
{
    spinSliderGroup->setEnabled(flag);
    labelEditGroup->setEnabled(flag);
}

void QLabelSpinSliderCheckBoxGroup::setBitCheckBoxEnable(bool flag)
{
    bitCheckBoxGroup->setEnabled(flag);
}

void QLabelSpinSliderCheckBoxGroup::setFloatIntType(FloatIntType type)
{
    m_type=type;
}
void QLabelSpinSliderCheckBoxGroup::recvValueChanged(QString addr)
{
    //表格上获取的address为QString格式的16进制,展示寄存器name及value
    currentRegister=DatabaseOrm::getRegisterByAddress(addr.toInt(nullptr,16));
    currentModule =DatabaseOrm::getModuleByRegisterAddress(addr.toInt(nullptr,16));
    labelEditGroup->setLabelValue(QString::fromStdString(currentRegister.displayName));
    updateValue();
}

void QLabelSpinSliderCheckBoxGroup::on_readButton_clicked()
{
    uint8_t value = 0;
    int ret = QUsbCommunication::getInstance().readBurst(currentRegister.address, &value, ADDRESS_LENGTH, VALUE_LENGTH);
    setValue(value);
    emit signalValueChange(currentRegister.address,value);//更新表格显示
    if(ret!=0){
        //读写失败
        QString str;
        str=QDateTime::currentDateTime().toString("[yyyy/MM/dd hh:mm:ss]")+"  ["+QString::fromStdString(currentModule.moduleName)+"]"+
              "  "+QString::fromStdString(currentRegister.displayName)+";  Addr: "+QString("0x%1").arg(QString::number(currentRegister.address,16).toUpper())
              +"; Read False";
        emit signalValueChange(str);//更新log
    }else{
        QString str;
        str=QDateTime::currentDateTime().toString("[yyyy/MM/dd hh:mm:ss]")+"  ["+QString::fromStdString(currentModule.moduleName)+"]"+
              "  "+QString::fromStdString(currentRegister.displayName)+";  Addr: "+QString("0x%1").arg(QString::number(currentRegister.address,16).toUpper())
              +";  Val: "+QString("0x%1").arg(QString::number(value,16).rightJustified(VALUE_HEX_INFO,'0').toUpper())+";   Read OK";
        emit signalValueChange(str);//更新log
    }
}

void QLabelSpinSliderCheckBoxGroup::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing,true);
    p.setRenderHints(QPainter::SmoothPixmapTransform,true);
    p.setPen(QPen(QColor("black"),0.5));
    p.drawRect(QRectF(1,5,width()-2,height()-11));
}
