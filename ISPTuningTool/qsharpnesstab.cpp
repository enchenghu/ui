#include "qsharpnesstab.h"
#include "Common/UI/quidatamiddleware.h"
#include <QComboBox>
#include "qtransfermanager.h"
#include "Common/Others/qusbcommunication.h"

QSharpnessTab::QSharpnessTab(QWidget *parent)
{

    int iMode = TransferManager::getInstance().getRgbIrMode();
    strRgbIrMode = iMode == 0 ? "RGB" : "IR";
    tabComm=new QTabWidget();
    tableListGroup=new QTableListGroup();
    comboBoxLM=new QComboBox();
    comboBoxSV=new QComboBox();
    moduleName="Sharpness";
    strDelRegListTxt = QCoreApplication::applicationDirPath() + "\\extracted_data\\Sharpness.txt";
    loadDeletedRegistersFromFile(strDelRegListTxt);
    init();
    initConnect();
}

QSharpnessTab::~QSharpnessTab()
{

}

void QSharpnessTab::init()
{
    addTabWidget(tabComm);
    QStringList tabList;
    if(m_nLevel==LEVEL_ADMINISTRATOR)
    {
        tabList<<"BR(L/M)"<<"BR(S/LFS)"<<"YUVDNS";
    }
    else
    {
        tabList<<"YUVDNS";
    }
    for(int i=0;i<tabList.size();i++){
        QPointer<QWidget> _tab=new QWidget;
        tabComm->insertTab(i,_tab,tabList.at(i));
        tabMap.insert(tabList.at(i),std::make_pair(_tab,false));
        subPageMap.insert(i, tabList[i]);
    }
    for(int i=0;i<3;i++)
        changeTab(i);
    recordUIRegister();
    recordUIRegSubPageInfo();
    addRegisterTable();
}

void QSharpnessTab::updateValue()
{
    changeTab(tabComm->currentIndex());
}

void QSharpnessTab::initConnect()
{
    connect(tabComm,&QTabWidget::currentChanged,this,&QSharpnessTab::changeTab);
    connect(this,&QTabWndComm::readSignal,this,[=](){updateValue();});
    connect(this,&QTabWndComm::reConnectRead,this,[=](){recordUIRegister();updateValue();});
    connect(this,&QTabWndComm::updateValue,this,[=](){updateValue();});
    connect(&TransferManager::getInstance(), &TransferManager::rgbIrModeChanged, 
        this, &QSharpnessTab::changeRgbIrMode);
}

std::vector<CombinedRegister> QSharpnessTab::combinedRegisterSubPageRecording(QString strModuleName, QString strUiName, std::shared_ptr<int> ptrCamera, QString strSubPage) 
{
    std::vector<UiInfo> uiInfo;
    qDebug() << strModuleName << ", " << strUiName;
    uiInfo= DatabaseOrm::getUiInfoByModuleNameAndControlName(moduleName, strUiName.toStdString());
    std::vector<CombinedRegister> combinevector;
    for (auto i : uiInfo) {
        CombinedRegister combineReg
            = DatabaseOrm::getCombinedRegisterByModuleNameAndCameraIdAndCombinedName(
                strModuleName.toStdString(), ptrCamera, *(i.combinedName));
        combinevector.push_back(combineReg);
    }

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

std::vector<CombinedRegister> QSharpnessTab::getCombinedRegister(QString strModuleName, QString strUiName, std::shared_ptr<int> ptrCamera)
{
    std::vector<UiInfo> uiInfo;
    qDebug() << strModuleName << ", " << strUiName;
    uiInfo= DatabaseOrm::getUiInfoByModuleNameAndControlName(moduleName, strUiName.toStdString());
    std::vector<CombinedRegister> combinevector;
    for (auto i : uiInfo) {
        CombinedRegister combineReg
            = DatabaseOrm::getCombinedRegisterByModuleNameAndCameraIdAndCombinedName(
                strModuleName.toStdString(), ptrCamera, *(i.combinedName));
        combinevector.push_back(combineReg);
    }
    return combinevector;
}

void QSharpnessTab::recordUIRegister()
{
    combinedRegisterMap.clear();
    QStringList strListLM, strListSV;
    strListLM << "L" << "M";
    strListSV << "S" << "V";
    for (auto item : LM_box)
    {
        for (auto iter : strListLM)
            recordCombineRegbyUIifo("CIP_LM", item->Label() + iter, std::make_shared<int>(getCurrentCamera()));
    }
    for (auto item : LM_edit)
    {
        for (auto iter : strListLM)
            recordCombineRegbyUIifo("CBAYERCIP_V65_HW_RAW"+iter, item->Label() + iter, nullptr);
    }

    for (auto iter : strListSV)
    {
        recordCombineRegbyUIifo("CBAYERCIP_V60_HW_RAW" + iter, "hp_reduce_en" + iter, nullptr);
        recordCombineRegbyUIifo("CIP_" + iter, "CHpEnable" + iter, std::make_shared<int>(getCurrentCamera()));
    }
    for (auto item : LFS_edit)
    {
        for (auto iter : strListSV)
            recordCombineRegbyUIifo("CBAYERCIP_V60_HW_RAW"+iter, item->Label() + iter, nullptr);
    }

    for (auto item : YU_box)
        recordCombineRegbyUIifo("YUVDNS", item->Label(), std::make_shared<int>(getCurrentCamera()));

    for (auto item : YU_edit){
        if(item->Label()=="CurGain")
            recordCombineRegbyUIifo("PipeCtrl", item->Label(), std::make_shared<int>(getCurrentCamera()));
        else
        {
            recordCombineRegbyUIifo("CYUVDNS_V60_HW", item->Label().remove("*"), nullptr);
            recordCombineRegbyUIifo("CYUVDNS_V60_HW_IR_AB", item->Label().remove("*") + "_IR_AB", nullptr);
            recordCombineRegbyUIifo("CYUVDNS_V60_HW_RGB_AB", item->Label().remove("*") + "_RGB_AB", nullptr);
        }
            
    }

    for(int i=0;i<7;++i){
        for(int j=0;j<8;++j)
        {
            recordCombineRegbyUIifo("YUVDNS", (list.at(i)+QString::number(j)), std::make_shared<int>(getCurrentCamera()));
            if (SHARPNESS_RGBIR_ENABLE_MAP.contains({QString("Sharpness"), list.at(i)+QString::number(j)})) 
            {
                recordCombineRegbyUIifo("YUVDNS_IR_AB", (list.at(i) + QString::number(j) + "_IR_AB"), std::make_shared<int>(getCurrentCamera()));
                recordCombineRegbyUIifo("YUVDNS_RGB_AB", (list.at(i) + QString::number(j) + "_RGB_AB"), std::make_shared<int>(getCurrentCamera()));
                // setRegisterTypeMap(combinedRegisterMap.value(list.at(i)+QString::number(j)), combinedRegisterMap.value(list.at(i)+QString::number(j)+"_RGB_AB"), combinedRegisterMap.value(list.at(i)+QString::number(j)+"_IR_AB"));
            }
        }
            
    }
    // saveCombinedReg();
}

void QSharpnessTab::recordUIRegSubPageInfo()
{
    // combinedRegisterMap.clear();
    QStringList strListLM, strListSV;
    strListLM << "L" << "M";
    strListSV << "S" << "V";
    for (auto item : LM_box)
    {
        for (auto iter : strListLM)
        {
            combinedRegisterSubPageRecording("CIP_LM", item->Label() + iter, std::make_shared<int>(getCurrentCamera()), "BR(L/M)");
        }
    }
    for (auto item : LM_edit)
    {
        for (auto iter : strListLM)
        {
            combinedRegisterSubPageRecording("CBAYERCIP_V65_HW_RAW" + iter, item->Label() + iter, nullptr, "BR(L/M)");
        }
    }

    for (auto iter : strListSV)
    {
        combinedRegisterSubPageRecording("CBAYERCIP_V60_HW_RAW" + iter, "hp_reduce_en" + iter, nullptr, "BR(S/LFS)");
        combinedRegisterSubPageRecording("CIP_" + iter, "CHpEnable" + iter, std::make_shared<int>(getCurrentCamera()), "BR(S/LFS)");
    }
    for (auto item : LFS_edit)
    {
        for (auto iter : strListSV)
        {
            combinedRegisterSubPageRecording("CBAYERCIP_V60_HW_RAW" + iter, item->Label() + iter, nullptr, "BR(S/LFS)");
        }
    }

    for (auto item : YU_box)
    {
        combinedRegisterSubPageRecording("YUVDNS", item->Label(), std::make_shared<int>(getCurrentCamera()), "YUVDNS");
    }

    for (auto item : YU_edit){
        if(item->Label()=="CurGain")
            combinedRegisterSubPageRecording("PipeCtrl", item->Label(), std::make_shared<int>(getCurrentCamera()), "YUVDNS");
        else
        {
            auto vecOutRegs = combinedRegisterSubPageRecording("CYUVDNS_V60_HW", item->Label().remove("*"), nullptr, "YUVDNS");
            auto vecIrRegs = getCombinedRegister("CYUVDNS_V60_HW_IR_AB", (item->Label().remove("*") + "_IR_AB"), nullptr);
            auto vecRgbRegs = getCombinedRegister("CYUVDNS_V60_HW_RGB_AB", (item->Label().remove("*") + "_RGB_AB"), nullptr);
            setRegisterTypeMap(vecOutRegs, vecRgbRegs, vecIrRegs);
        }
            

    }

    for(int i=0;i<7;++i){
        for(int j=0;j<8;++j)
        {
            auto vecOutRegs = combinedRegisterSubPageRecording("YUVDNS", (list.at(i)+QString::number(j)), std::make_shared<int>(getCurrentCamera()), "YUVDNS");
            if (SHARPNESS_RGBIR_ENABLE_MAP.contains({QString("Sharpness"), list.at(i)+QString::number(j)})) 
            {
                auto vecIrRegs = getCombinedRegister("YUVDNS_IR_AB", (list.at(i) + QString::number(j) + "_IR_AB"), std::make_shared<int>(getCurrentCamera()));
                auto vecRgbRegs = getCombinedRegister("YUVDNS_RGB_AB", (list.at(i) + QString::number(j) + "_RGB_AB"), std::make_shared<int>(getCurrentCamera()));
                setRegisterTypeMap(vecOutRegs, vecRgbRegs, vecIrRegs);
            }
        }
    }
}

void QSharpnessTab::initLM()
{
    if(!tabMap.value("BR(L/M)").second){
        auto iter=tabMap.find("BR(L/M)");
        auto widget=iter.value().first;
        QPointer<QGridLayout> glayout=new QGridLayout;
        widget->setLayout(glayout);
        QStringList boxstr;
        boxstr<<"BRHpEnable";
        QStringList editstr;
        editstr<<"rhp_adj"<<"rhp_thr"<<"rhp_rat"<<"bhp_adj"<<"bhp_thr"<<"bhp_rat";
        for(int i=0;i<boxstr.size();i++){
            QPointer<QLabelCheckBox> box=new QLabelCheckBox;
            box->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
            box->setLabel(boxstr.at(i));
            box->setMouseTracking(true);
            glayout->addWidget(box,1,0);
            LM_box.push_back(box);
        }
        for(int i=0;i<editstr.size();i++){
            QPointer<QLabelEditGroup> edit=new QLabelEditGroup;
            edit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
            edit->setEditable(false);
            edit->setLabelValue(editstr.at(i));
            edit->setMouseTracking(true);
            int col=i>2?1:0;
            int row=i>2?(i-1):(i+2);
            glayout->addWidget(edit,row,col);
            LM_edit.push_back(edit);
        }
        comboBoxLM->setMaximumWidth(150);
        comboBoxLM->addItems(QStringList()<<"L"<<"M");
        connect(comboBoxLM,&QComboBox::currentIndexChanged,this,[=](){updateComboBox();updateValue();});
        glayout->addWidget(comboBoxLM,0,0);
        iter.value().second=true;
        for(QPointer<QLabelEditGroup> item:LM_edit)
            connect(item,&QLabelEditGroup::releaseLabel,this,[=](){recvEdit(item->Label()+getStrByLSMV());});

        for(QPointer<QLabelCheckBox> item:LM_box){
            connect(item,&QLabelCheckBox::release,this,[=](){recvEdit(item->Label()+getStrByLSMV());});
            connect(item,&QLabelCheckBox::checkSignal,this,[=](){recvCheckBox(item->Label()+getStrByLSMV(),item->checkState());});
        }
    }
    updateUIValue<QLabelEditGroup>(LM_edit,getStrByLSMV());
    updateUIValue<QLabelCheckBox>(LM_box,getStrByLSMV());
}

void QSharpnessTab::initLFS()
{
    if(!tabMap.value("BR(S/LFS)").second){
        auto iter=tabMap.find("BR(S/LFS)");
        auto widget=iter.value().first;
        QPointer<QGridLayout> glayout=new QGridLayout;
        widget->setLayout(glayout);
        QStringList boxstr;
        boxstr<<"hp_reduce_en"<<"CHpEnable";
        QStringList editstr;
        editstr<<"rhp_amp"<<"rhp_thr"<<"rhp_rat"<<"bhp_amp"<<"bhp_thr"<<"bhp_rat";
        for(int i=0;i<boxstr.size();i++){
            QPointer<QLabelCheckBox> box=new QLabelCheckBox;
            box->setLabel(boxstr.at(i));
            box->setMouseTracking(true);
            glayout->addWidget(box,1,i);
            LFS_box.push_back(box);
        }
        for(int i=0;i<editstr.size();i++){
            QPointer<QLabelEditGroup> edit=new QLabelEditGroup;
            edit->setEditable(false);
            edit->setLabelValue(editstr.at(i));
            edit->setMouseTracking(true);
            int col=i>2?1:0;
            int row=i>2?(i-1):(i+2);
            glayout->addWidget(edit,row,col);
            LFS_edit.push_back(edit);
        }
        comboBoxSV->setMaximumWidth(150);
        comboBoxSV->addItems(QStringList()<<"S"<<"V");
        connect(comboBoxSV,&QComboBox::currentIndexChanged,this,[=](){updateComboBox();updateValue();});
        glayout->addWidget(comboBoxSV,0,0);
        iter.value().second=true;
        for(QPointer<QLabelEditGroup> item:LFS_edit)
            connect(item,&QLabelEditGroup::releaseLabel,this,[=](){recvEdit(item->Label()+getStrByLSMV());});
        for(QPointer<QLabelCheckBox> item:LFS_box){
            connect(item,&QLabelCheckBox::release,this,[=](){recvEdit(item->Label()+getStrByLSMV());});
            connect(item,&QLabelCheckBox::checkSignal,this,[=](){recvCheckBox(item->Label()+getStrByLSMV(),item->checkState());});
        }
    }
    updateUIValue<QLabelEditGroup>(LFS_edit,getStrByLSMV());
    updateUIValue<QLabelCheckBox>(LFS_box,getStrByLSMV());
}

void QSharpnessTab::initYUVDNS()
{
    if(!tabMap.value("YUVDNS").second){
        auto iter=tabMap.find("YUVDNS");
        auto widget=iter.value().first;
        QPointer<QGridLayout> glayout=new QGridLayout;
        QPointer<QVBoxLayout> vlayout=new QVBoxLayout;
        widget->setLayout(glayout);
        glayout->addLayout(vlayout,0,1,3,1);
        QStringList boxstr;
        boxstr<<"ShpEnable";
        QStringList editstr;
        editstr<<"shp_limit_rat"<<"CurGain";
        for(int i=0;i<boxstr.size();i++){
            QPointer<QLabelCheckBox> box=new QLabelCheckBox;
            box->setLabel(boxstr.at(i));
            box->setMouseTracking(true);
            glayout->addWidget(box,0,0);
            YU_box.push_back(box);
        }
        for(int i=0;i<editstr.size();i++){
            QPointer<QLabelEditGroup> edit=new QLabelEditGroup;
            edit->setEditable(false);
            if (editstr.at(i) == "shp_limit_rat")
                edit->setLabelValue("*" + editstr.at(i));
            else
                edit->setLabelValue(editstr.at(i));
            // edit->setLabelValue(editstr.at(i));
            edit->setMouseTracking(true);
            glayout->addWidget(edit,i+1,0);
            YU_edit.push_back(edit);
        }
        tableListGroup=new QTableListGroup();
        list.clear();
        list<<"WhtBoost"<<"BlkBoost"<<"Int"<<"Rat"<<"Adjust"<<"Back"<<"GainNode";
        tableListGroup->setSortingEnabled(false);
        tableListGroup->setTableMenuRequested(false);
        tableListGroup->setAlternatingRowColors(true);
        tableListGroup->setStyle("QTableView{gridline-color:white;color:rgb(0,0,0);outline:0px;background-color:rgb(230,230,230);}"
                                 "QHeaderView::section{border:0px solid rgb(0,0,0);background-color:rgb(165,165,165);color:rgb(255,255,255);font:bold};"
            );
        tableListGroup->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
        tableListGroup->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tableListGroup->setMinimumHeight(230);
        tableListGroup->setMinimumWidth(680);
        tableListGroup->setHeader(QStringList()<<"0"<<"1"<<"2"<<"3"<<"*4"<<"*5"<<"*6"<<"*7");
        for(int j=0;j<8;j++)
            tableListGroup->horizontalHeader()->setSectionResizeMode(j,QHeaderView::ResizeMode::Stretch);
        for(int k=0;k<7;k++){
            for(int j=0;j<8;j++)
                tableListGroup->appendRow(k,j,"",Qt::AlignCenter);
        }
        tableListGroup->setVerticalHeader(QStringList()<<"WhtBoost"<<"BlkBoost"<<"Int"<<"Rat"<<"Adjust"<<"Back"<<"GainNode");
        tableListGroup->setVerticalHeaderVisible(true);
        for(int j=0;j<7;j++)
            tableListGroup->verticalHeader()->setSectionResizeMode(j,QHeaderView::ResizeMode::Stretch);
        tableListGroup->horizontalHeader()->setMinimumHeight(40) ;
        tableListGroup->verticalHeader()->setDefaultAlignment(Qt::AlignCenter);
        tableListGroup->verticalHeader()->setMinimumWidth(40) ;
        vlayout->addWidget(tableListGroup);
        iter.value().second=true;
        for(QPointer<QLabelEditGroup> item:YU_edit)
            connect(item,&QLabelEditGroup::releaseLabel,this,[=](){
                if (SHARPNESS_RGBIR_ENABLE_MAP.contains({QString("Sharpness"), item->Label().remove("*")}))
                    recvEditTarget(item->Label().remove("*"));
                else
                    recvEdit(item->Label());
            });
        for(QPointer<QLabelCheckBox> item:YU_box){
            connect(item,&QLabelCheckBox::release,this,[=](){recvEdit(item->Label());});
            connect(item,&QLabelCheckBox::checkSignal,this,[=](){recvCheckBox(item->Label(),item->checkState());});
        }
        connect(tableListGroup,SIGNAL(sendCurrentItem(QStandardItem*,int,int)),this,SLOT(recvValueChanged(QStandardItem*,int,int)));
    }
    updateUIValue<QLabelEditGroup>(YU_edit);
    updateUIValue<QLabelCheckBox>(YU_box);
    updateTable();
}

template <typename T>
void QSharpnessTab::updateUIValue(std::vector<QPointer<T>>& vector, const QString &extraname)
{
    int valTemp = 0;
    QUsbCommunication::getInstance().readBurst(IR_RGB_MODE_REG_ADDR, (unsigned char*)(&valTemp), ADDRESS_LENGTH, VALUE_LENGTH);
    for(int i = 0; i < vector.size(); i++)
    {
        uint32_t value=0;
        QString strLabelOrigin = vector[i]->Label().remove("*") + extraname;
        auto reg = combinedRegisterMap.value(strLabelOrigin);
        if (SHARPNESS_RGBIR_ENABLE_MAP.contains({moduleName.c_str(), strLabelOrigin}) && valTemp == 0x01) 
        {
            std::string strSuffix = strRgbIrMode == "RGB" ? "_RGB_AB" : "_IR_AB";
            reg = combinedRegisterMap.value(strLabelOrigin + strSuffix.c_str());
        }
        
        for(int j=0;j<reg.size();j++)
        {
            readCombinedRegisterValue(reg.at(j), value);
            if (std::is_same<T, QLabelEditGroup>::value)
                dynamic_cast<QLabelEditGroup*>(vector[i].get())->setValue(QString("0x%1").arg(QString::number(value,16).toUpper().rightJustified(VALUE_HEX_INFO,'0')));
            else if (std::is_same<T, QLabelCheckBox>::value)
            {
                if (value == 0)
                    dynamic_cast<QLabelCheckBox*>(vector[i].get())->setChecked(false);
                else if (value == 1)
                    dynamic_cast<QLabelCheckBox*>(vector[i].get())->setChecked(true);
            }
        }
    }
}

void QSharpnessTab::recvEditTarget(QString str)
{
    labelSpinSliderCheckBoxGroup->setSpinSliderEnable(true);
    labelSpinSliderCheckBoxGroup->setBitCheckBoxEnable(true);
    if (SHARPNESS_RGBIR_ENABLE_MAP.contains({moduleName.c_str(), str})) 
    {
        int valTemp = 0;
        QUsbCommunication::getInstance().readBurst(IR_RGB_MODE_REG_ADDR, (unsigned char*)(&valTemp), ADDRESS_LENGTH, VALUE_LENGTH);

        std::string strSuffix = strRgbIrMode == "RGB" ? "_RGB_AB" : "_IR_AB";
        if (valTemp == 0x01)
        {
            updateRegisterTable(combinedRegisterMap.value(str + QString::fromStdString(strSuffix)));
        }
        else 
        {
            updateRegisterTable(combinedRegisterMap.value(str));
        }
    }
    else 
    {
        updateRegisterTable(combinedRegisterMap.value(str));
    }
    // updateRegisterTable(combinedRegisterMap.value(str));//写寄存器
}

void QSharpnessTab::updateTable()
{
    int valTemp = 0;
    QUsbCommunication::getInstance().readBurst(IR_RGB_MODE_REG_ADDR, (unsigned char*)(&valTemp), ADDRESS_LENGTH, VALUE_LENGTH);
    if(!tableListGroup.get())
        return;
    for(int k=0;k<8;k++){
        for(int j=0;j<7;j++){
            QString itemName=list.at(j)+QString::number(k);
            uint32_t value=0;
            auto reg = combinedRegisterMap.value(itemName);
            if (SHARPNESS_RGBIR_ENABLE_MAP.contains({QString("Sharpness"), list.at(j)+QString::number(k)})) 
            {
                std::string strSuffix = strRgbIrMode == "RGB" ? "_RGB_AB" : "_IR_AB";
                if (valTemp == 0x01)
                {
                    reg = combinedRegisterMap.value(itemName + strSuffix.c_str());
                }
                else
                {
                    
                }
            }
            
            for(int i=0;i<reg.size();i++){
                readCombinedRegisterValue(reg.at(i), value);
                QStandardItem* item=tableListGroup->getItem(j,k);
                if(item)
                    item->setText(QString("0x%1").arg(QString::number(value,16).toUpper().rightJustified(VALUE_HEX_INFO,'0')));
            }
        }
    }
}

QString QSharpnessTab::getStrByLSMV()
{
    QString str;
    if(tabComm->currentIndex()==0){
        str=comboBoxLM->currentText();//写寄存器
    }else if(tabComm->currentIndex()==1){
        str=comboBoxSV->currentText();//写寄存器
    }else{
        str="";
    }
    return str;
}

void QSharpnessTab::updateComboBox()
{
    for (auto item : LM_box)
        recordCombineRegbyUIifo("CIP_LM",item->Label()+comboBoxLM->currentText(),std::make_shared<int>(getCurrentCamera()));
    for (auto item : LM_edit)
        recordCombineRegbyUIifo("CBAYERCIP_V65_HW_RAW"+comboBoxLM->currentText(),item->Label()+comboBoxLM->currentText(),nullptr);

    recordCombineRegbyUIifo("CBAYERCIP_V60_HW_RAW"+comboBoxSV->currentText(),"hp_reduce_en"+comboBoxSV->currentText(),nullptr);
    recordCombineRegbyUIifo("CIP_"+comboBoxSV->currentText(),"CHpEnable"+comboBoxSV->currentText(),std::make_shared<int>(getCurrentCamera()));
    for (auto item : LFS_edit)
        recordCombineRegbyUIifo("CBAYERCIP_V60_HW_RAW"+comboBoxSV->currentText(),item->Label()+comboBoxSV->currentText(),nullptr);
    // saveCombinedReg();
}

void QSharpnessTab::changeTab(int index)
{
    if(tabMap.contains(tabComm->tabText(index))){
        switch (index) {
        case 0:
            if(m_nLevel==LEVEL_ADMINISTRATOR)
            {
                initLM();
                break;
            }
            initYUVDNS();
            break;
        case 1:
            initLFS();
            break;
        case 2:
            initYUVDNS();
            break;
        default:
            break;
        }
    }
}

void QSharpnessTab::recvValueChanged(QStandardItem *item, int row, int col)
{
    QString itemName=list.at(row)+QString::number(col);
    if (SHARPNESS_RGBIR_ENABLE_MAP.contains({QString("Sharpness"), list.at(row)+QString::number(col)})) 
    {
        int valTemp = 0;
        QUsbCommunication::getInstance().readBurst(IR_RGB_MODE_REG_ADDR, (unsigned char*)(&valTemp), ADDRESS_LENGTH, VALUE_LENGTH);
        std::string strSuffix = strRgbIrMode == "RGB" ? "_RGB_AB" : "_IR_AB";
        
        if (valTemp == 0x01)
        {
            itemName += strSuffix.c_str();
            auto re = combinedRegisterMap.value(itemName);
            updateRegisterTable(re);
            // for(int i = 0; i < re.size(); i++)
            // {
            //     uint32_t value=0; 
            //     readCombinedRegisterValue(re.at(i), value);
            //     item->setText(QString("0x%1").arg(QString::number(value,16).toUpper().rightJustified(VALUE_HEX_INFO,'0')));
            // }
        }
        else
        {
            auto re = combinedRegisterMap.value(itemName);
            updateRegisterTable(re);
        }
    }
    else
    {
        auto re = combinedRegisterMap.value(itemName);
        updateRegisterTable(re);
    }
}


