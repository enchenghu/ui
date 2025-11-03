#ifndef QLABELBUTTON_H
#define QLABELBUTTON_H

#include <QLabel>
#include <QPushButton>
#include <QWidget>

namespace Ui {
class QLabelButton;
}

class QLabelButton : public QWidget
{
    Q_OBJECT

public:
    explicit QLabelButton(QWidget *parent = nullptr);
    ~QLabelButton();
    void init ();
    void initConnect();
    void setLabel(QString str);
    void setValue();
    void getValue();
    void setONOFF(QString on,QString off);
    void setChecked(bool isChecked);
signals:
    void clickSignal(bool flag);
private slots:
    void btnClick(bool flag);


private:
    Ui::QLabelButton *ui;
    std::shared_ptr<QLabel> label=nullptr;
    std::shared_ptr<QPushButton> pushButton=nullptr;
    QString onstr;
    QString offstr;
};

#endif // QLABELBUTTON_H
