#include "qtabwndcomm.h"
#include "Common/Others/qcameratype.h"
#include "Common/Others/qusbcommunication.h"
#include "Common/UI/quidatamiddleware.h"
#include "Common/Others/qconfigfiletrans.h"
#include "ui_qtabwndcomm.h"
#include <QSet>
#include <algorithm>
enum func
{
    drawTabWidget,
    drawDefault,
}drawFunc;

QTabWndComm::QTabWndComm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::QTabWndComm)
{
    ui->setupUi(this);
    buttonGroup                  = new QPushButtonGroup();
    contentWidget                = new QWidget();
    contentLayout                = new QGridLayout();
    registerSetWidget            = new QWidget();
    registerSetLayout            = new QGridLayout();
    titleLayout                  = new QHBoxLayout();
    labelSpinSliderCheckBoxGroup = new QLabelSpinSliderCheckBoxGroup();
    drawFunc=drawDefault;
    update();
    init();
    initConnect();
}

QTabWndComm::~QTabWndComm()
{
    delete ui;
}

void QTabWndComm::init()
{
    QHBoxLayout *buttonLayout = new QHBoxLayout(this);
    ui->gridLayout->addLayout(buttonLayout, 0, 0);
    QSpacerItem *horizontalSpacer = new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Expanding);
    buttonLayout->addLayout(titleLayout);
    buttonLayout->addItem(horizontalSpacer);
    buttonGroup->setFixedWidth(600);
    buttonLayout->addWidget(buttonGroup.get());
    contentWidget->setMinimumHeight(350);
    contentWidget->setLayout(contentLayout.get());
    contentLayout->setVerticalSpacing(10);
    contentWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->gridLayout->addWidget(contentWidget.get(), 1, 0);
    ui->gridLayout->setRowStretch(1, 1);
    registerSetWidget->setLayout(registerSetLayout.get());
    ui->gridLayout->addWidget(registerSetWidget.get(), 2, 0);
    contentWidget->setContentsMargins(20,1,20,1);
}

void QTabWndComm::initConnect()
{
    connect(buttonGroup.get(), &QPushButtonGroup::readSignal, this, &QTabWndComm::read);
    connect(buttonGroup.get(), &QPushButtonGroup::loadSignal, this, &QTabWndComm::load);
    connect(buttonGroup.get(), &QPushButtonGroup::saveSignal, this, &QTabWndComm::save);
    // 用以绑定tab页下方“寄存器调整模块”与寄存器“writeBurst写入功能”，适用高低bit位的寄存器组合。
    connect(labelSpinSliderCheckBoxGroup.get(), &QLabelSpinSliderCheckBoxGroup::changeValueByHLBits, this,
        &QTabWndComm::recValueByHighLowBits); // 根据高低位寄存器信息，将值拆分写入对应地址
}

void QTabWndComm::writeSensorRegisterValue(std::vector<std::pair<uint32_t, uint32_t>> &regValue)
{
    uint8_t value = 0;
    for (int i = 0; i < regValue.size(); ++i) {
        uint32_t address = regValue.at(i).first;
        value            = (uint8_t)(regValue.at(i).second);
        QUsbCommunication::getInstance().writeBurst(address, &value, ADDRESS_LENGTH, VALUE_LENGTH);
    }
}

void QTabWndComm::reConnectCamera()
{
    emit reConnectRead();
}

void QTabWndComm::recordCombineRegbyUIifo(QString moudle_name, QString uiname, std::shared_ptr<int> cameraId)
{
    std::vector<UiInfo> uiInfo;
    uiInfo= DatabaseOrm::getUiInfoByModuleNameAndControlName(moduleName,uiname.toStdString());
    std::vector<CombinedRegister> combinevector;
    for (auto i : uiInfo) {
        CombinedRegister combineReg
            = DatabaseOrm::getCombinedRegisterByModuleNameAndCameraIdAndCombinedName(
                moudle_name.toStdString(), cameraId, *(i.combinedName));
        combinevector.push_back(combineReg);
    }
    combinedRegisterMap.insert(uiname,combinevector);
}

void QTabWndComm::setRegisterTypeMap(std::vector<CombinedRegister> regVector, std::vector<CombinedRegister> regVectorRgb, std::vector<CombinedRegister> regVectorIr)
{
    if (regVector.empty())
        return;
    for(int i = 0; i < regVector.size(); i++)
    {
        std::vector<RegisterBitInfo> regBitInfo = DatabaseOrm::getRegisterBitInfoByCombinedRegisterId(regVector[i].combinedId);
        std::vector<RegisterBitInfo> regBitInfoRgb = DatabaseOrm::getRegisterBitInfoByCombinedRegisterId(regVectorRgb[i].combinedId);
        std::vector<RegisterBitInfo> regBitInfoIr = DatabaseOrm::getRegisterBitInfoByCombinedRegisterId(regVectorIr[i].combinedId);
        for(int j = 0; j < regBitInfo.size(); j++)
        {
            registerTypeMap.insert(regBitInfo[j].registerAddr, {regBitInfoRgb[j].registerAddr, regBitInfoIr[j].registerAddr});
            // registerSubPageMap.insert(regBitInfo[j].registerAddr, subPageName);
        }
    }
}

void QTabWndComm::setRegisterSubPage(CombinedRegister reg, QString strSubPage) 
{ 
    std::vector<RegisterBitInfo> regBitInfo = DatabaseOrm::getRegisterBitInfoByCombinedRegisterId(reg.combinedId);
    for(int j = 0; j < regBitInfo.size(); j++)
    {
        // if (registerSubPageMap.count(regBitInfo[j].registerAddr) == 0)
        //     registerSubPageMap.insert(regBitInfo[j].registerAddr, std::vector<QString>(1, strSubPage));
        // else
        registerSubPageMap[regBitInfo[j].registerAddr].emplace_back(strSubPage);
    }
}

void QTabWndComm::saveCombinedReg()
{
    auto future = QtConcurrent::run([=](){
        
        // mtx.lock();
        registerMoudle.clear();
        for(auto it=combinedRegisterMap.begin();it!=combinedRegisterMap.end();it++){
            std::vector<CombinedRegister> combinevector=it.value();
            for(auto regVector:combinevector){
                // qDebug() << regVector.combinedId;
                std::vector<RegisterBitInfo> regBitInfo = DatabaseOrm::getRegisterBitInfoByCombinedRegisterId(regVector.combinedId);
                for(int j=0;j<regBitInfo.size();j++)
                    registerMoudle.push_back(regBitInfo.at(j).registerAddr);
            }
        }
        // emit registerMoudleUpdateDone();
        registerMoudleReadable = true;
        // mtx.unlock();
        // qDebug() << "saved registerMoudle size: " << registerMoudle.size();s
    });
}

void QTabWndComm::read()
{
    emit readSignal();
}

void QTabWndComm::load()
{
    std::vector<std::pair<uint32_t, uint32_t>> regValue;
    // 加载文件
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("Text Files (*.txt)"));
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream txtIn(&file);
            QString     line    = "";
            QStringList list    = { "" };
            uint32_t    address = 0;
            uint32_t    value   = 0;
            while (!txtIn.atEnd()) {
                line    = txtIn.readLine();
                list    = line.split(',');
                address = getQStringToUint(list[0]);
                value   = getQStringToUint(list[1]);
                regValue.push_back(std::make_pair(address, value));
            }
            file.close();
        }
    }
    if (regValue.size() != 0)
        writeSensorRegisterValue(regValue);
    read();
}

void QTabWndComm::save()
{
    if (!futureSaveInfoInit.isFinished())
        futureSaveInfoInit.waitForFinished();
    int valTemp = 0;
    QUsbCommunication::getInstance().readBurst(IR_RGB_MODE_REG_ADDR, (unsigned char*)(&valTemp), ADDRESS_LENGTH, VALUE_LENGTH);
    QString fileName = iniPathName() + "Save";
    QDir    dir(fileName);
    if (!dir.exists())
        dir.mkpath(fileName);
    fileName += "/" + QString::fromStdString(moduleName) + "_" + getCurrentTime() + ".txt";

    QString fileSavePath = QFileDialog::getSaveFileName(this, tr("Save File"), fileName, tr("Text Files (*.txt)"));
    if (!fileSavePath.isEmpty()) {
        QFile file(fileSavePath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream txtOutput(&file);
            QString     line = "";
            // qDebug() << "registerMoudle size: " << registerMoudle.size();
            for (int subPageIndex = 0; subPageIndex < subPageMap.size(); subPageIndex++) {
                line = QString("///////////////%1///////////////\n").arg(subPageMap.value(subPageIndex));
                txtOutput << line;
                line = "";
                if (valTemp == 0x01)
                {
                    QVector<uint32_t> regRgbIr;
                    QVector<uint32_t> regNormal;
                    for (int i = 0; i < registerMoudle.size(); i++)
                    {
                        auto vecSubPages = registerSubPageMap.value(registerMoudle[i]);
                        if (std::count(vecSubPages.begin(), vecSubPages.end(), subPageMap[subPageIndex]) != 0
                            && std::count(vecSubPages.begin(), vecSubPages.end(), "deleted") == 0) 
                        {
                            if (registerTypeMap.count(registerMoudle[i]) == 1) 
                            {
                                regRgbIr.push_back(registerMoudle[i]);
                            } 
                            else
                            {
                                regNormal.push_back(registerMoudle[i]);
                            }
                        }
                    }
                    QSet<uint32_t> uniqueData_regRgbIr(regRgbIr.begin(), regRgbIr.end());
                    regRgbIr = uniqueData_regRgbIr.values().toVector();
                    std::sort(regRgbIr.begin(), regRgbIr.end());
                    QSet<uint32_t> uniqueData_regNormal(regNormal.begin(), regNormal.end());
                    regNormal = uniqueData_regNormal.values().toVector();
                    std::sort(regNormal.begin(), regNormal.end());
                    // qDebug() << "regRgbIr size: " << regRgbIr.size();
                    // qDebug() << "regNormal size: " << regNormal.size();
                    
                    line = ("//AB帧共用\n");
                    txtOutput << line;
                    for (int i = 0; i < regNormal.size(); i++) {
                        uint8_t value = 0;
                        QUsbCommunication::getInstance().readBurst(regNormal[i], &value, ADDRESS_LENGTH, VALUE_LENGTH);
                        if (m_saveType == 2)
                        {
                            int iSlaveId = QUsbCommunication::getInstance().getI2cSlaveID();
                            line = QString("%1 %2 %3 %4 %5\n").arg(
                                QString::number(iSlaveId, 16).toUpper().rightJustified(VALUE_HEX_INFO, '0'),
                                QString::number(regNormal[i], 16).toUpper().rightJustified(ADDRESS_HEX_INFO, '0'),
                                QString::number(value, 16).toUpper().rightJustified(VALUE_HEX_INFO, '0'),
                                "4",
                                "1"
                            );
                            txtOutput << line;
                        }
                        else
                        {
                            line = QString("0x%1").arg(
                                    QString::number(regNormal[i], 16).toUpper().rightJustified(ADDRESS_HEX_INFO, '0'))
                                + ","
                                + QString("0x%1").arg(QString::number(value, 16).toUpper().rightJustified(VALUE_HEX_INFO, '0'))
                                + "\n";
                            txtOutput << line;
                        }
                    }
                    line = ("//AB帧分开调\n");
                    txtOutput << line;
                    line = ("//RGB\n");
                    txtOutput << line;
                    for (int i = 0; i < regRgbIr.size(); i++) {
                        uint8_t value = 0;
                        QUsbCommunication::getInstance().readBurst(registerTypeMap[regRgbIr[i]].first, &value, ADDRESS_LENGTH, VALUE_LENGTH);
                        if (m_saveType == 2)
                        {
                            int iSlaveId = QUsbCommunication::getInstance().getI2cSlaveID();
                            line = QString("%1 %2 %3 %4 %5\n").arg(
                                QString::number(iSlaveId, 16).toUpper().rightJustified(VALUE_HEX_INFO, '0'),
                                QString::number(registerTypeMap[regRgbIr[i]].first, 16).toUpper().rightJustified(ADDRESS_HEX_INFO, '0'),
                                QString::number(value, 16).toUpper().rightJustified(VALUE_HEX_INFO, '0'),
                                "4",
                                "1"
                            );
                            txtOutput << line;
                        }
                        else
                        {
                            line = QString("0x%1").arg(
                                    QString::number(registerTypeMap[regRgbIr[i]].first, 16).toUpper().rightJustified(ADDRESS_HEX_INFO, '0'))
                                + ","
                                + QString("0x%1").arg(QString::number(value, 16).toUpper().rightJustified(VALUE_HEX_INFO, '0'))
                                + "\n";
                            txtOutput << line;
                        }
                    }
                    line = ("//IR\n");
                    txtOutput << line;
                    for (int i = 0; i < regRgbIr.size(); i++) {
                        uint8_t value = 0;
                        QUsbCommunication::getInstance().readBurst(registerTypeMap[regRgbIr[i]].second, &value, ADDRESS_LENGTH, VALUE_LENGTH);
                        if (m_saveType == 2)
                        {
                            int iSlaveId = QUsbCommunication::getInstance().getI2cSlaveID();
                            line = QString("%1 %2 %3 %4 %5\n").arg(
                                QString::number(iSlaveId, 16).toUpper().rightJustified(VALUE_HEX_INFO, '0'),
                                QString::number(registerTypeMap[regRgbIr[i]].second, 16).toUpper().rightJustified(ADDRESS_HEX_INFO, '0'),
                                QString::number(value, 16).toUpper().rightJustified(VALUE_HEX_INFO, '0'),
                                "4",
                                "1"
                            );
                            txtOutput << line;
                        }
                        else
                        {
                            line = QString("0x%1").arg(
                                    QString::number(registerTypeMap[regRgbIr[i]].second, 16).toUpper().rightJustified(ADDRESS_HEX_INFO, '0'))
                                + ","
                                + QString("0x%1").arg(QString::number(value, 16).toUpper().rightJustified(VALUE_HEX_INFO, '0'))
                                + "\n";
                            txtOutput << line;
                        }
                    }
                }
                else
                {
                    line = ("//AB帧共用\n");
                    txtOutput << line;
                    QVector<uint32_t> regOutput;
                    for (int i = 0; i < registerMoudle.size(); i++) 
                    {
                        auto vecSubPages = registerSubPageMap.value(registerMoudle[i]);
                        if (std::count(vecSubPages.begin(), vecSubPages.end(), subPageMap[subPageIndex]) != 0 && std::count(vecSubPages.begin(), vecSubPages.end(), "deleted") == 0)
                        {
                            regOutput.push_back(registerMoudle[i]);
                        }
                    }
                    QSet<uint32_t> uniqueData_regOutput(regOutput.begin(), regOutput.end());
                    regOutput = uniqueData_regOutput.values().toVector();
                    std::sort(regOutput.begin(), regOutput.end());
                    for (int i = 0; i < regOutput.size(); i++) 
                    {
                        uint8_t value = 0;
                        QUsbCommunication::getInstance().readBurst(regOutput[i], &value, ADDRESS_LENGTH, VALUE_LENGTH);
                        if (m_saveType == 2)
                        {
                            int iSlaveId = QUsbCommunication::getInstance().getI2cSlaveID();
                            line = QString("%1 %2 %3 %4 %5\n").arg(
                                QString::number(iSlaveId, 16).toUpper().rightJustified(VALUE_HEX_INFO, '0'),
                                QString::number(regOutput[i], 16).toUpper().rightJustified(ADDRESS_HEX_INFO, '0'),
                                QString::number(value, 16).toUpper().rightJustified(VALUE_HEX_INFO, '0'),
                                "4",
                                "1"
                            );
                            txtOutput << line;
                        }
                        else
                        {
                            line = QString("0x%1").arg(
                                    QString::number(regOutput[i], 16).toUpper().rightJustified(ADDRESS_HEX_INFO, '0'))
                                + ","
                                + QString("0x%1").arg(QString::number(value, 16).toUpper().rightJustified(VALUE_HEX_INFO, '0'))
                                + "\n";
                            txtOutput << line;
                        }
                    }
                }

            }
            // for (int i = 0; i < registerMoudle.size(); i++) {
            //     uint8_t value = 0;
            //     QUsbCommunication::getInstance().readBurst(registerMoudle[i], &value, ADDRESS_LENGTH, VALUE_LENGTH);
            //     line = QString("0x%1").arg(
            //                QString::number(registerMoudle[i], 16).toUpper().rightJustified(ADDRESS_HEX_INFO, '0'))
            //         + ","
            //         + QString("0x%1").arg(QString::number(value, 16).toUpper().rightJustified(VALUE_HEX_INFO, '0'))
            //         + "\n";
            //     txtOutput << line;
            // }
            file.close();
        }
    }
}

void QTabWndComm::recValueByHighLowBits(uint32_t value)
{
    if (labelSpinSliderCheckBoxGroup->getCombinedRegister().empty())
        return;
    std::vector<CombinedRegister> combineReg = labelSpinSliderCheckBoxGroup->getCombinedRegister();
    qDebug() << "combineReg[0].combinedName: " << QString::fromStdString(combineReg[0].combinedName);
    updateTable(combineReg);
    emit updateValue();
}

void QTabWndComm::recvUiDataMiddlewareUpdateRegisterTable(QPointer<QUiDataMiddleware> uiDataMiddleware)
{
    if (!uiDataMiddleware)
        return;
    if (uiDataMiddleware->getCameraId())
        uiDataMiddleware->setCameraId(QSharedPointer<int>(new int(getCurrentCamera())));
    auto qCombinedRegisterList = uiDataMiddleware->getCombinedRegisterList();
    if (qCombinedRegisterList.size() == 0)
        return;
    updateRegisterTable(
        std::vector<CombinedRegister>(qCombinedRegisterList.constBegin(), qCombinedRegisterList.constEnd()));
}

void QTabWndComm::recvEdit(QString str)
{
    labelSpinSliderCheckBoxGroup->setSpinSliderEnable(true);
    labelSpinSliderCheckBoxGroup->setBitCheckBoxEnable(true);
    // qDebug() << str << ":" << combinedRegisterMap.value(str).size();
    updateRegisterTable(combinedRegisterMap.value(str));//写寄存器
}

void QTabWndComm::recvCheckBox(QString str, bool isChecked)
{
    labelSpinSliderCheckBoxGroup->setSpinSliderEnable(true);
    clearCurrentRigester();
    uint32_t value=0;
    if(isChecked){
        value=1;
    }else{
        value=0;
    }
    for(int i=0;i<combinedRegisterMap.value(str).size();i++)
        writeCombinedRegisterValue(combinedRegisterMap.value(str).at(i),value);
    updateRegisterTable(combinedRegisterMap.value(str));
}
void QTabWndComm::addTabWidget(QPointer<QTabWidget> tabWidget)
{
    tabWidget->setStyleSheet(
        "QTabBar::tab{width:100px;margin-right: 10px;font-size:14px;}"
        "QTabBar::tab::selected{font:bold;}"
        "QTabWidget::pane{background-color:transparent; border:0px solid black;border-radius:10px;padding:2px 2px;}");
    contentLayout->addWidget(tabWidget);
    drawFunc=drawTabWidget;
    paintertype=1;
    update();
}

void QTabWndComm::updatePainter()
{
    if(paintertype==0){
        drawFunc=drawDefault;
    }else if(paintertype==1){
        drawFunc=drawTabWidget;
    }
    update();
}

int QTabWndComm::getCurrentCamera()
{
    return CameraType::instance()->getCurrentCamera();
}

void QTabWndComm::setCurrentCamera(int cameraType)
{
    CameraType::instance()->setCurrentCamera(cameraType);
    currentCamera = CameraType::instance()->getCurrentCamera();
}

void QTabWndComm::updateTabCamera()
{
    currentCamera = CameraType::instance()->getCurrentCamera();
}

int QTabWndComm::getTabCamera()
{
    return currentCamera;
}

void QTabWndComm::setRegisterSetVisible(int isVisible)
{
    registerSetWidget->setVisible(isVisible);
}

int QTabWndComm::getValuebyCombineBitAddr(int lowAddr, int highAddr)
{
    int     val = 0;
    QString valuestr;
    QString lowBit;
    QString highBit;
    uint8_t value_low  = 0;
    uint8_t value_high = 0;
    QUsbCommunication::getInstance().readBurst(lowAddr, &value_low, ADDRESS_LENGTH, VALUE_LENGTH);
    QUsbCommunication::getInstance().readBurst(highAddr, &value_high, ADDRESS_LENGTH, VALUE_LENGTH);

    lowBit   = lowBit.setNum((int)value_low, 2).rightJustified(8, '0');
    highBit  = highBit.setNum((int)value_high, 2).rightJustified(8, '0');
    valuestr = highBit + lowBit;
    val      = valuestr.toInt(nullptr, 2);
    return val;
}

int QTabWndComm::getValuebyCombineBit(int low, int high)
{
    int     val = 0;
    QString lowBit;
    QString highBit;
    QString valuestr;
    lowBit   = lowBit.setNum((int)low, 2).rightJustified(8, '0');
    highBit  = highBit.setNum((int)high, 2).rightJustified(8, '0');
    valuestr = highBit + lowBit;
    val      = valuestr.toInt(nullptr, 2);
    return val;
}

void QTabWndComm::setValuebyCombinedBit(int value, int lowAddr, int highAddr)
{
    QString str;
    QString ctr = "0";
    str         = str.setNum(value, 2).rightJustified(16, '0');
    QString highValueStr;
    QString lowValueStr;
    for (int i = 0; i < 8; i++)
        highValueStr.append(str[i]);
    for (int i = 8; i < 16; i++)
        lowValueStr.append(str[i]);

    uint8_t value_low  = 0;
    uint8_t value_high = 0;
    value_low          = lowValueStr.toInt(nullptr, 2);
    value_high         = highValueStr.toInt(nullptr, 2);
    QUsbCommunication::getInstance().writeBurst(lowAddr, &value_low, ADDRESS_LENGTH, VALUE_LENGTH);
    QUsbCommunication::getInstance().writeBurst(highAddr, &value_high, ADDRESS_LENGTH, VALUE_LENGTH);
}

void QTabWndComm::setValuebyCombinedBit(int value, QString lowAddr, QString highAddr)
{
    QString str;
    QString ctr = "0";
    str         = str.setNum(value, 2).rightJustified(16, '0');
    QString highValueStr;
    QString lowValueStr;
    for (int i = 0; i < 8; i++)
        highValueStr.append(str[i]);
    for (int i = 8; i < 16; i++)
        lowValueStr.append(str[i]);

    uint8_t value_low  = 0;
    uint8_t value_high = 0;
    value_low          = lowValueStr.toInt(nullptr, 2);
    value_high         = highValueStr.toInt(nullptr, 2);
    QUsbCommunication::getInstance().writeBurst(lowAddr.toInt(nullptr, 16), &value_low, ADDRESS_LENGTH, VALUE_LENGTH);
    QUsbCommunication::getInstance().writeBurst(highAddr.toInt(nullptr, 16), &value_high, ADDRESS_LENGTH, VALUE_LENGTH);
}

void QTabWndComm::addRegisterTable()
{
    labelSpinSliderCheckBoxGroup->changeGroupLayout(); // 默认布局
    labelSpinSliderCheckBoxGroup->setTableHeader(QStringList() << "Address"
                                                               << "Bit"
                                                               << "NAME"
                                                               << "HEX"
                                                               << "DEC"
                                                               << "Real"
                                                               << "Description");
    labelSpinSliderCheckBoxGroup->setTableListVisible(true);
    labelSpinSliderCheckBoxGroup->setReadBtnVisible(false);
    registerSetLayout->addWidget(labelSpinSliderCheckBoxGroup.get(), 1, 0);
}

void QTabWndComm::updateRegisterTable(std::vector<CombinedRegister> regVector)
{
    clearCurrentRigester();
    labelSpinSliderCheckBoxGroup->setCombinedRegister(regVector);
    updateTable(regVector);
}

void QTabWndComm::updateTable(std::vector<CombinedRegister> regVector)
{
    labelSpinSliderCheckBoxGroup->clearTableRows();
    for (int i = 0; i < regVector.size(); ++i) {
        auto     c_reg = regVector.at(i);
        uint32_t value = 0;
        readCombinedRegisterValue(c_reg, value);
        std::vector<RegisterBitInfo> regBitInfo = DatabaseOrm::getRegisterBitInfoByCombinedRegisterId(c_reg.combinedId);
        // qDebug() << c_reg.combinedId << "," << c_reg.moduleName.c_str() << "," << c_reg.combinedName.c_str();
        for (int j = 0; j < regBitInfo.size(); j++) {
            int  row = labelSpinSliderCheckBoxGroup->currentTableRow();
            auto reg = DatabaseOrm::getRegisterByAddress(regBitInfo.at(j).registerAddr);
            uint8_t  regvalue = 0;
            QUsbCommunication::getInstance().readBurst(reg.address, &regvalue, ADDRESS_LENGTH, VALUE_LENGTH);
            qDebug() << "upDateTable read address:" << QString::number(regBitInfo.at(j).registerAddr, 16)
                     << "value:" << QString::number(regvalue, 2).rightJustified(8, '0');
            labelSpinSliderCheckBoxGroup->appendTableRow(
                row, 0,
                QString("0x%1").arg(QString::number(regBitInfo.at(j).registerAddr, 16)
                                        .toUpper()
                                        .rightJustified(VALUE_HEX_INFO, '0')));
            labelSpinSliderCheckBoxGroup->appendTableRow(
                    row, 1,
                    QString("0x%1").arg(QString::number(regBitInfo.at(j).bitMask, 16)
                                                .toUpper()
                                                .rightJustified(VALUE_HEX_INFO, '0')));
            labelSpinSliderCheckBoxGroup->appendTableRow(row, 2,
                                                         QString::fromStdString(regBitInfo.at(j).registerBitName));

            int bitVal = getBitValue(regBitInfo.at(j).bitMask, regvalue);
            labelSpinSliderCheckBoxGroup->appendTableRow(
                    row, 3,
                    QString("0x%1").arg(QString::number(bitVal, 16).toUpper().rightJustified(VALUE_HEX_INFO, '0')));
            labelSpinSliderCheckBoxGroup->appendTableRow(row, 4, QString::number(bitVal, 10));
            labelSpinSliderCheckBoxGroup->appendTableRow(row, 5, QString::number(0, 'f', 2));
            labelSpinSliderCheckBoxGroup->appendTableRow(row, 6, QString::fromStdString(reg.registerDescription));
        }
    }
}

void QTabWndComm::updateTableList(QList<CombinedRegister> regList)
{
    labelSpinSliderCheckBoxGroup->clearTableRows();
    for (int i = 0; i < regList.size(); ++i) {
        auto     c_reg = regList.at(i);
        uint32_t value = 0;
        readCombinedRegisterValue(c_reg, value);
        std::vector<RegisterBitInfo> regBitInfo = DatabaseOrm::getRegisterBitInfoByCombinedRegisterId(c_reg.combinedId);
        for (int j = 0; j < regBitInfo.size(); j++) {
            int  row = labelSpinSliderCheckBoxGroup->currentTableRow();
            auto reg = DatabaseOrm::getRegisterByAddress(regBitInfo.at(j).registerAddr);
            uint8_t  regvalue = 0;
            QUsbCommunication::getInstance().readBurst(reg.address, &regvalue, ADDRESS_LENGTH, VALUE_LENGTH);
            labelSpinSliderCheckBoxGroup->appendTableRow(
                row, 0,
                QString("0x%1").arg(QString::number(regBitInfo.at(j).registerAddr, 16)
                                        .toUpper()
                                        .rightJustified(VALUE_HEX_INFO, '0')));
            labelSpinSliderCheckBoxGroup->appendTableRow(
                row, 1,
                QString("0x%1").arg(QString::number(regBitInfo.at(j).bitMask, 16)
                                        .toUpper()
                                        .rightJustified(VALUE_HEX_INFO, '0')));
            labelSpinSliderCheckBoxGroup->appendTableRow(row, 2,
                QString::fromStdString(regBitInfo.at(j).registerBitName));

            int bitVal = getBitValue(regBitInfo.at(j).bitMask, regvalue);
            labelSpinSliderCheckBoxGroup->appendTableRow(
                row, 3,
                QString("0x%1").arg(QString::number(bitVal, 16).toUpper().rightJustified(VALUE_HEX_INFO, '0')));
            labelSpinSliderCheckBoxGroup->appendTableRow(row, 4, QString::number(bitVal, 10));
            labelSpinSliderCheckBoxGroup->appendTableRow(row, 5, QString::number(0, 'f', 2));
            labelSpinSliderCheckBoxGroup->appendTableRow(row, 6, QString::fromStdString(reg.registerDescription));
        }
    }
}

int QTabWndComm::getIntbyFloat(float currentValue)
{
    int   value    = 0;
    float fTempccm = currentValue;
    // fccm = ((int)(1000.0f * fccm)) * 256 / 1000;
    if (fTempccm > 0) {
        fTempccm += (float)0.0005;
    }
    if (fTempccm < 0) {
        fTempccm -= (float)0.0005;
    }
    uint16_t uccmDataH=0;
    uint16_t uccmDataL =0;
    if(fTempccm<0){
         uccmDataH = (((int16_t)(fTempccm * 256)) & 0xff00) >> 8;
         uccmDataL = ((int16_t)(fTempccm * 256)) & 0xff;
    }else{
         uccmDataH = (((int16_t)(fTempccm * 256) & 0x7ff) & 0xff00) >> 8;
         uccmDataL = ((int16_t)(fTempccm * 256) & 0x7ff) & 0xff;
    }
    value              = getValuebyCombineBit(uccmDataL, uccmDataH);
    return value;
}

float QTabWndComm::getFloatbyInt(int currentValue)
{
    float fccm = 0.0;
    if (currentValue & 0x400) {
        fccm = (float)((int16_t)(currentValue | 0xf800)) / 256;
    } else {
        fccm = (float)currentValue / 256;
    }
    if (fccm > 0) {
        fccm = fccm + 0.0005;
    }
    if (fccm < 0) {
        fccm = fccm - 0.0005;
    }
    return fccm;
}

void QTabWndComm::clearCurrentRigester()
{
    currentAddressVector = std::vector<int>();
    labelSpinSliderCheckBoxGroup->clearTableRows();
    labelSpinSliderCheckBoxGroup->clearRegister();
    regComVector.clear();
}

void QTabWndComm::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setRenderHints(QPainter::SmoothPixmapTransform, true);
    p.setPen(QPen(QColor("black"), 0.5));
    switch (drawFunc) {
    case drawTabWidget:
        p.drawRect(QRectF(contentWidget->x()+10,contentWidget->y()+40,contentWidget->width()-20,contentWidget->height()-40));
        break;
    case drawDefault:
        p.drawRect(QRectF(contentWidget->x() + 10, contentWidget->y() + 5, contentWidget->width() - 20,
            contentWidget->height() - 5));
        break;
    default:
        break;
    }
}

void QTabWndComm::loadDeletedRegistersFromFile(QString strDelRegListTxt)
{
    // 检查文件是否存在
    QFile file(strDelRegListTxt);
    if (!file.exists()) {
        qDebug() << "File does not exist:" << strDelRegListTxt;
        return;
    }

    // 打开文件
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Cannot open file for reading:" << strDelRegListTxt;
        return;
    }

    QTextStream in(&file);
    QString content = in.readAll();
    file.close();

    // 使用正则表达式匹配0x开头的十六进制数，不区分大小写
    QRegularExpression hexRegex("0x[0-9a-fA-F]+");
    QRegularExpressionMatchIterator matches = hexRegex.globalMatch(content);

    // 处理所有匹配的地址
    while (matches.hasNext()) {
        QRegularExpressionMatch match = matches.next();
        QString hexAddress = match.captured(0);
        
        // 转换为uint32_t
        bool ok;
        uint32_t address = hexAddress.toUInt(&ok, 16);
        
        if (ok) {
            // 在registerSubPageMap中将该地址对应的值设定为"deleted"
            registerSubPageMap[address].push_back("deleted");
        } else {
            qDebug() << "Failed to convert hex address:" << hexAddress;
        }
    }
}

void QTabWndComm::saveSpecial(QString folderPath)
{
    // 直接使用传入的文件夹路径，无需弹出窗口选择
    QDir dir(folderPath);
    if (!dir.exists())
        dir.mkpath(folderPath);
    
    QString fileName = folderPath + "/" + QString::fromStdString(moduleName) + ".txt";
    int valTemp = 0;
    QUsbCommunication::getInstance().readBurst(IR_RGB_MODE_REG_ADDR, (unsigned char*)(&valTemp), ADDRESS_LENGTH, VALUE_LENGTH);
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream txtOutput(&file);
        QString     line = "";
        qDebug() << "registerMoudle size: " << registerMoudle.size();
        
        for (int subPageIndex = 0; subPageIndex < subPageMap.size(); subPageIndex++) {
            line = QString("///////////////%1///////////////\n").arg(subPageMap.value(subPageIndex));
            txtOutput << line;
            line = "";
            if (valTemp == 0x01)
            {
                std::vector<uint32_t> regRgbIr;
                std::vector<uint32_t> regNormal;
                for (int i = 0; i < registerMoudle.size(); i++)
                {
                    auto vecSubPages = registerSubPageMap.value(registerMoudle[i]);
                    if (std::count(vecSubPages.begin(), vecSubPages.end(), subPageMap[subPageIndex]) != 0
                        && std::count(vecSubPages.begin(), vecSubPages.end(), "deleted") == 0) 
                    {
                        if (registerTypeMap.count(registerMoudle[i]) == 1) 
                        {
                            regRgbIr.push_back(registerMoudle[i]);
                        } 
                        else
                        {
                            regNormal.push_back(registerMoudle[i]);
                        }
                    }
                }
                qDebug() << "regRgbIr size: " << regRgbIr.size();
                qDebug() << "regNormal size: " << regNormal.size();
                
                line = ("//AB帧共用\n");
                txtOutput << line;
                for (int i = 0; i < regNormal.size(); i++) {
                    uint8_t value = 0;
                    QUsbCommunication::getInstance().readBurst(regNormal[i], &value, ADDRESS_LENGTH, VALUE_LENGTH);
                    if (m_saveType == 2)
                    {
                        int iSlaveId = QUsbCommunication::getInstance().getI2cSlaveID();
                        line = QString("%1 %2 %3 %4 %5\n").arg(
                            QString::number(iSlaveId, 16).toUpper().rightJustified(VALUE_HEX_INFO, '0'),
                            QString::number(regNormal[i], 16).toUpper().rightJustified(ADDRESS_HEX_INFO, '0'),
                            QString::number(value, 16).toUpper().rightJustified(VALUE_HEX_INFO, '0'),
                            "4",
                            "1"
                        );
                        txtOutput << line;
                    }
                    else
                    {
                        line = QString("0x%1").arg(
                                QString::number(regNormal[i], 16).toUpper().rightJustified(ADDRESS_HEX_INFO, '0'))
                            + ","
                            + QString("0x%1").arg(QString::number(value, 16).toUpper().rightJustified(VALUE_HEX_INFO, '0'))
                            + "\n";
                        txtOutput << line;
                    }
                }
                line = ("//AB帧分开调\n");
                txtOutput << line;
                line = ("//RGB\n");
                txtOutput << line;
                for (int i = 0; i < regRgbIr.size(); i++) {
                    uint8_t value = 0;
                    QUsbCommunication::getInstance().readBurst(registerTypeMap[regRgbIr[i]].first, &value, ADDRESS_LENGTH, VALUE_LENGTH);
                    if (m_saveType == 2)
                    {
                        int iSlaveId = QUsbCommunication::getInstance().getI2cSlaveID();
                        line = QString("%1 %2 %3 %4 %5\n").arg(
                            QString::number(iSlaveId, 16).toUpper().rightJustified(VALUE_HEX_INFO, '0'),
                            QString::number(registerTypeMap[regRgbIr[i]].first, 16).toUpper().rightJustified(ADDRESS_HEX_INFO, '0'),
                            QString::number(value, 16).toUpper().rightJustified(VALUE_HEX_INFO, '0'),
                            "4",
                            "1"
                        );
                        txtOutput << line;
                    }
                    else
                    {
                        line = QString("0x%1").arg(
                                QString::number(registerTypeMap[regRgbIr[i]].first, 16).toUpper().rightJustified(ADDRESS_HEX_INFO, '0'))
                            + ","
                            + QString("0x%1").arg(QString::number(value, 16).toUpper().rightJustified(VALUE_HEX_INFO, '0'))
                            + "\n";
                        txtOutput << line;
                    }
                }
                line = ("//IR\n");
                txtOutput << line;
                for (int i = 0; i < regRgbIr.size(); i++) {
                    uint8_t value = 0;
                    QUsbCommunication::getInstance().readBurst(registerTypeMap[regRgbIr[i]].second, &value, ADDRESS_LENGTH, VALUE_LENGTH);
                    if (m_saveType == 2)
                    {
                        int iSlaveId = QUsbCommunication::getInstance().getI2cSlaveID();
                        line = QString("%1 %2 %3 %4 %5\n").arg(
                            QString::number(iSlaveId, 16).toUpper().rightJustified(VALUE_HEX_INFO, '0'),
                            QString::number(registerTypeMap[regRgbIr[i]].second, 16).toUpper().rightJustified(ADDRESS_HEX_INFO, '0'),
                            QString::number(value, 16).toUpper().rightJustified(VALUE_HEX_INFO, '0'),
                            "4",
                            "1"
                        );
                        txtOutput << line;
                    }
                    else
                    {
                        line = QString("0x%1").arg(
                                QString::number(registerTypeMap[regRgbIr[i]].second, 16).toUpper().rightJustified(ADDRESS_HEX_INFO, '0'))
                            + ","
                            + QString("0x%1").arg(QString::number(value, 16).toUpper().rightJustified(VALUE_HEX_INFO, '0'))
                            + "\n";
                        txtOutput << line;
                    }
                }
            }
            else
            {
                line = ("//AB帧共用\n");
                txtOutput << line;
                std::vector<uint32_t> regOutput;
                for (int i = 0; i < registerMoudle.size(); i++) 
                {
                    auto vecSubPages = registerSubPageMap.value(registerMoudle[i]);
                    if (std::count(vecSubPages.begin(), vecSubPages.end(), subPageMap[subPageIndex]) != 0 && std::count(vecSubPages.begin(), vecSubPages.end(), "deleted") == 0)
                    {
                        regOutput.push_back(registerMoudle[i]);
                    }
                }

                for (int i = 0; i < regOutput.size(); i++) 
                {
                    uint8_t value = 0;
                    QUsbCommunication::getInstance().readBurst(regOutput[i], &value, ADDRESS_LENGTH, VALUE_LENGTH);
                    if (m_saveType == 2)
                    {
                        int iSlaveId = QUsbCommunication::getInstance().getI2cSlaveID();
                        line = QString("%1 %2 %3 %4 %5\n").arg(
                            QString::number(iSlaveId, 16).toUpper().rightJustified(VALUE_HEX_INFO, '0'),
                            QString::number(regOutput[i], 16).toUpper().rightJustified(ADDRESS_HEX_INFO, '0'),
                            QString::number(value, 16).toUpper().rightJustified(VALUE_HEX_INFO, '0'),
                            "4",
                            "1"
                        );
                        txtOutput << line;
                    }
                    else
                    {
                        line = QString("0x%1").arg(
                                QString::number(regOutput[i], 16).toUpper().rightJustified(ADDRESS_HEX_INFO, '0'))
                            + ","
                            + QString("0x%1").arg(QString::number(value, 16).toUpper().rightJustified(VALUE_HEX_INFO, '0'))
                            + "\n";
                        txtOutput << line;
                    }
                }
            }
        }
        
        // 额外保存registerSubPageMap中值含有"deleted"的寄存器地址及其对应值
        line = QString("///////////////Deleted Registers///////////////\n");
        txtOutput << line;
        // line = ("//Deleted registers\n");
        // txtOutput << line;
        
        for (auto it = registerSubPageMap.begin(); it != registerSubPageMap.end(); ++it) {
            uint32_t regAddr = it.key();
            auto vecSubPages = it.value();
            
            // 检查是否包含"deleted"
            if (std::count(vecSubPages.begin(), vecSubPages.end(), "deleted") > 0) {
                uint8_t value = 0;
                QUsbCommunication::getInstance().readBurst(regAddr, &value, ADDRESS_LENGTH, VALUE_LENGTH);
                if (m_saveType == 2)
                {
                    int iSlaveId = QUsbCommunication::getInstance().getI2cSlaveID();
                    line = QString("%1 %2 %3 %4 %5\n").arg(
                        QString::number(iSlaveId, 16).toUpper().rightJustified(VALUE_HEX_INFO, '0'),
                        QString::number(regAddr, 16).toUpper().rightJustified(ADDRESS_HEX_INFO, '0'),
                        QString::number(value, 16).toUpper().rightJustified(VALUE_HEX_INFO, '0'),
                        "4",
                        "1"
                    );
                    txtOutput << line;
                }
                else
                {
                    line = QString("0x%1").arg(
                            QString::number(regAddr, 16).toUpper().rightJustified(ADDRESS_HEX_INFO, '0'))
                        + ","
                        + QString("0x%1").arg(QString::number(value, 16).toUpper().rightJustified(VALUE_HEX_INFO, '0'))
                        + "\n";
                    txtOutput << line;
                }
           
            }
        }
        
        file.close();
    }
}
