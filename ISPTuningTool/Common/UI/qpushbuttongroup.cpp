#include "qpushbuttongroup.h"
#include "ui_qpushbuttongroup.h"

QPushButtonGroup::QPushButtonGroup(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::QPushButtonGroup)
{
    ui->setupUi(this);
    readBtn=std::make_shared<QPushButton>();
    loadBtn=std::make_shared<QPushButton>();
    saveBtn=std::make_shared<QPushButton>();
    init();
    initConnect();
}

QPushButtonGroup::~QPushButtonGroup()
{
    delete ui;
}

void QPushButtonGroup::init()
{
    readBtn->setText("Read");
    loadBtn->setText("Load");
    saveBtn->setText("Save");

    readBtn->setMinimumWidth(100);
    loadBtn->setMinimumWidth(100);
    saveBtn->setMinimumWidth(100);

    ui->horizontalLayout->addWidget(readBtn.get());
    ui->horizontalLayout->addWidget(loadBtn.get());
    ui->horizontalLayout->addWidget(saveBtn.get());
}

void QPushButtonGroup::initConnect()
{
    connect(loadBtn.get(),SIGNAL(clicked()),this,SLOT(loadBtnClick()));
    connect(readBtn.get(),SIGNAL(clicked()),this,SLOT(readBtnClick()));
    connect(saveBtn.get(),SIGNAL(clicked()),this,SLOT(saveBtnClick()));
}

void QPushButtonGroup::readBtnClick()
{
    emit readSignal();
}

void QPushButtonGroup::saveBtnClick()
{
    emit saveSignal();
}

void QPushButtonGroup::loadBtnClick()
{
    emit loadSignal();

}
