#include "qmainwnd.h"
#include "./ui_qmainwnd.h"
#include "Common/Others/qdefaultinfostorage.h"
#include "Common/UI/qtabwndcommfactory.h"
#include <Common/Others/qcameratype.h>
#include <Common/UI/qconfigdialog.h>
#include <Common/UI/qdialogwidget.h>
#include <QColorDialog>
#include <QMenu>
#include <QtConcurrent/QtConcurrent>
#include <windows.h>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

int qRegister::gSensorID=0x46534D31;
std::string qRegister::m_strKey="";
BOOL bDemoSensUDP = FALSE;
DWORD processId=0;
static const QStringList calibrationToolsList = {"FPNC ", "AWB ", "CCM ", "DCPC ", "LSC ", "DeIR "};

QMainWnd::QMainWnd(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::QMainWnd)
    , usbConnected(false)
    , m_nLevel(0)
{
    L_TRACE_FUNC;
    ui->setupUi(this);
    QStringList args = QCoreApplication::arguments();
    if (args.contains("--enable-udp")) {
        confirmUDP();
    }
    setWindowTitle("M1 ISP TuningTool"); // 设置窗口名称

    processId = GetCurrentProcessId(); // 获取当前进程 ID
    tabComm        = std::make_shared<QTabWidget>();
    treeModel      = std::make_shared<QStandardItemModel>();
    toolBar        = new QToolBar(this);
    dialog         = new QConfigDialog(this);
    interfaceDebug = new QInterfaceDebug();
    VCSetingTool = new QVCSeting();

    
    // dlgProgress.close();s
}

QMainWnd::~QMainWnd()
{
    L_TRACE_FUNC;
    delete ui;
    if (tabComm) {
        tabComm.reset();
        tabComm = nullptr;
    }
    if (treeModel) {
        treeModel.reset();
        treeModel = nullptr;
    }
    if (toolBar) {
        toolBar->clear();
        delete toolBar;
        toolBar = nullptr;
    }
    if (dialog) {
        dialog->close();
        delete dialog;
        dialog = nullptr;
    }
    if (interfaceDebug) {
        interfaceDebug->close();
        delete interfaceDebug;
        interfaceDebug = nullptr;
    }
    if(VCSetingTool){
        VCSetingTool->close();
        delete VCSetingTool;
        VCSetingTool=nullptr;
    }

    if(m_pRegister)
    {
        m_pRegister->close();
        delete m_pRegister;
        m_pRegister = nullptr;
    }

    QFlashImgBurn::instance()->close();
}

void QMainWnd::initWnd()
{
    L_TRACE_FUNC;
    //初始化用户权限
    initUserAuthority();
    // 加载UI样式
    loadToolBar();
    // 获取ORM数据库接口
    QString dbPath = QCoreApplication::applicationDirPath() + "\\M1.sqlite";

    DatabaseOrm::initDatabase(dbPath.toStdString()/*"M1.sqlite"*/);
    ui_modules.clear();
    ui_modules = DatabaseOrm::getAllUiModules();
    initAWBWriteFunc();
    initAWBGetRegisterFunc();
    initAWBGetRegisterValueFunc();

    modules.clear();
    modules = DatabaseOrm::getAllModules();

    if (ui_modules.size() > 0) {
        // 插入初始tab页面
        ui->tabLayout->addWidget(tabComm.get());
        tabComm->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        tabComm->setTabsClosable(true);
        tabComm->setMovable(true);
        // 构建ISP树
        treeModel->setHorizontalHeaderLabels(QStringList() << QStringLiteral("ISP"));
        QStandardItem *registerItem = new QStandardItem("Register");
        treeItemMap.insert("Register", registerItem);
        registerItem->setEditable(false);
        registerItem->setCheckable(true);
        registerItem->setCheckState(Qt::Checked);
        treeModel->appendRow(registerItem); // 添加树第一行：Register
        auto res = buildTree(ui_modules);   // 添加树第二行ISP及其下拉列表
        if (res) treeModel->appendRow(res);
        treeModel->appendRow(buildCalibrationTree());
        QStandardItem *gpioItem = new QStandardItem("GPIO");
        treeItemMap.insert("GPIO", gpioItem);
        gpioItem->setEditable(false);
        gpioItem->setCheckable(true);
        gpioItem->setCheckState(Qt::Checked);
        treeModel->appendRow(gpioItem);// 添加树第三行：GPIO

        ui->treeView->setModel(treeModel.get());
        ui->treeView->expandAll();
        // 初始化关联
        initConnect();
        // 设置样式
        QFont font;
        font.setBold(true);
        font.setPointSize(14);
        ui->treeView->header()->setFont(font);
        ui->treeView->header()->setSectionResizeMode(0, QHeaderView::ResizeMode::ResizeToContents);
        ui->treeView->header()->setDefaultAlignment(Qt::AlignHCenter);
        ui->treeView->setStyleSheet(
            "QTreeView{border:2px groove black;border-radius:5px;padding:1px 1px;font-size:16px;}"
            "QHeaderView::section{border:0px solid rgb(255,255,255);};");
        tabComm->setStyleSheet("QTabBar::tab{font-size:14px;padding:5px;}"
                               "QTabBar::tab::selected{font:bold;}"
                               "QTabWidget::pane{ border:2px solid black;border-radius:10px;padding:2px 2px;}");
    }
}

void QMainWnd::initConnect()
{
    L_TRACE_FUNC;
    connect(tabComm.get(), &QTabWidget::tabCloseRequested, this, &QMainWnd::closeCurrentTab); // 关联主窗口tab关闭信号
    connect(tabComm.get(), &QTabWidget::currentChanged, this, &QMainWnd::setCurrentTreeItem); // 关联tab选中高亮信号
    connect(treeModel.get(), &QStandardItemModel::itemChanged, this, &QMainWnd::on_treeItem_itemChanged);
    connect(CameraType::instance(), &CameraType::updateCamera, this,
        [=]() { updateTabCamera(tabComm->currentIndex()); });
    connect(dialog, &QConfigDialog::updateStyle, this, [=](QString str) { setStyleSheet(str); });
    
    connect(interfaceDebug, &QInterfaceDebug::emit_LoadTypeChange, this, [=](int iType){
        QTabWndComm::m_saveType = iType;
    });
    // connect(this, &QMainWnd::saveAllDone, this, [&]()
    // {
    //     QMessageBox::information(this, "Info", "SaveAll run success. ");
    // });
}

void QMainWnd::on_treeItem_itemChanged(QStandardItem *item)
{
    disconnect(treeModel.get(), &QStandardItemModel::itemChanged, this, &QMainWnd::on_treeItem_itemChanged);
    if (item == NULL || !item->isCheckable())
        return;

    if (item->hasChildren()) {
        setChildTreeItemStatus(item);
    }
    QStandardItem *parentItem = item->parent();
    if (parentItem && parentItem->isCheckable()) {
        setParentTreeItemStatus(parentItem);
    }
    connect(treeModel.get(), &QStandardItemModel::itemChanged, this, &QMainWnd::on_treeItem_itemChanged);
}

void QMainWnd::setChildTreeItemStatus(QStandardItem *item)
{
    if (item == NULL || !item->isCheckable())
        return;

    int            state     = item->checkState();
    Qt::CheckState itemState = (state == Qt::Unchecked ? Qt::Unchecked : Qt::Checked);
    for (int i = 0; i < item->rowCount(); i++) {
        QStandardItem *childItem = item->child(i);
        if (childItem->isCheckable()) {
            childItem->setCheckState(itemState);
            if (childItem->hasChildren()) {
                setChildTreeItemStatus(childItem);
            }
        }
    }
    if (state == Qt::PartiallyChecked)
        item->setCheckState(Qt::Checked);
}

void QMainWnd::setParentTreeItemStatus(QStandardItem *item)
{
    if (item == NULL || !item->isCheckable())
        return;

    int checkedCount = 0, unCheckedCount = 0;
    int len = item->rowCount();
    for (int i = 0; i < len; i++) {
        QStandardItem *childItem = item->child(i);
        if (childItem->checkState() == Qt::Checked)
            checkedCount++;
        else if (childItem->checkState() == Qt::Unchecked)
            unCheckedCount++;
    }
    if (checkedCount == len)
        item->setCheckState(Qt::Checked);
    else if (unCheckedCount == len)
        item->setCheckState(Qt::Unchecked);
    else
        item->setCheckState(Qt::PartiallyChecked);

    QStandardItem *parentItem = item->parent();
    if (parentItem && parentItem->isCheckable()) {
        setParentTreeItemStatus(parentItem);
    }
}

QStandardItem *QMainWnd::buildCalibrationTree()
{
    QEventLoop     loop;
    QStandardItem *root = new QStandardItem(QString("Calibration"));
    root->setCheckable(true);
    root->setCheckState(Qt::Checked);
    root->setEditable(false);
    for (auto &it : calibrationToolsList) {
        loop.processEvents();
        QStandardItem *CalibrationItem = new QStandardItem(it);
        qDebug() << "=====CalibrationItem :" << it;
        treeItemMap.insert(it, CalibrationItem);
        CalibrationItem->setCheckable(true);
        CalibrationItem->setCheckState(Qt::Checked);
        CalibrationItem->setEditable(false);
        CalibrationItem->setData(it, Qt::UserRole); // 储存ISP模块地址
        // ISPModuleItem->setIcon(QIcon("C:\\Users\\admin\\Desktop\\radioDot.png"));//示例
        root->appendRow(CalibrationItem);
    }
    return root;
}

QStandardItem *QMainWnd::buildTree(std::vector<UiModule> modules)
{
    L_TRACE_FUNC;
    if (!modules.empty()) {
        
        QEventLoop     loop;
        QStandardItem *root = new QStandardItem(QString("ISP"));
        root->setCheckable(true);
        root->setCheckState(Qt::Checked);
        root->setEditable(false);
        size_t len = modules.size();
        for (size_t i = 0; i < len; i++) {
            if(m_nLevel!=LEVEL_ADMINISTRATOR)
            {
                if(modules.at(i).moduleName=="DPC"||modules.at(i).moduleName=="PRECCM"||modules.at(i).moduleName=="GBalance")
                {
                    continue;
                }
            }
            UiModule model = modules.at(i);
            loop.processEvents();
            QStandardItem *ISPModuleItem = new QStandardItem(QString::fromStdString(model.moduleName));
            treeItemMap.insert(QString::fromStdString(model.moduleName), ISPModuleItem);
            ISPModuleItem->setCheckable(true);
            ISPModuleItem->setCheckState(Qt::Checked);
            ISPModuleItem->setEditable(false);
            // if (skipModule)
            // {
            //     if(modules.at(i).moduleName=="Combine"||modules.at(i).moduleName=="TM"||modules.at(i).moduleName=="PRECCM"||modules.at(i).moduleName=="GBalance")
            //     {
            //         ISPModuleItem->setEnabled(false);
            //     }
            // }
            ISPModuleItem->setData(QString::fromStdString(model.moduleName), Qt::UserRole); // 储存ISP模块地址
            // ISPModuleItem->setIcon(QIcon("C:\\Users\\admin\\Desktop\\radioDot.png"));//示例
            root->appendRow(ISPModuleItem);
        }
        return root;
    }
    return NULL;
}

void QMainWnd::updateTabCamera(int index)
{
    if (tabWndMap.contains(tabComm->tabText(index))) {
        QPointer<QTabWndComm> ctabWnd = tabWndMap.value(tabComm->tabText(index));
        ctabWnd->updateTabCamera();
        ctabWnd->reConnectCamera(); // 更新cameraid
    }
}

void QMainWnd::loadToolBar()
{
    this->addToolBar(Qt::TopToolBarArea, toolBar);
    toolBar->addSeparator();
    QPointer<QAction> action_Config = new QAction("Config");
    QPointer<QMenu>   itemMenu      = new QMenu();
    action_Config->setMenu(itemMenu);
    connect(action_Config, &QAction::triggered, this, [=]() {
        itemMenu->move(QCursor::pos() + QPoint(-10, 10));
        itemMenu->showNormal();
    });
    QPointer<QAction> list0 = new QAction(QString::fromUtf8("Load Style"), itemMenu);
    connect(list0, &QAction::triggered, this, [=]() { dialog->show(); });
    QPointer<QAction> list1 = new QAction(QString::fromUtf8("Debug"), itemMenu);
    connect(list1, &QAction::triggered, this, [=]() {
        interfaceDebug->close();
        interfaceDebug->show();
    });
    QPointer<QAction> list2 = new QAction(QString::fromUtf8("Flash Burn"), itemMenu);
    connect(list2, &QAction::triggered, this, [=]() {
        QFlashImgBurn::instance()->close();
        QFlashImgBurn::instance()->show();
    });
    QPointer<QAction> list3 = new QAction(QString::fromUtf8("AWBCalibration"), itemMenu);
    connect(list3, &QAction::triggered, this, &AWB_CreateWindow);

    QPointer<QAction> list4 = new QAction(QString::fromUtf8("VC Setting"), itemMenu);
    connect(list4, &QAction::triggered, this, [=](){
        VCSetingTool->close();
        VCSetingTool->show();
    });

    QPointer<QAction> list5 = new QAction(QString::fromUtf8("Register"), itemMenu);
    connect(list5, &QAction::triggered, this, [=](){
        if(m_pRegister == nullptr)
        {
            m_pRegister = new qRegister(&m_nLevel);
            connect(m_pRegister, &qRegister::updateLevel, this, &QMainWnd::updateNewLevel);
        }
        m_pRegister->close();
        m_pRegister->show();
    });

    QPointer<QAction> list6 = new QAction(QString::fromUtf8("SaveAll"), itemMenu);
    connect(list6, &QAction::triggered, this, [=](){
        saveAll();
    });

    itemMenu->addAction(list0);
    itemMenu->addAction(list1);
    itemMenu->addAction(list2);
    itemMenu->addAction(list3);
    itemMenu->addAction(list5);
    itemMenu->addAction(list6);
    toolBar->addAction(action_Config);
    this->setStyleSheet(dialog->initStyle());
}

void QMainWnd::initAWBWriteFunc()
{
    AWB_SetWriteFunc([](uint32_t awbRegisterCode, uint32_t cameraId,uint32_t writeData)->int{
        //获取寄存器名字两个段
        std::vector<AWBCalibrationRegister> vecAWBCalibration;
        vecAWBCalibration=getAWBRegisterModuleCombine(awbRegisterCode);
        if(vecAWBCalibration.size()<1)
        {
            return -1;
        }
        //获取combineId
        std::vector<CombinedRegister> vecCombinedRegister;
        vecCombinedRegister = getAWBRegisterCombinedId(vecAWBCalibration[0].moduleName,vecAWBCalibration[0].combinedName, cameraId);
        if(vecCombinedRegister.size()<1)
        {
            return -1;
        }
        //获取寄存器具体地址
        std::vector<RegisterBitInfo> vecRegisterInfo;
        vecRegisterInfo=getAWBRegisterAddress(vecCombinedRegister[0].combinedId);
        if(vecRegisterInfo.size()<1)
        {
            return -1;
        }
        for (int var = 0; var < vecRegisterInfo.size(); ++var) {
            uint8_t value=(writeData>>(var*8))&vecRegisterInfo[var].bitMask;
            QUsbCommunication::getInstance().writeBurst(vecRegisterInfo[var].registerAddr, &value, 4, 1);
        }
        return 0;});
}

void QMainWnd::initAWBGetRegisterFunc()
{
    AWB_SetRegisterGetFunc([](uint32_t awbRegisterCode, uint32_t cameraId, uint32_t* registe,
        uint32_t* mask,uint8_t* registeNum)->int{
        //获取寄存器名字两个段
        std::vector<AWBCalibrationRegister> vecAWBCalibration;
        vecAWBCalibration=getAWBRegisterModuleCombine(awbRegisterCode);
        if(vecAWBCalibration.size()<1)
        {
            return -1;
        }
        //获取combineId
        std::vector<CombinedRegister> vecCombinedRegister;
        vecCombinedRegister = getAWBRegisterCombinedId(vecAWBCalibration[0].moduleName,vecAWBCalibration[0].combinedName, cameraId);
        if(vecCombinedRegister.size()<1)
        {
            return -1;
        }
        //获取寄存器具体地址
        std::vector<RegisterBitInfo> vecRegisterInfo;
        vecRegisterInfo=getAWBRegisterAddress(vecCombinedRegister[0].combinedId);
        *registeNum=(uint8_t)vecRegisterInfo.size();
        for (int var = 0; var < vecRegisterInfo.size(); ++var) {
            registe[var]=vecRegisterInfo[var].registerAddr;
            mask[var]=vecRegisterInfo[var].bitMask;
        }
        return 0;
    });
}

void QMainWnd::initAWBGetRegisterValueFunc()
{
    AWB_SetRegisterValueGetFunc([](uint32_t address, uint8_t* pBuf, uint32_t addrLength, uint32_t valueLength)->int{
        return QUsbCommunication::getInstance().readBurst(address, pBuf,addrLength,valueLength);
    });
}

void QMainWnd::updateNewLevel()
{
    QTabWndCommFactory::instance().QTabWndUpdateUserLevel(m_nLevel);
}

void QMainWnd::initUserAuthority()
{
    char cValue[2]={0};
    QUsbCommunication::getInstance().readBurst(0x31003050, (unsigned char*)(&cValue[0]));
    QUsbCommunication::getInstance().readBurst(0x31003051, (unsigned char*)(&cValue[1]));
    if(cValue[0]!=0&&cValue[1]!=0)
    {
        qRegister::vSetSensorID(((cValue[0]&0x7F)<<7)+cValue[1]&0x7F);
    }
    if (qRegister::bIsAuthorized())//注册过
    {
        std::string strLevelEnable("");
        m_nLevel=qRegister::iGetUserLevel(strLevelEnable);
    }
    m_nLevel=LEVEL_ADMINISTRATOR;
    updateNewLevel();
}



void QMainWnd::confirmUDP()
{
    QJsonObject requestJson;
    if(ui->ConnectDemoBtn->text()=="Disconnect Demo"){
        bDemoSensUDP=false;
        ui->ConnectDemoBtn->setText("Connect Demo");
        ui->comboDevice->clear();
        ui->comboDevice->setEnabled(true);
        ui->bthRefreshHardware->setEnabled(true);
        ui->btnOpenDevice->setEnabled(true);
        size_t len = ui_modules.size();
        for (size_t i = 0; i < len; i++) {
            // QStandardItem *ISPModuleItem = new QStandardItem(QString::fromStdString(ui_modules.at(i).moduleName));

            QStandardItem *item = treeItemMap.value(QString::fromStdString(ui_modules.at(i).moduleName));
            if (item) {
                item->setEnabled(true);
            }
        }
    }else{
        requestJson["command"] = "ISPConnect"; // 指令代码
        requestJson["processID"] = static_cast<int>(processId); // 读写的数据量

        QJsonObject responseJson;
        QISPClient::getInstance().sendMessage(requestJson,responseJson);
        if(responseJson.contains("result_data")){
            if(responseJson.value("result_data").toString()=="Connection Established"){
                // QMessageBox msgBox(this);
                // QString imagePath = QCoreApplication::applicationDirPath() + "\\FlyingChip.png";
                // msgBox.setText("Connect DemoSens Success!");
                // msgBox.setIcon(QMessageBox::NoIcon);
                // msgBox.addButton(QMessageBox::Ok);
                // QPixmap pixmap(imagePath);
                // if (!pixmap.isNull()){
                //     QLabel* iconLabel = new QLabel(&msgBox);
                //     iconLabel->setPixmap(pixmap.scaled(32, 32, Qt::KeepAspectRatio, Qt::SmoothTransformation));
                // }
                //msgBox.exec();
                bDemoSensUDP=true;
                ui->ConnectDemoBtn->setText("Disconnect Demo");
                ui->comboDevice->clear();
                ui->comboDevice->addItem(QString("Connecting to SmartSensDemo."));
                ui->comboDevice->setCurrentIndex(0);
                ui->comboDevice->setEnabled(false);
                ui->bthRefreshHardware->setEnabled(false);
                ui->btnOpenDevice->setEnabled(false);

                // std::this_thread::sleep_for(std::chrono::seconds(1));
                // uint8_t value802f = 0;
                // QUsbCommunication::getInstance().readI2cBurst(0x40, 0x80f2, &value802f, 2, VALUE_LENGTH);
                // value802f &= 0xfe;
                // QUsbCommunication::getInstance().writeI2cBurst(0x40, 0x80f2, &value802f, 2, VALUE_LENGTH);
                // uint8_t valueA = 0, valueB = 0;
                // QUsbCommunication::getInstance().readI2cBurst(0x30, 0x3107, &valueA, 2, VALUE_LENGTH, 0, 1, 400, 1);
                // QUsbCommunication::getInstance().readI2cBurst(0x30, 0x3108, &valueB, 2, VALUE_LENGTH, 0, 1, 400, 1);
                // value802f |= 0x01;
                // QUsbCommunication::getInstance().writeI2cBurst(0x40, 0x80f2, &value802f, 2, VALUE_LENGTH);
                // bool skipModule = false;
                // qDebug() << "valueA: " << QString("0x%1").arg(valueA, 0, 16).toUpper();
                // qDebug() << "valueB: " << QString("0x%1").arg(valueB, 0, 16).toUpper();
                // if (valueA == 0xCB && valueB == 0x61 || valueA == 0xCE && valueB == 0x6F)
                // {
                //     skipModule = true;
                // }
                // // skipModule = true;
                // size_t len = ui_modules.size();
                // for (size_t i = 0; i < len; i++) {
                //     // QStandardItem *ISPModuleItem = new QStandardItem(QString::fromStdString(ui_modules.at(i).moduleName));
                //     if (skipModule)
                //     {
                //         if(ui_modules.at(i).moduleName=="Combine"||ui_modules.at(i).moduleName=="TM"||ui_modules.at(i).moduleName=="PRECCM"||ui_modules.at(i).moduleName=="GBalance")
                //         {
                //             QStandardItem *item = treeItemMap.value(QString::fromStdString(ui_modules.at(i).moduleName));
                //             if (item) {
                //                 item->setEnabled(false);
                //             }
                //         }
                //     }
                // }

                return;
            }
        }
        bDemoSensUDP=false;
        QMessageBox::warning(this, "", "Connect Fail!", QMessageBox::Ok);
    }
}

void QMainWnd::closeCurrentTab(int index)
{
    L_TRACE_FUNC;
    tabCommOpenedMap.remove(tabComm->tabText(index));
    tabWndMap.remove(tabComm->tabText(index));
    tabComm->removeTab(index);
    // delete tabComm->widget(index);
}

void QMainWnd::on_treeView_doubleClicked(const QModelIndex &index)
{
    L_TRACE_FUNC;
    QStandardItem *currentItem = treeModel->itemFromIndex(index); // 获取ISP树点击位置的ISP模块及属性
    if (!currentItem->hasChildren()) {                            // 为叶子节点
        if (!tabCommOpenedMap.contains(currentItem->text())) {    // 判断是否打开
            QPointer<QDialogWidget> waitDialog = new QDialogWidget(this);
            waitDialog->setTipName(currentItem->text() + " module" + " is loading...");
            waitDialog->show();
            tabComm->setVisible(false);
            QCoreApplication::processEvents();
            if (currentItem->text() == "Register") {
                qregistertabwind *moduleTab = new qregistertabwind(this);
                moduleTab->setCheckState(currentItem->checkState());
                if (!modules.empty())
                    moduleTab->recordAllModules(modules);
                tabCommOpenedMap.insert(currentItem->text(), moduleTab);
                tabComm->insertTab(tabComm->currentIndex(), moduleTab, currentItem->text()); // 主界面打开tab页面
            }else if(currentItem->text() == "GPIO"){
                QGPIOSet *moduleTab = new QGPIOSet(this);
                tabCommOpenedMap.insert(currentItem->text(), moduleTab);
                tabComm->insertTab(tabComm->currentIndex(), moduleTab, currentItem->text()); // 主界面打开tab页面
            } else if(currentItem->text() == "FPNC "){
                QFpnc *moduleTab = new QFpnc(this);
                tabCommOpenedMap.insert(currentItem->text(), moduleTab);
                tabComm->insertTab(tabComm->currentIndex(), moduleTab, currentItem->text()); // 主界面打开tab页面
            } else if(currentItem->text() == "DeIR "){
                Deir *moduleTab = new Deir(this);
                tabCommOpenedMap.insert(currentItem->text(), moduleTab);
                tabComm->insertTab(tabComm->currentIndex(), moduleTab, currentItem->text()); // 主界面打开tab页面 
            } else {
                QPointer<QTabWndComm> moduleTab =
                    QTabWndCommFactory::instance().createTabWndComm(currentItem->text(), this);
                moduleTab->updateTabCamera();
                tabCommOpenedMap.insert(currentItem->text(), moduleTab);
                tabWndMap.insert(currentItem->text(), moduleTab);
                tabComm->insertTab(tabComm->currentIndex(), moduleTab, currentItem->text()); // 主界面打开tab页面
                if (currentItem->checkState() == Qt::Checked)
                    moduleTab->read();
            }
            tabComm->setVisible(true);
            waitDialog->setVisible(false);
        }
        tabComm->setCurrentWidget(tabCommOpenedMap.value(currentItem->text()));
        ui->treeView->setCurrentIndex(index);
    }
}

void QMainWnd::setCurrentTreeItem(int index)
{
    if (tabWndMap.contains(tabComm->tabText(index))) {
        tabWndMap.value(tabComm->tabText(index))->updatePainter();
    }
    L_TRACE_FUNC;
    if (index > 0) {
        ui->treeView->setCurrentIndex(treeItemMap.value(tabComm->tabText(index))->index());
    } else if (index == 0) {
        ui->treeView->setCurrentIndex(treeItemMap.value(tabComm->tabText(index))->index());
    }
    if (tabWndMap.contains(tabComm->tabText(index))) {
        QPointer<QTabWndComm> ctabWnd = tabWndMap.value(tabComm->tabText(index));
        if (ctabWnd->getTabCamera() != CameraType::instance()->getCurrentCamera()) {
            ctabWnd->updateTabCamera();
            ctabWnd->reConnectCamera(); // 更新cameraid
        }
    }
}

void QMainWnd::on_bthRefreshHardware_clicked()
{
    L_TRACE_FUNC;
    ui->comboDevice->clear();
    QUsbCommunication::getInstance().enumDevice(deviceNameList, deviceAddrList, deviceCount);
    for (int i = 0; i < deviceCount; i++) {
        QString name = deviceNameList[i];
        ui->comboDevice->addItem(name);
    }
    if (ui->comboDevice->count() > 0)
        ui->comboDevice->setCurrentIndex(0);
}

void QMainWnd::on_btnOpenDevice_clicked()
{
    L_TRACE_FUNC;
    if (bDemoSensUDP)
        return;
    if (ui->comboDevice->count() < 1)
        return;
    bDemoSensUDP=false;
    ui->ConnectDemoBtn->setText("Connect Demo");
    if (!usbConnected) { // openDevice
        int index = ui->comboDevice->currentIndex();
        QUsbCommunication::getInstance().openDevice(deviceAddrList[index]);
        if (0 == QUsbCommunication::getInstance().checkState()) {
            usbConnected = true;
            ui->comboDevice->setDisabled(true);
            ui->bthRefreshHardware->setDisabled(true);
            ui->btnOpenDevice->setText("Close Device");
            //testReadWrite();
            // todo: uncomment this line after m1 is ok
            //  auto future = QtConcurrent::run([=]() { QDefaultInfoStorage::getInstance().gammmaInfoInit(); });
        } else {
            QUsbCommunication::getInstance().closeDevice();
            ui->btnOpenDevice->setText("Open Device");
        }
    } else { // closeDevice
        QUsbCommunication::getInstance().closeDevice();
        ui->comboDevice->setDisabled(false);
        ui->bthRefreshHardware->setDisabled(false);
        ui->btnOpenDevice->setText("Open Device");
        usbConnected = false;
    }  
}

void QMainWnd::loadStyleConfig() { }

void QMainWnd::testReadWrite()
{
    L_TRACE_FUNC;

    int test = getBitValue(1, 60);
    L_INFO("60,1:{}", test);

    unsigned int  addres = 0x31006E3A;
    unsigned char value  = 0;
    int           ret4   = QUsbCommunication::getInstance().readBurst(addres, &value, ADDRESS_LENGTH, VALUE_LENGTH);
    L_INFO("read addres:{},value:{}", addres, value);
    value    = 225;
    int ret6 = QUsbCommunication::getInstance().writeBurst(addres, &value, ADDRESS_LENGTH, VALUE_LENGTH);
    L_INFO("write addres:{},value:{}", addres, value);
    value    = 0;
    int ret7 = QUsbCommunication::getInstance().readBurst(addres, &value, ADDRESS_LENGTH, VALUE_LENGTH);
    L_INFO("read addres:{},value:{}", addres, value);
}

void QMainWnd::closeEvent(QCloseEvent *event)
{
    QUsbCommunication::getInstance().closeDevice();
    close();
    QFlashImgBurn::instance()->close();
    interfaceDebug->close();
    event->accept();
}

void QMainWnd::saveAll()
{
    // 遍历ISP树的第二行ISP下拉列表中的所有节点（除了Register和GPIO）
    QString dirPath = QFileDialog::getExistingDirectory(this,tr("浏览选择文件夹"),QCoreApplication::applicationDirPath() + "\\Save");//返回用户选择的文件夹路径
    if(dirPath.isEmpty()) //如果用户选择的路径为空
    {
        return; //不改变原来编辑栏里的文件路径
    }

    QString folderPath = dirPath + "\\SaveAll_"+ getCurrentTime();

    QDir dir(folderPath);
    if (!dir.exists())
        dir.mkpath(folderPath);

    QProgressDialog dlgProgress(this);
    dlgProgress.setWindowModality(Qt::WindowModal);
    dlgProgress.setRange(0, treeItemMap.size() - 2);
    dlgProgress.setLabelText("Creating tabs...");
    dlgProgress.setCancelButton(nullptr);
    dlgProgress.setValue(0);
    dlgProgress.show();
    QCoreApplication::processEvents();
    QThread::msleep(20);
    QMap<QString, QPointer<QTabWndComm>> tabWndMapNew;
    int iCount = 0;
    for (auto it = treeItemMap.begin(); it != treeItemMap.end(); ++it) {
        QString moduleName = it.key();
        
        // 跳过Register和GPIO节点
        if (moduleName == "Register" || moduleName == "GPIO") {
            continue;
        }
        
        //检查该模块是否已经打开
        if (tabWndMap.contains(moduleName)) {
            // 如果已经打开，直接调用save函数
            
        } else {
            // 如果未打开，需要构造窗口对象
            QPointer<QTabWndComm> newTab = QTabWndCommFactory::instance().createTabWndComm(moduleName, this);
            if (newTab) {
                tabWndMapNew.insert(moduleName, newTab);
            }
        }
        QThread::msleep(20);
        dlgProgress.setLabelText("Creating tabs...");
        dlgProgress.setValue(++iCount);
        QCoreApplication::processEvents();
    }

    dlgProgress.setWindowModality(Qt::WindowModal);
    dlgProgress.setRange(0, 0);
    dlgProgress.setLabelText("Saving...");
    dlgProgress.setCancelButton(nullptr);
    dlgProgress.show();

    auto lambdaSave = [&]()
    {
        for (auto it = treeItemMap.begin(); it != treeItemMap.end(); ++it) {
            QString moduleName = it.key();
            
            // 跳过Register和GPIO节点
            if (moduleName == "Register" || moduleName == "GPIO") {
                continue;
            }
            
            //检查该模块是否已经打开
            if (tabWndMap.contains(moduleName)) {
                // 如果已经打开，直接调用save函数
                QPointer<QTabWndComm> existingTab = tabWndMap.value(moduleName);
                if (existingTab) {
                    existingTab->saveSpecial(folderPath);
                }
            } else {
                // 如果未打开，需要构造窗口对象
                QPointer<QTabWndComm> newTab = tabWndMapNew.value(moduleName);
                if (newTab) {
                    // 调用save函数
                    // std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 等待100毫秒，确保对象完全构造
                    newTab->saveSpecial(folderPath);
                    // 用完之后及时释放内存
                    newTab.clear();
                }
            }
        }
        // emit saveAllDone();
        // QMessageBox::information(this, "Info", "SaveAll run success. ");
    };
    
    // tSave.detach();

    
    // std::thread tSave(lambdaSave);
    // tSave.join();
    auto future = QFuture<void>(
        QtConcurrent::run(lambdaSave));
    while (!future.isFinished())
    {
        // _flashImage->addPartition(partType, partName);
        // std::vector<PartitionInfo> partitionInfos;
        // _flashImage->getAllPartitionInfo(partitionInfos);
        QThread::msleep(10);
        dlgProgress.setLabelText(QString("Saving..."));
        QCoreApplication::processEvents();
    }
    dlgProgress.close();
    QMessageBox::information(this, "Info", "SaveAll run success. ");
}

void QMainWnd::on_ConnectDemoBtn_clicked()
{
    if(usbConnected)
    {//当前ISP tuningTool已连接USB设备
        bDemoSensUDP=false;
        QMessageBox::warning(this, "", "Please Close Device ! \n\nThen Play DemoSens", QMessageBox::Ok);
        return;
    }

    confirmUDP();
}

