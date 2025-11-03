#ifndef QCONTRAST_H
#define QCONTRAST_H

#include "Common/UI/qcommonuiinterface.h"
#include "Common/UI/qtabwndcomm.h"
#include "Common/UI/qlabelcheckbox.h"
#include "Common/UI/qlabelcombobox.h"
#include "Common/UI/qtablelistgroup.h"
#include <QSharedPointer>
#include <QStackedWidget>
#include <set>

const QMap<QPair<QString, QString>, bool> CONTRAST_RGBIR_ENABLE_MAP = {
    { {"Contrast", "ContrastEnable"}, true },
    // { {"Contrast", "SPoint"}, true },
    // { {"Contrast", "Intensity"}, true },
    // { {"Contrast", "ManLowLevel"}, true },
    // { {"Contrast", "ManStretchGain"}, true },
    { {"Contrast", "AdjustLuma"}, true },
    { {"Contrast", "StretchISOList[0]"}, true },
    { {"Contrast", "StretchISOList[1]"}, true },
    { {"Contrast", "StretchISOList[2]"}, true },
    { {"Contrast", "StretchISOList[3]"}, true },
    { {"Contrast", "StretchISOList[4]"}, true },
    { {"Contrast", "LowLevelOffset[0]"}, true },
    { {"Contrast", "LowLevelOffset[1]"}, true },
    { {"Contrast", "LowLevelOffset[2]"}, true },
    { {"Contrast", "LowLevelOffset[3]"}, true },
    { {"Contrast", "LowLevelOffset[4]"}, true },
    { {"Contrast", "StretchRat[0]"}, true },
    { {"Contrast", "StretchRat[1]"}, true },
    { {"Contrast", "StretchRat[2]"}, true },
    { {"Contrast", "StretchRat[3]"}, true },
    { {"Contrast", "StretchRat[4]"}, true },
    { {"Contrast", "LowPoints"}, true },
    { {"Contrast", "HighPoints"}, true },
    { {"Contrast", "LowStep"}, true },
    { {"Contrast", "HighStep"}, true },
    { {"Contrast", "MinLowLevelA"}, true },
    { {"Contrast", "MinLowLevelB"}, true },
    { {"Contrast", "MaxLowLevelA"}, true },
    { {"Contrast", "MaxLowLevelB"}, true },
    { {"Contrast", "MinHighLevelA"}, true },
    { {"Contrast", "MinHighLevelB"}, true },
    { {"Contrast", "CurveThre1"}, true },
    { {"Contrast", "CurveThre2"}, true },
    { {"Contrast", "Mode"}, true },
    { {"Contrast", "Manual"}, true },
    { {"Contrast", "ISO Node[0]"}, true },
    { {"Contrast", "ISO Node[1]"}, true },
    { {"Contrast", "ISO Node[2]"}, true },
    { {"Contrast", "ISO Node[3]"}, true },
    { {"Contrast", "ISO Node[4]"}, true },
    { {"Contrast", "Low Point[0]"}, true },
    { {"Contrast", "Low Point[1]"}, true },
    { {"Contrast", "Low Point[2]"}, true },
    { {"Contrast", "Low Point[3]"}, true },
    { {"Contrast", "Low Point[4]"}, true },
    { {"Contrast", "High Point[0]"}, true },
    { {"Contrast", "High Point[1]"}, true },
    { {"Contrast", "High Point[2]"}, true },
    { {"Contrast", "High Point[3]"}, true },
    { {"Contrast", "High Point[4]"}, true },
    { {"Contrast", "Low Str[0]"}, true },
    { {"Contrast", "Low Str[1]"}, true },
    { {"Contrast", "Low Str[2]"}, true },
    { {"Contrast", "Low Str[3]"}, true },
    { {"Contrast", "Low Str[4]"}, true },
    { {"Contrast", "High Str[0]"}, true },
    { {"Contrast", "High Str[1]"}, true },
    { {"Contrast", "High Str[2]"}, true },
    { {"Contrast", "High Str[3]"}, true },
    { {"Contrast", "High Str[4]"}, true }
};

class QContrast : public QTabWndComm
{
    Q_OBJECT
public:
    explicit QContrast(QWidget *parent = nullptr);
    ~QContrast();
    void init();
    void initConnect();
    void updateValue();
    template <typename T>
    void updateUIValue(QPointer<T> *vector, int iLength, const QString &extraname);
public slots:
    void recvEditTarget(QString str);
    void changeRgbIrMode(int &newMode);
    void changeTab(int index);
    void recvMode(QString str);
    void recvTableValueChanged(QStandardItem *item, int row, int col);
    // void onCheckBoxManualClicked(std::vector<CombinedRegister> combineList);
    void UpdateTable();

private:
    
    QPointer<QTabWidget> tabWidget = nullptr;
    std::set<uint32_t> regAddrModule;
    
    // Contrast tab controls
    QPointer<QLabelCheckBox> sCurveEnable = nullptr;
    QPointer<QLabelEditGroup> sPoint = nullptr;
    QPointer<QLabelEditGroup> intensity = nullptr;
    QPointer<QLabelCheckBox> manStretchEnable = nullptr;
    QPointer<QLabelEditGroup> manLowLevel = nullptr;
    QPointer<QLabelEditGroup> manStretchGain = nullptr;
    QPointer<QLabelComboBox> modeComboBox = nullptr;
    QPointer<QLabelEditGroup> curIso = nullptr;
    QPointer<QLabelCheckBox> checkAuto = nullptr;
    QPointer<QLabelEditGroup> isoNode[5] = {nullptr, nullptr, nullptr, nullptr, nullptr};
    QPointer<QTableListGroup> tableListGroup = nullptr;
    QPointer<QStackedWidget> stackedWidget = nullptr;

    std::vector<std::vector<CombinedRegister>> ModeCombineVector;
    std::vector<std::vector<CombinedRegister>> ManualCombineVector;
    
    // AutoStrech tab controls
    QPointer<QLabelCheckBox> adjustLuma = nullptr;
    QPointer<QLabelEditGroup> stretchIsoList[5] = {nullptr};
    QPointer<QLabelEditGroup> lowLevelOffset[5] = {nullptr};
    QPointer<QLabelEditGroup> stretchRat[5] = {nullptr};
    
    // LowLevel tab controls
    QPointer<QLabelEditGroup> minLowLevelA = nullptr;
    QPointer<QLabelEditGroup> minLowLevelB = nullptr;
    QPointer<QLabelEditGroup> minHighLevelA = nullptr;
    QPointer<QLabelEditGroup> maxLowLevelA = nullptr;
    QPointer<QLabelEditGroup> maxLowLevelB = nullptr;
    QPointer<QLabelEditGroup> minHighLevelB = nullptr;
    QPointer<QLabelEditGroup> curveThre1 = nullptr;
    QPointer<QLabelEditGroup> curveThre2 = nullptr;
    QPointer<QLabelEditGroup> currentGainL = nullptr;

    void initContrastSubTab(QPointer<QWidget> subTab);
    void initAutoStrechSubTab(QPointer<QWidget> subTab);
    void initLowLevelSubTab(QPointer<QWidget> subTab);
    void recordTableUIRegister();
    
    // 替代insertControl的新方法
    std::vector<CombinedRegister> combinedRegisterSubPageRecording(QString strModuleName, QString strUiName, std::shared_ptr<int> ptrCamera, QString strSubPage);
    std::vector<CombinedRegister> getCombinedRegister(QString strModuleName, QString strUiName, std::shared_ptr<int> ptrCamera);
    void recordUIRegSubPageInfo();
    void recordUIRegister();
    
    QString strRgbIrMode = "RGB";
};

#endif // QCONTRAST_H
