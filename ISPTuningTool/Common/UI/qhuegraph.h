#ifndef QHUEGRAPH_H
#define QHUEGRAPH_H

#include "Common/UI/qcommonuiinterface.h"
#include <QLabel>
#include <QWidget>

class QHueGraph : public QCommonUiInterface
{
    Q_OBJECT
public:
    explicit QHueGraph(QWidget *parent = nullptr);
    void     setValue(uint32_t value);
    uint32_t getValue();
    void     setMouseTracking(bool flag);
    void     setUiDataMiddleware(const QPointer<QUiDataMiddleware> &value);
    void     setUiDataMiddleware(const QUiDataMiddleware &value);

protected:
    bool eventFilter(QObject *watched, QEvent *event);

private:
    bool              mouseTracking;
    QPointer<QWidget> toolTipWidget;
    QPointer<QLabel>  toolTipLabel;
    int               splitCount;

    void init();
    void paintEvent(QPaintEvent *event);
};

#endif // QHUEGRAPH_H
