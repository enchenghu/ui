#ifndef QLSCTAB_H
#define QLSCTAB_H

#include "Common/UI/qcurvechartgroup.h"
#include "Common/UI/qlabelcheckbox.h"
#include "Common/UI/qlabelcombobox.h"
#include "Common/UI/qtabwndcomm.h"
#include <QGroupBox>
#include <QPointer>
#include <QWidget>

const int LSC_CP_TABLE_ROW = 8;
const int LSC_CP_TABLE_COL = 10;

enum LSC_TYPE {
    LSC_NORMAL = 0,
    LSC_LFS,
    LSC_TYPE_MAX,
};

enum LSC_CURVE_TBALE_TYPE {
    LSC_CURVE_TBALE_CP = 0,
    LSC_CURVE_TBALE_QCELL,
    LSC_CURVE_TBALE_TYPE_MAX,
};

enum LSC_COLOR_TYPE {
    LSC_COLOR_G = 0,
    LSC_COLOR_B,
    LSC_COLOR_R,
    LSC_COLOR_TYPE_MAX,
};

enum LSC_CP_TPYE {
    LSC_CP_CPA = 0,
    LSC_CP_CPM,
    LSC_CP_CPD,
    LSC_CP_TPYE_MAX,
};

class QLscTab : public QTabWndComm
{
    Q_OBJECT
public:
    explicit QLscTab(QWidget *parent = nullptr);
    ~QLscTab();
    void init();
    void initConnect();

public slots:
    void comboCurrentTypeValueChanged(int index);
    void comboCpQCellTypeValueChanged(int index);
    void labelComboCpIndexValueChanged(int index);
    void labelComboCpTypeValueChanged(int index);
    void curveDoubleClicked(QString name);
    void recvItemDoubleClicked(QStandardItem *item, int row, int col);
    void updateCombineRegisterTable(std::vector<CombinedRegister> combineList);
    void readPage();
    void reConnectReadPage();

private:
    QPointer<QComboBox>        comboCurrentType     = nullptr;
    QPointer<QVBoxLayout>      layoutLeft           = nullptr;
    QPointer<QVBoxLayout>      layoutMiddle         = nullptr;
    QPointer<QVBoxLayout>      layoutRight          = nullptr;
    QPointer<QLabelCheckBox>   labelCheckLscEnable  = nullptr;
    QPointer<QLabelCheckBox>   labelCheckMonoEn     = nullptr;
    //QPointer<QLabelEditGroup>  labelEditGainSel     = nullptr;
    QPointer<QLabelEditGroup>  labelEditManCTOption = nullptr;
    QPointer<QLabelEditGroup>  labelEditHScale      = nullptr;
    QPointer<QLabelEditGroup>  labelEditVScale      = nullptr;
    QPointer<QLabelEditGroup>  labelEditCurCT       = nullptr;
    QPointer<QLabelEditGroup>  labelEditCTD         = nullptr;
    QPointer<QLabelEditGroup>  labelEditCTM1        = nullptr;
    QPointer<QLabelEditGroup>  labelEditCTM2        = nullptr;
    QPointer<QLabelEditGroup>  labelEditCTA         = nullptr;
    QPointer<QComboBox>        comboCpQCellType     = nullptr;
    QPointer<QGroupBox>        groupQCellCurve      = nullptr;
    QPointer<QGroupBox>        groupCpTable         = nullptr;
    QPointer<QVBoxLayout>      layoutQCellCurve     = nullptr;
    QPointer<QVBoxLayout>      layoutCpTable        = nullptr;
    QPointer<QHBoxLayout>      layoutCpTableTitle   = nullptr;
    QPointer<QLabelComboBox>   labelComboCpIndex    = nullptr;
    QPointer<QLabelComboBox>   labelComboCpType     = nullptr;
    QPointer<QTableListGroup>  tableCp              = nullptr;
    QPointer<QCurveChartGroup> curveChartQCell      = nullptr;
    QString                                            currModuleName;
    std::vector<std::pair<CombinedRegister, uint32_t>> cpTableList; // 用于CP表格信息
    std::vector<std::pair<CombinedRegister, uint32_t>> curvePointListX; // 用于QCell X轴坐标信息
    std::vector<std::pair<CombinedRegister, uint32_t>> curvePointListY; // 用于QCell Y轴坐标信息
    QStringList                                        curvePointNameX;
    QStringList                                        curvePointNameY;
    std::vector<std::tuple<int, int, int>>             cpTableColor[LSC_COLOR_TYPE_MAX];

    void setControlEnable(bool enable);
    void setControlMouseTracking(bool enable);
    void getAllRegInfoInit();
    void initCpTableColor();
    void initCpTable();
    void readCpTableUpate();
    void readCpTable();
    void updateCpTable();
    void readQCellCurveUpate();
    void readQCellCurve();
    void updateQCellCurve();
    // void combineRegisterSet(QCommonUiInterface *widget);
    void setControlCombineRegister();
    void readControlInfoUpdate();
};

#endif // QLSCTAB_H
