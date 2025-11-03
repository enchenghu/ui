#ifndef QCACTAB_H
#define QCACTAB_H

#include "Common/UI/qtabwndcomm.h"

class QCacTab : public QTabWndComm
{
    Q_OBJECT
public:
    QCacTab(QWidget *parent = nullptr);

private:
    QMap<QString,QPointer<QWidget>> tabMap;
    QPointer<QHBoxLayout>           subTabLayout;

    void init();
    void initLmSubTab(QString tabName);
    void initSOrLfsSubTab(QString tabName);
    void insertControl(QPointer<QGridLayout> layout, QPointer<QCommonUiInterface> widget, int row, int column,
        int rowSpan, int columnSpan);
    void insertControl(QPointer<QGridLayout> layout, QPointer<QCommonUiInterface> widget, int row, int column,
        int rowSpan, int columnSpan, int topDownMargin);
public slots:
    void reConnectReadExecute();
};

#endif // QCACTAB_H
