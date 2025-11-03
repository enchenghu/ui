#include "qcactab.h"
#include "Common/UI/qcommonuiinterface.h"
#include "Common/UI/qlabeleditgroup.h"
#include "Common/UI/qtabwndcomm.h"
#include <QComboBox>
#include <QGroupBox>
#include <QPointer>
#include <QSplitter>
#include <set>

const QString     TAB_NAME_L   = "CAC_L";
const QString     TAB_NAME_M   = "CAC_M";
const QString     TAB_NAME_S    = "CAC_S";
const QString     TAB_NAME_LFS  = "CAC_LFS";
const QStringList TAB_NAME_LIST = { TAB_NAME_L,TAB_NAME_M, TAB_NAME_S, TAB_NAME_LFS };

QCacTab::QCacTab(QWidget *parent)
    : QTabWndComm(parent)
    , subTabLayout(nullptr)
{
    moduleName = "CAC";
    strDelRegListTxt = QCoreApplication::applicationDirPath() + "\\extracted_data\\CAC.txt";
    loadDeletedRegistersFromFile(strDelRegListTxt);
    init();
}

void QCacTab::init()
{
    subPageMap.insert(0, moduleName.c_str());
    // for (int i = 0; i < TAB_NAME_LIST.size(); i++) {
    //     subPageMap.insert(i, TAB_NAME_LIST[i]);
    // }
    addRegisterTable();
    contentWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    contentLayout->setAlignment(Qt::AlignCenter);
    contentLayout->setSpacing(10);
    contentLayout->setContentsMargins(20, 10, 10, 10);
    QPointer<QGroupBox> groupBox = new QGroupBox(contentWidget);
    groupBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    groupBox->setTitle("Exp_Type");
    contentLayout->addWidget(groupBox, 0, 0, 1, 1);
    QPointer<QComboBox> comboBox = new QComboBox(groupBox);
    comboBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    comboBox->addItems(TAB_NAME_LIST);
    comboBox->setMaximumSize(100, 30);
    groupBox->setLayout(new QVBoxLayout());
    groupBox->layout()->addWidget(comboBox);
    subTabLayout = new QHBoxLayout();
    contentLayout->addLayout(subTabLayout, 1, 0, 1, 2);
    initLmSubTab(TAB_NAME_L);
    initLmSubTab(TAB_NAME_M);
    initSOrLfsSubTab(TAB_NAME_S);
    initSOrLfsSubTab(TAB_NAME_LFS);
    contentLayout->setRowStretch(0, 1);
    contentLayout->setRowStretch(1, 7);
    contentLayout->setColumnStretch(0, 1);
    contentLayout->setColumnStretch(1, 6);
    connect(
        comboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
        [=](int index) {
            for (int i = 0; i < TAB_NAME_LIST.size(); i++) {
                if (i != index) {
                    tabMap[TAB_NAME_LIST[i]]->hide();
                }
            }
            tabMap[TAB_NAME_LIST[index]]->show();
            read();
        },
        Qt::QueuedConnection);
    comboBox->setCurrentIndex(0);
    connect(this, &QTabWndComm::updateValue, this, &QTabWndComm::read, Qt::QueuedConnection);
    connect(
        this, &QTabWndComm::reConnectRead, this,
        [=]() {
            registerMoudle.clear();
            std::set<uint32_t> addressSet;
            for (auto tabName : TAB_NAME_LIST) {
                auto allRegisters = QUiDataMiddleware::getRegisterAddressByUiModule(tabName);
                for (auto reg : allRegisters) {
                    if (addressSet.find(reg) != addressSet.end())
                        continue;
                    registerMoudle.push_back(reg);
                    addressSet.insert(reg);
                }
            }

            registerSubPageMap.clear();
            loadDeletedRegistersFromFile(strDelRegListTxt);

            for (auto reg : registerMoudle)
            {
                // registerMoudle.push_back(reg);
                // if (registerSubPageMap.count(reg) == 0)
                //     registerSubPageMap.insert(reg, std::vector<QString>(1, moduleName.c_str()));
                // else
                registerSubPageMap[reg].emplace_back(moduleName.c_str());
            }
        },
        Qt::QueuedConnection);
    // emit reConnectRead();
    reConnectReadExecute();
}

void QCacTab::reConnectReadExecute()
{
    registerMoudle.clear();
    std::set<uint32_t> addressSet;
    for (auto tabName : TAB_NAME_LIST) {
        auto allRegisters = QUiDataMiddleware::getRegisterAddressByUiModule(tabName);
        for (auto reg : allRegisters) {
            if (addressSet.find(reg) != addressSet.end())
                continue;
            registerMoudle.push_back(reg);
            addressSet.insert(reg);
        }
    }

    registerSubPageMap.clear();
    loadDeletedRegistersFromFile(strDelRegListTxt);

    for (auto reg : registerMoudle)
    {
        // registerMoudle.push_back(reg);
        // if (registerSubPageMap.count(reg) == 0)
        //     registerSubPageMap.insert(reg, std::vector<QString>(1, moduleName.c_str()));
        // else
        registerSubPageMap[reg].emplace_back(moduleName.c_str());
    }
}

void QCacTab::initLmSubTab(QString tabName)
{
    tabMap[tabName] = new QWidget(contentWidget);
    tabMap[tabName]->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    tabMap[tabName]->setLayout(new QGridLayout());
    tabMap[tabName]->layout()->setAlignment(Qt::AlignLeft);
    subTabLayout->addWidget(tabMap[tabName]);
    if(tabName=="CAC_M")
        tabMap[tabName]->hide();
    //Sat与Exposed列位置
    int nPosition=0;
    QVector<QPair<QString, QString>> firstColumn ;
    if(m_nLevel==LEVEL_ADMINISTRATOR)
    {
        firstColumn = {
            { "QLabelCheckBox", "CAC_Enable" },
            { "QLabelCheckBox", "Yellow_Fringle" },
            { "QLabelEditGroup", "CAC_Weight" },
            // { "", "" },
            // { "", "" },
            // { "", "" },
            // { "QLabelEditGroup", "Color_Weight[0]" },
            // { "QLabelEditGroup", "Color_Weight[1]" },
            // { "QLabelEditGroup", "Color_Weight[2]" },
            };
        nPosition=1;
    }
    else
    {
        firstColumn = {
            { "QLabelCheckBox", "CAC_Enable" },
            { "QLabelCheckBox", "Yellow_Fringle" },
            { "QLabelEditGroup", "CAC_Weight" },
            };
        nPosition=0;
    }
    for (int i = 0; i < firstColumn.size(); i++) {
        insertControl(dynamic_cast<QGridLayout *>(tabMap[tabName]->layout()),
            QCommonUiInterfaceFactory::instance().createUiInterface(firstColumn[i].first, tabName,
                firstColumn[i].second, this),
            i, 0, 1, 1);
    }
    const QVector<QPair<QString, QString>> contrastConditionGroup = {
        { "QLabelEditGroup", "Contrast_Low_Thre" },
        { "QLabelEditGroup", "Contrast_High_Thre" },
        { "QLabelEditGroup", "Contrast_Thre" },
    };
    QPointer<QGroupBox> contrastConditionGroupBox = new QGroupBox(tabMap[tabName]);
    contrastConditionGroupBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    contrastConditionGroupBox->setTitle("Contrast Condition");
    dynamic_cast<QGridLayout *>(tabMap[tabName]->layout())->addWidget(contrastConditionGroupBox, 0, 1, 3, 1);
    contrastConditionGroupBox->setLayout(new QGridLayout());
    for (int i = 0; i < contrastConditionGroup.size(); i++) {
        insertControl(dynamic_cast<QGridLayout *>(contrastConditionGroupBox->layout()),
            QCommonUiInterfaceFactory::instance().createUiInterface(contrastConditionGroup[i].first, tabName,
                contrastConditionGroup[i].second, this),
            i, 0, 1, 1, 1);
    }
    const QVector<QPair<QString, QString>> satConditionsGroup = {
        { "QLabelEditGroup", "Yellow_Sat_Thre" },
        { "QLabelEditGroup", "Purple_Sat_Thre" },
    };
    QPointer<QGroupBox> satConditionsGroupBox = new QGroupBox(tabMap[tabName]);
    satConditionsGroupBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    satConditionsGroupBox->setTitle("Sat Conditions");
    dynamic_cast<QGridLayout *>(tabMap[tabName]->layout())->addWidget(satConditionsGroupBox, 3, nPosition, 2, 1);
    satConditionsGroupBox->setLayout(new QGridLayout());
    for (int i = 0; i < satConditionsGroup.size(); i++) {
        insertControl(dynamic_cast<QGridLayout *>(satConditionsGroupBox->layout()),
            QCommonUiInterfaceFactory::instance().createUiInterface(satConditionsGroup[i].first, tabName,
                satConditionsGroup[i].second, this),
            i, 0, 1, 1, true);
    }

    const QVector<QPair<QString, QString>> overExpEdgeGroup = {
        { "QLabelCheckBox", "Only_Over_Exposed" },
        { "QLabelEditGroup", "Over_Exposure_Thre" },
    };
    QPointer<QGroupBox> overExpEdgeGroupBox = new QGroupBox(tabMap[tabName]);
    overExpEdgeGroupBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    overExpEdgeGroupBox->setTitle("Over_Exp_Edge");
    dynamic_cast<QGridLayout *>(tabMap[tabName]->layout())->addWidget(overExpEdgeGroupBox, 5, nPosition, 2, 1);
    overExpEdgeGroupBox->setLayout(new QGridLayout());
    for (int i = 0; i < overExpEdgeGroup.size(); i++) {
        insertControl(dynamic_cast<QGridLayout *>(overExpEdgeGroupBox->layout()),
            QCommonUiInterfaceFactory::instance().createUiInterface(overExpEdgeGroup[i].first, tabName,
                overExpEdgeGroup[i].second, this),
            i, 0, 1, 1, 1);
    }
    if(m_nLevel==LEVEL_ADMINISTRATOR)
    {
        QPointer<QGroupBox> hueGroupBox = new QGroupBox(tabMap[tabName]);
        hueGroupBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        hueGroupBox->setTitle("Hue Weight");
        dynamic_cast<QGridLayout *>(tabMap[tabName]->layout())->addWidget(hueGroupBox, 0, 2, 7, 1);
        hueGroupBox->setLayout(new QGridLayout());
        insertControl(dynamic_cast<QGridLayout *>(hueGroupBox->layout()),
            QCommonUiInterfaceFactory::instance().createUiInterface("QHueGraph", tabName, "hue_weight", this), 0, 0, 1,
            1);
    }
    dynamic_cast<QGridLayout *>(tabMap[tabName]->layout())->setColumnStretch(0, 2);
    dynamic_cast<QGridLayout *>(tabMap[tabName]->layout())->setColumnStretch(1, 2);
    dynamic_cast<QGridLayout *>(tabMap[tabName]->layout())->setColumnStretch(2, 3);
}

void QCacTab::initSOrLfsSubTab(QString tabName)
{
    tabMap[tabName] = new QWidget(contentWidget);
    tabMap[tabName]->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    tabMap[tabName]->setLayout(new QGridLayout());
    tabMap[tabName]->layout()->setAlignment(Qt::AlignLeft);
    subTabLayout->addWidget(tabMap[tabName]);
    tabMap[tabName]->hide();
    QPointer<QGroupBox> CACHueBox = new QGroupBox(tabMap[tabName]);
    CACHueBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    CACHueBox->setTitle("CAC_Hue");
    dynamic_cast<QGridLayout *>(tabMap[tabName]->layout())->addWidget(CACHueBox, 0, 0, 4, 1);
    CACHueBox->setLayout(new QGridLayout());
    const QVector<QPair<QString, QString>> firstColumn = {
        { "QLabelCheckBox", "CAC_Enable" },
        { "QLabelCheckBox", "Hue_Enable" },
        { "QLabelEditGroup", "Hue_Rat" },
        { "QLabelEditGroup", "CAC_Rat" },
    };
    for (int i = 0; i < firstColumn.size(); i++) {
        insertControl(dynamic_cast<QGridLayout *>(CACHueBox->layout()),
            QCommonUiInterfaceFactory::instance().createUiInterface(firstColumn[i].first, tabName,
                firstColumn[i].second, this),
            i, 0, 1, 1, 2);
    }
    const QVector<QPair<QString, QString>> overExpEdgeGroup = {
        { "QLabelEditGroup", "Over_Exposure_Thre" },
        { "QLabelEditGroup", "Over_Expo_Rat" },
    };
    QPointer<QGroupBox> overExpEdgeGroupBox = new QGroupBox(tabMap[tabName]);
    overExpEdgeGroupBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    overExpEdgeGroupBox->setTitle("Over_Exp_Edge");
    dynamic_cast<QGridLayout *>(tabMap[tabName]->layout())->addWidget(overExpEdgeGroupBox, 4, 0, 2, 1);
    overExpEdgeGroupBox->setLayout(new QGridLayout());
    for (int i = 0; i < overExpEdgeGroup.size(); i++) {
        insertControl(dynamic_cast<QGridLayout *>(overExpEdgeGroupBox->layout()),
            QCommonUiInterfaceFactory::instance().createUiInterface(overExpEdgeGroup[i].first, tabName,
                overExpEdgeGroup[i].second, this),
            i, 0, 1, 1, 2);
    }
    if(m_nLevel==LEVEL_ADMINISTRATOR)
    {
        QVector<QPair<QString, QString>> gainNodeGroup = {
            { "QLabelEditGroup", "Gain_Node[0]" },
            { "QLabelEditGroup", "Gain_Node[1]" },
            { "QLabelEditGroup", "Gain_Node[2]" },
            { "QLabelEditGroup", "Gain_Node[3]" },
            { "QLabelEditGroup", "Gain_Node[4]" },
            { "QLabelEditGroup", "Gain_Node[5]" },
            { "QLabelEditGroup", "Gain_Node[6]" },
            { "QLabelEditGroup", "Gain_Node[7]" },
            };
        QPointer<QGroupBox> gainNodeGroupBox = new QGroupBox(tabMap[tabName]);
        gainNodeGroupBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        gainNodeGroupBox->setTitle("Gain Node");
        dynamic_cast<QGridLayout *>(tabMap[tabName]->layout())->addWidget(gainNodeGroupBox, 0, 1, 6, 1);
        gainNodeGroupBox->setLayout(new QGridLayout());
        for (int i = 0; i < gainNodeGroup.size(); i++) {
            insertControl(dynamic_cast<QGridLayout *>(gainNodeGroupBox->layout()),
                QCommonUiInterfaceFactory::instance().createUiInterface(gainNodeGroup[i].first, tabName,
                    gainNodeGroup[i].second, this),
                i, 0, 1, 1, 2);
        }
        QVector<QVector<QPair<QString, QString>>> contrastConditionsGroup = {
            {
                { "QLabelEditGroup", "Contrast_Coe[0]" },
                { "QLabelEditGroup", "Contrast_Coe[1]" },
                { "QLabelEditGroup", "Contrast_Coe[2]" },
                { "QLabelEditGroup", "Contrast_Coe[3]" },
                { "QLabelEditGroup", "Contrast_Coe[4]" },
                { "QLabelEditGroup", "Contrast_Coe[5]" },
                { "QLabelEditGroup", "Contrast_Coe[6]" },
                { "QLabelEditGroup", "Contrast_Coe[7]" },
                },
            {
                { "QLabelEditGroup", "Contrast_Int[0]" },
                { "QLabelEditGroup", "Contrast_Int[1]" },
                { "QLabelEditGroup", "Contrast_Int[2]" },
                { "QLabelEditGroup", "Contrast_Int[3]" },
                { "QLabelEditGroup", "Contrast_Int[4]" },
                { "QLabelEditGroup", "Contrast_Int[5]" },
                { "QLabelEditGroup", "Contrast_Int[6]" },
                { "QLabelEditGroup", "Contrast_Int[7]" },
                },
            {
                { "QLabelEditGroup", "Contrast_Rat[0]" },
                { "QLabelEditGroup", "Contrast_Rat[1]" },
                { "QLabelEditGroup", "Contrast_Rat[2]" },
                { "QLabelEditGroup", "Contrast_Rat[3]" },
                { "QLabelEditGroup", "Contrast_Rat[4]" },
                { "QLabelEditGroup", "Contrast_Rat[5]" },
                { "QLabelEditGroup", "Contrast_Rat[6]" },
                { "QLabelEditGroup", "Contrast_Rat[7]" },
                },
            };
        QPointer<QGroupBox> contrastConditionsGroupBox = new QGroupBox(tabMap[tabName]);
        contrastConditionsGroupBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        contrastConditionsGroupBox->setTitle("Contrast Conditions");
        dynamic_cast<QGridLayout *>(tabMap[tabName]->layout())->addWidget(contrastConditionsGroupBox, 0, 2, 6, 1);
        contrastConditionsGroupBox->setLayout(new QGridLayout());
        for (int i = 0; i < contrastConditionsGroup.size(); i++) {
            for (int j = 0; j < contrastConditionsGroup[i].size(); j++) {
                insertControl(dynamic_cast<QGridLayout *>(contrastConditionsGroupBox->layout()),
                    QCommonUiInterfaceFactory::instance().createUiInterface(contrastConditionsGroup[i][j].first, tabName,
                        contrastConditionsGroup[i][j].second, this),
                    j, i * 2, 1, 1, 3);
            }
        }
        for (auto i : { 1, 3 }) {
            QPointer<QSplitter> splitter = new QSplitter(Qt::Vertical, tabMap[tabName]);
            splitter->setFrameShape(QFrame::WinPanel);
            splitter->setFixedWidth(1);
            splitter->setLineWidth(1);
            dynamic_cast<QGridLayout *>(contrastConditionsGroupBox->layout())->addWidget(splitter, 0, i, 8, 1);
        }
        dynamic_cast<QGridLayout *>(tabMap[tabName]->layout())->setColumnStretch(0, 2);
        dynamic_cast<QGridLayout *>(tabMap[tabName]->layout())->setColumnStretch(1, 5);
    }
};

void QCacTab::insertControl(QPointer<QGridLayout> layout, QPointer<QCommonUiInterface> widget, int row, int column,
    int rowSpan, int columnSpan)
{
    connect(widget, &QCommonUiInterface::releaseUiDataMiddleware, this,
        &QTabWndComm::recvUiDataMiddlewareUpdateRegisterTable, Qt::QueuedConnection);
    connect(this, &QTabWndComm::readSignal, widget, &QCommonUiInterface::read, Qt::QueuedConnection);
    // auto combineList = widget->getUiDataMiddleware()->getCombinedRegisterList();
    // // std::vector<CombinedRegister> combinevec;
    // // combinedRegisterMap.insert(widget->getUiDataMiddleware()->getUiControlName(), combinevec.toList());
    // for (auto reg : combineList)
    // {
    //     setRegisterSubPage(reg, moduleName.c_str());
    //     // combinevec.push_back(reg);
    // }
    // combinedRegisterMap.insert(widget->getUiDataMiddleware()->getUiControlName(), combinevec);
    layout->addWidget(widget, row, column, rowSpan, columnSpan);
    widget->setMouseTracking(true);
    widget->setValue(0);
    if (dynamic_cast<QLabelEditGroup *>(widget.data())) {
        dynamic_cast<QLabelEditGroup *>(widget.data())->setEditable(false);
    }
}

void QCacTab::insertControl(QPointer<QGridLayout> layout, QPointer<QCommonUiInterface> widget, int row, int column,
    int rowSpan, int columnSpan, int topDownMargin)
{
    insertControl(layout, widget, row, column, rowSpan, columnSpan);
    widget->layout()->setContentsMargins(0, 0, topDownMargin, topDownMargin);
}
