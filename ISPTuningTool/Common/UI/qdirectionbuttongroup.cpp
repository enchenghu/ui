#include "qdirectionbuttongroup.h"
#include "ui_qdirectionbuttongroup.h"

QDirectionButtonGroup::QDirectionButtonGroup(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::QDirectionButtonGroup)
{
    ui->setupUi(this);
    btnUp = std::make_shared<QPushButton>();
    btnDown = std::make_shared<QPushButton>();
    btnLeft = std::make_shared<QPushButton>();
    btnRight = std::make_shared<QPushButton>();
    verticalLayoutRight = std::make_shared<QVBoxLayout>();
    init();
    initConnect();
    btnUp->setAutoRepeat(true);
    btnDown->setAutoRepeat(true);
    btnLeft->setAutoRepeat(true);
    btnRight->setAutoRepeat(true);
}

QDirectionButtonGroup::~QDirectionButtonGroup()
{
    delete ui;
}

void QDirectionButtonGroup::init()
{
    btnUp->setText("▲");
    btnDown->setText("▼");
    btnLeft->setText("◀");
    btnRight->setText("▶");
    btnUp->setFixedSize(30, 15);
    btnDown->setFixedSize(30, 15);
    btnLeft->setFixedSize(15, 30);
    btnRight->setFixedSize(15, 30);
    ui->horizontalLayout->addWidget(btnLeft.get());
    ui->horizontalLayout->addLayout(verticalLayoutRight.get());
    ui->horizontalLayout->addWidget(btnRight.get());
    ui->horizontalLayout->setContentsMargins(0, 0, 0, 0);
    ui->horizontalLayout->setSpacing(0);
    ui->horizontalLayout->setSizeConstraint(QLayout::SetFixedSize);
    verticalLayoutRight->addWidget(btnUp.get());
    verticalLayoutRight->addWidget(btnDown.get());
    verticalLayoutRight->setContentsMargins(0, 0, 0, 0);
    verticalLayoutRight->setSpacing(0);

    //设置符号大小
    QFont font;
    font.setPixelSize(20);
    btnLeft->setFont(font);
    btnRight->setFont(font);
    font.setPixelSize(15);
    btnUp->setFont(font);
    btnDown->setFont(font);
}

void QDirectionButtonGroup::initConnect()
{
    connect(btnUp.get(), SIGNAL(clicked()), this, SLOT(upBtnClick()));
    connect(btnDown.get(), SIGNAL(clicked()), this, SLOT(downBtnClick()));
    connect(btnLeft.get(), SIGNAL(clicked()), this, SLOT(leftBtnClick()));
    connect(btnRight.get(), SIGNAL(clicked()), this, SLOT(rightBtnClick()));
}

void QDirectionButtonGroup::showUpDown()
{
    btnLeft->setVisible(false);
    btnRight->setVisible(false);
    btnUp->setFixedSize(20,12);
    btnDown->setFixedSize(20,12);
    QFont font;
    font.setPointSize(5);
    btnDown->setFont(font);
    btnUp->setFont(font);
    btnUp->setText("∧");
    btnDown->setText("∨");

}

void QDirectionButtonGroup::upBtnClick()
{
    emit upSignal();
}

void QDirectionButtonGroup::downBtnClick()
{
    emit downSignal();
}

void QDirectionButtonGroup::leftBtnClick()
{
    emit leftSignal();
}

void QDirectionButtonGroup::rightBtnClick()
{
    emit rightSignal();
}
