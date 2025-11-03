#include "qgbalance.h"
#include <QComboBox>
#include <QGroupBox>

const QString                                              TAB_NAME     = "GBalance";
const QString                                              TAB_NAME_S   = "GBalance-S";
const QString                                              TAB_NAME_LFS = "GBalance-LFS";
const QMap<QString, QList<QList<QPair<QString, QString>>>> CONTROL_MAP  = {
    { "GBalance-S",
         { { { "QLabelEditGroup", "GBalCoe[0]" }, { "QLabelEditGroup", "GBalCoe[1]" },
               { "QLabelEditGroup", "GBalCoe[2]" }, { "QLabelEditGroup", "GBalCoe[3]" },
               { "QLabelEditGroup", "GBalCoe[4]" }, { "QLabelEditGroup", "GBalCoe[5]" },
               { "QLabelEditGroup", "GBalCoe[6]" }, { "QLabelEditGroup", "GBalCoe[7]" } },
             { { "QLabelEditGroup", "GBalInt[0]" }, { "QLabelEditGroup", "GBalInt[1]" },
                 { "QLabelEditGroup", "GBalInt[2]" }, { "QLabelEditGroup", "GBalInt[3]" },
                 { "QLabelEditGroup", "GBalInt[4]" }, { "QLabelEditGroup", "GBalInt[5]" },
                 { "QLabelEditGroup", "GBalInt[6]" }, { "QLabelEditGroup", "GBalInt[7]" } },
             { { "QLabelEditGroup", "GainNode[0]" }, { "QLabelEditGroup", "GainNode[1]" },
                 { "QLabelEditGroup", "GainNode[2]" }, { "QLabelEditGroup", "GainNode[3]" },
                 { "QLabelEditGroup", "GainNode[4]" }, { "QLabelEditGroup", "GainNode[5]" },
                 { "QLabelEditGroup", "GainNode[6]" }, { "QLabelEditGroup", "GainNode[7]" } } } },
    { "GBalance-LFS",
         { { { "QLabelEditGroup", "GBalCoe[0]" }, { "QLabelEditGroup", "GBalCoe[1]" },
               { "QLabelEditGroup", "GBalCoe[2]" }, { "QLabelEditGroup", "GBalCoe[3]" },
               { "QLabelEditGroup", "GBalCoe[4]" }, { "QLabelEditGroup", "GBalCoe[5]" },
               { "QLabelEditGroup", "GBalCoe[6]" }, { "QLabelEditGroup", "GBalCoe[7]" } },
             { { "QLabelEditGroup", "GBalInt[0]" }, { "QLabelEditGroup", "GBalInt[1]" },
                 { "QLabelEditGroup", "GBalInt[2]" }, { "QLabelEditGroup", "GBalInt[3]" },
                 { "QLabelEditGroup", "GBalInt[4]" }, { "QLabelEditGroup", "GBalInt[5]" },
                 { "QLabelEditGroup", "GBalInt[6]" }, { "QLabelEditGroup", "GBalInt[7]" } },
             { { "QLabelEditGroup", "GainNode[0]" }, { "QLabelEditGroup", "GainNode[1]" },
                 { "QLabelEditGroup", "GainNode[2]" }, { "QLabelEditGroup", "GainNode[3]" },
                 { "QLabelEditGroup", "GainNode[4]" }, { "QLabelEditGroup", "GainNode[5]" },
                 { "QLabelEditGroup", "GainNode[6]" }, { "QLabelEditGroup", "GainNode[7]" } } } }
};

enum GBALANCE_TYPE {
    GBALANCE_S = 0,
    GBALANCE_LFS,
    GBALANCE_TYPE_MAX,
};

QGBalance::QGBalance(QWidget *parent)
    : QTabWndComm(parent)
    , tabWidgetS(new QWidget(this))
    , tabWidgetLFS(new QWidget(this))
{
    moduleName = TAB_NAME.toStdString();
    init();
    initConnect();
}

QGBalance::~QGBalance() { }

void QGBalance::init()
{
    subPageMap.insert(0, "GBalance");
    addRegisterTable();
    comboCurrentType = new QComboBox();
    comboCurrentType->insertItem(GBALANCE_S, "S");
    comboCurrentType->insertItem(GBALANCE_LFS, "LFS");
    comboCurrentType->setCurrentIndex(GBALANCE_S);
    comboCurrentType->setFixedWidth(100);
    titleLayout->addWidget(comboCurrentType.get());
    int left, right, top, bottom;
    titleLayout->getContentsMargins(&left, &top, &right, &bottom);
    titleLayout->setContentsMargins(10, top, right, bottom);

    contentWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    contentLayout->addWidget(tabWidgetS.get());
    contentLayout->addWidget(tabWidgetLFS.get());
    generateSubTab(tabWidgetS, TAB_NAME_S);
    generateSubTab(tabWidgetLFS, TAB_NAME_LFS);
    tabWidgetS->setVisible(true);
    tabWidgetLFS->setVisible(false);
    getAllRegInfoInit();
}

void QGBalance::initConnect()
{
    connect(comboCurrentType.get(), SIGNAL(currentIndexChanged(int)), this, SLOT(comboCurrentTypeValueChanged(int)));
    connect(this, &QTabWndComm::updateValue, this, &QTabWndComm::read, Qt::QueuedConnection);
    connect(
        this, &QTabWndComm::reConnectRead, this,
        [=]() {
            getAllRegInfoInit();
            read();
        },
        Qt::QueuedConnection);
}

void QGBalance::comboCurrentTypeValueChanged(int index)
{
    read();
    if (index == GBALANCE_S) {
        tabWidgetS->setVisible(true);
        tabWidgetLFS->setVisible(false);
    } else {
        tabWidgetS->setVisible(false);
        tabWidgetLFS->setVisible(true);
    }
}

void QGBalance::insertControl(QPointer<QGridLayout> layout, QPointer<QCommonUiInterface> widget, int row, int column,
    int rowSpan, int columnSpan)
{
    connect(widget, &QCommonUiInterface::releaseUiDataMiddleware, this,
        &QTabWndComm::recvUiDataMiddlewareUpdateRegisterTable, Qt::QueuedConnection);
    connect(this, &QTabWndComm::readSignal, widget, &QCommonUiInterface::read, Qt::QueuedConnection);
    layout->addWidget(widget, row, column, rowSpan, columnSpan);
    widget->setMouseTracking(true);
    widget->setValue(0);
}

void QGBalance::generateSubTab(QPointer<QWidget> subTab, QString tabName)
{
    QPointer<QGridLayout> layout = new QGridLayout(subTab);
    layout->setContentsMargins(20, 10, 20, 10);
    layout->setHorizontalSpacing(50);
    subTab->setLayout(layout);
    for (int i = 0; i < CONTROL_MAP[tabName].size(); i++) {
        for (int j = 0; j < CONTROL_MAP[tabName][i].size(); j++) {
            QPointer<QCommonUiInterface> widget = QCommonUiInterfaceFactory::instance().createUiInterface(
                CONTROL_MAP[tabName][i][j].first, tabName, CONTROL_MAP[tabName][i][j].second, this);
            if (dynamic_cast<QLabelEditGroup *>(widget.data())) {
                dynamic_cast<QLabelEditGroup *>(widget.data())->setEditable(false);
            }
            if (widget) {
                insertControl(layout, widget, j, i);
            }
        }
        layout->setColumnStretch(i, 1);
    }
}

void QGBalance::getAllRegInfoInit()
{

    setAddrModule.clear();
    registerMoudle.clear();
    for (int type = 0; type < GBALANCE_TYPE_MAX; type++) {
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
        registerSubPageMap[iter].emplace_back("GBalance");
    }
}
