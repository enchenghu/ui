#ifndef QSHARPNESSTAB_H
#define QSHARPNESSTAB_H

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

const QMap<QPair<QString, QString>, bool> SHARPNESS_RGBIR_ENABLE_MAP = {
    { {"Sharpness", "shp_limit_rat"}, true },
    { {"Sharpness", "WhtBoost4"}, true },
    { {"Sharpness", "WhtBoost5"}, true },
    { {"Sharpness", "WhtBoost6"}, true },
    { {"Sharpness", "WhtBoost7"}, true },
    { {"Sharpness", "BlkBoost4"}, true },
    { {"Sharpness", "BlkBoost5"}, true },
    { {"Sharpness", "BlkBoost6"}, true },
    { {"Sharpness", "BlkBoost7"}, true },
    { {"Sharpness", "Int4"}, true},
    { {"Sharpness", "Int5"}, true },
    { {"Sharpness", "Int6"}, true },
    { {"Sharpness", "Int7"}, true },
    { {"Sharpness", "Rat4"}, true },
    { {"Sharpness", "Rat5"}, true },
    { {"Sharpness", "Rat6"}, true },
    { {"Sharpness", "Rat7"}, true },
    { {"Sharpness", "Adjust4"}, true },
    { {"Sharpness", "Adjust5"}, true },
    { {"Sharpness", "Adjust6"}, true },
    { {"Sharpness", "Adjust7"}, true },
    { {"Sharpness", "Back4"}, true },
    { {"Sharpness", "Back5"}, true },
    { {"Sharpness", "Back6"}, true },
    { {"Sharpness", "Back7"}, true },
    { {"Sharpness", "GainNode4"}, true },
    { {"Sharpness", "GainNode5"}, true },
    { {"Sharpness", "GainNode6"}, true },
    { {"Sharpness", "GainNode7"}, true }
};

class QSharpnessTab : public QTabWndComm
{
    Q_OBJECT
public:
    explicit QSharpnessTab(QWidget *parent = nullptr);
    ~QSharpnessTab();

public slots:
    void changeRgbIrMode(int &newMode)
    {
        if (newMode == 0) 
        {
            strRgbIrMode = "RGB";
        } else if (newMode == 1) {
            strRgbIrMode = "IR";
        }
    };

private:
    void init();
    void updateValue();
    void initConnect();
    std::vector<CombinedRegister> combinedRegisterSubPageRecording(QString strModuleName, QString strUiName, std::shared_ptr<int> ptrCamera, QString strSubPage);
    std::vector<CombinedRegister> getCombinedRegister(QString strModuleName, QString strUiName, std::shared_ptr<int> ptrCamera);
    void recordUIRegister();
    void recordUIRegSubPageInfo();
    void initLM();
    void initLFS();
    void initYUVDNS();
    void updateValue(std::vector<QPointer<QLabelEditGroup>> edit);
    void updateValue(std::vector<QPointer<QLabelCheckBox>> box);
    void updateTable();
    QString getStrByLSMV();
    void updateComboBox();
    void recvEditTarget(QString str);
    template <typename T>
    void updateUIValue(std::vector<QPointer<T>>& vector,const QString &extraname="");

private slots:
    void changeTab(int index);
    void recvValueChanged(QStandardItem* item,int row,int col);

private:
    QPointer<QTabWidget> tabComm=nullptr;
    QMap<QString,std::pair<QWidget*,bool>> tabMap;
    std::vector<QPointer<QLabelEditGroup>> LM_edit;
    std::vector<QPointer<QLabelCheckBox>> LM_box;
    std::vector<QPointer<QLabelEditGroup>> LFS_edit;
    std::vector<QPointer<QLabelCheckBox>> LFS_box;
    std::vector<QPointer<QLabelEditGroup>> YU_edit;
    std::vector<QPointer<QLabelCheckBox>> YU_box;
    std::set<uint32_t> regAddrModule;
    QStringList list;
    QPointer<QTableListGroup> tableListGroup=nullptr;
    QPointer<QComboBox> comboBoxLM=nullptr;
    QPointer<QComboBox> comboBoxSV=nullptr;
    QString strRgbIrMode = "RGB";
};

#endif
