#ifndef QPUSHBUTTONGROUP_H
#define QPUSHBUTTONGROUP_H

#include <QPushButton>
#include <QWidget>

namespace Ui {
class QPushButtonGroup;
}

class QPushButtonGroup : public QWidget
{
    Q_OBJECT

public:
    explicit QPushButtonGroup(QWidget *parent = nullptr);
    ~QPushButtonGroup();
    void init();
    void initConnect();
signals:
    void readSignal();
    void loadSignal();
    void saveSignal();
private slots:
    void readBtnClick();
    void saveBtnClick();
    void loadBtnClick();

private:
    Ui::QPushButtonGroup *ui;
    std::shared_ptr<QPushButton> readBtn=nullptr;
    std::shared_ptr<QPushButton> loadBtn=nullptr;
    std::shared_ptr<QPushButton> saveBtn=nullptr;

};

#endif // QPUSHBUTTONGROUP_H
