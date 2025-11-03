#ifndef QMAINWND_H
#define QMAINWND_H

#include "Common/Others/qusbcommunication.h"
#include "Common/UI/qlabelbutton.h"
#include "Common/UI/qtabwndcomm.h"
#include "DatabaseOrm/databaseaccess.h"
#include "DatabaseOrm/databaseorm.h"
#include "AWBCalibrationDLL/awbcalibrationdll.h"
#include "qcommondef.h"
#include "qflashimgburn.h"
#include "qinterfacedebug.h"
#include "qregistertabwind.h"
#include "qgpioset.h"
#include "qvcseting.h"
#include <Common/UI/qconfigdialog.h>
#include <QDebug>
#include <QMainWindow>
#include <QMap>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QString>
#include <QTabWidget>
#include <QToolBar>
#include <QTreeWidget>
#include <QtConcurrent/QtConcurrent>
#include "qpipiclient.h"
#include "qregister.h"
#include "FPNC/qfpnc.h"
#include "Deir/deir.h"

using namespace std;
using namespace DatabaseOrm;

QT_BEGIN_NAMESPACE
namespace Ui {
class QMainWnd;
}
QT_END_NAMESPACE

class QMainWnd : public QMainWindow
{
    Q_OBJECT

public:
    QMainWnd(QWidget *parent = nullptr);
    ~QMainWnd();
    void initWnd();

private:
    /**
     * @brief initConnect 初始化信号连接
     * @return
     */
    void initConnect();
    /**
     * @brief buildTree 创建ISP树
     * @param registersVector 当前界面所有Register
     * @return
     */
    void on_treeItem_itemChanged(QStandardItem *item);
    /**
     * @brief setChildTreeItemStatus 设置子节点的选中状态
     * @param item 当前节点
     */
    void setChildTreeItemStatus(QStandardItem *item);
    /**
     * @brief setParentTreeItemStatus 设置父节点的选中状态
     * @param item 父节点
     */
    void           setParentTreeItemStatus(QStandardItem *item);
    QStandardItem *buildTree(std::vector<DatabaseOrm::UiModule> modules); // std::vector<modules> 与ORM接口相关联
    QStandardItem *buildCalibrationTree(); 

    void           updateTabCamera(int index);
    void           loadToolBar();

    //初始化awb数据库写入回调函数
    void initAWBWriteFunc();
    //初始化awb获取寄存器地址函数
    void initAWBGetRegisterFunc();
    //初始化awb获取寄存器值函数
    void initAWBGetRegisterValueFunc();
    //初始化用户权限
    void initUserAuthority();

    void confirmUDP();

    void saveAll();
public slots:
    void closeCurrentTab(int index);                          // 关闭当前tab页面信号槽
    void on_treeView_doubleClicked(const QModelIndex &index); // ISP树点击打开信号槽
    void setCurrentTreeItem(int index);
    //更新用户权限等级信息
    void updateNewLevel();
private slots:
    void on_bthRefreshHardware_clicked();
    void on_btnOpenDevice_clicked();
    void loadStyleConfig();    
    void on_ConnectDemoBtn_clicked();

private:
    Ui::QMainWnd                        *ui;
    QTreeWidget                         *treeComm  = 0;
    std::shared_ptr<QTabWidget>          tabComm   = nullptr; // 控制主界面唯一tab
    std::shared_ptr<QStandardItemModel>  treeModel = nullptr; // 控制主界面唯一tree
    QMap<QString, QWidget *>             tabCommOpenedMap;    // 记录主界面所有打开tab防止重复
    QMap<QString, QPointer<QTabWndComm>> tabWndMap;           // 记录主界面所有打开tab防止重复
    QMap<QString, QStandardItem *>       treeItemMap;         // 记录主界面所有打开tab防止重复
    std::vector<DatabaseOrm::UiModule>   ui_modules;
    std::vector<DatabaseOrm::Module>     modules;
    QPointer<QToolBar>                   toolBar        = nullptr;
    QPointer<QConfigDialog>              dialog         = nullptr;
    QPointer<QInterfaceDebug>            interfaceDebug = nullptr;
    QPointer<QVCSeting>                  VCSetingTool = nullptr;
    QPointer<qRegister>                  m_pRegister = nullptr;
    // QProgressDialog                      dlgProgress;

    bool                                 usbConnected;
    char                                 deviceNameList[MAX_DEVICE_NUM][DEVICE_NAME_LENGTH];
    int                                  deviceAddrList[MAX_DEVICE_NUM];
    int                                  deviceCount;
    uint                                 m_nLevel;       //使用者等级
    void testReadWrite();
    virtual void closeEvent(QCloseEvent *event);

signals:
    void saveAllDone();
};
#endif // QMAINWND_H
