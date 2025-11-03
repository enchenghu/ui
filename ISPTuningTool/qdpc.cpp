#include "qdpc.h"
#include <QComboBox>
#include <QGroupBox>
#include <QSplitter>

const QString     TAB_NAME                                             = "DPC";
const QString     TAB_NAME_LM                                          = "DPC-L&M";
const QString     TAB_NAME_S                                           = "DPC-S";
const QString     TAB_NAME_LFS                                         = "DPC-LFS";
const QStringList TABLE_COL_HEADER_LM                                  = { "W_Thre", "W_Ratio", "B_Thre", "B_Ratio" };
const QStringList TABLE_COL_HEADER_S_LFS                               = { "W_Thre", "W_Slope", "B_Thre", "B_Slope" };
const QMap<QString, QStringList> TABLE_COL_HEADER                      = { { TAB_NAME_LM, TABLE_COL_HEADER_LM },
                         { TAB_NAME_S, TABLE_COL_HEADER_S_LFS }, { TAB_NAME_LFS, TABLE_COL_HEADER_S_LFS } };
const QMap<QString, QList<QList<QPair<QString, QString>>>> CONTROL_MAP = {
    { "DPC-L&M",
        { { { ""/*"QLabelCheckBox"*/, ""/*"WPC_EnableL"*/}, {"" /*"QLabelCheckBox"*/,""/*"BPC_Enable"*/}, {"QLabelCheckBox", "BW_Enable" },
            { "QLabelEditGroup", "GainThre[0]" }, { "QLabelEditGroup", "GainThre[1]" }, { "QLabelEditGroup", "Cur Gain L"/*"GainThre[1]" */},
            { "QLabelEditGroup", "GainSwitch" }} } },
    { "DPC-S",
        { { { "QLabelCheckBox", "WPC_Enable" }, { "QLabelCheckBox", "BPC_Enable" }, { "QLabelCheckBox", "BW_Enable" },
            { "QLabelEditGroup", "GainPivot[0]" }, { "QLabelEditGroup", "GainPivot[1]" },
            { "QLabelEditGroup", "GainMargin" } } } },
    { "DPC-LFS",
        { { { "QLabelCheckBox", "WPC_Enable" }, { "QLabelCheckBox", "BPC_Enable" }, { "QLabelCheckBox", "BW_Enable" },
            { "QLabelEditGroup", "GainPivot[0]" }, { "QLabelEditGroup", "GainPivot[1]" },
            { "QLabelEditGroup", "GainMargin" } } } },
};

const int DPC_TABLE_ROW = 4;
const int DPC_TABLE_COL = 3;

enum DPC_TYPE {
    DPC_LM = 0,
    DPC_S,
    DPC_LFS,
    DPC_TYPE_MAX,
};

QDPC::QDPC(QWidget *parent)
    : QTabWndComm(parent)
    , tabWidgetLM(new QWidget(this))
    , tabWidgetS(new QWidget(this))
    , tabWidgetLFS(new QWidget(this))
    , tableList(new QTableListGroup())
{
    moduleName = TAB_NAME.toStdString();
    _WPCBox=new QLabelCheckBox();
    _BPCBox=new QLabelCheckBox();
    strDelRegListTxt = QCoreApplication::applicationDirPath() + "\\extracted_data\\DPC.txt";
    loadDeletedRegistersFromFile(strDelRegListTxt);
    //_BWBox=new QLabelCheckBox();

    init();
    initConnect();
}

QDPC::~QDPC() { }

void QDPC::init()
{
    addRegisterTable();
    subPageMap.insert(0, "DPC");
    contentWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QPointer<QHBoxLayout> layoutH = new QHBoxLayout(this);
    //QPointer<QGroupBox>   group   = new QGroupBox(contentWidget);
    //layoutH->addWidget(group.get(), Qt::AlignLeft);
    //layoutH->addStretch(2);
    contentLayout->addLayout(layoutH.get(), 0, 0, 1, 1);
    //group->setTitle("Exp_Type");
    //group->setLayout(new QVBoxLayout(group));
    comboCurrentType = new QComboBox();
    comboCurrentType->insertItem(DPC_LM, "L&M");
    comboCurrentType->insertItem(DPC_S, "S");
    comboCurrentType->insertItem(DPC_LFS, "LFS");
    comboCurrentType->setCurrentIndex(DPC_LM);
    //group->layout()->addWidget(comboCurrentType.get());
    layoutH->addWidget(new QLabel("  Exp_Type"), Qt::AlignLeft);
    layoutH->addWidget(comboCurrentType, Qt::AlignLeft);
    layoutH->setSpacing(22);

    QPointer<QVBoxLayout> layoutV = new QVBoxLayout(this);
    contentLayout->addLayout(layoutV.get(), 1, 0, 6, 1);
    layoutV->addWidget(tabWidgetLM.get());
    layoutV->addWidget(tabWidgetS.get());
    layoutV->addWidget(tabWidgetLFS.get());
    generateSubTab(tabWidgetLM, TAB_NAME_LM);
    generateSubTab(tabWidgetS, TAB_NAME_S);
    generateSubTab(tabWidgetLFS, TAB_NAME_LFS);
    tabWidgetLM->setVisible(true);
    tabWidgetS->setVisible(false);
    tabWidgetLFS->setVisible(false);

    QPointer<QSplitter> line = new QSplitter(Qt::Vertical, this);
    line->setFrameShape(QFrame::WinPanel);
    line->setFixedWidth(1);
    line->setLineWidth(1);
    contentLayout->addWidget(line, 0, 1, 0, 1);
    contentLayout->addWidget(tableList.get(), 1, 2, 5, 1);
    contentLayout->setColumnStretch(0, 1);
    contentLayout->setColumnStretch(1, 0);
    contentLayout->setColumnStretch(2, 2);
    contentLayout->setHorizontalSpacing(40);
    contentLayout->setContentsMargins(30, 15, 50, 15);

    initTable();
    getAllRegInfoInit();
}

void QDPC::initConnect()
{
    connect(comboCurrentType.get(), SIGNAL(currentIndexChanged(int)), this, SLOT(comboCurrentTypeValueChanged(int)));
    connect(tableList.get(), SIGNAL(sendCurrentItem(QStandardItem *, int, int)), this,
        SLOT(recvItemDoubleClicked(QStandardItem *, int, int)));
    connect(this, &QTabWndComm::updateValue, this, &QTabWndComm::read, Qt::QueuedConnection);
    connect(this, &QTabWndComm::readSignal, this, &QDPC::readTableUpate, Qt::QueuedConnection);

    connect(
        this, &QTabWndComm::reConnectRead, this,
        [=]() {
            getAllRegInfoInit();
            read();
        },
        Qt::QueuedConnection);
}

void QDPC::comboCurrentTypeValueChanged(int index)
{
    read();
    switch (index) {
    case DPC_LM: {
        tabWidgetLM->setVisible(true);
        tabWidgetS->setVisible(false);
        tabWidgetLFS->setVisible(false);
    } break;
    case DPC_S: {
        tabWidgetLM->setVisible(false);
        tabWidgetS->setVisible(true);
        tabWidgetLFS->setVisible(false);
    } break;
    case DPC_LFS: {
        tabWidgetLM->setVisible(false);
        tabWidgetS->setVisible(false);
        tabWidgetLFS->setVisible(true);
    } break;
    }
}

void QDPC::insertControl(QPointer<QGridLayout> layout, QPointer<QCommonUiInterface> widget, int row, int column,
    int rowSpan, int columnSpan)
{
    connect(widget, &QCommonUiInterface::releaseUiDataMiddleware, this,
        &QTabWndComm::recvUiDataMiddlewareUpdateRegisterTable, Qt::QueuedConnection);
    connect(this, &QTabWndComm::readSignal, widget, &QCommonUiInterface::read, Qt::QueuedConnection);
    layout->addWidget(widget, row, column, rowSpan, columnSpan);
    widget->setMouseTracking(true);
    widget->setValue(0);
}

void QDPC::generateSubTab(QPointer<QWidget> subTab, QString tabName)
{
    QPointer<QGridLayout> layout = new QGridLayout(subTab);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setAlignment(Qt::AlignLeft);
    subTab->setLayout(layout);
    if(TAB_NAME_LM==tabName)
    {
        _WPCBox->setLabel("WPC_Enable");
        _BPCBox->setLabel("BPC_Enable");
        //_BWBox->setLabel("BW_Enable");
        layout->addWidget(_WPCBox,0,0);
        layout->addWidget(_BPCBox,1,0);
        //layout->addWidget(_BWBox,2,0);
        _WPCBox->setMouseTracking(true);
        _BPCBox->setMouseTracking(true);
       // _BWBox->setMouseTracking(true);

        connect(_WPCBox,&QLabelCheckBox::release,this,[=](){
            QList<CombinedRegister> list= QUiDataMiddleware("DPC-L&M", "WPC_EnableL").getCombinedRegisterList();
            QList<CombinedRegister> list2= QUiDataMiddleware("DPC-L&M", "WPC_EnableM").getCombinedRegisterList();
            list+=list2;
           QTabWndComm::updateTableList(list);
        });
        connect(_BPCBox,&QLabelCheckBox::release,this,[=](){
            QList<CombinedRegister> list= QUiDataMiddleware("DPC-L&M", "BPC_EnableL").getCombinedRegisterList();
            QList<CombinedRegister> list2= QUiDataMiddleware("DPC-L&M", "BPC_EnableM").getCombinedRegisterList();
            list+=list2;
            QTabWndComm::updateTableList(list);
        });
        // connect(_BWBox,&QLabelCheckBox::release,this,[=](){
        //     QList<CombinedRegister> list= QUiDataMiddleware("DPC-L&M", "BW_Enable").getCombinedRegisterList();
        //     QTabWndComm::updateTableList(list);
        // });

        connect(_WPCBox,&QLabelCheckBox::checkSignal,this,[=](){
            uint32_t WPCL      = QUiDataMiddleware("DPC-L&M", "WPC_EnableL").readValueFromDevice();
            uint32_t WPCM      = QUiDataMiddleware("DPC-L&M", "WPC_EnableM").readValueFromDevice();

            if(_WPCBox->checkState())
            {
                QUiDataMiddleware("DPC-L&M", "WPC_EnableL").writeValueToDevice((WPCL|0x1));
                QUiDataMiddleware("DPC-L&M", "WPC_EnableM").writeValueToDevice((WPCM|0x1));
            }else{
                QUiDataMiddleware("DPC-L&M", "WPC_EnableL").writeValueToDevice((WPCL&(~0x1)));
                QUiDataMiddleware("DPC-L&M", "WPC_EnableM").writeValueToDevice((WPCM&(~0x1)));
            }
        });

        connect(_BPCBox,&QLabelCheckBox::checkSignal,this,[=](){
            uint32_t BPCL      = QUiDataMiddleware("DPC-L&M", "BPC_EnableL").readValueFromDevice();
            uint32_t BPCM      = QUiDataMiddleware("DPC-L&M", "BPC_EnableM").readValueFromDevice();
            if(_BPCBox->checkState())
            {
                QUiDataMiddleware("DPC-L&M", "BPC_EnableL").writeValueToDevice((BPCL|0x1));
                QUiDataMiddleware("DPC-L&M", "BPC_EnableM").writeValueToDevice((BPCM|0x1));
            }else{
                QUiDataMiddleware("DPC-L&M", "BPC_EnableL").writeValueToDevice((BPCL&(~0x1)));
                QUiDataMiddleware("DPC-L&M", "BPC_EnableM").writeValueToDevice((BPCM&(~0x1)));
            }
        });

        // connect(_BWBox,&QLabelCheckBox::checkSignal,this,[=](){
        //     uint32_t BWVal      = QUiDataMiddleware("DPC-L&M", "BW_Enable").readValueFromDevice();
        //     if(_BWBox->checkState())
        //     {
        //         QUiDataMiddleware("DPC-L&M", "BW_Enable").writeValueToDevice((BWVal|0x1));
        //     }else{
        //         QUiDataMiddleware("DPC-L&M", "BW_Enable").writeValueToDevice((BWVal&(~0x1)));
        //     }
        // });
    }
    for (int i = 0; i < CONTROL_MAP[tabName].size(); i++) {
        for (int j = 0; j < CONTROL_MAP[tabName][i].size(); j++) {
            if(CONTROL_MAP[tabName][i][j].second=="GainThre[0]") {
                GainThr0 = QCommonUiInterfaceFactory::instance().createUiInterface(
                    CONTROL_MAP[tabName][i][j].first, tabName, CONTROL_MAP[tabName][i][j].second, this);
                if (dynamic_cast<QLabelEditGroup *>(GainThr0.data())) {
                    dynamic_cast<QLabelEditGroup *>(GainThr0.data())->setEditable(false);
                }
                if (GainThr0) {
                    insertControl(layout, GainThr0, j, i);
                }
            } else if(CONTROL_MAP[tabName][i][j].second=="GainThre[1]") {
                GainThr1 = QCommonUiInterfaceFactory::instance().createUiInterface(
                    CONTROL_MAP[tabName][i][j].first, tabName, CONTROL_MAP[tabName][i][j].second, this);
                if (dynamic_cast<QLabelEditGroup *>(GainThr1.data())) {
                    dynamic_cast<QLabelEditGroup *>(GainThr1.data())->setEditable(false);
                }
                if (GainThr1) {
                    insertControl(layout, GainThr1, j, i);
                }
            } else {
                QPointer<QCommonUiInterface> widget = QCommonUiInterfaceFactory::instance().createUiInterface(
                    CONTROL_MAP[tabName][i][j].first, tabName, CONTROL_MAP[tabName][i][j].second, this);
                if (dynamic_cast<QLabelEditGroup *>(widget.data())) {
                    dynamic_cast<QLabelEditGroup *>(widget.data())->setEditable(false);
                }
                if (widget) {
                    insertControl(layout, widget, j, i);
                }
            }


        }
        layout->setColumnStretch(i, 1);
    }
}

void QDPC::getAllRegInfoInit()
{
    setAddrModule.clear();
    registerMoudle.clear();
    for (int type = 0; type < DPC_TYPE_MAX; type++) {
        QString module  = QString::fromStdString(moduleName) + "-" + comboCurrentType->itemText(type);
        auto    regList = QUiDataMiddleware::getRegisterAddressByUiModule(module);
        auto    regVec  = std::vector<uint32_t>(regList.constBegin(), regList.constEnd());
        for (auto iter : regVec)
        {
            setAddrModule.insert(iter);
        }
        // setAddrModule.insert(setAddrModule.end(), regVec.begin(), regVec.end());
    }
    for (auto iter : setAddrModule)
    {
        registerMoudle.emplace_back(iter);
        registerSubPageMap[iter].emplace_back("DPC");
    }
}

void QDPC::initTable()
{
    tableList->setSortingEnabled(false);
    tableList->setMinimumHeight(200);
    tableList->setMinimumWidth(200);
    tableList->horizontalHeader()->setMinimumHeight(50);
    tableList->verticalHeader()->setMinimumWidth(100);
    tableList->setHorizontalHeaderVisible(true);
    tableList->setVerticalHeaderVisible(true);
    tableList->verticalHeader()->setDefaultAlignment(Qt::AlignCenter);
    tableList->setStyle(
        "QTableView{gridline-color:white;color:rgb(0,0,0);outline:0px;background-color:rgb(230,230,230);}"
        "QHeaderView::section{border:0px solid "
        "rgb(0,0,0);background-color:rgb(165,165,165);color:rgb(255,255,255);font:bold};");
    tableList->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    tableList->clearAll();
    tableList->setHeader(QStringList() << "0"
                                       << "1"
                                       << "2");
    tableList->setVerticalHeader(TABLE_COL_HEADER[TAB_NAME + "-" + comboCurrentType->currentText()]);
    int value = 0;
    for (int row = 0; row < DPC_TABLE_ROW; row++) {
        for (int col = 0; col < DPC_TABLE_COL; col++) {
            tableList->appendRow(row, col, QString("0x%1").arg(QString::number(value, 16)));
        }
    }
    // 自动拉伸
    for (int row = 0; row < DPC_TABLE_ROW; row++)
        tableList->verticalHeader()->setSectionResizeMode(row, QHeaderView::ResizeMode::Stretch);
    for (int col = 0; col < DPC_TABLE_COL; col++)
        tableList->horizontalHeader()->setSectionResizeMode(col, QHeaderView::ResizeMode::Stretch);
}

void QDPC::readTableUpate()
{
    readTable();
    updateTable();
    updateUIControl();
}

void QDPC::readTable()
{
    QString                     currModuleName   = TAB_NAME + "-" + comboCurrentType->currentText();
    QPointer<QUiDataMiddleware> data             = new QUiDataMiddleware(currModuleName, "DpcTableInfo");
    auto                        combineListValue = data->getCombinedRegisterValueList();
    tableListInfo =
        std::vector<std::pair<CombinedRegister, uint32_t>>(combineListValue.constBegin(), combineListValue.constEnd());
}

void QDPC::updateTable()
{
    if (tableListInfo.size() == 0)
        return;
    tableList->clearAll();
    tableList->setHeader(QStringList() << "0"
                                       << "1"
                                       << "2");
    tableList->setVerticalHeader(TABLE_COL_HEADER[TAB_NAME + "-" + comboCurrentType->currentText()]);
    int index = 0;
    int value = 0;
    for (int row = 0; row < DPC_TABLE_ROW; row++) {
        for (int col = 0; col < DPC_TABLE_COL; col++) {
            value = tableListInfo[index].second;
            tableList->appendRow(row, col, QString("0x%1").arg(QString::number(value, 16)));
            index++;
        }
    }
    // 自动拉伸
    for (int row = 0; row < DPC_TABLE_ROW; row++)
        tableList->verticalHeader()->setSectionResizeMode(row, QHeaderView::ResizeMode::Stretch);
    for (int col = 0; col < DPC_TABLE_COL; col++)
        tableList->horizontalHeader()->setSectionResizeMode(col, QHeaderView::ResizeMode::Stretch);
}

void QDPC::recvItemDoubleClicked(QStandardItem *item, int row, int col)
{
    if (tableListInfo.size() == 0)
        return;
    int                           index = row * DPC_TABLE_COL + col;
    std::vector<CombinedRegister> combineReg;
    combineReg.push_back(tableListInfo[index].first);
    clearCurrentRigester();
    updateRegisterTable(combineReg);
}


void QDPC::recordCombinedReg(QString moduleName, QString UIName,std::vector<CombinedRegister>& combinevector)
{
    std::vector<UiInfo> uiInfo;
    uiInfo = DatabaseOrm::getUiInfoByModuleNameAndControlName("CDPC_V65_HW_RAWL"/*moduleName.toStdString()*/,"WPC_EnableL"/*UIName.toStdString()*/);
    for (auto i : uiInfo) {
        CombinedRegister combineReg
            = DatabaseOrm::getCombinedRegisterByModuleNameAndCameraIdAndCombinedName(
                moduleName.toStdString(), std::make_shared<int>(getCurrentCamera()), *(i.combinedName));
        combinevector.push_back(combineReg);
    }
}

void QDPC::updateUIControl()
{
    if(GainThr0)
    {
        uint32_t GainThr0Val = QUiDataMiddleware("DPC-L&M", "GainThre[0]").readValueFromDevice();
        dynamic_cast<QLabelEditGroup *>(GainThr0.data())->setValue(QString("0x%4").arg(QString::number((GainThr0Val<<4),16).toUpper().rightJustified(VALUE_HEX_INFO,'0')));
    }

    if(GainThr1)
    {
        uint32_t GainThr1Val = QUiDataMiddleware("DPC-L&M", "GainThre[0]").readValueFromDevice();
        dynamic_cast<QLabelEditGroup *>(GainThr1.data())->setValue(QString("0x%4").arg(QString::number((GainThr1Val<<4),16).toUpper().rightJustified(VALUE_HEX_INFO,'0')));
    }

    uint32_t WPCL      = QUiDataMiddleware("DPC-L&M", "WPC_EnableL").readValueFromDevice();
    uint32_t WPCM      = QUiDataMiddleware("DPC-L&M", "WPC_EnableM").readValueFromDevice();
    if(((WPCL&0x1)&(WPCM&0x1))!=0)
        _WPCBox->setChecked(true);
    else
        _WPCBox->setChecked(false);

    uint32_t BPCL      = QUiDataMiddleware("DPC-L&M", "BPC_EnableL").readValueFromDevice();
    uint32_t BPCM      = QUiDataMiddleware("DPC-L&M", "BPC_EnableM").readValueFromDevice();
    if(((BPCL&0x1)&(BPCM&0x1))!=0)
        _BPCBox->setChecked(true);
    else
        _BPCBox->setChecked(false);

    // uint32_t BWVal      = QUiDataMiddleware("DPC-L&M", "BW_Enable").readValueFromDevice();
    // if((BWVal&0x1)!=0)
    //     _BWBox->setChecked(true);
    // else
    //     _BWBox->setChecked(false);
}

