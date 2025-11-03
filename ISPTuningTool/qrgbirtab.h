#ifndef QRGBIRTAB_H
#define QRGBIRTAB_H

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

const QMap<QPair<QString, QString>, bool> RGBIR_RGBIR_ENABLE_MAP = {
    { {"RGBIR", "DelRMrx[0]00"}, true },
    { {"RGBIR", "DelRMrx[0]01"}, true },
    { {"RGBIR", "DelRMrx[0]02"}, true },
    { {"RGBIR", "DelRMrx[0]03"}, true },
    { {"RGBIR", "DelRMrx[0]10"}, true },
    { {"RGBIR", "DelRMrx[0]11"}, true },
    { {"RGBIR", "DelRMrx[0]12"}, true },
    { {"RGBIR", "DelRMrx[0]13"}, true },
    { {"RGBIR", "DelRMrx[0]20"}, true },
    { {"RGBIR", "DelRMrx[0]21"}, true },
    { {"RGBIR", "DelRMrx[0]22"}, true },
    { {"RGBIR", "DelRMrx[0]23"}, true },
    { {"RGBIR", "DelRMrx[1]00"}, true },
    { {"RGBIR", "DelRMrx[1]01"}, true },
    { {"RGBIR", "DelRMrx[1]02"}, true },
    { {"RGBIR", "DelRMrx[1]03"}, true },
    { {"RGBIR", "DelRMrx[1]10"}, true },
    { {"RGBIR", "DelRMrx[1]11"}, true },
    { {"RGBIR", "DelRMrx[1]12"}, true },
    { {"RGBIR", "DelRMrx[1]13"}, true },
    { {"RGBIR", "DelRMrx[1]20"}, true },
    { {"RGBIR", "DelRMrx[1]21"}, true },
    { {"RGBIR", "DelRMrx[1]22"}, true },
    { {"RGBIR", "DelRMrx[1]23"}, true },
    { {"RGBIR", "DelRMrx[2]00"}, true },
    { {"RGBIR", "DelRMrx[2]01"}, true },
    { {"RGBIR", "DelRMrx[2]02"}, true },
    { {"RGBIR", "DelRMrx[2]03"}, true },
    { {"RGBIR", "DelRMrx[2]10"}, true },
    { {"RGBIR", "DelRMrx[2]11"}, true },
    { {"RGBIR", "DelRMrx[2]12"}, true },
    { {"RGBIR", "DelRMrx[2]13"}, true },
    { {"RGBIR", "DelRMrx[2]20"}, true },
    { {"RGBIR", "DelRMrx[2]21"}, true },
    { {"RGBIR", "DelRMrx[2]22"}, true },
    { {"RGBIR", "DelRMrx[2]23"}, true }
}; 

class QRGBIRTab : public QTabWndComm
{
    Q_OBJECT
public:
    explicit QRGBIRTab(QWidget *parent = nullptr);
    ~QRGBIRTab();
public slots:
    void changeRgbIrMode(int &newMode)
    {
        if (newMode == 0) 
        {
            strRgbIrMode = "RGB";
        } 
        else if (newMode == 1) 
        {
            strRgbIrMode = "IR";
        }
    };

private:
    void init();
    void updateValue();
    void initConnect();
    std::vector<CombinedRegister> combinedRegisterSubPageRecording(QString strModuleName, QString strUiName, std::shared_ptr<int> ptrCamera, QString strSubPage);
    std::vector<CombinedRegister> getCombinedRegister(QString strModuleName, QString strUiName, std::shared_ptr<int> ptrCamera);
    void recordUIRegSubPageInfo();
    void recordUIRegister();
    void initCIP();
    void initDPC();
    void updateValue(std::vector<QPointer<QLabelEditGroup>> edit);
    void updateValue(std::vector<QPointer<QLabelCheckBox>> box);
    void updateTable();

private slots:
    void changeTab(int index);
    void recvValueChanged(QStandardItem* item,int row,int col);
    void updateDPCEdit(int index);
private:
    QPointer<QTabWidget> tabComm=nullptr;
    QPointer<QComboBox> comboBox=nullptr;
    QMap<QString,std::pair<QWidget*,bool>> tabMap;
    QMap<int,std::pair<QString,QPointer<QTableListGroup>>> tableMap;
    std::vector<QPointer<QLabelEditGroup>> CIP_edit;
    std::vector<QPointer<QLabelCheckBox>> CIP_box;
    std::vector<QPointer<QLabelEditGroup>> DPC_edit;
    std::vector<QPointer<QLabelEditGroup>> DPC_Coe1edit;
    std::vector<QPointer<QLabelEditGroup>> DPC_Coe2edit;
    std::vector<QPointer<QLabelEditGroup>> DPC_Bpintedit;
    std::vector<QPointer<QLabelCheckBox>> DPC_box;
    QPointer<QTableListGroup> DelRMrx0=nullptr;
    QPointer<QTableListGroup> DelRMrx1=nullptr;
    QPointer<QTableListGroup> DelRMrx2=nullptr;
    std::set<uint32_t> regAddrModule;
    // std::vector<uint32_t> registerMoudle;
    QString strRgbIrMode = "RGB";
};

#endif
