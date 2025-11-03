#ifndef QTONEMAPPINGTAB_H
#define QTONEMAPPINGTAB_H

#include "Common/UI/qcommonuiinterface.h"
#include "Common/UI/qcurvechartgroup.h"
#include "Common/UI/qtabwndcomm.h"
#include "Common/UI/qlabeleditgroup.h"
#include "Common/UI/qlabelcheckbox.h"
#include <QSharedPointer>
#include <set>


class QToneMappingTab : public QTabWndComm
{
    Q_OBJECT
public:
    explicit QToneMappingTab(QWidget *parent = nullptr);
    ~QToneMappingTab();
    template <typename T>
    void updateUIValue(T& vector);
    
private:
    void init();
    void initConnect();
    std::vector<CombinedRegister> combinedRegisterSubPageRecording(QString strModuleName, QString strUiName, std::shared_ptr<int> ptrCamera, QString strSubPage);
    std::vector<CombinedRegister> getCombinedRegister(QString strModuleName, QString strUiName, std::shared_ptr<int> ptrCamera);
    void recordUIRegSubPageInfo();
    void recordUIRegister();
    void updateValue();
    
    void initTMSubTab(QPointer<QWidget> subTab);
    void initGTMHistSubTab(QPointer<QWidget> subTab);
    void initLTMSubTab(QPointer<QWidget> subTab);
    void initGTMSubTab(QPointer<QWidget> subTab);
    void initMotionSubTab(QPointer<QWidget> subTab);
    void initFisheyeSubTab(QPointer<QWidget> subTab);
    
    // 存储各个子页面的控件
    QMap<QString, QList<QPointer<QLabelEditGroup>>> labelEditGroups;
    QMap<QString, QList<QPointer<QLabelCheckBox>>> labelCheckBoxes;

private slots:
    void changeTab(int index);
    void gtmHistComboStringChanged(const QString &str);
    void recvPointChanged(QPoint point);
    void readCurve();
    
private:
    QPointer<QTabWidget>       tabWidget = nullptr;
    QPointer<QComboBox>        gtmHistCurveCombo = nullptr;
    QPointer<QCurveChartGroup> gtmHistCurve = nullptr;
    std::set<uint32_t>         regAddrModule;
};

#endif // QTONEMAPPINGTAB_H
