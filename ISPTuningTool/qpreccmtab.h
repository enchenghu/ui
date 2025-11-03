#ifndef QPRECCMTAB_H
#define QPRECCMTAB_H

#include "Common/UI/qlabelcheckbox.h"
#include "Common/UI/qtabwndcomm.h"
#include <QAbstractItemModel>
#include <QGroupBox>
#include <QPointer>
#include <QRadioButton>
#include <QSplitter>
#include <QStandardItem>
#include <QWidget>
#include <QtConcurrent/QtConcurrent>
#include <set>
class QPreCCMTab : public QTabWndComm
{
    Q_OBJECT
public:
    explicit QPreCCMTab(QWidget *parent = nullptr);
    ~QPreCCMTab();

private:
    void init();
    void updateValue();
    void initConnect();
    void recordUIRegister();
    std::vector<CombinedRegister> combinedRegisterSubPageRecording(QString strModuleName, QString strUiName, std::shared_ptr<int> ptrCamera, QString strSubPage);
    void recordUIRegSubPageInfo();
    void updateTable();
private slots:
    void recvValueChanged(QStandardItem* item,int row,int col);

private:
    QMap<QString,std::pair<QWidget*,bool>> tabMap;
    std::vector<QPointer<QLabelEditGroup>> pre_edit;
    std::vector<QPointer<QLabelCheckBox>> pre_box;
    QPointer<QTableListGroup> Current=nullptr;
    QPointer<QTableListGroup> A=nullptr;
    QPointer<QTableListGroup> B=nullptr;
    QPointer<QTableListGroup> C=nullptr;
    QPointer<QComboBox> comboBox=nullptr;    
    QMap<int,std::pair<QString,QPointer<QTableListGroup>>> tableMap;
    std::set<uint32_t> regAddrModule;

};

#endif
