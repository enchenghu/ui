#ifndef QAECTAB_H
#define QAECTAB_H

#include "Common/UI/qtabwndcomm.h"
#include "Common/UI/qlabeleditgroup.h"
#include "Common/UI/qlabelcombobox.h"
#include "Common/UI/qlabelcheckbox.h"
#include "Common/UI/qtablelistgroup.h"
#include <QGroupBox>
#include <set>

typedef QVector<QVector<QPair<QString, QString>>> ConfigType;

class QAECTab : public QTabWndComm
{
    Q_OBJECT
public:
    explicit QAECTab(QWidget *parent = nullptr);
    ~QAECTab();
    void recordCombineRegbyUIifo(QString moudle_name, QString uiname, std::shared_ptr<int> cameraId, QString uiModuleName);
public slots:
    void changeRgbIrMode(int &newMode);
    void recvEditTarget(QString str, QString strModule);
    void changeTab(int index);
private:
    void init();
    void initConnect();
    void recordUIRegister();
    std::vector<CombinedRegister> combinedRegisterSubPageRecording(QString strModuleName, QString strUiName, std::shared_ptr<int> ptrCamera, QString uiModuleName, QString strSubPage);
    std::vector<CombinedRegister> getCombinedRegister(QString strModuleName, QString strUiName, std::shared_ptr<int> ptrCamera, QString uiModuleName);
    void recordUIRegSubPageInfo();
    void initExpTypeSubTab(QPointer<QWidget> subTab);
    void initTargetSpeedSubTab(QPointer<QWidget> subTab);
    void initRoiSubTab(QPointer<QWidget> subTab);
    void initRoiSubTabLeftSub(QPointer<QWidget> subTab, QString tabName);
    void initRoiSubTabRightSub(QPointer<QWidget> subTab, QString tabName);
    
    QPointer<QGroupBox> generateGroupBox(QString groupBoxName, QString uiModuleName, ConfigType config, QString strSubPage, int width=100);
    // QPointer<QGroupBox> generateGroupBoxTarget(QString groupBoxName, QString uiModuleName, ConfigType config,int width=100);

    void generateWidgetGroupBox(QPointer<QGroupBox> groupBox, QPointer<QGridLayout> layout, QString uiModuleName, int col, int row, QString strSubPage, 
        QPair<QString, QString> uiTypeAndName, int width = 100);
    
    // 处理不同类型控件的子函数
    void handleLabelEditGroup(QPointer<QGroupBox> groupBox, QPointer<QGridLayout> layout, QString uiModuleName, int col, int row, QString strSubPage,
        QPair<QString, QString> uiTypeAndName, int width);
    void handleLabelComboBox(QPointer<QGroupBox> groupBox, QPointer<QGridLayout> layout, QString uiModuleName, int col, int row, QString strSubPage,
        QPair<QString, QString> uiTypeAndName, int width);
    void handleLabelCheckBox(QPointer<QGroupBox> groupBox, QPointer<QGridLayout> layout, QString uiModuleName, int col, int row, QString strSubPage,
        QPair<QString, QString> uiTypeAndName, int width);
    
    void generateWidgetLabelEditGroupBoxRgbIr(QPointer<QGroupBox> groupBox, QPointer<QGridLayout> layout, QString uiModuleName, int col, int row, QString strSubPage, 
        QPair<QString, QString> uiTypeAndName, int width = 100);
    
    template <typename T>
    void updateUIValue(QPointer<T> *vector, int iLength, const QString &extraname, QString strModuleName);
    
    QPointer<QTabWidget> tabWidget = nullptr;
    QString strRgbIrMode = "RGB";
    std::set<uint32_t> regAddrModule;
    
    // 控件指针声明
    std::vector<QPointer<QLabelEditGroup>> expGainEdit;
    std::vector<QPointer<QLabelCheckBox>> expGainCheck;
    std::vector<QPointer<QLabelEditGroup>> expGainPipeCtrlEdit;
    std::vector<QPointer<QLabelEditGroup>> targetEdit;
    std::vector<QPointer<QLabelEditGroup>> speedEdit;
    QPointer<QLabelComboBox> expTypeCombo = nullptr;
    QPointer<QLabelEditGroup> EVThreL_C = nullptr;
    std::vector<QPointer<QLabelEditGroup>> roiEditL;
    std::vector<QPointer<QLabelEditGroup>> roiEditM;
    std::vector<QPointer<QLabelEditGroup>> roiEditS;
    std::vector<QPointer<QLabelEditGroup>> roiEditV;
    QPointer<QLabelComboBox> roiType = nullptr;
    // QPointer<QTableListGroup> roiWeightTable = nullptr;
    std::vector<QPointer<QTableListGroup>> roiWeightTables; // 用于存储不同类型ROI的权重表指针
    std::vector<QPointer<QLabelCheckBox>> roiWeightEnableCheck;

    uint32_t roi_width;
    uint32_t roi_height;
    // uint32_t roi_blks;
};

#endif
