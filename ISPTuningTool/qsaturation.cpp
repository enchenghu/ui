#include "qsaturation.h"
#include "Common/UI/qcommonuiinterface.h"
#include "Common/UI/quidatamiddleware.h"
#include "qgroupbox.h"
#include <QComboBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <cstdint>
#include <functional>
#include <vector>
#include "qtransfermanager.h"
#include "Common/Others/qusbcommunication.h"

const QString        TAB_NAME          = "Saturation";
const QList<QString> TAB_NAME_LIST     = { "Curve", "YUVDNS" };
const QList<QString> TABLE_HEADER_LIST = { "SatLTH", "SatHTH", "SatLTar", "SatHTar", "SatRate" };

QSaturation::QSaturation(QWidget *parent)
    : QTabWndComm(parent)
    , tabWidget(new QTabWidget(this))
{

    int iMode = TransferManager::getInstance().getRgbIrMode();
    strRgbIrMode = iMode == 0 ? "RGB" : "IR";
    moduleName = TAB_NAME.toStdString();
    strDelRegListTxt = QCoreApplication::applicationDirPath() + "\\extracted_data\\Saturation.txt";
    loadDeletedRegistersFromFile(strDelRegListTxt);
    this->init();
    this->initConnect();
}

QSaturation::~QSaturation() { }

void QSaturation::recvEditTarget(QString str) 
{ 
    qDebug() << "QSaturation::recvEditTarget" << str;
    if (str.isEmpty()) return;
    qDebug() << "QSaturation::recvEditTarget clearCurrentRigester"; 
     // 清除当前寄存器，避免重复添加
    clearCurrentRigester();

    int valTemp = 0;
    QUsbCommunication::getInstance().readBurst(IR_RGB_MODE_REG_ADDR, (unsigned char*)(&valTemp), ADDRESS_LENGTH, VALUE_LENGTH);

    std::vector<CombinedRegister> re;
    std::string strSuffix = strRgbIrMode == "RGB" ? "_RGB_AB" : "_IR_AB";
    if (valTemp == 0x01)
    {
        re = combinedRegisterMap.value(str + strSuffix.c_str());
    }
    else 
    {
        re = combinedRegisterMap.value(str);
    }
    
    updateRegisterTable(re);
}

void QSaturation::init()
{
    addTabWidget(tabWidget);
    const std::map<QString, std::function<void(QWidget *)>> initFuncList = {
        { "Curve", std::bind(&QSaturation::initCurveSubTab, this, std::placeholders::_1) },
        { "YUVDNS", std::bind(&QSaturation::initYuvDnsSubTab, this, std::placeholders::_1) },
    };
    if(m_nLevel!=LEVEL_ADMINISTRATOR)
    {
        QList<QString>* pTemp = (QList<QString>*)&TAB_NAME_LIST;
        *pTemp={ "Curve" };
    }
    int iCount = 0;
    for (const auto &name : TAB_NAME_LIST) {
        QPointer<QWidget> subTab = new QWidget(this);
        tabWidget->addTab(subTab, name);
        initFuncList.at(name)(subTab);
        subPageMap.insert(iCount, name);
        iCount++;
    }
    recordUIRegister();
    qDebug() << "QSaturation::init recordUIRegister done";
    recordUIRegSubPageInfo();
    qDebug() << "QSaturation::init recordUIRegSubPageInfo done";
    addRegisterTable();
    qDebug() << "QSaturation::init addRegisterTable done";
    changeTab(0);
    
    // 获取基本寄存器地址，排除registerTypeMap中value包含的RGB/IR寄存器，并去重
    auto allRegisters = QUiDataMiddleware::getRegisterAddressByUiModule(TAB_NAME);
    regAddrModule.clear();
    
    // 构建RGB/IR寄存器地址集合以提高查找性能
    std::set<uint32_t> rgbIrRegisters;
    for (auto iter = registerTypeMap.begin(); iter != registerTypeMap.end(); ++iter) {
        rgbIrRegisters.insert(iter.value().first);  // RGB寄存器
        rgbIrRegisters.insert(iter.value().second); // IR寄存器
    }
    
    for (auto reg : allRegisters) {
        // 检查寄存器是否为RGB/IR寄存器
        if (rgbIrRegisters.find(reg) == rgbIrRegisters.end()) {
            regAddrModule.insert(reg);
        }
    }
    // 将去重后的寄存器地址复制到registerMoudle
    std::vector<uint32_t>().swap(registerMoudle);
    for (auto iter : regAddrModule) {
        registerMoudle.emplace_back(iter);
    }
}

void QSaturation::updateValue()
{
    changeTab(tabWidget->currentIndex());
}

void QSaturation::initConnect()
{
    connect(tabWidget.get(), &QTabWidget::currentChanged, this, &QSaturation::changeTab, Qt::QueuedConnection);
    connect(isoSatCurve, &QCurveChartGroup::signalAllPointsChanged, this, &QSaturation::recvPointsChanged,
        Qt::QueuedConnection);
    connect(this, &QTabWndComm::readSignal, this, &QSaturation::readCurve, Qt::QueuedConnection);
    connect(this, &QTabWndComm::readSignal, this, [=](){updateValue();});
    connect(this, &QTabWndComm::updateValue, this, &QSaturation::sortSatIsoList, Qt::QueuedConnection);
    connect(
        this, &QTabWndComm::reConnectRead, this,
        [=]() {
            recordUIRegister();updateValue();
        },
        Qt::QueuedConnection);
    connect(&TransferManager::getInstance(), &TransferManager::rgbIrModeChanged, 
        this, &QSaturation::changeRgbIrMode);
        
    // 连接Curve tab控件信号
    // if (gainSatEnable) {
    //     connect(gainSatEnable, &QLabelCheckBox::release, this, [=](){QTabWndComm::recvEdit(gainSatEnable->Label());});
    //     connect(gainSatEnable, &QLabelCheckBox::checkSignal, this, [=](){recvCheckBox(gainSatEnable->Label(), gainSatEnable->checkState());});
    // }
    
    // if (saturation) {
    //     connect(saturation, &QLabelEditGroup::releaseLabel, this, [=](){QTabWndComm::recvEdit(saturation->Label());});
    // }
    
    // for (int i = 0; i < 8; i++) {
    //     if (satIsoList[i]) {
    //         connect(satIsoList[i], &QLabelEditGroup::releaseLabel, this, [=](){QTabWndComm::recvEdit(satIsoList[i]->Label());});
    //     }
    // }
    
    // 连接YUVDNS tab控件信号
    // for (int i = 0; i < 8; i++) {
    //     if (gainNode[i]) {
    //         QString label = gainNode[i]->Label().remove("*");
    //         if (SATURATION_RGBIR_ENABLE_MAP.contains({"Saturation", label})) {
    //             connect(gainNode[i], &QLabelEditGroup::releaseLabel, this, [=](){recvEditTarget(label);});
    //         } else {
    //             connect(gainNode[i], &QLabelEditGroup::releaseLabel, this, [=](){QTabWndComm::recvEdit(gainNode[i]->Label());});
    //         }
    //     }
    // }
    
    // if (satAdjEnable) {
    //     connect(satAdjEnable, &QLabelCheckBox::release, this, [=](){QTabWndComm::recvEdit(satAdjEnable->Label());});
    //     connect(satAdjEnable, &QLabelCheckBox::checkSignal, this, [=](){recvCheckBox(satAdjEnable->Label(), satAdjEnable->checkState());});
    // }
    
    // if (curGain) {
    //     connect(curGain, &QLabelEditGroup::releaseLabel, this, [=](){QTabWndComm::recvEdit(curGain->Label());});
    // }
}

void QSaturation::initCurveSubTab(QPointer<QWidget> subTab)
{
    QPointer<QGridLayout> layout = new QGridLayout(subTab);
    layout->setAlignment(Qt::AlignLeft);
    subTab->setLayout(layout);

    // GainSatEnable
    gainSatEnable = new QLabelCheckBox();
    gainSatEnable->setLabel("GainSatEnable");
    gainSatEnable->setMouseTracking(true);
    layout->addWidget(gainSatEnable, 0, 0);
    connect(gainSatEnable, &QLabelCheckBox::release, this, [=](){QTabWndComm::recvEdit(gainSatEnable->Label());});
    connect(gainSatEnable, &QLabelCheckBox::checkSignal, this, [=](){recvCheckBox(gainSatEnable->Label(), gainSatEnable->checkState());});

    curIso = new QLabelEditGroup(subTab);
    curIso->setLabelValue("Cur Iso");
    curIso->setValue(0);
    curIso->setMouseTracking(true);
    curIso->setEditable(false);
    connect(this, &QTabWndComm::readSignal, curIso, [&]() {
        if (!curIso->isVisible())
            return;
        auto gainL      = QUiDataMiddleware(TAB_NAME, "Current Gain L").readValueFromDevice();
        auto expoL      = QUiDataMiddleware(TAB_NAME, "Current Expo L").readValueFromDevice();
        auto currentIso = (static_cast<uint64_t>(gainL) * (static_cast<uint64_t>(expoL) >> 4)) >> 4;
        curIso->setValue(currentIso);
    });
    
    layout->addWidget(curIso, 1, 0);

    // Saturation
    saturation = new QLabelEditGroup();
    saturation->setLabelValue("Saturation");
    saturation->setMouseTracking(true);
    saturation->setEditable(false);
    layout->addWidget(saturation, 2, 0);
    connect(saturation, &QLabelEditGroup::releaseLabel, this, [=](){QTabWndComm::recvEdit(saturation->Label());});

    // SatISOList
    for (int i = 0; i < 8; i++) {
        satIsoList[i] = new QLabelEditGroup();
        satIsoList[i]->setLabelValue(QString("SatISOList[%1]").arg(i));
        satIsoList[i]->setMouseTracking(true);
        satIsoList[i]->setEditable(false);
        layout->addWidget(satIsoList[i], i + 3, 0);
        connect(satIsoList[i], &QLabelEditGroup::releaseLabel, this, [=](){QTabWndComm::recvEdit(satIsoList[i]->Label());});
    }

    isoSatCurve = new QCurveChartGroup(this);
    isoSatCurve->setXMax(65535);
    isoSatCurve->setYMax(255);
    isoSatCurve->setAxisRange(0, 65535, 0, 255);
    isoSatCurve->setAutoAdjustAxisRange(false);
    isoSatCurve->setCurveFittingType(CurveFittingType::NONE);
    isoSatCurve->setCurvePointType(7);//single Mode
    isoSatCurve->setXAxisLabelFormat("0x%x");
    layout->addWidget(isoSatCurve, 0, 1, 11, 1);
    layout->setColumnStretch(0, 1);
    layout->setColumnStretch(1, 3);
}

void QSaturation::initYuvDnsSubTab(QPointer<QWidget> subTab)
{
    QPointer<QGridLayout> layout = new QGridLayout(subTab);
    layout->setAlignment(Qt::AlignLeft);
    subTab->setLayout(layout);
    
    // 创建控件
    QString gainNodeFormat = "GainNode[%1]";
    for (int i = 0; i < 8; i++) {
        gainNode[i] = new QLabelEditGroup();
        QString label = gainNodeFormat.arg(i);
        if (SATURATION_RGBIR_ENABLE_MAP.contains({"Saturation", label})) {
            gainNode[i]->setLabelValue("*" + label);
        } else {
            gainNode[i]->setLabelValue(label);
        }
        gainNode[i]->setMouseTracking(true);
        gainNode[i]->setEditable(false);
        layout->addWidget(gainNode[i], i + 2, 0);
        QString label2 = gainNode[i]->Label().remove("*");
        if (SATURATION_RGBIR_ENABLE_MAP.contains({"Saturation", label2})) {
            connect(gainNode[i], &QLabelEditGroup::releaseLabel, this, [=](){recvEditTarget(label2);});
        } else {
            connect(gainNode[i], &QLabelEditGroup::releaseLabel, this, [=](){QTabWndComm::recvEdit(gainNode[i]->Label());});
        }
    }

    satAdjEnable = new QLabelCheckBox();
    satAdjEnable->setLabel("SatAdjEnable");
    satAdjEnable->setMouseTracking(true);
    layout->addWidget(satAdjEnable, 1, 0);
    connect(satAdjEnable, &QLabelCheckBox::release, this, [=](){QTabWndComm::recvEdit(satAdjEnable->Label());});
    connect(satAdjEnable, &QLabelCheckBox::checkSignal, this, [=](){recvCheckBox(satAdjEnable->Label(), satAdjEnable->checkState());});

    curGain = new QLabelEditGroup();
    curGain->setLabelValue("CurGain");
    curGain->setMouseTracking(true);
    curGain->setEditable(false);
    layout->addWidget(curGain, 0, 0);
    connect(curGain, &QLabelEditGroup::releaseLabel, this, [=](){QTabWndComm::recvEdit(curGain->Label());});

    tableList = new QTableListGroup();
    tableList->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    tableList->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
    tableList->verticalHeader()->setDefaultAlignment(Qt::AlignCenter);
    tableList->setMinimumHeight(0);
    tableList->setHeader(QStringList(TABLE_HEADER_LIST));
    tableList->setVerticalHeader([]() {
        QStringList verticalHeader;
        for (int i = 0; i < 8; i++) {
            verticalHeader << QString("[%1]").arg(i);
        }
        return verticalHeader;
    }());
    for (int i = 0; i < tableList->verticalHeader()->count(); i++) {
        for (int j = 0; j < tableList->horizontalHeader()->count(); j++) {
            tableList->appendRow(i, j, QString("0x%1").arg(0, 2, 16, QChar('0')), Qt::AlignCenter);
        }
    }
    for (int i = 0; i < tableList->verticalHeader()->count(); i++) {
        tableList->verticalHeader()->setSectionResizeMode(i, QHeaderView::Stretch);
    }
    for (int i = 0; i < tableList->horizontalHeader()->count(); i++) {
        tableList->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Stretch);
    }
    tableList->setVerticalHeaderVisible(true);
    tableList->setHorizontalHeaderVisible(true);
    
    connect(tableList, &QTableListGroup::sendCurrentItem, this, &QSaturation::recvTableListItem, Qt::QueuedConnection);
    connect(this, &QTabWndComm::readSignal, this, [=]() {
        for (int i = 0; i < TABLE_HEADER_LIST.size(); i++) {
            auto uiDataMiddleware = QUiDataMiddleware(TAB_NAME, TABLE_HEADER_LIST[i]);
            auto crValueList      = uiDataMiddleware.getCombinedRegisterValueList();
            for (int j = 0; j < crValueList.size(); j++) {
                tableList->getItem(j, i)->setText(QString("0x%1").arg(crValueList[j].second, 2, 16, QChar('0')));
            }
        }
    });
    QPointer<QGroupBox>   groupBox = new QGroupBox(this);
    QPointer<QVBoxLayout> vLayout  = new QVBoxLayout(groupBox);
    vLayout->addWidget(tableList);
    groupBox->setLayout(vLayout);
    layout->addWidget(groupBox, 0, 1, 10, 1);
    layout->setColumnStretch(0, 1);
    layout->setColumnStretch(1, 4);
}

void QSaturation::sortSatIsoList()
{
    std::vector<std::pair<DatabaseOrm::CombinedRegister, uint32_t>> crIsoValueList;
    for (int i = 0; i < 8; ++i) {
        auto uiDataMiddlewareIso = QUiDataMiddleware(TAB_NAME, QString("SatISOList[%1]").arg(i));
        auto crValueList         = uiDataMiddlewareIso.getCombinedRegisterValueList();
        crIsoValueList.push_back(crValueList[0]);
    }
    auto valueList = std::vector<uint32_t>(crIsoValueList.size());
    bool isSorted  = true;
    for (int i = 0; i < crIsoValueList.size(); ++i) {
        valueList[i] = crIsoValueList[i].second;
        if (i > 0 && valueList[i] < valueList[i - 1]) {
            isSorted = false;
        }
    }
    if (!isSorted) {
        // sort
        std::sort(valueList.begin(), valueList.end());
        bool updated = false;
        // set
        for (int i = 0; i < crIsoValueList.size(); ++i) {
            crIsoValueList[i].second = valueList[i];
            if (!updated && labelSpinSliderCheckBoxGroup->getValue() == valueList[i]) {
                updateRegisterTable({ crIsoValueList[i].first });
                labelSpinSliderCheckBoxGroup->setValue(valueList[i]);
                updated = true;
            }
        }
        // write
        for (int i = 0; i < crIsoValueList.size(); ++i) {
            QUiDataMiddleware(TAB_NAME, QString("SatISOList[%1]").arg(i))
                .setCombinedRegisterValueList({ crIsoValueList[i] });
        }
    }
    read();
}

std::vector<CombinedRegister> QSaturation::combinedRegisterSubPageRecording(QString strModuleName, QString strUiName, std::shared_ptr<int> ptrCamera, QString strSubPage) 
{
    std::vector<UiInfo> uiInfo;
    uiInfo = DatabaseOrm::getUiInfoByModuleNameAndControlName(moduleName, strUiName.toStdString());
    std::vector<CombinedRegister> combinevector;
    for (auto i : uiInfo) {
        CombinedRegister combineReg
            = DatabaseOrm::getCombinedRegisterByModuleNameAndCameraIdAndCombinedName(
                strModuleName.toStdString(), ptrCamera, *(i.combinedName));
        combinevector.push_back(combineReg);
    }

    for (auto reg : combinevector)
    {
        auto reBitInfo = DatabaseOrm::getRegisterBitInfoByCombinedRegister(reg);
        for (int i = 0; i < reBitInfo.size(); i++)
        {
            regAddrModule.insert(reBitInfo[i].registerAddr);
        }
        std::vector<uint32_t>().swap(registerMoudle);
        for (auto iter : regAddrModule)
        {
            registerMoudle.emplace_back(iter);
        }
        setRegisterSubPage(reg, strSubPage);
    }

    return combinevector;
}

std::vector<CombinedRegister> QSaturation::getCombinedRegister(QString strModuleName, QString strUiName, std::shared_ptr<int> ptrCamera)
{
    std::vector<UiInfo> uiInfo;
    uiInfo = DatabaseOrm::getUiInfoByModuleNameAndControlName(moduleName, strUiName.toStdString());
    std::vector<CombinedRegister> combinevector;
    for (auto i : uiInfo) {
        CombinedRegister combineReg
            = DatabaseOrm::getCombinedRegisterByModuleNameAndCameraIdAndCombinedName(
                strModuleName.toStdString(), ptrCamera, *(i.combinedName));
        combinevector.push_back(combineReg);
    }
    return combinevector;
}

void QSaturation::recordUIRegSubPageInfo()
{
    // 记录Curve tab的控件寄存器信息
    if (gainSatEnable) {
        combinedRegisterSubPageRecording("CURVE", gainSatEnable->Label(), std::make_shared<int>(getCurrentCamera()), "Curve");
    }
    
    if (saturation) {
        combinedRegisterSubPageRecording("CURVE", saturation->Label(), std::make_shared<int>(getCurrentCamera()), "Curve");
    }
    
    for (int i = 0; i < 8; i++) {
        if (satIsoList[i]) {
            combinedRegisterSubPageRecording("CURVE", satIsoList[i]->Label(), std::make_shared<int>(getCurrentCamera()), "Curve");
        }
    }
    qDebug() << "QSaturation::recordUIRegSubPageInfo record Curve tab done";
    // 记录YUVDNS tab的控件寄存器信息
    for (int i = 0; i < 8; i++) {
        if (gainNode[i]) {
            QString label = gainNode[i]->Label().remove("*");
            qDebug() << "QSaturation::recordUIRegSubPageInfo record GainNode: " << label;
            auto vecRegs = combinedRegisterSubPageRecording("YUVDNS", label, std::make_shared<int>(getCurrentCamera()), "YUVDNS");
            qDebug() << "QSaturation::recordUIRegSubPageInfo record GainNode done";
            if (SATURATION_RGBIR_ENABLE_MAP.contains({"Saturation", label})) {
                auto vecIrRegs = getCombinedRegister("YUVDNS_IR_AB", label + "_IR_AB", std::make_shared<int>(getCurrentCamera()));
                qDebug() << "QSaturation::recordUIRegSubPageInfo get IR regs done, vecIrRegs.size(): " << vecIrRegs.size();
                auto vecRgbRegs = getCombinedRegister("YUVDNS_RGB_AB", label + "_RGB_AB", std::make_shared<int>(getCurrentCamera()));
                qDebug() << "QSaturation::recordUIRegSubPageInfo get RGB regs done, vecRgbRegs.size(): " << vecRgbRegs.size();
                setRegisterTypeMap(vecRegs, vecRgbRegs, vecIrRegs);
            }
        }
    }
    qDebug() << "QSaturation::recordUIRegSubPageInfo record YUVDNS tab GainNode done";
    
    if (satAdjEnable) {
        combinedRegisterSubPageRecording("YUVDNS", satAdjEnable->Label(), std::make_shared<int>(getCurrentCamera()), "YUVDNS");
    }
    qDebug() << "QSaturation::recordUIRegSubPageInfo record YUVDNS tab SatAdjEnable done";
    
    if (curGain) {
        combinedRegisterSubPageRecording("PipeCtrl", curGain->Label(), std::make_shared<int>(getCurrentCamera()), "YUVDNS");
    }
    qDebug() << "QSaturation::recordUIRegSubPageInfo record YUVDNS tab CurGain done";
    
    // 记录表格控件寄存器信息
    for (int i = 0; i < TABLE_HEADER_LIST.size(); i++) {
        combinedRegisterSubPageRecording("YUVDNS", TABLE_HEADER_LIST[i], std::make_shared<int>(getCurrentCamera()), "YUVDNS");
    }
    qDebug() << "QSaturation::recordUIRegSubPageInfo record YUVDNS tab TableList done";
    
    // 记录曲线控件寄存器信息
    combinedRegisterSubPageRecording("CURVE", "SatStrList", std::make_shared<int>(getCurrentCamera()), "Curve");
    
    // 记录Current Gain L和Current Expo L寄存器信息
    combinedRegisterSubPageRecording("PipeCtrl", "Current Gain L", std::make_shared<int>(getCurrentCamera()), "Curve");
    combinedRegisterSubPageRecording("PipeCtrl", "Current Expo L", std::make_shared<int>(getCurrentCamera()), "Curve");

    // combinedRegisterSubPageRecording("CURVE", "SatStrList", std::make_shared<int>(getCurrentCamera()), "Curve");
    qDebug() << "QSaturation::recordUIRegSubPageInfo record YUVDNS tab Current Gain L and Current Expo L done";
}

void QSaturation::recordUIRegister()
{
    combinedRegisterMap.clear();
    
    // 记录Curve tab的控件寄存器信息
    if (gainSatEnable) {
        recordCombineRegbyUIifo("CURVE", gainSatEnable->Label(), std::make_shared<int>(getCurrentCamera()));
    }
    
    if (saturation) {
        recordCombineRegbyUIifo("CURVE", saturation->Label(), std::make_shared<int>(getCurrentCamera()));
    }
    
    for (int i = 0; i < 8; i++) {
        if (satIsoList[i]) {
            recordCombineRegbyUIifo("CURVE", satIsoList[i]->Label(), std::make_shared<int>(getCurrentCamera()));
        }
    }
    
    // 记录YUVDNS tab的控件寄存器信息
    for (int i = 0; i < 8; i++) {
        if (gainNode[i]) {
            QString label = gainNode[i]->Label().remove("*");
            // qDebug() << "QSaturation::recordUIRegister record GainNode: " << label;
            recordCombineRegbyUIifo("YUVDNS", label, std::make_shared<int>(getCurrentCamera()));
            // qDebug() << "QSaturation::recordUIRegister record GainNode done";
            if (SATURATION_RGBIR_ENABLE_MAP.contains({"Saturation", label})) {
                recordCombineRegbyUIifo("YUVDNS_IR_AB", label + "_IR_AB", std::make_shared<int>(getCurrentCamera()));
                recordCombineRegbyUIifo("YUVDNS_RGB_AB", label + "_RGB_AB", std::make_shared<int>(getCurrentCamera()));
            }
        }
    }
    
    if (satAdjEnable) {
        recordCombineRegbyUIifo("YUVDNS", satAdjEnable->Label(), std::make_shared<int>(getCurrentCamera()));
    }
    
    if (curGain) {
        recordCombineRegbyUIifo("PipeCtrl", curGain->Label(), std::make_shared<int>(getCurrentCamera()));
    }
    
    // 记录表格控件寄存器信息
    for (int i = 0; i < TABLE_HEADER_LIST.size(); i++) {
        recordCombineRegbyUIifo("YUVDNS", TABLE_HEADER_LIST[i], std::make_shared<int>(getCurrentCamera()));
    }
    
    // 记录曲线控件寄存器信息
    recordCombineRegbyUIifo("CURVE", "SatStrList", std::make_shared<int>(getCurrentCamera()));
    
    // 记录Current Gain L和Current Expo L寄存器信息
    recordCombineRegbyUIifo("PipeCtrl", "Current Gain L", std::make_shared<int>(getCurrentCamera()));
    recordCombineRegbyUIifo("PipeCtrl", "Current Expo L", std::make_shared<int>(getCurrentCamera()));
}

template <typename T>
void QSaturation::updateUIValue(QPointer<T>* vector, int iLength, const QString &extraname)
{
    int valTemp = 0;
    QUsbCommunication::getInstance().readBurst(IR_RGB_MODE_REG_ADDR, (unsigned char*)(&valTemp), ADDRESS_LENGTH, VALUE_LENGTH);
    for(int i = 0; i < iLength; i++)
    {
        uint32_t value=0;
        QString strLabelOrigin = vector[i]->Label().remove("*") + extraname;
        auto reg = combinedRegisterMap.value(strLabelOrigin);
        if (SATURATION_RGBIR_ENABLE_MAP.contains({moduleName.c_str(), strLabelOrigin}) && valTemp == 0x01) 
        {
            std::string strSuffix = strRgbIrMode == "RGB" ? "_RGB_AB" : "_IR_AB";
            reg = combinedRegisterMap.value(strLabelOrigin + strSuffix.c_str());
        }
        
        for(int j=0;j<reg.size();j++)
        {
            readCombinedRegisterValue(reg.at(j), value);
            if (std::is_same<T, QLabelEditGroup>::value)
                dynamic_cast<QLabelEditGroup *>(vector[i].get())->setValue(QString("0x%1").arg(QString::number(value,16).toUpper().rightJustified(VALUE_HEX_INFO,'0')));
            else if (std::is_same<T, QLabelCheckBox>::value)
            {
                if (value == 0)
                    dynamic_cast<QLabelCheckBox* >(vector[i].get())->setChecked(false);
                else if (value == 1)
                    dynamic_cast<QLabelCheckBox* >(vector[i].get())->setChecked(true);
            }
        }
    }
    // {
    //     uint32_t value=0;
    //     QString strLabelOrigin = item->Label().remove("*") + extraname;
    //     auto reg = combinedRegisterMap.value(strLabelOrigin);
    //     if (SATURATION_RGBIR_ENABLE_MAP.contains({moduleName.c_str(), strLabelOrigin})) 
    //     {
    //         // int valTemp = 0;
    //         // QUsbCommunication::getInstance().readBurst(IR_RGB_MODE_REG_ADDR, (unsigned char*)(&valTemp), ADDRESS_LENGTH, VALUE_LENGTH);

    //         std::string strSuffix = strRgbIrMode == "RGB" ? "_RGB_AB" : "_IR_AB";
    //         reg = combinedRegisterMap.value(strLabelOrigin + strSuffix.c_str());
    //     }
        
    //     for(int i=0;i<reg.size();i++)
    //     {
    //         readCombinedRegisterValue(reg.at(i), value);
    //         item->setValue(QString("0x%1").arg(QString::number(value,16).toUpper().rightJustified(VALUE_HEX_INFO,'0')));
    //     }
    // }
}

void QSaturation::changeTab(int index)
{
    switch (index) {
    case 0: // Curve tab
        updateUIValue<QLabelEditGroup>(&saturation, 1, "");
        updateUIValue<QLabelEditGroup>(satIsoList, 8, "");
        updateUIValue<QLabelCheckBox>(&gainSatEnable, 1, "");
        readCurve();
        break;
    case 1: // YUVDNS tab
        updateUIValue<QLabelEditGroup>(gainNode, 8, "");
        updateUIValue<QLabelCheckBox>(&satAdjEnable, 1, "");
        updateUIValue<QLabelEditGroup>(&curGain, 1, "");
        // 更新表格数据
        if (tableList) {
            for (int i = 0; i < TABLE_HEADER_LIST.size(); i++) {
                auto uiDataMiddleware = QUiDataMiddleware(TAB_NAME, TABLE_HEADER_LIST[i]);
                auto crValueList      = uiDataMiddleware.getCombinedRegisterValueList();
                for (int j = 0; j < crValueList.size(); j++) {
                    tableList->getItem(j, i)->setText(QString("0x%1").arg(crValueList[j].second, 2, 16, QChar('0')));
                }
            }
        }
        break;
    default:
        break;
    }
}

void QSaturation::recvPointsChanged(std::vector<QPoint> points)
{
    auto uiDataMiddlewareStr = QUiDataMiddleware(TAB_NAME, "SatStrList");
    std::sort(points.begin(), points.end(), [](const QPoint &a, const QPoint &b) { return a.x() < b.x(); });
    auto strCrValueList = uiDataMiddlewareStr.getCombinedRegisterValueList();
    for (int i = 0; i < points.size(); ++i) {
        strCrValueList[i].second = points[i].y();
    }
    uiDataMiddlewareStr.setCombinedRegisterValueList(strCrValueList);
    read();
}

void QSaturation::readCurve()
{
    std::vector<std::pair<DatabaseOrm::CombinedRegister, uint32_t>> crIsoValueList;
    for (int i = 0; i < 8; ++i) {
        auto uiDataMiddlewareIso = QUiDataMiddleware(TAB_NAME, QString("SatISOList[%1]").arg(i));
        auto crValueList         = uiDataMiddlewareIso.getCombinedRegisterValueList();
        crIsoValueList.push_back(crValueList[0]);
    }
    auto                uiDataMiddlewareStr = QUiDataMiddleware(TAB_NAME, "SatStrList");
    auto                crStrValueList      = uiDataMiddlewareStr.getCombinedRegisterValueList();
    std::vector<QPoint> pointList;
    for (int i = 0; i < crIsoValueList.size(); ++i) {
        pointList.push_back(QPoint(crIsoValueList[i].second, crStrValueList[i].second));
    }
    disconnect(isoSatCurve, &QCurveChartGroup::signalPointChanged, this, nullptr);
    disconnect(isoSatCurve, &QCurveChartGroup::signalAllPointsChanged, this, nullptr);
    isoSatCurve->setPoints(pointList);
    connect(
        isoSatCurve, &QCurveChartGroup::signalPointChanged, this,
        [=](QPoint p) {
            updateRegisterTable({ QUiDataMiddleware(TAB_NAME, "SatStrList")
                                      .getCombinedRegisterList()[isoSatCurve->getHighlightPointIndex()] });
        },
        Qt::QueuedConnection);
    connect(isoSatCurve, &QCurveChartGroup::signalAllPointsChanged, this, &QSaturation::recvPointsChanged);
}

void QSaturation::recvTableListItem(QStandardItem *citem, int row, int col)
{
    auto currentHeader    = TABLE_HEADER_LIST[col];
    auto uiDataMiddleware = QUiDataMiddleware(TAB_NAME, currentHeader);
    auto crList           = uiDataMiddleware.getCombinedRegisterList();
    updateRegisterTable({ crList[row] });
}
