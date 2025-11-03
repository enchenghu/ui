#ifndef QDPC_H
#define QDPC_H

#include "Common/UI/qtabwndcomm.h"
#include <QWidget>
#include <Common/UI/qlabelcheckbox.h>
#include <set>

class QDPC : public QTabWndComm
{
    Q_OBJECT
public:
    explicit QDPC(QWidget *parent = nullptr);
    ~QDPC();
    void init();
    void initConnect();

public slots:
    void comboCurrentTypeValueChanged(int index);
    void recvItemDoubleClicked(QStandardItem *item, int row, int col);

private:
    QPointer<QComboBox>       comboCurrentType = nullptr;
    QPointer<QWidget>         tabWidgetLM      = nullptr;
    QPointer<QWidget>         tabWidgetS       = nullptr;
    QPointer<QWidget>         tabWidgetLFS     = nullptr;
    QPointer<QTableListGroup>                          tableList        = nullptr;
    std::vector<std::pair<CombinedRegister, uint32_t>> tableListInfo; // 用于dpc表格信息
    QPointer<QLabelCheckBox> _WPCBox=nullptr;
    QPointer<QLabelCheckBox> _BPCBox=nullptr;
    QPointer<QLabelCheckBox> _BWBox=nullptr;
    std::set<uint32_t>       setAddrModule;

    void insertControl(QPointer<QGridLayout> layout, QPointer<QCommonUiInterface> widget, int row, int column,
        int rowSpan = 1, int columnSpan = 1);
    void generateSubTab(QPointer<QWidget> subTab, QString tabName);
    void getAllRegInfoInit();
    void initTable();
    void readTableUpate();
    void readTable();
    void updateTable();
    void recordCombinedReg(QString moduleName, QString UIName, std::vector<CombinedRegister> &combinevector);
    void updateUIControl();
    QPointer<QCommonUiInterface> GainThr0=nullptr;
    QPointer<QCommonUiInterface> GainThr1=nullptr;

};

#endif // QDPC_H
