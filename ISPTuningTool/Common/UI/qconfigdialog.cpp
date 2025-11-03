#include "qconfigdialog.h"
#include "Common/Others/qconfigfiletrans.h"
#include "ui_qconfigdialog.h"

#include <QColorDialog>
#include <QFileDialog>

QConfigDialog::QConfigDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::QConfigDialog)
{
    ui->setupUi(this);
    this->setWindowTitle("Config");
    box=new QComboBox;
    box->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
    loadBtn=new QToolButton();
    loadBtn->setText("Open File");
    loadBtn->setMaximumWidth(150);
    colorBtn=new QToolButton();
    colorBtn->setMaximumWidth(150);
    colorBtn->setIconSize(QSize(20,20));
    colorEdit=new QLineEdit;
    colorEdit->setMinimumWidth(100);
    fontColorBtn=new QToolButton();
    fontColorBtn->setMaximumWidth(150);
    fontColorBtn->setIconSize(QSize(20,20));
    fontColorEdit=new QLineEdit;
    fontColorEdit->setMinimumWidth(100);
    pathEdit=new QLineEdit;
    pathEdit->setEnabled(false);
    pathEdit->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
    /*borderRadiusEdit=new QLineEdit();
    borderRadiusEdit->setMinimumWidth(100);
    QIntValidator *validator = new QIntValidator(0, 999999, this);
    borderRadiusEdit->setValidator(validator);
    borderRadiusBtn=new QToolButton();
    borderRadiusBtn->setMaximumWidth(150);
    borderRadiusBtn->setText("Confirm");*/
    borderRadiusSlider=new QSlider(Qt::Horizontal);
    borderRadiusSlider->setRange(0, 10);
    borderRadiusSpinBox=new QSpinBox;
    borderRadiusSpinBox->setRange(0,10);
    borderColorBtn=new QToolButton();
    borderColorBtn->setMaximumWidth(150);
    borderColorBtn->setIconSize(QSize(20,20));
    borderColorEdit=new QLineEdit;
    borderColorEdit->setMinimumWidth(100);
    imagePathBtn=new QToolButton();
    imagePathBtn->setMaximumWidth(150);
    imagePathBtn->setIconSize(QSize(20,20));
    imagePathEdit=new QLineEdit;
    imagePathEdit->setMinimumWidth(100);
    ui->gridLayout->addWidget(new QLabel("Path"),0,0);
    ui->gridLayout->addWidget(pathEdit,0,1);
    ui->gridLayout->addWidget(loadBtn,0,2);
    ui->gridLayout->addWidget(new QLabel("Type"),1,0);
    ui->gridLayout->addWidget(box,1,1);
    ui->gridLayout->addWidget(new QLabel("Background Color"),2,0);
    ui->gridLayout->addWidget(colorEdit,2,1);
    ui->gridLayout->addWidget(colorBtn,2,2);
    ui->gridLayout->addWidget(new QLabel("Font Color"),3,0);
    ui->gridLayout->addWidget(fontColorEdit,3,1);
    ui->gridLayout->addWidget(fontColorBtn,3,2);
    ui->gridLayout->addWidget(new QLabel("Border Radius"),4,0);
    ui->gridLayout->addWidget(borderRadiusSlider,4,1);
    ui->gridLayout->addWidget(borderRadiusSpinBox,4,2);
    ui->gridLayout->addWidget(new QLabel("Border Color"),5,0);
    ui->gridLayout->addWidget(borderColorEdit,5,1);
    ui->gridLayout->addWidget(borderColorBtn,5,2);
    ui->gridLayout->addWidget(new QLabel("Image Path"),6,0);
    ui->gridLayout->addWidget(imagePathEdit,6,1);
    ui->gridLayout->addWidget(imagePathBtn,6,2);
    connect(colorBtn,&QToolButton::clicked,this,&QConfigDialog::showColorDialog);
    connect(fontColorBtn,&QToolButton::clicked,this,&QConfigDialog::showFontColorDialog);
    connect(loadBtn,&QToolButton::clicked,this,&QConfigDialog::showStyleConfig);
    connect(colorEdit,&QLineEdit::returnPressed,this,[=](){
        if(QColor(colorEdit->text()).isValid()){
            QPixmap pix_color(50,50);
            pix_color.fill(QColor(colorEdit->text()));
            colorBtn->setIcon(QIcon(pix_color));
            uiInfoMap[box->currentText()].backGroundColor=colorEdit->text();
            //保存更新配置文件
            saveConfigStyle();
        }
    });
    connect(fontColorEdit,&QLineEdit::returnPressed,this,[=](){
        if(QColor(fontColorEdit->text()).isValid()){
            QPixmap pix_color(50,50);
            pix_color.fill(QColor(fontColorEdit->text()));
            fontColorBtn->setIcon(QIcon(pix_color));
            uiInfoMap[box->currentText()].fontColor=fontColorEdit->text();
            //保存更新配置文件
            saveConfigStyle();
        }
    });
    /*connect(borderRadiusEdit,&QLineEdit::returnPressed,this,[=](){
        if(borderRadiusEdit->text()!=""){
            uiInfoMap[box->currentText()].radius=borderRadiusEdit->text().toInt();
            //保存更新配置文件
            saveConfigStyle();
        }
    });
    connect(borderRadiusBtn,&QToolButton::pressed,this,[=](){
        if(borderRadiusEdit->text()!=""){
            uiInfoMap[box->currentText()].radius=borderRadiusEdit->text().toInt();
            //保存更新配置文件
            saveConfigStyle();
        }
    });*/
    connect(borderRadiusSlider,&QSlider::valueChanged,this,[=](){
        uiInfoMap[box->currentText()].radius=borderRadiusSlider->value();
        borderRadiusSpinBox->setValue(borderRadiusSlider->value());
        //保存更新配置文件
        changeStyleForSlider();
    });
    connect(borderRadiusSpinBox,&QSpinBox::valueChanged,this,[=](){
        uiInfoMap[box->currentText()].radius=borderRadiusSpinBox->value();
        borderRadiusSlider->setValue(borderRadiusSpinBox->value());
        //保存更新配置文件
        changeStyleForSlider();
    });
    connect(borderColorEdit,&QLineEdit::returnPressed,this,[=](){
        if(QColor(borderColorEdit->text()).isValid()){
            QPixmap pix_color(50,50);
            pix_color.fill(QColor(borderColorEdit->text()));
            borderColorBtn->setIcon(QIcon(pix_color));
            uiInfoMap[box->currentText()].borderColor=borderColorEdit->text();
            //保存更新配置文件
            saveConfigStyle();
        }
    });
    connect(borderColorBtn,&QToolButton::clicked,this,&QConfigDialog::showBorderColorDialog);
    connect(imagePathBtn,&QToolButton::clicked,this,&QConfigDialog::changePic);
}

QConfigDialog::~QConfigDialog()
{
    delete ui;
}

void QConfigDialog::updateEdit()
{
    QString colorstr=uiInfoMap[box->currentText()].backGroundColor;
    colorBtn->setEnabled(true);
    colorEdit->setEnabled(true);
    colorEdit->setText(colorstr);
    QPixmap pix_color(50,50);
    if(colorstr=="")
    {
        pix_color.fill(QColor("transparent"));
        colorBtn->setIcon(QIcon(pix_color));
        colorBtn->setEnabled(false);
        colorEdit->setEnabled(false);
    }
    else
    {
        pix_color.fill(QColor(colorstr));
        colorBtn->setIcon(QIcon(pix_color));
    }

    QString fontColor=uiInfoMap[box->currentText()].fontColor;
    fontColorBtn->setEnabled(true);
    fontColorEdit->setEnabled(true);
    fontColorEdit->setText(fontColor);
    QPixmap font_color(50,50);
    if(fontColor=="")
    {
        pix_color.fill(QColor("transparent"));
        fontColorBtn->setIcon(QIcon(pix_color));
        fontColorBtn->setEnabled(false);
        fontColorEdit->setEnabled(false);
    }
    else
    {
        pix_color.fill(QColor(fontColor));
        fontColorBtn->setIcon(QIcon(pix_color));
    }

    int nRadius=uiInfoMap[box->currentText()].radius;
    if(nRadius>=0)
    {
        borderRadiusSlider->setEnabled(true);
        borderRadiusSpinBox->setEnabled(true);
        borderRadiusSlider->setValue(nRadius);
        borderRadiusSpinBox->setValue(nRadius);
        /*borderRadiusEdit->setEnabled(true);
        borderRadiusBtn->setEnabled(true);
        borderRadiusEdit->setText(QString::number(nRadius));*/
    }
    else
    {
        borderRadiusSlider->setEnabled(false);
        borderRadiusSpinBox->setEnabled(false);
        borderRadiusSlider->setValue(0);
        borderRadiusSpinBox->setValue(0);
        /*borderRadiusEdit->setText("");
        borderRadiusEdit->setEnabled(false);
        borderRadiusBtn->setEnabled(false);*/
    }

    QString borderColor=uiInfoMap[box->currentText()].borderColor;
    borderColorBtn->setEnabled(true);
    borderColorEdit->setEnabled(true);
    borderColorEdit->setText(borderColor);
    QPixmap border_color(50,50);
    if(borderColor=="")
    {
        border_color.fill(QColor("transparent"));
        borderColorBtn->setIcon(QIcon(border_color));
        borderColorBtn->setEnabled(false);
        borderColorEdit->setEnabled(false);
    }
    else
    {
        border_color.fill(QColor(borderColor));
        borderColorBtn->setIcon(QIcon(border_color));
    }

    imagePathBtn->setEnabled(true);
    imagePathEdit->setEnabled(true);
    if(box->currentText()!="QComboBox-down-arrow")
    {
        border_color.fill(QColor("transparent"));
        imagePathBtn->setIcon(QIcon(border_color));
        imagePathEdit->setText("");
        imagePathBtn->setEnabled(false);
        imagePathEdit->setEnabled(false);
    }
}

void QConfigDialog::showColorDialog()
{
    if(colorEdit->text()=="")
        return;
    QPointer<QConfigDialog> dialog=new QConfigDialog;
    QColor color=QColorDialog::getColor(Qt::white,this);
    if(color.isValid())
    {
        QPixmap pix_color(50,50);
        pix_color.fill(color);
        colorBtn->setIcon(QIcon(pix_color));
        colorEdit->setText("#"+QString::number(color.rgb(),16).remove(0,2));
        //保存更新配置文件
        saveConfigStyle();
    }
}

void QConfigDialog::showFontColorDialog()
{
    if(fontColorEdit->text()=="")
        return;
    QPointer<QConfigDialog> dialog=new QConfigDialog;
    QColor color=QColorDialog::getColor(Qt::white,this);
    if(color.isValid())
    {
        QPixmap pix_color(50,50);
        pix_color.fill(color);
        fontColorBtn->setIcon(QIcon(pix_color));
        fontColorEdit->setText("#"+QString::number(color.rgb(),16).remove(0,2));
        //保存更新配置文件
        saveConfigStyle();
    }
}
void QConfigDialog::showStyleConfig()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Load Style"), "", tr("All file(*.*)"));
    QString style;
    if (!fileName.isEmpty()){
        pathEdit->setText(fileName);
        uiInfoMap.clear();
        style=getConfigStyle(fileName);
        updateEdit();
        emit updateStyle(style);
        QString destPath=iniPathName() + "Config/M1_style.ini";
        if(QFile::exists(destPath))
        {
            QFile::remove(destPath);
        }
        QFile::copy(fileName,destPath);
    }
    connect(box,&QComboBox::currentIndexChanged,this,[=](){updateEdit();});
}

void QConfigDialog::changeStyleForSlider()
{
    QSettings setFile(iniPathName() + "Config/M1_style.ini",QSettings::IniFormat);
    QStringList groupList=setFile.childGroups();
    for(int i=0;i<groupList.size();i++){
        if(groupList.at(i)==box->currentText()){
            setFile.beginGroup(box->currentText());
            setFile.remove("background");
            setFile.setValue("background",colorEdit->text());
            setFile.remove("background-color");
            setFile.setValue("background-color",colorEdit->text());
            uiInfoMap[box->currentText()].backGroundColor=colorEdit->text();
            setFile.remove("color");
            setFile.setValue("color",fontColorEdit->text());
            uiInfoMap[box->currentText()].fontColor=fontColorEdit->text();
            if(uiInfoMap[box->currentText()].radius>=0)
            {
                setFile.remove("border-radius");
                setFile.setValue("border-radius",QString::number(borderRadiusSlider->value())+"px");
            }
            setFile.endGroup();
            break;
        }
    }
    QString style;
    for(int i=0;i<groupList.size();i++){
        QString groupstr;
        setFile.beginGroup(groupList.at(i));
        QString uiControlName=setFile.value("type").toString();
        uiInfoMap.insert(groupList.at(i),ConfigInfo());
        QString str;
        foreach(QString key,setFile.childKeys()){
            if(key=="type")
            {
                continue;
            }

            QString uiProperty=key;
            QString uiValue=setFile.value(key).toString();
            if(key=="image")
            {
                //只有图片走此分支
                str=str+uiProperty+": url("+iniPathName() + "Config/arrow.png);\n";
            }
            else
            {
                //其余所有情况走此分支
                str=str+uiProperty+":"+uiValue+";\n";
            }
            if(key=="background"||key=="background-color"){
                uiInfoMap[groupList.at(i)].backGroundColor=uiValue;
            }
            if(key=="border-radius"){
                uiValue=uiValue.left(uiValue.indexOf("p"));
                uiInfoMap[groupList.at(i)].radius=uiValue.toInt();
            }
            if(key=="color"){
                uiInfoMap[groupList.at(i)].fontColor=uiValue;
            }
            if(key=="border-color"){
                uiInfoMap[groupList.at(i)].borderColor=uiValue;
            }
        }
        groupstr=uiControlName+"{\n"+str+"}\n";
        style=style+groupstr;
        setFile.endGroup();
    }
    updateStyle(style);
}

void QConfigDialog::showBorderColorDialog()
{
    if(borderColorEdit->text()=="")
        return;
    QPointer<QConfigDialog> dialog=new QConfigDialog;
    QColor color=QColorDialog::getColor(Qt::white,this);
    if(color.isValid())
    {
        QPixmap pix_color(50,50);
        pix_color.fill(color);
        borderColorBtn->setIcon(QIcon(pix_color));
        borderColorEdit->setText("#"+QString::number(color.rgb(),16).remove(0,2));
        //保存更新配置文件
        saveConfigStyle();
    }
}

void QConfigDialog::changePic()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Choose Picture"), "", tr("PNG file(*.png)"));
    if(fileName.isEmpty())
    {
        return;
    }
    QString destPath=iniPathName() + "Config/arrow.png";
    if(QFile::exists(destPath))
    {
        QFile::remove(destPath);
    }
    QFile::copy(fileName,destPath);
    imagePathBtn->setIcon(QIcon(fileName));
    imagePathEdit->setText(fileName);
    //保存更新配置文件
    changeStyleForSlider();
}
//加载配置文件样式
QString QConfigDialog::getConfigStyle(QString filePath)
{
    int nIndex=0;
    if(box->count()>0)
    {
        nIndex=box->currentIndex();
    }
    box->clear();
    QString style;
    QSettings setFile(filePath,QSettings::IniFormat);
    QStringList groupList=setFile.childGroups();
    for(int i=0;i<groupList.size();i++){
        QString groupstr;
        setFile.beginGroup(groupList.at(i));
        QString uiControlName=setFile.value("type").toString();
        box->addItem(groupList.at(i));
        uiInfoMap.insert(groupList.at(i),ConfigInfo());
        QString str;
        foreach(QString key,setFile.childKeys()){
            if(key=="type")
            {
                continue;
            }

            QString uiProperty=key;
            QString uiValue=setFile.value(key).toString();
            if(key=="image")
            {
                //只有图片走此分支
                str=str+uiProperty+": url("+iniPathName() + "Config/arrow.png);\n";
            }
            else
            {
                //其余所有情况走此分支
                str=str+uiProperty+":"+uiValue+";\n";
            }
            if(key=="background"||key=="background-color"){
                uiInfoMap[groupList.at(i)].backGroundColor=uiValue;
            }
            if(key=="border-radius"){
                uiValue=uiValue.left(uiValue.indexOf("p"));
                uiInfoMap[groupList.at(i)].radius=uiValue.toInt();
            }
            if(key=="color"){
                uiInfoMap[groupList.at(i)].fontColor=uiValue;
            }
            if(key=="border-color"){
                uiInfoMap[groupList.at(i)].borderColor=uiValue;
            }
        }
        groupstr=uiControlName+"{\n"+str+"}\n";
        style=style+groupstr;
        setFile.endGroup();
    }
    box->setCurrentIndex(nIndex);
    return style;
}
//更新控件样式
void QConfigDialog::saveConfigStyle(QString filePath)
{
    if(!QFile(filePath).exists())
        return;
    QSettings setFile(filePath,QSettings::IniFormat);
    QStringList groupList=setFile.childGroups();
    for(int i=0;i<groupList.size();i++){
        if(groupList.at(i)==box->currentText()){
            setFile.beginGroup(box->currentText());
            setFile.remove("background");
            setFile.setValue("background",colorEdit->text());
            setFile.remove("background-color");
            setFile.setValue("background-color",colorEdit->text());
            uiInfoMap[box->currentText()].backGroundColor=colorEdit->text();
            setFile.remove("color");
            setFile.setValue("color",fontColorEdit->text());
            uiInfoMap[box->currentText()].fontColor=fontColorEdit->text();
            if(uiInfoMap[box->currentText()].radius>=0)
            {
                setFile.remove("border-radius");
                setFile.setValue("border-radius",QString::number(borderRadiusSlider->value())+"px");
            }
            setFile.remove("border-color");
            setFile.setValue("border-color",borderColorEdit->text());
            uiInfoMap[box->currentText()].borderColor=borderColorEdit->text();
            setFile.endGroup();
            break;
        }
    }
    emit updateStyle(getConfigStyle(filePath));
}

QString QConfigDialog::initStyle()
{
    QString style;
    QSettings setFile(iniPathName() + "Config/M1_style.ini",QSettings::IniFormat);
    QStringList groupList=setFile.childGroups();
    for(int i=0;i<groupList.size();i++){
        QString groupstr;
        setFile.beginGroup(groupList.at(i));
        QString uiControlName=setFile.value("type").toString();
        QString str;
        foreach(QString key,setFile.childKeys()){
            if(key=="type")
            {
                continue;
            }

            QString uiProperty=key;
            QString uiValue=setFile.value(key).toString();
            if(key=="image")
            {
                //只有图片走此分支
                str=str+uiProperty+": url("+iniPathName() + "Config/arrow.png);\n";
            }
            else
            {
                //其余所有情况走此分支
                str=str+uiProperty+":"+uiValue+";\n";
            }
        }
        groupstr=uiControlName+"{\n"+str+"}\n";
        style=style+groupstr;
        setFile.endGroup();
    }
    return style;
}
