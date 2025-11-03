#ifndef QSPINSLIDERGROUP_H
#define QSPINSLIDERGROUP_H

#include <QWidget>
#include <QSpinBox>
#include <QSlider>
#include <QMouseEvent>

namespace Ui {
class QSpinSliderGroup;
}

class QSpinSliderGroup : public QWidget
{
    Q_OBJECT
public:
    explicit QSpinSliderGroup(QWidget *parent = nullptr);
    ~QSpinSliderGroup();
    void init();
    void setValue(uint32_t value);
    int getValue();
    std::shared_ptr<QSlider> getSlider();
    void initConnect();
    /**
     * @brief 设置取值范围
     * @return
     */
    void setValueRange(uint32_t minval, uint32_t maxval);
    /**
     * @brief 设置取值步长
     * @return
     */
    void setValueStep(int step);
signals:
    void signalValueChange(uint32_t,bool);
    void signalValueChange(uint32_t);

public slots:
    void recvValueChanged(int value);
    void recvValueChanged(double value);

private:
    Ui::QSpinSliderGroup *ui;
    std::shared_ptr<QDoubleSpinBox> spinBox=nullptr;
    std::shared_ptr<QSlider> slider=nullptr;
    bool sendRegisterFlag=true;
    uint32_t maxValue=0;
    uint32_t minValue=0;
    int stepfactor=1;

};

#endif // QSPINSLIDERGROUP_H
