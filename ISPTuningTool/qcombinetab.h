#ifndef QCOMBINETAB_H
#define QCOMBINETAB_H

#include "Common/UI/qlabelcheckbox.h"
#include "Common/UI/qlabelcombobox.h"
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
class QCombineTab : public QTabWndComm
{
	Q_OBJECT
public:
	explicit QCombineTab(QWidget *parent = nullptr);
	~QCombineTab();

private:
    void init();
    void updateValue();
    void initConnect();
    void recordUIRegister();
    std::vector<CombinedRegister> combinedRegisterSubPageRecording(QString strModuleName, QString strUiName, std::shared_ptr<int> ptrCamera, QString strSubPage);
    std::vector<CombinedRegister> getCombinedRegister(QString strModuleName, QString strUiName, std::shared_ptr<int> ptrCamera);
    void recordUIRegSubPageInfo();
    void initCombine();
    void initHC();
    void initDarkColor();
    void initDoubleEdge();
    void initGhost();
    void initLFSLED();
    void loadHCTab();
    void writeCombine(int val);
    void appendCombineRow(uint32_t addr);
    int  readCombine(int flag);
    void updateValue(std::vector<QPointer<QLabelEditGroup>> edit,std::vector<QPointer<QLabelCheckBox>> box);

private slots:
	void changeTab(int index);

private:
    QPointer<QTabWidget> tabComm=nullptr;
    QMap<QString,std::pair<QWidget*,bool>> tabMap;
    QPointer<QLabelCheckBox> blooming_mode_en;//Combine
    QPointer<QLabelComboBox> dataSeL;
    QPointer<QLabelEditGroup> highMargn;
    std::vector<QPointer<QLabelEditGroup>> blooming_shif;
    std::vector<QPointer<QLabelEditGroup>> iso;//H&C
    QStringList hlist;
    QStringList complist;
    QPointer<QLabelCheckBox> CompS_en;
    QMap<std::pair<int,int>,QPointer<QLabelEditGroup>> histMap;
    QMap<std::pair<int,int>,QPointer<QLabelEditGroup>> compMap;
    QPointer<QGroupBox> box;
    QPointer<QGroupBox> box2;
    QPointer<QGridLayout> histLayout;
    QPointer<QGridLayout> compLayout;
    std::vector<QPointer<QLabelEditGroup>> dark_color_edit;//DarkColor
    std::vector<QPointer<QLabelCheckBox>> dar_color_box;
    std::vector<QPointer<QLabelEditGroup>> HC_edit;//DoubleEdge
    std::vector<QPointer<QLabelCheckBox>> HC_box;//DoubleEdge

    std::vector<QPointer<QLabelEditGroup>> doubleEdge_edit;//DoubleEdge
    std::vector<QPointer<QLabelCheckBox>> doubleEdge_box;
    std::vector<QPointer<QLabelEditGroup>> chost_edit;//Ghost
    std::vector<QPointer<QLabelCheckBox>> chost_box;
    std::vector<QPointer<QLabelEditGroup>> LFS_edit;//Ghost
    std::vector<QPointer<QLabelCheckBox>> LFS_box;
    std::set<uint32_t> regAddrModule;
};

#endif // QCCMTAB_H
