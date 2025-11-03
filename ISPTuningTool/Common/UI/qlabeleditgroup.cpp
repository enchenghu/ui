#include "qlabeleditgroup.h"
#include "ui_qlabeleditgroup.h"
#include <QMouseEvent>

QLabelEditGroup::QLabelEditGroup(QWidget *parent)
    : QCommonUiInterface(parent)
    , ui(new Ui::QLabelEditGroup)
{
    ui->setupUi(this);
    label    = std::make_shared<QLabel>();
    lineEdit = new QLineEdit();
    ui->horizontalLayout->addWidget(label.get());
    ui->horizontalLayout->addWidget(lineEdit.get());
    init();
    initConnect();
}

QLabelEditGroup::~QLabelEditGroup()
{
    delete ui;
}

void QLabelEditGroup::init()
{
    QFont font;
    font.setBold(false);
    label->setFont(font);
    label->setText(QString("寄存器参数:"));
    label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    lineEdit->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    lineEdit->setMaximumHeight(20);
    lineEdit->setMaximumWidth(150);
    label->setMinimumWidth(120);
    lineEdit->installEventFilter(this);
    ui->horizontalLayout->setStretchFactor(label.get(), 1);
    ui->horizontalLayout->setStretchFactor(lineEdit.get(), 2);
}

void QLabelEditGroup::initConnect()
{
    connect(lineEdit.get(), &QLineEdit::editingFinished, this, [=]() { recvValueChanged(); });
}

void QLabelEditGroup::setValue(QString editstr)
{
    lineEdit->setText(editstr);
}

void QLabelEditGroup::setLabelValue(QString labelstr)
{
    label->setText(labelstr);
}

void QLabelEditGroup::setEditWidth(int width)
{
    lineEdit->setMinimumWidth(width);
}

void QLabelEditGroup::setlabelWidth(int width)
{
    label->setMinimumWidth(width);
}

QString QLabelEditGroup::Label()
{
    return label->text();
}

QString QLabelEditGroup::value()
{
    return lineEdit->text();
}

void QLabelEditGroup::setLabelVisible(bool isVisible)
{
    label->setVisible(isVisible);
}

void QLabelEditGroup::getValue(QString &labelstr, QString &editstr)
{
    labelstr = label->text();
    editstr  = lineEdit->text();
}

void QLabelEditGroup::setEditable(bool flag)
{
    lineEdit->setReadOnly(!flag);
}

void QLabelEditGroup::setMouseTracking(bool flag)
{
    isMouseTracking = flag;
}

void QLabelEditGroup::recvValueChanged()
{
    if (label->text() == "Search") {
        if (lineEdit->text() == "") {
            emit signalValueChange(0);
            return;
        }
        Register _register = DatabaseOrm::getRegisterByAddress(lineEdit->text().toInt(nullptr, 16));
        if (_register.address == 0) {
            Register _register = DatabaseOrm::getRegisterByAddress(lineEdit->text().toInt(nullptr, 10));
            emit     signalValueChange(lineEdit->text().toUInt(nullptr, 10));
            return;
        }
    }

    if (combineReg.size() > 0 or uiDataMiddleware) {
        uint32_t value = lineEdit->text().toUInt(nullptr, 16);
        setSensorValue(value);
    }
    emit signalValueChange(lineEdit->text().toUInt(nullptr, 16));
    emit floatValueChange(lineEdit->text().toFloat());
}

void QLabelEditGroup::mouseReleaseEvent(QMouseEvent *event)
{
    if (!isMouseTracking)
        return;
    if (event->button() == Qt::LeftButton) {
        emit release();
        emit releaseLabel(label->text());
        emit releaseUiDataMiddleware(uiDataMiddleware);
        emit releaseUpadteCombineReg(combineReg);
    }
    QWidget::mousePressEvent(event);
}

bool QLabelEditGroup::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonRelease) {
        emit releaseLabel(label->text());
        emit release();
        emit releaseUiDataMiddleware(uiDataMiddleware);
        emit releaseUpadteCombineReg(combineReg);
    }
    return QWidget::eventFilter(obj, event);
}

void QLabelEditGroup::setLabelFont(QFont font)
{
    label->setFont(font);
}

void QLabelEditGroup::setSensorValue(uint32_t value)
{
    if (combineReg.size() > 0) {
        QString strValue = QString("0x%1").arg(QString::number(value, 16));
        lineEdit->setText(strValue);
        for (int i = 0; i < combineReg.size(); i++)
            writeCombinedRegisterValue(combineReg[i], value);
    } else if (uiDataMiddleware && !uiDataMiddleware->getUiModuleName().isEmpty()) {
        uiDataMiddleware->writeValueToDevice(value);
    }
}

void QLabelEditGroup::getSensorValue(uint32_t &value)
{
    if (combineReg.size() > 0) {
        uint32_t valueTemp = 0;
        readCombinedRegisterValue(combineReg[0], valueTemp);
        value            = valueTemp;
        QString strValue = QString("0x%1").arg(QString::number(value, 16));
        lineEdit->setText(strValue);
    } else if (uiDataMiddleware && !uiDataMiddleware->getUiModuleName().isEmpty()) {
        uiDataMiddleware->getValueFromDevice(value);
        QString strValue = QString("0x%1").arg(QString::number(value, 16));
        lineEdit->setText(strValue);
    }
}

void QLabelEditGroup::setCombineRegister(QString uiModuleName)
{
    QPointer<QUiDataMiddleware> data = nullptr;
    data                             = new QUiDataMiddleware(uiModuleName, label->text());
    auto combineList                 = data->getCombinedRegisterList();
    combineReg.clear();
    for (auto combine : combineList)
        combineReg.push_back(combine);
}

void QLabelEditGroup::setCombineRegister(CombinedRegister reg)
{
    combineReg.clear();
    combineReg.push_back(reg);
}

std::vector<CombinedRegister> QLabelEditGroup::getCombineRegister()
{
    std::vector<CombinedRegister> reg;
    reg.assign(combineReg.begin(), combineReg.end());
    return reg;
}

void QLabelEditGroup::setValue(uint32_t value)
{
    lineEdit->setText(QString("0x%1").arg(QString::number(value, 16)));
}

uint32_t QLabelEditGroup::getValue()
{
    return lineEdit->text().toUInt(nullptr, 16);
}

void QLabelEditGroup::setUiDataMiddleware(const QPointer<QUiDataMiddleware> &value)
{
    QCommonUiInterface::setUiDataMiddleware(value);
    if (uiDataMiddleware)
        label->setText(uiDataMiddleware->getUiControlName());
}

void QLabelEditGroup::setUiDataMiddleware(const QUiDataMiddleware &value)
{
    QCommonUiInterface::setUiDataMiddleware(value);
    if (uiDataMiddleware)
        label->setText(uiDataMiddleware->getUiControlName());
}

void QLabelEditGroup::setSpacing(int spacing)
{
    ui->horizontalLayout->setSpacing(spacing);
}
