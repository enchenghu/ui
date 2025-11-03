#include "qaectab.h"
#include "Common/UI/qlabeleditgroup.h"
#include "Common/UI/qlabelcombobox.h"
#include "Common/UI/qlabelcheckbox.h"
#include "Common/UI/qtablelistgroup.h"
#include "qaectabuiconfig.h"
#include <QGroupBox>
#include "qtransfermanager.h"
#include "Common/Others/qusbcommunication.h"

QAECTab::QAECTab(QWidget *parent)
    : QTabWndComm(parent),
    roi_width(0),
    roi_height(0)
    // roi_blks(0)
{
    int iMode = TransferManager::getInstance().getRgbIrMode();
    strRgbIrMode = iMode == 0 ? "RGB" : "IR";
    strDelRegListTxt = QCoreApplication::applicationDirPath() + "\\extracted_data\\AEC.txt";
    loadDeletedRegistersFromFile(strDelRegListTxt);
    this->init();
    this->initConnect();
    moduleName = TAB_NAME.toStdString();
    // std::vector<uint32_t>(0).swap(registerMoudle);
    // const QStringList TAB_NAME_LIST = { TAB_NAME, "AEC_L", "AEC_M", "AEC_S", "AEC_V" };
    // for (auto &item : TAB_NAME_LIST) {
    //     auto allRegisters = QUiDataMiddleware::getRegisterAddressByUiModule(item);
    //     for (auto &reg : allRegisters) {
    //         registerMoudle.push_back(reg);
    //     }
    // }
    // qDebug() << registerMoudle.size();

}

QAECTab::~QAECTab() { }

void QAECTab::recvEditTarget(QString str, QString strModule) 
{ 
    clearCurrentRigester();

    int valTemp = 0;
    QUsbCommunication::getInstance().readBurst(IR_RGB_MODE_REG_ADDR, (unsigned char*)(&valTemp), ADDRESS_LENGTH, VALUE_LENGTH);

    // QString str = middleWare->getUiControlName();
    // QString strModule = middleWare->getUiModuleName();
    std::vector<CombinedRegister> re;
    std::string strSuffix = strRgbIrMode == "RGB" ? "RGB_AB" : "IR_AB";
    if (valTemp == 0x01)
    {
        re = combinedRegisterMap.value(strModule + "_" + str + " " + strSuffix.c_str());
    }
    else 
    {
        re = combinedRegisterMap.value(strModule + "_" + str);
    }
    
    // qDebug() << re.size();
    // for(auto it:combinedRegisterMap.value(str)){
    //     re.push_back(it);
    // }
    
    updateRegisterTable(re);
}

void QAECTab::init()
{
    expGainEdit.clear();
    expGainCheck.clear();
    targetEdit.clear();
    speedEdit.clear();
    roiEditL.clear();
    roiEditM.clear();
    roiEditS.clear();
    roiEditV.clear();
    roiWeightTables.clear();
    roiWeightEnableCheck.clear();
    
    const std::vector<std::pair<QString, std::function<void(QPointer<QWidget>)>>> INIT_LIST = {
        { QString("Exp&Gain"), std::bind(&QAECTab::initExpTypeSubTab, this, std::placeholders::_1) },
        { QString("Target&Speed"), std::bind(&QAECTab::initTargetSpeedSubTab, this, std::placeholders::_1) },
        { QString("ROI"), std::bind(&QAECTab::initRoiSubTab, this, std::placeholders::_1) },
    };
    
    addRegisterTable();
    tabWidget = new QTabWidget(this);
    addTabWidget(tabWidget);
    int iCount = 0;
    for (auto &item : INIT_LIST) {
        QPointer<QWidget> subTab = new QWidget(tabWidget.get());
        tabWidget->addTab(subTab.get(), item.first);
        item.second(subTab);
        subPageMap.insert(iCount, item.first);
        iCount++;
        // qDebug() << item.first;
    }
    
    recordUIRegister();
    recordUIRegSubPageInfo();
    changeTab(0);
}

void QAECTab::initConnect()
{
    connect(tabWidget.get(), &QTabWidget::currentChanged, this, &QAECTab::changeTab, Qt::QueuedConnection);
    connect(this, &QTabWndComm::readSignal, this, [=]() { changeTab(tabWidget->currentIndex()); });
    connect(this, &QTabWndComm::reConnectRead, this, [=]() {
        std::vector<uint32_t>(0).swap(registerMoudle);
        const QStringList TAB_NAME_LIST = { TAB_NAME, "AEC_L", "AEC_M", "AEC_S", "AEC_V" };
        for (auto &item : TAB_NAME_LIST) {
            auto allRegisters = QUiDataMiddleware::getRegisterAddressByUiModule(item);
            for (auto &reg : allRegisters) {
                registerMoudle.push_back(reg);
            }
        }
        recordUIRegister();
        changeTab(tabWidget->currentIndex());
    });

    connect(this, &QTabWndComm::updateValue, this,[=](){
        changeTab(tabWidget->currentIndex());});
    
    connect(&TransferManager::getInstance(), &TransferManager::rgbIrModeChanged, 
        this, &QAECTab::changeRgbIrMode);
}

void QAECTab::changeRgbIrMode(int &newMode) 
{ 
    if (newMode == 0) 
    {
        strRgbIrMode = "RGB";
    } 
    else if (newMode == 1) 
    {
        strRgbIrMode = "IR";
    }
}

void QAECTab::initExpTypeSubTab(QPointer<QWidget> subTab)
{
    QPointer<QGridLayout> layout = new QGridLayout(subTab.get());
    subTab->setLayout(layout.get());
    QPointer<QGroupBox>   expTypeGroupBox = new QGroupBox("Exp Type");
    QPointer<QGridLayout> expTypeLayout   = new QGridLayout(expTypeGroupBox.get());
    expTypeGroupBox->setLayout(expTypeLayout.get());
    QPointer<QLabelComboBox> combo = new QLabelComboBox(expTypeGroupBox.get());
    combo->setLabel("");
    combo->setTextRegValuePairList({ { "Auto", 0 }, { "Manual", 1 } });
    
    // 记录控件和寄存器的关联
    std::shared_ptr<int> pCameraId = std::make_shared<int>(getCurrentCamera());
    auto uiInfo = DatabaseOrm::getUiInfoByModuleNameAndControlName(TAB_NAME.toStdString(), "AECManEnable");

    // 在寄存器栏显示对应的combine register
    connect(combo, &QLabelComboBox::release, this,
        [=](QString str)
        {
            recvEdit(TAB_NAME + "_AECManEnable");
        }, Qt::QueuedConnection);
    
    // 连接读取信号，读取不同模式对应reg的值
    
    expTypeLayout->addWidget(combo, 0, 0);
    layout->addWidget(expTypeGroupBox, 0, 0, 1, 1);
    combo->setMouseTracking(true);
    combo->setMinimumWidth(10);
    combo->setValue(0);
    combo->layout()->setContentsMargins(0, 0, 0, 0);
    
    expTypeCombo = combo;
    
    connect(combo, &QLabelComboBox::indexChange, this, 
        [=](int value)
        {
            std::vector<CombinedRegister> re = combinedRegisterMap.value(TAB_NAME + "_AECManEnable");
            for (auto it : re) 
            {
                writeCombinedRegisterValue(it, value);
            }
            updateRegisterTable(re);
        }, Qt::QueuedConnection);

   // layout->addWidget(generateGroupBox("LFS", TAB_NAME, EXP_GAIN_CONFIG_MAP["LFS"]), 0, 1, 1, 1);
    layout->addWidget(generateGroupBox("Manual", TAB_NAME, EXP_GAIN_CONFIG_MAP["Manual Expo"], "Exp&Gain", 80), 1, 0, 1, 1);
    layout->addWidget(generateGroupBox("Manual", TAB_NAME, EXP_GAIN_CONFIG_MAP["Manual Gain"], "Exp&Gain", 80), 2, 0, 1, 1);
    layout->addWidget(generateGroupBox("Auto", TAB_NAME, EXP_GAIN_CONFIG_MAP["Auto Expo"], "Exp&Gain", 80), 1, 1, 1, 1);
    layout->addWidget(generateGroupBox("Auto", TAB_NAME, EXP_GAIN_CONFIG_MAP["Auto Gain"], "Exp&Gain", 80), 2, 1, 1, 1);
    layout->addWidget(generateGroupBox("Current", TAB_NAME, EXP_GAIN_CONFIG_MAP["Current Expo"], "Exp&Gain", 80), 1, 2, 1, 1);
    layout->addWidget(generateGroupBox("Current", TAB_NAME, EXP_GAIN_CONFIG_MAP["Current Gain"], "Exp&Gain", 80), 2, 2, 1, 1);
    if(m_nLevel==LEVEL_ADMINISTRATOR)
    {
        layout->addWidget(generateGroupBox("Graded Expo", TAB_NAME, EXP_GAIN_CONFIG_MAP["Graded Expo"], "Exp&Gain", 80), 1, 3, 1, 1);
        layout->addWidget(generateGroupBox("Band", TAB_NAME, EXP_GAIN_CONFIG_MAP["Band"], "Exp&Gain", 80), 2, 3, 1, 1);
    }
    layout->setColumnStretch(0, 1);
    layout->setColumnStretch(1, 2);
    layout->setColumnStretch(2, 1);
    layout->setColumnStretch(3, 1);
    layout->setRowStretch(0, 1);
    layout->setRowStretch(1, 5);
    layout->setRowStretch(2, 5);
}

void QAECTab::initTargetSpeedSubTab(QPointer<QWidget> subTab)
{
    // 初始化子tab
    QPointer<QGridLayout> layout = new QGridLayout(subTab.get());
    subTab->setLayout(layout.get());
    auto                      targetGroupBox = generateGroupBox("Target", TAB_NAME, TARGET_SPEED_CONFIG_MAP["Target"], "Target&Speed");
    auto                      targetLayout   = dynamic_cast<QGridLayout *>(targetGroupBox->layout());
    QPointer<QLabelEditGroup> EVThreL_C_tmp      = new QLabelEditGroup(targetGroupBox.get());
    EVThreL_C_tmp->setLabelValue("EVThreL_C");
    EVThreL_C_tmp->setValue(0);
    EVThreL_C_tmp->layout()->setContentsMargins(0, 0, 0, 0);
    connect(this, &QTabWndComm::readSignal, EVThreL_C_tmp, [EVThreL_C_tmp]() {
        if (!EVThreL_C_tmp->isVisible())
            return;
        auto gainL     = QUiDataMiddleware(TAB_NAME, "Gain L").readValueFromDevice();
        auto expoL     = QUiDataMiddleware(TAB_NAME, "Expo L").readValueFromDevice();
        auto evThreL_C = (static_cast<uint64_t>(gainL) * (static_cast<uint64_t>(expoL)>>4)) >> 4;
        EVThreL_C_tmp->setValue(evThreL_C);
    });
    targetLayout->addWidget(EVThreL_C_tmp, 8, 1, 1, 1);
    this->EVThreL_C = EVThreL_C_tmp;
    layout->addWidget(targetGroupBox, 0, 0, 1, 1);
    layout->addWidget(generateGroupBox("Speed", TAB_NAME, TARGET_SPEED_CONFIG_MAP["Speed"], "Target&Speed"), 0, 1, 1, 1);
    layout->setColumnStretch(0, 1);
    layout->setColumnStretch(1, 1);
}

void QAECTab::initRoiSubTab(QPointer<QWidget> subTab)
{
    QPointer<QGridLayout> layout = new QGridLayout(subTab.get());
    subTab->setLayout(layout.get());
    QPointer<QLabelComboBox> roiType = new QLabelComboBox(subTab.get());
    roiType->layout()->setContentsMargins(0, 0, 0, 0);
    QStringList roiTypeList = { "L", "M", "S", "V" };
    roiType->setValue("RoiType", roiTypeList);
    layout->addWidget(roiType, 0, 0, 1, 1);
    this->roiType = roiType;
    QVector<QPair<QPointer<QWidget>, QPointer<QWidget>>> subTabSubList;

    QPointer<QHBoxLayout> subTabSubLeftLayout = new QHBoxLayout();
    layout->addLayout(subTabSubLeftLayout, 1, 0, 1, 1);
    QPointer<QHBoxLayout> subTabSubRightLayout = new QHBoxLayout();
    layout->addLayout(subTabSubRightLayout, 0, 1, 2, 1);

    for (int i = 0; i < roiTypeList.size(); i++) {
        QPointer<QWidget> subTabLeftSub  = new QWidget(subTab.get());
        QPointer<QWidget> subTabRightSub = new QWidget(subTab.get());
        subTabSubList.push_back({ subTabLeftSub, subTabRightSub });
        initRoiSubTabLeftSub(subTabLeftSub, TAB_NAME + "_" + roiTypeList[i]);
        initRoiSubTabRightSub(subTabRightSub, TAB_NAME + "_" + roiTypeList[i]);
        subTabSubLeftLayout->addWidget(subTabLeftSub);
        subTabSubRightLayout->addWidget(subTabRightSub);
        subTabLeftSub->hide();
        subTabRightSub->hide();
    }
    subTabSubList[0].first->show();
    subTabSubList[0].second->show();
    connect(roiType, &QLabelComboBox::indexChange, this, [=](int index) {
        for (int i = 0; i < subTabSubList.size(); i++) {
            subTabSubList[i].first->hide();
            subTabSubList[i].second->hide();
        }
        subTabSubList[index].first->show();
        subTabSubList[index].second->show();
        changeTab(2); // 更新ROI tab
    });
    layout->setColumnStretch(0, 1);
    layout->setColumnStretch(1, 6);
    layout->setRowStretch(0, 1);
    layout->setRowStretch(1, 10);
}

void QAECTab::initRoiSubTabLeftSub(QPointer<QWidget> subTab, QString tabName)
{
    QPointer<QGridLayout> layout = new QGridLayout(subTab.get());
    subTab->setLayout(layout.get());
    layout->addWidget(generateGroupBox("AEC ROI", tabName, ROI_CONFIG_MAP["AEC ROI"], "ROI", 40), 0, 0, 1, 1);
    layout->addWidget(generateGroupBox("ROI Block", tabName, ROI_CONFIG_MAP["ROI Block"], "ROI", 40), 1, 0, 1, 1);
    layout->setRowStretch(0, 1);
    layout->setRowStretch(1, 1);
}

void QAECTab::initRoiSubTabRightSub(QPointer<QWidget> subTab, QString tabName)
{
    QPointer<QGridLayout> layout = new QGridLayout(subTab.get());
    subTab->setLayout(layout.get());
    auto roiWeightGroup = generateGroupBox("", tabName, ROI_CONFIG_MAP["roi_weight"], "ROI");
    layout->addWidget(roiWeightGroup, 0, 0, 0, 0);
    auto                      roiWeightLayout = dynamic_cast<QGridLayout *>(roiWeightGroup->layout());
    auto roiWeightTable  = new QTableListGroup();
    roiWeightLayout->addWidget(roiWeightTable, 1, 0, 1, 2);
    roiWeightLayout->setRowStretch(0, 1);
    roiWeightLayout->setRowStretch(1, 12);
    roiWeightLayout->setColumnStretch(0, 1);
    roiWeightLayout->setColumnStretch(1, 3);
    roiWeightTable->setMinimumSize(0, 0);
    roiWeightTable->setVisible(true);
    roiWeightTable->setHorizontalHeaderVisible(false);
    roiWeightTable->setVerticalHeaderVisible(false);
    roiWeightTables.push_back(roiWeightTable);
    
    connect(roiWeightTable, &QTableListGroup::sendCurrentItem, this, [=](QStandardItem *item, int row, int column) {
        auto roi_blks = QUiDataMiddleware(tabName, "roi_blk_w").getCombinedRegisterList();
        updateRegisterTable({ roi_blks[row * roiWeightTable->columnCount() + column] });
    });
}

std::vector<CombinedRegister> QAECTab::combinedRegisterSubPageRecording(QString strModuleName, QString strUiName, std::shared_ptr<int> ptrCamera, QString uiModuleName, QString strSubPage) 
{
    std::vector<UiInfo> uiInfo;
    uiInfo = DatabaseOrm::getUiInfoByModuleNameAndControlName(uiModuleName.toStdString(), strUiName.toStdString());
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

std::vector<CombinedRegister> QAECTab::getCombinedRegister(QString strModuleName, QString strUiName, std::shared_ptr<int> ptrCamera, QString uiModuleName)
{
    std::vector<UiInfo> uiInfo;
    uiInfo = DatabaseOrm::getUiInfoByModuleNameAndControlName(uiModuleName.toStdString(), strUiName.toStdString());
    std::vector<CombinedRegister> combinevector;
    for (auto i : uiInfo) {
        CombinedRegister combineReg
            = DatabaseOrm::getCombinedRegisterByModuleNameAndCameraIdAndCombinedName(
                strModuleName.toStdString(), ptrCamera, *(i.combinedName));
        combinevector.push_back(combineReg);
    }
    return combinevector;
}

void QAECTab::recordCombineRegbyUIifo(QString moudle_name, QString uiname, std::shared_ptr<int> cameraId, QString uiModuleName)
{
    std::vector<UiInfo> uiInfo;
    uiInfo= DatabaseOrm::getUiInfoByModuleNameAndControlName(uiModuleName.toStdString(), uiname.toStdString());
    std::vector<CombinedRegister> combinevector;
    for (auto i : uiInfo) {
        CombinedRegister combineReg
            = DatabaseOrm::getCombinedRegisterByModuleNameAndCameraIdAndCombinedName(
                moudle_name.toStdString(), cameraId, *(i.combinedName));
        combinevector.push_back(combineReg);
        //qDebug() << "combineReg: " << combineReg.combinedId << "," << combineReg.moduleName.c_str() << "," << combineReg.combinedName.c_str();
    }
    qDebug() << moudle_name + "_" + uiname + ": " << combinevector.size();
    combinedRegisterMap.insert(uiModuleName + "_" + uiname, combinevector);
}

void QAECTab::recordUIRegister()
{
    combinedRegisterMap.clear();
    
    if (expTypeCombo) {
        auto combineRegs = getUiInfoRegModuleByUiModuleNameAndUiControlName(TAB_NAME.toStdString(), "AECManEnable");
        QString qstrModuleName = combineRegs[0].regModuleName.c_str();
        recordCombineRegbyUIifo(qstrModuleName, "AECManEnable", std::make_shared<int>(getCurrentCamera()), TAB_NAME);
    }

    // 记录Exp&Gain tab的控件寄存器信息
    for (auto edit : expGainEdit) {
        if (edit) {
            QString label = edit->Label().remove("*");
            auto combineRegs = getUiInfoRegModuleByUiModuleNameAndUiControlName(TAB_NAME.toStdString(), label.toStdString());
            QString qstrModuleName = combineRegs[0].regModuleName.c_str();
            recordCombineRegbyUIifo(qstrModuleName, label, std::make_shared<int>(getCurrentCamera()), TAB_NAME);
            if (RGBIR_ENABLE_MAP.contains({TAB_NAME, label})) {
                recordCombineRegbyUIifo(qstrModuleName + "_IR_AB", label + " IR_AB", std::make_shared<int>(getCurrentCamera()), TAB_NAME);
                recordCombineRegbyUIifo(qstrModuleName + "_RGB_AB", label + " RGB_AB", std::make_shared<int>(getCurrentCamera()), TAB_NAME);
            }
        }
    }

    for (auto check : expGainCheck) {
        if (check) {
            QString label = check->Label().remove("*");
            auto combineRegs = getUiInfoRegModuleByUiModuleNameAndUiControlName(TAB_NAME.toStdString(), label.toStdString());
            QString qstrModuleName = combineRegs[0].regModuleName.c_str();
            recordCombineRegbyUIifo(qstrModuleName, label, std::make_shared<int>(getCurrentCamera()), TAB_NAME);
            if (RGBIR_ENABLE_MAP.contains({TAB_NAME, label})) {
                recordCombineRegbyUIifo(qstrModuleName + "_IR_AB", label + " IR_AB", std::make_shared<int>(getCurrentCamera()), TAB_NAME);
                recordCombineRegbyUIifo(qstrModuleName + "_RGB_AB", label + " RGB_AB", std::make_shared<int>(getCurrentCamera()), TAB_NAME);
            }
        }
    }
    
    // 记录Target&Speed tab的控件寄存器信息
    if (EVThreL_C) {
        recordCombineRegbyUIifo("PipeCtrl", "Gain L", std::make_shared<int>(getCurrentCamera()), TAB_NAME);
        recordCombineRegbyUIifo("PipeCtrl", "Expo L", std::make_shared<int>(getCurrentCamera()), TAB_NAME);
    }

    for (auto edit : targetEdit) {
        if (edit) {
            QString label = edit->Label().remove("*");
            auto combineRegs = getUiInfoRegModuleByUiModuleNameAndUiControlName(TAB_NAME.toStdString(), label.toStdString());
            QString qstrModuleName = combineRegs[0].regModuleName.c_str();
            recordCombineRegbyUIifo(qstrModuleName, label, std::make_shared<int>(getCurrentCamera()), TAB_NAME);
            if (RGBIR_ENABLE_MAP.contains({TAB_NAME, label})) {
                recordCombineRegbyUIifo(qstrModuleName + "_IR_AB", label + " IR_AB", std::make_shared<int>(getCurrentCamera()), TAB_NAME);
                recordCombineRegbyUIifo(qstrModuleName + "_RGB_AB", label + " RGB_AB", std::make_shared<int>(getCurrentCamera()), TAB_NAME);
            }
        }
    }
    
    for (auto edit : speedEdit) {
        if (edit) {
            QString label = edit->Label().remove("*");
            auto combineRegs = getUiInfoRegModuleByUiModuleNameAndUiControlName(TAB_NAME.toStdString(), label.toStdString());
            QString qstrModuleName = combineRegs[0].regModuleName.c_str();
            recordCombineRegbyUIifo(qstrModuleName, label, std::make_shared<int>(getCurrentCamera()), TAB_NAME);
            if (RGBIR_ENABLE_MAP.contains({TAB_NAME, label})) {
                recordCombineRegbyUIifo(qstrModuleName + "_IR_AB", label + " IR_AB", std::make_shared<int>(getCurrentCamera()), TAB_NAME);
                recordCombineRegbyUIifo(qstrModuleName + "_RGB_AB", label + " RGB_AB", std::make_shared<int>(getCurrentCamera()), TAB_NAME);
            }
        }
    }
    
    // 记录ROI tab的控件寄存器信息

    std::vector<std::vector<QPointer<QLabelEditGroup>>> vecRoiEditList({ roiEditL, roiEditM, roiEditS, roiEditV });
    std::vector<QString> roiTypeList = { "L", "M", "S", "V" };
    for (int i = 0; i < vecRoiEditList.size(); i++) {
        auto roiEdit = vecRoiEditList[i];
        for (auto edit : roiEdit) {
            if (edit) {
                QString label = edit->Label().remove("*");
                auto combineRegs = getUiInfoRegModuleByUiModuleNameAndUiControlName((TAB_NAME + "_" + roiTypeList[i]).toStdString(), label.toStdString());
                QString qstrModuleName = combineRegs[0].regModuleName.c_str();
                qDebug() << qstrModuleName + "_" + label;
                recordCombineRegbyUIifo(qstrModuleName, label, nullptr, TAB_NAME + "_" + roiTypeList[i]);
                if (RGBIR_ENABLE_MAP.contains({TAB_NAME, label})) {
                    recordCombineRegbyUIifo(qstrModuleName + "_IR_AB", label + " IR_AB", nullptr, TAB_NAME + "_" + roiTypeList[i]);
                    recordCombineRegbyUIifo(qstrModuleName + "_RGB_AB", label + " RGB_AB", nullptr, TAB_NAME + "_" + roiTypeList[i]);
                }
            }
        }
    }

    for (int i = 0; i < roiWeightEnableCheck.size(); i++)
        if (roiWeightEnableCheck[i])
        {
            QString label = roiWeightEnableCheck[i]->Label().remove("*");
            auto combineRegs = getUiInfoRegModuleByUiModuleNameAndUiControlName((TAB_NAME + "_" + roiTypeList[i]).toStdString(), label.toStdString());
            QString qstrModuleName = combineRegs[0].regModuleName.c_str();
            recordCombineRegbyUIifo(qstrModuleName, label, nullptr, TAB_NAME + "_" + roiTypeList[i]);
        }

    for (auto &suffix : roiTypeList) {
        QString tabName = TAB_NAME + "_" + suffix;
        auto roi_blks = QUiDataMiddleware(tabName, "roi_blk_w").getCombinedRegisterList();
        combinedRegisterMap.insert(tabName + "_roi_blk_w", std::vector<DatabaseOrm::CombinedRegister>(roi_blks.begin(), roi_blks.end()));
    }
}

void QAECTab::recordUIRegSubPageInfo()
{
    // 记录Exp&Gain tab的控件寄存器信息
    if (expTypeCombo) {
        auto combineRegs = getUiInfoRegModuleByUiModuleNameAndUiControlName(TAB_NAME.toStdString(), "AECManEnable");
        QString qstrModuleName = combineRegs[0].regModuleName.c_str();
        combinedRegisterSubPageRecording(qstrModuleName, "AECManEnable", std::make_shared<int>(getCurrentCamera()), TAB_NAME, "Exp&Gain");
    }

    for (auto edit : expGainEdit) {
        if (edit) {
            QString label = edit->Label().remove("*");
            auto combineRegs = getUiInfoRegModuleByUiModuleNameAndUiControlName(TAB_NAME.toStdString(), label.toStdString());
            QString qstrModuleName = combineRegs[0].regModuleName.c_str();
            auto vecRegs = combinedRegisterSubPageRecording(qstrModuleName, label, std::make_shared<int>(getCurrentCamera()), TAB_NAME, "Exp&Gain");
            if (RGBIR_ENABLE_MAP.contains({TAB_NAME, label})) {
                auto vecIrRegs = getCombinedRegister(qstrModuleName + "_IR_AB", label + " IR_AB", std::make_shared<int>(getCurrentCamera()), TAB_NAME);
                auto vecRgbRegs = getCombinedRegister(qstrModuleName + "_RGB_AB", label + " RGB_AB", std::make_shared<int>(getCurrentCamera()), TAB_NAME);
                setRegisterTypeMap(vecRegs, vecRgbRegs, vecIrRegs);
            }
        }
    }

    for (auto check : expGainCheck) {
        if (check) {
            QString label = check->Label().remove("*");
            auto combineRegs = getUiInfoRegModuleByUiModuleNameAndUiControlName(TAB_NAME.toStdString(), label.toStdString());
            QString qstrModuleName = combineRegs[0].regModuleName.c_str();
            auto vecRegs = combinedRegisterSubPageRecording(qstrModuleName, label, std::make_shared<int>(getCurrentCamera()), TAB_NAME, "Exp&Gain");
            if (RGBIR_ENABLE_MAP.contains({TAB_NAME, label})) {
                auto vecIrRegs = getCombinedRegister(qstrModuleName + "_IR_AB", label + " IR_AB", std::make_shared<int>(getCurrentCamera()), TAB_NAME);
                auto vecRgbRegs = getCombinedRegister(qstrModuleName + "_RGB_AB", label + " RGB_AB", std::make_shared<int>(getCurrentCamera()), TAB_NAME);
                setRegisterTypeMap(vecRegs, vecRgbRegs, vecIrRegs);
            }
        }
    }
    
    // 记录Target&Speed tab的控件寄存器信息
    if (EVThreL_C) {
        combinedRegisterSubPageRecording(TAB_NAME, "Gain L", std::make_shared<int>(getCurrentCamera()), TAB_NAME, "Target&Speed");
        combinedRegisterSubPageRecording(TAB_NAME, "Expo L", std::make_shared<int>(getCurrentCamera()), TAB_NAME, "Target&Speed");
    }

    for (auto edit : targetEdit) {
        if (edit) {
            QString label = edit->Label().remove("*");
            auto combineRegs = getUiInfoRegModuleByUiModuleNameAndUiControlName(TAB_NAME.toStdString(), label.toStdString());
            QString qstrModuleName = combineRegs[0].regModuleName.c_str();
            auto vecRegs = combinedRegisterSubPageRecording(qstrModuleName, label, std::make_shared<int>(getCurrentCamera()), TAB_NAME, "Target&Speed");
            if (RGBIR_ENABLE_MAP.contains({TAB_NAME, label})) {
                auto vecIrRegs = getCombinedRegister(qstrModuleName + "_IR_AB", label + " IR_AB", std::make_shared<int>(getCurrentCamera()), TAB_NAME);
                auto vecRgbRegs = getCombinedRegister(qstrModuleName + "_RGB_AB", label + " RGB_AB", std::make_shared<int>(getCurrentCamera()), TAB_NAME);
                setRegisterTypeMap(vecRegs, vecRgbRegs, vecIrRegs);
            }
        }
    }
    
    for (auto edit : speedEdit) {
        if (edit) {
            QString label = edit->Label().remove("*");
            auto combineRegs = getUiInfoRegModuleByUiModuleNameAndUiControlName(TAB_NAME.toStdString(), label.toStdString());
            QString qstrModuleName = combineRegs[0].regModuleName.c_str();
            auto vecRegs = combinedRegisterSubPageRecording(qstrModuleName, label, std::make_shared<int>(getCurrentCamera()), TAB_NAME, "Target&Speed");
            if (RGBIR_ENABLE_MAP.contains({TAB_NAME, label})) {
                auto vecIrRegs = getCombinedRegister(qstrModuleName + "_IR_AB", label + " IR_AB", std::make_shared<int>(getCurrentCamera()), TAB_NAME);
                auto vecRgbRegs = getCombinedRegister(qstrModuleName + "_RGB_AB", label + " RGB_AB", std::make_shared<int>(getCurrentCamera()), TAB_NAME);
                setRegisterTypeMap(vecRegs, vecRgbRegs, vecIrRegs);
            }
        }
    }
    
    // 记录ROI tab的控件寄存器信息
    std::vector<std::vector<QPointer<QLabelEditGroup>>> vecRoiEditList({ roiEditL, roiEditM, roiEditS, roiEditV });
    std::vector<QString> roiTypeList = { "L", "M", "S", "V" };

    for (int i = 0; i < vecRoiEditList.size(); i++) {
        auto roiEdit = vecRoiEditList[i];
        for (auto edit : roiEdit) {
            if (edit) {
                QString label = edit->Label().remove("*");
                auto combineRegs = getUiInfoRegModuleByUiModuleNameAndUiControlName((TAB_NAME + "_" + roiTypeList[i]).toStdString(), label.toStdString());
                QString qstrModuleName = combineRegs[0].regModuleName.c_str();
                auto vecRegs = combinedRegisterSubPageRecording(qstrModuleName, label, nullptr, TAB_NAME + "_" + roiTypeList[i], "ROI");
                if (RGBIR_ENABLE_MAP.contains({TAB_NAME, label})) {
                    auto vecIrRegs = getCombinedRegister(qstrModuleName + "_IR_AB", label + " IR_AB", nullptr, TAB_NAME + "_" + roiTypeList[i]);
                    auto vecRgbRegs = getCombinedRegister(qstrModuleName + "_RGB_AB", label + " RGB_AB", nullptr, TAB_NAME + "_" + roiTypeList[i]);
                    setRegisterTypeMap(vecRegs, vecRgbRegs, vecIrRegs);
                }
            }
        }
    }

    for (int i = 0; i < roiWeightEnableCheck.size(); i++)
        if (roiWeightEnableCheck[i])
        {
            QString label = roiWeightEnableCheck[i]->Label().remove("*");
            auto combineRegs = getUiInfoRegModuleByUiModuleNameAndUiControlName((TAB_NAME + "_" + roiTypeList[i]).toStdString(), label.toStdString());
            QString qstrModuleName = combineRegs[0].regModuleName.c_str();
            combinedRegisterSubPageRecording(qstrModuleName, label, nullptr, "ROI", TAB_NAME + "_" + roiTypeList[i]);
        }

    for (auto &suffix : roiTypeList) {
        QString tabName = TAB_NAME + "_" + suffix;
        auto roi_blks = QUiDataMiddleware(tabName, "roi_blk_w").getCombinedRegisterList();
        for (auto reg : roi_blks)
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
            setRegisterSubPage(reg, "ROI");
        }
    }
}

void QAECTab::changeTab(int index)
{
    switch (index) 
    {
    case 0: // Exp&Gain tab
        updateUIValue<QLabelEditGroup>(expGainEdit.data(), expGainEdit.size(), "", TAB_NAME);
        updateUIValue<QLabelCheckBox>(expGainCheck.data(), expGainCheck.size(), "", TAB_NAME);
        if (expTypeCombo) {
            uint32_t value = 0;
            auto reg = combinedRegisterMap.value(TAB_NAME + "_AECManEnable");
            for (int i = 0; i < reg.size(); i++) {
                readCombinedRegisterValue(reg.at(i), value);
                expTypeCombo->setCurrentIndex(value);
            }
        }
        break;
    case 1: // Target&Speed tab
        updateUIValue<QLabelEditGroup>(targetEdit.data(), targetEdit.size(), "", TAB_NAME);
        updateUIValue<QLabelEditGroup>(speedEdit.data(), speedEdit.size(), "", TAB_NAME);
        // 更新EVThreL_C
        if (EVThreL_C && EVThreL_C->isVisible()) {
            auto gainL = QUiDataMiddleware(TAB_NAME, "Gain L").readValueFromDevice();
            auto expoL = QUiDataMiddleware(TAB_NAME, "Expo L").readValueFromDevice();
            auto evThreL_C = (static_cast<uint64_t>(gainL) * (static_cast<uint64_t>(expoL) >> 4)) >> 4;
            EVThreL_C->setValue(evThreL_C);
        }
        break;
    case 2: // ROI tab
    {
        auto strRoiType = roiType->getValue(roiType->getCurrentIndex());
        if (strRoiType == "L")
            updateUIValue<QLabelEditGroup>(roiEditL.data(), roiEditL.size(), "", TAB_NAME + "_L");
        else if (strRoiType == "M")
            updateUIValue<QLabelEditGroup>(roiEditM.data(), roiEditM.size(), "", TAB_NAME + "_M");
        else if (strRoiType == "S")
            updateUIValue<QLabelEditGroup>(roiEditS.data(), roiEditS.size(), "", TAB_NAME + "_S");
        else if (strRoiType == "V")
            updateUIValue<QLabelEditGroup>(roiEditV.data(), roiEditV.size(), "", TAB_NAME + "_V");
        // 更新ROI权重表
        qDebug() << "changeTab roiType: " << strRoiType;
        auto roiWeightTable = roiWeightTables[roiType->getCurrentIndex()];
        if (roiWeightTable && roiWeightTable->isVisible()) {
            roi_width = QUiDataMiddleware(TAB_NAME + "_" + strRoiType, "roi_width").readValueFromDevice();
            roi_height = QUiDataMiddleware(TAB_NAME + "_" + strRoiType, "roi_height").readValueFromDevice();
            auto roi_blks = QUiDataMiddleware(TAB_NAME + "_" + strRoiType, "roi_blk_w").getCombinedRegisterList();
            
            if (roi_width == 0 && roi_height == 0) {
                roi_width = 16;
                roi_height = 12;
            }
            roi_width = fmin(roi_width, 16);
            roi_height = fmin(roi_height, 12);
            qDebug() << "roi_width: " << roi_width << ", roi_height: " << roi_height;
            qDebug() << "roi_blks size: " << roi_blks.size();
            roiWeightTable->clearAll();
            for (uint32_t i = 0; i < roi_height; i++) {
                for (uint32_t j = 0; j < roi_width; j++) {
                    uint32_t value = 0;
                    readCombinedRegisterValue(roi_blks[i * roi_width + j], value);
                    roiWeightTable->appendRow(i, j, QString("0x%1").arg(value, 2, 16, QChar('0')), Qt::AlignCenter);
                    if (i == 0)
                        roiWeightTable->horizontalHeader()->setSectionResizeMode(j, QHeaderView::Stretch);
                }
                roiWeightTable->verticalHeader()->setSectionResizeMode(i, QHeaderView::Stretch);
            }
        }
        else if (!roiWeightTable) {
            qDebug() << "roiWeightTable is null";
        }
        else if (!roiWeightTable->isVisible()) {
            qDebug() << "roiWeightTable is not visible";
        }
        break;
    }
    default:
        break;
    }
}

template <typename T>
void QAECTab::updateUIValue(QPointer<T> *vector, int iLength, const QString &extraname, QString strModuleName)
{
    int valTemp = 0;
    QUsbCommunication::getInstance().readBurst(IR_RGB_MODE_REG_ADDR, (unsigned char*)(&valTemp), ADDRESS_LENGTH, VALUE_LENGTH);
    for (int i = 0; i < iLength; i++) {
        uint32_t value = 0;
        QString strLabelOrigin = vector[i]->Label().remove("*") + extraname;

        // auto combineRegs = getUiInfoRegModuleByUiModuleNameAndUiControlName(strModuleName.toStdString(), strLabelOrigin.toStdString());
        // QString qstrModuleName = combineRegs[0].regModuleName.c_str();

        auto reg = combinedRegisterMap.value(strModuleName + "_" + strLabelOrigin);
        if (RGBIR_ENABLE_MAP.contains({ TAB_NAME, strLabelOrigin })) {
            
            if (valTemp == 0x01)
            {
                std::string strSuffix = strRgbIrMode == "RGB" ? "RGB_AB" : "IR_AB";
                reg = combinedRegisterMap.value(strModuleName + "_" + strLabelOrigin + " " + strSuffix.c_str());
            }
        }

        for (int j = 0; j < reg.size(); j++) {
            readCombinedRegisterValue(reg.at(j), value);
            if (std::is_same<T, QLabelEditGroup>::value)
                dynamic_cast<QLabelEditGroup*>(vector[i].get())->setValue(QString("0x%1").arg(QString::number(value, 16).toUpper().rightJustified(VALUE_HEX_INFO, '0')));
            if (std::is_same<T, QLabelCheckBox>::value)
            {
                bool bValue = (value != 0);
                dynamic_cast<QLabelCheckBox*>(vector[i].get())->setChecked(bValue);
                // dynamic_cast<QLabelEditGroup*>(vector[i].get())->setValue(QString("0x%1").arg(QString::number(value, 16).toUpper().rightJustified(VALUE_HEX_INFO, '0')));
            }
        }
    }
}

QPointer<QGroupBox> QAECTab::generateGroupBox(QString groupBoxName, QString uiModuleName, ConfigType config, QString strSubPage, int width)
{
    QPointer<QGroupBox>   groupBox = new QGroupBox(groupBoxName);
    QPointer<QGridLayout> layout   = new QGridLayout(groupBox.get());
    groupBox->setLayout(layout.get());
    for (int col = 0; col < config.size(); col++) {
        for (int row = 0; row < config[col].size(); row++) {
            if (RGBIR_ENABLE_MAP.contains({TAB_NAME, config[col][row].second})) {
                generateWidgetLabelEditGroupBoxRgbIr(groupBox, layout, uiModuleName, col, row, strSubPage, config[col][row], width);
            }
            else 
            {
                generateWidgetGroupBox(groupBox, layout, uiModuleName, col, row, strSubPage, config[col][row], width);
            }
        }
    }
    return groupBox;
}

void QAECTab::handleLabelEditGroup(QPointer<QGroupBox> groupBox, QPointer<QGridLayout> layout, QString uiModuleName, int col, int row, QString strSubPage,
    QPair<QString, QString> uiTypeAndName, int width)
{
    QPointer<QLabelEditGroup> editGroup = new QLabelEditGroup(groupBox.get());
    editGroup->setLabelValue(uiTypeAndName.second);
    
    QString strLabel = uiTypeAndName.second;
    
    // 添加到对应的控件列表中
    if (strSubPage == "Exp&Gain") {
        expGainEdit.push_back(editGroup);
    } else if (strSubPage == "Target&Speed") {
        if (strLabel.contains("Target")) {
            targetEdit.push_back(editGroup);
        } else {
            speedEdit.push_back(editGroup);
        }
    } else if (strSubPage == "ROI") {
        if (uiModuleName == TAB_NAME + "_L")
            roiEditL.push_back(editGroup);
        else if (uiModuleName == TAB_NAME + "_M")
            roiEditM.push_back(editGroup);
        else if (uiModuleName == TAB_NAME + "_S")
            roiEditS.push_back(editGroup);
        else if (uiModuleName == TAB_NAME + "_V")
            roiEditV.push_back(editGroup);
    }
    
    layout->addWidget(editGroup, row, col);
    editGroup->setMouseTracking(true);
    editGroup->setMinimumWidth(10);
    editGroup->setValue(0);
    editGroup->layout()->setContentsMargins(0, 0, 0, 0);
    editGroup->setEditWidth(width);
    
    // 在寄存器栏显示对应的combine register
    connect(editGroup, &QLabelEditGroup::releaseLabel, this,
        [=](QString str)
        {
            // auto combineRegs = getUiInfoRegModuleByUiModuleNameAndUiControlName(uiModuleName.toStdString(), strLabel.toStdString());
            // QString qstrModuleName = combineRegs[0].regModuleName.c_str();
            // recvEdit(qstrModuleName + "_" + strLabel);
            recvEdit(uiModuleName + "_" + strLabel);
        }, Qt::QueuedConnection);
    
    // 连接读取信号，读取不同模式对应reg的值
    // connect(this, &QTabWndComm::readSignal, editGroup, 
    //     [=]()
    //     {
    //         uint32_t regValue = QUiDataMiddleware(uiModuleName, strLabel).readValueFromDevice();
    //         editGroup->setValue(regValue);
    //     }, Qt::QueuedConnection);
    
    // 连接值变化信号
    connect(editGroup, &QLabelEditGroup::signalValueChange, this, 
        [=](uint32_t value)
        {
            auto combineRegs = getUiInfoRegModuleByUiModuleNameAndUiControlName(uiModuleName.toStdString(), strLabel.toStdString());
            QString qstrModuleName = combineRegs[0].regModuleName.c_str();
            std::vector<CombinedRegister> re = combinedRegisterMap.value(qstrModuleName + "_" + strLabel);
            for (auto it : re) 
            {
                writeCombinedRegisterValue(it, value);
            }
        }, Qt::QueuedConnection);
}

void QAECTab::handleLabelComboBox(QPointer<QGroupBox> groupBox, QPointer<QGridLayout> layout, QString uiModuleName, int col, int row, QString strSubPage,
    QPair<QString, QString> uiTypeAndName, int width)
{
    QPointer<QLabelComboBox> comboBox = new QLabelComboBox(groupBox.get());
    comboBox->setLabel(uiTypeAndName.second);
    
    QString strLabel = uiTypeAndName.second;
    
    layout->addWidget(comboBox, row, col);
    comboBox->setMouseTracking(true);
    comboBox->setMinimumWidth(10);
    comboBox->setValue(0);
    comboBox->layout()->setContentsMargins(0, 0, 0, 0);
    
    // 在寄存器栏显示对应的combine register
    connect(comboBox, &QLabelComboBox::release, this,
        [=](QString str)
        {
            // auto combineRegs = getUiInfoRegModuleByUiModuleNameAndUiControlName(uiModuleName.toStdString(), strLabel.toStdString());
            // QString qstrModuleName = combineRegs[0].regModuleName.c_str();
            recvEdit(uiModuleName + "_" + strLabel);
        }, Qt::QueuedConnection);
    
    // 连接读取信号，读取不同模式对应reg的值
    // connect(this, &QTabWndComm::readSignal, comboBox, 
    //     [=]()
    //     {
    //         uint32_t regValue = QUiDataMiddleware(uiModuleName, strLabel).readValueFromDevice();
    //         comboBox->setValue(regValue);
    //     }, Qt::QueuedConnection);
}

void QAECTab::handleLabelCheckBox(QPointer<QGroupBox> groupBox, QPointer<QGridLayout> layout, QString uiModuleName, int col, int row, QString strSubPage,
    QPair<QString, QString> uiTypeAndName, int width)
{
    QPointer<QLabelCheckBox> checkBox = new QLabelCheckBox(groupBox.get());
    checkBox->setLabel(uiTypeAndName.second);
    
    QString strLabel = uiTypeAndName.second;

    if (strSubPage == "Exp&Gain") {
        expGainCheck.push_back(checkBox);
    }
    else if (strSubPage == "ROI") {
        roiWeightEnableCheck.push_back(checkBox);
    }
    
    layout->addWidget(checkBox, row, col);
    checkBox->setMouseTracking(true);
    checkBox->setMinimumWidth(10);
    checkBox->setValue(0);
    checkBox->layout()->setContentsMargins(0, 0, 0, 0);
    
    // 在寄存器栏显示对应的combine register
    connect(checkBox, &QLabelCheckBox::release, this,
        [=](QString str)
        {
            // auto combineRegs = getUiInfoRegModuleByUiModuleNameAndUiControlName(uiModuleName.toStdString(), strLabel.toStdString());
            // QString qstrModuleName = combineRegs[0].regModuleName.c_str();
            recvEdit(uiModuleName + "_" + strLabel);
        }, Qt::QueuedConnection);
    
    // 连接值变化信号
    connect(checkBox, &QLabelCheckBox::checkSignal, this, 
        [=]()
        {
            uint32_t value = checkBox->checkState()
             ? 1 : 0;
            // auto combineRegs = getUiInfoRegModuleByUiModuleNameAndUiControlName(uiModuleName.toStdString(), strLabel.toStdString());
            // QString qstrModuleName = combineRegs[0].regModuleName.c_str();
            std::vector<CombinedRegister> re = combinedRegisterMap.value(uiModuleName + "_" + strLabel);
            for (auto it : re) 
            {
                writeCombinedRegisterValue(it, value);
            }
        }, Qt::QueuedConnection);
}

void QAECTab::generateWidgetGroupBox(QPointer<QGroupBox> groupBox, QPointer<QGridLayout> layout,
    QString uiModuleName, int col, int row, QString strSubPage, QPair<QString, QString> uiTypeAndName, int width)
{
    // 根据控件类型创建相应的控件
    if (uiTypeAndName.first == LABEL_EDIT_GROUP) {
        handleLabelEditGroup(groupBox, layout, uiModuleName, col, row, strSubPage, uiTypeAndName, width);
    } else if (uiTypeAndName.first == LABEL_COMBO_BOX) {
        handleLabelComboBox(groupBox, layout, uiModuleName, col, row, strSubPage, uiTypeAndName, width);
    } else if (uiTypeAndName.first == LABEL_CHECK_BOX) {
        handleLabelCheckBox(groupBox, layout, uiModuleName, col, row, strSubPage, uiTypeAndName, width);
    }
}

void QAECTab::generateWidgetLabelEditGroupBoxRgbIr(QPointer<QGroupBox> groupBox, QPointer<QGridLayout> layout,
    QString uiModuleName, int col, int row, QString strSubPage, QPair<QString, QString> uiTypeAndName, int width)
{
    // QPointer<QCommonUiInterface> widget = QCommonUiInterfaceFactory::instance().createUiInterface(
    //             uiTypeAndName.first, uiModuleName, uiTypeAndName.second, groupBox.get());
    // QPointer<QCommonUiInterface> widget;
    if (uiTypeAndName.first == LABEL_EDIT_GROUP)
    {
        // QLabelEditGroup edit;
        // edit.setLabelValue(uiTypeAndName.second);
        // vecSpecialEdit.push_back(edit);
        // QPointer<QCommonUiInterface> widget = vecSpecialEdit.data() + (vecSpecialEdit.size() - 1);
        QPointer<QLabelEditGroup> widget = new QLabelEditGroup();
        widget->setLabelValue(uiTypeAndName.second);
        QString strLabel = uiTypeAndName.second;

        if (strSubPage == "Exp&Gain") {
            if (dynamic_cast<QLabelEditGroup *>(widget.get()) != nullptr) {
                expGainEdit.push_back(dynamic_cast<QLabelEditGroup *>(widget.get()));
            }
        } else if (strSubPage == "Target&Speed") {
            if (strLabel.contains("Target")) {
                targetEdit.push_back(dynamic_cast<QLabelEditGroup *>(widget.get()));
            } else {
                speedEdit.push_back(dynamic_cast<QLabelEditGroup *>(widget.get()));
            }
        } else if (strSubPage == "ROI") {
            if (uiModuleName == TAB_NAME + "_L")
                roiEditL.push_back(dynamic_cast<QLabelEditGroup *>(widget.get()));
            else if (uiModuleName == TAB_NAME + "_M")
                roiEditM.push_back(dynamic_cast<QLabelEditGroup *>(widget.get()));
            else if (uiModuleName == TAB_NAME + "_S")
                roiEditS.push_back(dynamic_cast<QLabelEditGroup *>(widget.get()));
            else if (uiModuleName == TAB_NAME + "_V")
                roiEditV.push_back(dynamic_cast<QLabelEditGroup *>(widget.get()));
        }

        layout->addWidget(widget.get(), row, col);
        widget->setMouseTracking(true);
        widget->setMinimumWidth(10);
        widget->setValue(0);
        widget->layout()->setContentsMargins(0, 0, 0, 0);
        
        //在寄存器栏显示对应的combine register
        connect(widget, &QLabelEditGroup::releaseLabel, this,
            [=](QString str)
            {
                // auto combineRegs = getUiInfoRegModuleByUiModuleNameAndUiControlName(uiModuleName.toStdString(), strLabel.toStdString());
                // QString qstrModuleName = combineRegs[0].regModuleName.c_str();
                recvEditTarget(strLabel, uiModuleName);
            }, Qt::QueuedConnection);
        
        // 如果是QLabelEditGroup类型的控件，设置编辑宽度和标签值
        // 并连接信号以更新寄存器值
        // if (dynamic_cast<QLabelEditGroup *>(widget.get()) != nullptr){
        (widget.get())->setEditWidth(width);
        (widget.get())->setLabelValue("*" + uiTypeAndName.second);
        // qDebug() << "binding combined reg size: " << (widget.get())->getCombineRegister().size();
        connect((widget.get()), &QLabelEditGroup::signalValueChange, this, 
            [=](uint32_t value)
            {
                int valTemp = 0;
                QUsbCommunication::getInstance().readBurst(IR_RGB_MODE_REG_ADDR, (unsigned char*)(&valTemp), ADDRESS_LENGTH, VALUE_LENGTH);
                std::vector<CombinedRegister> re;
                std::string strSuffix = strRgbIrMode == "RGB" ? "RGB_AB" : "IR_AB";
                qDebug() << "valTemp: " << valTemp;
                // auto combineRegs = getUiInfoRegModuleByUiModuleNameAndUiControlName(uiModuleName.toStdString(), strLabel.toStdString());
                // QString qstrModuleName = combineRegs[0].regModuleName.c_str();
                if (valTemp == 0x01)
                {
                    re = combinedRegisterMap.value(uiModuleName + "_" + strLabel + " " + strSuffix.c_str());
                }
                else 
                {
                    re = combinedRegisterMap.value(uiModuleName + "_" + strLabel);
                }
                for (auto it : re) 
                {
                    qDebug() << "RegModuleName: " << it.moduleName.c_str();
                    writeCombinedRegisterValue(it, value);
                }
                clearCurrentRigester();
                updateRegisterTable(re);
            }, Qt::QueuedConnection);
        // }
    }
}
