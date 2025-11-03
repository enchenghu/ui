#include "qlabelcombobox.h"
#include "ui_qlabelcombobox.h"
#include <QMouseEvent>

QLabelComboBox::QLabelComboBox(QWidget *parent)
    : QCommonUiInterface(parent)
    , ui(new Ui::QLabelComboBox)
{
    ui->setupUi(this);
    label=std::make_shared<QLabel> ();
    comboBox=std::make_shared<QComboBox> ();
    init();
}

QLabelComboBox::~QLabelComboBox()
{
    delete ui;
}

void QLabelComboBox::init()
{
    ui->horizontalLayout->addWidget(label.get());
    ui->horizontalLayout->addWidget(comboBox.get());
    label->setText("Module:");
    QFont font;
    font.setBold(false);
    label->setFont(font);
    label->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Minimum);
    comboBox->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Minimum);
    comboBox->setMinimumWidth(80);
    comboBox->setMaximumWidth(150);
    comboBox->setMaximumHeight(30);
}

void QLabelComboBox::initConnect()
{
    // connect(comboBox.get(),&QComboBox::currentIndexChanged,this,&QLabelComboBox::recvValueChanged);
    connect(comboBox.get(),&QComboBox::activated,this,&QLabelComboBox::recvValueChanged);

}

void QLabelComboBox::setValue(QString lstr, QStringList cstr)
{
    label->setText(lstr);
    comboBox->clear();
    comboBox->addItems(cstr);
    initConnect();
}

QString QLabelComboBox::getValue(int index)
{
    if(index == -1)
        index = comboBox->currentIndex();
    return comboBox->itemText(index);
}

int QLabelComboBox::getCurrentIndex()
{
    return comboBox->currentIndex();
}

void QLabelComboBox::setCurrentIndex(int index)
{
    comboBox->setCurrentIndex(index);
}

void QLabelComboBox::recvValueChanged(int index)
{
    emit signalValueChange(comboBox->itemText(index));
    emit signalValueChange(index);
    emit indexChange(index);
    if (uiDataMiddleware && index < textRegValuePairList.size())
        uiDataMiddleware->writeValueToDevice(textRegValuePairList[index].second);
}

void QLabelComboBox::setLabel(QString str)
{
    label->setText(str);
    if (str.isEmpty())
        label->hide();
    else
        label->show();
}

void QLabelComboBox::setLabelFont(QFont font)
{
    label->setFont(font);
}

void QLabelComboBox::setMouseTracking(bool flag)
{
    isMouseTracking=flag;
}

QString QLabelComboBox::Label()
{
    return label->text();
}

void QLabelComboBox::mouseReleaseEvent(QMouseEvent *event)
{
    if(!isMouseTracking)
        return;
    if(event->button()==Qt::LeftButton){
        emit release(label->text()) ;
        if (uiDataMiddleware)
            emit releaseUiDataMiddleware(uiDataMiddleware);
    }
    QWidget::mousePressEvent(event);
}

void QLabelComboBox::setValue(uint32_t value)
{
    comboBox->setCurrentIndex(value);
}

uint32_t QLabelComboBox::getValue()
{
    return comboBox->currentIndex();
}

void QLabelComboBox::setUiDataMiddleware(const QPointer<QUiDataMiddleware> &value)
{
    QCommonUiInterface::setUiDataMiddleware(value);
    if (uiDataMiddleware)
        label->setText(uiDataMiddleware->getUiControlName());
}

void QLabelComboBox::setUiDataMiddleware(const QUiDataMiddleware &value)
{
    QCommonUiInterface::setUiDataMiddleware(value);
    if (uiDataMiddleware)
        label->setText(uiDataMiddleware->getUiControlName());
}

void QLabelComboBox::setTextRegValuePairList(QList<QPair<QString, uint32_t>> list)
{
    textRegValuePairList = list;
    QStringList strList;
    for (auto pair : list) {
        strList.append(pair.first);
    }
    setValue(label->text(), strList);
}

void QLabelComboBox::setComboBoxWidth(int width)
{
    comboBox->setMinimumWidth(width);
}

void QLabelComboBox::setSpacing(int spacing)
{
    ui->horizontalLayout->setSpacing(spacing);
}
