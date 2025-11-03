#include "qimgfmttab.h"
#include "Common/Others/qusbcommunication.h"

QImgFmtTab::QImgFmtTab(QWidget *parent)
{
    DualCam=new QLabelButton();
    CamNum=new QLabelButton();
    Width=new QLabelEditGroup();
    Height=new QLabelEditGroup();
    BLC=new QLabelEditGroup();
    CurTemp=new QLabelEditGroup();
    RGBIR=new QLabelComboBox();
    GSHDR=new QLabelButton();
    HDRMode=new QLabelComboBox();
    moduleName="ImgFmt";
    strDelRegListTxt = QCoreApplication::applicationDirPath() + "\\extracted_data\\ImgFmt.txt";
    
    init();
    initConnect();
}

QImgFmtTab::~QImgFmtTab()
{

}

void QImgFmtTab::updateValue()
{
    updateUIValue<std::vector<QPointer<QLabelEditGroup>>>(Img_edit);
}

void QImgFmtTab::init()
{
    subPageMap.insert(0, moduleName.c_str());
    contentWidget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    QVBoxLayout*vlayout=new QVBoxLayout;
    contentLayout->addLayout(vlayout,0,0);
    QGridLayout*glayout=new QGridLayout;
    contentLayout->addLayout(glayout,0,1);
    contentLayout->setHorizontalSpacing(40);
    DualCam->setLabel("DualCam");
    vlayout->addWidget(DualCam);
    CamNum->setLabel("CamNum");
    CamNum->setChecked(true);
    CamNum->setONOFF("camera0","camera1");
    vlayout->addWidget(CamNum);
    Width->setLabelValue("Width");
    Width->setMouseTracking(true);
    Width->setEditable(false);
    connect(this, &QTabWndComm::readSignal, Width, [=]() {
        if (!Width->isVisible())
            return;
        uint32_t value=0;
        auto reg=combinedRegisterMap.value("Width");
        for(int i=0;i<reg.size();i++){
            readCombinedRegisterValue(reg.at(i), value);
            Width->setValue(QString("%1").arg(value));
        }
    });
    connect(Width, &QLabelEditGroup::releaseLabel, this, [=](){QTabWndComm::recvEdit("Width");
        CurUpdateEdit.clear();
        CurUpdateEdit.push_back(Width);});
    Width->setValue("0");
    vlayout->addWidget(Width);
    Height->setLabelValue("Height");
    Height->setMouseTracking(true);
    Height->setEditable(false);
    connect(this, &QTabWndComm::readSignal, Height, [=]() {
        if (!Height->isVisible())
            return;
        uint32_t value=0;
        auto reg=combinedRegisterMap.value("Height");
        for(int i=0;i<reg.size();i++){
            readCombinedRegisterValue(reg.at(i), value);
            Height->setValue(QString("%1").arg(value));
        }
    });
    connect(Height, &QLabelEditGroup::releaseLabel, this, [=](){QTabWndComm::recvEdit("Height");
        CurUpdateEdit.clear();
        CurUpdateEdit.push_back(Height);});
    Height->setValue("0");
    vlayout->addWidget(Height);
    BLC->setLabelValue("BLC");
    BLC->setMouseTracking(true);
    BLC->setEditable(false);
    connect(this, &QTabWndComm::readSignal, BLC, [=]() {
        if (!BLC->isVisible())
            return;
        uint32_t value=0;
        auto reg=combinedRegisterMap.value("BLC");
        for(int i=0;i<reg.size();i++){
            readCombinedRegisterValue(reg.at(i), value);
            BLC->setValue(QString("0X%1").arg(QString::number(value,16).toUpper().rightJustified(VALUE_HEX_INFO,'0')));
        }
    });
    connect(BLC, &QLabelEditGroup::releaseLabel, this, [=](){
        recvBLC("BLC");
        CurUpdateEdit.clear();
        CurUpdateEdit.push_back(BLC);
    });
    connect(labelSpinSliderCheckBoxGroup, &QLabelSpinSliderCheckBoxGroup::ctrlTriggered, this, [=](uint32_t value){
        // qDebug() << "ctrlTrigger enter.";
        if (CurUpdateEdit.size() && CurUpdateEdit[CurUpdateEdit.size() - 1]->Label() == "BLC")
        {
            // qDebug() << "ctrlTrigger success.";
            labelSpinSliderCheckBoxGroup->setcurrentValue(value);
            labelSpinSliderCheckBoxGroup->setValue(value);

            auto combinedRegister = combinedRegisterMap.value("BLC");
            auto combinedRegister2 = combinedRegisterMap.value("BLCH");
            for (int i = 0; i < combinedRegister.size(); i++)
                writeCombinedRegisterValue(combinedRegister.at(i), value);
            for (int i = 0; i < combinedRegister2.size(); i++)
            {
                writeCombinedRegisterValue(combinedRegister2.at(i), value);
                combinedRegister.push_back(combinedRegister2.at(i));
            }
            updateTable(combinedRegister);
            emit updateValue();
            // emit changeValueByHLBits(value);
        }
    });
    BLC->setValue("0x00");
    vlayout->addWidget(BLC);

    CurTemp->setLabelValue("CurTemp(°C)");
    CurTemp->setMouseTracking(true);
    CurTemp->setEditable(false);
    CurTemp->setValue("0.00");

    

    connect(this, &QTabWndComm::readSignal, CurTemp, [=]() {
        if (!CurTemp->isVisible())
            return;
        auto Temp = QUiDataMiddleware("ImgFmt", "CurTempH", "NewFuncPatch_ImgFmt", nullptr).readValueFromDevice();
        CurTemp->setValue(QString("%1").arg((float)Temp / 4.0 - 273.15));

        
    });
    // connect(CurTemp, &QLabelEditGroup::releaseLabel, this, [=](){QTabWndComm::recvEdit("Height");
    //     CurUpdateEdit.clear();
    //     CurUpdateEdit.push_back(CurTemp);});

    vlayout->addWidget(CurTemp);
    Img_edit.push_back(Width);
    Img_edit.push_back(Height);
    Img_edit.push_back(BLC);
    Img_edit.push_back(CurTemp);
    // for(QPointer<QLabelEditGroup> item:Img_edit)
    //     connect(item,&QLabelEditGroup::releaseLabel,this,[=](){QTabWndComm::recvEdit(item->Label());
    //     CurUpdateEdit.clear();
    //     CurUpdateEdit.push_back(item);});

    connect(CurTemp.get(), &QLabelEditGroup::releaseLabel, this, [=](){
        qDebug() << "Enter recvEdit.";
        // clearCurrentRigester();
        // std::vector<CombinedRegister> re;
        // re = combinedRegisterMap.value("CurTempH");
        // // for(auto it : ModeCombineVector){
        // //     re.push_back(it);
        // // }
        // qDebug() << re.size() << "," << re[0].combinedName.c_str() << "\n";
        // regComVector = re;
        
        // updateTable(re);
        QTabWndComm::recvEdit("CurTempH");
        labelSpinSliderCheckBoxGroup->setSpinSliderEnable(false);
        labelSpinSliderCheckBoxGroup->setBitCheckBoxEnable(false);
        CurUpdateEdit.clear();
        CurUpdateEdit.push_back(CurTemp);
    } );
    
    RGBIR->setValue("RGBIR", QStringList()<<"RGB"<<"IR");
    vlayout->addWidget(RGBIR);
    int iMode = TransferManager::getInstance().getRgbIrMode();
    RGBIR->setCurrentIndex(iMode);
    
    GSHDR->setLabel("GS HDR");
    vlayout->addWidget(GSHDR);
    HDRMode->setValue("HDR Mode",QStringList()<<"LM"<<"LMS"<<"LMSV"<<"LMSF"<<"dual-DCG");
    vlayout->addWidget(HDRMode);
    QStringList strlist;
    strlist<<"AEC"<<"AWB"<<"RawDNS"<<"YUVDNS"<<"Sharpness"<<"DPC-main"<<"DPC-lite"<<"CAC-L"<<"CAC-M"<<"Combine"<<"TM"<<"CCM"<<"Gamma"<<"GammaGain"<<"Curve"//<<"Saturation"
            <<"PreCCM"<<"LSC"/*<<"GBalance"*/<<"DCPC"<<"KPC"<<"FPNC";
    int j=0;
    checkBoxMap.clear();
    for(int i=0;i<strlist.size();i++){
        if(j==9)
            j=0;
        QPointer<QLabelCheckBox>checkBox=new QLabelCheckBox();
        checkBoxMap.insert(strlist.at(i),checkBox);
        checkBox->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
        checkBox->setLabel(strlist.at(i));
        checkBox->setMouseTracking(true);
        connect(checkBox,&QLabelCheckBox::boxClicked,this,&QImgFmtTab::recvCheckBox);
        connect(checkBox,&QLabelCheckBox::release,this,&QImgFmtTab::recvCheckBoxLabel);
        glayout->addWidget(checkBox,j,std::floor((double)i/9.0));
        // if(strlist.at(i)=="KPC"/*||strlist.at(i)=="FPNC"*/){
        //     checkBox->setEnabled(false);
        // }
        // if (strlist.at(i) == "FPNC")
        // {
        //     checkBox->setVisible(false);
        // }
        j=j+1;
    }
    contentLayout->setColumnStretch(0,1);
    contentLayout->setColumnStretch(1,1);
    recordUIRegister();
    addRegisterTable();
}

void QImgFmtTab::initConnect()
{
    connect(this,&QTabWndComm::readSignal,this,[=](){
        updateCheckBox();
        // updateValue();
    });
    connect(this,&QTabWndComm::reConnectRead,this,[=](){
        recordUIRegister();
        updateCheckBox();
        updateValue();});
    connect(DualCam.get(),&QLabelButton::clickSignal,this,&QImgFmtTab::recvDualCam);
    connect(CamNum.get(),&QLabelButton::clickSignal,this,&QImgFmtTab::recvCamNum);
    connect(RGBIR.get(),SIGNAL(signalValueChange(QString)),this,SLOT(recvRGBIR(QString)));
    connect(GSHDR.get(),&QLabelButton::clickSignal,this,&QImgFmtTab::recvGSHDR);
    connect(HDRMode.get(),SIGNAL(signalValueChange(QString)),this,SLOT(recvHDRMode(QString)));
    connect(labelSpinSliderCheckBoxGroup.get(), &QLabelSpinSliderCheckBoxGroup::changeValueByHLBits, this,
        &QImgFmtTab::recValueByHighLowBits);
    connect(this,&QTabWndComm::updateValue,this,[=](){updateUIValue<std::vector<QPointer<QLabelEditGroup>>>(CurUpdateEdit);});
    connect(&TransferManager::getInstance(), &TransferManager::lscValueChanged, this, [=](bool &bNewValue)
    {
        updateCheckBoxByLabel("LSC", bNewValue);
    });
}

void QImgFmtTab::recordUIRegister()
{
    combinedRegisterMap.clear();
    for (auto item : Img_edit)
    {
        // if (item->Label() == "CurTemp")
        // {
            
        // }
        recordCombineRegbyUIifo("ImgFmt",item->Label(),std::make_shared<int>(getCurrentCamera()));
        // recordCombineRegbyUIifo("ImgFmt",item->Label() + "H",std::make_shared<int>(getCurrentCamera()));
    }
        

    // for (auto item : Img_edit)
    //     recordCombineRegbyUIifo("ImgFmt",item->Label() + "H",std::make_shared<int>(getCurrentCamera()));
    
    std::vector<UiInfo> uiInfo;
    // std::vector<CombinedRegister> combinevector0;
    // uiInfo = DatabaseOrm::getUiInfoByModuleNameAndControlName(moduleName,"RGBIR");
    // for (auto i : uiInfo) {
    //     CombinedRegister combineReg
    //         = DatabaseOrm::getCombinedRegisterByModuleNameAndCameraIdAndCombinedName(
    //             "PipeCtrl", std::make_shared<int>(getCurrentCamera()), *(i.combinedName));
    //     combinevector0.push_back(combineReg);
    //     std::vector<RegisterBitInfo> regBitInfo=DatabaseOrm::getRegisterBitInfoByCombinedRegisterId(combineReg.combinedId);
    //     for(int j=0;j<regBitInfo.size();j++){
    //         if(i.index==0)
    //             RGBIR_address_0=regBitInfo.at(j).registerAddr;
    //         if(i.index==1)
    //             RGBIR_address_1=regBitInfo.at(j).registerAddr;
    //     }
    // }
    // combinedRegisterMap.insert("RGBIR",combinevector0);

    std::vector<CombinedRegister> combinevector1;
    uiInfo = DatabaseOrm::getUiInfoByModuleNameAndControlName(moduleName,"HDR Mode");
    for (auto i : uiInfo) {
        CombinedRegister combineReg
            = DatabaseOrm::getCombinedRegisterByModuleNameAndCameraIdAndCombinedName(
                "PipeCtrl", std::make_shared<int>(getCurrentCamera()), *(i.combinedName));
        combinevector1.push_back(combineReg);
        std::vector<RegisterBitInfo> regBitInfo=DatabaseOrm::getRegisterBitInfoByCombinedRegisterId(combineReg.combinedId);
        for(int j=0;j<regBitInfo.size();j++){
            HDR_Mode_address=regBitInfo.at(j).registerAddr;
        }
    }
    combinedRegisterMap.insert("HDR Mode",combinevector1);

    // QStringList list;
    std::vector<CombinedRegister> combinevector;
    // uiInfo = DatabaseOrm::getUiInfoByModuleNameAndControlName(moduleName,"Saturation0");
    // for (auto i : uiInfo) {
    //     CombinedRegister combineReg
    //             = DatabaseOrm::getCombinedRegisterByModuleNameAndCameraIdAndCombinedName(
    //                     "CURVE", std::make_shared<int>(getCurrentCamera()), *(i.combinedName));
    //     combinevector.push_back(combineReg);
    //     std::vector<RegisterBitInfo> regBitInfo=DatabaseOrm::getRegisterBitInfoByCombinedRegisterId(combineReg.combinedId);
    //     for(int j=0;j<regBitInfo.size();j++){
    //         QString addr=QString::number(regBitInfo.at(j).registerAddr,16);
    //         list.append(addr);
    //     }
    // }
    // combinedRegisterMap.insert("Saturation",combinevector);
    

    // FWAddressMap.insert("Saturation",list);
    // recordFWUI("PipeCtrl","AEC");
    // recordFWUI("PipeCtrl","AWB");
    // recordFWUI("PipeCtrl","RawDNS");
    recordFWUI("YUVDNS","YUVDNS");
    recordFWUI("YUVDNS","Sharpness");
    // recordFWUI("PipeCtrl","DPC");
    // recordFWUI("PipeCtrl","Combine");
    // recordFWUI("PipeCtrl","TM");
    // recordFWUI("PipeCtrl","Curve");
    // recordFWUI("PipeCtrl","PreCCM");
    // recordFWUI("PipeCtrl","LSC");
    // recordFWUI("CIP_LM","GBalance");
    // recordFWUI("PipeCtrl", "KPC");

    HDRcombinevector.clear();
    uiInfo= DatabaseOrm::getUiInfoByModuleNameAndControlName(moduleName,"HDR ModeH");
    for (auto i : uiInfo) {
        CombinedRegister combineReg
            = DatabaseOrm::getCombinedRegisterByModuleNameAndCameraIdAndCombinedName(
                "ISP", nullptr, *(i.combinedName));
        HDRcombinevector.push_back(combineReg);
    }
    combinedRegisterMap.insert("HDR ModeH",HDRcombinevector);
    // RGBIRcombinevector.clear();
    // uiInfo= DatabaseOrm::getUiInfoByModuleNameAndControlName(moduleName,"RGBIRH");
    // for (auto i : uiInfo) {
    //     CombinedRegister combineReg
    //         = DatabaseOrm::getCombinedRegisterByModuleNameAndCameraIdAndCombinedName(
    //             "ISP", nullptr, *(i.combinedName));
    //     RGBIRcombinevector.push_back(combineReg);
    // }
    // combinedRegisterMap.insert("RGBIRH",RGBIRcombinevector);

    std::vector<CombinedRegister> vecCombinedRegCurTemp;
    uiInfo = DatabaseOrm::getUiInfoByModuleNameAndControlName(moduleName, "CurTempH");
    for (auto i : uiInfo) {
        CombinedRegister combineReg
            = DatabaseOrm::getCombinedRegisterByModuleNameAndCameraIdAndCombinedName(
                "NewFuncPatch_ImgFmt", nullptr, *(i.combinedName));
        // qDebug() << combineReg.moduleName.c_str() << "," << combineReg.combinedName.c_str() << "," << combineReg.combinedId;
        vecCombinedRegCurTemp.push_back(combineReg);
    }
    combinedRegisterMap.insert("CurTempH", vecCombinedRegCurTemp);

    // list.clear();
    combinevector.clear();
    uiInfo = DatabaseOrm::getUiInfoByModuleNameAndControlName(moduleName,"BLCH");
    for (auto i : uiInfo) {
        CombinedRegister combineReg
                = DatabaseOrm::getCombinedRegisterByModuleNameAndCameraIdAndCombinedName(
                        "ISP", nullptr, *(i.combinedName));
        combinevector.push_back(combineReg);
        std::vector<RegisterBitInfo> regBitInfo=DatabaseOrm::getRegisterBitInfoByCombinedRegisterId(combineReg.combinedId);
        // for(int j=0;j<regBitInfo.size();j++){
        //     QString addr=QString::number(regBitInfo.at(j).registerAddr,16);
        //     list.append(addr);
        // }
    }
    combinedRegisterMap.insert("BLCH",combinevector);

    // combinevector.clear();
    // uiInfo = DatabaseOrm::getUiInfoByModuleNameAndControlName(moduleName,"AWBH");
    // for (auto i : uiInfo) {
    //     CombinedRegister combineReg
    //             = DatabaseOrm::getCombinedRegisterByModuleNameAndCameraIdAndCombinedName(
    //                     "ISP", nullptr, *(i.combinedName));
    //     combinevector.push_back(combineReg);
    //     std::vector<RegisterBitInfo> regBitInfo=DatabaseOrm::getRegisterBitInfoByCombinedRegisterId(combineReg.combinedId);
    //     // for(int j=0;j<regBitInfo.size();j++){
    //     //     QString addr=QString::number(regBitInfo.at(j).registerAddr,16);
    //     //     list.append(addr);
    //     // }
    // }
    // combinedRegisterMap.insert("AWBH",combinevector);
    // HWAddressMap.insert("BLCH", std::make_pair(bitmask,list));
    
    recordHWUI("ISP","AECH",7);
    recordHWUI("ISP","AWBH",5);
    recordHWUI("ISP","RawDNSH",5);
    // recordHWUI("ISP","RGBIRH",4);
    // recordHWUI("CYUVDNS_V60_HW","YUVDNSH",0);
    // recordHWUI("CYUVDNS_V60_HW","SharpnessH",0);
    recordHWUI("ISP","DPC-mainH",7);
    recordHWUI("ISP","DPC-liteH",7);
    recordHWUI("RAWDNS_V65_HW_RAWL","CAC-LH",2);
    recordHWUI("RAWDNS_V65_HW_RAWM","CAC-MH",2);
    recordHWUI("ISP","CombineH",5);
    recordHWUI("ISP","TMH",6);
    recordHWUI("CCurveFilter_V60_HW","CCMH",0);
    recordHWUI("CCurveFilter_V60_HW","GammaH",5);
    recordHWUI("CCurveFilter_V60_HW","GammaGainH",4);
    recordHWUI("ISP","CurveH",7);
    recordHWUI("ISP","PreCCMH",6);
    recordHWUI("ISP","LSCH",5);
    recordHWUI("ISP","KPCH",7);
    recordHWUI("3D_DNS","FPNCH",1);
    saveCombinedReg();

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
}

void QImgFmtTab::recordFWUI(std::string name, std::string UIName)
{
    std::vector<UiInfo> uiInfo;
    QStringList list;
    uiInfo = DatabaseOrm::getUiInfoByModuleNameAndControlName(moduleName,UIName);
    std::vector<CombinedRegister> combinevector;
    for (auto i : uiInfo) {
        CombinedRegister combineReg
            = DatabaseOrm::getCombinedRegisterByModuleNameAndCameraIdAndCombinedName(
                name, std::make_shared<int>(getCurrentCamera()), *(i.combinedName));
        combinevector.push_back(combineReg);
        std::vector<RegisterBitInfo> regBitInfo=DatabaseOrm::getRegisterBitInfoByCombinedRegisterId(combineReg.combinedId);
        for(int j=0;j<regBitInfo.size();j++){
            QString addr=QString::number(regBitInfo.at(j).registerAddr,16);
            list.append(addr);
        }
    }
    FWAddressMap.insert(QString::fromStdString(UIName),list);
    combinedRegisterMap.insert(QString::fromStdString(UIName),combinevector);
}

void QImgFmtTab::recordHWUI(std::string name, std::string UIName, int bitmask)
{
    std::vector<UiInfo> uiInfo;
    QStringList list;
    std::vector<CombinedRegister> combinevector;
    std::vector<std::pair<int, QString>> vecPairList;
    uiInfo = DatabaseOrm::getUiInfoByModuleNameAndControlName(moduleName,UIName);
    for (auto i : uiInfo) {
        CombinedRegister combineReg
            = DatabaseOrm::getCombinedRegisterByModuleNameAndCameraIdAndCombinedName(
                name, nullptr, *(i.combinedName));
        std::vector<RegisterBitInfo> regBitInfo=DatabaseOrm::getRegisterBitInfoByCombinedRegisterId(combineReg.combinedId);
        combinevector.push_back(combineReg);
        for(int j=0;j<regBitInfo.size();j++){
            QString addr=QString::number(regBitInfo.at(j).registerAddr,16);
            // list.append(addr);
            int iBitMask = regBitInfo.at(j).bitMask;
            auto countOnes = [&](int iSrc, int &oneCount) -> int
            {
                oneCount = 0;
                int lowestOneBit = -1;
                
                for(int i = 0; i < 8; i++)
                {
                    if (iSrc & (1 << i))
                    {
                        oneCount++;
                        if (lowestOneBit == -1)
                        {
                            lowestOneBit = i;
                        }
                    }
                }
                
                return lowestOneBit;
            };
            int iOneCount = 0;
            int iLowestBit = countOnes(iBitMask, iOneCount);
            if (iOneCount == 1)
                vecPairList.push_back(std::make_pair(iLowestBit, addr));
            else
                vecPairList.push_back(std::make_pair(bitmask, addr));
        }
    }
    HWAddressMap.insert(QString::fromStdString(UIName), vecPairList);
    combinedRegisterMap.insert(QString::fromStdString(UIName),combinevector);
}



void QImgFmtTab::recvDualCam(bool flag)
{
    CurUpdateEdit.clear();
    isDualCam=flag;
}

void QImgFmtTab::recvCamNum(bool flag)
{
    CurUpdateEdit.clear();
    if(flag){
        setCurrentCamera(0);
    }else{
        setCurrentCamera(1);
    }
}

void QImgFmtTab::recvBLC(QString str) 
{ 
    labelSpinSliderCheckBoxGroup->setSpinSliderEnable(true);
    labelSpinSliderCheckBoxGroup->setBitCheckBoxEnable(true);

    clearCurrentRigester();

    auto combinedRegister = combinedRegisterMap.value("BLC");
    labelSpinSliderCheckBoxGroup->setCombinedRegister2(combinedRegister);
    auto combinedRegister2 = combinedRegisterMap.value("BLCH");
    for (int i = 0; i < combinedRegister2.size(); i++)
    {
        combinedRegister.push_back(combinedRegister2.at(i));
    }
    updateTable(combinedRegister);
}

void QImgFmtTab::recvRGBIR(QString str)
{
    CurUpdateEdit.clear();
    clearCurrentRigester();
    labelSpinSliderCheckBoxGroup->setCurrentRange(0,1);
    if (str == "RGB") {
        TransferManager::getInstance().changeRgbIrMode(0);
    } else if (str == "IR") {
        TransferManager::getInstance().changeRgbIrMode(1);
    }
}

void QImgFmtTab::recvGSHDR(bool flag)
{
    CurUpdateEdit.clear();
    isGSHDR=flag;
    checkBoxMap.value("KPC")->setEnabled(flag);
    // checkBoxMap.value("FPNC")->setEnabled(flag);
}

void QImgFmtTab::recvHDRMode(QString str)
{
    CurUpdateEdit.clear();
    clearCurrentRigester();
    labelSpinSliderCheckBoxGroup->setCurrentRange(0,1);
    uint8_t value=0;
    uint32_t HW_value=0;
    if(str==""){
        return;
    }
    if(str=="LM"){
        value=4;
        HW_value=0;
    }
    if(str=="LMS"){
        value=5;
        HW_value=1;
    }
    if(str=="LMSV"){
        value=6;
        HW_value=2;
    }
    if(str=="LMSF"){
        value=7;
        HW_value=3;
    }
    if(str=="dual-DCG"){
        value=8;
        HW_value=4;
    }
    QUsbCommunication::getInstance().writeBurst(HDR_Mode_address, &value, ADDRESS_LENGTH, VALUE_LENGTH);
    for(auto item:HDRcombinevector){
        std::vector<RegisterBitInfo> regBitInfo = DatabaseOrm::getRegisterBitInfoByCombinedRegister(item);
        writeCombinedRegisterValue(item,HW_value);
    }
    std::vector<CombinedRegister> re;
    re=combinedRegisterMap.value("HDR Mode");
    for(auto it:HDRcombinevector){
        re.push_back(it);
    }
    setCombinedReg(re);
    updateTable(re);
}

void QImgFmtTab::recvCheckBox(QString str, bool isChecked)
{
    CurUpdateEdit.clear();
    clearCurrentRigester();
    qDebug() << "checkBoxClicked: " << str << ", isChecked: " << isChecked;
    // if(str=="YUVDNS")
    //     checkBoxMap.value("Sharpness")->setEnabled(isChecked);
    labelSpinSliderCheckBoxGroup->setCurrentRange(0,1);
    labelSpinSliderCheckBoxGroup->setSpinSliderEnable(true);
    labelSpinSliderCheckBoxGroup->setBitCheckBoxEnable(true);
    QStringList FWaddr=FWAddressMap.value(str);
    if(isChecked){
        for(int i=0;i<FWaddr.size();i++){
            uint32_t faddress=FWaddr.at(i).toInt(nullptr,16);
            uint8_t value=1;
            QUsbCommunication::getInstance().writeBurst(faddress, &value, ADDRESS_LENGTH, VALUE_LENGTH);
        }
        writeHWbyBitMask(str,true);
        // qDebug() << "writeHWbyBitMask. ";
        labelSpinSliderCheckBoxGroup->setValue(1);
        // qDebug() << "setValue. ";
    }else{
        for(int i=0;i<FWaddr.size();i++){
            uint32_t faddress=FWaddr.at(i).toInt(nullptr,16);
            uint8_t value=0;
            QUsbCommunication::getInstance().writeBurst(faddress, &value, ADDRESS_LENGTH, VALUE_LENGTH);
        }
        writeHWbyBitMask(str,false);
        // qDebug() << "writeHWbyBitMask. ";
        labelSpinSliderCheckBoxGroup->setValue(0);
        // qDebug() << "setValue. ";
    }
    checkBoxValueUpdate(str, isChecked);
    // qDebug() << "checkBoxValueUpdate. ";
    std::vector<CombinedRegister> re;
    re=combinedRegisterMap.value(str+"H");
    for(auto it:combinedRegisterMap.value(str)){
        re.push_back(it);
    }
    // qDebug() << "setCombinedReg. ";
    setCombinedReg(re);
    // qDebug() << "updateTable. ";
    updateTable(re);
}

void QImgFmtTab::recvCheckBoxLabel(QString str)
{
    CurUpdateEdit.clear();
    clearCurrentRigester();
    labelSpinSliderCheckBoxGroup->setCurrentRange(0,1);
    //checkbox状态恢复
    labelSpinSliderCheckBoxGroup->setSpinSliderEnable(true);
    labelSpinSliderCheckBoxGroup->setBitCheckBoxEnable(true);
    std::vector<CombinedRegister> re;
    re=combinedRegisterMap.value(str+"H");
    for(auto it:combinedRegisterMap.value(str)){
        re.push_back(it);
    }
    setCombinedReg(re);
    updateTable(re);
}

void QImgFmtTab::recValueByHighLowBits(int value)
{
    if (labelSpinSliderCheckBoxGroup->getCombinedRegister().empty()) {
            updateTable(regComVector);
    }
}
void QImgFmtTab::writeHWbyBitMask(QString str,bool isChecked)
{
    // QStringList HWaddr=HWAddressMap.value(str+"H").second;
    // int currentBitMask=HWAddressMap.value(str+"H").first;
    QString ctr;
    if(isChecked){
        ctr="1";
    }else{
        ctr="0";
    }
    auto vecBitAddrPair = HWAddressMap.value(str + "H");
    for(int i=0;i<vecBitAddrPair.size();i++)
    {
        QString strAddr = vecBitAddrPair.at(i).second;
        int currentBitMask = vecBitAddrPair.at(i).first;
        // qDebug() << "currentBitMask: " << currentBitMask;
        uint8_t value=0;
        QString val_str;
        uint32_t haddress=strAddr.toInt(nullptr,16);
        QUsbCommunication::getInstance().readBurst(haddress, &value, ADDRESS_LENGTH, VALUE_LENGTH);
        val_str=val_str.setNum(value,2).rightJustified(8,'0');
        val_str[7-currentBitMask]=ctr[0];
        value=val_str.toInt(nullptr,2);
        QUsbCommunication::getInstance().writeBurst(haddress, &value, ADDRESS_LENGTH, VALUE_LENGTH);
        qDebug() << "HW write address: " << strAddr << ", bit: " << currentBitMask << ", value: " << QString::number(value,2).toUpper().rightJustified(8,'0');
    }
}

void QImgFmtTab::updateCheckBox()
{
    uint8_t value = 0;
    // QUsbCommunication::getInstance().readBurst(RGBIR_address_0, &value, ADDRESS_LENGTH, VALUE_LENGTH);
    // if(value==0){
    //     RGBIR->setChecked(false);
    // }else if(value==1){
    //     RGBIR->setChecked(true);
    // }

    QUsbCommunication::getInstance().readBurst(HDR_Mode_address, &value, ADDRESS_LENGTH, VALUE_LENGTH);
    HDRMode->setCurrentIndex(value-4);
    for(auto it=FWAddressMap.begin();it!=FWAddressMap.end();it++){
        QString addr=it.value().at(0);
        uint8_t value=0;
        QUsbCommunication::getInstance().readBurst(addr.toInt(nullptr, 16), &value, ADDRESS_LENGTH, VALUE_LENGTH);
        if(value==1){
            checkBoxMap.value(it.key())->setChecked(true);
        }else{
            checkBoxMap.value(it.key())->setChecked(false);
        }
    }
    for(auto it=HWAddressMap.begin();it!=HWAddressMap.end();it++){
        QString str=it.key();
        str.chop(1);
        if(FWAddressMap.contains(str))
            continue;
        QString addr=it.value().at(0).second;
        uint8_t value=0;
        QUsbCommunication::getInstance().readBurst(addr.toInt(nullptr, 16), &value, ADDRESS_LENGTH, VALUE_LENGTH);
        QString val_str;
        int bit=0;
        bit=it.value().at(0).first;
        val_str=val_str.setNum(value,2).rightJustified(8,'0');
        QString ctr="1";

        if(val_str[7-bit]==ctr[0]){
            if(checkBoxMap.contains(str)){
                checkBoxMap.value(str)->setChecked(true);
            }
        }else{
            if(checkBoxMap.contains(str)){
                checkBoxMap.value(str)->setChecked(false);
            }
        }
    }
}

void QImgFmtTab::updateCheckBoxByLabel(QString str, bool bNewValue) 
{ 
    if(checkBoxMap.contains(str)){
        checkBoxMap.value(str)->setChecked(bNewValue);
    }
}

void QImgFmtTab::checkBoxValueUpdate(QString strLabelValue, bool isChecked) 
{ 
    if (strLabelValue == "LSC")
    {
        TransferManager::getInstance().changeLscValue(isChecked);
    }
}

void QImgFmtTab::setCombinedReg(std::vector<CombinedRegister> regVector)
{
    regComVector=regVector;
}
