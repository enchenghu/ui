#include "qdnstab.h"
#include <QComboBox>
#include <cstdint>
#include <functional>
#include <vector>
#include "qtransfermanager.h"
#include "Common/Others/qusbcommunication.h"

const QList<QString>                                       TAB_NAME_LIST = { "RAWDNS", "YUVDNS", "CDNS", "3DDNS" };
const QList<QString>                                       pxList     = { "0", "8","16","24","32","48","64","80","96","128","160","192","224","255"};

QDNSTab::QDNSTab(QWidget *parent)
{
    moduleName="DNS";
    int iMode = TransferManager::getInstance().getRgbIrMode();
    strRgbIrMode = iMode == 0 ? "RGB" : "IR";
    strDelRegListTxt = QCoreApplication::applicationDirPath() + "\\extracted_data\\DNS.txt";
    loadDeletedRegistersFromFile(strDelRegListTxt);
    tabWidget=new QTabWidget();
    YUVCurve = new QCurveChartGroup(this);
    YUVCurve->setMinimumWidth(600);
    CDNCurve = new QCurveChartGroup(this);
    CDNCurve->setMinimumWidth(600);
    YUVbox=new QComboBox();
    CDNSbox=new QComboBox();
    init();
    initConnect();

}

QDNSTab::~QDNSTab()
{

}

void QDNSTab::init()
{
    addTabWidget(tabWidget);
    std::map<QString, std::function<void(QWidget *)>> initFuncList;
    if(m_nLevel==LEVEL_ADMINISTRATOR)
    {
        initFuncList= {
            { "RAWDNS", std::bind(&QDNSTab::initRAWDNS, this, std::placeholders::_1) },
            { "YUVDNS", std::bind(&QDNSTab::initYUVDNS, this, std::placeholders::_1) },
            { "CDNS", std::bind(&QDNSTab::initCDNS, this, std::placeholders::_1) },
            { "3DDNS", std::bind(&QDNSTab::init3DDNS, this, std::placeholders::_1)}
        };
        QString subPageLabel[] = { "RAWDNS", "YUVDNS", "CDNS", "3D_DNS" };
        int iCount = 0;
        for (const auto &name : subPageLabel) {
            subPageMap.insert(iCount, name);
            iCount++;
        }
    }
    else
    {
        initFuncList= {
            { "RAWDNS", std::bind(&QDNSTab::initRAWDNS, this, std::placeholders::_1) },
            { "YUVDNS", std::bind(&QDNSTab::initYUVDNS, this, std::placeholders::_1) },
            { "3DDNS", std::bind(&QDNSTab::init3DDNS, this, std::placeholders::_1) }
        };
        QString subPageLabel[] = { "RAWDNS", "YUVDNS", "3D_DNS" };
        int iCount = 0;
        for (const auto &name : subPageLabel) {
            subPageMap.insert(iCount, name);
            iCount++;
        }
        //TAB_NAME_LIST = { "RAWDNS", "YUVDNS"};
        QList<QString>* tempTab = (QList<QString>*)&TAB_NAME_LIST;
        *tempTab= { "RAWDNS", "YUVDNS", "3DDNS" };
        CDNCurve->setVisible(false);
    }
    // int iCount = 0;
    for (const auto &name : TAB_NAME_LIST) {
        QPointer<QWidget> subTab = new QWidget(this);
        tabWidget->addTab(subTab, name);
        initFuncList.at(name)(subTab);
        // subPageMap.insert(iCount, name);
        // iCount++;
    }
    // QString subPageLabel[] = (m_nLevel==LEVEL_ADMINISTRATOR) ? { "RAWDNS", "YUVDNS", "CDNS", "3D_DNS" } : { "RAWDNS", "YUVDNS", "3D_DNS" }
    recordUIRegister();
    recordUIRegSubPageInfo();
    addRegisterTable();
}

void QDNSTab::updateValue()
{
    changeTab(tabWidget->currentIndex());
}

void QDNSTab::initConnect()
{
    connect(tabWidget,&QTabWidget::currentChanged,this,&QDNSTab::changeTab);
    connect(this,&QTabWndComm::readSignal,this,[=](){updateValue();});
    connect(this,&QTabWndComm::reConnectRead,this,[=](){recordUIRegister();updateValue();});
    connect(this,&QTabWndComm::updateValue,this,[=](){updateValue();});
    connect(YUVbox, &QComboBox::currentIndexChanged, this, &QDNSTab::readCurve, Qt::QueuedConnection);
    connect(CDNSbox, &QComboBox::currentIndexChanged, this, &QDNSTab::readCurve, Qt::QueuedConnection);
    connect(&TransferManager::getInstance(), &TransferManager::rgbIrModeChanged, 
        this, &QDNSTab::changeRgbIrMode);
}

void QDNSTab::changeRgbIrMode(int &newMode) 
{ 
    if (newMode == 0) 
    {
        strRgbIrMode = "RGB";
    } 
    else if (newMode == 1) 
    {
        strRgbIrMode = "IR";
    }
}

template <typename T>
void QDNSTab::updateUIValue(T& vector,const QString &extraname)
{
    int valTemp = 0;
    QUsbCommunication::getInstance().readBurst(IR_RGB_MODE_REG_ADDR, (unsigned char*)(&valTemp), ADDRESS_LENGTH, VALUE_LENGTH);
    for(auto item:vector)
    {
        uint32_t value=0;
        QString strLabelOrigin = item->Label().remove("*") + extraname;
        auto reg = combinedRegisterMap.value(strLabelOrigin);
        if (DNS_RGBIR_ENABLE_MAP.contains({moduleName.c_str(), strLabelOrigin}) && valTemp == 0x01) 
        {
            std::string strSuffix = strRgbIrMode == "RGB" ? "_RGB_AB" : "_IR_AB";
            reg = combinedRegisterMap.value(strLabelOrigin + strSuffix.c_str());
        }
        
        for(int i=0;i<reg.size();i++)
        {
            readCombinedRegisterValue(reg.at(i), value);
            item->setValue(QString("0x%1").arg(QString::number(value,16).toUpper().rightJustified(VALUE_HEX_INFO,'0')));
        }
    }
}

template <typename T>
void QDNSTab::updateUIValueLeftShift(T& vector,const QString &extraname, int iShiftBits)
{
    int valTemp = 0;
    QUsbCommunication::getInstance().readBurst(IR_RGB_MODE_REG_ADDR, (unsigned char*)(&valTemp), ADDRESS_LENGTH, VALUE_LENGTH);
    for(auto item:vector)
    {
        uint32_t value=0;
        QString strLabelOrigin = item->Label().remove("*") + extraname;
        auto reg = combinedRegisterMap.value(strLabelOrigin);
        if (DNS_RGBIR_ENABLE_MAP.contains({moduleName.c_str(), strLabelOrigin}) && valTemp == 0x01) 
        {
            std::string strSuffix = strRgbIrMode == "RGB" ? "_RGB_AB" : "_IR_AB";
            reg = combinedRegisterMap.value(strLabelOrigin + strSuffix.c_str());
        }
        
        for(int i=0;i<reg.size();i++)
        {
            readCombinedRegisterValue(reg.at(i), value);
            item->setValue(QString("0x%1").arg(QString::number(value << iShiftBits, 16).toUpper().rightJustified(4, '0')));
        }
    }
}

std::vector<CombinedRegister> QDNSTab::combinedRegisterSubPageRecording(QString strModuleName, QString strUiName, std::shared_ptr<int> ptrCamera, QString strSubPage) 
{
    std::vector<UiInfo> uiInfo;
    // qDebug() << strModuleName << ", " << strUiName;
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

std::vector<CombinedRegister> QDNSTab::getCombinedRegister(QString strModuleName, QString strUiName, std::shared_ptr<int> ptrCamera)
{
    std::vector<UiInfo> uiInfo;
    // qDebug() << strModuleName << ", " << strUiName;
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

void QDNSTab::recordUIRegSubPageInfo()
{
    // combinedRegisterMap.clear();
    // combinedRegisterMap.clear();
    for (auto item : Raw_box){
        if(item->Label()=="Mono Enable"){
            combinedRegisterSubPageRecording("CIP_S", item->Label()+"S", std::make_shared<int>(getCurrentCamera()), "RAWDNS");
            combinedRegisterSubPageRecording("CIP_V", item->Label()+"V", std::make_shared<int>(getCurrentCamera()), "RAWDNS");
        }else{
            combinedRegisterSubPageRecording("ISP", item->Label()+"L", nullptr, "RAWDNS");
            combinedRegisterSubPageRecording("ISP", item->Label()+"M", nullptr, "RAWDNS");
            combinedRegisterSubPageRecording("ISP", item->Label()+"S", nullptr, "RAWDNS");
            combinedRegisterSubPageRecording("ISP", item->Label()+"V", nullptr, "RAWDNS");
        }
    }
    for (auto item : Raw_edit1)
    {
        auto vecRegs = combinedRegisterSubPageRecording("RAWDNS_LM", item->Label().remove("*")+"L", std::make_shared<int>(getCurrentCamera()), "RAWDNS");
        if (DNS_RGBIR_ENABLE_MAP.contains({moduleName.c_str(), item->Label().remove("*") + "L"})) 
        {
            auto vecIrRegs = getCombinedRegister("RAWDNS_LM_IR_AB", item->Label().remove("*") + "L_IR_AB", std::make_shared<int>(getCurrentCamera()));
            auto vecRgbRegs = getCombinedRegister("RAWDNS_LM_RGB_AB", item->Label().remove("*") + "L_RGB_AB", std::make_shared<int>(getCurrentCamera()));
            setRegisterTypeMap(vecRegs, vecRgbRegs, vecIrRegs);
        }
        combinedRegisterSubPageRecording("RAWDNS_LM", item->Label().remove("*")+"M", std::make_shared<int>(getCurrentCamera()), "RAWDNS");
    }
    for (auto item : Raw_edit2){
        combinedRegisterSubPageRecording("CIP_S", item->Label()+"S", std::make_shared<int>(getCurrentCamera()), "RAWDNS");
        combinedRegisterSubPageRecording("CIP_V", item->Label()+"V", std::make_shared<int>(getCurrentCamera()), "RAWDNS");
    }
    for (auto item : YUV_edit)
    {
        auto vecRegs = combinedRegisterSubPageRecording("YUVDNS", item->Label().remove("*")+"YUV", std::make_shared<int>(getCurrentCamera()), "YUVDNS");
        if (DNS_RGBIR_ENABLE_MAP.contains({moduleName.c_str(), item->Label().remove("*") + "YUV"})) 
        {
            auto vecIrRegs = getCombinedRegister("YUVDNS_IR_AB", item->Label().remove("*") + "YUV_IR_AB", std::make_shared<int>(getCurrentCamera()));
            auto vecRgbRegs = getCombinedRegister("YUVDNS_RGB_AB", item->Label().remove("*") + "YUV_RGB_AB", std::make_shared<int>(getCurrentCamera()));
            setRegisterTypeMap(vecRegs, vecRgbRegs, vecIrRegs);
        }
    }
        
    for (auto item : YUV_box)
        combinedRegisterSubPageRecording("YUVDNS", item->Label()+"YUV", std::make_shared<int>(getCurrentCamera()), "YUVDNS");
    for (auto item : CDNS_edit)
        combinedRegisterSubPageRecording("YUVDNS", item->Label()+"CDNS", std::make_shared<int>(getCurrentCamera()), "CDNS");
    for (auto item : CDNS_box)
        combinedRegisterSubPageRecording("YUVDNS", item->Label()+"CDNS", std::make_shared<int>(getCurrentCamera()), "CDNS");
    
    for (auto item : _3D_edit)
    {
        combinedRegisterSubPageRecording("3D_DNS", item->Label()+"3D", nullptr, "3D_DNS");
        // qDebug() << "3D Label:" << item->Label()+"3D" << " combinedRegisterMap size:" << combinedRegisterMap[item->Label() + "3D"].size();
    }
        
    for (auto item : _3D_edit_shift)
        combinedRegisterSubPageRecording("3D_DNS", item->Label()+"3D", nullptr, "3D_DNS");
    for (auto item : _3D_box)
        combinedRegisterSubPageRecording("3D_DNS", item->Label()+"3D", nullptr, "3D_DNS");

    for(int i=0;i<8;i++){
        std::vector<QPoint> pvector;
        for(int j=0;j<14;j++){
            QPoint p;
            p.setX(pxList.at(i).toInt());
            p.setY(0);
            pvector.push_back(p);
            auto vecRegs = combinedRegisterSubPageRecording("YUVDNS", "Node"+QString::number(i)+QString::number(j)+"YUV", std::make_shared<int>(getCurrentCamera()), "YUVDNS");
            if (DNS_RGBIR_ENABLE_MAP.contains({moduleName.c_str(), "Node"+QString::number(i)+QString::number(j)+"YUV"})) 
            {
                auto vecIrRegs = getCombinedRegister("YUVDNS_IR_AB", "Node"+QString::number(i)+QString::number(j)+"YUV_IR_AB", std::make_shared<int>(getCurrentCamera()));
                auto vecRgbRegs = getCombinedRegister("YUVDNS_RGB_AB", "Node"+QString::number(i)+QString::number(j)+"YUV_RGB_AB", std::make_shared<int>(getCurrentCamera()));
                setRegisterTypeMap(vecRegs, vecRgbRegs, vecIrRegs);
            }
            combinedRegisterSubPageRecording("YUVDNS", "Node"+QString::number(i)+QString::number(j)+"CDNS", std::make_shared<int>(getCurrentCamera()), "CDNS");
        }
        curveMap.insert("Node"+QString::number(i),pvector);
        curveMap2.insert("Node"+QString::number(i),pvector);
    }
    combinedRegisterSubPageRecording("PipeCtrl", "Cur GainL", std::make_shared<int>(getCurrentCamera()), "RAWDNS");
    combinedRegisterSubPageRecording("PipeCtrl", "Cur GainM", std::make_shared<int>(getCurrentCamera()), "RAWDNS");
    combinedRegisterSubPageRecording("PipeCtrl", "Cur GainS", std::make_shared<int>(getCurrentCamera()), "RAWDNS");
    combinedRegisterSubPageRecording("PipeCtrl", "Cur GainV", std::make_shared<int>(getCurrentCamera()), "RAWDNS");
    combinedRegisterSubPageRecording("PipeCtrl", "Cur GainCDNS", std::make_shared<int>(getCurrentCamera()), "CDNS");
    combinedRegisterSubPageRecording("PipeCtrl", "Cur GainYUV", std::make_shared<int>(getCurrentCamera()), "YUVDNS");
    combinedRegisterSubPageRecording("PipeCtrl", "Cur Gain3D", std::make_shared<int>(getCurrentCamera()), "3D_DNS");
}

void QDNSTab::recordUIRegister()
{
    combinedRegisterMap.clear();
    for (auto item : Raw_box){
        if(item->Label()=="Mono Enable"){
            recordCombineRegbyUIifo("CIP_S",item->Label()+"S",std::make_shared<int>(getCurrentCamera()));
            recordCombineRegbyUIifo("CIP_V",item->Label()+"V",std::make_shared<int>(getCurrentCamera()));
        }else{
            recordCombineRegbyUIifo("ISP",item->Label()+"L",nullptr);
            recordCombineRegbyUIifo("ISP",item->Label()+"M",nullptr);
            recordCombineRegbyUIifo("ISP",item->Label()+"S",nullptr);
            recordCombineRegbyUIifo("ISP",item->Label()+"V",nullptr);
        }
    }
    for (auto item : Raw_edit1)
    {
        recordCombineRegbyUIifo("RAWDNS_LM",item->Label().remove("*")+"L",std::make_shared<int>(getCurrentCamera()));
        if (DNS_RGBIR_ENABLE_MAP.contains({moduleName.c_str(), item->Label().remove("*") + "L"})) 
        {
            recordCombineRegbyUIifo("RAWDNS_LM_IR_AB", item->Label().remove("*") + "L_IR_AB", std::make_shared<int>(getCurrentCamera()));
            recordCombineRegbyUIifo("RAWDNS_LM_RGB_AB", item->Label().remove("*") + "L_RGB_AB", std::make_shared<int>(getCurrentCamera()));
        }
        recordCombineRegbyUIifo("RAWDNS_LM",item->Label().remove("*")+"M",std::make_shared<int>(getCurrentCamera()));
    }
    for (auto item : Raw_edit2){
        recordCombineRegbyUIifo("CIP_S",item->Label()+"S",std::make_shared<int>(getCurrentCamera()));
        recordCombineRegbyUIifo("CIP_V",item->Label()+"V",std::make_shared<int>(getCurrentCamera()));
    }
    for (auto item : YUV_edit)
    {
        recordCombineRegbyUIifo("YUVDNS",item->Label().remove("*")+"YUV",std::make_shared<int>(getCurrentCamera()));
        if (DNS_RGBIR_ENABLE_MAP.contains({moduleName.c_str(), item->Label().remove("*") + "YUV"})) 
        {
            recordCombineRegbyUIifo("YUVDNS_IR_AB", item->Label().remove("*") + "YUV_IR_AB", std::make_shared<int>(getCurrentCamera()));
            recordCombineRegbyUIifo("YUVDNS_RGB_AB", item->Label().remove("*") + "YUV_RGB_AB", std::make_shared<int>(getCurrentCamera()));
        }
    }
        
    for (auto item : YUV_box)
        recordCombineRegbyUIifo("YUVDNS",item->Label()+"YUV",std::make_shared<int>(getCurrentCamera()));
    for (auto item : CDNS_edit)
        recordCombineRegbyUIifo("YUVDNS",item->Label()+"CDNS",std::make_shared<int>(getCurrentCamera()));
    for (auto item : CDNS_box)
        recordCombineRegbyUIifo("YUVDNS",item->Label()+"CDNS",std::make_shared<int>(getCurrentCamera()));
    
    for (auto item : _3D_edit)
    {
        recordCombineRegbyUIifo("3D_DNS",item->Label()+"3D",nullptr);
        // qDebug() << "3D Label:" << item->Label()+"3D" << " combinedRegisterMap size:" << combinedRegisterMap[item->Label() + "3D"].size();
    }
        
    for (auto item : _3D_edit_shift)
        recordCombineRegbyUIifo("3D_DNS",item->Label()+"3D",nullptr);
    for (auto item : _3D_box)
        recordCombineRegbyUIifo("3D_DNS",item->Label()+"3D",nullptr);

    for(int i=0;i<8;i++){
        std::vector<QPoint> pvector;
        for(int j=0;j<14;j++){
            QPoint p;
            p.setX(pxList.at(i).toInt());
            p.setY(0);
            pvector.push_back(p);
            recordCombineRegbyUIifo("YUVDNS","Node"+QString::number(i)+QString::number(j)+"YUV",std::make_shared<int>(getCurrentCamera()));
            if (DNS_RGBIR_ENABLE_MAP.contains({moduleName.c_str(), "Node"+QString::number(i)+QString::number(j)+"YUV"})) 
            {
                recordCombineRegbyUIifo("YUVDNS_IR_AB", "Node"+QString::number(i)+QString::number(j)+"YUV_IR_AB", std::make_shared<int>(getCurrentCamera()));
                recordCombineRegbyUIifo("YUVDNS_RGB_AB", "Node"+QString::number(i)+QString::number(j)+"YUV_RGB_AB", std::make_shared<int>(getCurrentCamera()));
            }
            recordCombineRegbyUIifo("YUVDNS","Node"+QString::number(i)+QString::number(j)+"CDNS",std::make_shared<int>(getCurrentCamera()));
        }
        curveMap.insert("Node"+QString::number(i),pvector);
        curveMap2.insert("Node"+QString::number(i),pvector);
    }
    recordCombineRegbyUIifo("PipeCtrl","Cur GainL",std::make_shared<int>(getCurrentCamera()));
    recordCombineRegbyUIifo("PipeCtrl","Cur GainM",std::make_shared<int>(getCurrentCamera()));
    recordCombineRegbyUIifo("PipeCtrl","Cur GainS",std::make_shared<int>(getCurrentCamera()));
    recordCombineRegbyUIifo("PipeCtrl","Cur GainV",std::make_shared<int>(getCurrentCamera()));
    recordCombineRegbyUIifo("PipeCtrl","Cur GainCDNS",std::make_shared<int>(getCurrentCamera()));
    recordCombineRegbyUIifo("PipeCtrl","Cur GainYUV",std::make_shared<int>(getCurrentCamera()));
    recordCombineRegbyUIifo("PipeCtrl","Cur Gain3D",std::make_shared<int>(getCurrentCamera()));
    // saveCombinedReg();
}

void QDNSTab::initRAWDNS(QPointer<QWidget> subTab)
{
    QPointer<QGridLayout> tablayout=new QGridLayout;
    subTab->setLayout(tablayout);
    QPointer<QHBoxLayout> hlayout=new QHBoxLayout;
    tablayout->addLayout(hlayout,0,0,1,2);
    tablayout->setRowStretch(1,1);
    tablayout->setRowStretch(2,1);
    tablayout->setRowStretch(0,0);

    QPointer<QGroupBox> box_Gain=new QGroupBox("Gain_Node");
    tablayout->addWidget(box_Gain,1,0);
    QPointer<QVBoxLayout> box_Gainlayout=new QVBoxLayout;
    box_Gainlayout->setSpacing(0);
    box_Gain->setLayout(box_Gainlayout);
    RawL_Group.push_back(box_Gain);
    QPointer<QGroupBox> box_Noise=new QGroupBox("NoiseAddBack");
    tablayout->addWidget(box_Noise,2,0);
    QPointer<QVBoxLayout> box_Noiselayout=new QVBoxLayout;
    box_Noiselayout->setSpacing(0);
    box_Noise->setLayout(box_Noiselayout);
    RawL_Group.push_back(box_Noise);

    QPointer<QGroupBox> box_PatternT=new QGroupBox("PatternThre");
    tablayout->addWidget(box_PatternT,1,1,2,1);
    QPointer<QVBoxLayout> box_PatternTlayout=new QVBoxLayout;
    box_PatternTlayout->setSpacing(0);
    box_PatternT->setLayout(box_PatternTlayout);
    RawL_Group.push_back(box_PatternT);

    QPointer<QGroupBox> box_PatternW=new QGroupBox("PatternWeight");
    tablayout->addWidget(box_PatternW,0,2,3,1);
    QPointer<QVBoxLayout> box_PatternWlayout=new QVBoxLayout;
    box_PatternWlayout->setSpacing(0);
    box_PatternW->setLayout(box_PatternWlayout);
    RawL_Group.push_back(box_PatternW);

    QPointer<QGroupBox> box_Level=new QGroupBox("Noise Level");
    tablayout->addWidget(box_Level,0,3,3,1);
    QPointer<QVBoxLayout> box_Levellayout=new QVBoxLayout;
    box_Levellayout->setSpacing(0);
    box_Level->setLayout(box_Levellayout);
    RawL_Group.push_back(box_Level);

    QStringList grouplist;
    grouplist<<"Gain Node"<<"DNS Sid"<<"Dns add Back";
    std::vector<QPointer<QVBoxLayout>> RawS_Grouplayout;
    for(int i=0;i<grouplist.size();++i){
        QPointer<QGroupBox> box=new QGroupBox(grouplist.at(i));
        box->setVisible(false);
        tablayout->addWidget(box,1,i,2,1);
        QPointer<QVBoxLayout> boxlayout=new QVBoxLayout;
        RawS_Grouplayout.push_back(boxlayout);
        boxlayout->setSpacing(0);
        box->setLayout(boxlayout);
        RawS_Group.push_back(box);
    }

    QStringList str;
    str<<"GainNode0"<<"GainNode1"<<"GainNode2"<<"GainNode3"
       <<"AddBack0"<<"AddBack1"<<"AddBack2"<<"AddBack3"
       <<"ThreET0"<<"ThreET1"<<"ThreET2"<<"ThreET3"<<"ThreTF0"<<"ThreTF1"<<"ThreTF2"<<"ThreTF3"
       <<"Edge0"<<"Edge1"<<"Edge2"<<"Edge3"<<"Text0"<<"Text1"<<"Text2"<<"Text3"<<"Flat0"<<"Flat1"<<"Flat2"<<"Flat3"
       <<"NoiseSlope0"<<"NoiseSlope1"<<"NoiseSlope2"<<"NoiseSlope3"<<"NoiseIntercept0"<<"NoiseIntercept1"<<"NoiseIntercept2"<<"NoiseIntercept3"
       <<"GainNode0"<<"GainNode1"<<"GainNode2"<<"GainNode3"<<"GainNode4"<<"GainNode5"<<"GainNode6"<<"GainNode7"
       <<"DnsSid0"<<"DnsSid1"<<"DnsSid2"<<"DnsSid3"<<"DnsSid4"<<"DnsSid5"<<"DnsSid6"<<"DnsSid7"
       <<"DnsBack0"<<"DnsBack1"<<"DnsBack2"<<"DnsBack3"<<"DnsBack4"<<"DnsBack5"<<"DnsBack6"<<"DnsBack7"
       <<"Cur Gain";

    for(int i=0;i<str.size();i++){
        QPointer<QLabelEditGroup> edit=new QLabelEditGroup;
        edit->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
        edit->setEditable(false);
        edit->setLabelValue(str.at(i));
        edit->setMouseTracking(true);
        Raw_edit.push_back(edit);
        if(i<=3)
            box_Gainlayout->addWidget(edit);
        else if(i>3&&i<=7)
            box_Noiselayout->addWidget(edit);
        else if(i>7&&i<=15)
            box_PatternTlayout->addWidget(edit);
        else if(i>15&&i<=27)
            box_PatternWlayout->addWidget(edit);
        else if(i>27&&i<=35)
            box_Levellayout->addWidget(edit);
        else if(i>35&&i<=43)
            RawS_Grouplayout.at(0)->addWidget(edit);
        else if(i>43&&i<=51)
            RawS_Grouplayout.at(1)->addWidget(edit);
        else if(i>51&&i<=59)
            RawS_Grouplayout.at(2)->addWidget(edit);
        if(i>35){
            Raw_edit2.push_back(edit);
        }else{
            Raw_edit1.push_back(edit);
        }
    }
    Raw_edit[Raw_edit.size()-1]->setSpacing(40);
    hlayout->setSpacing(0);
    QPointer<QGroupBox> box=new QGroupBox("Exp_Type");
    box->setMinimumHeight(20);
    QPointer<QVBoxLayout> layout=new QVBoxLayout;
    box->setLayout(layout);
    hlayout->addWidget(box);
    typebox=new QComboBox();
    layout->addWidget(typebox);
    typebox->addItems(QStringList()<<"L"<<"M"/*<<"S"<<"V"*/);
    if(typebox->currentIndex()==0)
    {
        for (auto item : Raw_edit1) 
        {
            if (DNS_RGBIR_ENABLE_MAP.contains({moduleName.c_str(), item->Label().remove("*") + "L"})) 
            {
                item->setLabelValue("*" + item->Label());
            }
        }
    }
    connect(typebox,&QComboBox::currentIndexChanged,this,[=](){
        if(typebox->currentIndex()==0){
            for (auto item : Raw_edit1) {
                item->setLabelValue("*" + item->Label());
            }
        }
        else
        {
            for (auto item : Raw_edit1) {
                item->setLabelValue(item->Label().remove("*"));
            }
        }
    });

    //非管理员不显示Noise Level
    if(m_nLevel!=LEVEL_ADMINISTRATOR)
    {
        box_Level->setVisible(false);
    }

    QPointer<QLabelCheckBox> enbox=new QLabelCheckBox;
    enbox->setLabel("Enable");
    enbox->setMouseTracking(true);
    QPointer<QLabelCheckBox> monobox=new QLabelCheckBox;
    monobox->setLabel("Mono Enable");
    monobox->setVisible(false);
    monobox->setMouseTracking(true);
    Raw_box.push_back(enbox);
    Raw_box.push_back(monobox);
    hlayout->addWidget(enbox);
    hlayout->addWidget(monobox);
    hlayout->addWidget(*(Raw_edit.end()-1));
    connect(typebox,&QComboBox::currentIndexChanged,this,[=](){
        if(typebox->currentIndex()<2){
            for(auto item:RawL_Group)
                item->setVisible(true);
            for(auto item:RawS_Group)
                item->setVisible(false);
            Raw_box.at(1)->setVisible(false);

            //非管理员不显示Noise Level
            if(m_nLevel!=LEVEL_ADMINISTRATOR)
            {
                box_Level->setVisible(false);
            }
        }else{
            for(auto item:RawL_Group)
                item->setVisible(false);
            for(auto item:RawS_Group)
                item->setVisible(true);
            Raw_box.at(1)->setVisible(true);
        }
        updateValue();
    });

    for(QPointer<QLabelEditGroup> item:Raw_edit)
    {
        if (DNS_RGBIR_ENABLE_MAP.contains({moduleName.c_str(), item->Label().remove("*") + "L"})) 
        {
            connect(item,&QLabelEditGroup::releaseLabel,this,[=](){recvEdit(item->Label().remove("*")+getStr());});
        }
        else
        {
            connect(item,&QLabelEditGroup::releaseLabel,this,[=](){QTabWndComm::recvEdit(item->Label()+getStr());});
        }
    }
        
    for(QPointer<QLabelCheckBox> item:Raw_box){
        connect(item,&QLabelCheckBox::release,this,[=](){QTabWndComm::recvEdit(item->Label()+getStr());});
        connect(item,&QLabelCheckBox::checkSignal,this,[=](){recvCheckBox(item->Label()+getStr(),item->checkState());});
    }
}

void QDNSTab::initYUVDNS(QPointer<QWidget> subTab)
{
    QPointer<QGridLayout> tablayout=new QGridLayout;
    subTab->setLayout(tablayout);
    QPointer<QLabelCheckBox> enbox=new QLabelCheckBox;
    enbox->setLabel("Enable");
    enbox->setMouseTracking(true);
    enbox->setSpacing(18);
    tablayout->addWidget(enbox,0,0);
    YUV_box.push_back(enbox);
    YUVbox->setMaximumWidth(200);
    YUVbox->addItems(QStringList()<<"Node0"<<"Node1"<<"Node2"<<"Node3"<<"Node4"<<"Node5"<<"Node6"<<"Node7");
    tablayout->addWidget(YUVbox,0,2);
    QStringList grouplist;
    grouplist<<"Gain Node";
    //管理员显示Edge
    if(m_nLevel==LEVEL_ADMINISTRATOR)
    {
        grouplist<<"Edge";
    }
    std::vector<QPointer<QVBoxLayout>> YUV_Group;
    for(int i=0;i<grouplist.size();++i){
        QPointer<QGroupBox> box=new QGroupBox(grouplist.at(i));
        tablayout->addWidget(box,1,i);
        QPointer<QVBoxLayout> boxlayout=new QVBoxLayout;
        boxlayout->setSpacing(0);
        box->setLayout(boxlayout);
        YUV_Group.push_back(boxlayout);
    }
    QStringList str;
    str<<"GainNode0"<<"GainNode1"<<"GainNode2"<<"GainNode3"<<"GainNode4"<<"GainNode5"<<"GainNode6"<<"GainNode7"
       <<"EdgeRat0"<<"EdgeRat1"<<"EdgeRat2"<<"EdgeRat3"<<"EdgeRat4"<<"EdgeRat5"<<"EdgeRat6"<<"EdgeRat7"
       <<"Cur Gain";
    for(int i=0;i<str.size();i++){
        QPointer<QLabelEditGroup> edit=new QLabelEditGroup;
        edit->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
        edit->setEditable(false);
        QString strLabel = str.at(i);
        if (DNS_RGBIR_ENABLE_MAP.contains({moduleName.c_str(), strLabel.remove("*") + "YUV"})) 
        {
            edit->setLabelValue("*" + strLabel);
        } 
        else 
        {
            edit->setLabelValue(strLabel);
        }
        // edit->setLabelValue(str.at(i));
        edit->setMouseTracking(true);
        YUV_edit.push_back(edit);
        if(i<=7)
            YUV_Group.at(0)->addWidget(edit);
        else if(i>7&&i<=15)
            if(m_nLevel==LEVEL_ADMINISTRATOR)
                YUV_Group.at(1)->addWidget(edit);
    }
    YUV_edit[YUV_edit.size()-1]->setSpacing(17);
    tablayout->addWidget(*(YUV_edit.end()-1),0,1);
    tablayout->addWidget(YUVCurve,1,2,1,1);
    tablayout->setColumnStretch(2,1);
    tablayout->setColumnStretch(0,0);
    tablayout->setColumnStretch(1,0);
    tablayout->setRowStretch(1,1);
    tablayout->setRowStretch(0,0);
    YUVCurve->setXMax(255);
    YUVCurve->setYMax(255);
    YUVCurve->setAxisRange(0, 255, 0, 255);
    YUVCurve->setAutoAdjustAxisRange(false);
    YUVCurve->setCurveFittingType(CurveFittingType::NONE);
    for(QPointer<QLabelEditGroup> item:YUV_edit)
    {
        if (DNS_RGBIR_ENABLE_MAP.contains({moduleName.c_str(), item->Label().remove("*") + "YUV"})) 
        {
            connect(item,&QLabelEditGroup::releaseLabel,this,[=](){recvEdit(item->Label().remove("*")+"YUV");});
        }
        else
        {
            connect(item,&QLabelEditGroup::releaseLabel,this,[=](){QTabWndComm::recvEdit(item->Label()+"YUV");});
        }
    }
    for(QPointer<QLabelCheckBox> item:YUV_box){
        connect(item,&QLabelCheckBox::release,this,[=](){QTabWndComm::recvEdit(item->Label()+"YUV");});
        connect(item,&QLabelCheckBox::checkSignal,this,[=](){recvCheckBox(item->Label()+"YUV",item->checkState());});
    }
}

void QDNSTab::initCDNS(QPointer<QWidget> subTab)
{
    QPointer<QGridLayout> tablayout=new QGridLayout;
    subTab->setLayout(tablayout);
    QPointer<QLabelCheckBox> enbox=new QLabelCheckBox;
    enbox->setLabel("Enable");
    enbox->setMouseTracking(true);
    enbox->setSpacing(18);
    tablayout->addWidget(enbox,0,0);
    CDNS_box.push_back(enbox);
    CDNSbox->setMaximumWidth(200);
    CDNSbox->addItems(QStringList()<<"Node0"<<"Node1"<<"Node2"<<"Node3"<<"Node4"<<"Node5"<<"Node6"<<"Node7");
    tablayout->addWidget(CDNSbox,0,2);
    QStringList grouplist;
    grouplist<<"Gain Node"/*<<"ColorSatur"*/;
    std::vector<QPointer<QVBoxLayout>> YUV_Group;
    for(int i=0;i<grouplist.size();++i){
        QPointer<QGroupBox> box=new QGroupBox(grouplist.at(i));
        tablayout->addWidget(box,1,i);
        QPointer<QVBoxLayout> boxlayout=new QVBoxLayout;
        boxlayout->setSpacing(0);
        box->setLayout(boxlayout);
        YUV_Group.push_back(boxlayout);
    }
    QStringList str;
    str<<"GainNode0"<<"GainNode1"<<"GainNode2"<<"GainNode3"<<"GainNode4"<<"GainNode5"<<"GainNode6"<<"GainNode7"
        <<"SegPower0"<<"SegPower1"<<"SegPower2"
        <<"Cur Gain";
    for(int i=0;i<str.size();i++){
        QPointer<QLabelEditGroup> edit=new QLabelEditGroup;
        edit->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
        edit->setEditable(false);
        edit->setLabelValue(str.at(i));
        edit->setMouseTracking(true);
        CDNS_edit.push_back(edit);
        if(i<=7)
            YUV_Group.at(0)->addWidget(edit);
        else if(i>7&&i<=10)
            continue;
            // YUV_Group.at(1)->addWidget(edit);
    }
    CDNS_edit[CDNS_edit.size()-1]->setSpacing(17);
    tablayout->addWidget(*(CDNS_edit.end()-1),0,1);
    tablayout->addWidget(CDNCurve,1,2,1,1);
    tablayout->setColumnStretch(2,1);
    tablayout->setColumnStretch(0,0);
    tablayout->setColumnStretch(1,0);
    tablayout->setRowStretch(1,1);
    tablayout->setRowStretch(0,0);
    CDNCurve->setXMax(255);
    CDNCurve->setYMax(255);
    CDNCurve->setAxisRange(0, 255, 0, 255);
    CDNCurve->setAutoAdjustAxisRange(false);
    CDNCurve->setCurveFittingType(CurveFittingType::NONE);
    for(QPointer<QLabelEditGroup> item:CDNS_edit)
        connect(item,&QLabelEditGroup::releaseLabel,this,[=](){QTabWndComm::recvEdit(item->Label()+"CDNS");});
    for(QPointer<QLabelCheckBox> item:CDNS_box){
        connect(item,&QLabelCheckBox::release,this,[=](){QTabWndComm::recvEdit(item->Label()+"CDNS");});
        connect(item,&QLabelCheckBox::checkSignal,this,[=](){recvCheckBox(item->Label()+"CDNS",item->checkState());});
    }

}

void QDNSTab::init3DDNS(QPointer<QWidget> subTab) 
{ 
    QPointer<QGridLayout> tablayout = new QGridLayout;
    subTab->setLayout(tablayout);
    QPointer<QLabelCheckBox> enbox = new QLabelCheckBox;
    enbox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    enbox->setLabel("Enable");
    enbox->setMouseTracking(true);
    // enbox->setSpacing(18);
    tablayout->addWidget(enbox, 1, 0);
    _3D_box.push_back(enbox);
    QPointer<QLabelEditGroup> editCurGain = new QLabelEditGroup;
    editCurGain->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    editCurGain->setLabelValue("Cur Gain");
    // editCurGain->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    editCurGain->setEditable(false);
    editCurGain->setMouseTracking(true);
    tablayout->addWidget(editCurGain, 3, 0);
    _3D_edit.push_back(editCurGain);
    QPointer<QLabelEditGroup> editStartGain = new QLabelEditGroup;
    editStartGain->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    editStartGain->setLabelValue("Start Gain");
    // editStartGain->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    editStartGain->setEditable(false);
    editStartGain->setMouseTracking(true);
    tablayout->addWidget(editStartGain, 5, 0);
    _3D_edit_shift.push_back(editStartGain);

    // Initialize the 3D DNS group box
    QStringList grouplist;
    grouplist  << "K(Slope)" << "B(Intercept)";
    std::vector<QPointer<QVBoxLayout>> DNS3D_Group;
    for(int i = 0; i < grouplist.size(); ++i)
    {
        QPointer<QGroupBox> box = new QGroupBox(grouplist.at(i));
        tablayout->addWidget(box, 0, i + 1, 15, 1);
        QPointer<QVBoxLayout> boxlayout=new QVBoxLayout;
        boxlayout->setSpacing(0);
        box->setLayout(boxlayout);
        DNS3D_Group.push_back(boxlayout);
    }

    QStringList strNameList;
    strNameList  << "Slope" << "Intercept";
    for(int i = 0; i < strNameList.size(); ++i)
    {
        for(int j = 0; j < 14; ++j)
        {
            QPointer<QLabelEditGroup> edit = new QLabelEditGroup;
            edit->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
            edit->setEditable(false);
            edit->setLabelValue(strNameList.at(i) + QString::number(j));
            edit->setMouseTracking(true);
            // if (i == 0)
            //     _3D_edit_shift.push_back(edit);
            // else
            _3D_edit.push_back(edit);
            DNS3D_Group.at(i)->addWidget(edit);
        }
    }

    for(QPointer<QLabelEditGroup> item:_3D_edit)
        connect(item,&QLabelEditGroup::releaseLabel,this,[=](){QTabWndComm::recvEdit(item->Label()+"3D");});
    for(QPointer<QLabelEditGroup> item:_3D_edit_shift)
        connect(item,&QLabelEditGroup::releaseLabel,this,[=](){QTabWndComm::recvEdit(item->Label()+"3D");});
    for(QPointer<QLabelCheckBox> item:_3D_box){
        connect(item,&QLabelCheckBox::release,this,[=](){QTabWndComm::recvEdit(item->Label()+"3D");});
        connect(item,&QLabelCheckBox::checkSignal,this,[=](){recvCheckBox(item->Label()+"3D",item->checkState());});
    }
    // Add additional initialization for 3D DNS if needed
}

QString QDNSTab::getStr()
{
    if(typebox.get()){
        if(typebox->currentIndex()==0)
            return "L";
        if(typebox->currentIndex()==1)
            return "M";
        if(typebox->currentIndex()==2)
            return "S";
        if(typebox->currentIndex()==3)
            return "V";
    }
    return "";
}

void QDNSTab::readPoint(std::vector<QPoint> &points)
{
    int valTemp = 0;
    QUsbCommunication::getInstance().readBurst(IR_RGB_MODE_REG_ADDR, (unsigned char*)(&valTemp), ADDRESS_LENGTH, VALUE_LENGTH);
    if(tabWidget->currentIndex()==1){
        for(int i=0;i<points.size();i++){
            QString pointType=YUVbox->currentText()+QString::number(i)+"YUV";
            if (DNS_RGBIR_ENABLE_MAP.contains({moduleName.c_str(), pointType}) && valTemp == 0x01) 
            {
                std::string strSuffix = strRgbIrMode == "RGB" ? "_RGB_AB" : "_IR_AB";
                pointType = pointType + strSuffix.c_str();
            }
            uint32_t val=0;
            for (auto item:combinedRegisterMap.value(pointType)){
                readCombinedRegisterValue(item,val);
            }
            points.at(i).setX(pxList.at(i).toInt());
            points.at(i).setY(val);
        }
        curveMap.insert(YUVbox->currentText(),points);
    }
    if(tabWidget->currentIndex()==2){
        for(int i=0;i<points.size();i++){
            QString pointType=CDNSbox->currentText()+QString::number(i)+"CDNS";
            uint32_t val=0;
            for (auto item:combinedRegisterMap.value(pointType)){
                readCombinedRegisterValue(item,val);
            }
            points.at(i).setX(pxList.at(i).toInt());
            points.at(i).setY(val);
        }
        curveMap2.insert(CDNSbox->currentText(),points);
    }
}

void QDNSTab::changeTab(int index)
{
    switch (index) {
    case 0:
        updateUIValue<std::vector<QPointer<QLabelEditGroup>>>(Raw_edit,getStr());
        updateUIValue<std::vector<QPointer<QLabelCheckBox>>>(Raw_box,getStr());
        break;
    case 1:
        updateUIValue<std::vector<QPointer<QLabelEditGroup>>>(YUV_edit,"YUV");
        updateUIValue<std::vector<QPointer<QLabelCheckBox>>>(YUV_box,"YUV");
        readCurve();
        break;
    case 2:
        updateUIValue<std::vector<QPointer<QLabelEditGroup>>>(CDNS_edit,"CDNS");
        updateUIValue<std::vector<QPointer<QLabelCheckBox>>>(CDNS_box,"CDNS");
        readCurve();
        break;
    case 3:
        updateUIValue<std::vector<QPointer<QLabelEditGroup>>>(_3D_edit,"3D");
        updateUIValueLeftShift<std::vector<QPointer<QLabelEditGroup>>>(_3D_edit_shift, "3D", 6);
        updateUIValue<std::vector<QPointer<QLabelCheckBox>>>(_3D_box,"3D");
        break;
    default:
        break;
    }
}

void QDNSTab::readCurve()
{
    int valTemp = 0;
    QUsbCommunication::getInstance().readBurst(IR_RGB_MODE_REG_ADDR, (unsigned char*)(&valTemp), ADDRESS_LENGTH, VALUE_LENGTH);
    if(tabWidget->currentIndex()==1){
        disconnect(YUVCurve, &QCurveChartGroup::signalPointChanged, this, nullptr);
        auto iter=curveMap.find(YUVbox->currentText());
        readPoint(iter.value());
        YUVCurve->setPoints(iter.value());
        connect(
            YUVCurve, &QCurveChartGroup::signalPointChanged, this,
            [=](QPoint p) {
                QString pointType=YUVbox->currentText()+QString::number(YUVCurve->getHighlightPointIndex())+"YUV";
                if (DNS_RGBIR_ENABLE_MAP.contains({moduleName.c_str(), pointType}) && valTemp == 0x01) 
                {
                    std::string strSuffix = strRgbIrMode == "RGB" ? "_RGB_AB" : "_IR_AB";
                    pointType = pointType + strSuffix.c_str();
                }
                if(curveMap.value(YUVbox->currentText()).at(YUVCurve->getHighlightPointIndex()).y()!=p.y()){
                    for (auto item:combinedRegisterMap.value(pointType))
                        writeCombinedRegisterValue(item, p.y());
                    auto iter=curveMap.find(YUVbox->currentText());
                    iter.value().at(YUVCurve->getHighlightPointIndex()).setY(p.y());
                };
                updateRegisterTable(combinedRegisterMap.value(pointType));
            },
            Qt::QueuedConnection);
    }
    if(tabWidget->currentIndex()==2){
        disconnect(CDNCurve, &QCurveChartGroup::signalPointChanged, this, nullptr);
        auto iter=curveMap2.find(CDNSbox->currentText());
        readPoint(iter.value());
        CDNCurve->setPoints(iter.value());
        connect(
            CDNCurve, &QCurveChartGroup::signalPointChanged, this,
            [=](QPoint p) {
                QString pointType=CDNSbox->currentText()+QString::number(CDNCurve->getHighlightPointIndex())+"CDNS";
                if(curveMap2.value(CDNSbox->currentText()).at(CDNCurve->getHighlightPointIndex()).y()!=p.y()){
                    for (auto item:combinedRegisterMap.value(pointType))
                        writeCombinedRegisterValue(item, p.y());
                    auto iter=curveMap2.find(CDNSbox->currentText());
                    iter.value().at(CDNCurve->getHighlightPointIndex()).setY(p.y());
                };
                updateRegisterTable(combinedRegisterMap.value(pointType));
            },
            Qt::QueuedConnection);
    }
}

void QDNSTab::recvEdit(QString str)
{
    labelSpinSliderCheckBoxGroup->setSpinSliderEnable(true);
    labelSpinSliderCheckBoxGroup->setBitCheckBoxEnable(true);
    if (DNS_RGBIR_ENABLE_MAP.contains({moduleName.c_str(), str})) 
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
