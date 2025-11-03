#ifndef QDIRECTIONBUTTONGROUP_H
#define QDIRECTIONBUTTONGROUP_H

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>

namespace Ui {
class QDirectionButtonGroup;
}

class QDirectionButtonGroup : public QWidget
{
    Q_OBJECT

public:
    explicit QDirectionButtonGroup(QWidget *parent = nullptr);
    ~QDirectionButtonGroup();
    void init();
    void initConnect();
    void showUpDown();
signals:
    void upSignal();
    void downSignal();
    void leftSignal();
    void rightSignal();
private slots:
    void upBtnClick();
    void downBtnClick();
    void leftBtnClick();
    void rightBtnClick();

private:
    Ui::QDirectionButtonGroup *ui;
    std::shared_ptr<QPushButton> btnUp = nullptr;
    std::shared_ptr<QPushButton> btnDown = nullptr;
    std::shared_ptr<QPushButton> btnLeft = nullptr;
    std::shared_ptr<QPushButton> btnRight = nullptr;
    std::shared_ptr<QVBoxLayout> layoutUpDown = nullptr;
    std::shared_ptr<QVBoxLayout> verticalLayoutRight;
};

#endif // QDIRECTIONBUTTONGROUP_H
