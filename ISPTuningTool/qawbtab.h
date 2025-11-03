#ifndef QAWBTAB_H
#define QAWBTAB_H

#include "Common/UI/qlabelcheckbox.h"
#include "Common/UI/qtabwndcomm.h"
#include <Common/UI/qcurvechartgroup.h>
#include <QAbstractItemModel>
#include <QGroupBox>
#include <QPointer>
#include <QRadioButton>
#include <QSplitter>
#include <QStandardItem>
#include <QWidget>
#include <Common/UI/qlabelcombobox.h>
#include <set>
class QAWBTab : public QTabWndComm
{
    Q_OBJECT
public:
    explicit QAWBTab(QWidget *parent = nullptr);
    ~QAWBTab();

private:
    void init();
    void initConnect();
    void recordUIRegister();
    std::vector<CombinedRegister> combinedRegisterSubPageRecording(QString strModuleName, QString strUiName, std::shared_ptr<int> ptrCamera, QString strSubPage);
    std::vector<CombinedRegister> getCombinedRegister(QString strModuleName, QString strUiName, std::shared_ptr<int> ptrCamera);
    void recordUIRegSubPageInfo();
    void initAWBCtrl(QPointer<QWidget> subTab);
    void initGreenSKY(QPointer<QWidget> subTab);
    void initLowCT(QPointer<QWidget> subTab);
    void initLFS(QPointer<QWidget> subTab);
    void updatebox();

private slots:
    void changeTab(int index);
private:
    std::vector<QPointer<QLabelEditGroup>> AWBedit;
    std::vector<QPointer<QLabelCheckBox>> AWBbox;
    std::vector<QPointer<QLabelEditGroup>> Greenedit;
    std::vector<QPointer<QLabelCheckBox>> Greenbox;
    std::vector<QPointer<QLabelEditGroup>> Lowedit;
    QPointer<QLabelComboBox> combox=nullptr;
    QPointer<QLabelComboBox> comboxLFSTrans=nullptr;
    QPointer<QLabelEditGroup> CurCTEidt=nullptr;

    std::vector<QPointer<QLabelEditGroup>> LFSedit;
    std::vector<QPointer<QLabelCheckBox>> LFSbox;
    QPointer<QTabWidget> tabWidget = nullptr;
    std::set<uint32_t> regAddrModule;
    // std::vector<uint32_t> registerMoudle;

};

#endif
