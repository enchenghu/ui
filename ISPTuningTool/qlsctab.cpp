#include "qlsctab.h"
#include <QSplitter>
#include "qtransfermanager.h"

QLscTab::QLscTab(QWidget *parent)
{
    moduleName = "LSC";
    curvePointNameX << "SensorGain1";
    curvePointNameX << "SensorGain2";
    curvePointNameY << "MinQ";
    curvePointNameY << "MaxQ";

    strDelRegListTxt = QCoreApplication::applicationDirPath() + "\\extracted_data\\LSC.txt";
    loadDeletedRegistersFromFile(strDelRegListTxt);

    comboCurrentType     = new QComboBox();
    layoutLeft           = new QVBoxLayout(this);
    layoutMiddle         = new QVBoxLayout(this);
    layoutRight          = new QVBoxLayout(this);
    labelCheckLscEnable  = new QLabelCheckBox();
    labelCheckMonoEn     = new QLabelCheckBox();
    //labelEditGainSel     = new QLabelEditGroup();
    labelEditManCTOption = new QLabelEditGroup();
    labelEditHScale      = new QLabelEditGroup();
    labelEditVScale      = new QLabelEditGroup();
    labelEditCurCT       = new QLabelEditGroup();
    labelEditCTD         = new QLabelEditGroup();
    labelEditCTM1        = new QLabelEditGroup();
    labelEditCTM2        = new QLabelEditGroup();
    labelEditCTA         = new QLabelEditGroup();
    comboCpQCellType     = new QComboBox();
    groupQCellCurve      = new QGroupBox(this);
    groupCpTable         = new QGroupBox(this);
    layoutQCellCurve     = new QVBoxLayout(groupQCellCurve.get());
    layoutCpTable        = new QVBoxLayout(groupCpTable.get());
    layoutCpTableTitle   = new QHBoxLayout(this);
    labelComboCpIndex    = new QLabelComboBox();
    labelComboCpType     = new QLabelComboBox();
    tableCp              = new QTableListGroup();
    curveChartQCell      = new QCurveChartGroup(this);

    init();
    initConnect();
    // futureSaveInfoInit = QtConcurrent::run(&QLscTab::getAllRegInfoInit, this);
    getAllRegInfoInit();
}

QLscTab::~QLscTab() { }

void QLscTab::init()
{
    if(m_nLevel==LEVEL_ADMINISTRATOR)
    {
        comboCurrentType->insertItem(LSC_NORMAL, "Normal");
    }

    subPageMap.insert(0, "LSC");
    // subPageMap.insert(0, "Delete");

    comboCurrentType->insertItem(LSC_LFS, "LFS");
    comboCurrentType->setCurrentIndex(LSC_NORMAL);
    comboCurrentType->setFixedWidth(100);

    titleLayout->addWidget(comboCurrentType.get());
    int left, right, top, bottom;
    titleLayout->getContentsMargins(&left, &top, &right, &bottom);
    titleLayout->setContentsMargins(10, top, right, bottom);

    labelCheckLscEnable->setLabel("LSCEnable");
    labelCheckMonoEn->setLabel("Mono_en");
    layoutLeft->addWidget(labelCheckLscEnable.get());
    layoutLeft->addWidget(labelCheckMonoEn.get());
    if(m_nLevel==LEVEL_ADMINISTRATOR||comboCurrentType->currentText()=="Normal")
    {
        //labelEditGainSel->setLabelValue("GainSel");
        labelEditManCTOption->setLabelValue("ManCTOption");
        labelEditHScale->setLabelValue("HScale");
        labelEditVScale->setLabelValue("VScale");
        //layoutLeft->addWidget(labelEditGainSel.get());
        layoutLeft->addWidget(labelEditManCTOption.get());
        layoutLeft->addWidget(labelEditHScale.get());
        layoutLeft->addWidget(labelEditVScale.get());
    }

    labelEditCurCT->setLabelValue("CurCT");
    labelEditCTD->setLabelValue("CTD");
    labelEditCTM1->setLabelValue("CTM1");
    labelEditCTM2->setLabelValue("CTM2");
    labelEditCTA->setLabelValue("CTA");
    layoutMiddle->addWidget(labelEditCurCT.get());
    layoutMiddle->addWidget(labelEditCTD.get());
    layoutMiddle->addWidget(labelEditCTM1.get());
    layoutMiddle->addWidget(labelEditCTM2.get());
    layoutMiddle->addWidget(labelEditCTA.get());

    QStringList cpIndex;
    cpIndex << "LSC G";
    cpIndex << "LSC B";
    cpIndex << "LSC R";
    labelComboCpIndex->setValue("Index", cpIndex);
    labelComboCpIndex->setFixedWidth(150);
    QStringList cpType;
    cpType << "CPA";
    cpType << "CPM";
    cpType << "CPD";
    labelComboCpType->setValue("Type", cpType);
    labelComboCpType->setFixedWidth(150);
    QSpacerItem *horizontalSpacer = new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Expanding);

    layoutCpTableTitle->addWidget(labelComboCpIndex.get());
    layoutCpTableTitle->addWidget(labelComboCpType.get());
    layoutCpTableTitle->addItem(horizontalSpacer);

    layoutCpTable->addLayout(layoutCpTableTitle.get(), 0);
    layoutCpTable->addWidget(tableCp.get(), 1);
    groupCpTable->setLayout(layoutCpTable.get());

    curveChartQCell->setYStringInfo(curvePointNameY);
    curveChartQCell->setXStringInfo(curvePointNameX);
    curveChartQCell->setAxisRange(0, 255, 0, 127);
    curveChartQCell->setPointChangeEnable(false);
    curveChartQCell->setCurveFittingType(CurveFittingType::NONE);
    layoutQCellCurve->addWidget(curveChartQCell.get());
    groupQCellCurve->setLayout(layoutQCellCurve.get());

    comboCpQCellType->insertItem(LSC_CURVE_TBALE_CP, "CP");
    comboCpQCellType->insertItem(LSC_CURVE_TBALE_QCELL, "QCell");
    comboCpQCellType->setCurrentIndex(LSC_CURVE_TBALE_CP);
    comboCpQCellType->setFixedWidth(100);

    layoutRight->addWidget(comboCpQCellType.get());
    layoutRight->addWidget(groupCpTable.get());
    layoutRight->addWidget(groupQCellCurve.get());
    groupCpTable->setVisible(true);
    groupQCellCurve->setVisible(false);

    contentWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    contentWidget->setContentsMargins(10, 10, 10, 10);

    QSplitter *line = new QSplitter(Qt::Vertical, this);
    line->setFrameShape(QFrame::WinPanel);
    line->setFixedWidth(1);
    line->setLineWidth(1);
    contentLayout->addLayout(layoutLeft.get(), 0, 0);
    contentLayout->addWidget(line, 0, 1, 0, 1);
    contentLayout->addLayout(layoutMiddle.get(), 0, 2);
    contentLayout->addLayout(layoutRight.get(), 0, 3);
    contentLayout->setColumnStretch(0, 2);
    contentLayout->setColumnStretch(1, 0);
    contentLayout->setColumnStretch(2, 2);
    contentLayout->setColumnStretch(3, 5);
    addRegisterTable();

    setControlEnable(false);
    setControlMouseTracking(true);
    initCpTable();
    currModuleName = QString::fromStdString(moduleName) + "-" + comboCurrentType->currentText();
    setControlCombineRegister();
}

void QLscTab::initConnect()
{
    connect(comboCurrentType.get(), SIGNAL(currentIndexChanged(int)), this, SLOT(comboCurrentTypeValueChanged(int)));
    connect(comboCpQCellType.get(), SIGNAL(currentIndexChanged(int)), this, SLOT(comboCpQCellTypeValueChanged(int)));
    connect(labelComboCpIndex.get(), SIGNAL(signalValueChange(int)), this, SLOT(labelComboCpIndexValueChanged(int)));
    connect(labelComboCpType.get(), SIGNAL(signalValueChange(int)), this, SLOT(labelComboCpTypeValueChanged(int)));
    connect(tableCp.get(), SIGNAL(sendCurrentItem(QStandardItem *, int, int)), this,
        SLOT(recvItemDoubleClicked(QStandardItem *, int, int)));
    connect(curveChartQCell.get(), SIGNAL(signalDoubleClickedString(QString)), this, SLOT(curveDoubleClicked(QString)));
    connect(labelCheckLscEnable.get(), SIGNAL(releaseUpadteCombineReg(std::vector<CombinedRegister>)), this,
        SLOT(updateCombineRegisterTable(std::vector<CombinedRegister>)));
    connect(labelCheckLscEnable.get(), &QLabelCheckBox::checkSignal, this,
        [=](){
            TransferManager::getInstance().changeLscValue(labelCheckLscEnable->checkState());
        });
    connect(labelCheckMonoEn.get(), SIGNAL(releaseUpadteCombineReg(std::vector<CombinedRegister>)), this,
        SLOT(updateCombineRegisterTable(std::vector<CombinedRegister>)));
    // connect(labelEditGainSel.get(), SIGNAL(releaseUpadteCombineReg(std::vector<CombinedRegister>)), this,
    //     SLOT(updateCombineRegisterTable(std::vector<CombinedRegister>)));
    connect(labelEditManCTOption.get(), SIGNAL(releaseUpadteCombineReg(std::vector<CombinedRegister>)), this,
        SLOT(updateCombineRegisterTable(std::vector<CombinedRegister>)));
    connect(labelEditHScale.get(), SIGNAL(releaseUpadteCombineReg(std::vector<CombinedRegister>)), this,
        SLOT(updateCombineRegisterTable(std::vector<CombinedRegister>)));
    connect(labelEditVScale.get(), SIGNAL(releaseUpadteCombineReg(std::vector<CombinedRegister>)), this,
        SLOT(updateCombineRegisterTable(std::vector<CombinedRegister>)));
    // connect(labelEditCurCT.get(), SIGNAL(releaseUpadteCombineReg(std::vector<CombinedRegister>)), this,
    //     SLOT(updateCombineRegisterTable(std::vector<CombinedRegister>)));


    connect(this, &QTabWndComm::readSignal, labelEditCurCT, [&]() {
        uint32_t gainB      = QUiDataMiddleware("LSC-Normal", "Current Gain B").readValueFromDevice();
        uint32_t gainR      = QUiDataMiddleware("LSC-Normal", "Current Gain R").readValueFromDevice();
        UINT _CTCurrent =0;
        if(gainB!=0&&gainR!=0){
            double fCurCT = ((float)gainB) / ((float)gainR);
            _CTCurrent = (UINT)(fCurCT * 256);
        }
        labelEditCurCT->setValue(_CTCurrent);
    });

    connect(&TransferManager::getInstance(), &TransferManager::lscValueChanged, this, [=](bool &bNewValue)
    {
        labelCheckLscEnable->setChecked(bNewValue);
    });

    connect(labelEditCTD.get(), SIGNAL(releaseUpadteCombineReg(std::vector<CombinedRegister>)), this,
        SLOT(updateCombineRegisterTable(std::vector<CombinedRegister>)));
    connect(labelEditCTM1.get(), SIGNAL(releaseUpadteCombineReg(std::vector<CombinedRegister>)), this,
        SLOT(updateCombineRegisterTable(std::vector<CombinedRegister>)));
    connect(labelEditCTM2.get(), SIGNAL(releaseUpadteCombineReg(std::vector<CombinedRegister>)), this,
        SLOT(updateCombineRegisterTable(std::vector<CombinedRegister>)));
    connect(labelEditCTA.get(), SIGNAL(releaseUpadteCombineReg(std::vector<CombinedRegister>)), this,
        SLOT(updateCombineRegisterTable(std::vector<CombinedRegister>)));
    connect(this, &QTabWndComm::readSignal, this, &QLscTab::readPage);
    connect(this, &QTabWndComm::updateValue, this, &QLscTab::readPage);
    connect(this, &QTabWndComm::reConnectRead, this, &QLscTab::reConnectReadPage);
}

void QLscTab::comboCurrentTypeValueChanged(int index)
{
    currModuleName = QString::fromStdString(moduleName) + "-" + comboCurrentType->currentText();
    setControlCombineRegister();
    readPage();
}

void QLscTab::comboCpQCellTypeValueChanged(int index)
{
    if (index == LSC_CURVE_TBALE_CP) {
        groupCpTable->setVisible(true);
        groupQCellCurve->setVisible(false);
        readCpTableUpate();
    } else {
        groupCpTable->setVisible(false);
        groupQCellCurve->setVisible(true);
        readQCellCurveUpate();
    }
}

void QLscTab::labelComboCpIndexValueChanged(int index)
{
    readCpTableUpate();
}

void QLscTab::labelComboCpTypeValueChanged(int index)
{
    readCpTableUpate();
}

void QLscTab::curveDoubleClicked(QString name)
{
    QPointer<QUiDataMiddleware> data        = new QUiDataMiddleware(currModuleName, name);
    auto                        combineList = data->getCombinedRegisterList();

    std::vector<CombinedRegister> combineReg;
    for (auto combine : combineList)
    {
        combineReg.push_back(combine);
        // setRegisterSubPage(combine, "LSC");
    }
        
    clearCurrentRigester();
    updateRegisterTable(combineReg);
}

void QLscTab::recvItemDoubleClicked(QStandardItem *item, int row, int col)
{
    if (cpTableList.size() == 0)
        return;
    int                           index = col * LSC_CP_TABLE_ROW + row;
    std::vector<CombinedRegister> combineReg;
    combineReg.push_back(cpTableList[index].first);
    clearCurrentRigester();
    updateRegisterTable(combineReg);
}

void QLscTab::updateCombineRegisterTable(std::vector<CombinedRegister> combineList)
{
    if (combineList.size() > 0)
        updateRegisterTable(combineList);
}

void QLscTab::readPage()
{
    readControlInfoUpdate();
    if (comboCpQCellType->currentIndex() == LSC_CURVE_TBALE_CP)
        readCpTableUpate();
    else
        readQCellCurveUpate();
}

void QLscTab::reConnectReadPage()
{
    setControlCombineRegister();
    readPage();
    if (!futureSaveInfoInit.isFinished())
        futureSaveInfoInit.waitForFinished();
    futureSaveInfoInit = QtConcurrent::run(&QLscTab::getAllRegInfoInit, this);
}

void QLscTab::setControlEnable(bool enable)
{
    //labelEditGainSel->setEditable(enable);
    labelEditManCTOption->setEditable(enable);
    labelEditHScale->setEditable(enable);
    labelEditVScale->setEditable(enable);
    labelEditCurCT->setEditable(enable);
    labelEditCTD->setEditable(enable);
    labelEditCTM1->setEditable(enable);
    labelEditCTM2->setEditable(enable);
    labelEditCTA->setEditable(enable);
}

void QLscTab::setControlMouseTracking(bool enable)
{
    labelCheckLscEnable->setMouseTracking(enable);
    labelCheckMonoEn->setMouseTracking(enable);
   // labelEditGainSel->setMouseTracking(enable);
    labelEditManCTOption->setMouseTracking(enable);
    labelEditHScale->setMouseTracking(enable);
    labelEditVScale->setMouseTracking(enable);
    labelEditCurCT->setMouseTracking(enable);
    labelEditCTD->setMouseTracking(enable);
    labelEditCTM1->setMouseTracking(enable);
    labelEditCTM2->setMouseTracking(enable);
    labelEditCTA->setMouseTracking(enable);
}

void QLscTab::getAllRegInfoInit()
{
    registerMoudle.clear();
    for (int type = 0; type < LSC_TYPE_MAX; type++) {
        QString module  = QString::fromStdString(moduleName) + "-" + comboCurrentType->itemText(type);
        auto    regList = QUiDataMiddleware::getRegisterAddressByUiModule(module);
        for (auto reg : regList)
        {
            registerMoudle.push_back(reg);
            registerSubPageMap[reg].emplace_back("LSC");
        }
            
    }
}

void QLscTab::initCpTableColor()
{
    cpTableColor[LSC_COLOR_R].clear();
    cpTableColor[LSC_COLOR_G].clear();
    cpTableColor[LSC_COLOR_B].clear();
    for (int i = 0; i <= 255; i++) {
        std::tuple<int, int, int> tupleR(255, 255 - i, 255 - i);
        std::tuple<int, int, int> tupleB(255 - i, 255 - i, 255);
        cpTableColor[LSC_COLOR_R].push_back(tupleR);
        cpTableColor[LSC_COLOR_B].push_back(tupleB);
        if (i <= 75) {
            std::tuple<int, int, int> tupleG(0, 180 + i, 0);
            cpTableColor[LSC_COLOR_G].push_back(tupleG);
        } else {
            std::tuple<int, int, int> tupleG((int)((i - 75) * (255 / (float)180)), 255,
                (int)((i - 75) * (255 / (float)180)));
            cpTableColor[LSC_COLOR_G].push_back(tupleG);
        }
    }
    reverse(cpTableColor[LSC_COLOR_G].begin(), cpTableColor[LSC_COLOR_G].end());
}

void QLscTab::initCpTable()
{
    initCpTableColor();
    tableCp->setHorizontalHeaderVisible(false);
    tableCp->setVerticalHeaderVisible(false);
    tableCp->setMinimumHeight(300);

    tableCp->clearAll();
    int color = labelComboCpIndex->getCurrentIndex();
    int value = 0;
    for (int col = 0; col < LSC_CP_TABLE_COL; col++) {
        for (int row = 0; row < LSC_CP_TABLE_ROW; row++) {
            tableCp->appendRow(row, col, QString("0x%1").arg(QString::number(value, 16)));
            tableCp->getItem(row, col)->setBackground(QBrush(QColor(std::get<0>(cpTableColor[color][value]),
                std::get<1>(cpTableColor[color][value]), std::get<2>(cpTableColor[color][value]))));
        }
    }

    // 自动拉伸
    for (int col = 0; col < LSC_CP_TABLE_COL; col++)
        tableCp->horizontalHeader()->setSectionResizeMode(col, QHeaderView::ResizeMode::Stretch);
    for (int row = 0; row < LSC_CP_TABLE_ROW; row++)
        tableCp->verticalHeader()->setSectionResizeMode(row, QHeaderView::ResizeMode::Stretch);
}

void QLscTab::readCpTableUpate()
{
    readCpTable();
    updateCpTable();
}

void QLscTab::readCpTable()
{
    QString                     uiName = labelComboCpType->getValue(-1) + "-" + labelComboCpIndex->getValue(-1);
    QPointer<QUiDataMiddleware> data             = new QUiDataMiddleware(currModuleName, uiName);
    auto                        combineListValue = data->getCombinedRegisterValueList();
    cpTableList.clear();
    for (auto combine : combineListValue)
        cpTableList.push_back(combine);
}

void QLscTab::updateCpTable()
{
    if (cpTableList.size() == 0)
        return;
    tableCp->clearAll();
    int color = labelComboCpIndex->getCurrentIndex();
    int index = 0;
    int value = 0;
    for (int col = 0; col < LSC_CP_TABLE_COL; col++) {
        for (int row = 0; row < LSC_CP_TABLE_ROW; row++) {
            value = cpTableList[index].second;
            tableCp->appendRow(row, col, QString("0x%1").arg(QString::number(value, 16)));
            tableCp->getItem(row, col)->setBackground(QBrush(QColor(std::get<0>(cpTableColor[color][value]),
                std::get<1>(cpTableColor[color][value]), std::get<2>(cpTableColor[color][value]))));
            index++;
        }
    }

    // 自动拉伸
    for (int col = 0; col < LSC_CP_TABLE_COL; col++)
        tableCp->horizontalHeader()->setSectionResizeMode(col, QHeaderView::ResizeMode::Stretch);
    for (int row = 0; row < LSC_CP_TABLE_ROW; row++)
        tableCp->verticalHeader()->setSectionResizeMode(row, QHeaderView::ResizeMode::Stretch);
}

void QLscTab::readQCellCurveUpate()
{
    readQCellCurve();
    updateQCellCurve();
}

void QLscTab::readQCellCurve()
{
    curvePointListX.clear();
    curvePointListY.clear();
    for (auto name : curvePointNameX) {
        QPointer<QUiDataMiddleware> data             = new QUiDataMiddleware(currModuleName, name);
        auto                        combineListValue = data->getCombinedRegisterValueList();
        for (auto combine : combineListValue)
            curvePointListX.push_back(combine);
    }
    for (auto name : curvePointNameY) {
        QPointer<QUiDataMiddleware> data             = new QUiDataMiddleware(currModuleName, name);
        auto                        combineListValue = data->getCombinedRegisterValueList();
        for (auto combine : combineListValue)
            curvePointListY.push_back(combine);
    }
}

void QLscTab::updateQCellCurve()
{
    if ((curvePointListX.size() == 0) || (curvePointListY.size() == 0))
        return;

    std::vector<QPoint> pointList;
    QPoint              point;
    point.setX(0);
    point.setY(curvePointListY[1].second);
    pointList.push_back(point);

    point.setX(curvePointListX[0].second);
    point.setY(curvePointListY[1].second);
    pointList.push_back(point);

    point.setX(curvePointListX[1].second);
    point.setY(curvePointListY[0].second);
    pointList.push_back(point);

    point.setX(255);
    point.setY(curvePointListY[0].second);
    pointList.push_back(point);

    curveChartQCell->setPoints(pointList);
}

// void QLscTab::combineRegisterSet(QCommonUiInterface *widget) 
// { 
//     if (dynamic_cast<QLabelCheckBox*>(widget) != nullptr)
//     {
//         auto ptrCheckBox = dynamic_cast<QLabelCheckBox*>(widget);
//         ptrCheckBox->setCombineRegister(currModuleName);
//         for (auto iter : ptrCheckBox->getCombineRegister())
//             setRegisterSubPage(iter, "LSC");
//     }
//     else if (dynamic_cast<QLabelEditGroup*>(widget) != nullptr)
//     {
//         auto ptrCheckBox = dynamic_cast<QLabelEditGroup*>(widget);
//         ptrCheckBox->setCombineRegister(currModuleName);
//         for (auto iter : ptrCheckBox->getCombineRegister())
//             setRegisterSubPage(iter, "LSC");
//     }
// }

void QLscTab::setControlCombineRegister()
{
    labelCheckLscEnable->setCombineRegister(currModuleName);
    labelCheckMonoEn->setCombineRegister(currModuleName);
    //labelEditGainSel->setCombineRegister(currModuleName);
    labelEditManCTOption->setCombineRegister(currModuleName);
    labelEditHScale->setCombineRegister(currModuleName);
    labelEditVScale->setCombineRegister(currModuleName);
    //labelEditCurCT->setCombineRegister(currModuleName);
    labelEditCTD->setCombineRegister(currModuleName);
    labelEditCTM1->setCombineRegister(currModuleName);
    labelEditCTM2->setCombineRegister(currModuleName);
    labelEditCTA->setCombineRegister(currModuleName);
    // combineRegisterSet(labelCheckLscEnable.get());
    // combineRegisterSet(labelCheckMonoEn.get());
    // combineRegisterSet(labelEditManCTOption.get());
    // combineRegisterSet(labelEditHScale.get());
    // combineRegisterSet(labelEditVScale.get());
    // combineRegisterSet(labelEditCTD.get());
    // combineRegisterSet(labelEditCTM1.get());
    // combineRegisterSet(labelEditCTM2.get());
    // combineRegisterSet(labelEditCTA.get());
}

void QLscTab::readControlInfoUpdate()
{
    uint32_t value;
    labelCheckLscEnable->getSensorValue(value);
    labelCheckMonoEn->getSensorValue(value);
    //labelEditGainSel->getSensorValue(value);
    labelEditManCTOption->getSensorValue(value);
    labelEditHScale->getSensorValue(value);
    labelEditVScale->getSensorValue(value);
    //labelEditCurCT->getSensorValue(value);
    labelEditCTD->getSensorValue(value);
    labelEditCTM1->getSensorValue(value);
    labelEditCTM2->getSensorValue(value);
    labelEditCTA->getSensorValue(value);
}
