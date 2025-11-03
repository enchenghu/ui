#ifndef QGPIOSET_H
#define QGPIOSET_H

#include <QWidget>

namespace Ui {
class QGPIOSet;
}

class QGPIOSet : public QWidget
{
    Q_OBJECT

public:
    explicit QGPIOSet(QWidget *parent = nullptr);
    ~QGPIOSet();
    void init();
    void initConnect();
private:

    bool GetGpioEn(int gpioIndex);
    int  GetGpioMode(int gpioIndex);
    int  GetGpioPinValue(int gpioIndex);

    void GpioInputEn(int gpioIndex);
    void GpioOutputEn(int gpioIndex);
    void SetGpioPinValue(int gpioIndex,int value);
    void UpdateGPIO();

    int CurGpioIndex(QString str);
private slots:

    void on_GPIO0comboBox_currentTextChanged(const QString &arg1);

    void on_GPIO1comboBox_currentTextChanged(const QString &arg1);

    void on_GPIO2comboBox_currentTextChanged(const QString &arg1);

    void on_GPIO3comboBox_currentTextChanged(const QString &arg1);

    void on_GPIO4comboBox_currentTextChanged(const QString &arg1);

    void on_GPIO5comboBox_currentTextChanged(const QString &arg1);

    void on_GPIO6comboBox_currentTextChanged(const QString &arg1);

    void on_GPIO7comboBox_currentTextChanged(const QString &arg1);

    void on_GPIO8comboBox_currentTextChanged(const QString &arg1);

    void on_GPIO9comboBox_currentTextChanged(const QString &arg1);

    void on_GPIO10comboBox_currentTextChanged(const QString &arg1);

    void on_UpdateButton_clicked();

    void on_SaveGPIOButton_clicked();

private:
    Ui::QGPIOSet *ui;
};

#endif // QGPIOSET_H
