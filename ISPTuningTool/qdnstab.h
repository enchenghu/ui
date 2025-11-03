#ifndef QDNSTAB_H
#define QDNSTAB_H

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
#include <QtConcurrent/QtConcurrent>
#include <set>

const QMap<QPair<QString, QString>, bool> DNS_RGBIR_ENABLE_MAP = {
    { {"DNS", "GainNode0L"}, true },
    { {"DNS", "GainNode1L"}, true },
    { {"DNS", "GainNode2L"}, true },
    { {"DNS", "GainNode3L"}, true },
    { {"DNS", "AddBack0L"}, true },
    { {"DNS", "AddBack1L"}, true },
    { {"DNS", "AddBack2L"}, true },
    { {"DNS", "AddBack3L"}, true },
    { {"DNS", "NoiseSlope0L"}, true },
    { {"DNS", "NoiseSlope1L"}, true },
    { {"DNS", "NoiseSlope2L"}, true },
    { {"DNS", "NoiseSlope3L"}, true },
    { {"DNS", "NoiseIntercept0L"}, true },
    { {"DNS", "NoiseIntercept1L"}, true },
    { {"DNS", "NoiseIntercept2L"}, true },
    { {"DNS", "NoiseIntercept3L"}, true },
    { {"DNS", "Edge0L"}, true },
    { {"DNS", "Edge1L"}, true },
    { {"DNS", "Edge2L"}, true },
    { {"DNS", "Edge3L"}, true },
    { {"DNS", "Text0L"}, true },
    { {"DNS", "Text1L"}, true },
    { {"DNS", "Text2L"}, true },
    { {"DNS", "Text3L"}, true },
    { {"DNS", "Flat0L"}, true },
    { {"DNS", "Flat1L"}, true },
    { {"DNS", "Flat2L"}, true },
    { {"DNS", "Flat3L"}, true },
    { {"DNS", "GainNode4YUV"}, true },
    { {"DNS", "GainNode5YUV"}, true },
    { {"DNS", "GainNode6YUV"}, true },
    { {"DNS", "GainNode7YUV"}, true },
    { {"DNS", "EdgeRat4YUV"}, true },
    { {"DNS", "EdgeRat5YUV"}, true },
    { {"DNS", "EdgeRat6YUV"}, true },
    { {"DNS", "EdgeRat7YUV"}, true },
    { {"DNS", "Node40YUV"}, true },
    { {"DNS", "Node41YUV"}, true },
    { {"DNS", "Node42YUV"}, true },
    { {"DNS", "Node43YUV"}, true },
    { {"DNS", "Node44YUV"}, true },
    { {"DNS", "Node45YUV"}, true },
    { {"DNS", "Node46YUV"}, true },
    { {"DNS", "Node47YUV"}, true },
    { {"DNS", "Node48YUV"}, true },
    { {"DNS", "Node49YUV"}, true },
    { {"DNS", "Node410YUV"}, true },
    { {"DNS", "Node411YUV"}, true },
    { {"DNS", "Node412YUV"}, true },
    { {"DNS", "Node413YUV"}, true },
    { {"DNS", "Node50YUV"}, true },
    { {"DNS", "Node51YUV"}, true },
    { {"DNS", "Node52YUV"}, true },
    { {"DNS", "Node53YUV"}, true },
    { {"DNS", "Node54YUV"}, true },
    { {"DNS", "Node55YUV"}, true },
    { {"DNS", "Node56YUV"}, true },
    { {"DNS", "Node57YUV"}, true },
    { {"DNS", "Node58YUV"}, true },
    { {"DNS", "Node59YUV"}, true },
    { {"DNS", "Node510YUV"}, true },
    { {"DNS", "Node511YUV"}, true },
    { {"DNS", "Node512YUV"}, true },
    { {"DNS", "Node513YUV"}, true },
    { {"DNS", "Node60YUV"}, true },
    { {"DNS", "Node61YUV"}, true },
    { {"DNS", "Node62YUV"}, true },
    { {"DNS", "Node63YUV"}, true },
    { {"DNS", "Node64YUV"}, true },
    { {"DNS", "Node65YUV"}, true },
    { {"DNS", "Node66YUV"}, true },
    { {"DNS", "Node67YUV"}, true },
    { {"DNS", "Node68YUV"}, true },
    { {"DNS", "Node69YUV"}, true },
    { {"DNS", "Node610YUV"}, true },
    { {"DNS", "Node611YUV"}, true },
    { {"DNS", "Node612YUV"}, true },
    { {"DNS", "Node613YUV"}, true },
    { {"DNS", "Node70YUV"}, true },
    { {"DNS", "Node71YUV"}, true },
    { {"DNS", "Node72YUV"}, true },
    { {"DNS", "Node73YUV"}, true },
    { {"DNS", "Node74YUV"}, true },
    { {"DNS", "Node75YUV"}, true },
    { {"DNS", "Node76YUV"}, true },
    { {"DNS", "Node77YUV"}, true },
    { {"DNS", "Node78YUV"}, true },
    { {"DNS", "Node79YUV"}, true },
    { {"DNS", "Node710YUV"}, true },
    { {"DNS", "Node711YUV"}, true },
    { {"DNS", "Node712YUV"}, true },
    { {"DNS", "Node713YUV"}, true }
};

class QDNSTab : public QTabWndComm
{
    Q_OBJECT
public:
    explicit QDNSTab(QWidget *parent = nullptr);
    ~QDNSTab();
    void recvEdit(QString str);
    // void recvEditShift(QString str);
    template <typename T>
    void updateUIValue(T& vector,const QString &extraname="");
    template <typename T>
    void updateUIValueLeftShift(T &vector, const QString &extraname = "", int iShiftBits = 0);
public slots:
    void changeRgbIrMode(int &newMode);
private:
    void init();
    void updateValue();
    void initConnect();
    std::vector<CombinedRegister> combinedRegisterSubPageRecording(QString strModuleName, QString strUiName, std::shared_ptr<int> ptrCamera, QString strSubPage);
    std::vector<CombinedRegister> getCombinedRegister(QString strModuleName, QString strUiName, std::shared_ptr<int> ptrCamera);
    void recordUIRegSubPageInfo();
    void recordUIRegister();
    void initRAWDNS(QPointer<QWidget> subTab);
    void initYUVDNS(QPointer<QWidget> subTab);
    void initCDNS(QPointer<QWidget> subTab);
    void init3DDNS(QPointer<QWidget> subTab);
    QString getStr();
    void readPoint(std::vector<QPoint> &points);
private slots:
    void changeTab(int index);
    void readCurve();
private:
    QPointer<QComboBox> typebox=nullptr;
    QPointer<QComboBox> YUVbox=nullptr;
    QPointer<QComboBox> CDNSbox=nullptr;
    std::vector<QPointer<QGroupBox>> RawL_Group;
    std::vector<QPointer<QGroupBox>> RawS_Group;
    std::vector<QPointer<QLabelEditGroup>> Raw_edit;
    std::vector<QPointer<QLabelEditGroup>> Raw_edit1;
    std::vector<QPointer<QLabelEditGroup>> Raw_edit2;
    std::vector<QPointer<QLabelCheckBox>> Raw_box;
    std::vector<QPointer<QLabelEditGroup>> YUV_edit;
    std::vector<QPointer<QLabelCheckBox>> YUV_box;
    std::vector<QPointer<QLabelEditGroup>> CDNS_edit;
    std::vector<QPointer<QLabelCheckBox>> CDNS_box;
    std::vector<QPointer<QLabelEditGroup>> _3D_edit;
    std::vector<QPointer<QLabelEditGroup>> _3D_edit_shift;
    std::vector<QPointer<QLabelCheckBox>> _3D_box;
    std::set<uint32_t> regAddrModule;
    QPointer<QCurveChartGroup>          YUVCurve = nullptr;
    QPointer<QCurveChartGroup>          CDNCurve = nullptr;
    QString curveType;
    QMap<QString,std::vector<QPoint>> curveMap;
    QMap<QString,std::vector<QPoint>> curveMap2;
    QPointer<QTabWidget> tabWidget = nullptr;
    QString strRgbIrMode = "RGB";
};

#endif
