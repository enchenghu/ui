#include "qspinslidergroup.h"
#include "ui_qspinslidergroup.h"

QSpinSliderGroup::QSpinSliderGroup(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::QSpinSliderGroup)
{
    ui->setupUi(this);
    spinBox=std::make_shared<QDoubleSpinBox>();
    slider=std::make_shared<QSlider>();
    init();
}

QSpinSliderGroup::~QSpinSliderGroup()
{
    delete ui;
}

void QSpinSliderGroup::init()
{
    this->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Expanding);
    slider->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    slider->setOrientation(Qt::Orientation::Horizontal);
    ui->horizontalLayout->addWidget(spinBox.get());
    ui->horizontalLayout->addWidget(slider.get());
    slider->setValue(0);//设置默认值
    spinBox->setValue(0);
    spinBox->setMinimumWidth(80);
    spinBox->setMaximumWidth(150);
    slider->setMinimumWidth(150);
    slider->setMaximumWidth(200);
    initConnect();
}

void QSpinSliderGroup::setValue(uint32_t value)
{
    if(value>maxValue||value<minValue)
        return;
    if(stepfactor>1){
        int val=(uint32_t)(value/stepfactor);
        slider->setValue(val);
    }else{
        slider->setValue(value);
    }
    spinBox->setValue(value);
}

int QSpinSliderGroup::getValue()
{
    return spinBox->value();
}

std::shared_ptr<QSlider> QSpinSliderGroup::getSlider()
{
    if(slider.get()){
        return slider;
    }
    return std::shared_ptr<QSlider>();
}

void QSpinSliderGroup::initConnect()
{
    connect(slider.get(),&QSlider::sliderPressed,this,[=](){
        sendRegisterFlag=false;});
    connect(slider.get(),&QSlider::sliderReleased,this,[=](){
        sendRegisterFlag=true;
        emit signalValueChange((uint32_t)spinBox->value(),true);
    });
    connect(spinBox.get(),SIGNAL(valueChanged(double)),this,SLOT(recvValueChanged(double)));
    connect(slider.get(),SIGNAL(valueChanged(int)),this,SLOT(recvValueChanged(int)));
}

void QSpinSliderGroup::setValueRange(uint32_t minval, uint32_t maxval)
{
    if(maxval==4294967295){
        maxValue=maxval;
        minValue=minval;
        stepfactor=15;
        slider->setMaximum(286331153);
        slider->setMinimum(minval);
        slider->setPageStep(1);
        spinBox->setMaximum(maxval);
        spinBox->setMinimum(minval);
    }else{
        stepfactor=1;
        maxValue=maxval;
        minValue=minval;
        slider->setMaximum(maxval);
        slider->setMinimum(minval);
        spinBox->setMaximum(maxval);
        spinBox->setMinimum(minval);
        if(maxval<100)
            slider->setPageStep(1);
        else
            slider->setPageStep(10);
    }
}

void QSpinSliderGroup::setValueStep(int step)
{
    slider->setSingleStep(step);
    spinBox->setSingleStep(step);
}

void QSpinSliderGroup::recvValueChanged(int value)
{
    //对当前寄存器值同步更新
    if(sendRegisterFlag){
        emit signalValueChange((uint32_t)(value*stepfactor),true);
    }else{
        emit signalValueChange((uint32_t)(value*stepfactor),false);
    }
}

void QSpinSliderGroup::recvValueChanged(double value)
{
    if(sendRegisterFlag){
        emit signalValueChange((uint32_t)value,true);
    }
}


