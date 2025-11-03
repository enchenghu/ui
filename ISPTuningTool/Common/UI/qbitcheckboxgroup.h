#ifndef QBITCHECKBOXGROUP_H
#define QBITCHECKBOXGROUP_H

#include "qlabel.h"
#include <QWidget>
#include <QCheckBox>
#include <QPainter>
#include <QColor>
#include <QGraphicsScene>
#include <QGraphicsView>
namespace Ui {
class QBitCheckBoxGroup;
}

class QBitCheckBoxGroup : public QWidget
{
    Q_OBJECT

public:
    explicit QBitCheckBoxGroup(QWidget *parent = nullptr);
    ~QBitCheckBoxGroup();
    void init();
    void setMargins(int left,int up,int right,int down);
    void initConnect();
    /**
     * @brief changeBits 更改进制
     * @param count 进制位数（0~count）
     * @return
     */
    void changeBits(int count);
    void setBits(int count);
    void setValue(uint32_t value);
    void getValue();
    void setStyle(QString qss);
    void setEnableCheckBox(long long totalbitMask);
    void setAllEnable(bool isEnable);
    uint32_t getCurrentEnableRange();
    void setPainterPen(double width);
protected:
    void paintEvent(QPaintEvent*event) override;
signals:
    void signalValueChange(uint32_t);
public slots:
    void recvValueChanged(int value);
    void updateValue();
public:
    int currentBits=0;
private:
    Ui::QBitCheckBoxGroup *ui;
    std::vector<std::shared_ptr<QCheckBox>> checkBoxVector;
    QMap<QCheckBox*,QLabel*> checkBoxLabelMap;
    bool bitNochange=false;
    double penWidth=0.5;
};

#endif // QBITCHECKBOXGROUP_H
