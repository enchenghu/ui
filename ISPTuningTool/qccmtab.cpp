#include "qccmtab.h"
#include <QSplitter>
#include <QWidget>
#include <Common/UI/qtabwndcomm.h>


QCCMTab::QCCMTab(QWidget *parent)
{
    CCMEnable=new QLabelCheckBox();
    AutoNormal=new QLabelCheckBox();
    ManCCMEnable=new QLabelCheckBox();
    CTCurrent=new QLabelEditGroup();
    CTD=new QLabelEditGroup();
    CTM1=new QLabelEditGroup();
    CTM2=new QLabelEditGroup();
    CTA=new QLabelEditGroup();
    MANTable=new QTableListGroup();
    CCMDTable=new QTableListGroup();
    CCMTTable=new QTableListGroup();
    CCMATable=new QTableListGroup();
    moduleName="CCM";
    strDelRegListTxt = QCoreApplication::applicationDirPath() + "\\extracted_data\\CCM.txt";
    loadDeletedRegistersFromFile(strDelRegListTxt);
    init();
    initConnect();
}

void QCCMTab::init()
{
    subPageMap.insert(0, moduleName.c_str());

    QStringList strlist;
    strlist<<"MAN"<<"CCM D"<<"CCM T"<<"CCM A";
    tableMap.insert(0,std::make_pair("ManCCM",MANTable));
    tableMap.insert(1,std::make_pair("CCMD",CCMDTable));
    tableMap.insert(2,std::make_pair("CCMT",CCMTTable));
    tableMap.insert(3,std::make_pair("CCMA",CCMATable));
    contentWidget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    CCMEnable->setLabel("CCMEnable");
    CCMEnable->setMouseTracking(true);
    contentLayout->addWidget(CCMEnable.get(),0,0);
    AutoNormal->setLabel("Auto Normalization");
    contentLayout->addWidget(AutoNormal.get(),1,0);
    ManCCMEnable->setLabel("ManCCMEnable");
    ManCCMEnable->setMouseTracking(true);
    contentLayout->addWidget(ManCCMEnable.get(),2,0);

    CTCurrent->setLabelValue("CT Current");
    CTCurrent->setEditable(false);
    CTCurrent->setMouseTracking(true);
    contentLayout->addWidget(CTCurrent.get(),1,1);
    CTD->setLabelValue("CTD");
    CTD->setEditable(false);
    CTD->setMouseTracking(true);
    contentLayout->addWidget(CTD.get(),2,1);
    CTM1->setLabelValue("CTM1");
    CTM1->setEditable(false);
    CTM1->setMouseTracking(true);
    contentLayout->addWidget(CTM1.get(),2,2);
    CTM2->setLabelValue("CTM2");
    CTM2->setEditable(false);
    CTM2->setMouseTracking(true);
    contentLayout->addWidget(CTM2.get(),2,3);
    CTA->setLabelValue("CTA");
    CTA->setEditable(false);
    CTA->setMouseTracking(true);
    contentLayout->addWidget(CTA.get(),2,4);
    contentLayout->setHorizontalSpacing(0);
    QGridLayout*hboxLayout=new QGridLayout;
    hboxLayout->setContentsMargins(0,20,0,0);
    hboxLayout->setVerticalSpacing(0);
    hboxLayout->setHorizontalSpacing(8);
    contentLayout->addLayout(hboxLayout,3,0,1,5);

    ccm_edit.push_back(CTCurrent);
    ccm_edit.push_back(CTD);
    ccm_edit.push_back(CTM1);
    ccm_edit.push_back(CTM2);
    ccm_edit.push_back(CTA);
    ccm_box.push_back(CCMEnable);
    ccm_box.push_back(ManCCMEnable);

    for(int i=0;i<tableMap.size();i++){
        if(i==1){
            QSplitter* line = new QSplitter(Qt::Vertical, this);
            line->setFrameShape(QFrame::VLine);
            line->setFixedWidth(2);
            line->setLineWidth(2);
            hboxLayout->addWidget(line,1,1);
        }
        QString tableName=tableMap.value(i).first;
        QPointer<QTableListGroup> tableListGroup=tableMap.value(i).second;
        tableListGroup->setSortingEnabled(false);
        tableListGroup->setEditTriggers(QAbstractItemView::NoEditTriggers);
        hboxLayout->addWidget(new QLabel(strlist.at(i)),0,(i==0?i:i+1));
        hboxLayout->addWidget(tableListGroup.get(),1,(i==0?i:i+1));
        tableListGroup->setAlternatingRowColors(false);
        tableListGroup->setStyle("QTableView{gridline-color:white;color:rgb(0,0,0);outline:0px;background-color:rgb(230,230,230);}"
                                 "QHeaderView::section{border:0px solid rgb(0,0,0);background-color:rgb(165,165,165);color:rgb(255,255,255);font:bold};"
                                 );
        tableListGroup->setMinimumHeight(210);
        tableListGroup->setMaximumHeight(350);
        tableListGroup->setHeader(QStringList()<<"R"<<"G"<<"B"<<"SUM");
        for(int j=0;j<4;j++)
            tableListGroup->horizontalHeader()->setSectionResizeMode(j,QHeaderView::ResizeMode::Stretch);
        for(int j=0;j<4;j++){
            for(int k=0;k<3;k++){
                tableListGroup->appendRow(k,j,"",Qt::AlignCenter);
                tableListGroup->setRowHeight(k,50);
                tableListGroup->setColumnWidth(j,50);
                tableListGroup->getItem(k,j)->setData(tableName,Qt::UserRole+3);
                if(j==3)
                    tableListGroup->getItem(k,j)->setEnabled(false);
            }
        }
        tableListGroup->setVerticalHeader(QStringList()<<"R"<<"G"<<"B");
        tableListGroup->setVerticalHeaderVisible(true);
        for(int j=0;j<3;j++)
            tableListGroup->verticalHeader()->setSectionResizeMode(j,QHeaderView::ResizeMode::Stretch);
        tableListGroup->horizontalHeader()->setMinimumHeight(40) ;
        tableListGroup->verticalHeader()->setDefaultAlignment(Qt::AlignCenter);
        tableListGroup->verticalHeader()->setMinimumWidth(40) ;
    }
    for(auto item:ccm_edit)
        connect(item,&QLabelEditGroup::releaseLabel,this,[=](){groupUIIndex=0;recvEdit(item->Label());});
    for(auto item:ccm_box){
        connect(item,&QLabelCheckBox::release,this,[=](){groupUIIndex=0;recvEdit(item->Label());});
        connect(item,&QLabelCheckBox::checkSignal,this,[=](){groupUIIndex=0;recvCheckBox(item->Label(),item->checkState());});
    }
    for(auto iter=tableMap.begin();iter!=tableMap.end();iter++){
        QPointer<QTableListGroup> table=iter.value().second;
        connect(table.get(),SIGNAL(sendCurrentItem(QStandardItem*,int,int)),this,SLOT(recvValueChanged(QStandardItem*,int,int)));
    }
    recordUIRegister();
    addRegisterTable();
    labelSpinSliderCheckBoxGroup->setRealEdit(true);
}

void QCCMTab::updateValue()
{
    updateUIValue<std::vector<QPointer<QLabelEditGroup>>>(ccm_edit);
    updateUIValue<std::vector<QPointer<QLabelCheckBox>>>(ccm_box);
    for(int i=0;i<tableMap.size();++i){
        QPointer<QTableListGroup> table=tableMap.value(i).second;
        if(UIRegisterMap.contains(tableMap.value(i).first))
        {
            std::vector<std::vector<QStringList>> uiVector=UIRegisterMap.value(tableMap.value(i).first);
            if(normalization){
                for(int i=0;i<3;i++){
                    float sum=0.0;
                    for(int j=0;j<3;j++){
                        float _value=0.0;
                        QStringList str=uiVector.at(i).at(j);
                        QString lstr=str.at(0);
                        QString hstr=str.at(1);
                        int laddr=0;
                        int haddr=0;
                        laddr=lstr.toInt(nullptr,16);
                        haddr=hstr.toInt(nullptr,16);
                        if(i!=j){
                            _value=getValuebyCombineBitAddr(laddr,haddr);//从寄存器获取
                            QString ss=QString::number(getFloatbyInt(_value),'f',2);
                            table->getItem(i,j)->setText(ss);
                            table->getItem(i,j)->setData(laddr,Qt::UserRole+1);
                            table->getItem(i,j)->setData(haddr,Qt::UserRole+2);
                            sum=sum+getFloatbyInt(_value);
                        }
                    }
                    QStringList str=uiVector.at(i).at(i);
                    QString lstr=str.at(0);
                    QString hstr=str.at(1);
                    int laddr=0;
                    int haddr=0;
                    laddr=lstr.toInt(nullptr,16);
                    haddr=hstr.toInt(nullptr,16);
                    setValuebyCombinedBit(getIntbyFloat(1.0-sum),laddr,haddr);//normlization模式下自动写入对角行寄存器
                    table->getItem(i,i)->setText(QString::number(1.0-sum,'f',2));
                    table->getItem(i,i)->setData(laddr,Qt::UserRole+1);
                    table->getItem(i,i)->setData(haddr,Qt::UserRole+2);
                    table->getItem(i,3)->setText(QString::number(1,'f',2));
                }
            }else{
                for(int i=0;i<3;i++){
                    float sumValue=0.0;
                    for(int j=0;j<3;j++){
                        int _value=0;
                        QStringList str=uiVector.at(i).at(j);
                        QString lstr=str.at(0);
                        QString hstr=str.at(1);
                        int laddr=0;
                        int haddr=0;
                        laddr=lstr.toInt(nullptr,16);
                        haddr=hstr.toInt(nullptr,16);
                        _value=getValuebyCombineBitAddr(laddr,haddr);
                        QString ss=QString::number(getFloatbyInt(_value),'f',2);
                        table->getItem(i,j)->setText(ss);
                        table->getItem(i,j)->setData(laddr,Qt::UserRole+1);
                        table->getItem(i,j)->setData(haddr,Qt::UserRole+2);
                        sumValue=sumValue+getFloatbyInt(_value);
                    }
                    table->getItem(i,3)->setText(QString::number(sumValue,'f',2));
                }
            }
        }
    }
  uint32_t gainB      = QUiDataMiddleware("CCM", "Current Gain B").readValueFromDevice();
  uint32_t gainR      = QUiDataMiddleware("CCM", "Current Gain R").readValueFromDevice();
  UINT _CTCurrent =0;
  if(gainB!=0&&gainR!=0){
     // _CTCurrent = (1.0*(gainB << 8))/(1.0*(gainR));
      double fCurCT = ((float)gainB) / ((float)gainR);
      _CTCurrent = (UINT)(fCurCT * 256);
  }
  CTCurrent->setValue(QString("0x%1").arg(QString::number(_CTCurrent, 16)));
}

void QCCMTab::updateValueEditGroup()
{
    updateUIValue<std::vector<QPointer<QLabelEditGroup>>>(ccm_edit);
    updateUIValue<std::vector<QPointer<QLabelCheckBox>>>(ccm_box);
    uint32_t gainB      = QUiDataMiddleware("CCM", "Current Gain B").readValueFromDevice();
    uint32_t gainR      = QUiDataMiddleware("CCM", "Current Gain R").readValueFromDevice();
    UINT _CTCurrent =0;
    if(gainB!=0&&gainR!=0){
        // _CTCurrent = (1.0*(gainB << 8))/(1.0*(gainR));
        double fCurCT = ((float)gainB) / ((float)gainR);
        _CTCurrent = (UINT)(fCurCT * 256);
    }
    CTCurrent->setValue(QString("0x%1").arg(QString::number(_CTCurrent, 16)));
}

void QCCMTab::updateValueMANMartix()
{
    const int i=0;
    QPointer<QTableListGroup> table=tableMap.value(i).second;
    if(UIRegisterMap.contains(tableMap.value(i).first))
    {
        std::vector<std::vector<QStringList>> uiVector=UIRegisterMap.value(tableMap.value(i).first);
        if(normalization){
            for(int i=0;i<3;i++){
                float sum=0.0;
                for(int j=0;j<3;j++){
                    float _value=0.0;
                    QStringList str=uiVector.at(i).at(j);
                    QString lstr=str.at(0);
                    QString hstr=str.at(1);
                    int laddr=0;
                    int haddr=0;
                    laddr=lstr.toInt(nullptr,16);
                    haddr=hstr.toInt(nullptr,16);
                    if(i!=j){
                        _value=getValuebyCombineBitAddr(laddr,haddr);//从寄存器获取
                        QString ss=QString::number(getFloatbyInt(_value),'f',2);
                        table->getItem(i,j)->setText(ss);
                        table->getItem(i,j)->setData(laddr,Qt::UserRole+1);
                        table->getItem(i,j)->setData(haddr,Qt::UserRole+2);
                        sum=sum+getFloatbyInt(_value);
                    }
                }
                QStringList str=uiVector.at(i).at(i);
                QString lstr=str.at(0);
                QString hstr=str.at(1);
                int laddr=0;
                int haddr=0;
                laddr=lstr.toInt(nullptr,16);
                haddr=hstr.toInt(nullptr,16);
                setValuebyCombinedBit(getIntbyFloat(1.0-sum),laddr,haddr);//normlization模式下自动写入对角行寄存器
                table->getItem(i,i)->setText(QString::number(1.0-sum,'f',2));
                table->getItem(i,i)->setData(laddr,Qt::UserRole+1);
                table->getItem(i,i)->setData(haddr,Qt::UserRole+2);
                table->getItem(i,3)->setText(QString::number(1,'f',2));
            }
        }else{
            for(int i=0;i<3;i++){
                float sumValue=0.0;
                for(int j=0;j<3;j++){
                    int _value=0;
                    QStringList str=uiVector.at(i).at(j);
                    QString lstr=str.at(0);
                    QString hstr=str.at(1);
                    int laddr=0;
                    int haddr=0;
                    laddr=lstr.toInt(nullptr,16);
                    haddr=hstr.toInt(nullptr,16);
                    _value=getValuebyCombineBitAddr(laddr,haddr);
                    QString ss=QString::number(getFloatbyInt(_value),'f',2);
                    table->getItem(i,j)->setText(ss);
                    table->getItem(i,j)->setData(laddr,Qt::UserRole+1);
                    table->getItem(i,j)->setData(haddr,Qt::UserRole+2);
                    sumValue=sumValue+getFloatbyInt(_value);
                }
                table->getItem(i,3)->setText(QString::number(sumValue,'f',2));
            }
        }
    }
}

void QCCMTab::updateValueCCMDMartix()
{
    const int i=1;
    QPointer<QTableListGroup> table=tableMap.value(i).second;
    if(UIRegisterMap.contains(tableMap.value(i).first))
    {
        std::vector<std::vector<QStringList>> uiVector=UIRegisterMap.value(tableMap.value(i).first);
        if(normalization){
            for(int i=0;i<3;i++){
                float sum=0.0;
                for(int j=0;j<3;j++){
                    float _value=0.0;
                    QStringList str=uiVector.at(i).at(j);
                    QString lstr=str.at(0);
                    QString hstr=str.at(1);
                    int laddr=0;
                    int haddr=0;
                    laddr=lstr.toInt(nullptr,16);
                    haddr=hstr.toInt(nullptr,16);
                    if(i!=j){
                        _value=getValuebyCombineBitAddr(laddr,haddr);//从寄存器获取
                        QString ss=QString::number(getFloatbyInt(_value),'f',2);
                        table->getItem(i,j)->setText(ss);
                        table->getItem(i,j)->setData(laddr,Qt::UserRole+1);
                        table->getItem(i,j)->setData(haddr,Qt::UserRole+2);
                        sum=sum+getFloatbyInt(_value);
                    }
                }
                QStringList str=uiVector.at(i).at(i);
                QString lstr=str.at(0);
                QString hstr=str.at(1);
                int laddr=0;
                int haddr=0;
                laddr=lstr.toInt(nullptr,16);
                haddr=hstr.toInt(nullptr,16);
                setValuebyCombinedBit(getIntbyFloat(1.0-sum),laddr,haddr);//normlization模式下自动写入对角行寄存器
                table->getItem(i,i)->setText(QString::number(1.0-sum,'f',2));
                table->getItem(i,i)->setData(laddr,Qt::UserRole+1);
                table->getItem(i,i)->setData(haddr,Qt::UserRole+2);
                table->getItem(i,3)->setText(QString::number(1,'f',2));
            }
        }else{
            for(int i=0;i<3;i++){
                float sumValue=0.0;
                for(int j=0;j<3;j++){
                    int _value=0;
                    QStringList str=uiVector.at(i).at(j);
                    QString lstr=str.at(0);
                    QString hstr=str.at(1);
                    int laddr=0;
                    int haddr=0;
                    laddr=lstr.toInt(nullptr,16);
                    haddr=hstr.toInt(nullptr,16);
                    _value=getValuebyCombineBitAddr(laddr,haddr);
                    QString ss=QString::number(getFloatbyInt(_value),'f',2);
                    table->getItem(i,j)->setText(ss);
                    table->getItem(i,j)->setData(laddr,Qt::UserRole+1);
                    table->getItem(i,j)->setData(haddr,Qt::UserRole+2);
                    sumValue=sumValue+getFloatbyInt(_value);
                }
                table->getItem(i,3)->setText(QString::number(sumValue,'f',2));
            }
        }
    }
}

void QCCMTab::updateValueCCMTMartix()
{
    const int i=2;
    QPointer<QTableListGroup> table=tableMap.value(i).second;
    if(UIRegisterMap.contains(tableMap.value(i).first))
    {
        std::vector<std::vector<QStringList>> uiVector=UIRegisterMap.value(tableMap.value(i).first);
        if(normalization){
            for(int i=0;i<3;i++){
                float sum=0.0;
                for(int j=0;j<3;j++){
                    float _value=0.0;
                    QStringList str=uiVector.at(i).at(j);
                    QString lstr=str.at(0);
                    QString hstr=str.at(1);
                    int laddr=0;
                    int haddr=0;
                    laddr=lstr.toInt(nullptr,16);
                    haddr=hstr.toInt(nullptr,16);
                    if(i!=j){
                        _value=getValuebyCombineBitAddr(laddr,haddr);//从寄存器获取
                        QString ss=QString::number(getFloatbyInt(_value),'f',2);
                        table->getItem(i,j)->setText(ss);
                        table->getItem(i,j)->setData(laddr,Qt::UserRole+1);
                        table->getItem(i,j)->setData(haddr,Qt::UserRole+2);
                        sum=sum+getFloatbyInt(_value);
                    }
                }
                QStringList str=uiVector.at(i).at(i);
                QString lstr=str.at(0);
                QString hstr=str.at(1);
                int laddr=0;
                int haddr=0;
                laddr=lstr.toInt(nullptr,16);
                haddr=hstr.toInt(nullptr,16);
                setValuebyCombinedBit(getIntbyFloat(1.0-sum),laddr,haddr);//normlization模式下自动写入对角行寄存器
                table->getItem(i,i)->setText(QString::number(1.0-sum,'f',2));
                table->getItem(i,i)->setData(laddr,Qt::UserRole+1);
                table->getItem(i,i)->setData(haddr,Qt::UserRole+2);
                table->getItem(i,3)->setText(QString::number(1,'f',2));
            }
        }else{
            for(int i=0;i<3;i++){
                float sumValue=0.0;
                for(int j=0;j<3;j++){
                    int _value=0;
                    QStringList str=uiVector.at(i).at(j);
                    QString lstr=str.at(0);
                    QString hstr=str.at(1);
                    int laddr=0;
                    int haddr=0;
                    laddr=lstr.toInt(nullptr,16);
                    haddr=hstr.toInt(nullptr,16);
                    _value=getValuebyCombineBitAddr(laddr,haddr);
                    QString ss=QString::number(getFloatbyInt(_value),'f',2);
                    table->getItem(i,j)->setText(ss);
                    table->getItem(i,j)->setData(laddr,Qt::UserRole+1);
                    table->getItem(i,j)->setData(haddr,Qt::UserRole+2);
                    sumValue=sumValue+getFloatbyInt(_value);
                }
                table->getItem(i,3)->setText(QString::number(sumValue,'f',2));
            }
        }
    }
}

void QCCMTab::updateValueCCMAMartix()
{
    const int i=3;
    QPointer<QTableListGroup> table=tableMap.value(i).second;
    if(UIRegisterMap.contains(tableMap.value(i).first))
    {
        std::vector<std::vector<QStringList>> uiVector=UIRegisterMap.value(tableMap.value(i).first);
        if(normalization){
            for(int i=0;i<3;i++){
                float sum=0.0;
                for(int j=0;j<3;j++){
                    float _value=0.0;
                    QStringList str=uiVector.at(i).at(j);
                    QString lstr=str.at(0);
                    QString hstr=str.at(1);
                    int laddr=0;
                    int haddr=0;
                    laddr=lstr.toInt(nullptr,16);
                    haddr=hstr.toInt(nullptr,16);
                    if(i!=j){
                        _value=getValuebyCombineBitAddr(laddr,haddr);//从寄存器获取
                        QString ss=QString::number(getFloatbyInt(_value),'f',2);
                        table->getItem(i,j)->setText(ss);
                        table->getItem(i,j)->setData(laddr,Qt::UserRole+1);
                        table->getItem(i,j)->setData(haddr,Qt::UserRole+2);
                        sum=sum+getFloatbyInt(_value);
                    }
                }
                QStringList str=uiVector.at(i).at(i);
                QString lstr=str.at(0);
                QString hstr=str.at(1);
                int laddr=0;
                int haddr=0;
                laddr=lstr.toInt(nullptr,16);
                haddr=hstr.toInt(nullptr,16);
                setValuebyCombinedBit(getIntbyFloat(1.0-sum),laddr,haddr);//normlization模式下自动写入对角行寄存器
                table->getItem(i,i)->setText(QString::number(1.0-sum,'f',2));
                table->getItem(i,i)->setData(laddr,Qt::UserRole+1);
                table->getItem(i,i)->setData(haddr,Qt::UserRole+2);
                table->getItem(i,3)->setText(QString::number(1,'f',2));
            }
        }else{
            for(int i=0;i<3;i++){
                float sumValue=0.0;
                for(int j=0;j<3;j++){
                    int _value=0;
                    QStringList str=uiVector.at(i).at(j);
                    QString lstr=str.at(0);
                    QString hstr=str.at(1);
                    int laddr=0;
                    int haddr=0;
                    laddr=lstr.toInt(nullptr,16);
                    haddr=hstr.toInt(nullptr,16);
                    _value=getValuebyCombineBitAddr(laddr,haddr);
                    QString ss=QString::number(getFloatbyInt(_value),'f',2);
                    table->getItem(i,j)->setText(ss);
                    table->getItem(i,j)->setData(laddr,Qt::UserRole+1);
                    table->getItem(i,j)->setData(haddr,Qt::UserRole+2);
                    sumValue=sumValue+getFloatbyInt(_value);
                }
                table->getItem(i,3)->setText(QString::number(sumValue,'f',2));
            }
        }
    }
}

void QCCMTab::initConnect()
{
    connect(this,&QTabWndComm::readSignal,this,[=](){updateValue();});
    connect(this,&QTabWndComm::reConnectRead,this,[=](){recordUIRegister();updateValue();});
    // connect(labelSpinSliderCheckBoxGroup.get(),&QLabelSpinSliderCheckBoxGroup::changeValueByHLBits,this,[=](){updateValue();});
    connect(this,&QTabWndComm::updateValue,this,[=](){
        switch (groupUIIndex) {
        case 0:
            updateValueEditGroup();
            break;
        case 1:
            updateValueMANMartix();
            break;
        case 2:
            updateValueCCMDMartix();
            break;
        case 3:
            updateValueCCMTMartix();
            break;
        case 4:
            updateValueCCMAMartix();
            break;
        default:
            updateValue();
            break;
        }
        });
    connect(AutoNormal,&QLabelCheckBox::checkSignal,this,&QCCMTab::recvAutoNormal);
}

void QCCMTab::recordUIRegister()
{
    combinedRegisterMap.clear();
    for (auto item : ccm_box){
        if(item->Label()=="CCMEnable"){
            recordCombineRegbyUIifo("CCurveFilter_V60_HW",item->Label(),nullptr);
        }else{
            recordCombineRegbyUIifo("CURVE",item->Label(),std::make_shared<int>(getCurrentCamera()));
        }
    }
    for (auto item : ccm_edit)
        recordCombineRegbyUIifo("CURVE",item->Label(),std::make_shared<int>(getCurrentCamera()));
    for(auto iter=tableMap.begin();iter!=tableMap.end();iter++){
        QString tabelName=iter.value().first;
        for(int i=0;i<3;++i){
            for(int j=0;j<3;++j)
                recordCombineRegbyUIifo("CURVE",(tabelName+QString::number(i)+QString::number(j)),std::make_shared<int>(getCurrentCamera()));
        }
    }
    UIRegisterMap.clear();
    recordUITableRegister("CURVE","ManCCM");
    recordUITableRegister("CURVE","CCMD");
    recordUITableRegister("CURVE","CCMT");
    recordUITableRegister("CURVE","CCMA");
    saveCombinedReg();

    // connect (this, &QTabWndComm::registerMoudleUpdateDone, this, [=]()
    // {
    //     registerMoudleReadable = true;
    // });
    
    while (!registerMoudleReadable)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    registerSubPageMap.clear();
    loadDeletedRegistersFromFile(strDelRegListTxt);

    for (auto reg : registerMoudle)
    {
        // registerMoudle.push_back(reg);
        // if (registerSubPageMap.count(reg) == 0)
        //     registerSubPageMap.insert(reg, std::vector<QString>(1, moduleName.c_str()));
        // else
        registerSubPageMap[reg].emplace_back(moduleName.c_str());
    }

    registerMoudleReadable = false;
    // qDebug() << "combinedRegisterMap size: " << combinedRegisterMap.size();
    // qDebug() << "registerMoudle size: " << registerMoudle.size();
}

void QCCMTab::writeTable(QPointer<QTableListGroup> table,QString tableName)
{
    if(!UIRegisterMap.contains(tableName))
        return;
    std::vector<std::vector<QStringList>> uiVector=UIRegisterMap.value(tableName);
    for(int i=0;i<3;i++){
        for(int j=0;j<3;j++){
            float fvalue=table->getItem(i,j)->text().toFloat();
            QStringList str=uiVector.at(i).at(j);
            QString lstr=str.at(0);
            QString hstr=str.at(1);
            int uValue=getIntbyFloat(fvalue);//RGB转换
            setValuebyCombinedBit(uValue,lstr,hstr);
        }
    }
}

void QCCMTab::recordUITableRegister(QString name, QString UIName)
{
    std::vector<UiInfo> uiInfo;
    std::vector<std::vector<QStringList>> uiRegVector;
    std::vector<QStringList> vector0;
    std::vector<QStringList> vector1;
    std::vector<QStringList> vector2;
    vector2.reserve(3);
    uiInfo= DatabaseOrm::getUiInfoByModuleNameAndControlName(moduleName,UIName.toStdString());
    for (auto i : uiInfo) {
        CombinedRegister combineReg
            = DatabaseOrm::getCombinedRegisterByModuleNameAndCameraIdAndCombinedName(
                name.toStdString(), std::make_shared<int>(getCurrentCamera()), *(i.combinedName));
        std::vector<RegisterBitInfo> regBitInfo=DatabaseOrm::getRegisterBitInfoByCombinedRegisterId(combineReg.combinedId);
        if(i.index<3){
            QStringList List;
            QString lowaddr;
            QString highaddr;
            for(int j=0;j<regBitInfo.size();j++){
                if(regBitInfo.at(j).combinedBitMask==255)
                    lowaddr=QString::number(regBitInfo.at(j).registerAddr,16);
                if(regBitInfo.at(j).combinedBitMask==65280)
                    highaddr=QString::number(regBitInfo.at(j).registerAddr,16);
            }
            List<<lowaddr<<highaddr;
            vector0.push_back(List);
        }else if(i.index>=3&&i.index<6){
            QStringList List;
            QString lowaddr;
            QString highaddr;
            for(int j=0;j<regBitInfo.size();j++){
                if(regBitInfo.at(j).combinedBitMask==255)
                    lowaddr=QString::number(regBitInfo.at(j).registerAddr,16);
                if(regBitInfo.at(j).combinedBitMask==65280)
                    highaddr=QString::number(regBitInfo.at(j).registerAddr,16);
            }
            List<<lowaddr<<highaddr;
            vector1.push_back(List);

        }else if(i.index>=6&&i.index<9){
            QStringList List;
            QString lowaddr;
            QString highaddr;
            for(int j=0;j<regBitInfo.size();j++){
                if(regBitInfo.at(j).combinedBitMask==255)
                    lowaddr=QString::number(regBitInfo.at(j).registerAddr,16);
                if(regBitInfo.at(j).combinedBitMask==65280)
                    highaddr=QString::number(regBitInfo.at(j).registerAddr,16);
            }
            List<<lowaddr<<highaddr;
            vector2.push_back(List);
        }
    }
    std::vector<QStringList> vector;
    vector.push_back(vector0[2]);
    vector.push_back(vector0[1]);
    vector.push_back(vector0[0]);
    vector0=vector;
    vector.clear();
    vector.push_back(vector1[2]);
    vector.push_back(vector1[1]);
    vector.push_back(vector1[0]);
    vector1=vector;
    vector.clear();
    vector.push_back(vector2[2]);
    vector.push_back(vector2[1]);
    vector.push_back(vector2[0]);
    vector2=vector;
    uiRegVector.push_back(vector2);
    uiRegVector.push_back(vector1);
    uiRegVector.push_back(vector0);
    UIRegisterMap.insert(UIName,uiRegVector);
}

void QCCMTab::recvAutoNormal(QString str, bool flag)
{
    if(flag){
        normalization=true;
        for(int j=0;j<tableMap.size();++j){
            QPointer<QTableListGroup> table=tableMap.value(j).second;
            for(int i=0;i<3;i++){
                float num,num1,num2=0.0;
                if(i==0){
                    num1=table->getItem(i,1)->text().toFloat();
                    num2=table->getItem(i,2)->text().toFloat();
                }
                if(i==1){
                    num1=table->getItem(i,0)->text().toFloat();
                    num2=table->getItem(i,2)->text().toFloat();
                }
                if(i==2){
                    num1=table->getItem(i,0)->text().toFloat();
                    num2=table->getItem(i,1)->text().toFloat();
                }
                num=1.0-num1-num2;
                table->getItem(i,i)->setText(QString::number(num,'f',2));
                table->getItem(i,i)->setEnabled(false);
                table->getItem(i,i)->setBackground(QBrush(QColor(200,200,200)));
                table->getItem(i,3)->setText(QString::number(1.0,'f',2));
                table->getItem(i,3)->setEnabled(false);
            }
            writeTable(table,tableMap.value(j).first);
        }
        clearCurrentRigester();
    }else{
        normalization=false;
        for(int j=0;j<tableMap.size();++j){
            QPointer<QTableListGroup> table=tableMap.value(j).second;
            for(int i=0;i<3;i++){
                table->getItem(i,i)->setEnabled(true);
                table->getItem(i,i)->setBackground(QBrush(QColor(230,230,230)));
            }
        }
    }
}

void QCCMTab::recvValueChanged(QStandardItem *item, int row, int col)
{
    labelSpinSliderCheckBoxGroup->setSpinSliderEnable(false);
    QString tableName=item->data(Qt::UserRole+3).toString();
    if(tableName=="ManCCM")
        groupUIIndex=1;
    if(tableName=="CCMD")
        groupUIIndex=2;
    if(tableName=="CCMT")
        groupUIIndex=3;
    if(tableName=="CCMA")
        groupUIIndex=4;
    QString itemName=tableName+QString::number(row)+QString::number(col);
    updateRegisterTable(combinedRegisterMap.value(itemName));
}
QCCMTab::~QCCMTab()
{

}
