#ifndef QGAMMA_H
#define QGAMMA_H

#include "Common/UI/qcurvechartgroup.h"
#include "Common/UI/qdirectionbuttongroup.h"
#include "Common/UI/qlabelcheckbox.h"
#include "Common/UI/qlabelcombobox.h"
#include "Common/UI/qtabwndcomm.h"
#include <QGroupBox>
#include <QPushButton>
#include <QWidget>

namespace Ui {
class QGamma;
}

class QGamma : public QTabWndComm
{
    Q_OBJECT

public:
    explicit QGamma(QWidget *parent = nullptr, QString moudle = "Gamma");
    ~QGamma();
    void init();
    void initConnect();

public slots:
    void labelComboIndexValueChanged(int index);
    void btnUpClicked();
    void btnDownClicked();
    void btnLeftClicked();
    void btnRightClicked();
    void curvePointsValueChanged(std::vector<QPoint> vecPoint);
    void btnReadClicked();
    void btnResetClicked();
    void comboPointNumValueChanged(int index);
    void comboCurveTypeValueChanged(int index);
    void comboReferenceSetValueChanged(int index);
    void btnReferenceSetSaveClicked();
    void btnReferenceSetUseClicked();
    void btnImportDataClicked();
    void btnExportDataClicked();
    void readOptionInfoUpdate();
    void reConnectReadPage();
    void refreshRegInfo(int &newMode);

private:
    Ui::QGamma                     *ui;
    QPointer<QLabelCheckBox>        labelCheckEnable     = nullptr;
    QPointer<QLabelComboBox>        labelComboIndex      = nullptr;
    QPointer<QLabelEditGroup>       labelEditCurrentGain = nullptr;
    QPointer<QLabelEditGroup>       labelEditCurveThre1  = nullptr;
    QPointer<QLabelEditGroup>       labelEditCurveThre2  = nullptr;
    QPointer<QCurveChartGroup>      curveChartGroup      = nullptr;
    QPointer<QTableListGroup>       tableRegValue        = nullptr;
    QPointer<QDirectionButtonGroup> btnGroupLRTB         = nullptr;
    QPointer<QPushButton>           btnRead              = nullptr;
    QPointer<QPushButton>           btnReset             = nullptr;
    QPointer<QComboBox>             comboPointNum        = nullptr;
    QPointer<QComboBox>             comboCurveType       = nullptr;
    QPointer<QComboBox>             comboReferenceSet    = nullptr;
    QPointer<QPushButton>           btnReferenceSetSave  = nullptr;
    QPointer<QPushButton>           btnReferenceSetUse   = nullptr;
    QPointer<QPushButton>           btnImportData        = nullptr;
    QPointer<QPushButton>           btnExportData        = nullptr;
    QPointer<QHBoxLayout>           layoutCurveOption    = nullptr;
    QPointer<QHBoxLayout>           layoutCurve          = nullptr;
    QPointer<QHBoxLayout>           layoutReferenceSet   = nullptr;
    QPointer<QHBoxLayout>           layoutImportExport   = nullptr;
    QPointer<QGroupBox>             groupCurve           = nullptr;
    QPointer<QGroupBox>             groupReferenceSet    = nullptr;
    QPointer<QGroupBox>             groupImportExport    = nullptr;

    QString regMoudleName;            // 数据库实际moudel name
    QString regMoudleNameHw;          // 数据库实际moudel name
    QString regMoudleNameCurrentGain; // 数据库实际moudel name
    int     cameraId;                 // cameraId
    int     referencesetIndex;
    int     curveDelta;
    bool    isSetSave[GAMMA_TYPE_MAX][REFERENCE_SET_MAX];
    bool    readFlag;
    std::vector<uint32_t> gammaValueReferenceSet[GAMMA_TYPE_MAX][REFERENCE_SET_MAX]; // 用于ReferenceSet存储曲线点的信息
    std::vector<std::pair<CombinedRegister, uint32_t>> gammaList;                    // 用于存储曲线点的信息
    std::vector<std::pair<CombinedRegister, uint32_t>> gammaListA;                    // 用于存储曲线点的信息
    std::vector<std::pair<CombinedRegister, uint32_t>> gammaListB;                    // 用于存储曲线点的信息
    std::vector<std::pair<CombinedRegister, uint32_t>> gammaListARgb;                    // 用于存储曲线点的信息
    std::vector<std::pair<CombinedRegister, uint32_t>> gammaListBRgb;                    // 用于存储曲线点的信息
    std::vector<std::pair<CombinedRegister, uint32_t>> gammaListAIr;                    // 用于存储曲线点的信息
    std::vector<std::pair<CombinedRegister, uint32_t>> gammaListBIr;                    // 用于存储曲线点的信息
    std::vector<std::vector<CombinedRegister>> vecCombinedRegsPair1;
    std::vector<std::vector<CombinedRegister>> vecCombinedRegsPair2;
    QString strRgbIrMode = "RGB";
    int iModeRegValue = 0;

    void                  getOptionInfoInit();       // 从数据库读取option系列所使用的信息
    void                  getGammmaInfoInit();       // 从数据库读取gamma曲线所使用的信息
    void                  getCurrentGammaListInfo(); // 从数据库读取当前使用gammaList信息
    void                  readGammaListInfo();
    void                  readGammaListInfoUpdate();
    void                  updateGammaListTable();
    void                  updateOptionListInfo();
    std::vector<QPoint>   getGammaListPoints();
    QString               getCombinedRegisterAddressString(CombinedRegister &combineReg);
    std::vector<uint32_t> getCombineRegisterAddressSingle(CombinedRegister &combineReg);
    std::vector<std::pair<uint32_t, uint32_t>> getCombineRegisterAddressValueSingle();
};

class QGammaTab : public QGamma
{
    Q_OBJECT
public:
    QGammaTab(QWidget *parent = nullptr)
        : QGamma(parent, "Gamma")
    {
    }
};

class QGammaGainTab : public QGamma
{
    Q_OBJECT
public:
    QGammaGainTab(QWidget *parent = nullptr)
        : QGamma(parent, "GammaGain")
    {
    }
};

#endif // QGAMMA_H
