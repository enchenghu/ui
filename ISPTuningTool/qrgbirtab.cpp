#include "qrgbirtab.h"
#include "Common/UI/quidatamiddleware.h"
#include <QComboBox>
#include <QPointer>
#include "qtransfermanager.h"
#include "Common/Others/qusbcommunication.h"

QRGBIRTab::QRGBIRTab(QWidget *parent)
{
    int iMode = TransferManager::getInstance().getRgbIrMode();
    strRgbIrMode = iMode == 0 ? "RGB" : "IR";
    tabComm    = new QTabWidget(); // 控制主界面唯一tab
    moduleName = "RGBIR";
    DelRMrx0   = new QTableListGroup();
    DelRMrx1   = new QTableListGroup();
    DelRMrx2   = new QTableListGroup();
    comboBox   = new QComboBox();
    strDelRegListTxt = QCoreApplication::applicationDirPath() + "\\extracted_data\\RGBIR.txt";
    loadDeletedRegistersFromFile(strDelRegListTxt);
    init();
    initConnect();

}

QRGBIRTab::~QRGBIRTab() { }

void QRGBIRTab::init()
{
    addTabWidget(tabComm);
    QStringList tabList;
    if(m_nLevel==LEVEL_ADMINISTRATOR)
    {
        tabList << "CIP"
                << "DPC";
    }
    else
    {
        tabList << "CIP";
    }
    int iCount = 0;
    for (int i = 0; i < tabList.size(); i++) {
        QPointer<QWidget> _tab = new QWidget;
        tabComm->insertTab(i, _tab, tabList.at(i));
        tabMap.insert(tabList.at(i), std::make_pair(_tab, false));
        subPageMap.insert(iCount, tabList[i]);
        iCount++;
    }
    tableMap.insert(0, std::make_pair("DelRMrx[0]", DelRMrx0));
    tableMap.insert(1, std::make_pair("DelRMrx[1]", DelRMrx1));
    tableMap.insert(2, std::make_pair("DelRMrx[2]", DelRMrx2));
    for (int i = 0; i < 2; i++)
        changeTab(i);
    recordUIRegister();
    recordUIRegSubPageInfo();
    addRegisterTable();
    labelSpinSliderCheckBoxGroup->setRealEdit(true);
    labelSpinSliderCheckBoxGroup->setFloatIntType(FloatIntType::Type_1_0x200);
}

void QRGBIRTab::updateValue()
{
    changeTab(tabComm->currentIndex());
}

void QRGBIRTab::initConnect()
{
    connect(tabComm, &QTabWidget::currentChanged, this, &QRGBIRTab::changeTab);
    connect(this, &QTabWndComm::readSignal, this, [=]() { updateValue(); });
    connect(this, &QTabWndComm::reConnectRead, this, [=]() {
        recordUIRegister();
        // recordUIRegSubPageInfo();
        updateValue();
    });
    connect(this, &QTabWndComm::updateValue, this, [=]() { updateValue(); });
    connect(&TransferManager::getInstance(), &TransferManager::rgbIrModeChanged, 
        this, &QRGBIRTab::changeRgbIrMode);
}

void QRGBIRTab::recordUIRegister()
{
    combinedRegisterMap.clear();
    QStringList comboStringList;
    comboStringList << "G-Blk"
                    << "G-Wht"
                    << "N-Blk"
                    << "N-Wht";
    // CIP
    for (auto item : CIP_box)
        recordCombineRegbyUIifo("RGBIRCIP_LM", item->Label(), std::make_shared<int>(getCurrentCamera()));
    for (auto item : CIP_edit)
        recordCombineRegbyUIifo("RGBIRCIP_LM", item->Label(), std::make_shared<int>(getCurrentCamera()));
    // DPC
    for (auto item : DPC_box)
        recordCombineRegbyUIifo("RGBIRCIP_LM", item->Label(), std::make_shared<int>(getCurrentCamera()));
    for (auto item : DPC_edit)
        recordCombineRegbyUIifo("RGBIRCIP_LM", item->Label(), std::make_shared<int>(getCurrentCamera()));
    // for (auto item : DPC_Coe1edit)
    //     recordCombineRegbyUIifo("RGBIRCIP_LM", item->Label(), std::make_shared<int>(getCurrentCamera()));
    // for (auto item : DPC_Coe2edit)
    //     recordCombineRegbyUIifo("RGBIRCIP_LM", item->Label(), std::make_shared<int>(getCurrentCamera()));
    // for (auto item : DPC_Bpintedit)
    //     recordCombineRegbyUIifo("RGBIRCIP_LM", item->Label(), std::make_shared<int>(getCurrentCamera()));
    for (int i = 0; i < 8; i++)
    {
        for (auto iter : comboStringList)
        {
            recordCombineRegbyUIifo("RGBIRCIP_LM", iter + "Coe1" + QString::number(i), std::make_shared<int>(getCurrentCamera()));
        }
    }
        
    for (int i = 0; i < 8; i++)
    {
        for (auto iter : comboStringList)
        {
            recordCombineRegbyUIifo("RGBIRCIP_LM", iter + "Coe2" + QString::number(i), std::make_shared<int>(getCurrentCamera()));
        }
    }
        
    for (int i = 0; i < 8; i++)
    {
        for (auto iter : comboStringList)
        {
            recordCombineRegbyUIifo("RGBIRCIP_LM", iter + "BpInt" + QString::number(i), std::make_shared<int>(getCurrentCamera()));
        }
    }

    for (auto iter = tableMap.begin(); iter != tableMap.end(); iter++) {
        QString tabelName = iter.value().first;
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 4; ++j)
            {
                recordCombineRegbyUIifo("RGBIRCIP_LM", (tabelName + QString::number(i) + QString::number(j)),
                    std::make_shared<int>(getCurrentCamera()));
                if (RGBIR_RGBIR_ENABLE_MAP.contains({"RGBIR", tabelName + QString::number(i) + QString::number(j)})) 
                {
                    recordCombineRegbyUIifo("RGBIRCIP_LM_IR_AB", (tabelName + QString::number(i) + QString::number(j) + "_IR_AB"),
                        std::make_shared<int>(getCurrentCamera()));
                    recordCombineRegbyUIifo("RGBIRCIP_LM_RGB_AB", (tabelName + QString::number(i) + QString::number(j) + "_RGB_AB"),
                        std::make_shared<int>(getCurrentCamera()));
                }
            }
        }
    }
    // saveCombinedReg();
}

std::vector<CombinedRegister> QRGBIRTab::combinedRegisterSubPageRecording(QString strModuleName, QString strUiName, std::shared_ptr<int> ptrCamera, QString strSubPage) 
{
    std::vector<UiInfo> uiInfo;
    // qDebug() << strModuleName << ", " << strUiName;
    uiInfo= DatabaseOrm::getUiInfoByModuleNameAndControlName(moduleName, strUiName.toStdString());
    std::vector<CombinedRegister> combinevector;
    // qDebug() << "uiInfo size: " << uiInfo.size();
    for (auto i : uiInfo) {
        CombinedRegister combineReg
            = DatabaseOrm::getCombinedRegisterByModuleNameAndCameraIdAndCombinedName(
                strModuleName.toStdString(), ptrCamera, *(i.combinedName));
        combinevector.push_back(combineReg);
    }
    // qDebug() << "combinevector size: " << combinevector.size();
    for (auto reg : combinevector)
    {
        auto reBitInfo = DatabaseOrm::getRegisterBitInfoByCombinedRegister(reg);
        // qDebug() << "reBitInfo size: " << reBitInfo.size();
        for (int i = 0; i < reBitInfo.size(); i++)
        {
            regAddrModule.insert(reBitInfo[i].registerAddr);
        }
        std::vector<uint32_t>().swap(registerMoudle);
        // qDebug() << "clear.";
        for (auto iter : regAddrModule)
        {
            registerMoudle.emplace_back(iter);
        }
        // qDebug() << reg.combinedName.c_str() << "," << strSubPage;
        setRegisterSubPage(reg, strSubPage);
    }
    // qDebug() << "registerMoudle size: " << registerMoudle.size();

    return combinevector;
}

std::vector<CombinedRegister> QRGBIRTab::getCombinedRegister(QString strModuleName, QString strUiName, std::shared_ptr<int> ptrCamera)
{
    std::vector<UiInfo> uiInfo;
    // qDebug() << strModuleName << ", " << strUiName;
    uiInfo= DatabaseOrm::getUiInfoByModuleNameAndControlName(moduleName, strUiName.toStdString());
    std::vector<CombinedRegister> combinevector;
    for (auto i : uiInfo) {
        CombinedRegister combineReg
            = DatabaseOrm::getCombinedRegisterByModuleNameAndCameraIdAndCombinedName(
                strModuleName.toStdString(), ptrCamera, *(i.combinedName));
        combinevector.push_back(combineReg);
    }
    return combinevector;
}

void QRGBIRTab::recordUIRegSubPageInfo()
{
    // CIP
    QStringList comboStringList;
    comboStringList << "G-Blk"
                    << "G-Wht"
                    << "N-Blk"
                    << "N-Wht";
    for (auto item : CIP_box)
        combinedRegisterSubPageRecording("RGBIRCIP_LM", item->Label(), std::make_shared<int>(getCurrentCamera()), "CIP");
    for (auto item : CIP_edit)
    {
        auto vecRegs = combinedRegisterSubPageRecording("RGBIRCIP_LM", item->Label(), std::make_shared<int>(getCurrentCamera()), "CIP");
        if (RGBIR_RGBIR_ENABLE_MAP.contains({moduleName.c_str(), item->Label()})) 
        {
            auto vecIrRegs = getCombinedRegister("RGBIRCIP_LM_IR_AB", item->Label() + "_IR_AB", std::make_shared<int>(getCurrentCamera()));
            auto vecRgbRegs = getCombinedRegister("RGBIRCIP_LM_RGB_AB", item->Label() + "_RGB_AB", std::make_shared<int>(getCurrentCamera()));
            setRegisterTypeMap(vecRegs, vecRgbRegs, vecIrRegs);
        }
    }
        
    // DPC
    for (auto item : DPC_box)
        combinedRegisterSubPageRecording("RGBIRCIP_LM", item->Label(), std::make_shared<int>(getCurrentCamera()), "DPC");
    for (auto item : DPC_edit)
        combinedRegisterSubPageRecording("RGBIRCIP_LM", item->Label(), std::make_shared<int>(getCurrentCamera()), "DPC");
    for (int i = 0; i < 8; i++)
    {
        for (auto iter : comboStringList)
        {
            combinedRegisterSubPageRecording("RGBIRCIP_LM", iter + "Coe1" + QString::number(i), std::make_shared<int>(getCurrentCamera()), "DPC");
        }
    }
        
    for (int i = 0; i < 8; i++)
    {
        for (auto iter : comboStringList)
        {
            combinedRegisterSubPageRecording("RGBIRCIP_LM", iter + "Coe2" + QString::number(i), std::make_shared<int>(getCurrentCamera()), "DPC");
        }
    }
        
    for (int i = 0; i < 8; i++)
    {
        for (auto iter : comboStringList)
        {
            combinedRegisterSubPageRecording("RGBIRCIP_LM", iter + "BpInt" + QString::number(i), std::make_shared<int>(getCurrentCamera()), "DPC");
        }
    }
        

    for (auto iter = tableMap.begin(); iter != tableMap.end(); iter++) {
        QString tabelName = iter.value().first;
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 4; ++j)
            {
                QString itemName = tabelName + QString::number(i) + QString::number(j);
                auto vecRegs = combinedRegisterSubPageRecording("RGBIRCIP_LM", itemName, std::make_shared<int>(getCurrentCamera()), "CIP");
                if (RGBIR_RGBIR_ENABLE_MAP.contains({moduleName.c_str(), itemName})) 
                {
                    auto vecIrRegs = getCombinedRegister("RGBIRCIP_LM_IR_AB", itemName + "_IR_AB", std::make_shared<int>(getCurrentCamera()));
                    auto vecRgbRegs = getCombinedRegister("RGBIRCIP_LM_RGB_AB", itemName + "_RGB_AB", std::make_shared<int>(getCurrentCamera()));
                    setRegisterTypeMap(vecRegs, vecRgbRegs, vecIrRegs);
                }
            }
        }
    }
}

void QRGBIRTab::initCIP()
{
    if (!tabMap.value("CIP").second) {
        auto                  iter    = tabMap.find("CIP");
        auto                  widget  = iter.value().first;
        QPointer<QHBoxLayout> glayout = new QHBoxLayout;
        glayout->setContentsMargins(0, 5, 0, 0);
        widget->setLayout(glayout);
        QStringList boxstr;
        boxstr << "WBGainEnable"
               << "DeColorEnable"
               << "GreyPattern"
               << "NirWbEnable";
        QStringList editstr;
        editstr << "RAWPattern"
                << "FrameRate"
                << "HpBoost"
                << "LumaBoost"
                << "RemainThr"
                << "RemainRat"
                << "DelRCT[0]"
                << "DelRCT[1]"
                << "DelRCT[2]";
        QStringList tablestr;
        tablestr << "*DelRMrx[0]"
                 << "*DelRMrx[1]"
                 << "*DelRMrx[2]";
        QPointer<QVBoxLayout> vlayout = new QVBoxLayout;
        vlayout->setContentsMargins(0, 0, 0, 0);
        glayout->addLayout(vlayout);
        for (int i = 0; i < boxstr.size(); i++) {
            QPointer<QLabelCheckBox> box = new QLabelCheckBox;
            box->setMaximumWidth(500);
            box->setLabel(boxstr.at(i));
            box->setMouseTracking(true);
            vlayout->addWidget(box);
            CIP_box.push_back(box);
        }
        for (int i = 0; i < 5; i++) {
            QPointer<QLabelEditGroup> edit = new QLabelEditGroup;
            edit->setMaximumWidth(450);
            edit->setEditable(false);
            edit->setLabelValue(editstr.at(i));
            edit->setMouseTracking(true);
            vlayout->addWidget(edit);
            CIP_edit.push_back(edit);
        }
        QPointer<QGridLayout> glayout2 = new QGridLayout;
        // glayout2->setVerticalSpacing(40);
        QSplitter *line = new QSplitter(Qt::Vertical, this);
        line->setFrameShape(QFrame::WinPanel);
        line->setFixedWidth(1);
        line->setLineWidth(1);
        glayout->addWidget(line);
        glayout->addLayout(glayout2);
        for (int i = 5; i <= 8 ; i++) {
            QPointer<QLabelEditGroup> edit = new QLabelEditGroup;
            edit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
            edit->setEditable(false);
            edit->setLabelValue(editstr.at(i));
            edit->setMouseTracking(true);
            CIP_edit.push_back(edit);
            glayout2->addWidget(edit, 0, i - 5);
        }
        QPointer<QLabelEditGroup> CTCurrent = new QLabelEditGroup;
        CTCurrent->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        CTCurrent->setEditable(false);
        CTCurrent->setEnabled(false);
        CTCurrent->setLabelValue("CT Current");
        CTCurrent->setMouseTracking(true);
        glayout2->addWidget(CTCurrent,1, 0);
        connect(this, &QTabWndComm::readSignal, this, [=]() {
            uint32_t gainB      = QUiDataMiddleware("RGBIR", "Current Gain B").readValueFromDevice();
            uint32_t gainR      = QUiDataMiddleware("RGBIR", "Current Gain R").readValueFromDevice();
            UINT _CTCurrent =0;
            if(gainB!=0&&gainR!=0)
                _CTCurrent = (UINT)(((float)(gainB << 10) / (float)gainR)) ;
            CTCurrent->setValue(_CTCurrent);
        });

        QSpacerItem *horizontalSpacer = new QSpacerItem(20, 80, QSizePolicy::Preferred, QSizePolicy::Preferred);
        glayout2->addItem(horizontalSpacer, 1, 0);
        QPointer<QHBoxLayout> hlayout2 = new QHBoxLayout;
        glayout2->addLayout(hlayout2, 2, 0, 1, 4);
        for (int i = 0; i < tableMap.size(); i++) {
            QString                   tableName      = tableMap.value(i).first;
            QPointer<QTableListGroup> tableListGroup = tableMap.value(i).second;
            tableListGroup->setTableMenuRequested(false);
            tableListGroup->setSortingEnabled(false);
            tableListGroup->setAlternatingRowColors(false);
            tableListGroup->setStyle(
                "QTableView{gridline-color:white;color:rgb(0,0,0);outline:0px;background-color:rgb(230,230,230);}"
                "QHeaderView::section{border:0px solid "
                "rgb(0,0,0);background-color:rgb(165,165,165);color:rgb(255,255,255);font:bold};");
            tableListGroup->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            tableListGroup->setEditTriggers(QAbstractItemView::NoEditTriggers);
            QPointer<QVBoxLayout> vlayout = new QVBoxLayout;
            QPointer<QLabel>      table   = new QLabel;
            table->setAlignment(Qt::AlignCenter);
            table->setText(tablestr.at(i));
            vlayout->addWidget(table);
            vlayout->addWidget(tableListGroup);
            hlayout2->addLayout(vlayout);

            tableListGroup->setMinimumHeight(230);
            tableListGroup->setMinimumWidth(280);
            tableListGroup->setMaximumWidth(450);
            tableListGroup->setMaximumHeight(600);
            tableListGroup->setHeader(QStringList() << "R"
                                                    << "G"
                                                    << "B"
                                                    << "IR");
            for (int j = 0; j < 4; j++)
                tableListGroup->horizontalHeader()->setSectionResizeMode(j, QHeaderView::ResizeMode::Stretch);
            for (int k = 0; k < 3; k++) {
                for (int j = 0; j < 4; j++) {
                    tableListGroup->appendRow(k, j, "", Qt::AlignCenter);
                    tableListGroup->getItem(k, j)->setData(tableName, Qt::UserRole + 1);
                    tableListGroup->setRowHeight(k, 50);
                    tableListGroup->setColumnWidth(j, 50);
                }
            }
            tableListGroup->setVerticalHeader(QStringList() << "R"
                                                            << "G"
                                                            << "B");
            tableListGroup->setVerticalHeaderVisible(true);
            for (int j = 0; j < 3; j++)
                tableListGroup->verticalHeader()->setSectionResizeMode(j, QHeaderView::ResizeMode::Stretch);
            tableListGroup->horizontalHeader()->setMinimumHeight(40);
            tableListGroup->verticalHeader()->setDefaultAlignment(Qt::AlignCenter);
            tableListGroup->verticalHeader()->setMinimumWidth(40);
        }
        iter.value().second = true;
        for (QPointer<QLabelEditGroup> item : CIP_edit)
            connect(item, &QLabelEditGroup::releaseLabel, this, [=]() { recvEdit(item->Label()); });
        for (QPointer<QLabelCheckBox> item : CIP_box) {
            connect(item, &QLabelCheckBox::release, this, [=]() { recvEdit(item->Label()); });
            connect(item, &QLabelCheckBox::checkSignal, this,
                [=]() { recvCheckBox(item->Label(), item->checkState()); });
        }
        for (auto iter = tableMap.begin(); iter != tableMap.end(); iter++) {
            QPointer<QTableListGroup> table = iter.value().second;
            connect(table.get(), SIGNAL(sendCurrentItem(QStandardItem *, int, int)), this,
                SLOT(recvValueChanged(QStandardItem *, int, int)));
        }
    }
    updateUIValue<std::vector<QPointer<QLabelEditGroup>>>(CIP_edit);
    updateUIValue<std::vector<QPointer<QLabelCheckBox>>>(CIP_box);
    updateTable();
}

void QRGBIRTab::initDPC()
{
    if (!tabMap.value("DPC").second) {
        auto                  iter    = tabMap.find("DPC");
        QPointer<QHBoxLayout> hlayout = new QHBoxLayout;
        hlayout->setContentsMargins(0, 5, 0, 0);
        auto widget = iter.value().first;
        widget->setLayout(hlayout);
        QStringList boxstr;
        boxstr << "DPCEnable";
        QStringList editstr;
        editstr << "GainNode 0"
                << "GainNode 1"
                << "GainNode 2"
                << "GainNode 3"
                << "GainNode 4"
                << "GainNode 5"
                << "GainNode 6"
                << "GainNode 7";
        QPointer<QVBoxLayout> vlayout = new QVBoxLayout;
        vlayout->setContentsMargins(0, 0, 0, 0);
        hlayout->addLayout(vlayout);
        for (int i = 0; i < boxstr.size(); i++) {
            QPointer<QLabelCheckBox> box = new QLabelCheckBox;
            box->setMaximumWidth(300);
            box->setLabel(boxstr.at(i));
            box->setMouseTracking(true);
            vlayout->addWidget(box);
            DPC_box.push_back(box);
        }
        for (int i = 0; i < editstr.size(); i++) {
            QPointer<QLabelEditGroup> edit = new QLabelEditGroup;
            edit->setMaximumWidth(300);
            edit->setEditable(false);
            edit->setLabelValue(editstr.at(i));
            edit->setMouseTracking(true);
            vlayout->addWidget(edit);
            DPC_edit.push_back(edit);
        }
        QSplitter *line = new QSplitter(Qt::Vertical, this);
        line->setFrameShape(QFrame::WinPanel);
        line->setFixedWidth(1);
        line->setLineWidth(1);
        hlayout->addWidget(line);
        QPointer<QGridLayout> glayout = new QGridLayout;
        hlayout->addLayout(glayout);
        comboBox = new QComboBox;
        comboBox->addItems(QStringList() << "G-Blk"
                                         << "G-Wht"
                                         << "N-Blk"
                                         << "N-Wht");
        connect(comboBox, &QComboBox::currentIndexChanged, this, &QRGBIRTab::updateDPCEdit);
        glayout->addWidget(comboBox, 0, 0);
        for (int j = 0; j < 3; j++) {
            for (int i = 0; i < 8; i++) {
                QPointer<QLabelEditGroup> edit = new QLabelEditGroup;
                edit->setEditable(false);
                edit->setMouseTracking(true);
                edit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
                if (j == 0) {
                    edit->setLabelValue(comboBox->currentText() + "Coe1" + QString::number(i));
                    DPC_Coe1edit.push_back(edit);
                } else if (j == 1) {
                    edit->setLabelValue(comboBox->currentText() + "Coe2" + QString::number(i));
                    DPC_Coe2edit.push_back(edit);
                } else if (j == 2) {
                    edit->setLabelValue(comboBox->currentText() + "BpInt" + QString::number(i));
                    DPC_Bpintedit.push_back(edit);
                }
                edit->setLabelVisible(false);
                glayout->addWidget(edit, (i + 1), (j * 2 + 1));
            }
        }
        QStringList strlist;
        strlist << "BpCoe1"
                << "BpCoe2"
                << "BpInt";
        for (int i = 0; i < strlist.size(); i++) {
            QPointer<QLabel> label = new QLabel;
            label->setText(strlist.at(i));
            glayout->addWidget(label, 1, (i * 2));
        }
        iter.value().second = true;
        for (QPointer<QLabelEditGroup> item : DPC_Coe1edit)
            connect(item, &QLabelEditGroup::releaseLabel, this, [=]() { recvEdit(item->Label()); });
        for (QPointer<QLabelEditGroup> item : DPC_Coe2edit)
            connect(item, &QLabelEditGroup::releaseLabel, this, [=]() { recvEdit(item->Label()); });
        for (QPointer<QLabelEditGroup> item : DPC_Bpintedit)
            connect(item, &QLabelEditGroup::releaseLabel, this, [=]() { recvEdit(item->Label()); });
        for (QPointer<QLabelEditGroup> item : DPC_edit)
            connect(item, &QLabelEditGroup::releaseLabel, this, [=]() { recvEdit(item->Label()); });
        for (QPointer<QLabelCheckBox> item : DPC_box) {
            connect(item, &QLabelCheckBox::release, this, [=]() { recvEdit(item->Label()); });
            connect(item, &QLabelCheckBox::checkSignal, this,
                [=]() { recvCheckBox(item->Label(), item->checkState()); });
        }
    }
    updateUIValue<std::vector<QPointer<QLabelEditGroup>>>(DPC_edit);
    updateUIValue<std::vector<QPointer<QLabelEditGroup>>>(DPC_Coe1edit);
    updateUIValue<std::vector<QPointer<QLabelEditGroup>>>(DPC_Coe2edit);
    updateUIValue<std::vector<QPointer<QLabelEditGroup>>>(DPC_Bpintedit);
    updateUIValue<std::vector<QPointer<QLabelCheckBox>>>(DPC_box);
}

void QRGBIRTab::updateTable()
{
    int valTemp = 0;
    QUsbCommunication::getInstance().readBurst(IR_RGB_MODE_REG_ADDR, (unsigned char*)(&valTemp), ADDRESS_LENGTH, VALUE_LENGTH);
    for (int i = 0; i < tableMap.size(); i++) {
        QString                   tableName      = tableMap.value(i).first;
        QPointer<QTableListGroup> tableListGroup = tableMap.value(i).second;
        for (int k = 0; k < 3; k++) {
            for (int j = 0; j < 4; j++) {
                tableListGroup->getItem(k, j)->setData(tableName, Qt::UserRole + 1);
                QString  itemName = tableName + QString::number(k) + QString::number(j);
                uint32_t value    = 0;
                auto     reg      = combinedRegisterMap.value(itemName);
                if (RGBIR_RGBIR_ENABLE_MAP.contains({moduleName.c_str(), itemName})) 
                {
                    if (valTemp == 0x01) 
                    {
                        itemName += (strRgbIrMode == "RGB" ? "_RGB_AB" : "_IR_AB");
                    } 
                    else 
                    {
                        
                    }

                    reg = combinedRegisterMap.value(itemName);
                }
                // qDebug() << "reg length:" << reg.size();
                for (int i = 0; i < reg.size(); i++) {
                    readCombinedRegisterValue(reg.at(i), value);
                    float          fval = 0;
                    // 11bit 浮点数 1bit符号位 1bit整数位 9bit小数位
                    if (value & 0x400) {
                        fval = (float)((int16_t)(value | 0xf800)) / 512;
                    } else {
                        fval = (float)value / 512;
                    }
                    if (fval > 0) {
                        fval = fval + 0.0005;
                    }
                    if (fval < 0) {
                        fval = fval - 0.0005;
                    }

                    QStandardItem *item = tableListGroup->getItem(k, j);
                    if (item)
                        item->setText(QString::number(fval, 'f', 2));
                }
            }
        }
    }
}

void QRGBIRTab::changeTab(int index)
{
    if (tabMap.contains(tabComm->tabText(index))) {
        switch (index) {
        case 0:
            initCIP();
            break;
        case 1:
            initDPC();
            break;
        default:
            break;
        }
    }
}

void QRGBIRTab::recvValueChanged(QStandardItem *item, int row, int col)
{
    labelSpinSliderCheckBoxGroup->setFloatIntType(FloatIntType::Type_1_0x200);
    labelSpinSliderCheckBoxGroup->setSpinSliderEnable(false);
    QString tableName = item->data(Qt::UserRole + 1).toString();
    QString itemName  = tableName + QString::number(row) + QString::number(col);
    if (RGBIR_RGBIR_ENABLE_MAP.contains({moduleName.c_str(), itemName})) 
    {
        int valTemp = 0;
        QUsbCommunication::getInstance().readBurst(IR_RGB_MODE_REG_ADDR, (unsigned char*)(&valTemp), ADDRESS_LENGTH, VALUE_LENGTH);
        std::string strSuffix = strRgbIrMode == "RGB" ? "_RGB_AB" : "_IR_AB";

        if (valTemp == 0x01) 
        {
            itemName += strSuffix.c_str();
            auto re = combinedRegisterMap.value(itemName);
            updateRegisterTable(re);
            // for(int i = 0; i < re.size(); i++)
            // {
            //     uint32_t value=0; 
            //     readCombinedRegisterValue(re.at(i), value);
            //     item->setText(QString("0x%1").arg(QString::number(value,16).toUpper().rightJustified(VALUE_HEX_INFO,'0')));
            // }
        } 
        else 
        {
            auto re = combinedRegisterMap.value(itemName);
            updateRegisterTable(re);
        }
    } 
    else 
    {
        updateRegisterTable(combinedRegisterMap.value(itemName));
    }
    // updateRegisterTable(combinedRegisterMap.value(itemName));
}

void QRGBIRTab::updateDPCEdit(int index)
{
    QString comboBoxName = comboBox->currentText();
    for (int i = 0; i < 8; i++) {
        DPC_Coe1edit.at(i)->setLabelValue(comboBoxName + "Coe1" + QString::number(i));
        DPC_Coe2edit.at(i)->setLabelValue(comboBoxName + "Coe2" + QString::number(i));
        DPC_Bpintedit.at(i)->setLabelValue(comboBoxName + "BpInt" + QString::number(i));
    }
    for (auto item : DPC_Coe1edit)
        recordCombineRegbyUIifo("RGBIRCIP_LM", item->Label(), std::make_shared<int>(getCurrentCamera()));
    for (auto item : DPC_Coe2edit)
        recordCombineRegbyUIifo("RGBIRCIP_LM", item->Label(), std::make_shared<int>(getCurrentCamera()));
    for (auto item : DPC_Bpintedit)
        recordCombineRegbyUIifo("RGBIRCIP_LM", item->Label(), std::make_shared<int>(getCurrentCamera()));
    updateUIValue<std::vector<QPointer<QLabelEditGroup>>>(DPC_Coe1edit);
    updateUIValue<std::vector<QPointer<QLabelEditGroup>>>(DPC_Coe2edit);
    updateUIValue<std::vector<QPointer<QLabelEditGroup>>>(DPC_Bpintedit);
    // saveCombinedReg();
}
