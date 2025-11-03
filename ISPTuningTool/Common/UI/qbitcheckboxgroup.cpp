        #include "qbitcheckboxgroup.h"
#include "qlabel.h"
#include "ui_qbitcheckboxgroup.h"


QBitCheckBoxGroup::QBitCheckBoxGroup(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::QBitCheckBoxGroup)
{
    ui->setupUi(this);
    init();
}

QBitCheckBoxGroup::~QBitCheckBoxGroup()
{
    delete ui;
}

void QBitCheckBoxGroup::init()
{
    for(int i=0;i<32;++i){//初始化最大16进制
        std::shared_ptr<QCheckBox> checkBox=std::make_shared<QCheckBox>();
        connect(checkBox.get(),&QCheckBox::clicked,this,[=](){updateValue();});
        checkBoxVector.push_back(checkBox);
        QVBoxLayout*vboxlayout=new QVBoxLayout();
        QLabel*label=new QLabel(QString::number(31-i));
        vboxlayout->addWidget(label);
        vboxlayout->setSpacing(0);
        vboxlayout->setContentsMargins(0,0,0,0);
        vboxlayout->addWidget(checkBox.get());
        vboxlayout->setAlignment(Qt::AlignCenter);
        checkBox->setVisible(false);
        label->setVisible(false);
        checkBoxLabelMap.insert(checkBox.get(),label);
        if(i<16){
            ui->gridLayout->addLayout(vboxlayout,1,i);
        }else{
            ui->gridLayout->addLayout(vboxlayout,0,i-16);
        }
    }
    this->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    changeBits(8);
}

void QBitCheckBoxGroup::setMargins(int left,int up,int right,int down)
{
    ui->gridLayout->setContentsMargins(left,up,right,down);
}

void QBitCheckBoxGroup::initConnect()
{

}

void QBitCheckBoxGroup::changeBits(int count)
{
    if(bitNochange)
        return;
    if(currentBits==count)
        return;
    for(int i=0;i<32;++i){
        checkBoxLabelMap.value(checkBoxVector[i].get())->setVisible(false);
        checkBoxVector[i].get()->setVisible(false);
        checkBoxVector[i].get()->setChecked(false);
    }
    if(count==8){
        for(int i=0;i<8;++i){
            checkBoxLabelMap.value(checkBoxVector[31-i].get())->setVisible(true);
            checkBoxVector[31-i].get()->setVisible(true);
        }
        ui->gridLayout->setContentsMargins(60,15,0,25);
    }
    if(count==16){
        for(int i=0;i<16;++i){
            checkBoxLabelMap.value(checkBoxVector[31-i].get())->setVisible(true);
            checkBoxVector[31-i].get()->setVisible(true);
        }
        ui->gridLayout->setContentsMargins(30,5,0,25);
    }
    if(count==32){
        for(int i=0;i<32;++i){
            checkBoxLabelMap.value(checkBoxVector[i].get())->setVisible(true);
            checkBoxVector[i].get()->setVisible(true);
        }
        ui->gridLayout->setContentsMargins(30,5,0,15);
    }
    currentBits=count;
}

void QBitCheckBoxGroup::setBits(int count)
{
    bitNochange=false;
    changeBits(count);
    bitNochange=true;
}

void QBitCheckBoxGroup::setValue(uint32_t value)
{
    QString str;
    QString ctr="0";
    str=str.setNum(value,2).rightJustified(32,'0');
    for(int i=0;i<str.length();++i){
        if(str[i]==ctr[0])
            checkBoxVector[i]->setChecked(false);
        else
            checkBoxVector[i]->setChecked(true);
    };
}

void QBitCheckBoxGroup::getValue()
{
    QString valuestr;
    for(int i=0;i<checkBoxVector.size();++i){
        if(checkBoxVector[i]->isChecked()){
            valuestr.append("1");
        }else{
            valuestr.append("0");
        }
    }
    uint32_t val=valuestr.toUInt(nullptr,2);
    emit signalValueChange(val);
}

void QBitCheckBoxGroup::setStyle(QString qss)
{

}

void QBitCheckBoxGroup::setEnableCheckBox(long long totalbitMask)
{
    QString defaultBit;
    QString ctr="1";
    defaultBit=defaultBit.setNum(totalbitMask,2).rightJustified(32, '0');
    for(int i=0;i<defaultBit.length();++i){
        if(defaultBit[i]==ctr[0])
            checkBoxVector[i]->setEnabled(true);
    };
}

void QBitCheckBoxGroup::setAllEnable(bool isEnable)
{
    for(int i=0;i<32;++i)
        checkBoxVector[i]->setEnabled(isEnable);
}

uint32_t QBitCheckBoxGroup::getCurrentEnableRange()
{
    QString valuestr;
    for(int i=0;i<32;++i){
        if(checkBoxVector[i]->isEnabled())
            valuestr.append("1");
        else
            valuestr.append("0");
    }
    uint32_t val=valuestr.toUInt(nullptr,2);
    return val;
}

void QBitCheckBoxGroup::setPainterPen(double width)
{
    penWidth=width;
    update();
}

void QBitCheckBoxGroup::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing,true);
    p.setRenderHints(QPainter::SmoothPixmapTransform,true);
    p.setPen(QPen(QColor("black"),penWidth));
    p.drawRoundedRect(QRectF(1,5,width()-5,height()-10),10,10,Qt::AbsoluteSize);
}

void QBitCheckBoxGroup::recvValueChanged(int value)
{
}

void QBitCheckBoxGroup::updateValue()
{
    getValue();//每次勾选，立即更新当前数值
}
