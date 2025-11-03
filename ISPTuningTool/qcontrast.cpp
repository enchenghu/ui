#include "qcontrast.h"
#include <QSplitter>
#include "qtransfermanager.h"
#include "Common/Others/qusbcommunication.h"

const QString TAB_NAME = "Contrast";
const QList<QString> TAB_NAME_LIST = { "Contrast", "AutoStrech", "LowLevel" };
const std::vector<uint32_t> M1_INNER_REG = {0x30080F34, 0x30080F35, 0x30080F36, 0x30080F37, 0x30080F38, 0x30080F39};
const std::vector<uint32_t> M1_INNER_REG_RGB = {0x30082F44, 0x30082F45, 0x30082F46, 0x30082F47, 0x30082F48, 0x30082F49};
const std::vector<uint32_t> M1_INNER_REG_IR = {0x30082F5A, 0x30082F5B, 0x30082F5C, 0x30082F5D, 0x30082F5E, 0x30082F5F};

QContrast::QContrast(QWidget *parent)
    : QTabWndComm(parent)
    , tabWidget(new QTabWidget(this))
{
    int iMode = TransferManager::getInstance().getRgbIrMode();
    strRgbIrMode = iMode == 0 ? "RGB" : "IR";
    moduleName = TAB_NAME.toStdString();
    strDelRegListTxt = QCoreApplication::applicationDirPath() + "\\extracted_data\\Contrast.txt";
    loadDeletedRegistersFromFile(strDelRegListTxt);
    init();
    initConnect();
}

QContrast::~QContrast() { }

void QContrast::init()
{
    addRegisterTable();
    addTabWidget(tabWidget);
    const std::map<QString, std::function<void(QWidget *)>> initFuncList = {
        { "Contrast", std::bind(&QContrast::initContrastSubTab, this, std::placeholders::_1) },
        { "AutoStrech", std::bind(&QContrast::initAutoStrechSubTab, this, std::placeholders::_1) },
        { "LowLevel", std::bind(&QContrast::initLowLevelSubTab, this, std::placeholders::_1) }
    };
    
    if(m_nLevel != LEVEL_ADMINISTRATOR)
    {
        QList<QString>* pTemp = (QList<QString>*)&TAB_NAME_LIST;
        *pTemp = { "Contrast", "LowLevel" };
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
    qDebug() << "QContrast::recordUIRegister end";
    recordUIRegSubPageInfo();
    qDebug() << "QContrast::recordUIRegSubPageInfo end";
    changeTab(0);
    qDebug() << "QContrast::changeTab end";
    // addRegisterTable();
    // qDebug() << "QContrast::addRegisterTable end";
    // auto allRegisters = QUiDataMiddleware::getRegisterAddressByUiModule(TAB_NAME);
    // registerMoudle = std::vector<uint32_t>(allRegisters.constBegin(), allRegisters.constEnd());
}

void QContrast::initConnect()
{
    connect(tabWidget.get(), &QTabWidget::currentChanged, this, &QContrast::changeTab, Qt::QueuedConnection);
    connect(this,&QTabWndComm::readSignal,this,[=](){updateValue();});
    connect(this, &QTabWndComm::updateValue, this, &QContrast::read, Qt::QueuedConnection);
    connect(
        this, &QTabWndComm::reConnectRead, this,
        [=]() {
            recordUIRegister();updateValue();
        },
        Qt::QueuedConnection);
    connect(&TransferManager::getInstance(), &TransferManager::rgbIrModeChanged, this, &QContrast::changeRgbIrMode);
}

std::vector<CombinedRegister> QContrast::combinedRegisterSubPageRecording(QString strModuleName, QString strUiName, std::shared_ptr<int> ptrCamera, QString strSubPage) 
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

std::vector<CombinedRegister> QContrast::getCombinedRegister(QString strModuleName, QString strUiName, std::shared_ptr<int> ptrCamera)
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

void QContrast::recordUIRegSubPageInfo()
{
    // 记录Contrast tab的控件寄存器信息
    if (sCurveEnable) {
        QString label = sCurveEnable->Label().remove("*");
        auto vecRegs = combinedRegisterSubPageRecording("CURVE", label, std::make_shared<int>(getCurrentCamera()), "Contrast");
        if (CONTRAST_RGBIR_ENABLE_MAP.contains({moduleName.c_str(), label})) {
            auto vecIrRegs = getCombinedRegister("CURVE_IR_AB", label + "_IR_AB", std::make_shared<int>(getCurrentCamera()));
            auto vecRgbRegs = getCombinedRegister("CURVE_RGB_AB", label + "_RGB_AB", std::make_shared<int>(getCurrentCamera()));
            setRegisterTypeMap(vecRegs, vecRgbRegs, vecIrRegs);
        }
    }
    
    if (sPoint) {
        QString label = sPoint->Label().remove("*");
        auto vecRegs = combinedRegisterSubPageRecording("CURVE", label, std::make_shared<int>(getCurrentCamera()), "Contrast");
        if (CONTRAST_RGBIR_ENABLE_MAP.contains({moduleName.c_str(), label})) {
            auto vecIrRegs = getCombinedRegister("CURVE_IR_AB", label + "_IR_AB", std::make_shared<int>(getCurrentCamera()));
            auto vecRgbRegs = getCombinedRegister("CURVE_RGB_AB", label + "_RGB_AB", std::make_shared<int>(getCurrentCamera()));
            setRegisterTypeMap(vecRegs, vecRgbRegs, vecIrRegs);
        }
    }
    
    if (intensity) {
        QString label = intensity->Label().remove("*");
        auto vecRegs = combinedRegisterSubPageRecording("CURVE", label, std::make_shared<int>(getCurrentCamera()), "Contrast");
        if (CONTRAST_RGBIR_ENABLE_MAP.contains({moduleName.c_str(), label})) {
            auto vecIrRegs = getCombinedRegister("CURVE_IR_AB", label + "_IR_AB", std::make_shared<int>(getCurrentCamera()));
            auto vecRgbRegs = getCombinedRegister("CURVE_RGB_AB", label + "_RGB_AB", std::make_shared<int>(getCurrentCamera()));
            setRegisterTypeMap(vecRegs, vecRgbRegs, vecIrRegs);
        }
    }
    
    if (modeComboBox) {
        QString label = modeComboBox->Label().remove("*");
        auto vecRegs = combinedRegisterSubPageRecording("NewFunctionPatch", label, std::make_shared<int>(getCurrentCamera()), "Contrast");
        if (CONTRAST_RGBIR_ENABLE_MAP.contains({moduleName.c_str(), label})) {
            auto vecIrRegs = getCombinedRegister("NewFunctionPatch_IR_AB", label + "_IR_AB", std::make_shared<int>(getCurrentCamera()));
            auto vecRgbRegs = getCombinedRegister("NewFunctionPatch_RGB_AB", label + "_RGB_AB", std::make_shared<int>(getCurrentCamera()));
            setRegisterTypeMap(vecRegs, vecRgbRegs, vecIrRegs);
        }
    }
    
    if (curIso) {
        combinedRegisterSubPageRecording("PipeCtrl", "Current Gain L", std::make_shared<int>(getCurrentCamera()), "Contrast");
        combinedRegisterSubPageRecording("PipeCtrl", "Current Expo L", std::make_shared<int>(getCurrentCamera()), "Contrast");
    }
    
    if (checkAuto) {
        QString label = checkAuto->Label().remove("*");
        auto vecRegs = combinedRegisterSubPageRecording("NewFunctionPatch", label, std::make_shared<int>(getCurrentCamera()), "Contrast");
        if (CONTRAST_RGBIR_ENABLE_MAP.contains({moduleName.c_str(), label})) {
            auto vecIrRegs = getCombinedRegister("NewFunctionPatch_IR_AB", label + "_IR_AB", std::make_shared<int>(getCurrentCamera()));
            auto vecRgbRegs = getCombinedRegister("NewFunctionPatch_RGB_AB", label + "_RGB_AB", std::make_shared<int>(getCurrentCamera()));
            setRegisterTypeMap(vecRegs, vecRgbRegs, vecIrRegs);
        }
    }
    
    for (int i = 0; i < 5; i++) {
        if (isoNode[i]) {
            auto vecRegs = combinedRegisterSubPageRecording("NewFunctionPatch", QString("ISO Node[%1]").arg(i), std::make_shared<int>(getCurrentCamera()), "Contrast");
            if (CONTRAST_RGBIR_ENABLE_MAP.contains({moduleName.c_str(), QString("ISO Node[%1]").arg(i)})) {
                auto vecIrRegs = getCombinedRegister("NewFunctionPatch_IR_AB", QString("ISO Node[%1]").arg(i) + "_IR_AB", std::make_shared<int>(getCurrentCamera()));
                auto vecRgbRegs = getCombinedRegister("NewFunctionPatch_RGB_AB", QString("ISO Node[%1]").arg(i) + "_RGB_AB", std::make_shared<int>(getCurrentCamera()));
                setRegisterTypeMap(vecRegs, vecRgbRegs, vecIrRegs);
            }
        }
    }
    
    // 记录表格控件寄存器信息
    QString itemNames[] = {QString("Low Point[%1]"), QString("High Point[%1]"), QString("Low Str[%1]"), QString("High Str[%1]")};
    for (int j = 0; j < 4; j++) {
        for (int k = 0; k < 5; k++) {
            auto vecRegs = combinedRegisterSubPageRecording("NewFunctionPatch", itemNames[j].arg(k), std::make_shared<int>(getCurrentCamera()), "Contrast");
            if (CONTRAST_RGBIR_ENABLE_MAP.contains({moduleName.c_str(), itemNames[j].arg(k)})) {
                auto vecIrRegs = getCombinedRegister("NewFunctionPatch_IR_AB", itemNames[j].arg(k) + "_IR_AB", std::make_shared<int>(getCurrentCamera()));
                auto vecRgbRegs = getCombinedRegister("NewFunctionPatch_RGB_AB", itemNames[j].arg(k) + "_RGB_AB", std::make_shared<int>(getCurrentCamera()));
                setRegisterTypeMap(vecRegs, vecRgbRegs, vecIrRegs);
            }
        }
    }

    qDebug() << "QContrast::recordUIRegSubPageInfo Contrast end";
    
    // 记录AutoStrech tab的控件寄存器信息
    if (adjustLuma) {
        auto vecRegs = combinedRegisterSubPageRecording("CURVE", adjustLuma->Label().remove("*"), std::make_shared<int>(getCurrentCamera()), "AutoStrech");
        auto vecIrRegs = getCombinedRegister("CURVE_IR_AB", adjustLuma->Label().remove("*") + "_IR_AB", std::make_shared<int>(getCurrentCamera()));
        auto vecRgbRegs = getCombinedRegister("CURVE_RGB_AB", adjustLuma->Label().remove("*") + "_RGB_AB", std::make_shared<int>(getCurrentCamera()));
        qDebug() << "QContrast::recordUIRegSubPageInfo AutoStrech adjustLuma vecRegs size:" << vecRegs.size();
        qDebug() << "QContrast::recordUIRegSubPageInfo AutoStrech adjustLuma vecIrRegs size:" << vecIrRegs.size();
        qDebug() << "QContrast::recordUIRegSubPageInfo AutoStrech adjustLuma vecRgbRegs size:" << vecRgbRegs.size();
        setRegisterTypeMap(vecRegs, vecRgbRegs, vecIrRegs);
    }

    qDebug() << "QContrast::recordUIRegSubPageInfo AutoStrech adjustLuma end";
    
    for (int i = 0; i < 5; i++) {
        if (stretchIsoList[i]) {
            auto vecRegs = combinedRegisterSubPageRecording("CURVE", QString("StretchISOList[%1]").arg(i), std::make_shared<int>(getCurrentCamera()), "AutoStrech");
            if (CONTRAST_RGBIR_ENABLE_MAP.contains({moduleName.c_str(), QString("StretchISOList[%1]").arg(i)})) {
                auto vecIrRegs = getCombinedRegister("CURVE_IR_AB", QString("StretchISOList[%1]").arg(i) + "_IR_AB", std::make_shared<int>(getCurrentCamera()));
                auto vecRgbRegs = getCombinedRegister("CURVE_RGB_AB", QString("StretchISOList[%1]").arg(i) + "_RGB_AB", std::make_shared<int>(getCurrentCamera()));
                setRegisterTypeMap(vecRegs, vecRgbRegs, vecIrRegs);
            }
        }
        if (lowLevelOffset[i]) {
            auto vecRegs = combinedRegisterSubPageRecording("CURVE", QString("LowLevelOffset[%1]").arg(i), std::make_shared<int>(getCurrentCamera()), "AutoStrech");
            if (CONTRAST_RGBIR_ENABLE_MAP.contains({moduleName.c_str(), QString("LowLevelOffset[%1]").arg(i)})) {        
                auto vecIrRegs = getCombinedRegister("CURVE_IR_AB", QString("LowLevelOffset[%1]").arg(i) + "_IR_AB", std::make_shared<int>(getCurrentCamera()));
                auto vecRgbRegs = getCombinedRegister("CURVE_RGB_AB", QString("LowLevelOffset[%1]").arg(i) + "_RGB_AB", std::make_shared<int>(getCurrentCamera()));
                setRegisterTypeMap(vecRegs, vecRgbRegs, vecIrRegs);
            }
        }
        if (stretchRat[i]) {
            auto vecRegs = combinedRegisterSubPageRecording("CURVE", QString("StretchRat[%1]").arg(i), std::make_shared<int>(getCurrentCamera()), "AutoStrech");
            if (CONTRAST_RGBIR_ENABLE_MAP.contains({moduleName.c_str(), QString("StretchRat[%1]").arg(i)})) {
                auto vecIrRegs = getCombinedRegister("CURVE_IR_AB", QString("StretchRat[%1]").arg(i) + "_IR_AB", std::make_shared<int>(getCurrentCamera()));
                auto vecRgbRegs = getCombinedRegister("CURVE_RGB_AB", QString("StretchRat[%1]").arg(i) + "_RGB_AB", std::make_shared<int>(getCurrentCamera()));
                setRegisterTypeMap(vecRegs, vecRgbRegs, vecIrRegs);
            }
        }
    }
    
    // 记录LowLevel tab的控件寄存器信息
    QPointer<QLabelEditGroup>* editGroups[] = {&minLowLevelA, &minLowLevelB, &minHighLevelA, &maxLowLevelA, &maxLowLevelB, &minHighLevelB, &curveThre1, &curveThre2};
    for (auto editGroup : editGroups) {
        if (*editGroup) {
            QString label = (*editGroup)->Label().remove("*");
            auto vecRegs = combinedRegisterSubPageRecording("CURVE", label, std::make_shared<int>(getCurrentCamera()), "LowLevel");
            if (CONTRAST_RGBIR_ENABLE_MAP.contains({moduleName.c_str(), label})) {
                auto vecIrRegs = getCombinedRegister("CURVE_IR_AB", label + "_IR_AB", std::make_shared<int>(getCurrentCamera()));
                auto vecRgbRegs = getCombinedRegister("CURVE_RGB_AB", label + "_RGB_AB", std::make_shared<int>(getCurrentCamera()));
                setRegisterTypeMap(vecRegs, vecRgbRegs, vecIrRegs);
            }
        }
    }

    if (currentGainL) {
        combinedRegisterSubPageRecording("PipeCtrl", currentGainL->Label(), std::make_shared<int>(getCurrentCamera()), "LowLevel");
    }
    
    if (manStretchEnable) {
        combinedRegisterSubPageRecording("CURVE", manStretchEnable->Label(), std::make_shared<int>(getCurrentCamera()), "Contrast");
    }
    if (manLowLevel) {
        QString label = manLowLevel->Label().remove("*");
        auto vecRegs = combinedRegisterSubPageRecording("CURVE", label, std::make_shared<int>(getCurrentCamera()), "Contrast");
        if (CONTRAST_RGBIR_ENABLE_MAP.contains({moduleName.c_str(), label})) {
            auto vecIrRegs = getCombinedRegister("CURVE_IR_AB", label + "_IR_AB", std::make_shared<int>(getCurrentCamera()));
            auto vecRgbRegs = getCombinedRegister("CURVE_RGB_AB", label + "_RGB_AB", std::make_shared<int>(getCurrentCamera()));
            setRegisterTypeMap(vecRegs, vecRgbRegs, vecIrRegs);
        }
    }
    if (manStretchGain) {
        QString label = manStretchGain->Label().remove("*");
        auto vecRegs = combinedRegisterSubPageRecording("CURVE", label, std::make_shared<int>(getCurrentCamera()), "Contrast");
        if (CONTRAST_RGBIR_ENABLE_MAP.contains({moduleName.c_str(), label})) {
            auto vecIrRegs = getCombinedRegister("CURVE_IR_AB", label + "_IR_AB", std::make_shared<int>(getCurrentCamera()));
            auto vecRgbRegs = getCombinedRegister("CURVE_RGB_AB", label + "_RGB_AB", std::make_shared<int>(getCurrentCamera()));
            setRegisterTypeMap(vecRegs, vecRgbRegs, vecIrRegs);
        }
    }


    for (int i = 0; i < M1_INNER_REG.size(); i++) 
    {   
        regAddrModule.insert(M1_INNER_REG[i]);
        // registerTypeMap.insert(M1_INNER_REG[i], {M1_INNER_REG_RGB[i], M1_INNER_REG_IR[i]});
        // registerSubPageMap[M1_INNER_REG[i]].emplace_back("LowLevel");
        std::vector<uint32_t>().swap(registerMoudle);
        for (auto iter : regAddrModule)
        {
            registerMoudle.emplace_back(iter);
        }
        for (auto reg : getCombinedRegisterByRegisterAddress(M1_INNER_REG[i]))
        {
            setRegisterSubPage(reg, "LowLevel");
        }
        setRegisterTypeMap(getCombinedRegisterByRegisterAddress(M1_INNER_REG[i]),
                           getCombinedRegisterByRegisterAddress(M1_INNER_REG_RGB[i]),
                           getCombinedRegisterByRegisterAddress(M1_INNER_REG_IR[i])); 
    }

    combinedRegisterSubPageRecording("CURVE", "AdjCurveEnable", std::make_shared<int>(getCurrentCamera()), "Contrast");
}

void QContrast::recordUIRegister()
{
    combinedRegisterMap.clear();
    
    // 记录Contrast tab的控件寄存器信息
    if (sCurveEnable) {
        QString label = sCurveEnable->Label().remove("*");
        recordCombineRegbyUIifo("CURVE", label, std::make_shared<int>(getCurrentCamera()));
        if (CONTRAST_RGBIR_ENABLE_MAP.contains({moduleName.c_str(), label})) {
            recordCombineRegbyUIifo("CURVE_IR_AB", label + "_IR_AB", std::make_shared<int>(getCurrentCamera()));
            recordCombineRegbyUIifo("CURVE_RGB_AB", label + "_RGB_AB", std::make_shared<int>(getCurrentCamera()));
        }
    }
    
    if (sPoint) {
        QString label = sPoint->Label().remove("*");
        recordCombineRegbyUIifo("CURVE", label, std::make_shared<int>(getCurrentCamera()));
        if (CONTRAST_RGBIR_ENABLE_MAP.contains({moduleName.c_str(), label})) {
            recordCombineRegbyUIifo("CURVE_IR_AB", label + "_IR_AB", std::make_shared<int>(getCurrentCamera()));
            recordCombineRegbyUIifo("CURVE_RGB_AB", label + "_RGB_AB", std::make_shared<int>(getCurrentCamera()));
        }
    }
    
    if (intensity) {
        QString label = intensity->Label().remove("*");
        recordCombineRegbyUIifo("CURVE", label, std::make_shared<int>(getCurrentCamera()));
        if (CONTRAST_RGBIR_ENABLE_MAP.contains({moduleName.c_str(), label})) {
            recordCombineRegbyUIifo("CURVE_IR_AB", label + "_IR_AB", std::make_shared<int>(getCurrentCamera()));
            recordCombineRegbyUIifo("CURVE_RGB_AB", label + "_RGB_AB", std::make_shared<int>(getCurrentCamera()));
        }
    }
    
    if (modeComboBox) {
        QString label = modeComboBox->Label().remove("*");
        recordCombineRegbyUIifo("NewFunctionPatch",label, std::make_shared<int>(getCurrentCamera()));
        if (CONTRAST_RGBIR_ENABLE_MAP.contains({moduleName.c_str(), label})) {
            recordCombineRegbyUIifo("NewFunctionPatch_IR_AB", label + "_IR_AB", std::make_shared<int>(getCurrentCamera()));
            recordCombineRegbyUIifo("NewFunctionPatch_RGB_AB", label + "_RGB_AB", std::make_shared<int>(getCurrentCamera()));
        }
    }
    
    if (curIso) 
    {
        recordCombineRegbyUIifo("PipeCtrl", "Current Gain L", std::make_shared<int>(getCurrentCamera()));
        recordCombineRegbyUIifo("PipeCtrl", "Current Expo L", std::make_shared<int>(getCurrentCamera()));
    }
    
    if (checkAuto) {
        QString label = checkAuto->Label().remove("*");
        recordCombineRegbyUIifo("NewFunctionPatch", label, std::make_shared<int>(getCurrentCamera()));
        if (CONTRAST_RGBIR_ENABLE_MAP.contains({moduleName.c_str(), label})) {
            recordCombineRegbyUIifo("NewFunctionPatch_IR_AB", label + "_IR_AB", std::make_shared<int>(getCurrentCamera()));
            recordCombineRegbyUIifo("NewFunctionPatch_RGB_AB", label + "_RGB_AB", std::make_shared<int>(getCurrentCamera()));
        }
    }
    
    for (int i = 0; i < 5; i++) {
        if (isoNode[i]) {
            recordCombineRegbyUIifo("NewFunctionPatch", QString("ISO Node[%1]").arg(i), std::make_shared<int>(getCurrentCamera()));
            if (CONTRAST_RGBIR_ENABLE_MAP.contains({moduleName.c_str(), QString("ISO Node[%1]").arg(i)})) {
                recordCombineRegbyUIifo("NewFunctionPatch_IR_AB", QString("ISO Node[%1]").arg(i) + "_IR_AB", std::make_shared<int>(getCurrentCamera()));
                recordCombineRegbyUIifo("NewFunctionPatch_RGB_AB", QString("ISO Node[%1]").arg(i) + "_RGB_AB", std::make_shared<int>(getCurrentCamera()));
            }
        }
    }
    
    // 记录表格控件寄存器信息
    QString itemNames[] = {QString("Low Point[%1]"), QString("High Point[%1]"), QString("Low Str[%1]"), QString("High Str[%1]")};
    for (int j = 0; j < 4; j++) {
        for (int k = 0; k < 5; k++) {
            recordCombineRegbyUIifo("NewFunctionPatch", itemNames[j].arg(k), std::make_shared<int>(getCurrentCamera()));
            if (CONTRAST_RGBIR_ENABLE_MAP.contains({moduleName.c_str(), itemNames[j].arg(k)})) {
                recordCombineRegbyUIifo("NewFunctionPatch_IR_AB", itemNames[j].arg(k) + "_IR_AB", std::make_shared<int>(getCurrentCamera()));
                recordCombineRegbyUIifo("NewFunctionPatch_RGB_AB", itemNames[j].arg(k) + "_RGB_AB", std::make_shared<int>(getCurrentCamera()));
            }
        }
    }
    
    // 记录AutoStrech tab的控件寄存器信息
    if (adjustLuma) {
        recordCombineRegbyUIifo("CURVE", adjustLuma->Label().remove("*"), std::make_shared<int>(getCurrentCamera()));
        recordCombineRegbyUIifo("CURVE_IR_AB", adjustLuma->Label().remove("*") + "_IR_AB", std::make_shared<int>(getCurrentCamera()));
        recordCombineRegbyUIifo("CURVE_RGB_AB", adjustLuma->Label().remove("*") + "_RGB_AB", std::make_shared<int>(getCurrentCamera()));
    }
    
    for (int i = 0; i < 5; i++) {
        if (stretchIsoList[i]) {
            recordCombineRegbyUIifo("CURVE", QString("StretchISOList[%1]").arg(i), std::make_shared<int>(getCurrentCamera()));
            if (CONTRAST_RGBIR_ENABLE_MAP.contains({moduleName.c_str(), QString("StretchISOList[%1]").arg(i)})) {
                recordCombineRegbyUIifo("CURVE_IR_AB", QString("StretchISOList[%1]").arg(i) + "_IR_AB", std::make_shared<int>(getCurrentCamera()));
                recordCombineRegbyUIifo("CURVE_RGB_AB", QString("StretchISOList[%1]").arg(i) + "_RGB_AB", std::make_shared<int>(getCurrentCamera()));
            }
        }
        if (lowLevelOffset[i]) {
            recordCombineRegbyUIifo("CURVE", QString("LowLevelOffset[%1]").arg(i), std::make_shared<int>(getCurrentCamera()));
            if (CONTRAST_RGBIR_ENABLE_MAP.contains({moduleName.c_str(), QString("LowLevelOffset[%1]").arg(i)})) {
                recordCombineRegbyUIifo("CURVE_IR_AB", QString("LowLevelOffset[%1]").arg(i) + "_IR_AB", std::make_shared<int>(getCurrentCamera()));
                recordCombineRegbyUIifo("CURVE_RGB_AB", QString("LowLevelOffset[%1]").arg(i) + "_RGB_AB", std::make_shared<int>(getCurrentCamera()));
            }
        }
        if (stretchRat[i]) {
            recordCombineRegbyUIifo("CURVE", QString("StretchRat[%1]").arg(i), std::make_shared<int>(getCurrentCamera()));
            if (CONTRAST_RGBIR_ENABLE_MAP.contains({moduleName.c_str(), QString("StretchRat[%1]").arg(i)})) {
                recordCombineRegbyUIifo("CURVE_IR_AB", QString("StretchRat[%1]").arg(i) + "_IR_AB", std::make_shared<int>(getCurrentCamera()));
                recordCombineRegbyUIifo("CURVE_RGB_AB", QString("StretchRat[%1]").arg(i) + "_RGB_AB", std::make_shared<int>(getCurrentCamera()));
            }
        }
    }
    
    // 记录LowLevel tab的控件寄存器信息
    QPointer<QLabelEditGroup>* editGroups[] = {&minLowLevelA, &minLowLevelB, &minHighLevelA, &maxLowLevelA, &maxLowLevelB, &minHighLevelB, &curveThre1, &curveThre2};
    for (auto editGroup : editGroups) {
        if (*editGroup) {
            QString label = (*editGroup)->Label().remove("*");
            recordCombineRegbyUIifo("CURVE", label, std::make_shared<int>(getCurrentCamera()));
            if (CONTRAST_RGBIR_ENABLE_MAP.contains({moduleName.c_str(), label})) {
                recordCombineRegbyUIifo("CURVE_IR_AB", label + "_IR_AB", std::make_shared<int>(getCurrentCamera()));
                recordCombineRegbyUIifo("CURVE_RGB_AB", label + "_RGB_AB", std::make_shared<int>(getCurrentCamera()));
            }
        }
    }

    if (currentGainL) {
        recordCombineRegbyUIifo("PipeCtrl", currentGainL->Label(), std::make_shared<int>(getCurrentCamera()));
    }
    
    if (manStretchEnable) {
        recordCombineRegbyUIifo("CURVE", manStretchEnable->Label(), std::make_shared<int>(getCurrentCamera()));
    }
    if (manLowLevel) {
        QString label = manLowLevel->Label().remove("*");
        recordCombineRegbyUIifo("CURVE", label, std::make_shared<int>(getCurrentCamera()));
        if (CONTRAST_RGBIR_ENABLE_MAP.contains({moduleName.c_str(), label})) {
            recordCombineRegbyUIifo("CURVE_IR_AB", label + "_IR_AB", std::make_shared<int>(getCurrentCamera()));
            recordCombineRegbyUIifo("CURVE_RGB_AB", label + "_RGB_AB", std::make_shared<int>(getCurrentCamera()));
        }
    }
    if (manStretchGain) {
        QString label = manStretchGain->Label().remove("*");
        recordCombineRegbyUIifo("CURVE", label, std::make_shared<int>(getCurrentCamera()));
        if (CONTRAST_RGBIR_ENABLE_MAP.contains({moduleName.c_str(), label})) {
            recordCombineRegbyUIifo("CURVE_IR_AB", label + "_IR_AB", std::make_shared<int>(getCurrentCamera()));
            recordCombineRegbyUIifo("CURVE_RGB_AB", label + "_RGB_AB", std::make_shared<int>(getCurrentCamera()));
        }
    }

    std::vector<CombinedRegister> combinedVector;

    for (int i = 0; i < M1_INNER_REG.size(); i++) 
    {   
        for (auto reg : getCombinedRegisterByRegisterAddress(M1_INNER_REG[i]))
        {
            combinedVector.push_back(reg);
        }
    }
    combinedRegisterMap.insert("M1 Inner", combinedVector);

    recordCombineRegbyUIifo("CURVE", "AdjCurveEnable", std::make_shared<int>(getCurrentCamera()));
}

template <typename T>
void QContrast::updateUIValue(QPointer<T> *vector, int iLength, const QString &extraname)
{
    int valTemp = 0;
    QUsbCommunication::getInstance().readBurst(IR_RGB_MODE_REG_ADDR, (unsigned char*)(&valTemp), ADDRESS_LENGTH, VALUE_LENGTH);
    for(int i = 0; i < iLength; i++)
    {
        uint32_t value=0;
        QString strLabelOrigin = vector[i]->Label().remove("*") + extraname;
        auto reg = combinedRegisterMap.value(strLabelOrigin);
        if (CONTRAST_RGBIR_ENABLE_MAP.contains({moduleName.c_str(), strLabelOrigin}) && valTemp == 0x01) 
        {
            std::string strSuffix = strRgbIrMode == "RGB" ? "_RGB_AB" : "_IR_AB";
            reg = combinedRegisterMap.value(strLabelOrigin + strSuffix.c_str());
        }
        
        for(int j=0;j<reg.size();j++)
        {
            readCombinedRegisterValue(reg.at(j), value);
            if (std::is_same<T, QLabelEditGroup>::value)
                dynamic_cast<QLabelEditGroup*>(vector[i].get())->setValue(QString("0x%1").arg(QString::number(value,16).toUpper().rightJustified(VALUE_HEX_INFO,'0')));
            else if (std::is_same<T, QLabelCheckBox>::value)
            {
                if (value == 0 && strLabelOrigin.contains("Manual"))
                    dynamic_cast<QLabelCheckBox*>(vector[i].get())->setChecked(true);
                else if (value == 1 && strLabelOrigin.contains("Manual"))
                    dynamic_cast<QLabelCheckBox*>(vector[i].get())->setChecked(false);
                else if (value == 0)
                    dynamic_cast<QLabelCheckBox*>(vector[i].get())->setChecked(false);
                else if (value == 1)
                    dynamic_cast<QLabelCheckBox*>(vector[i].get())->setChecked(true);
            }
                
            else if (std::is_same<T, QLabelComboBox>::value)
                dynamic_cast<QLabelComboBox*>(vector[i].get())->setCurrentIndex(value);
        }
    }
}

void QContrast::initContrastSubTab(QPointer<QWidget> subTab)
{
    QPointer<QHBoxLayout> hlayout = new QHBoxLayout(subTab);
    hlayout->setContentsMargins(0, 10, 0, 10);
    subTab->setLayout(hlayout);

    // 三种模式的公共部分
    QPointer<QGridLayout> glayout = new QGridLayout;
    sCurveEnable = new QLabelCheckBox();
    sCurveEnable->setLabel("*ContrastEnable");
    sCurveEnable->setMouseTracking(true);
    sCurveEnable->setChecked(false);
    if (sCurveEnable) 
    {
        glayout->addWidget(sCurveEnable, 0, 0, 1, 1);
        QString strLabelOrigin = sCurveEnable->Label().remove("*");
        connect(sCurveEnable, &QLabelCheckBox::release, this, [=](){recvEditTarget(strLabelOrigin);});
        connect(sCurveEnable, &QLabelCheckBox::checkSignal, this,
                [=]()
                {
                    int valTemp = 0;
                    QUsbCommunication::getInstance().readBurst(IR_RGB_MODE_REG_ADDR, (unsigned char*)(&valTemp), ADDRESS_LENGTH, VALUE_LENGTH);
                    std::vector<CombinedRegister> re;
                    std::string strSuffix = strRgbIrMode == "RGB" ? "_RGB_AB" : "_IR_AB";
                    if (valTemp == 0x01)
                    {
                        re = combinedRegisterMap.value(strLabelOrigin + strSuffix.c_str());
                    }
                    else 
                    {
                        re = combinedRegisterMap.value(strLabelOrigin);
                    }
                    for (auto it : re) 
                    {
                        writeCombinedRegisterValue(it, sCurveEnable->checkState());
                    }
                }, Qt::QueuedConnection);
    }

    modeComboBox = new QLabelComboBox();
    modeComboBox->setValue("*Mode", QStringList() << "Original" << "Spoint" << "Line Stretch");
    modeComboBox->setMouseTracking(true);
    glayout->addWidget(modeComboBox, 0, 1, 1, 1);
    connect(modeComboBox, &QLabelComboBox::release, this, [=](){recvEditTarget(modeComboBox->Label().remove("*"));});
    connect(modeComboBox, SIGNAL(signalValueChange(QString)), this, SLOT(recvMode(QString)));

    curIso = new QLabelEditGroup();
    curIso->setLabelValue("Cur Iso");
    curIso->setValue(0);
    curIso->setMouseTracking(true);
    connect(this, &QTabWndComm::readSignal, this, [=]() {
        auto gainL      = QUiDataMiddleware("Combine", "Current Gain L").readValueFromDevice();
        auto expoL      = QUiDataMiddleware("Combine", "Current Expo L").readValueFromDevice();
        auto currentIso = (static_cast<uint64_t>(gainL) * (static_cast<uint64_t>(expoL) >> 4)) >> 4;
        curIso->setValue(currentIso);
    });
    curIso->setEditable(false);
    curIso->setEnabled(false);
    glayout->addWidget(curIso, 0, 2, 1, 1);

    // 创建堆叠窗口
    stackedWidget = new QStackedWidget();

    // 创建不同的页面（Original）
    QWidget *pageOriginal = new QWidget();
    QPointer<QGridLayout> originalLayout = new QGridLayout();
    sPoint = new QLabelEditGroup();
    sPoint->setLabelValue("SPoint");
    sPoint->setMouseTracking(true);
    sPoint->setEditable(false);

    intensity = new QLabelEditGroup();
    intensity->setLabelValue("Intensity");
    intensity->setMouseTracking(true);
    intensity->setEditable(false);
    
    if (sPoint) 
    {
        originalLayout->addWidget(sPoint, 2, 0, 1, 1);
        connect(sPoint, &QLabelEditGroup::releaseLabel, this, [=](){recvEditTarget(sPoint->Label().remove("*"));});
    }
    if (intensity) 
    {
        originalLayout->addWidget(intensity, 5, 0, 1, 1);
        connect(intensity, &QLabelEditGroup::releaseLabel, this, [=](){recvEditTarget(intensity->Label().remove("*"));});
    }

    for (int i = 0; i < std::max(originalLayout->columnCount(), 4); i++) {
        originalLayout->setColumnStretch(i, 1);
    }
    for (int j = 0; j < std::max(originalLayout->rowCount(), 7); j++){
        originalLayout->setRowStretch(j, 1);
    }

    pageOriginal->setLayout(originalLayout);
    stackedWidget->addWidget(pageOriginal);

    // 创建页面(Scurve/Line Stretch)
    QWidget *pageOther = new QWidget();
    QPointer<QGridLayout> otherLayout = new QGridLayout();

    //Auto / Manual
    checkAuto = new QLabelCheckBox();
    checkAuto->setLabel("*Manual");
    checkAuto->setMouseTracking(true);
    checkAuto->setChecked(true);
    otherLayout->addWidget(checkAuto, 0, 0, 1, 1);
    connect(checkAuto.get(), &QLabelCheckBox::release, this, [=](){recvEditTarget(checkAuto->Label().remove("*"));});
    // connect(checkAuto.get(), SIGNAL(releaseUpadteCombineReg(std::vector<CombinedRegister>)), 
    //         this, SLOT(onCheckBoxManualClicked(std::vector<CombinedRegister>)));
    connect(checkAuto.get(), &QLabelCheckBox::checkSignal, this,
        [=]() { 
            int valTemp = 0;
            QUsbCommunication::getInstance().readBurst(IR_RGB_MODE_REG_ADDR, (unsigned char*)(&valTemp), ADDRESS_LENGTH, VALUE_LENGTH);

            std::string strSuffix = strRgbIrMode == "RGB" ? "_RGB_AB" : "_IR_AB";
            // uint32_t index = 0;
            if (valTemp == 0x01)
            {
                // index = strRgbIrMode == "RGB" ? 1 : 2;
            }
            else 
            {
                strSuffix = "";
            }

            for(auto item : combinedRegisterMap.value(checkAuto->Label().remove("*") + strSuffix.c_str()))
            {
                std::vector<RegisterBitInfo> regBitInfo = DatabaseOrm::getRegisterBitInfoByCombinedRegister(item);
                writeCombinedRegisterValue(item, !checkAuto->getValue());
            }

            qDebug() << "checkAuto changed to " << checkAuto->getValue();

            if (checkAuto->getValue())
            {
                for (int i = 1; i < 5; i++)
                {
                    isoNode[i]->setEnabled(false);
                    for (int j = 0; j < 4; j++)
                    {
                        tableListGroup->getItem(i, j)->setEnabled(false);
                    }
                }
            }
            else 
            {
                for (int i = 1; i < 5; i++)
                {
                    isoNode[i]->setEnabled(true);
                    for (int j = 0; j < 4; j++)
                    {
                        tableListGroup->getItem(i, j)->setEnabled(true);
                    }
                }
            }
        }
    );

    for (int i = 0; i < 5; i++)
    {
        isoNode[i] = new QLabelEditGroup();
        isoNode[i]->setLabelValue(QString("*ISO Node[%1]").arg(i));
        isoNode[i]->setValue(0);
        isoNode[i]->setMouseTracking(true);
        otherLayout->addWidget(isoNode[i], i + 2, 0, 1, 1);
        connect(isoNode[i], &QLabelEditGroup::releaseLabel, this, [=](){recvEditTarget(QString("ISO Node[%1]").arg(i));});
    }

    QString tableName = "Result";

    //构造表格
    tableListGroup = new QTableListGroup();

    tableListGroup->setTableMenuRequested(false);
    tableListGroup->setSortingEnabled(false);
    tableListGroup->setAlternatingRowColors(false);
    tableListGroup->setStyle(
        "QTableView{gridline-color:rgb(0,0,0);color:rgb(0,0,0);outline:3px;background-color:rgb(230,230,230);}"
        "QHeaderView::section{border:3px solid "
        "rgb(0,0,0);background-color:rgb(165,165,165);color:rgb(255,255,255);font:bold};");
    tableListGroup->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    tableListGroup->setEditTriggers(QAbstractItemView::NoEditTriggers);

    otherLayout->addWidget(tableListGroup, 1, 1, 6, 3);
    
    tableListGroup->setMinimumHeight(0);
    tableListGroup->setHeader(QStringList() << "*Low Point"
                                            << "*High Point"
                                            << "*Low Str"
                                            << "*High Str");
    for (int j = 0; j < 4; j++)
        tableListGroup->horizontalHeader()->setSectionResizeMode(j, QHeaderView::ResizeMode::Stretch);
    for (int k = 0; k < 5; k++) {
        for (int j = 0; j < 4; j++) {
            tableListGroup->appendRow(k, j, "", Qt::AlignCenter);
        }
    }
    tableListGroup->setVerticalHeader(QStringList() << "[0]"
                                                << "[1]"
                                                << "[2]"
                                                << "[3]"
                                                << "[4]");
    connect(tableListGroup->verticalHeader(), &QHeaderView::sectionResized, 
    [=](int logicalIndex, int oldSize, int newSize) {
        tableListGroup->horizontalHeader()->setFixedHeight(newSize);
    });
    for (int j = 0; j < 5; j++)
    {
        tableListGroup->verticalHeader()->setSectionResizeMode(j, QHeaderView::ResizeMode::Stretch);
        tableListGroup->verticalHeader()->setDefaultAlignment(Qt::AlignCenter);
    }
    
    tableListGroup->verticalHeader()->setMinimumHeight(0);
    tableListGroup->setVerticalHeaderVisible(true);

    QString itemNames[] = {QString("Low Point[%1]"), QString("High Point[%1]"), QString("Low Str[%1]"), QString("High Str[%1]")};

    for (int i = 1; i < 5; i++)
    {
        isoNode[i]->setEnabled(false);
        for (int j = 0; j < 4; j++)
        {
            tableListGroup->getItem(i, j)->setEnabled(false);
        }
    }

    connect(tableListGroup.get(), SIGNAL(sendCurrentItem(QStandardItem *, int, int)), this,
                SLOT(recvTableValueChanged(QStandardItem *, int, int)));
    
    for (int i = 0; i < std::max(otherLayout->columnCount(), 4); i++) {
        otherLayout->setColumnStretch(i, 1);
    }
    for (int j = 0; j < std::max(otherLayout->rowCount(), 7); j++){
        otherLayout->setRowStretch(j, 1);
    }

    pageOther->setLayout(otherLayout);
    stackedWidget->addWidget(pageOther);
    stackedWidget->addWidget(pageOther);

    
    glayout->addWidget(stackedWidget, 1, 0, std::max(otherLayout->rowCount(), 7), std::max(otherLayout->columnCount(), 4));

    for (int i = 0; i < std::max(glayout->columnCount(), 4); i++) {
        glayout->setColumnStretch(i, 1);
    }
    for (int j = 0; j < std::max(glayout->rowCount(), 8); j++){
        glayout->setRowStretch(j, 1);
    }

    hlayout->addLayout(glayout);

    // 右侧部分
    if (m_nLevel == LEVEL_ADMINISTRATOR)
    {
        manStretchEnable = new QLabelCheckBox();
        manStretchEnable->setLabel("ManStretchEnable");
        manStretchEnable->setMouseTracking(true);
        manLowLevel = new QLabelEditGroup();
        manLowLevel->setLabelValue("ManLowLevel");
        manLowLevel->setMouseTracking(true);
        manStretchGain = new QLabelEditGroup();
        manStretchGain->setLabelValue("ManStretchGain");
        manStretchGain->setMouseTracking(true);
        manLowLevel->setEditable(false);
        manStretchGain->setEditable(false);

        QSplitter *line = new QSplitter(Qt::Vertical, this);
        line->setFrameShape(QFrame::WinPanel);
        line->setFixedWidth(1);
        line->setLineWidth(1);
        hlayout->addWidget(line);

        QPointer<QGridLayout> glayout2 = new QGridLayout;
        if (manStretchEnable) 
        {
            manStretchEnable->setMaximumWidth(200);
            glayout2->addWidget(manStretchEnable, 0, 1, 1, 1);
            connect(manStretchEnable, &QLabelCheckBox::release, this, [=](){recvEdit(manStretchEnable->Label());});
            // connect(manStretchEnable, &QLabelCheckBox::checkSignal, this, [=](){recvCheckBox(manStretchEnable->Label(), manStretchEnable->checkState());});
            connect(manStretchEnable, &QLabelCheckBox::checkSignal, this, 
                [=]()
                {
                    int value = manStretchEnable->checkState() ? 1 : 0; 
                    std::vector<CombinedRegister> re = combinedRegisterMap.value(manStretchEnable->Label());
                    for (auto it : re) 
                    {
                        writeCombinedRegisterValue(it, value);
                    }
                    updateTable(re);
                }, Qt::QueuedConnection);
        }
        if (manLowLevel) 
        {
            manLowLevel->setMaximumWidth(200);
            glayout2->addWidget(manLowLevel, 1, 1, 1, 1);
            connect(manLowLevel, &QLabelEditGroup::releaseLabel, this, [=](){recvEditTarget(manLowLevel->Label().remove("*"));});
        }
        if (manStretchGain) 
        {
            manStretchGain->setMaximumWidth(200);
            glayout2->addWidget(manStretchGain, 2, 1, 1, 1);
            connect(manStretchGain, &QLabelEditGroup::releaseLabel, this, [=](){recvEditTarget(manStretchGain->Label().remove("*"));});
        }
        hlayout->addLayout(glayout2);
    }

    UpdateTable();

    qDebug() << "QContrast::initContrastSubTab end";
}

void QContrast::changeRgbIrMode(int &newMode)
{
    if (newMode == 0) 
    {
        strRgbIrMode = "RGB";
    } 
    else if (newMode == 1) 
    {
        strRgbIrMode = "IR";
    }

    int valTemp = 0;
    QUsbCommunication::getInstance().readBurst(IR_RGB_MODE_REG_ADDR, (unsigned char*)(&valTemp), ADDRESS_LENGTH, VALUE_LENGTH);

    std::string strSuffix = strRgbIrMode == "RGB" ? "_RGB_AB" : "_IR_AB";
    uint32_t index = 0;
    if (valTemp == 0x01)
    {
        
    }
    else 
    {
        strSuffix = "";
    }
    
    for(auto item : combinedRegisterMap[QString("Mode") + strSuffix.c_str()])
    {
        std::vector<RegisterBitInfo> regBitInfo = DatabaseOrm::getRegisterBitInfoByCombinedRegister(item);
        if (stackedWidget->currentIndex() == 1)
            writeCombinedRegisterValue(item, 0);
        else if (stackedWidget->currentIndex() == 1)
            writeCombinedRegisterValue(item, 1);
    }
};

void QContrast::initAutoStrechSubTab(QPointer<QWidget> subTab)
{
    // Create controls for AutoStrech tab
    adjustLuma = new QLabelCheckBox();
    adjustLuma->setLabel("*AdjustLuma");
    adjustLuma->setMouseTracking(true);
    adjustLuma->setChecked(false);
    
    for (int i = 0; i < 5; i++) {
        stretchIsoList[i] = new QLabelEditGroup();
        stretchIsoList[i]->setLabelValue(QString("*StretchISOList[%1]").arg(i));
        stretchIsoList[i]->setMouseTracking(true);
        lowLevelOffset[i] = new QLabelEditGroup();
        lowLevelOffset[i]->setLabelValue(QString("*LowLevelOffset[%1]").arg(i));
        lowLevelOffset[i]->setMouseTracking(true);
        stretchRat[i] = new QLabelEditGroup();
        stretchRat[i]->setLabelValue(QString("*StretchRat[%1]").arg(i));
        stretchRat[i]->setMouseTracking(true);
        stretchIsoList[i]->setEditable(false);
        lowLevelOffset[i]->setEditable(false);
        stretchRat[i]->setEditable(false);
    }
    
    QPointer<QGridLayout> layout = new QGridLayout(subTab);
    layout->setContentsMargins(20, 10, 20, 10);
    layout->setHorizontalSpacing(50);
    subTab->setLayout(layout);
    
    if (adjustLuma) 
    {
        layout->addWidget(adjustLuma, 0, 0, 1, 1);
        connect(adjustLuma, &QLabelCheckBox::release, this, [=](){recvEditTarget(adjustLuma->Label().remove("*"));});
        connect(adjustLuma, &QLabelCheckBox::checkSignal, this, [=]()
                {
                    int valTemp = 0;
                    QUsbCommunication::getInstance().readBurst(IR_RGB_MODE_REG_ADDR, (unsigned char*)(&valTemp), ADDRESS_LENGTH, VALUE_LENGTH);
                    std::vector<CombinedRegister> re;
                    std::string strSuffix = strRgbIrMode == "RGB" ? "_RGB_AB" : "_IR_AB";
                    if (valTemp == 0x01)
                    {
                        re = combinedRegisterMap.value(adjustLuma->Label().remove("*") + strSuffix.c_str());
                    }
                    else 
                    {
                        re = combinedRegisterMap.value(adjustLuma->Label().remove("*"));
                    }
                    for (auto it : re) 
                    {
                        writeCombinedRegisterValue(it, sCurveEnable->checkState());
                    }
                }, Qt::QueuedConnection);
    }
    for (int i = 0; i < 5; i++) {
        if (stretchIsoList[i]) 
        {
            layout->addWidget(stretchIsoList[i], i+1, 0, 1, 1);
            connect(stretchIsoList[i], &QLabelEditGroup::releaseLabel, this, [=](){recvEditTarget(QString("StretchISOList[%1]").arg(i));});
        }
        if (lowLevelOffset[i]) 
        {
            layout->addWidget(lowLevelOffset[i], i+1, 1, 1, 1);
            connect(lowLevelOffset[i], &QLabelEditGroup::releaseLabel, this, [=](){recvEditTarget(QString("LowLevelOffset[%1]").arg(i));});
        }
        if (stretchRat[i]) 
        {
            layout->addWidget(stretchRat[i], i+1, 2, 1, 1);
            connect(stretchRat[i], &QLabelEditGroup::releaseLabel, this, [=](){recvEditTarget(QString("StretchRat[%1]").arg(i));});
        }
    }

    for (int i = 0; i < layout->columnCount(); i++) {
        layout->setColumnStretch(i, 1);
    }
    
    // Add Cur Iso widget
    auto targetLayout = static_cast<QGridLayout *>(subTab->layout());
    QPointer<QLabelEditGroup> curIso = new QLabelEditGroup(subTab);
    curIso->setLabelValue("Cur Iso");
    curIso->setValue(0);
    curIso->setMouseTracking(true);
    connect(this, &QTabWndComm::readSignal, curIso, [curIso]() {
        if (!curIso->isVisible())
            return;
        auto gainL = QUiDataMiddleware(TAB_NAME, "Current Gain L").readValueFromDevice();
        auto expoL = QUiDataMiddleware(TAB_NAME, "Current Expo L").readValueFromDevice();
        auto currentIso = (static_cast<uint64_t>(gainL) * (static_cast<uint64_t>(expoL) >> 4)) >> 4;
        curIso->setValue(currentIso);
    });
    targetLayout->addWidget(curIso, 0, 1);

    qDebug() << "QContrast::initAutoStrechSubTab end";
}

void QContrast::initLowLevelSubTab(QPointer<QWidget> subTab)
{
    // Create controls for LowLevel tab
    minLowLevelA = new QLabelEditGroup();
    minLowLevelA->setLabelValue("*MinLowLevelA");
    minLowLevelB = new QLabelEditGroup();
    minLowLevelB->setLabelValue("*MinLowLevelB");
    minHighLevelA = new QLabelEditGroup();
    minHighLevelA->setLabelValue("*MinHighLevelA");
    maxLowLevelA = new QLabelEditGroup();
    maxLowLevelA->setLabelValue("*MaxLowLevelA");
    maxLowLevelB = new QLabelEditGroup();
    maxLowLevelB->setLabelValue("*MaxLowLevelB");
    minHighLevelB = new QLabelEditGroup();
    minHighLevelB->setLabelValue("*MinHighLevelB");
    curveThre1 = new QLabelEditGroup();
    curveThre1->setLabelValue("*CurveThre1");
    curveThre2 = new QLabelEditGroup();
    curveThre2->setLabelValue("*CurveThre2");
    currentGainL = new QLabelEditGroup();
    currentGainL->setLabelValue("Current Gain L");
    
    QPointer<QGridLayout> layout = new QGridLayout(subTab);
    layout->setContentsMargins(20, 10, 20, 10);
    layout->setHorizontalSpacing(50);
    subTab->setLayout(layout);

    QPair<QPointer<QLabelEditGroup>, QString> pairList[] = {
        { minLowLevelA, "MinLowLevelA" },
        { minLowLevelB, "MinLowLevelB" },
        { minHighLevelA, "MinHighLevelA" },
        { maxLowLevelA, "MaxLowLevelA" },
        { maxLowLevelB, "MaxLowLevelB" },
        { minHighLevelB, "MinHighLevelB" },
        { curveThre1, "CurveThre1" },
        { curveThre2, "CurveThre2" },
        { currentGainL, "Current Gain L" }
    };

    for (int col = 0; col < 3; col++) 
    {
        for (int row = 0; row < 3; row++) 
        {
            int index = col * 3 + row;
            if (!pairList[index].first) 
                continue;
            
            pairList[index].first->setMouseTracking(true);
            pairList[index].first->setEditable(false);
            layout->addWidget(pairList[index].first, row, col, 1, 1);
            connect(pairList[index].first, &QLabelEditGroup::releaseLabel, this, [=](){recvEditTarget(pairList[index].second);});
        }
    }
    
    for (int i = 0; i < layout->columnCount(); i++) {
        layout->setColumnStretch(i, 1);
    }

    qDebug() << "QContrast::initLowLevelSubTab end";
}

void QContrast::recordTableUIRegister() 
{ 
    saveCombinedReg();
}

void QContrast::UpdateTable() 
{ 
    int valTemp = 0;
    QUsbCommunication::getInstance().readBurst(IR_RGB_MODE_REG_ADDR, (unsigned char*)(&valTemp), ADDRESS_LENGTH, VALUE_LENGTH);
    std::string strSuffix = strRgbIrMode == "RGB" ? "_RGB_AB" : "_IR_AB";
    
    QString itemNames[] = {QString("Low Point[%1]"), QString("High Point[%1]"), QString("Low Str[%1]"), QString("High Str[%1]")};
    for (int k = 0; k < 5; k++) {
        for (int j = 0; j < 4; j++) {
            QString  itemName = itemNames[j].arg(k);
            uint32_t value    = 0;
            if (valTemp == 0x01 && CONTRAST_RGBIR_ENABLE_MAP.contains({moduleName.c_str(), itemName}))
            {
                itemName += strSuffix.c_str();
            }
            auto     reg      = combinedRegisterMap.value(itemName);
            for (int i = 0; i < reg.size(); i++) {
                readCombinedRegisterValue(reg.at(i), value);

                QStandardItem *item = tableListGroup->getItem(k, j);
                if (item)
                    item->setText(QString("0x") + QString::number(value, 16));
            }
        }
    }
}

// void QContrast::onCheckBoxManualClicked(std::vector<CombinedRegister> combineList) 
// { 
//     clearCurrentRigester();

//     if (checkAuto->getValue())
//     {
//         for (int i = 1; i < 5; i++)
//         {
//             isoNode[i]->setEnabled(false);
//             for (int j = 0; j < 4; j++)
//             {
//                 tableListGroup->getItem(i, j)->setEnabled(false);
//             }
//         }
//     }
//     else 
//     {
//         for (int i = 1; i < 5; i++)
//         {
//             isoNode[i]->setEnabled(true);
//             for (int j = 0; j < 4; j++)
//             {
//                 tableListGroup->getItem(i, j)->setEnabled(true);
//             }
//         }
//     }
    
//     std::vector<CombinedRegister> re;
//     int valTemp = 0;
//     QUsbCommunication::getInstance().readBurst(IR_RGB_MODE_REG_ADDR, (unsigned char*)(&valTemp), ADDRESS_LENGTH, VALUE_LENGTH);

//     std::string strSuffix = strRgbIrMode == "RGB" ? "_RGB_AB" : "_IR_AB";
//     re = combinedRegisterMap.value("Manual");
//     if (valTemp == 0x01)
//     {
//         re = combinedRegisterMap.value(("Manual" + strSuffix).c_str());
//     }
    
//     regComVector = re;
//     updateTable(re);
// }

void QContrast::recvMode(QString str) 
{ 
    clearCurrentRigester();
    std::map<QString, int> mapStrIndex = {{"Original", 0}, {"Spoint", 1}, {"Line Stretch", 1}};

    int valTemp = 0;
    QUsbCommunication::getInstance().readBurst(IR_RGB_MODE_REG_ADDR, (unsigned char*)(&valTemp), ADDRESS_LENGTH, VALUE_LENGTH);

    std::string strSuffix = strRgbIrMode == "RGB" ? "_RGB_AB" : "_IR_AB";
    uint32_t index = 0;
    clearCurrentRigester();
    std::vector<CombinedRegister> re;
    re = combinedRegisterMap.value("Mode");
    if (valTemp == 0x01)
    {
        index = strRgbIrMode == "RGB" ? 1 : 2;
        re = combinedRegisterMap.value(("Mode" + strSuffix).c_str());
    }
    else 
    {
        index = 0;
    }
    
    for(auto item : re)
    {
        std::vector<RegisterBitInfo> regBitInfo = DatabaseOrm::getRegisterBitInfoByCombinedRegister(item);
        if (str == "Spoint")
            writeCombinedRegisterValue(item, 0);
        else if (str == "Line Stretch")
            writeCombinedRegisterValue(item, 1);
    }

    stackedWidget->setCurrentIndex(mapStrIndex[str]);

    regComVector = re;
    updateTable(re);
}

void QContrast::recvTableValueChanged(QStandardItem *item, int row, int col)
{
    QString itemNames[] = {QString("Low Point[%1]"), QString("High Point[%1]"), QString("Low Str[%1]"), QString("High Str[%1]")};
    labelSpinSliderCheckBoxGroup->setFloatIntType(FloatIntType::Type_1_0x200);
    QString itemName  = itemNames[col].arg(row);
    int valTemp = 0;
    QUsbCommunication::getInstance().readBurst(IR_RGB_MODE_REG_ADDR, (unsigned char*)(&valTemp), ADDRESS_LENGTH, VALUE_LENGTH);

    std::string strSuffix = strRgbIrMode == "RGB" ? "_RGB_AB" : "_IR_AB";
    if (valTemp == 0x01 && CONTRAST_RGBIR_ENABLE_MAP.contains({moduleName.c_str(), itemName}))
    {
        itemName += strSuffix.c_str();
    }
    
    updateRegisterTable(combinedRegisterMap.value(itemName));
}

void QContrast::changeTab(int index)
{
    qDebug() << "QContrast::changeTab " << index;
    switch (index) {
    case 0: // Contrast tab
    {
        std::vector<QPointer<QLabelCheckBox>> checkBoxList;
        std::vector<QPointer<QLabelEditGroup>> editGroupList;
        // if (sCurveEnable) updateUIValue<QLabelCheckBox>(&sCurveEnable, 1, "");
        if (sCurveEnable) checkBoxList.push_back(sCurveEnable);
        // if (sPoint) updateUIValue<QLabelEditGroup>(&sPoint, 1, "");
        if (sPoint) editGroupList.push_back(sPoint);
        // if (intensity) updateUIValue<QLabelEditGroup>(&intensity, 1, "");
        if (intensity) editGroupList.push_back(intensity);
        if (modeComboBox) updateUIValue<QLabelComboBox>(&modeComboBox, 1, modeComboBox->getValue(modeComboBox->getCurrentIndex()));
        // if (curIso) updateUIValue<QLabelEditGroup>(&curIso, 1, "");
        if (curIso) editGroupList.push_back(curIso);
        // if (checkAuto) updateUIValue<QLabelCheckBox>(&checkAuto, 1, "");
        if (checkAuto) checkBoxList.push_back(checkAuto);
        // if (manStretchEnable) updateUIValue<QLabelCheckBox>(&manStretchEnable, 1, "");
        if (manStretchEnable) checkBoxList.push_back(manStretchEnable);
        // if (manLowLevel) updateUIValue<QLabelEditGroup>(&manLowLevel, 1, "");
        if (manLowLevel) editGroupList.push_back(manLowLevel);
        // if (manStretchGain) updateUIValue<QLabelEditGroup>(&manStretchGain, 1, "");
        if (manStretchGain) editGroupList.push_back(manStretchGain);
        // updateUIValue<QLabelEditGroup>(isoNode, 5, "");
        for (int i = 0; i < 5; i++)
        {
            if (isoNode[i]) editGroupList.push_back(isoNode[i]);
        }
        updateUIValue<QLabelCheckBox>(checkBoxList.data(), checkBoxList.size(), "");
        updateUIValue<QLabelEditGroup>(editGroupList.data(), editGroupList.size(), "");
        UpdateTable();
        break;
    }
    case 1: // AutoStrech tab
    {
        if (adjustLuma) updateUIValue<QLabelCheckBox>(&adjustLuma, 1, "");
        std::vector<QPointer<QLabelEditGroup>> editGroupList;
        for (int i = 0; i < 5; i++)
        {
            if (stretchIsoList[i]) editGroupList.push_back(stretchIsoList[i]);
            if (lowLevelOffset[i]) editGroupList.push_back(lowLevelOffset[i]);
            if (stretchRat[i]) editGroupList.push_back(stretchRat[i]);
        }
        updateUIValue<QLabelEditGroup>(editGroupList.data(), editGroupList.size(), "");
        //
        // updateUIValue<QLabelEditGroup>(stretchIsoList, 5, "");
        // updateUIValue<QLabelEditGroup>(lowLevelOffset, 5, "");
        // updateUIValue<QLabelEditGroup>(stretchRat, 5, "");
        break;
    }
        
    case 2: // LowLevel tab
    {
        std::vector<QPointer<QLabelEditGroup>> editGroupList = {
            minLowLevelA, minLowLevelB, minHighLevelA, maxLowLevelA, maxLowLevelB,
            minHighLevelB, curveThre1, curveThre2, currentGainL
        };
        updateUIValue<QLabelEditGroup>(editGroupList.data(), editGroupList.size(), "");
        //
        // updateUIValue<QLabelEditGroup>(&minLowLevelA, 1, "");
        // updateUIValue<QLabelEditGroup>(&minLowLevelB, 1, "");
        // updateUIValue<QLabelEditGroup>(&minHighLevelA, 1, "");
        // updateUIValue<QLabelEditGroup>(&maxLowLevelA, 1, "");
        // updateUIValue<QLabelEditGroup>(&maxLowLevelB, 1, "");
        // updateUIValue<QLabelEditGroup>(&minHighLevelB, 1, "");
        // updateUIValue<QLabelEditGroup>(&curveThre1, 1, "");
        // updateUIValue<QLabelEditGroup>(&curveThre2, 1, "");
        // updateUIValue<QLabelEditGroup>(&currentGainL, 1, "");
        break;
    }
        
    default:
        break;
    }
    
    // read();
}

void QContrast::recvEditTarget(QString str) 
{ 
    clearCurrentRigester();
    qDebug() << "QContrast::recvEditTarget: " << str;
    int valTemp = 0;
    QUsbCommunication::getInstance().readBurst(IR_RGB_MODE_REG_ADDR, (unsigned char*)(&valTemp), ADDRESS_LENGTH, VALUE_LENGTH);

    std::vector<CombinedRegister> re;
    std::string strSuffix = strRgbIrMode == "RGB" ? "_RGB_AB" : "_IR_AB";
    if (valTemp == 0x01 && CONTRAST_RGBIR_ENABLE_MAP.contains({moduleName.c_str(), str}))
    {
        re = combinedRegisterMap.value(str + strSuffix.c_str());
    } else {
        re = combinedRegisterMap.value(str);
    }
    updateRegisterTable(re);
}

void QContrast::updateValue()
{
    changeTab(tabWidget->currentIndex());
}
