#ifndef QGBALANCE_H
#define QGBALANCE_H

#include "Common/UI/qtabwndcomm.h"
#include <QWidget>
#include <set>

class QGBalance : public QTabWndComm
{
    Q_OBJECT
public:
    explicit QGBalance(QWidget *parent = nullptr);
    ~QGBalance();
    void init();
    void initConnect();

public slots:
    void comboCurrentTypeValueChanged(int index);

private:
    QPointer<QComboBox> comboCurrentType = nullptr;
    QPointer<QWidget>   tabWidgetS       = nullptr;
    QPointer<QWidget>   tabWidgetLFS     = nullptr;
    std::set<uint32_t>  setAddrModule;

    void insertControl(QPointer<QGridLayout> layout, QPointer<QCommonUiInterface> widget, int row, int column,
        int rowSpan = 1, int columnSpan = 1);
    void generateSubTab(QPointer<QWidget> subTab, QString tabName);
    void getAllRegInfoInit();
};

#endif // QGBALANCE_H
