#ifndef QDIALOGWIDGET_H
#define QDIALOGWIDGET_H

#include <QDialog>

namespace Ui {
class QDialogWidget;
}

class QDialogWidget : public QDialog
{
    Q_OBJECT

public:
    explicit QDialogWidget(QWidget *parent = nullptr);
    void setTipName(QString str);
    ~QDialogWidget();

private:
    Ui::QDialogWidget *ui;
};

#endif // QDIALOGWIDGET_H
