#include "qtonemappingtab.h"
#include "Common/UI/quidatamiddleware.h"
#include <QComboBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <cstdint>
#include <functional>
#include <vector>
#include "Common/UI/qlabelcheckbox.h"

const QString        TAB_NAME            = "TM";
const QList<QString> TAB_NAME_LIST       = { "TM", "GTM Hist", "LTM", "GTM", "Motion"/*, "Fisheye" */};
const QList<QString> GTM_HIST_CURVE_NAME = { "HistEQList", "CurveStep1", "CurveStep2" };
const QMap<QString, QList<QList<QPair<QString, QString>>>> CONTROL_MAP = {
    { "TM",
        { { { "QLabelEditGroup", "TM Option" }, { "QLabelCheckBox", "Manual DR Mode" } },
            { { "QLabelEditGroup", "Manual DR" }, { "QLabelEditGroup", "DR List[0]" },
                { "QLabelEditGroup", "DR List[1]" }, { "QLabelEditGroup", "DR List[2]" },
                { "QLabelEditGroup", "DR List[3]" }, { "QLabelEditGroup", "DR List[4]" } } } },
    { "GTM Hist",
        { { { "QLabelCheckBox", "UpDateGTM" }, { "QLabelCheckBox", "HistEQEnable" },
              { "QLabelEditGroup", "HistPointStepList[0]" }, { "QLabelEditGroup", "HistPointStepList[1]" },
              { "QLabelEditGroup", "HistPointStepList[2]" }, { "QLabelEditGroup", "HistPointStepList[3]" },
              { "QLabelEditGroup", "HistPointStepList[4]" } },
            { { "QLabelEditGroup", "GainThre1" }, { "QLabelEditGroup", "GainThre2" },
                { "QLabelEditGroup", "CurveAlpha[0]" }, { "QLabelEditGroup", "CurveAlpha[1]" },
                { "QLabelEditGroup", "CurveAlpha[2]" }, { "QLabelEditGroup", "CurveAlpha[3]" },
                { "QLabelEditGroup", "CurveAlpha[4]" } } } },
    { "LTM",
        { { { "QLabelCheckBox", "LTMEnable" }, { "QLabelCheckBox", "UpDateLTM" }, { "QLabelEditGroup", "SmoothOption" },
              { "QLabelCheckBox", "ClipLogGain" }, { "QLabelEditGroup", "LogGainCursLimit" },
              { "QLabelEditGroup", "LogGainLimit" } },
            { { "QLabelEditGroup", "LTMAlpha2" }, { "QLabelEditGroup", "LocalGammaAlphaList[0]" },
                { "QLabelEditGroup", "LocalGammaAlphaList[1]" }, { "QLabelEditGroup", "LocalGammaAlphaList[2]" },
                { "QLabelEditGroup", "LocalGammaAlphaList[3]" }, { "QLabelEditGroup", "LocalGammaAlphaList[4]" } },
            { { "", "" }, { "QLabelEditGroup", "LocalGammaStepList[0]" },
                { "QLabelEditGroup", "LocalGammaStepList[1]" }, { "QLabelEditGroup", "LocalGammaStepList[2]" },
                { "QLabelEditGroup", "LocalGammaStepList[3]" }, { "QLabelEditGroup", "LocalGammaStepList[4]" } } } },
    { "GTM",
        { { { "QLabelCheckBox", "GammaAdjustEnable" }, { "QLabelEditGroup", "GammaAdjustThre" },
              { "QLabelEditGroup", "GlobalGammaList[0]" }, { "QLabelEditGroup", "GlobalGammaList[1]" },
              { "QLabelEditGroup", "GlobalGammaList[2]" }, { "QLabelEditGroup", "GlobalGammaList[3]" },
              { "QLabelEditGroup", "GlobalGammaList[4]" } },
            { { "QLabelEditGroup", "LogTarget" }, { "QLabelEditGroup", "TargetStep" },
                { "QLabelEditGroup", "MaxGammaList[0]" }, { "QLabelEditGroup", "MaxGammaList[1]" },
                { "QLabelEditGroup", "MaxGammaList[2]" }, { "QLabelEditGroup", "MaxGammaList[3]" },
                { "QLabelEditGroup", "MaxGammaList[4]" } },
            { { "QLabelEditGroup", "GammaAdjustStep" }, { "", "" }, { "QLabelEditGroup", "MinGammaList[0]" },
                { "QLabelEditGroup", "MinGammaList[1]" }, { "QLabelEditGroup", "MinGammaList[2]" },
                { "QLabelEditGroup", "MinGammaList[3]" }, { "QLabelEditGroup", "MinGammaList[4]" } } } },
    { "Motion",
        { { { "QLabelEditGroup", "MotionOpt" }, { "QLabelEditGroup", "MotionStartFrame" } },
            { { "QLabelEditGroup", "MotionCoeff[0]" }, { "QLabelEditGroup", "MotionCoeff[1]" },
                { "QLabelEditGroup", "MotionCoeff[2]" }, { "QLabelEditGroup", "MotionCoeff[3]" },
                { "QLabelEditGroup", "MotionCoeff[4]" } },
            { { "QLabelEditGroup", "MotionWeight[0]" }, { "QLabelEditGroup", "MotionWeight[1]" },
                { "QLabelEditGroup", "MotionWeight[2]" }, { "QLabelEditGroup", "MotionWeight[3]" },
                { "QLabelEditGroup", "MotionWeight[4]" } },
            { { "QLabelEditGroup", "MotionGammaStep[0]" }, { "QLabelEditGroup", "MotionGammaStep[1]" },
                { "QLabelEditGroup", "MotionGammaStep[2]" }, { "", "" }, { "QLabelEditGroup", "MotionThre" } } } },
    // { "Fisheye",
    //     { { { "QLabelEditGroup", "Fish Len Opt" }, { "QLabelEditGroup", "Fisheye Radius" },
    //         { "QLabelEditGroup", "Fisheye Center X" }, { "QLabelEditGroup", "Fisheye Center Y" } } } }
};

QToneMappingTab::QToneMappingTab(QWidget *parent)
    : QTabWndComm(parent)
    , tabWidget(new QTabWidget(this))
{
    moduleName = TAB_NAME.toStdString();
    this->init();
    this->initConnect();
}

QToneMappingTab::~QToneMappingTab() { }

void QToneMappingTab::init()
{
    addTabWidget(tabWidget);
    std::map<QString, std::function<void(QWidget *)>> initFuncList= {
        { "TM", std::bind(&QToneMappingTab::initTMSubTab, this, std::placeholders::_1) },
        { "GTM Hist", std::bind(&QToneMappingTab::initGTMHistSubTab, this, std::placeholders::_1) },
        { "LTM", std::bind(&QToneMappingTab::initLTMSubTab, this, std::placeholders::_1) },
        { "GTM", std::bind(&QToneMappingTab::initGTMSubTab, this, std::placeholders::_1) },
        { "Motion", std::bind(&QToneMappingTab::initMotionSubTab, this, std::placeholders::_1) },
        { "Fisheye", std::bind(&QToneMappingTab::initFisheyeSubTab, this, std::placeholders::_1) }
    };
    if(m_nLevel!=LEVEL_ADMINISTRATOR)
    {
        QList<QString>* pTemp = (QList<QString>*)&TAB_NAME_LIST;
        *pTemp={ "TM", "LTM", "GTM"};
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
    recordUIRegSubPageInfo();
    addRegisterTable();
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

void QToneMappingTab::initConnect()
{
    connect(tabWidget.get(), &QTabWidget::currentChanged, this, &QToneMappingTab::changeTab, Qt::QueuedConnection);
    connect(gtmHistCurveCombo, &QComboBox::currentTextChanged, this, &QToneMappingTab::gtmHistComboStringChanged,
        Qt::QueuedConnection);
    connect(gtmHistCurve, &QCurveChartGroup::signalPointChanged, this, &QToneMappingTab::recvPointChanged,
        Qt::QueuedConnection);
    connect(this, &QTabWndComm::readSignal, this, [=](){updateValue();});
    connect(this, &QTabWndComm::updateValue, this, &QToneMappingTab::updateValue, Qt::QueuedConnection);
    connect(
        this, &QTabWndComm::reConnectRead, this,
        [=]() {
            recordUIRegister();
            updateValue();
        },
        Qt::QueuedConnection);
}

std::vector<CombinedRegister> QToneMappingTab::combinedRegisterSubPageRecording(QString strModuleName, QString strUiName, std::shared_ptr<int> ptrCamera, QString strSubPage) 
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

std::vector<CombinedRegister> QToneMappingTab::getCombinedRegister(QString strModuleName, QString strUiName, std::shared_ptr<int> ptrCamera)
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

void QToneMappingTab::recordUIRegSubPageInfo()
{
    // 遍历所有子页面和控件，记录寄存器信息
    for (const auto &tabName : TAB_NAME_LIST) {
        // 处理LabelEditGroup控件
        if (labelEditGroups.contains(tabName)) {
            for (auto item : labelEditGroups[tabName]) {
                QString label = item->Label();
                combinedRegisterSubPageRecording(TAB_NAME, label, std::make_shared<int>(getCurrentCamera()), tabName);
            }
        }
        
        // 处理LabelCheckBox控件
        if (labelCheckBoxes.contains(tabName)) {
            for (auto item : labelCheckBoxes[tabName]) {
                QString label = item->Label();
                combinedRegisterSubPageRecording(TAB_NAME, label, std::make_shared<int>(getCurrentCamera()), tabName);
            }
        }
    }
    
    // 记录GTM Hist曲线控件寄存器信息
    for (auto &name : GTM_HIST_CURVE_NAME) {
        auto uiDataMiddleware = QUiDataMiddleware(TAB_NAME, name);
        auto crValueList      = uiDataMiddleware.getCombinedRegisterList();
        for (auto iter : crValueList)
        {
            setRegisterSubPage(iter, "GTM Hist");
        }
    }
}

void QToneMappingTab::recordUIRegister()
{
    combinedRegisterMap.clear();
    
    // 遍历所有子页面和控件，记录寄存器信息
    for (const auto &tabName : TAB_NAME_LIST) {
        // 处理LabelEditGroup控件
        if (labelEditGroups.contains(tabName)) {
            for (auto item : labelEditGroups[tabName]) {
                QString label = item->Label();
                recordCombineRegbyUIifo(TAB_NAME, label, std::make_shared<int>(getCurrentCamera()));
            }
        }
        
        // 处理LabelCheckBox控件
        if (labelCheckBoxes.contains(tabName)) {
            for (auto item : labelCheckBoxes[tabName]) {
                QString label = item->Label();
                recordCombineRegbyUIifo(TAB_NAME, label, std::make_shared<int>(getCurrentCamera()));
            }
        }
    }
    
    // 记录GTM Hist曲线控件寄存器信息
    for (auto &name : GTM_HIST_CURVE_NAME) {
        recordCombineRegbyUIifo(TAB_NAME, name, std::make_shared<int>(getCurrentCamera()));
    }
}

template <typename T>
void QToneMappingTab::updateUIValue(T& vector)
{
    for(auto item:vector)
    {
        uint32_t value=0;
        QString strLabel = item->Label();
        auto reg = combinedRegisterMap.value(strLabel);
        
        for(int i=0;i<reg.size();i++)
        {
            readCombinedRegisterValue(reg.at(i), value);
            if (dynamic_cast<QLabelEditGroup*>(item.data())) {
                dynamic_cast<QLabelEditGroup*>(item.data())->setValue(QString("0x%1").arg(QString::number(value,16).toUpper().rightJustified(VALUE_HEX_INFO,'0')));
            } else if (dynamic_cast<QLabelCheckBox*>(item.data())) {
                if (value == 0)
                    dynamic_cast<QLabelCheckBox*>(item.data())->setChecked(false);
                else if (value == 1)
                    dynamic_cast<QLabelCheckBox*>(item.data())->setChecked(true);
            }
        }
    }
}

void QToneMappingTab::updateValue()
{
    changeTab(tabWidget->currentIndex());
}

void QToneMappingTab::initTMSubTab(QPointer<QWidget> subTab)
{
    QPointer<QGridLayout> layout = new QGridLayout(subTab);
    layout->setAlignment(Qt::AlignLeft);
    subTab->setLayout(layout);
    layout->setContentsMargins(0,0,0,0);
    layout->setVerticalSpacing(1);
    
    // 创建TM子页面的控件
    QList<QPointer<QLabelEditGroup>> tmLabelEdits;
    QList<QPointer<QLabelCheckBox>> tmLabelCheckBoxes;
    
    // 第一列控件
    QPointer<QLabelEditGroup> tmOption = new QLabelEditGroup();
    tmOption->setLabelValue("TM Option");
    tmOption->setEditable(false);
    tmOption->setlabelWidth(130);
    tmOption->setMouseTracking(true);
    layout->addWidget(tmOption, 0, 0);
    tmLabelEdits.append(tmOption);
    
    QPointer<QLabelCheckBox> manualDRMode = new QLabelCheckBox();
    manualDRMode->setLabel("Manual DR Mode");
    manualDRMode->setMouseTracking(true);
    layout->addWidget(manualDRMode, 1, 0);
    tmLabelCheckBoxes.append(manualDRMode);
    
    // 第二列控件
    QPointer<QLabelEditGroup> manualDR = new QLabelEditGroup();
    manualDR->setLabelValue("Manual DR");
    manualDR->setEditable(false);
    manualDR->setlabelWidth(130);
    manualDR->setMouseTracking(true);
    layout->addWidget(manualDR, 0, 1);
    tmLabelEdits.append(manualDR);
    
    // DR List控件
    for (int i = 0; i < 5; i++) {
        QPointer<QLabelEditGroup> drList = new QLabelEditGroup();
        drList->setLabelValue(QString("DR List[%1]").arg(i));
        drList->setEditable(false);
        drList->setlabelWidth(130);
        drList->setMouseTracking(true);
        layout->addWidget(drList, i + 1, 1);
        tmLabelEdits.append(drList);
    }
    
    layout->setColumnStretch(0, 1);
    layout->setColumnStretch(1, 1);
    
    // 存储控件引用
    labelEditGroups["TM"] = tmLabelEdits;
    labelCheckBoxes["TM"] = tmLabelCheckBoxes;
    
    // 连接信号
    for (auto item : tmLabelEdits) {
        connect(item, &QLabelEditGroup::releaseLabel, this, [=](){QTabWndComm::recvEdit(item->Label());});
        connect(item, &QLabelEditGroup::signalValueChange, this, 
            [=](uint32_t value)
            {
                std::vector<CombinedRegister> re = combinedRegisterMap.value(item->Label());
                for (auto it : re) 
                {
                    writeCombinedRegisterValue(it, value);
                }
            });
    }
    
    for (auto item : tmLabelCheckBoxes) {
        connect(item, &QLabelCheckBox::release, this, [=](){QTabWndComm::recvEdit(item->Label());});
        connect(item, &QLabelCheckBox::checkSignal, this, 
            [=](){
                uint32_t value = item->checkState() ? 1 : 0;
                std::vector<CombinedRegister> re = combinedRegisterMap.value(item->Label());
                for (auto it : re) 
                {
                    writeCombinedRegisterValue(it, value);
                }
            });
    }
}

void QToneMappingTab::initGTMHistSubTab(QPointer<QWidget> subTab)
{
    QPointer<QGridLayout> layout = new QGridLayout(subTab);
    layout->setAlignment(Qt::AlignLeft);
    subTab->setLayout(layout);
    layout->setContentsMargins(0,0,0,0);
    layout->setVerticalSpacing(1);
    
    // 创建GTM Hist子页面的控件
    QList<QPointer<QLabelEditGroup>> gtmHistLabelEdits;
    QList<QPointer<QLabelCheckBox>> gtmHistLabelCheckBoxes;
    
    // 第一列控件
    QPointer<QLabelCheckBox> updateGTM = new QLabelCheckBox();
    updateGTM->setLabel("UpDateGTM");
    updateGTM->setMouseTracking(true);
    layout->addWidget(updateGTM, 0, 0);
    gtmHistLabelCheckBoxes.append(updateGTM);
    
    QPointer<QLabelCheckBox> histEQEnable = new QLabelCheckBox();
    histEQEnable->setLabel("HistEQEnable");
    histEQEnable->setMouseTracking(true);
    layout->addWidget(histEQEnable, 1, 0);
    gtmHistLabelCheckBoxes.append(histEQEnable);
    
    // HistPointStepList控件
    for (int i = 0; i < 5; i++) {
        QPointer<QLabelEditGroup> histPointStep = new QLabelEditGroup();
        histPointStep->setLabelValue(QString("HistPointStepList[%1]").arg(i));
        histPointStep->setEditable(false);
        histPointStep->setlabelWidth(130);
        histPointStep->setMouseTracking(true);
        layout->addWidget(histPointStep, i + 2, 0);
        gtmHistLabelEdits.append(histPointStep);
    }
    
    // 第二列控件
    QPointer<QLabelEditGroup> gainThre1 = new QLabelEditGroup();
    gainThre1->setLabelValue("GainThre1");
    gainThre1->setEditable(false);
    gainThre1->setlabelWidth(130);
    gainThre1->setMouseTracking(true);
    layout->addWidget(gainThre1, 0, 1);
    gtmHistLabelEdits.append(gainThre1);
    
    QPointer<QLabelEditGroup> gainThre2 = new QLabelEditGroup();
    gainThre2->setLabelValue("GainThre2");
    gainThre2->setEditable(false);
    gainThre2->setlabelWidth(130);
    gainThre2->setMouseTracking(true);
    layout->addWidget(gainThre2, 1, 1);
    gtmHistLabelEdits.append(gainThre2);
    
    // CurveAlpha控件
    for (int i = 0; i < 5; i++) {
        QPointer<QLabelEditGroup> curveAlpha = new QLabelEditGroup();
        curveAlpha->setLabelValue(QString("CurveAlpha[%1]").arg(i));
        curveAlpha->setEditable(false);
        curveAlpha->setlabelWidth(130);
        curveAlpha->setMouseTracking(true);
        layout->addWidget(curveAlpha, i + 2, 1);
        gtmHistLabelEdits.append(curveAlpha);
    }
    
    // 曲线图控件
    QPointer<QGroupBox> group = new QGroupBox(subTab);
    layout->setColumnStretch(2, 2);
    layout->addWidget(group, 0, 2, 7, 1);
    group->setLayout(new QVBoxLayout(group));
    group->layout()->setAlignment(Qt::AlignLeft);
    group->layout()->setSpacing(0);
    gtmHistCurveCombo = new QComboBox(group);
    for (auto &name : GTM_HIST_CURVE_NAME) {
        gtmHistCurveCombo->addItem(name);
    }
    gtmHistCurveCombo->setCurrentIndex(0);
    group->layout()->addWidget(gtmHistCurveCombo);
    gtmHistCurve = new QCurveChartGroup(group);
    gtmHistCurve->setXMax(30);
    gtmHistCurve->setYMax(255);
    gtmHistCurve->setAxisRange(0, 30, 0, 255);
    gtmHistCurve->setAutoAdjustAxisRange(false);
    gtmHistCurve->setCurveFittingType(CurveFittingType::NONE);

    for (auto &name : GTM_HIST_CURVE_NAME) {
        auto uiDataMiddleware = QUiDataMiddleware(TAB_NAME, name);
        auto crValueList      = uiDataMiddleware.getCombinedRegisterList();
        for (auto iter : crValueList)
        {
            setRegisterSubPage(iter, "GTM Hist");
        }
    }
    
    group->layout()->addWidget(gtmHistCurve);
    gtmHistComboStringChanged(gtmHistCurveCombo->currentText());
    
    layout->setColumnStretch(0, 1);
    layout->setColumnStretch(1, 1);
    
    // 存储控件引用
    labelEditGroups["GTM Hist"] = gtmHistLabelEdits;
    labelCheckBoxes["GTM Hist"] = gtmHistLabelCheckBoxes;
    
    // 连接信号
    for (auto item : gtmHistLabelEdits) {
        connect(item, &QLabelEditGroup::releaseLabel, this, [=](){QTabWndComm::recvEdit(item->Label());});
        connect(item, &QLabelEditGroup::signalValueChange, this, 
            [=](uint32_t value)
            {
                std::vector<CombinedRegister> re = combinedRegisterMap.value(item->Label());
                for (auto it : re) 
                {
                    writeCombinedRegisterValue(it, value);
                }
            });
    }
    
    for (auto item : gtmHistLabelCheckBoxes) {
        connect(item, &QLabelCheckBox::release, this, [=](){QTabWndComm::recvEdit(item->Label());});
        connect(item, &QLabelCheckBox::checkSignal, this, 
            [=](){
                uint32_t value = item->checkState() ? 1 : 0;
                std::vector<CombinedRegister> re = combinedRegisterMap.value(item->Label());
                for (auto it : re) 
                {
                    writeCombinedRegisterValue(it, value);
                }
            });
    }
}

void QToneMappingTab::initLTMSubTab(QPointer<QWidget> subTab)
{
    QPointer<QGridLayout> layout = new QGridLayout(subTab);
    layout->setAlignment(Qt::AlignLeft);
    subTab->setLayout(layout);
    layout->setContentsMargins(0,0,0,0);
    layout->setVerticalSpacing(1);
    
    // 创建LTM子页面的控件
    QList<QPointer<QLabelEditGroup>> ltmLabelEdits;
    QList<QPointer<QLabelCheckBox>> ltmLabelCheckBoxes;
    
    // 第一列控件
    QPointer<QLabelCheckBox> ltmEnable = new QLabelCheckBox();
    ltmEnable->setLabel("LTMEnable");
    ltmEnable->setMouseTracking(true);
    layout->addWidget(ltmEnable, 0, 0);
    ltmLabelCheckBoxes.append(ltmEnable);
    
    QPointer<QLabelCheckBox> updateLTM = new QLabelCheckBox();
    updateLTM->setLabel("UpDateLTM");
    updateLTM->setMouseTracking(true);
    layout->addWidget(updateLTM, 1, 0);
    ltmLabelCheckBoxes.append(updateLTM);
    
    QPointer<QLabelEditGroup> smoothOption = new QLabelEditGroup();
    smoothOption->setLabelValue("SmoothOption");
    smoothOption->setEditable(false);
    smoothOption->setlabelWidth(130);
    smoothOption->setMouseTracking(true);
    layout->addWidget(smoothOption, 2, 0);
    ltmLabelEdits.append(smoothOption);
    
    QPointer<QLabelCheckBox> clipLogGain = new QLabelCheckBox();
    clipLogGain->setLabel("ClipLogGain");
    clipLogGain->setMouseTracking(true);
    layout->addWidget(clipLogGain, 3, 0);
    ltmLabelCheckBoxes.append(clipLogGain);
    
    QPointer<QLabelEditGroup> logGainCursLimit = new QLabelEditGroup();
    logGainCursLimit->setLabelValue("LogGainCursLimit");
    logGainCursLimit->setEditable(false);
    logGainCursLimit->setlabelWidth(130);
    logGainCursLimit->setMouseTracking(true);
    layout->addWidget(logGainCursLimit, 4, 0);
    ltmLabelEdits.append(logGainCursLimit);
    
    QPointer<QLabelEditGroup> logGainLimit = new QLabelEditGroup();
    logGainLimit->setLabelValue("LogGainLimit");
    logGainLimit->setEditable(false);
    logGainLimit->setlabelWidth(130);
    logGainLimit->setMouseTracking(true);
    layout->addWidget(logGainLimit, 5, 0);
    ltmLabelEdits.append(logGainLimit);
    
    // 第二列控件
    QPointer<QLabelEditGroup> ltmAlpha2 = new QLabelEditGroup();
    ltmAlpha2->setLabelValue("LTMAlpha2");
    ltmAlpha2->setEditable(false);
    ltmAlpha2->setlabelWidth(130);
    ltmAlpha2->setMouseTracking(true);
    layout->addWidget(ltmAlpha2, 0, 1);
    ltmLabelEdits.append(ltmAlpha2);
    
    // LocalGammaAlphaList控件
    for (int i = 0; i < 5; i++) {
        QPointer<QLabelEditGroup> localGammaAlpha = new QLabelEditGroup();
        localGammaAlpha->setLabelValue(QString("LocalGammaAlphaList[%1]").arg(i));
        localGammaAlpha->setEditable(false);
        localGammaAlpha->setlabelWidth(130);
        localGammaAlpha->setMouseTracking(true);
        layout->addWidget(localGammaAlpha, i + 1, 1);
        ltmLabelEdits.append(localGammaAlpha);
    }
    
    // 第三列控件 (LocalGammaStepList)
    // 空白占位
    // LocalGammaStepList控件
    for (int i = 0; i < 5; i++) {
        QPointer<QLabelEditGroup> localGammaStep = new QLabelEditGroup();
        localGammaStep->setLabelValue(QString("LocalGammaStepList[%1]").arg(i));
        localGammaStep->setEditable(false);
        localGammaStep->setlabelWidth(130);
        localGammaStep->setMouseTracking(true);
        layout->addWidget(localGammaStep, i + 1, 2);
        ltmLabelEdits.append(localGammaStep);
    }
    
    layout->setColumnStretch(0, 1);
    layout->setColumnStretch(1, 1);
    layout->setColumnStretch(2, 1);
    
    // 存储控件引用
    labelEditGroups["LTM"] = ltmLabelEdits;
    labelCheckBoxes["LTM"] = ltmLabelCheckBoxes;
    
    // 连接信号
    for (auto item : ltmLabelEdits) {
        connect(item, &QLabelEditGroup::releaseLabel, this, [=](){QTabWndComm::recvEdit(item->Label());});
        connect(item, &QLabelEditGroup::signalValueChange, this, 
            [=](uint32_t value)
            {
                std::vector<CombinedRegister> re = combinedRegisterMap.value(item->Label());
                for (auto it : re) 
                {
                    writeCombinedRegisterValue(it, value);
                }
            });
    }
    
    for (auto item : ltmLabelCheckBoxes) {
        connect(item, &QLabelCheckBox::release, this, [=](){QTabWndComm::recvEdit(item->Label());});
        connect(item, &QLabelCheckBox::checkSignal, this, 
            [=](){
                uint32_t value = item->checkState() ? 1 : 0;
                std::vector<CombinedRegister> re = combinedRegisterMap.value(item->Label());
                for (auto it : re) 
                {
                    writeCombinedRegisterValue(it, value);
                }
            });
    }
}

void QToneMappingTab::initGTMSubTab(QPointer<QWidget> subTab)
{
    QPointer<QGridLayout> layout = new QGridLayout(subTab);
    layout->setAlignment(Qt::AlignLeft);
    subTab->setLayout(layout);
    layout->setContentsMargins(0,0,0,0);
    layout->setVerticalSpacing(1);
    
    // 创建GTM子页面的控件
    QList<QPointer<QLabelEditGroup>> gtmLabelEdits;
    QList<QPointer<QLabelCheckBox>> gtmLabelCheckBoxes;
    
    // 第一列控件
    QPointer<QLabelCheckBox> gammaAdjustEnable = new QLabelCheckBox();
    gammaAdjustEnable->setLabel("GammaAdjustEnable");
    gammaAdjustEnable->setMouseTracking(true);
    layout->addWidget(gammaAdjustEnable, 0, 0);
    gtmLabelCheckBoxes.append(gammaAdjustEnable);
    
    QPointer<QLabelEditGroup> gammaAdjustThre = new QLabelEditGroup();
    gammaAdjustThre->setLabelValue("GammaAdjustThre");
    gammaAdjustThre->setEditable(false);
    gammaAdjustThre->setlabelWidth(130);
    gammaAdjustThre->setMouseTracking(true);
    layout->addWidget(gammaAdjustThre, 1, 0);
    gtmLabelEdits.append(gammaAdjustThre);
    
    // GlobalGammaList控件
    for (int i = 0; i < 5; i++) {
        QPointer<QLabelEditGroup> globalGamma = new QLabelEditGroup();
        globalGamma->setLabelValue(QString("GlobalGammaList[%1]").arg(i));
        globalGamma->setEditable(false);
        globalGamma->setlabelWidth(130);
        globalGamma->setMouseTracking(true);
        layout->addWidget(globalGamma, i + 2, 0);
        gtmLabelEdits.append(globalGamma);
    }
    
    // 第二列控件
    QPointer<QLabelEditGroup> logTarget = new QLabelEditGroup();
    logTarget->setLabelValue("LogTarget");
    logTarget->setEditable(false);
    logTarget->setlabelWidth(130);
    logTarget->setMouseTracking(true);
    layout->addWidget(logTarget, 0, 1);
    gtmLabelEdits.append(logTarget);
    
    QPointer<QLabelEditGroup> targetStep = new QLabelEditGroup();
    targetStep->setLabelValue("TargetStep");
    targetStep->setEditable(false);
    targetStep->setlabelWidth(130);
    targetStep->setMouseTracking(true);
    layout->addWidget(targetStep, 1, 1);
    gtmLabelEdits.append(targetStep);
    
    // MaxGammaList控件
    for (int i = 0; i < 5; i++) {
        QPointer<QLabelEditGroup> maxGamma = new QLabelEditGroup();
        maxGamma->setLabelValue(QString("MaxGammaList[%1]").arg(i));
        maxGamma->setEditable(false);
        maxGamma->setlabelWidth(130);
        maxGamma->setMouseTracking(true);
        layout->addWidget(maxGamma, i + 2, 1);
        gtmLabelEdits.append(maxGamma);
    }
    
    // 第三列控件
    QPointer<QLabelEditGroup> gammaAdjustStep = new QLabelEditGroup();
    gammaAdjustStep->setLabelValue("GammaAdjustStep");
    gammaAdjustStep->setEditable(false);
    gammaAdjustStep->setlabelWidth(130);
    gammaAdjustStep->setMouseTracking(true);
    layout->addWidget(gammaAdjustStep, 0, 2);
    gtmLabelEdits.append(gammaAdjustStep);
    
    // MinGammaList控件
    for (int i = 0; i < 5; i++) {
        QPointer<QLabelEditGroup> minGamma = new QLabelEditGroup();
        minGamma->setLabelValue(QString("MinGammaList[%1]").arg(i));
        minGamma->setEditable(false);
        minGamma->setlabelWidth(130);
        minGamma->setMouseTracking(true);
        layout->addWidget(minGamma, i + 2, 2);
        gtmLabelEdits.append(minGamma);
    }
    
    layout->setColumnStretch(0, 1);
    layout->setColumnStretch(1, 1);
    layout->setColumnStretch(2, 1);
    
    // 存储控件引用
    labelEditGroups["GTM"] = gtmLabelEdits;
    labelCheckBoxes["GTM"] = gtmLabelCheckBoxes;
    
    // 连接信号
    for (auto item : gtmLabelEdits) {
        connect(item, &QLabelEditGroup::releaseLabel, this, [=](){QTabWndComm::recvEdit(item->Label());});
        connect(item, &QLabelEditGroup::signalValueChange, this, 
            [=](uint32_t value)
            {
                std::vector<CombinedRegister> re = combinedRegisterMap.value(item->Label());
                for (auto it : re) 
                {
                    writeCombinedRegisterValue(it, value);
                }
            });
    }
    
    for (auto item : gtmLabelCheckBoxes) {
        connect(item, &QLabelCheckBox::release, this, [=](){QTabWndComm::recvEdit(item->Label());});
        connect(item, &QLabelCheckBox::checkSignal, this, 
            [=](){
                uint32_t value = item->checkState() ? 1 : 0;
                std::vector<CombinedRegister> re = combinedRegisterMap.value(item->Label());
                for (auto it : re) 
                {
                    writeCombinedRegisterValue(it, value);
                }
            });
    }
}

void QToneMappingTab::initMotionSubTab(QPointer<QWidget> subTab)
{
    QPointer<QGridLayout> layout = new QGridLayout(subTab);
    layout->setAlignment(Qt::AlignLeft);
    subTab->setLayout(layout);
    layout->setContentsMargins(0,0,0,0);
    layout->setVerticalSpacing(1);
    
    // 创建Motion子页面的控件
    QList<QPointer<QLabelEditGroup>> motionLabelEdits;
    QList<QPointer<QLabelCheckBox>> motionLabelCheckBoxes;
    
    // 第一列控件
    QPointer<QLabelEditGroup> motionOpt = new QLabelEditGroup();
    motionOpt->setLabelValue("MotionOpt");
    motionOpt->setEditable(false);
    motionOpt->setlabelWidth(130);
    motionOpt->setMouseTracking(true);
    layout->addWidget(motionOpt, 0, 0);
    motionLabelEdits.append(motionOpt);
    
    QPointer<QLabelEditGroup> motionStartFrame = new QLabelEditGroup();
    motionStartFrame->setLabelValue("MotionStartFrame");
    motionStartFrame->setEditable(false);
    motionStartFrame->setlabelWidth(130);
    motionStartFrame->setMouseTracking(true);
    layout->addWidget(motionStartFrame, 1, 0);
    motionLabelEdits.append(motionStartFrame);
    
    // MotionCoeff控件
    for (int i = 0; i < 5; i++) {
        QPointer<QLabelEditGroup> motionCoeff = new QLabelEditGroup();
        motionCoeff->setLabelValue(QString("MotionCoeff[%1]").arg(i));
        motionCoeff->setEditable(false);
        motionCoeff->setlabelWidth(130);
        motionCoeff->setMouseTracking(true);
        layout->addWidget(motionCoeff, i + 2, 0);
        motionLabelEdits.append(motionCoeff);
    }
    
    // 第二列控件
    // MotionWeight控件
    for (int i = 0; i < 5; i++) {
        QPointer<QLabelEditGroup> motionWeight = new QLabelEditGroup();
        motionWeight->setLabelValue(QString("MotionWeight[%1]").arg(i));
        motionWeight->setEditable(false);
        motionWeight->setlabelWidth(130);
        motionWeight->setMouseTracking(true);
        layout->addWidget(motionWeight, i + 0, 1);
        motionLabelEdits.append(motionWeight);
    }
    
    // 第三列控件
    // MotionGammaStep控件
    for (int i = 0; i < 3; i++) {
        QPointer<QLabelEditGroup> motionGammaStep = new QLabelEditGroup();
        motionGammaStep->setLabelValue(QString("MotionGammaStep[%1]").arg(i));
        motionGammaStep->setEditable(false);
        motionGammaStep->setlabelWidth(130);
        motionGammaStep->setMouseTracking(true);
        layout->addWidget(motionGammaStep, i + 0, 2);
        motionLabelEdits.append(motionGammaStep);
    }
    
    // 空白占位
    QPointer<QLabelEditGroup> motionThre = new QLabelEditGroup();
    motionThre->setLabelValue("MotionThre");
    motionThre->setEditable(false);
    motionThre->setlabelWidth(130);
    motionThre->setMouseTracking(true);
    layout->addWidget(motionThre, 4, 2);
    motionLabelEdits.append(motionThre);
    
    layout->setColumnStretch(0, 1);
    layout->setColumnStretch(1, 1);
    layout->setColumnStretch(2, 1);
    
    // 存储控件引用
    labelEditGroups["Motion"] = motionLabelEdits;
    labelCheckBoxes["Motion"] = motionLabelCheckBoxes;
    
    // 连接信号
    for (auto item : motionLabelEdits) {
        connect(item, &QLabelEditGroup::releaseLabel, this, [=](){QTabWndComm::recvEdit(item->Label());});
        connect(item, &QLabelEditGroup::signalValueChange, this, 
            [=](uint32_t value)
            {
                std::vector<CombinedRegister> re = combinedRegisterMap.value(item->Label());
                for (auto it : re) 
                {
                    writeCombinedRegisterValue(it, value);
                }
            });
    }
    
    for (auto item : motionLabelCheckBoxes) {
        connect(item, &QLabelCheckBox::release, this, [=](){QTabWndComm::recvEdit(item->Label());});
        connect(item, &QLabelCheckBox::checkSignal, this, 
            [=](){
                uint32_t value = item->checkState() ? 1 : 0;
                std::vector<CombinedRegister> re = combinedRegisterMap.value(item->Label());
                for (auto it : re) 
                {
                    writeCombinedRegisterValue(it, value);
                }
            });
    }
}

void QToneMappingTab::initFisheyeSubTab(QPointer<QWidget> subTab)
{
    QPointer<QGridLayout> layout = new QGridLayout(subTab);
    layout->setAlignment(Qt::AlignLeft);
    subTab->setLayout(layout);
    layout->setContentsMargins(0,0,0,0);
    layout->setVerticalSpacing(1);
    
    // 创建Fisheye子页面的控件
    QList<QPointer<QLabelEditGroup>> fisheyeLabelEdits;
    QList<QPointer<QLabelCheckBox>> fisheyeLabelCheckBoxes;
    
    // 控件
    QPointer<QLabelEditGroup> fishLenOpt = new QLabelEditGroup();
    fishLenOpt->setLabelValue("Fish Len Opt");
    fishLenOpt->setEditable(false);
    fishLenOpt->setlabelWidth(130);
    fishLenOpt->setMouseTracking(true);
    layout->addWidget(fishLenOpt, 0, 0);
    fisheyeLabelEdits.append(fishLenOpt);
    
    QPointer<QLabelEditGroup> fisheyeRadius = new QLabelEditGroup();
    fisheyeRadius->setLabelValue("Fisheye Radius");
    fisheyeRadius->setEditable(false);
    fisheyeRadius->setlabelWidth(130);
    fisheyeRadius->setMouseTracking(true);
    layout->addWidget(fisheyeRadius, 1, 0);
    fisheyeLabelEdits.append(fisheyeRadius);
    
    QPointer<QLabelEditGroup> fisheyeCenterX = new QLabelEditGroup();
    fisheyeCenterX->setLabelValue("Fisheye Center X");
    fisheyeCenterX->setEditable(false);
    fisheyeCenterX->setlabelWidth(130);
    fisheyeCenterX->setMouseTracking(true);
    layout->addWidget(fisheyeCenterX, 2, 0);
    fisheyeLabelEdits.append(fisheyeCenterX);
    
    QPointer<QLabelEditGroup> fisheyeCenterY = new QLabelEditGroup();
    fisheyeCenterY->setLabelValue("Fisheye Center Y");
    fisheyeCenterY->setEditable(false);
    fisheyeCenterY->setlabelWidth(130);
    fisheyeCenterY->setMouseTracking(true);
    layout->addWidget(fisheyeCenterY, 3, 0);
    fisheyeLabelEdits.append(fisheyeCenterY);
    
    layout->setColumnStretch(0, 1);
    layout->setColumnStretch(1, 3);
    
    // 存储控件引用
    labelEditGroups["Fisheye"] = fisheyeLabelEdits;
    labelCheckBoxes["Fisheye"] = fisheyeLabelCheckBoxes;
    
    // 连接信号
    for (auto item : fisheyeLabelEdits) {
        connect(item, &QLabelEditGroup::releaseLabel, this, [=](){QTabWndComm::recvEdit(item->Label());});
        connect(item, &QLabelEditGroup::signalValueChange, this, 
            [=](uint32_t value)
            {
                std::vector<CombinedRegister> re = combinedRegisterMap.value(item->Label());
                for (auto it : re) 
                {
                    writeCombinedRegisterValue(it, value);
                }
            });
    }
    
    for (auto item : fisheyeLabelCheckBoxes) {
        connect(item, &QLabelCheckBox::release, this, [=](){QTabWndComm::recvEdit(item->Label());});
        connect(item, &QLabelCheckBox::checkSignal, this, 
            [=](){
                uint32_t value = item->checkState() ? 1 : 0;
                std::vector<CombinedRegister> re = combinedRegisterMap.value(item->Label());
                for (auto it : re) 
                {
                    writeCombinedRegisterValue(it, value);
                }
            });
    }
}

void QToneMappingTab::changeTab(int index)
{
    QString currentTabName = subPageMap.value(index);
    
    // 根据当前tab更新UI值
    if (labelEditGroups.contains(currentTabName)) {
        updateUIValue<QList<QPointer<QLabelEditGroup>>>(labelEditGroups[currentTabName]);
    }
    
    if (labelCheckBoxes.contains(currentTabName)) {
        updateUIValue<QList<QPointer<QLabelCheckBox>>>(labelCheckBoxes[currentTabName]);
    }
    
    // 特殊处理GTM Hist曲线图
    if (currentTabName == "GTM Hist") {
        readCurve();
    }
}

void QToneMappingTab::gtmHistComboStringChanged(const QString &str)
{
    auto uiDataMiddleware = QUiDataMiddleware(TAB_NAME, str);
    auto crValueList      = uiDataMiddleware.getCombinedRegisterValueList();
    if (crValueList.size() == 0) {
        return;
    }
    std::vector<QPoint> pointList;
    for (int i = 0; i < crValueList.size(); i++) {
        pointList.push_back(QPoint(i, crValueList[i].second));
    }
    disconnect(gtmHistCurve, &QCurveChartGroup::signalPointChanged, this, &QToneMappingTab::recvPointChanged);
    gtmHistCurve->setPoints(pointList);
    connect(gtmHistCurve, &QCurveChartGroup::signalPointChanged, this, &QToneMappingTab::recvPointChanged);
}

void QToneMappingTab::recvPointChanged(QPoint point)
{
    auto uiDataMiddleware = QUiDataMiddleware(TAB_NAME, gtmHistCurveCombo->currentText());
    auto crValueList      = uiDataMiddleware.getCombinedRegisterValueList();
    if (crValueList.size() <= point.x()) {
        return;
    }
    writeCombinedRegisterValue(crValueList[point.x()].first, point.y());
    updateRegisterTable({ crValueList[point.x()].first });
}

void QToneMappingTab::readCurve()
{
    if(m_nLevel!=LEVEL_ADMINISTRATOR)
    {
        return;
    }
    if (gtmHistCurve->isVisible())
        gtmHistComboStringChanged(gtmHistCurveCombo->currentText());
}
