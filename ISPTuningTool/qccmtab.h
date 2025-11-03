#ifndef QCCMTAB_H
#define QCCMTAB_H

#include "Common/UI/qlabelcheckbox.h"
#include "Common/UI/qlabeleditgroup.h"
#include "Common/UI/qtabwndcomm.h"
#include <QAbstractItemModel>
#include <QRadioButton>
#include <QStandardItem>
#include <QWidget>
#include <QtConcurrent/QtConcurrent>

class QCCMTab : public QTabWndComm
{
    Q_OBJECT
public:
    explicit QCCMTab(QWidget *parent = nullptr);
    ~QCCMTab();
private:
    void init();
    void updateValue();
    void updateValueEditGroup();
    void updateValueMANMartix();
    void updateValueCCMDMartix();
    void updateValueCCMTMartix();
    void updateValueCCMAMartix();

    void initConnect();
    void recordUIRegister();
    void writeTable(QPointer<QTableListGroup> table, QString tableName);
    void recordUITableRegister(QString name,QString UIName);

public slots:
    void recvValueChanged(QStandardItem* item,int row,int col);
private slots:
    void recvAutoNormal(QString str,bool flag);

private:
    QPointer<QLabelCheckBox> CCMEnable=nullptr;
    QPointer<QLabelCheckBox> AutoNormal=nullptr;
    QPointer<QLabelCheckBox> ManCCMEnable=nullptr;
    QPointer<QLabelEditGroup> CTCurrent=nullptr;
    QPointer<QLabelEditGroup> CTD=nullptr;
    QPointer<QLabelEditGroup> CTM1=nullptr;
    QPointer<QLabelEditGroup> CTM2=nullptr;
    QPointer<QLabelEditGroup> CTA=nullptr;
    QPointer<QTableListGroup> MANTable=nullptr;
    QPointer<QTableListGroup> CCMDTable=nullptr;
    QPointer<QTableListGroup> CCMTTable=nullptr;
    QPointer<QTableListGroup> CCMATable=nullptr;
    QMap<int,std::pair<QString,QPointer<QTableListGroup>>> tableMap;
    QMap<QString,std::vector<std::vector<QStringList>>> UIRegisterMap;
    bool normalization=false;
    
    std::vector<QPointer<QLabelEditGroup>> ccm_edit;
    std::vector<QPointer<QLabelCheckBox>> ccm_box;
    int groupUIIndex=-1;
};

#endif // QCCMTAB_H
