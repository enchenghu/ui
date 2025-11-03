#ifndef QSATURATION_H
#define QSATURATION_H

#include "Common/UI/qcommonuiinterface.h"
#include "Common/UI/qcurvechartgroup.h"
#include "Common/UI/qtabwndcomm.h"
#include "Common/UI/qlabelcheckbox.h"
#include <QSharedPointer>
#include <set>

const QMap<QPair<QString, QString>, bool> SATURATION_RGBIR_ENABLE_MAP = {
    { {"Saturation", "GainNode[4]"}, true },
    { {"Saturation", "GainNode[5]"}, true },
    { {"Saturation", "GainNode[6]"}, true },
    { {"Saturation", "GainNode[7]"}, true }
};

class QSaturation : public QTabWndComm
{
    Q_OBJECT
public:
    explicit QSaturation(QWidget *parent = nullptr);
    ~QSaturation();
    void init();
    void initConnect();
    template <typename T>
    void updateUIValue(QPointer<T> *vector, int iLength, const QString &extraname);
    void updateValue();
public slots:
    void recvEditTarget(QString str);
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
    }
    void changeTab(int index);
    void recvPointsChanged(std::vector<QPoint> points);
    void readCurve();
    void recvTableListItem(QStandardItem *citem, int row, int col);

private:
    QPointer<QTabWidget>                tabWidget = nullptr;
    QList<QPointer<QCommonUiInterface>> subWidgets;
    QPointer<QCurveChartGroup>          isoSatCurve = nullptr;
    std::set<uint32_t> regAddrModule;
    
    // Curve tab widgets
    QPointer<QLabelCheckBox> gainSatEnable = nullptr;
    QPointer<QLabelEditGroup> saturation = nullptr;
    QPointer<QLabelEditGroup> satIsoList[8] = {nullptr};
    QPointer<QLabelEditGroup> curIso = nullptr;
    
    // YUVDNS tab widgets
    QPointer<QLabelEditGroup> gainNode[8] = {nullptr};
    QPointer<QLabelCheckBox> satAdjEnable = nullptr;
    QPointer<QLabelEditGroup> curGain = nullptr;
    QPointer<QTableListGroup> tableList = nullptr;

    void initCurveSubTab(QPointer<QWidget> subTab);
    void initYuvDnsSubTab(QPointer<QWidget> subTab);
    void sortSatIsoList();
    
    // 替代insertControl的新方法
    std::vector<CombinedRegister> combinedRegisterSubPageRecording(QString strModuleName, QString strUiName, std::shared_ptr<int> ptrCamera, QString strSubPage);
    std::vector<CombinedRegister> getCombinedRegister(QString strModuleName, QString strUiName, std::shared_ptr<int> ptrCamera);
    void recordUIRegSubPageInfo();
    void recordUIRegister();
    
    QString strRgbIrMode = "RGB";
};

#endif // QSATURATION_H