#include "qtexteditgroup.h"
#include "ui_qtexteditgroup.h"

QTextEditGroup::QTextEditGroup(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::QTextEditGroup)
{
    ui->setupUi(this);
    textEdit=std::make_shared<QTextEdit>();
    label=std::make_shared<QLabel>();
    cleanButton=std::make_shared<QPushButton>();
    init();
    initConnect();
}

QTextEditGroup::~QTextEditGroup()
{
    delete ui;
}

void QTextEditGroup::init()
{
    ui->verticalLayout->addWidget(label.get());
    ui->verticalLayout->addWidget(textEdit.get());
    ui->verticalLayout->addWidget(cleanButton.get());
    ui->verticalLayout->setAlignment(cleanButton.get(),Qt::AlignRight);
    label->setText("Communication Log");
    cleanButton->setText("Clean All");
    this->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Expanding);
}

void QTextEditGroup::initConnect()
{
    connect(cleanButton.get(),SIGNAL(clicked()),this,SLOT(clean()));

}

void QTextEditGroup::setValue()
{

}

void QTextEditGroup::getValue()
{

}

void QTextEditGroup::recvValueChanged(QString log)
{
    textEdit->append(log);
}

void QTextEditGroup::recvValueChanged(int value)
{
}

void QTextEditGroup::clean()
{
    textEdit->clear();
    logCache="";
}

void QTextEditGroup::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing,true);
    p.setRenderHints(QPainter::SmoothPixmapTransform,true);
    p.setPen(QPen(QColor("black"),2));
    p.drawRect(QRectF(1,5,width()-2,height()-11));
}
