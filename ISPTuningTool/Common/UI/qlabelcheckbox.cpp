#include "qlabelcheckbox.h"
#include "Common/UI/qcommonuiinterface.h"
#include "qtabwndcomm.h"
#include "ui_qlabelcheckbox.h"

QLabelCheckBox::QLabelCheckBox(QWidget *parent)
    : QCommonUiInterface(parent)
    , ui(new Ui::QLabelCheckBox)
{
    ui->setupUi(this);
    label    = new QLabel();
    checkBox = new QCheckBox();
    init();
    initConnect();
}


QLabelCheckBox::~QLabelCheckBox()
{
    delete ui;
}

void QLabelCheckBox::setBold(bool isBold)
{
    QFont font;
    font.setBold(isBold);
    label->setFont(font);
}

void QLabelCheckBox::setMouseTracking(bool flag)
{
    isMouseTracking = flag;
}

void QLabelCheckBox::setLabel(QString str)
{
    label->setText(str);
}

void QLabelCheckBox::getLabel(QString &str)
{
    str = label->text();
}

QString QLabelCheckBox::Label()
{
    return label->text();
}

void QLabelCheckBox::setCheckable(bool isCheckable)
{
    checkBox->setCheckable(isCheckable);
    checkBox->setEnabled(isCheckable);
    label->setEnabled(isCheckable);
}

void QLabelCheckBox::setChecked(bool isChecked)
{
    checkBox->setChecked(isChecked);
}

void QLabelCheckBox::init()
{
    ui->horizontalLayout->addWidget(label.get());
    ui->horizontalLayout->addWidget(checkBox.get());
    label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    checkBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    checkBox->setMaximumWidth(150);
    label->setMinimumWidth(120);
    ui->horizontalLayout->setStretchFactor(label.get(),1);
    ui->horizontalLayout->setStretchFactor(checkBox.get(),2);

}

void QLabelCheckBox::initConnect()
{
    connect(checkBox, &QCheckBox::stateChanged, this, &QLabelCheckBox::recCheckBoxState);
    connect(checkBox, &QCheckBox::clicked, this, &QLabelCheckBox::recCheckBoxClicked);
}

void QLabelCheckBox::recCheckBoxState(int state)
{
    if (state == Qt::Checked) {
        emit checkSignal(label->text(), true);
    }
    if (state == Qt::Unchecked) {
        emit checkSignal(label->text(), false);
    }

    if (combineReg.size() > 0) {
        uint32_t value = ((state == Qt::Unchecked) ? false : true);
        setSensorValue(value);
    } else if (uiDataMiddleware && !uiDataMiddleware->getUiModuleName().isEmpty()) {
        uint32_t value = ((state == Qt::Unchecked) ? false : true);
        uiDataMiddleware->writeValueToDevice(value);
        emit releaseUiDataMiddleware(uiDataMiddleware);
    }
}

void QLabelCheckBox::recCheckBoxClicked()
{
    
    if (checkBox->isChecked()) {
        emit boxClicked(label->text(), true);
    }
    else {
        emit boxClicked(label->text(), false);
    }

    emit releaseUpadteCombineReg(combineReg);
}

void QLabelCheckBox::mouseReleaseEvent(QMouseEvent *event)
{
    if (!isMouseTracking)
        return;
    if (event->button() == Qt::LeftButton) {
        emit release(label->text());
        emit releaseUpadteCombineReg(combineReg);
        emit releaseUiDataMiddleware(uiDataMiddleware);
    }
    QWidget::mousePressEvent(event);
}

void QLabelCheckBox::setSensorValue(uint32_t value)
{
    if (combineReg.size() > 0) {
        checkBox->setChecked(((value == 0) ? false : true));
        for (int i = 0; i < combineReg.size(); i++)
            writeCombinedRegisterValue(combineReg[i], value);
    } else if (uiDataMiddleware && !uiDataMiddleware->getUiModuleName().isEmpty()) {
        uiDataMiddleware->writeValueToDevice(value);
    }
}
void QLabelCheckBox::getSensorValue(uint32_t &value)
{
    if (combineReg.size() > 0) {
        uint32_t valueTemp = 0;
        readCombinedRegisterValue(combineReg[0], valueTemp);
        value = valueTemp;
        checkBox->setChecked(((value == 0) ? false : true));
    } else if (uiDataMiddleware && !uiDataMiddleware->getUiModuleName().isEmpty()) {
        uiDataMiddleware->getValueFromDevice(value);
        checkBox->setChecked(((value == 0) ? false : true));
    }
}

void QLabelCheckBox::setCombineRegister(QString uiModuleName)
{
    QPointer<QUiDataMiddleware> data = nullptr;
    data                             = new QUiDataMiddleware(uiModuleName, label->text());
    auto combineList                 = data->getCombinedRegisterList();
    combineReg.clear();
    for (auto combine : combineList)
        combineReg.push_back(combine);
}

void QLabelCheckBox::setCombineRegister(CombinedRegister reg)
{
    combineReg.clear();
    combineReg.push_back(reg);
}

std::vector<CombinedRegister> QLabelCheckBox::getCombineRegister()
{
    std::vector<CombinedRegister> reg;
    reg.assign(combineReg.begin(), combineReg.end());
    return reg;
}

void QLabelCheckBox::setValue(uint32_t value)
{
    setChecked(value != 0);
}

uint32_t QLabelCheckBox::getValue()
{
    return checkBox->isChecked() ? 1 : 0;
}

bool QLabelCheckBox::checkState()
{
    if(checkBox->isChecked()){
        return true;
    }else{
        return false;
    }
}

void QLabelCheckBox::setValue(int value)
{
    if(value==0)
        checkBox->setChecked(false);
    if(value==1)
        checkBox->setChecked(true);
}

void QLabelCheckBox::setValue(QString str)
{
    int val=str.toInt(nullptr,16);
    if(val==0)
        checkBox->setChecked(false);
    if(val==1)
        checkBox->setChecked(true);
}

void QLabelCheckBox::setUiDataMiddleware(const QPointer<QUiDataMiddleware> &value)
{
    QCommonUiInterface::setUiDataMiddleware(value);
    if (uiDataMiddleware) {
        label->setText(uiDataMiddleware->getUiControlName());
    }
}

void QLabelCheckBox::setUiDataMiddleware(const QUiDataMiddleware &value)
{
    QCommonUiInterface::setUiDataMiddleware(value);
    if (uiDataMiddleware) {
        label->setText(uiDataMiddleware->getUiControlName());
    }
}

void QLabelCheckBox::setSpacing(int spacing)
{
    ui->horizontalLayout->setSpacing(spacing);
}
