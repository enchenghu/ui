#include "qpreccmtab.h"
#include "Common/UI/quidatamiddleware.h"
#include <QComboBox>
#include <set>

QPreCCMTab::QPreCCMTab(QWidget *parent)
{
    moduleName="PreCCM";
    Current=new QTableListGroup();
    A=new QTableListGroup();
    B=new QTableListGroup();
    C=new QTableListGroup();
    comboBox=new QComboBox();

    init();
    initConnect();
}

QPreCCMTab::~QPreCCMTab()
{

}

void QPreCCMTab::init()
{
    subPageMap.insert(0, "PreCCM");
    QStringList strlist;
    strlist<<"Current"<<"A"<<"B"<<"C";
    tableMap.insert(0,std::make_pair("Current",Current));
    tableMap.insert(1,std::make_pair("A",A));
    tableMap.insert(2,std::make_pair("B",B));
    tableMap.insert(3,std::make_pair("C",C));
    contentWidget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    comboBox->setMaximumWidth(150);
    comboBox->addItems(QStringList()<<"S"<<"V");
    connect(comboBox,&QComboBox::currentTextChanged,this,[=](){recordUIRegister();updateValue();});
    contentLayout->addWidget(comboBox,0,0);
    QPointer<QLabelCheckBox> PreCCMEnable=new QLabelCheckBox();
    PreCCMEnable->setMaximumWidth(270);
    PreCCMEnable->setLabel("PreCCMEnable");
    PreCCMEnable->setMouseTracking(true);
    connect(PreCCMEnable,&QLabelCheckBox::release,this,[=](){recvEdit(PreCCMEnable->Label()+comboBox->currentText());});
    connect(PreCCMEnable,&QLabelCheckBox::checkSignal,this,[=](){recvCheckBox(PreCCMEnable->Label()+comboBox->currentText(),PreCCMEnable->checkState());});

    pre_box.push_back(PreCCMEnable);
    contentLayout->addWidget(PreCCMEnable,1,0);
    QStringList editstr;
    editstr<<"CT Current"<<"pCT[0]"<<"pCT[1]"<<"pCT[2]"<<"pCT[3]";
    for(int i=1;i<editstr.size();i++){
        QPointer<QLabelEditGroup> edit=new QLabelEditGroup();
        edit->setLabelValue(editstr.at(i));
        edit->setEditable(false);
        edit->setMouseTracking(true);
        int row=i==0?0:1;
        int col=i==0?1:i;
        contentLayout->addWidget(edit,row,col);
        pre_edit.push_back(edit);
    }

    QPointer<QLabelEditGroup> CTCurrent      = new QLabelEditGroup();
    CTCurrent->setLabelValue("CT Current");
    CTCurrent->setEditable(false);
    CTCurrent->setValue(0);
    contentLayout->addWidget(CTCurrent,0,1);
    connect(this, &QTabWndComm::readSignal, CTCurrent, [CTCurrent]() {
        uint32_t gainB      = QUiDataMiddleware("PreCCM", "Current Gain B").readValueFromDevice();
        uint32_t gainR      = QUiDataMiddleware("PreCCM", "Current Gain R").readValueFromDevice();
        UINT _CTCurrent =0;
        if(gainB!=0&&gainR!=0){
            double fCurCT = ((float)gainB) / ((float)gainR);
            _CTCurrent = (UINT)(fCurCT * 256);
        }
        CTCurrent->setValue(_CTCurrent);
    });


    QGridLayout*hboxLayout=new QGridLayout;
    hboxLayout->setContentsMargins(0,20,0,0);
    hboxLayout->setVerticalSpacing(0);
    hboxLayout->setHorizontalSpacing(8);
    contentLayout->addLayout(hboxLayout,2,0,1,5);
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
        tableListGroup->setTableMenuRequested(false);
        tableListGroup->setEditTriggers(QAbstractItemView::NoEditTriggers);
        hboxLayout->addWidget(new QLabel(strlist.at(i)),0,(i==0?i:i+1));
        hboxLayout->addWidget(tableListGroup.get(),1,(i==0?i:i+1));
        tableListGroup->setAlternatingRowColors(false);
        tableListGroup->setStyle("QTableView{gridline-color:white;color:rgb(0,0,0);outline:0px;background-color:rgb(230,230,230);}"
                                 "QHeaderView::section{border:0px solid rgb(0,0,0);background-color:rgb(165,165,165);color:rgb(255,255,255);font:bold};"
            );
        tableListGroup->setMinimumHeight(210);
        tableListGroup->setMaximumHeight(350);

        tableListGroup->setHeader(QStringList()<<"R"<<"G"<<"B");
        for(int j=0;j<3;j++)
            tableListGroup->horizontalHeader()->setSectionResizeMode(j,QHeaderView::ResizeMode::Stretch);
        for(int j=0;j<3;j++){
            for(int k=0;k<3;k++){
                tableListGroup->appendRow(k,j,"",Qt::AlignCenter);
                tableListGroup->getItem(k,j)->setData(tableName,Qt::UserRole+1);
                tableListGroup->setRowHeight(k,50);
                tableListGroup->setColumnWidth(j,50);
            }
            tableListGroup->getItem(j,j)->setBackground(QBrush(Qt::transparent));
            tableListGroup->getItem(j,j)->setEnabled(false);
        }
        tableListGroup->setVerticalHeader(QStringList()<<"R"<<"G"<<"B");
        tableListGroup->setVerticalHeaderVisible(true);
        for(int j=0;j<3;j++)
            tableListGroup->verticalHeader()->setSectionResizeMode(j,QHeaderView::ResizeMode::Stretch);
        tableListGroup->horizontalHeader()->setMinimumHeight(40) ;
        tableListGroup->verticalHeader()->setDefaultAlignment(Qt::AlignCenter);
        tableListGroup->verticalHeader()->setMinimumWidth(40) ;
    }
    for(QPointer<QLabelEditGroup> item:pre_edit)
        connect(item,&QLabelEditGroup::releaseLabel,this,[=](){recvEdit(item->Label()+comboBox->currentText());});
    for(QPointer<QLabelCheckBox> item:pre_box){
        connect(item,&QLabelCheckBox::release,this,[=](){recvEdit(item->Label()+comboBox->currentText());});
        connect(item,&QLabelCheckBox::checkSignal,this,[=](){recvCheckBox(item->Label()+comboBox->currentText(),item->checkState());});
    }
    for(auto iter=tableMap.begin();iter!=tableMap.end();iter++){
        QPointer<QTableListGroup> table=iter.value().second;
        connect(table.get(),SIGNAL(sendCurrentItem(QStandardItem*,int,int)),this,SLOT(recvValueChanged(QStandardItem*,int,int)));
    }
    recordUIRegister();
    recordUIRegSubPageInfo();
    // std::this_thread::sleep_for(std::chrono::milliseconds(50));
    addRegisterTable();
    // std::this_thread::sleep_for(std::chrono::milliseconds(50));
    labelSpinSliderCheckBoxGroup->setRealEdit(true);
}

void QPreCCMTab::updateValue()
{
    updateUIValue<std::vector<QPointer<QLabelEditGroup>>>(pre_edit,comboBox->currentText());
    updateUIValue<std::vector<QPointer<QLabelCheckBox>>>(pre_box,comboBox->currentText());
    updateTable();
}

void QPreCCMTab::initConnect()
{
    connect(this,&QTabWndComm::readSignal,this,[=](){updateValue();});
    connect(this,&QTabWndComm::reConnectRead,this,[=](){recordUIRegister();recordUIRegSubPageInfo();updateValue();});
    connect(this,&QTabWndComm::updateValue,this,[=](){updateValue();});
}

void QPreCCMTab::recordUIRegister()
{
    combinedRegisterMap.clear();
    for (auto item : pre_box)
        recordCombineRegbyUIifo("PipeCtrl",item->Label()+comboBox->currentText(),std::make_shared<int>(getCurrentCamera()));
    for (auto item : pre_edit)
        recordCombineRegbyUIifo("PRECCM_"+comboBox->currentText(),item->Label()+comboBox->currentText(),std::make_shared<int>(getCurrentCamera()));
    for(auto iter=tableMap.begin();iter!=tableMap.end();iter++){
        QString tabelName=iter.value().first;
        if(tabelName=="Current"){
            for(int i=0;i<3;++i){
                for(int j=0;j<3;++j)
                    recordCombineRegbyUIifo("CPRECCM_V60_HW_RAW"+comboBox->currentText(),(tabelName+QString::number(i)+QString::number(j)+comboBox->currentText()),nullptr);
            }
        }else{
            for(int i=0;i<3;++i){
                for(int j=0;j<3;++j)
                    recordCombineRegbyUIifo("PRECCM_"+comboBox->currentText(),(tabelName+QString::number(i)+QString::number(j)+comboBox->currentText()),std::make_shared<int>(getCurrentCamera()));
            }
        }
    }
    // saveCombinedReg();
    registerMoudle.clear();
    for(auto it=combinedRegisterMap.begin();it!=combinedRegisterMap.end();it++){
        std::vector<CombinedRegister> combinevector=it.value();
        for(auto regVector:combinevector){
            // qDebug() << regVector.combinedId;
            std::vector<RegisterBitInfo> regBitInfo = DatabaseOrm::getRegisterBitInfoByCombinedRegisterId(regVector.combinedId);
            for(int j=0;j<regBitInfo.size();j++)
                registerMoudle.push_back(regBitInfo.at(j).registerAddr);
        }
    }
}

void QPreCCMTab::updateTable()
{
    for(int i=0;i<tableMap.size();i++){
        QString tableName=tableMap.value(i).first;
        QPointer<QTableListGroup> tableListGroup=tableMap.value(i).second;
        for(int k=0;k<3;k++){
            float sum=0.0;
            for(int j=0;j<3;j++){
                tableListGroup->getItem(k,j)->setData(tableName,Qt::UserRole+1);
                QString itemName=tableName+QString::number(k)+QString::number(j);
                uint32_t value=0;
                auto reg=combinedRegisterMap.value(itemName+comboBox->currentText());
                for(int i=0;i<reg.size();i++){
                    readCombinedRegisterValue(reg.at(i), value);
                    float fval=getFloatbyInt(value);
                    QStandardItem* item=tableListGroup->getItem(k,j);
                    if(item){
                        sum+=fval;
                        item->setText(QString::number(fval,'f',2));
                    }
                }
            }
            float valDia=1.0-sum;
            QStandardItem* itemDia=tableListGroup->getItem(k,k);
            if(itemDia){
                itemDia->setEnabled(false);
                itemDia->setText(QString::number(valDia,'f',2));
            }
        }
    }
}


void QPreCCMTab::recvValueChanged(QStandardItem *item, int row, int col)
{
    labelSpinSliderCheckBoxGroup->setSpinSliderEnable(false);
    QString tableName=item->data(Qt::UserRole+1).toString();
    QString itemName=tableName+QString::number(row)+QString::number(col);
    updateRegisterTable(combinedRegisterMap.value(itemName+comboBox->currentText()));
}

std::vector<CombinedRegister> QPreCCMTab::combinedRegisterSubPageRecording(QString strModuleName, QString strUiName, std::shared_ptr<int> ptrCamera, QString strSubPage) 
{
    std::vector<UiInfo> uiInfo;
    // qDebug() << strModuleName << ", " << strUiName;
    uiInfo = DatabaseOrm::getUiInfoByModuleNameAndControlName(moduleName, strUiName.toStdString());
    std::vector<CombinedRegister> combinevector;
    for (auto i : uiInfo) {
        CombinedRegister combineReg
            = DatabaseOrm::getCombinedRegisterByModuleNameAndCameraIdAndCombinedName(
                strModuleName.toStdString(), ptrCamera, *(i.combinedName));
        combinevector.push_back(combineReg);
    }

    // Assuming regAddrModule is a member variable of type std::set<uint32_t>
    // If not, you'll need to declare it in the header file
    static std::set<uint32_t> regAddrModule;
    
    for (auto reg : combinevector)
    {
        auto reBitInfo = DatabaseOrm::getRegisterBitInfoByCombinedRegister(reg);
        for (int i = 0; i < reBitInfo.size(); i++)
        {
            regAddrModule.insert(reBitInfo[i].registerAddr);
        }
        std::vector<uint32_t>().swap(registerMoudle);
        for (auto iter : regAddrModule)
        {
            registerMoudle.emplace_back(iter);
        }
        // qDebug() << reg.combinedName.c_str() << "," << strSubPage;
        setRegisterSubPage(reg, strSubPage);
    }

    return combinevector;
}

void QPreCCMTab::recordUIRegSubPageInfo()
{
    // PreCCM Tab
    for (auto item : pre_box)
        combinedRegisterSubPageRecording("PipeCtrl", item->Label() + comboBox->currentText(), std::make_shared<int>(getCurrentCamera()), "PreCCM");
        
    for (auto item : pre_edit)
        combinedRegisterSubPageRecording("PRECCM_" + comboBox->currentText(), item->Label() + comboBox->currentText(), std::make_shared<int>(getCurrentCamera()), "PreCCM");
        
    for(auto iter = tableMap.begin(); iter != tableMap.end(); iter++) {
        QString tableName = iter.value().first;
        if(tableName == "Current") {
            for(int i = 0; i < 3; ++i) {
                for(int j = 0; j < 3; ++j)
                    combinedRegisterSubPageRecording("CPRECCM_V60_HW_RAW" + comboBox->currentText(), (tableName + QString::number(i) + QString::number(j) + comboBox->currentText()), nullptr, "PreCCM");
            }
        } else {
            for(int i = 0; i < 3; ++i) {
                for(int j = 0; j < 3; ++j)
                    combinedRegisterSubPageRecording("PRECCM_" + comboBox->currentText(), (tableName + QString::number(i) + QString::number(j) + comboBox->currentText()), std::make_shared<int>(getCurrentCamera()), "PreCCM");
            }
        }
    }
}
