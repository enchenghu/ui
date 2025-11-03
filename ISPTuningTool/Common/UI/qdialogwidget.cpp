#include "qdialogwidget.h"
#include "ui_qdialogwidget.h"

QDialogWidget::QDialogWidget(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::QDialogWidget)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    setWindowModality(Qt::NonModal);
    this->setWindowOpacity(1);
    QFont font;
    font.setBold(false);
    font.setPointSize(12);
    ui->label->setFont(font);
}

void QDialogWidget::setTipName(QString str)
{
    ui->label->setText(str);
}

QDialogWidget::~QDialogWidget()
{
    delete ui;
}
