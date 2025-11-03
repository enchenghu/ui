#include "qgamma.h"
#include "Common/Others/qdefaultinfostorage.h"
#include "Common/UI/quidatamiddleware.h"
#include "ui_qgamma.h"
#include "Common/Others/qusbcommunication.h"
#include "qtransfermanager.h"

QGamma::QGamma(QWidget *parent, QString moudle)
    : QTabWndComm(parent)
    , ui(new Ui::QGamma)
    , referencesetIndex(SET_1)
    , regMoudleName("CURVE")
    , regMoudleNameHw("CCurveFilter_V60_HW")
    , regMoudleNameCurrentGain("PipeCtrl")
{
    moduleName = moudle.toStdString();
    strRgbIrMode = TransferManager::getInstance().getRgbIrMode() == 0 ? "RGB" : "IR";
    ui->setupUi(this);
    groupCurve           = new QGroupBox(this);
    groupReferenceSet    = new QGroupBox(this);
    groupImportExport    = new QGroupBox(this);
    layoutCurveOption    = new QHBoxLayout(this);
    layoutCurve          = new QHBoxLayout(groupCurve.get());
    layoutReferenceSet   = new QHBoxLayout(groupReferenceSet.get());
    layoutImportExport   = new QHBoxLayout(groupImportExport.get());
    labelCheckEnable     = new QLabelCheckBox();
    labelComboIndex      = new QLabelComboBox();
    labelEditCurrentGain = new QLabelEditGroup();
    labelEditCurveThre1  = new QLabelEditGroup();
    labelEditCurveThre2  = new QLabelEditGroup();
    curveChartGroup      = new QCurveChartGroup();
    tableRegValue        = new QTableListGroup();
    btnGroupLRTB         = new QDirectionButtonGroup();
    btnRead              = new QPushButton();
    btnReset             = new QPushButton();
    comboPointNum        = new QComboBox(this);
    comboCurveType       = new QComboBox(this);
    comboReferenceSet    = new QComboBox(this);
    btnReferenceSetSave  = new QPushButton(this);
    btnReferenceSetUse   = new QPushButton(this);
    btnImportData        = new QPushButton(this);
    btnExportData        = new QPushButton(this);
    readFlag=false;
    vecCombinedRegsPair1.resize(3);
    vecCombinedRegsPair2.resize(3);
    init();
    // qDebug() << "init() done.";
    getOptionInfoInit();
    // qDebug() << "getOptionInfoInit() done.";
    getGammmaInfoInit();
    comboCurveTypeValueChanged(GAMMA_BEZIER);
    for (int i = 0; i < GAMMA_TYPE_MAX; i++)
        for (int j = 0; j < REFERENCE_SET_MAX; j++)
            isSetSave[i][j] = false;
    btnReferenceSetUse->setEnabled(false);
    initConnect();
}

QGamma::~QGamma()
{
    delete ui;
}

void QGamma::init()
{
    // 设置显示文字
    if (moduleName == "Gamma") {
        strDelRegListTxt = QCoreApplication::applicationDirPath() + "\\extracted_data\\Gamma.txt";
        loadDeletedRegistersFromFile(strDelRegListTxt);
        labelCheckEnable->setLabel("GamEnable");
        curveChartGroup->setAxisRange(0, 255, 0, 1023);
        curveChartGroup->setYMax(1023);
        curveDelta = 3;
        
    } else {
        strDelRegListTxt = QCoreApplication::applicationDirPath() + "\\extracted_data\\GammaGain.txt";
        loadDeletedRegistersFromFile(strDelRegListTxt);
        labelCheckEnable->setLabel("Apply_h_en");
        curveChartGroup->setAxisRange(0, 255, 0, 4095);
        curveChartGroup->setYMax(4095);
        curveDelta = 9;
    }
    subPageMap.insert(0, moduleName.c_str());
    

    QStringList gammaIndex;
    gammaIndex << "Gamma A";
    gammaIndex << "Gamma B";
    labelComboIndex->setValue("Index", gammaIndex);
    labelComboIndex->setSpacing(95);
    labelEditCurrentGain->setLabelValue("Current Gain");
    labelEditCurveThre1->setLabelValue("*Curve Thre1");
    labelEditCurveThre2->setLabelValue("*Curve Thre2");
    labelEditCurrentGain->setEditWidth(80);
    labelEditCurveThre1->setEditWidth(80);
    labelEditCurveThre2->setEditWidth(80);

    QFont font;
    font.setBold(false);
    labelCheckEnable->setFont(font);
    labelComboIndex->setLabelFont(font);
    labelComboIndex->setFixedWidth(240);
    labelEditCurrentGain->setLabelFont(font);
    labelEditCurveThre1->setLabelFont(font);
    labelEditCurveThre2->setLabelFont(font);

    btnRead->setText("Read");
    btnReset->setText("Reset");
    btnReferenceSetSave->setText("Save");
    btnReferenceSetUse->setText("Use");
    btnImportData->setText("Import Data");
    btnExportData->setText("Export Data");
    comboCurveType->insertItem(GAMMA_CUBIC_SPLINE, "Cubic Spline");
    comboCurveType->insertItem(GAMMA_BEZIER, "Bezier");
    comboCurveType->setCurrentIndex(GAMMA_BEZIER);
    comboPointNum->insertItem(POINT_3, "3 Points");
    comboPointNum->insertItem(POINT_5, "5 Points");
    comboPointNum->insertItem(POINT_8, "8 Points");
    comboPointNum->insertItem(POINT_12, "12 Points");
    comboPointNum->insertItem(POINT_16, "16 Points");
    comboPointNum->insertItem(POINT_20, "20 Points");
    comboPointNum->insertItem(POINT_33, "33 Points");
    comboPointNum->insertItem(POINT_33_Single, "33 Points(Single)");
    comboPointNum->setCurrentIndex(POINT_33);
    comboReferenceSet->insertItem(SET_1, "Set 1");
    comboReferenceSet->insertItem(SET_2, "Set 2");
    comboReferenceSet->insertItem(SET_3, "Set 3");
    comboReferenceSet->insertItem(SET_4, "Set 4");
    comboReferenceSet->insertItem(SET_5, "Set 5");
    comboReferenceSet->setCurrentIndex(SET_1);

    QStringList gammaTable;
    gammaTable << "Addr";
    gammaTable << "Value";
    tableRegValue->setHeader(gammaTable);
    tableRegValue->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeMode::ResizeToContents);
    tableRegValue->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeMode::Stretch);

    layoutCurveOption->addWidget(btnGroupLRTB.get());
    layoutCurveOption->addWidget(btnRead.get());
    layoutCurveOption->addWidget(btnReset.get());
    int left, right, top, bottom;
    layoutCurveOption->getContentsMargins(&left, &top, &right, &bottom);
    layoutCurveOption->setContentsMargins(left, top + 15, right, bottom);

    layoutCurve->addWidget(comboPointNum.get());
    layoutCurve->addWidget(comboCurveType.get());
    groupCurve->setTitle("Curve");
    groupCurve->setLayout(layoutCurve.get());

    layoutReferenceSet->addWidget(comboReferenceSet.get());
    layoutReferenceSet->addWidget(btnReferenceSetSave.get());
    layoutReferenceSet->addWidget(btnReferenceSetUse.get());
    groupReferenceSet->setTitle("Reference Set");
    groupReferenceSet->setLayout(layoutReferenceSet.get());

    layoutImportExport->addWidget(btnImportData.get());
    layoutImportExport->addWidget(btnExportData.get());
    groupImportExport->setTitle("Import Export");
    groupImportExport->setLayout(layoutImportExport.get());

    // 设置控件布局
    ui->verticalLayoutLeft->addWidget(labelCheckEnable.get());
    ui->verticalLayoutLeft->addWidget(labelComboIndex.get());
    ui->verticalLayoutLeft->addWidget(labelEditCurrentGain.get());
    ui->verticalLayoutLeft->addWidget(labelEditCurveThre1.get());
    ui->verticalLayoutLeft->addWidget(labelEditCurveThre2.get());
    ui->verticalLayoutLeft->setStretch(0, 1); // Enable
    ui->verticalLayoutLeft->setStretch(1, 1); // index
    ui->verticalLayoutLeft->setStretch(2, 1); // curren gain
    ui->verticalLayoutLeft->setStretch(3, 1); // thre1
    ui->verticalLayoutLeft->setStretch(4, 1); // thre2

    ui->horizontalLayoutCurve->addWidget(curveChartGroup.get());
    ui->horizontalLayoutCurve->addWidget(tableRegValue.get());
    ui->horizontalLayoutCurve->setStretch(0, 3);
    ui->horizontalLayoutCurve->setStretch(1, 1);
    ui->horizontalLayoutButton->addLayout(layoutCurveOption.get());
    ui->horizontalLayoutButton->addWidget(groupCurve.get());
    ui->horizontalLayoutButton->addWidget(groupReferenceSet.get());
    ui->horizontalLayoutButton->addWidget(groupImportExport.get());
    ui->horizontalLayoutButton->setStretch(0, 5); // 上下左右
    ui->horizontalLayoutButton->setStretch(1, 4); // Curve
    ui->horizontalLayoutButton->setStretch(2, 4); // ReferenceSet
    ui->horizontalLayoutButton->setStretch(3, 4); // ImportExport

    // addContentLayout(ui->horizontalLayoutGamma, 0, 1);
    contentLayout->addLayout(ui->horizontalLayoutGamma, 0, 1);

    contentLayout->setVerticalSpacing(10);
    contentLayout->setHorizontalSpacing(10);

    btnReset->setVisible(false); // todo: delete after m1 is ok
}

void QGamma::initConnect()
{
    // Index
    connect(labelComboIndex.get(), SIGNAL(signalValueChange(int)), this, SLOT(labelComboIndexValueChanged(int)));

    // 上下左右调节
    connect(btnGroupLRTB.get(), SIGNAL(upSignal()), this, SLOT(btnUpClicked()));
    connect(btnGroupLRTB.get(), SIGNAL(downSignal()), this, SLOT(btnDownClicked()));
    connect(btnGroupLRTB.get(), SIGNAL(leftSignal()), this, SLOT(btnLeftClicked()));
    connect(btnGroupLRTB.get(), SIGNAL(rightSignal()), this, SLOT(btnRightClicked()));
    connect(curveChartGroup.get(), SIGNAL(signalAllPointsChanged(std::vector<QPoint>)), this,
        SLOT(curvePointsValueChanged(std::vector<QPoint>)));
    // Read Reset
    connect(btnRead.get(), SIGNAL(clicked()), this, SLOT(btnReadClicked()));
    connect(btnReset.get(), SIGNAL(clicked()), this, SLOT(btnResetClicked()));

    // Curve
    connect(comboPointNum.get(), SIGNAL(currentIndexChanged(int)), this, SLOT(comboPointNumValueChanged(int)));
    connect(comboCurveType.get(), SIGNAL(currentIndexChanged(int)), this, SLOT(comboCurveTypeValueChanged(int)));

    // ReferenceSet
    connect(comboReferenceSet.get(), SIGNAL(currentIndexChanged(int)), this, SLOT(comboReferenceSetValueChanged(int)));
    connect(btnReferenceSetSave.get(), SIGNAL(clicked()), this, SLOT(btnReferenceSetSaveClicked()));
    connect(btnReferenceSetUse.get(), SIGNAL(clicked()), this, SLOT(btnReferenceSetUseClicked()));

    // Import Export
    connect(btnImportData.get(), SIGNAL(clicked()), this, SLOT(btnImportDataClicked()));
    connect(btnExportData.get(), SIGNAL(clicked()), this, SLOT(btnExportDataClicked()));

    connect(this, &QTabWndComm::readSignal, this, [=]() {
        if(gammaList.size()==0)
            getGammmaInfoInit();
        btnReadClicked();
        readOptionInfoUpdate();
    });
    connect(this, &QTabWndComm::reConnectRead, this, &QGamma::reConnectReadPage);

    connect(&TransferManager::getInstance(), &TransferManager::rgbIrModeChanged, 
        this, &QGamma::refreshRegInfo);
}

void QGamma::labelComboIndexValueChanged(int index)
{
    // 切回33个点
    comboPointNum->setCurrentIndex(POINT_33);
    // 先更新gammaList
    getCurrentGammaListInfo();
    // 读取寄存器
    readGammaListInfo();
    // 更新表格的显示
    updateGammaListTable();
    // 更新点的值至曲线
    std::vector<QPoint> point = getGammaListPoints();
    curveChartGroup->setPoints(point);
    // 更新ReferenceSet
    btnReferenceSetUse->setEnabled(isSetSave[index][comboReferenceSet->currentIndex()]);
}

void QGamma::btnUpClicked()
{
    curveChartGroup->highlightPointYIncrease(curveDelta);
}

void QGamma::btnDownClicked()
{
    curveChartGroup->highlightPointYDecrease(curveDelta);
}

void QGamma::btnLeftClicked()
{
    curveChartGroup->switchHighlightPointToLeft();
}

void QGamma::btnRightClicked()
{
    curveChartGroup->switchHighlightPointToRight();
}

void QGamma::curvePointsValueChanged(std::vector<QPoint> vecPoint)
{
    // 写寄存器
    for (int i = 0; i < gammaList.size(); i++) {
        gammaList[i].second = vecPoint[i].y();
        writeCombinedRegisterValue(gammaList[i].first, gammaList[i].second);
    }
    // 更新表格
    updateGammaListTable();
}

void QGamma::btnReadClicked()
{
    // 切回33个点
    comboPointNum->setCurrentIndex(POINT_33);
    // 读寄存器
    readGammaListInfo();
    // 更新表格
    updateGammaListTable();
    // 更新点的值至曲线
    std::vector<QPoint> point = getGammaListPoints();
    curveChartGroup->setPoints(point);
}

void QGamma::btnResetClicked()
{
    int cameraId = getCurrentCamera();

    // 切回33个点
    comboPointNum->setCurrentIndex(POINT_33);

    std::vector<std::pair<CombinedRegister, uint32_t>> gammaListTemp =
        QDefaultInfoStorage::getInstance().getCurveDefaultInfo(QString::fromStdString(moduleName), cameraId,
            labelComboIndex->getCurrentIndex());

    if (gammaListTemp.size() != 0) {
        gammaList.swap(gammaListTemp);
        // 写寄存器
        for (int i = 0; i < gammaList.size(); i++)
            writeCombinedRegisterValue(gammaList[i].first, gammaList[i].second);
        // 更新表格
        updateGammaListTable();
        // 更新点的值至曲线
        std::vector<QPoint> point = getGammaListPoints();
        curveChartGroup->setPoints(point);
    }
}

void QGamma::comboPointNumValueChanged(int index)
{
    int _type;
    std::vector<int> vecIndex;
    switch (index) {
    case POINT_3: {
        vecIndex.push_back(0);
        vecIndex.push_back(15);
        vecIndex.push_back(32);
        _type=POINT_3;
    } break;
    case POINT_5: {
        vecIndex.push_back(0);
        vecIndex.push_back(4);
        vecIndex.push_back(10);
        vecIndex.push_back(20);
        vecIndex.push_back(32);
        _type=POINT_5;
    } break;
    case POINT_8: {
        vecIndex.push_back(0);
        vecIndex.push_back(3);
        vecIndex.push_back(8);
        vecIndex.push_back(12);
        vecIndex.push_back(17);
        vecIndex.push_back(22);
        vecIndex.push_back(26);
        vecIndex.push_back(32);
        _type=POINT_8;
    } break;
    case POINT_12: {
        vecIndex.push_back(0);
        vecIndex.push_back(2);
        vecIndex.push_back(6);
        vecIndex.push_back(9);
        vecIndex.push_back(12);
        vecIndex.push_back(15);
        vecIndex.push_back(18);
        vecIndex.push_back(21);
        vecIndex.push_back(24);
        vecIndex.push_back(27);
        vecIndex.push_back(30);
        vecIndex.push_back(32);
        _type=POINT_12;
    } break;
    case POINT_16: {
        vecIndex.push_back(0);
        vecIndex.push_back(2);
        vecIndex.push_back(4);
        vecIndex.push_back(6);
        vecIndex.push_back(8);
        vecIndex.push_back(10);
        vecIndex.push_back(12);
        vecIndex.push_back(14);
        vecIndex.push_back(16);
        vecIndex.push_back(18);
        vecIndex.push_back(20);
        vecIndex.push_back(22);
        vecIndex.push_back(24);
        vecIndex.push_back(26);
        vecIndex.push_back(28);
        vecIndex.push_back(32);
        _type=POINT_16;
    } break;
    case POINT_20: {
        vecIndex.push_back(0);
        vecIndex.push_back(2);
        vecIndex.push_back(4);
        vecIndex.push_back(6);
        vecIndex.push_back(8);
        vecIndex.push_back(9);
        vecIndex.push_back(11);
        vecIndex.push_back(12);
        vecIndex.push_back(13);
        vecIndex.push_back(15);
        vecIndex.push_back(16);
        vecIndex.push_back(18);
        vecIndex.push_back(19);
        vecIndex.push_back(21);
        vecIndex.push_back(22);
        vecIndex.push_back(24);
        vecIndex.push_back(26);
        vecIndex.push_back(28);
        vecIndex.push_back(30);
        vecIndex.push_back(32);
        _type=POINT_20;
    } break;
    case POINT_33_Single:{
        _type=POINT_33_Single;
        for (int i = 0; i < GAMMAPOINT; i++)
            vecIndex.push_back(i);
    } break;
    case POINT_33:{
        _type=POINT_33;
        for (int i = 0; i < GAMMAPOINT; i++)
            vecIndex.push_back(i);
    } break;
    default: {
        for (int i = 0; i < GAMMAPOINT; i++)
            vecIndex.push_back(i);
    } break;
    }
    bool ret=curveChartGroup->setCurvePointType(_type);
    if(!ret)
        return;
    curveChartGroup->setCurveFittingPointIndex(vecIndex);
}

void QGamma::comboCurveTypeValueChanged(int index)
{
    curveChartGroup->setCurveFittingType((CurveFittingType)index);
}

void QGamma::comboReferenceSetValueChanged(int index)
{
    int type = labelComboIndex->getCurrentIndex();
    btnReferenceSetUse->setEnabled(isSetSave[type][index]);
}

void QGamma::btnReferenceSetSaveClicked()
{
    int type  = labelComboIndex->getCurrentIndex();
    int index = comboReferenceSet->currentIndex();
    gammaValueReferenceSet[type][index].clear();
    for (int i = 0; i < gammaList.size(); i++)
        gammaValueReferenceSet[type][index].push_back(gammaList[i].second);

    isSetSave[type][index] = true;
    btnReferenceSetUse->setEnabled(isSetSave[type][index]);
}

void QGamma::btnReferenceSetUseClicked()
{
    // 切回33个点
    comboPointNum->setCurrentIndex(POINT_33);

    int type  = labelComboIndex->getCurrentIndex();
    int index = comboReferenceSet->currentIndex();
    for (int i = 0; i < gammaValueReferenceSet[type][index].size(); i++) {
        gammaList[i].second = gammaValueReferenceSet[type][index][i];
        writeCombinedRegisterValue(gammaList[i].first, gammaList[i].second);
    }

    // 更新表格
    updateGammaListTable();
    // 更新点的值至曲线
    std::vector<QPoint> point = getGammaListPoints();
    curveChartGroup->setPoints(point);
}

void QGamma::btnImportDataClicked()
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
    if (regValue.size() != 0) {
        // 切回33个点
        comboPointNum->setCurrentIndex(POINT_33);
        writeSensorRegisterValue(regValue);
        btnReadClicked();
    }
}

void QGamma::btnExportDataClicked()
{
    QString fileName = iniPathName() + "Save";
    QDir    dir(fileName);
    if (!dir.exists())
        dir.mkpath(fileName);
    fileName += "/" + QString::fromStdString(moduleName) + "_" + labelComboIndex->getValue(-1) + "_" + getCurrentTime()
        + ".txt";

    QString fileSavePath = QFileDialog::getSaveFileName(this, tr("Save File"), fileName, tr("Text Files (*.txt)"));
    if (!fileSavePath.isEmpty()) {
        QFile file(fileSavePath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream                                txtOutput(&file);
            QString                                    line         = "";
            std::vector<std::pair<uint32_t, uint32_t>> registerList = getCombineRegisterAddressValueSingle();
            for (int i = 0; i < registerList.size(); i++) {
                line = QString("0x%1").arg(
                           QString::number(registerList[i].first, 16).toUpper().rightJustified(ADDRESS_HEX_INFO, '0'))
                    + ","
                    + QString("0x%1").arg(
                        QString::number(registerList[i].second, 16).toUpper().rightJustified(VALUE_HEX_INFO, '0'))
                    + "\n";
                txtOutput << line;
            }
            file.close();
        }
    }
}

void QGamma::getGammmaInfoInit()
{
    gammaList.clear();
    gammaListA.clear();
    gammaListB.clear();

    std::vector<UiInfo> uiInfo;
    QString             uiName;
    int                 cameraId = getCurrentCamera();
    // for (int i = (GAMMA_TYPE_MAX - 1); i >= 0; i--) {
    //     uiName = labelComboIndex->getValue(i);
    //     uiInfo = DatabaseOrm::getUiInfoByModuleNameAndControlName(moduleName, (uiName.toStdString()));
    //     for (auto i : uiInfo) {
    //         CombinedRegister combineReg = DatabaseOrm::getCombinedRegisterByModuleNameAndCameraIdAndCombinedName(
    //             (regMoudleName.toStdString()), std::make_shared<int>(cameraId), *(i.combinedName));
    //         std::vector<uint32_t> regList = getCombineRegisterAddressSingle(combineReg);
    //         registerMoudle.insert(registerMoudle.end(), regList.begin(), regList.end());
    //         gammaList.push_back(std::make_pair(combineReg, 0));
    //     }
    // }
    std::string strSuffix[] = {
        "", " RGB_AB", " IR_AB"
    };

    std::string strRegSuffix[] = {
        "", "_RGB_AB", "_IR_AB"
    };

    std::vector<std::pair<CombinedRegister, uint32_t>>* gammaListPtrArrayB[] = {
        &gammaListB, &gammaListBRgb, &gammaListBIr
    };

    // std::vector<std::vector<uint32_t>> vecRegList(3);
    for (int i = 0; i < 3; i++)
    {
        uiName = labelComboIndex->getValue(1);
        uiInfo = DatabaseOrm::getUiInfoByModuleNameAndControlName(moduleName, (uiName.toStdString() + strSuffix[i]));
        // qDebug() << "uiInfo[" << i << "].size() = " << uiInfo.size();
        for (auto j : uiInfo) 
        {
            CombinedRegister combineReg = DatabaseOrm::getCombinedRegisterByModuleNameAndCameraIdAndCombinedName(
                (regMoudleName.toStdString() + strRegSuffix[i]), std::make_shared<int>(cameraId), *(j.combinedName));
            std::vector<uint32_t> regList = getCombineRegisterAddressSingle(combineReg);
            if (i == 0)
                registerMoudle.insert(registerMoudle.end(), regList.begin(), regList.end());
            
            setRegisterSubPage(combineReg, moduleName.c_str());
            (gammaListPtrArrayB[i])->push_back(std::make_pair(combineReg, 0));
        }
        // qDebug() << "gammaListPtrArrayB[" << i << "].size() = " << (gammaListPtrArrayB[i])->size();
    }

    std::vector<CombinedRegister> vecGammaB, vecGammaBRgb, vecGammaBIr;

    for (int i = 0; i < gammaListB.size(); i++)
    {
        vecGammaB.emplace_back(gammaListB[i].first);
        vecGammaBRgb.emplace_back(gammaListBRgb[i].first);
        vecGammaBIr.emplace_back(gammaListBIr[i].first);
    }

    setRegisterTypeMap(vecGammaB, vecGammaBRgb, vecGammaBIr);

    std::vector<std::pair<CombinedRegister, uint32_t>>* gammaListPtrArrayA[] = {
        &gammaListA, &gammaListARgb, &gammaListAIr
    };

    for (int i = 0; i < 3; i++)
    {
        uiName = labelComboIndex->getValue(0);
        uiInfo = DatabaseOrm::getUiInfoByModuleNameAndControlName(moduleName, (uiName.toStdString() + strSuffix[i]));
        for (auto j : uiInfo) 
        {
            CombinedRegister combineReg = DatabaseOrm::getCombinedRegisterByModuleNameAndCameraIdAndCombinedName(
                (regMoudleName.toStdString() + strRegSuffix[i]), std::make_shared<int>(cameraId), *(j.combinedName));
            std::vector<uint32_t> regList = getCombineRegisterAddressSingle(combineReg);
            if (i == 0)
                registerMoudle.insert(registerMoudle.end(), regList.begin(), regList.end());
            
            setRegisterSubPage(combineReg, moduleName.c_str());
            (gammaListPtrArrayA[i])->push_back(std::make_pair(combineReg, 0));
        }
        // qDebug() << "gammaListPtrArrayA[" << i << "].size() = " << (gammaListPtrArrayA[i])->size();
    }

    std::vector<CombinedRegister> vecGammaA, vecGammaARgb, vecGammaAIr;
    for (int i = 0; i < gammaListA.size(); i++)
    {
        vecGammaA.emplace_back(gammaListA[i].first);
        vecGammaARgb.emplace_back(gammaListARgb[i].first);
        vecGammaAIr.emplace_back(gammaListAIr[i].first);
    }

    setRegisterTypeMap(vecGammaA, vecGammaARgb, vecGammaAIr);


    QUsbCommunication::getInstance().readBurst(IR_RGB_MODE_REG_ADDR, (unsigned char*)(&iModeRegValue), ADDRESS_LENGTH, VALUE_LENGTH);
    if (iModeRegValue == 0x01) {
        // RGB
        gammaList = strRgbIrMode == "RGB" ? gammaListARgb : gammaListAIr;
    }
    else
    {
        // RGB+IR
        gammaList = gammaListA;
    }
}

void QGamma::getOptionInfoInit()
{
    std::vector<UiInfo> uiInfo;
    QString             uiName;
    QString             uiEditInfo;
    int                 cameraId = getCurrentCamera();

    std::string strSuffix[] = {
        "", " RGB_AB", " IR_AB"
    };

    std::string strRegSuffix[] = {
        "", "_RGB_AB", "_IR_AB"
    };

    // Enable
    
    labelCheckEnable->getLabel(uiName);
    uiInfo = DatabaseOrm::getUiInfoByModuleNameAndControlName(moduleName, (uiName.toStdString()));
    for (auto i : uiInfo) {
        CombinedRegister combineReg = DatabaseOrm::getCombinedRegisterByModuleNameAndCameraIdAndCombinedName(
            (regMoudleNameHw.toStdString()), nullptr, *(i.combinedName));
        labelCheckEnable->setCombineRegister(combineReg);
        std::vector<uint32_t> regList = getCombineRegisterAddressSingle(combineReg);
        registerMoudle.insert(registerMoudle.end(), regList.begin(), regList.end());
        setRegisterSubPage(combineReg, moduleName.c_str());
    }

    // CurrentGain
    labelEditCurrentGain->getValue(uiName, uiEditInfo);
    uiInfo = DatabaseOrm::getUiInfoByModuleNameAndControlName(moduleName, (uiName.toStdString()));
    for (auto i : uiInfo) {
        CombinedRegister combineReg = DatabaseOrm::getCombinedRegisterByModuleNameAndCameraIdAndCombinedName(
            (regMoudleNameCurrentGain.toStdString()), std::make_shared<int>(cameraId), *(i.combinedName));
        labelEditCurrentGain->setCombineRegister(combineReg);
        std::vector<uint32_t> regList = getCombineRegisterAddressSingle(combineReg);
        registerMoudle.insert(registerMoudle.end(), regList.begin(), regList.end());
        setRegisterSubPage(combineReg, moduleName.c_str());
    }

    // CurveThre1
    labelEditCurveThre1->getValue(uiName, uiEditInfo);

    for (int idx = 0; idx < 3; idx++)
    {
        std::vector<CombinedRegister>().swap(vecCombinedRegsPair1[idx]);
        std::vector<CombinedRegister>().swap(vecCombinedRegsPair2[idx]);
    }
    
    for (int idx = 0; idx < 3; idx++)
    {
        uiInfo = DatabaseOrm::getUiInfoByModuleNameAndControlName(moduleName, (uiName.remove("*").toStdString()) + strSuffix[idx]);
        // qDebug() << "OptionInfo uiInfo[" << idx << "].size() = " << uiInfo.size();
        // qDebug() << moduleName.c_str() << uiName + strSuffix[idx].c_str();
        for (auto i : uiInfo) {
            CombinedRegister combineReg = DatabaseOrm::getCombinedRegisterByModuleNameAndCameraIdAndCombinedName(
                (regMoudleName.toStdString() + strRegSuffix[idx]), std::make_shared<int>(cameraId), *(i.combinedName));
            vecCombinedRegsPair1[idx].emplace_back(combineReg);
            // labelEditCurveThre1->setCombineRegister(combineReg);
            std::vector<uint32_t> regList = getCombineRegisterAddressSingle(combineReg);
            if (idx == 0)
                registerMoudle.insert(registerMoudle.end(), regList.begin(), regList.end());
            setRegisterSubPage(combineReg, moduleName.c_str());
        }
    }

    setRegisterTypeMap(vecCombinedRegsPair1[0], vecCombinedRegsPair1[1], vecCombinedRegsPair1[2]);
    
    // CurveThre2
    labelEditCurveThre2->getValue(uiName, uiEditInfo);
    
    for (int idx = 0; idx < 3; idx++)
    {
        // std::vector<CombinedRegister>().swap(vecCombinedRegsPair[idx]);
        uiInfo = DatabaseOrm::getUiInfoByModuleNameAndControlName(moduleName, (uiName.remove("*").toStdString()) + strSuffix[idx]);
        for (auto i : uiInfo) {
            CombinedRegister combineReg = DatabaseOrm::getCombinedRegisterByModuleNameAndCameraIdAndCombinedName(
                (regMoudleName.toStdString() + strRegSuffix[idx]), std::make_shared<int>(cameraId), *(i.combinedName));
            vecCombinedRegsPair2[idx].emplace_back(combineReg);
            // labelEditCurveThre2->setCombineRegister(combineReg);
            std::vector<uint32_t> regList = getCombineRegisterAddressSingle(combineReg);
            if (idx == 0)
                registerMoudle.insert(registerMoudle.end(), regList.begin(), regList.end());
            setRegisterSubPage(combineReg, moduleName.c_str());
        }
    }

    setRegisterTypeMap(vecCombinedRegsPair2[0], vecCombinedRegsPair2[1], vecCombinedRegsPair2[2]);
    
    QUsbCommunication::getInstance().readBurst(IR_RGB_MODE_REG_ADDR, (unsigned char*)(&iModeRegValue), ADDRESS_LENGTH, VALUE_LENGTH);
    if (iModeRegValue == 0x01) {
        // RGB
        auto combineReg1 = strRgbIrMode == "RGB" ? vecCombinedRegsPair1[1][0] : vecCombinedRegsPair1[2][0];
        labelEditCurveThre1->setCombineRegister(combineReg1);
        auto combineReg2 = strRgbIrMode == "RGB" ? vecCombinedRegsPair2[1][0] : vecCombinedRegsPair2[2][0];
        labelEditCurveThre2->setCombineRegister(combineReg2);
    }
    else
    {
        // RGB+IR
        labelEditCurveThre1->setCombineRegister(vecCombinedRegsPair1[0][0]);
        labelEditCurveThre2->setCombineRegister(vecCombinedRegsPair1[0][0]);
    }
}

void QGamma::getCurrentGammaListInfo()
{
    gammaList.clear();
    if(labelComboIndex->getCurrentIndex()==0)
    {
        gammaList=gammaListA;
        QUsbCommunication::getInstance().readBurst(IR_RGB_MODE_REG_ADDR, (unsigned char*)(&iModeRegValue), ADDRESS_LENGTH, VALUE_LENGTH);
        if (iModeRegValue == 0x01) {
            gammaList = strRgbIrMode == "RGB" ? gammaListARgb : gammaListAIr;
        } 
        else
        {
            
        }
    }
        
    else if(labelComboIndex->getCurrentIndex()==1)
    {
        gammaList=gammaListB;
        QUsbCommunication::getInstance().readBurst(IR_RGB_MODE_REG_ADDR, (unsigned char*)(&iModeRegValue), ADDRESS_LENGTH, VALUE_LENGTH);
        if (iModeRegValue == 0x01) {
            gammaList = strRgbIrMode == "RGB" ? gammaListBRgb : gammaListBIr;
        } 
        else
        {
            
        }
    }
        

    // std::vector<UiInfo> uiInfo;
    // QString             uiName;

    // gammaList.clear();
    // // gammaList
    // uiName = labelComboIndex->getValue(-1);
    // uiInfo = DatabaseOrm::getUiInfoByModuleNameAndControlName(moduleName, (uiName.toStdString()));
    // for (auto i : uiInfo) {
    //     CombinedRegister combineReg = DatabaseOrm::getCombinedRegisterByModuleNameAndCameraIdAndCombinedName(
    //         (regMoudleName.toStdString()), std::make_shared<int>(cameraId), *(i.combinedName));
    //     gammaList.push_back(std::make_pair(combineReg, 0));
    // }
}

void QGamma::readGammaListInfo()
{
    // 更新坐标
    uint32_t value;
    for (int i = 0; i < gammaList.size(); i++) {
        readCombinedRegisterValue(gammaList[i].first, value);
        gammaList[i].second = value;
    }
}

void QGamma::readOptionInfoUpdate()
{
    uint32_t value;
    QUsbCommunication::getInstance().readBurst(IR_RGB_MODE_REG_ADDR, (unsigned char*)(&iModeRegValue), ADDRESS_LENGTH, VALUE_LENGTH);
    if (iModeRegValue == 0x01) {
        // RGB
        auto combineReg1 = strRgbIrMode == "RGB" ? vecCombinedRegsPair1[1][0] : vecCombinedRegsPair1[2][0];
        labelEditCurveThre1->setCombineRegister(combineReg1);
        auto combineReg2 = strRgbIrMode == "RGB" ? vecCombinedRegsPair2[1][0] : vecCombinedRegsPair2[2][0];
        labelEditCurveThre2->setCombineRegister(combineReg2);
    }
    else
    {
        // RGB+IR
        labelEditCurveThre1->setCombineRegister(vecCombinedRegsPair1[0][0]);
        labelEditCurveThre2->setCombineRegister(vecCombinedRegsPair1[0][0]);
    }
    // Enable
    labelCheckEnable->getSensorValue(value);
    // CurrentGain
    labelEditCurrentGain->getSensorValue(value);
    // CurveThre1
    labelEditCurveThre1->getSensorValue(value);
    // CurveThre2
    labelEditCurveThre2->getSensorValue(value);
}

void QGamma::updateGammaListTable()
{
    QString address;
    tableRegValue->clearRows();
    for (int i = 0; i < gammaList.size(); i++) {
        address = getCombinedRegisterAddressString(gammaList[i].first);
        tableRegValue->appendRow(i, 0, address);
        tableRegValue->appendRow(i, 1, QString::number(gammaList[i].second, 16));
    }
}

void QGamma::updateOptionListInfo() { }

std::vector<QPoint> QGamma::getGammaListPoints()
{
    std::vector<QPoint> vecPoint;
    QPoint              point;
    for (int i = 0; i < gammaList.size(); i++) {
        if (i == 0) {
            point.setX(0);
        } else if (i == 1) {
            point.setX(3);
        } else {
            point.setX((i - 1) * 8 - 1);
        }
        point.setY(gammaList[i].second);
        vecPoint.push_back(point);
    }

    return vecPoint;
}

void QGamma::reConnectReadPage()
{
    registerMoudle.clear();
    getOptionInfoInit();
    getGammmaInfoInit();
    btnReadClicked();
    readOptionInfoUpdate();
}

void QGamma::refreshRegInfo(int &newMode) 
{ 
    if (newMode == 0) 
    {
        strRgbIrMode = "RGB";
    } 
    else if (newMode == 1) 
    {
        strRgbIrMode = "IR";
    }

    QUsbCommunication::getInstance().readBurst(IR_RGB_MODE_REG_ADDR, (unsigned char*)(&iModeRegValue), ADDRESS_LENGTH, VALUE_LENGTH);
    if (iModeRegValue == 0x01) {
        // RGB
        auto combineReg1 = strRgbIrMode == "RGB" ? vecCombinedRegsPair1[1][0] : vecCombinedRegsPair1[2][0];
        labelEditCurveThre1->setCombineRegister(combineReg1);
        auto combineReg2 = strRgbIrMode == "RGB" ? vecCombinedRegsPair2[1][0] : vecCombinedRegsPair2[2][0];
        labelEditCurveThre2->setCombineRegister(combineReg2);
    }
    else
    {
        // RGB+IR
        labelEditCurveThre1->setCombineRegister(vecCombinedRegsPair1[0][0]);
        labelEditCurveThre2->setCombineRegister(vecCombinedRegsPair1[0][0]);
    }

    // 切回33个点

    // 先更新gammaList
    getCurrentGammaListInfo();
    // 读取寄存器
    readGammaListInfo();
    // 更新表格的显示
    updateGammaListTable();
    // 更新点的值至曲线
    std::vector<QPoint> point = getGammaListPoints();
    curveChartGroup->setPoints(point);
    // 更新ReferenceSet
    btnReferenceSetUse->setEnabled(isSetSave[labelComboIndex->getCurrentIndex()][comboReferenceSet->currentIndex()]);
}

QString QGamma::getCombinedRegisterAddressString(CombinedRegister &combineReg)
{
    QString                      address    = "";
    std::vector<RegisterBitInfo> regBitInfo = DatabaseOrm::getRegisterBitInfoByCombinedRegister(combineReg);
    for (auto item : regBitInfo) {
        if (address != "")
            address += ",";
        address += QString::number(item.registerAddr, 16).rightJustified(ADDRESS_HEX_INFO, '0');
    }

    return address;
}

std::vector<uint32_t> QGamma::getCombineRegisterAddressSingle(CombinedRegister &combineReg)
{
    std::vector<uint32_t>        list;
    std::vector<RegisterBitInfo> regBitInfo = DatabaseOrm::getRegisterBitInfoByCombinedRegister(combineReg);
    for (auto item : regBitInfo) {
        list.push_back(item.registerAddr);
    }
    return list;
}

std::vector<std::pair<uint32_t, uint32_t>> QGamma::getCombineRegisterAddressValueSingle()
{
    std::vector<std::pair<uint32_t, uint32_t>> list;
    for (int i = 0; i < gammaList.size(); i++) {
        std::vector<RegisterBitInfo> regBitInfo = DatabaseOrm::getRegisterBitInfoByCombinedRegister(gammaList[i].first);
        int                          count      = static_cast<int>(regBitInfo.size());
        for (int j = 0; j < count; j++) {
            uint32_t value = (((gammaList[i].second) & (0xff << (j * 8))) >> (j * 8));
            list.push_back(std::make_pair(regBitInfo[j].registerAddr, value));
        }
    }

    return list;
}
