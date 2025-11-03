#include "qgpioset.h"
#include "ui_qgpioset.h"

#include <Common/Others/qusbcommunication.h>

#include <QDir>
#include <QFileDialog>
const QMap<QString, int> GPIOListValueMap = {
    {"[OUTPUT] spi_m2_cs[0]", 0},
    {"[OUTPUT] spi_m2_sck", 1},
    {"[OUTPUT] spi_m2_mosi[0]", 2},
    {"[OUTPUT] spi_m2_mosi[1]", 3},
    {"[OUTPUT] spi_m2_mosi[2]", 4},
    {"[OUTPUT] spi_m2_mosi[3]", 5},
    {"[OUTPUT] uart tx0", 6},
    {"[OUTPUT] uart tx1", 7},
    {"[OUTPUT] uart tx2", 8},
    {"[OUTPUT] pwm0", 9},
    {"[OUTPUT] pwm1", 10},
    {"[OUTPUT] pwm2", 11},
    {"[OUTPUT] spi s miso", 12},
    {"[OUTPUT] spi m1 cs[1]", 13},
    {"[OUTPUT] GPIO out", 14},
    {"[INPUT] gp_fsync", 15},
    {"[INPUT] spi_s_mosi", 16},
    {"[INPUT] spi_m2_miso[0]", 17},
    {"[INPUT] spi_m2_miso[1]", 18},
    {"[INPUT] spi_m2_miso[2]", 19},
    {"[INPUT] spi_m2_miso[3]", 20},
    {"[INPUT] uart rx0", 21},
    {"[INPUT] uart rx1", 22},
    {"[INPUT] uart rx2", 23},
    {"[INPUT] spi s cs", 24},
    {"[INPUT] spi s sck", 25},
    {"[INPUT] default", 26},
};

const QMap<int, QString> GPIOListNameMap = {
    {0,"[OUTPUT] spi_m2_cs[0]"},
    {1,"[OUTPUT] spi_m2_sck"},
    {2,"[OUTPUT] spi_m2_mosi[0]"},
    {3,"[OUTPUT] spi_m2_mosi[1]"},
    {4,"[OUTPUT] spi_m2_mosi[2]"},
    {5,"[OUTPUT] spi_m2_mosi[3]"},
    {6,"[OUTPUT] uart tx0"},
    {7,"[OUTPUT] uart tx1"},
    {8,"[OUTPUT] uart tx2"},
    {9,"[OUTPUT] pwm0"},
    {10,"[OUTPUT] pwm1"},
    {11,"[OUTPUT] pwm2"},
    {12,"[OUTPUT] spi s miso"},
    {13,"[OUTPUT] spi m1 cs[1]"},
    {14,"[OUTPUT] GPIO out"},
    {15,"[INPUT] gp_fsync"},
    {16,"[INPUT] spi_s_mosi"},
    {17,"[INPUT] spi_m2_miso[0]"},
    {18,"[INPUT] spi_m2_miso[1]"},
    {19,"[INPUT] spi_m2_miso[2]"},
    {20,"[INPUT] spi_m2_miso[3]"},
    {21,"[INPUT] uart rx0"},
    {22,"[INPUT] uart rx1"},
    {23,"[INPUT] uart rx2"},
    {24,"[INPUT] spi s cs"},
    {25,"[INPUT] spi s sck"},
    {26,"[INPUT] default"},
    };
QGPIOSet::QGPIOSet(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QGPIOSet)
{
    ui->setupUi(this);
    init();
    initConnect();
}

QGPIOSet::~QGPIOSet()
{
    delete ui;
}

void QGPIOSet::init()
{
    QStringList modelist = {"[OUTPUT] spi_m2_cs[0]", "[OUTPUT] spi_m2_sck","[OUTPUT] spi_m2_mosi[0]","[OUTPUT] spi_m2_mosi[1]","[OUTPUT] spi_m2_mosi[2]","[OUTPUT] spi_m2_mosi[3]","[OUTPUT] uart tx0","[OUTPUT] uart tx1"
                            ,"[OUTPUT] uart tx2","[OUTPUT] pwm0","[OUTPUT] pwm1","[OUTPUT] pwm2","[OUTPUT] spi s miso","[OUTPUT] spi m1 cs[1]","[OUTPUT] GPIO out"
        ,"[INPUT] gp_fsync","[INPUT] spi_s_mosi","[INPUT] spi_m2_miso[0]","[INPUT] spi_m2_miso[1]","[INPUT] spi_m2_miso[2]","[INPUT] spi_m2_miso[3]","[INPUT] uart rx0","[INPUT] uart rx1","[INPUT] uart rx2","[INPUT] spi s cs","[INPUT] spi s sck","[INPUT] default"};


    QStringList modelist2 = {"[OUTPUT] spi_m2_cs[0]", "[OUTPUT] spi_m2_sck","[OUTPUT] spi_m2_mosi[0]","[OUTPUT] uart tx0","[OUTPUT] uart tx1"
        ,"[OUTPUT] uart tx2","[OUTPUT] pwm0","[OUTPUT] pwm1","[OUTPUT] pwm2","[OUTPUT] spi s miso","[OUTPUT] spi m1 cs[1]","[OUTPUT] GPIO out"
        ,"[INPUT] gp_fsync","[INPUT] spi_s_mosi","[INPUT] spi_m2_miso[1]","[INPUT] uart rx0","[INPUT] uart rx1","[INPUT] uart rx2","[INPUT] spi s cs","[INPUT] spi s sck","[INPUT] default"};


    ui->GPIO0comboBox->addItems(modelist2);
    ui->GPIO1comboBox->addItems(modelist2);
    ui->GPIO2comboBox->addItems(modelist2);
    ui->GPIO3comboBox->addItems(modelist2);
    ui->GPIO4comboBox->addItems(modelist2);
    ui->GPIO5comboBox->addItems(modelist2);
    ui->GPIO6comboBox->addItems(modelist2);
    ui->GPIO7comboBox->addItems(modelist2);
    ui->GPIO8comboBox->addItems(modelist2);
    ui->GPIO9comboBox->addItems(modelist2);
    ui->GPIO10comboBox->addItems(modelist2);

    QStringList pinlist = {"0","1"};
    ui->Pin0ComboBox->addItems(pinlist);
    ui->Pin1ComboBox->addItems(pinlist);
    ui->Pin2ComboBox->addItems(pinlist);
    ui->Pin3ComboBox->addItems(pinlist);
    ui->Pin4ComboBox->addItems(pinlist);
    ui->Pin5ComboBox->addItems(pinlist);
    ui->Pin6ComboBox->addItems(pinlist);
    ui->Pin7ComboBox->addItems(pinlist);
    ui->Pin8ComboBox->addItems(pinlist);
    ui->Pin9ComboBox->addItems(pinlist);
    ui->Pin10ComboBox->addItems(pinlist);

    ui->widgetPin0->setVisible(false);
    ui->widgetPin1->setVisible(false);
    ui->widgetPin2->setVisible(false);
    ui->widgetPin3->setVisible(false);
    ui->widgetPin4->setVisible(false);
    ui->widgetPin5->setVisible(false);
    ui->widgetPin6->setVisible(false);
    ui->widgetPin7->setVisible(false);
    ui->widgetPin8->setVisible(false);
    ui->widgetPin9->setVisible(false);
    ui->widgetPin10->setVisible(false);
    UpdateGPIO();
}

void QGPIOSet::initConnect()
{
    connect(ui->Pin0ComboBox, &QComboBox::currentIndexChanged, this, [=](int index) {SetGpioPinValue(0,index); });
    connect(ui->Pin1ComboBox, &QComboBox::currentIndexChanged, this, [=](int index) {SetGpioPinValue(1,index); });
    connect(ui->Pin2ComboBox, &QComboBox::currentIndexChanged, this, [=](int index) {SetGpioPinValue(2,index); });
    connect(ui->Pin3ComboBox, &QComboBox::currentIndexChanged, this, [=](int index) {SetGpioPinValue(3,index); });
    connect(ui->Pin4ComboBox, &QComboBox::currentIndexChanged, this, [=](int index) {SetGpioPinValue(4,index); });
    connect(ui->Pin5ComboBox, &QComboBox::currentIndexChanged, this, [=](int index) {SetGpioPinValue(5,index); });
    connect(ui->Pin6ComboBox, &QComboBox::currentIndexChanged, this, [=](int index) {SetGpioPinValue(6,index); });
    connect(ui->Pin7ComboBox, &QComboBox::currentIndexChanged, this, [=](int index) {SetGpioPinValue(7,index); });
    connect(ui->Pin8ComboBox, &QComboBox::currentIndexChanged, this, [=](int index) {SetGpioPinValue(8,index); });
    connect(ui->Pin9ComboBox, &QComboBox::currentIndexChanged, this, [=](int index) {SetGpioPinValue(9,index); });
    connect(ui->Pin10ComboBox, &QComboBox::currentIndexChanged, this, [=](int index) {SetGpioPinValue(10,index); });

}

void QGPIOSet::GpioInputEn(int gpioIndex)
{
    if(gpioIndex>=0&&gpioIndex<=7)
    {
        unsigned char RegVal=0;
        QUsbCommunication::getInstance().readBurst(0x31003d16, &RegVal,4, 1);
        RegVal=(RegVal)&(~(0x1<<gpioIndex));
        QUsbCommunication::getInstance().writeBurst( 0x31003d16, (&RegVal),4, 1);
    }
    else if(gpioIndex>7&&gpioIndex<=10)
    {
        gpioIndex-=8;
        unsigned char RegVal=0;
        QUsbCommunication::getInstance().readBurst( 0x31003d17, &RegVal,4, 1);
        RegVal=(RegVal)&(~(0x1<<(gpioIndex+4)));
        QUsbCommunication::getInstance().writeBurst( 0x31003d17, (&RegVal),4, 1);
    }
}

void QGPIOSet::GpioOutputEn(int gpioIndex)
{
    if(gpioIndex>=0&&gpioIndex<=7)
    {
        unsigned char RegVal=0;
        QUsbCommunication::getInstance().readBurst(0x31003d16, &RegVal,4, 1);
        RegVal=(RegVal)|((0x1<<gpioIndex));
        QUsbCommunication::getInstance().writeBurst( 0x31003d16, (&RegVal),4, 1);
    }
    else if(gpioIndex>7&&gpioIndex<=10)
    {
        gpioIndex-=8;
        unsigned char RegVal=0;
        QUsbCommunication::getInstance().readBurst( 0x31003d17, &RegVal,4, 1);
        RegVal=(RegVal)|(0x1<<(gpioIndex+4));
        QUsbCommunication::getInstance().writeBurst( 0x31003d17, (&RegVal),4, 1);
    }
}

bool QGPIOSet::GetGpioEn(int gpioIndex)
{
    unsigned char RegVal=0;
    if(gpioIndex>=0&&gpioIndex<=7)
    {
        QUsbCommunication::getInstance().readBurst(0x31003d16, &RegVal,4, 1);
        RegVal=(RegVal)&((0x1<<gpioIndex));
    }
    else if(gpioIndex>7&&gpioIndex<=10)
    {
        gpioIndex-=8;
        QUsbCommunication::getInstance().readBurst(0x31003d17, &RegVal,4, 1);
        RegVal=(RegVal)&(0x1<<(gpioIndex+4));
    }
    if(RegVal>0x0)
        return true;//OUTPUT
    else
        return false;//INPUT
}

int QGPIOSet::GetGpioMode(int gpioIndex)
{
    int modeIndex=0;
    bool gpioEn=GetGpioEn(gpioIndex);
    unsigned int address=0x31003d23+gpioIndex;
    unsigned char RegVal=0;
    QUsbCommunication::getInstance().readBurst(address, &RegVal,4, 1);
    if(gpioEn)
    {//OUTPUT
        modeIndex=(RegVal&(0xf));
    }
    else
    {//INPUT
        modeIndex=(RegVal&(0xf0))>>4;
        modeIndex+=15;
    }
    return modeIndex;
}

int QGPIOSet::GetGpioPinValue(int gpioIndex)
{
    unsigned char RegVal=0;
    if(gpioIndex>=0&&gpioIndex<=7)
    {
        QUsbCommunication::getInstance().readBurst(0x31003d14, &RegVal,4, 1);
        RegVal=(RegVal)&((0x1<<gpioIndex));
    }
    else if(gpioIndex>7&&gpioIndex<=10)
    {
        gpioIndex-=8;
        QUsbCommunication::getInstance().readBurst(0x31003d15, &RegVal,4, 1);
        RegVal=(RegVal)&(0x1<<(gpioIndex+4));
    }
    return RegVal;
}

void QGPIOSet::SetGpioPinValue(int gpioIndex, int value)
{
    unsigned char RegVal=0;
    if(gpioIndex>=0&&gpioIndex<=7)
    {
        QUsbCommunication::getInstance().readBurst(0x31003d14, &RegVal,4, 1);
        if(value==0)
            RegVal=(RegVal)&(~(0x1<<gpioIndex));
        else
            RegVal=(RegVal)|((0x1<<gpioIndex));
        QUsbCommunication::getInstance().writeBurst( 0x31003d14, &RegVal,4, 1);


    }
    else if(gpioIndex>7&&gpioIndex<=10)
    {
        gpioIndex-=8;
        QUsbCommunication::getInstance().readBurst(0x31003d15, &RegVal,4, 1);
        if(value==0)
            RegVal=(RegVal)&(~(0x1<<(gpioIndex+4)));
        else
            RegVal=(RegVal)|(0x1<<(gpioIndex+4));

        QUsbCommunication::getInstance().writeBurst(0x31003d15, &RegVal,4, 1);

    }

    GpioOutputEn(gpioIndex);
}

void QGPIOSet::UpdateGPIO()
{
    // ui->GPIO0comboBox->setCurrentIndex(GetGpioMode(0));
    // ui->GPIO1comboBox->setCurrentIndex(GetGpioMode(1));
    // ui->GPIO2comboBox->setCurrentIndex(GetGpioMode(2));
    // ui->GPIO3comboBox->setCurrentIndex(GetGpioMode(3));
    // ui->GPIO4comboBox->setCurrentIndex(GetGpioMode(4));
    // ui->GPIO5comboBox->setCurrentIndex(GetGpioMode(5));
    // ui->GPIO6comboBox->setCurrentIndex(GetGpioMode(6));
    // ui->GPIO7comboBox->setCurrentIndex(GetGpioMode(7));
    // ui->GPIO8comboBox->setCurrentIndex(GetGpioMode(8));
    // ui->GPIO9comboBox->setCurrentIndex(GetGpioMode(9));
    // ui->GPIO10comboBox->setCurrentIndex(GetGpioMode(10));

    ui->GPIO0comboBox->setCurrentText(GPIOListNameMap.value(GetGpioMode(0)));
    ui->GPIO1comboBox->setCurrentText(GPIOListNameMap.value(GetGpioMode(1)));
    ui->GPIO2comboBox->setCurrentText(GPIOListNameMap.value(GetGpioMode(2)));
    ui->GPIO3comboBox->setCurrentText(GPIOListNameMap.value(GetGpioMode(3)));
    ui->GPIO4comboBox->setCurrentText(GPIOListNameMap.value(GetGpioMode(4)));
    ui->GPIO5comboBox->setCurrentText(GPIOListNameMap.value(GetGpioMode(5)));
    ui->GPIO6comboBox->setCurrentText(GPIOListNameMap.value(GetGpioMode(6)));
    ui->GPIO7comboBox->setCurrentText(GPIOListNameMap.value(GetGpioMode(7)));
    ui->GPIO8comboBox->setCurrentText(GPIOListNameMap.value(GetGpioMode(8)));
    ui->GPIO9comboBox->setCurrentText(GPIOListNameMap.value(GetGpioMode(9)));
    ui->GPIO10comboBox->setCurrentText(GPIOListNameMap.value(GetGpioMode(10)));


    ui->Pin0ComboBox->setCurrentIndex(GetGpioPinValue(0));
    ui->Pin1ComboBox->setCurrentIndex(GetGpioPinValue(1));
    ui->Pin2ComboBox->setCurrentIndex(GetGpioPinValue(2));
    ui->Pin3ComboBox->setCurrentIndex(GetGpioPinValue(3));
    ui->Pin4ComboBox->setCurrentIndex(GetGpioPinValue(4));
    ui->Pin5ComboBox->setCurrentIndex(GetGpioPinValue(5));
    ui->Pin6ComboBox->setCurrentIndex(GetGpioPinValue(6));
    ui->Pin7ComboBox->setCurrentIndex(GetGpioPinValue(7));
    ui->Pin8ComboBox->setCurrentIndex(GetGpioPinValue(8));
    ui->Pin9ComboBox->setCurrentIndex(GetGpioPinValue(9));
    ui->Pin10ComboBox->setCurrentIndex(GetGpioPinValue(10));
}

int QGPIOSet::CurGpioIndex(QString str)
{
    int index=0;
    index=GPIOListValueMap.value(str);
    return index;
}


void QGPIOSet::on_GPIO0comboBox_currentTextChanged(const QString &arg1)
{
    unsigned int address=0x31003d23;
    int index=-1;
    //index=ui->GPIO0comboBox->currentIndex();
    index=CurGpioIndex(ui->GPIO0comboBox->currentText());
    ui->widgetPin0->setVisible(false);
    if((index>=0)&&(index<=14))
    {//OutPut
        unsigned char RegVal=0;
        QUsbCommunication::getInstance().readBurst( address, &RegVal,4, 1);
        RegVal=(RegVal&(0xf0))|(index);
        QUsbCommunication::getInstance().writeBurst( address, (&RegVal),4, 1);
        if(index==14)
        {//GPIO
            ui->widgetPin0->setVisible(true);
            ui->Pin0ComboBox->setCurrentIndex(GetGpioPinValue(0));
        }
        else
            GpioOutputEn(0);
    }
    else if(index>14)
    {//InPut
        unsigned char RegVal=0;
        QUsbCommunication::getInstance().readBurst(address, &RegVal,4, 1);
        RegVal=(RegVal&(0xf))|((index-15)<<4);
        QUsbCommunication::getInstance().writeBurst( address, (&RegVal),4, 1);
        GpioInputEn(0);
    }
}


void QGPIOSet::on_GPIO1comboBox_currentTextChanged(const QString &arg1)
{
    unsigned int address=0x31003d24;
    int index=-1;
    //index=ui->GPIO1comboBox->currentIndex();
    index=CurGpioIndex(ui->GPIO1comboBox->currentText());
    ui->widgetPin1->setVisible(false);
    if((index>=0)&&(index<=14))
    {//OutPut
        unsigned char RegVal=0;
        QUsbCommunication::getInstance().readBurst(address, &RegVal,4, 1);
        RegVal=(RegVal&(0xf0))|(index);
        QUsbCommunication::getInstance().writeBurst( address, (&RegVal),4, 1);
        if(index==14)
        {//GPIO
            ui->widgetPin1->setVisible(true);
            ui->Pin1ComboBox->setCurrentIndex(GetGpioPinValue(1));
        }
        else
            GpioOutputEn(1);
    }
    else if(index>14)
    {//InPut
        unsigned char RegVal=0;
        QUsbCommunication::getInstance().readBurst(address, &RegVal,4, 1);
        RegVal=(RegVal&(0xf))|((index-15)<<4);
        QUsbCommunication::getInstance().writeBurst( address, (&RegVal),4, 1);
        GpioInputEn(1);
    }
}


void QGPIOSet::on_GPIO2comboBox_currentTextChanged(const QString &arg1)
{
    unsigned int address=0x31003d25;
    int index=-1;
    //index=ui->GPIO2comboBox->currentIndex();
    index=CurGpioIndex(ui->GPIO2comboBox->currentText());

    ui->widgetPin2->setVisible(false);

    if((index>=0)&&(index<=14))
    {//OutPut
        unsigned char RegVal=0;
        QUsbCommunication::getInstance().readBurst(address, &RegVal,4, 1);
        RegVal=(RegVal&(0xf0))|(index);
        QUsbCommunication::getInstance().writeBurst( address, (&RegVal),4, 1);
        if(index==14)
        {//GPIO
            ui->widgetPin2->setVisible(true);
            ui->Pin2ComboBox->setCurrentIndex(GetGpioPinValue(2));
        }
        else
            GpioOutputEn(2);
    }
    else if(index>14)
    {//InPut
        unsigned char RegVal=0;
        QUsbCommunication::getInstance().readBurst(address, &RegVal,4, 1);
        RegVal=(RegVal&(0xf))|((index-15)<<4);
        QUsbCommunication::getInstance().writeBurst( address, (&RegVal),4, 1);
        GpioInputEn(2);
    }
}


void QGPIOSet::on_GPIO3comboBox_currentTextChanged(const QString &arg1)
{
    unsigned int address=0x31003d26;
    int index=-1;
    //index=ui->GPIO3comboBox->currentIndex();
    index=CurGpioIndex(ui->GPIO3comboBox->currentText());

    ui->widgetPin3->setVisible(false);

    if((index>=0)&&(index<=14))
    {//OutPut
        unsigned char RegVal=0;
        QUsbCommunication::getInstance().readBurst(address, &RegVal,4, 1);
        RegVal=(RegVal&(0xf0))|(index);
        QUsbCommunication::getInstance().writeBurst( address, (&RegVal),4, 1);
        if(index==14)
        {//GPIO
            ui->widgetPin3->setVisible(true);
            ui->Pin3ComboBox->setCurrentIndex(GetGpioPinValue(3));
        }
        else
            GpioOutputEn(3);
    }
    else if(index>14)
    {//InPut
        unsigned char RegVal=0;
        QUsbCommunication::getInstance().readBurst( address, &RegVal,4, 1);
        RegVal=(RegVal&(0xf))|((index-15)<<4);
        QUsbCommunication::getInstance().writeBurst( address, (&RegVal),4, 1);
        GpioInputEn(3);
    }
}


void QGPIOSet::on_GPIO4comboBox_currentTextChanged(const QString &arg1)
{
    unsigned int address=0x31003d27;
    int index=-1;
    //index=ui->GPIO4comboBox->currentIndex();
    index=CurGpioIndex(ui->GPIO4comboBox->currentText());

    ui->widgetPin4->setVisible(false);

    if((index>=0)&&(index<=14))
    {//OutPut
        unsigned char RegVal=0;
        QUsbCommunication::getInstance().readBurst(address, &RegVal,4, 1);
        RegVal=(RegVal&(0xf0))|(index);
        QUsbCommunication::getInstance().writeBurst( address, (&RegVal),4, 1);
        if(index==14)
        {//GPIO
            ui->widgetPin4->setVisible(true);
            ui->Pin4ComboBox->setCurrentIndex(GetGpioPinValue(4));
        }
        else
            GpioOutputEn(4);
    }
    else if(index>14)
    {//InPut
        unsigned char RegVal=0;
        QUsbCommunication::getInstance().readBurst(address, &RegVal,4, 1);
        RegVal=(RegVal&(0xf))|((index-15)<<4);
        QUsbCommunication::getInstance().writeBurst( address, (&RegVal),4, 1);
        GpioInputEn(4);
    }
}


void QGPIOSet::on_GPIO5comboBox_currentTextChanged(const QString &arg1)
{
    unsigned int address=0x31003d28;
    int index=-1;
    //index=ui->GPIO5comboBox->currentIndex();
    index=CurGpioIndex(ui->GPIO5comboBox->currentText());

    ui->widgetPin5->setVisible(false);

    if((index>=0)&&(index<=14))
    {//OutPut
        unsigned char RegVal=0;
        QUsbCommunication::getInstance().readBurst(address, &RegVal,4, 1);
        RegVal=(RegVal&(0xf0))|(index);
        QUsbCommunication::getInstance().writeBurst( address, (&RegVal),4, 1);
        if(index==14)
        {//GPIO
            ui->widgetPin5->setVisible(true);
            ui->Pin5ComboBox->setCurrentIndex(GetGpioPinValue(5));
        }
        else
            GpioOutputEn(5);
    }
    else if(index>14)
    {//InPut
        unsigned char RegVal=0;
        QUsbCommunication::getInstance().readBurst(address, &RegVal,4, 1);
        RegVal=(RegVal&(0xf))|((index-15)<<4);
        QUsbCommunication::getInstance().writeBurst( address, (&RegVal),4, 1);
        GpioInputEn(5);
    }
}


void QGPIOSet::on_GPIO6comboBox_currentTextChanged(const QString &arg1)
{
    unsigned int address=0x31003d29;
    int index=-1;
    //index=ui->GPIO6comboBox->currentIndex();
    index=CurGpioIndex(ui->GPIO6comboBox->currentText());

    ui->widgetPin6->setVisible(false);
    if((index>=0)&&(index<=14))
    {//OutPut
        unsigned char RegVal=0;
        QUsbCommunication::getInstance().readBurst(address, &RegVal,4, 1);
        RegVal=(RegVal&(0xf0))|(index);
        QUsbCommunication::getInstance().writeBurst( address, (&RegVal),4, 1);
        if(index==14)
        {//GPIO
            ui->widgetPin6->setVisible(true);
            ui->Pin6ComboBox->setCurrentIndex(GetGpioPinValue(6));
        }
        else
            GpioOutputEn(6);
    }
    else if(index>14)
    {//InPut
        unsigned char RegVal=0;
        QUsbCommunication::getInstance().readBurst(address, &RegVal,4, 1);
        RegVal=(RegVal&(0xf))|((index-15)<<4);
        QUsbCommunication::getInstance().writeBurst( address, (&RegVal),4, 1);
        GpioInputEn(6);
    }
}


void QGPIOSet::on_GPIO7comboBox_currentTextChanged(const QString &arg1)
{
    unsigned int address=0x31003d2a;
    int index=-1;
    //index=ui->GPIO7comboBox->currentIndex();
    index=CurGpioIndex(ui->GPIO7comboBox->currentText());

    ui->widgetPin7->setVisible(false);

    if((index>=0)&&(index<=14))
    {//OutPut
        unsigned char RegVal=0;
        QUsbCommunication::getInstance().readBurst(address, &RegVal,4, 1);
        RegVal=(RegVal&(0xf0))|(index);
        QUsbCommunication::getInstance().writeBurst( address, (&RegVal),4, 1);
        if(index==14)
        {//GPIO
            ui->widgetPin7->setVisible(true);
            ui->Pin7ComboBox->setCurrentIndex(GetGpioPinValue(7));
        }
        else
            GpioOutputEn(7);
    }
    else if(index>14)
    {//InPut
        unsigned char RegVal=0;
        QUsbCommunication::getInstance().readBurst(address, &RegVal,4, 1);
        RegVal=(RegVal&(0xf))|((index-15)<<4);
        QUsbCommunication::getInstance().writeBurst( address, (&RegVal),4, 1);
        GpioInputEn(7);
    }
}


void QGPIOSet::on_GPIO8comboBox_currentTextChanged(const QString &arg1)
{
    unsigned int address=0x31003d2b;
    int index=-1;
    //index=ui->GPIO8comboBox->currentIndex();
    index=CurGpioIndex(ui->GPIO8comboBox->currentText());

    ui->widgetPin8->setVisible(false);

    if((index>=0)&&(index<=14))
    {//OutPut
        unsigned char RegVal=0;
        QUsbCommunication::getInstance().readBurst( address, &RegVal,4, 1);
        RegVal=(RegVal&(0xf0))|(index);
        QUsbCommunication::getInstance().writeBurst( address, (&RegVal),4, 1);
        if(index==14)
        {//GPIO
            ui->widgetPin8->setVisible(true);
            ui->Pin8ComboBox->setCurrentIndex(GetGpioPinValue(8));
        }
        else
            GpioOutputEn(8);
    }
    else if(index>14)
    {//InPut
        unsigned char RegVal=0;
        QUsbCommunication::getInstance().readBurst(address, &RegVal,4, 1);
        RegVal=(RegVal&(0xf))|((index-15)<<4);
        QUsbCommunication::getInstance().writeBurst( address, (&RegVal),4, 1);
        GpioInputEn(8);
    }
}


void QGPIOSet::on_GPIO9comboBox_currentTextChanged(const QString &arg1)
{
    unsigned int address=0x31003d2c;
    int index=-1;
    //index=ui->GPIO9comboBox->currentIndex();
    index=CurGpioIndex(ui->GPIO9comboBox->currentText());

    ui->widgetPin9->setVisible(false);

    if((index>=0)&&(index<=14))
    {//OutPut
        unsigned char RegVal=0;
        QUsbCommunication::getInstance().readBurst(address, &RegVal,4, 1);
        RegVal=(RegVal&(0xf0))|(index);
        QUsbCommunication::getInstance().writeBurst( address, (&RegVal),4, 1);

        if(index==14)
        {//GPIO
            ui->widgetPin9->setVisible(true);
            ui->Pin9ComboBox->setCurrentIndex(GetGpioPinValue(9));

        }
        else
            GpioOutputEn(9);
    }
    else if(index>14)
    {//InPut
        unsigned char RegVal=0;
        QUsbCommunication::getInstance().readBurst(address, &RegVal,4, 1);
        RegVal=(RegVal&(0xf))|((index-15)<<4);
        QUsbCommunication::getInstance().writeBurst( address, (&RegVal),4, 1);
        GpioInputEn(9);
    }
}


void QGPIOSet::on_GPIO10comboBox_currentTextChanged(const QString &arg1)
{
    unsigned int address=0x31003d2d;
    int index=-1;
    //index=ui->GPIO10comboBox->currentIndex();
    index=CurGpioIndex(ui->GPIO10comboBox->currentText());

    ui->widgetPin10->setVisible(false);

    if((index>=0)&&(index<=14))
    {//OutPut
        unsigned char RegVal=0;
        QUsbCommunication::getInstance().readBurst(address, &RegVal,4, 1);
        RegVal=(RegVal&(0xf0))|(index);
        QUsbCommunication::getInstance().writeBurst( address, (&RegVal),4, 1);
        if(index==14)
        {//GPIO
            ui->widgetPin10->setVisible(true);
            ui->Pin10ComboBox->setCurrentIndex(GetGpioPinValue(10));

        }
        else
            GpioOutputEn(10);
    }
    else if(index>14)
    {//InPut
        unsigned char RegVal=0;
        QUsbCommunication::getInstance().readBurst(address, &RegVal,4, 1);
        RegVal=(RegVal&(0xf))|((index-15)<<4);
        QUsbCommunication::getInstance().writeBurst( address, (&RegVal),4, 1);
        GpioInputEn(10);
    }
}


void QGPIOSet::on_UpdateButton_clicked()
{
    UpdateGPIO();
}


void QGPIOSet::on_SaveGPIOButton_clicked()
{
    std::vector<unsigned int> addressVec;
    addressVec.clear();
    for(int i=0;i<11;i++)
        addressVec.push_back(0x31003d23+i);
    addressVec.push_back(0x31003d14);
    addressVec.push_back(0x31003d15);
    addressVec.push_back(0x31003d16);
    addressVec.push_back(0x31003d17);

    QString fileSavePath = QFileDialog::getSaveFileName(this, tr("Save File"), "GPIORegList", tr("Text Files (*.txt)"));
    if (!fileSavePath.isEmpty()) {
        QFile file(fileSavePath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream txtOutput(&file);
            QString     line = "";
            for (int i = 0; i < addressVec.size(); i++) {
                uint8_t value = 0;
                QUsbCommunication::getInstance().readBurst(addressVec[i], &value, 4, 1);
                line = QString("0x%1").arg(
                           QString::number(addressVec[i], 16).toUpper().rightJustified(8, '0'))
                    + ","
                    + QString("0x%1").arg(QString::number(value, 16).toUpper().rightJustified(2, '0'))
                    + "\n";
                txtOutput << line;
            }
            file.close();
        }
    }
}

