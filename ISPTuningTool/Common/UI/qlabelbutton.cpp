#include "qlabelbutton.h"
#include "ui_qlabelbutton.h"

QLabelButton::QLabelButton(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::QLabelButton)
{
    ui->setupUi(this);
    label=std::make_shared<QLabel>();
    pushButton=std::make_shared<QPushButton>();
    init();
}

QLabelButton::~QLabelButton()
{
    delete ui;
}

void QLabelButton::init()
{
    onstr="ON";
    offstr="OFF";
    pushButton->setCheckable(true);
    pushButton->setText(offstr);
    pushButton->setMaximumWidth(150);
    label->setText("label");
    QFont font;
    font.setBold(false);
    label->setFont(font);
    ui->horizontalLayout->addWidget(label.get());
    ui->horizontalLayout->addWidget(pushButton.get());
    initConnect();
}

void QLabelButton::initConnect()
{
    connect(pushButton.get(),&QPushButton::clicked,this,&QLabelButton::btnClick);
}

void QLabelButton::setLabel(QString str)
{
    label->setText(str);
}

void QLabelButton::setValue()
{

}

void QLabelButton::getValue()
{

}

void QLabelButton::setONOFF(QString on, QString off)
{
    onstr=on;
    offstr=off;
    pushButton->setText(on);
}

void QLabelButton::setChecked(bool isChecked)
{
    pushButton->setChecked(isChecked);
    if(isChecked){
        pushButton->setText(onstr);
    }else{
        pushButton->setText(offstr);
    }
}

void QLabelButton::btnClick(bool flag)
{
    if(flag){
        pushButton->setText(onstr);
        emit clickSignal(true);
    }else{
        pushButton->setText(offstr);
        emit clickSignal(false);
    }
}
