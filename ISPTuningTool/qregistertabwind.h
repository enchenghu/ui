#ifndef QREGISTERTABWIND_H
#define QREGISTERTABWIND_H

#include <QWidget>
#include <QTableView>
#include <QtConcurrent/QtConcurrent>
#include <QFile>
#include <QDir>
#include <QFileDialog>
#include "Common/UI/qlabelcombobox.h"
#include "Common/UI/qlabelspinslidercheckboxgroup.h"
#include "Common/UI/qpushbuttongroup.h"
#include "Common/UI/qtablelistgroup.h"
#include "Common/UI/qtexteditgroup.h"
#include "DatabaseOrm/databaseorm.h"
#include "DatabaseOrm/databaseaccess.h"
#include "Common/Others/qusbcommunication.h"

using namespace DatabaseOrm;

namespace Ui {
class qregistertabwind;
}

class qregistertabwind : public QWidget
{
    Q_OBJECT

public:
    explicit qregistertabwind(QWidget *parent = nullptr);
    ~qregistertabwind();
    void init();
    void initConnect();
    void recordAllModules(std::vector<DatabaseOrm::Module> modules);
    int getValueByRegisterMoudle(int addr);
    void setCheckState(bool isChecked);
public slots:
    void updateTable(QString str);
    void updateTable(uint32_t addr);
    void readMoudleRegisterValue();
    void loadMoudleRegisterValue();
    void saveMoudleRegisterValue();
    void updateRegisterMoudlebyAddress(int addr);

private:
    Ui::qregistertabwind *ui;
    std::shared_ptr<QTableListGroup> registerTable=nullptr;//控制主界面4号表格
    std::shared_ptr<QTableListGroup> propertyTable=nullptr;//控制主界面5号表格
    std::shared_ptr<QLabelSpinSliderCheckBoxGroup> labelSpinSliderCheckBoxGroup=nullptr;//Register set控件
    std::shared_ptr<QTextEditGroup> textEditGroup=nullptr;//Communication log控件
    std::shared_ptr<QLabelComboBox> labelComboBox=nullptr;//Module 下拉列表控
    std::shared_ptr<QLabelEditGroup> labelEditGroup=nullptr;//Search 控件
    std::shared_ptr<QPushButtonGroup> pushButtonGroup=nullptr;//read,load,save,通用组合控件
    QMap<std::string,std::vector<std::pair<Register, uint32_t>>> registerMoudlesMap;//与treeview关联，缓存Moudle的所有register，用于快速打开切换页面
    std::vector<std::pair<Register, uint32_t>> registerMoudle;
    void updateTableRegisterValue();
    void writeSensorRegisterValue(std::vector<std::pair<uint32_t,uint32_t>> &regValue);
    bool isCheckedFlag=true;//勾选标记，ISP打开tab页是否主动调取readBrust
};

#endif // QREGISTERTABWIND_H
