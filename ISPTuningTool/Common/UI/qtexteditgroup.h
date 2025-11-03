#ifndef QTEXTEDITGROUP_H
#define QTEXTEDITGROUP_H

#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QWidget>
#include <QPainter>
namespace Ui {
class QTextEditGroup;
}

class QTextEditGroup : public QWidget
{
    Q_OBJECT

public:
    explicit QTextEditGroup(QWidget *parent = nullptr);
    ~QTextEditGroup();
    void init();
    void initConnect();
    void setValue();
    void getValue();
signals:
    void signalValueChange(int);
public slots:
    void recvValueChanged(QString log);
    void recvValueChanged(int value);

private slots:
    void clean();
protected:
    void paintEvent(QPaintEvent*event) override;

private:
    Ui::QTextEditGroup *ui;
    std::shared_ptr<QTextEdit> textEdit=nullptr;
    std::shared_ptr<QLabel> label=nullptr;
    std::shared_ptr<QPushButton> cleanButton=nullptr;
    QString logCache;
    int currentValue=0;


};

#endif // QTEXTEDITGROUP_H
