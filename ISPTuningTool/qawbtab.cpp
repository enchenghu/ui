#include "qawbtab.h"
#include <QComboBox>
#include <cstdint>
#include <functional>
#include <vector>
QList<QString>                                       TAB_NAME_LIST = { "AWB Ctrl", "Green&SKY", "Low CT", "LFS" };
QAWBTab::QAWBTab(QWidget *parent)
    : QTabWndComm(parent)
{
    moduleName="AWB";
    tabWidget = new QTabWidget();
    comboxLFSTrans=new QLabelComboBox;
    combox=new QLabelComboBox;
    CurCTEidt=new QLabelEditGroup;
    init();
    initConnect();
}
QAWBTab::~QAWBTab(){}

void QAWBTab::init()
{
    addTabWidget(tabWidget);
    std::map<QString, std::function<void(QWidget *)>> initFuncList;
    if(m_nLevel==LEVEL_ADMINISTRATOR)
    {
        initFuncList = {
            { "AWB Ctrl", std::bind(&QAWBTab::initAWBCtrl, this, std::placeholders::_1) },
            { "Green&SKY", std::bind(&QAWBTab::initGreenSKY, this, std::placeholders::_1) },
            { "Low CT", std::bind(&QAWBTab::initLowCT, this, std::placeholders::_1) },
            { "LFS", std::bind(&QAWBTab::initLFS, this, std::placeholders::_1) }
        };
    }
    else
    {
        initFuncList = {
            { "AWB Ctrl", std::bind(&QAWBTab::initAWBCtrl, this, std::placeholders::_1) },
            { "LFS", std::bind(&QAWBTab::initLFS, this, std::placeholders::_1) }
        };
        TAB_NAME_LIST = { "AWB Ctrl", "LFS" };
    }
    int iCount = 0;
    for (const auto &name : TAB_NAME_LIST) {
        QPointer<QWidget> subTab = new QWidget(this);
        tabWidget->addTab(subTab, name);
        initFuncList.at(name)(subTab);
        subPageMap.insert(iCount, name);
        iCount++;
    }
    recordUIRegister();
    recordUIRegSubPageInfo();
    addRegisterTable();
}
void QAWBTab::initConnect()
{
    connect(tabWidget,&QTabWidget::currentChanged,this,&QAWBTab::changeTab);
    connect(this,&QTabWndComm::readSignal,this,[=](){changeTab(tabWidget->currentIndex());});
    connect(this,&QTabWndComm::reConnectRead,this,[=](){recordUIRegister();recordUIRegSubPageInfo();changeTab(tabWidget->currentIndex());});
    connect(this,&QTabWndComm::updateValue,this,[=](){changeTab(tabWidget->currentIndex());});
}
void QAWBTab::recordUIRegister()
{
    combinedRegisterMap.clear();
    for (auto item : AWBedit){
        if(item->Label()=="ROI Left"||item->Label()=="ROI Right"||item->Label()=="ROI Top"||item->Label()=="ROI Bottom")
            recordCombineRegbyUIifo("AWB_STATIS_V60_A",item->Label(),nullptr);
        else
            recordCombineRegbyUIifo("AWB",item->Label(),std::make_shared<int>(getCurrentCamera()));
        }
    for (auto item : AWBbox)
        recordCombineRegbyUIifo("AWB",item->Label(),std::make_shared<int>(getCurrentCamera()));
    if(m_nLevel==LEVEL_ADMINISTRATOR)
    {
        for (auto item : Greenedit)
            recordCombineRegbyUIifo("AWB",item->Label(),std::make_shared<int>(getCurrentCamera()));
        for (auto item : Greenbox)
            recordCombineRegbyUIifo("AWB",item->Label(),std::make_shared<int>(getCurrentCamera()));
        for (auto item : Lowedit)
            recordCombineRegbyUIifo("AWB",item->Label(),std::make_shared<int>(getCurrentCamera()));
        recordCombineRegbyUIifo("AWB","FixLowCT Enable",std::make_shared<int>(getCurrentCamera()));
    }
    for (auto item : LFSedit)
        recordCombineRegbyUIifo("AWB",item->Label()+"V",std::make_shared<int>(getCurrentCamera()));
    recordCombineRegbyUIifo("AWB","LFSTransMode",std::make_shared<int>(getCurrentCamera()));
    for (auto item : LFSbox)
        recordCombineRegbyUIifo("AWB",item->Label(),std::make_shared<int>(getCurrentCamera()));
    // saveCombinedReg();
}

std::vector<CombinedRegister> QAWBTab::combinedRegisterSubPageRecording(QString strModuleName, QString strUiName, std::shared_ptr<int> ptrCamera, QString strSubPage) 
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

std::vector<CombinedRegister> QAWBTab::getCombinedRegister(QString strModuleName, QString strUiName, std::shared_ptr<int> ptrCamera)
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

void QAWBTab::recordUIRegSubPageInfo()
{
    // AWB Ctrl
    for (auto item : AWBedit){
        if(item->Label()=="ROI Left"||item->Label()=="ROI Right"||item->Label()=="ROI Top"||item->Label()=="ROI Bottom")
            combinedRegisterSubPageRecording("AWB_STATIS_V60_A", item->Label(), nullptr, "AWB Ctrl");
        else
            combinedRegisterSubPageRecording("AWB", item->Label(), std::make_shared<int>(getCurrentCamera()), "AWB Ctrl");
    }
    for (auto item : AWBbox)
        combinedRegisterSubPageRecording("AWB", item->Label(), std::make_shared<int>(getCurrentCamera()), "AWB Ctrl");
        
    // Green&SKY
    if(m_nLevel==LEVEL_ADMINISTRATOR)
    {
        for (auto item : Greenedit)
            combinedRegisterSubPageRecording("AWB", item->Label(), std::make_shared<int>(getCurrentCamera()), "Green&SKY");
        for (auto item : Greenbox)
            combinedRegisterSubPageRecording("AWB", item->Label(), std::make_shared<int>(getCurrentCamera()), "Green&SKY");
        for (auto item : Lowedit)
            combinedRegisterSubPageRecording("AWB", item->Label(), std::make_shared<int>(getCurrentCamera()), "Low CT");
        combinedRegisterSubPageRecording("AWB", "FixLowCT Enable", std::make_shared<int>(getCurrentCamera()), "Low CT");
    }
    
    // LFS
    for (auto item : LFSedit)
        combinedRegisterSubPageRecording("AWB", item->Label()+"V", std::make_shared<int>(getCurrentCamera()), "LFS");
    combinedRegisterSubPageRecording("AWB", "LFSTransMode", std::make_shared<int>(getCurrentCamera()), "LFS");
    for (auto item : LFSbox)
        combinedRegisterSubPageRecording("AWB", item->Label(), std::make_shared<int>(getCurrentCamera()), "LFS");
}

void QAWBTab::initAWBCtrl(QPointer<QWidget> subTab)
{
    QPointer<QGridLayout> glayout=new QGridLayout;
    subTab->setLayout(glayout);
    glayout->setHorizontalSpacing(150);
    QStringList strc=QStringList()<<"Manual AWB"<<"StepChange Enable"<<"Freeze AWB";
    QStringList strl=QStringList()<<"Man B Gain"<<"Man G Gain"<<"Man R Gain"<<"MaxAWBGain 0"<<"MaxAWBGain 1"<<"MinAWBGain 0"<<"MinAWBGain 1"
                                     <<""<<"Convergence"<<"ROI Left"<<"ROI Right"<<"ROI Top"<<"ROI Bottom";
    QStringList rowl=QStringList()<<"1"<<"2"<<"3"<<"4"<<"5"<<"6"<<"7"<<"1"<<"3"<<"4"<<"5"<<"6"<<"7";
    QStringList rowc=QStringList()<<"0"<<"2"<<"1";
    for(int i=0;i<strl.size();i++){
        if(i==7)
            continue;
        QPointer<QLabelEditGroup> edit=new QLabelEditGroup;
        edit->setEditable(false);
        edit->setMouseTracking(true);
        edit->setLabelValue(strl.at(i));
        glayout->addWidget(edit,rowl.at(i).toInt(),(i<7?0:1));
        AWBedit.push_back(edit);
        connect(edit,&QLabelEditGroup::releaseLabel,this,[=](){recvEdit(edit->Label());});
    }
    for(int i=0;i<strc.size();i++){
        QPointer<QLabelCheckBox> box=new QLabelCheckBox;
        box->setMouseTracking(true);
        box->setLabel(strc.at(i));
        glayout->addWidget(box,rowc.at(i).toInt(),(i>0?1:0));
        AWBbox.push_back(box);
        connect(box,&QLabelCheckBox::release,this,[=](){recvEdit(box->Label());});
        connect(box,&QLabelCheckBox::boxClicked,this,[=](){recvCheckBox(box->Label(),box->checkState());});
    }
}
void QAWBTab::initGreenSKY(QPointer<QWidget> subTab)
{
    QPointer<QGridLayout> glayout=new QGridLayout;
    subTab->setLayout(glayout);
    QStringList strc=QStringList()<<"GreenOption Enable"<<"SkyOption Enable";
    QStringList strl=QStringList()<<"Green Luma 0"<<"Green Luma 1"<<"Green Luma 2"<<"Green Luma 3"<<"Sky BG"
                                     <<"GreenCountRatioThr"<<"Green FixedBG 0"<<"Green FixedBG 1"<<"Green FixedBG 2"<<"Green FixedBG 3"<<"SkyLuma Thr"<<"Sky RG"
                                     <<"Green FixedRG 0"<<"Green FixedRG 1"<<"Green FixedRG 2"<<"Green FixedRG 3";
    QStringList rowc=QStringList()<<"0"<<"5";
    QStringList rowl=QStringList()<<"1"<<"2"<<"3"<<"4"<<"6"
                                     <<"0"<<"1"<<"2"<<"3"<<"4"<<"5"<<"6"
                                     <<"1"<<"2"<<"3"<<"4";
    for(int i=0;i<strl.size();i++){
        QPointer<QLabelEditGroup> edit=new QLabelEditGroup;
        edit->setEditable(false);
        edit->setMouseTracking(true);
        edit->setLabelValue(strl.at(i));
        int col;
        if(i<5)
            col=0;
        if(i>=5&&i<12)
            col=1;
        if(i>=12)
            col=2;
        glayout->addWidget(edit,rowl.at(i).toInt(),col);
        Greenedit.push_back(edit);
        connect(edit,&QLabelEditGroup::releaseLabel,this,[=](){recvEdit(edit->Label());});
    }
    for(int i=0;i<strc.size();i++){
        QPointer<QLabelCheckBox> box=new QLabelCheckBox;
        box->setMouseTracking(true);
        box->setLabel(strc.at(i));
        glayout->addWidget(box,rowc.at(i).toInt(),0);
        Greenbox.push_back(box);
        connect(box,&QLabelCheckBox::release,this,[=](){recvEdit(box->Label());});
        connect(box,&QLabelCheckBox::checkSignal,this,[=](){recvCheckBox(box->Label(),box->checkState());});
    }


    QPointer<QLabelEditGroup> CurLuma      = new QLabelEditGroup();
    CurLuma->setLabelValue("Cur Luma");
    CurLuma->setValue(0);
    connect(this, &QTabWndComm::readSignal, CurLuma, [CurLuma]() {
        if (!CurLuma->isVisible())
            return;
        auto gainL     = QUiDataMiddleware("AWB", "Current Gain L").readValueFromDevice();
        auto expoL     = QUiDataMiddleware("AWB", "Current Expo L").readValueFromDevice();
        auto curLumaVal = (static_cast<uint64_t>(gainL) * (static_cast<uint64_t>(expoL))) >> 8;
        CurLuma->setValue(curLumaVal);
    });
    glayout->addWidget(CurLuma,0,2);

}

void QAWBTab::initLowCT(QPointer<QWidget> subTab)
{
    QPointer<QGridLayout> glayout=new QGridLayout;
    subTab->setLayout(glayout);
    QStringList strl=QStringList()<<"LowCTLumaThr"<<"FixLowCTList 0"<<"FixLowCTList 1"<<"FixLowCTList 2"<<"FixLowCTList 3"<<"FixLowCTList 4"
                                     <<"FixLowBGList 0"<<"FixLowBGList 1"<<"FixLowBGList 2"<<"FixLowBGList 3"<<"FixLowBGList 4"
                                     <<"FixLowRGList 0"<<"FixLowRGList 1"<<"FixLowRGList 2"<<"FixLowRGList 3"<<"FixLowRGList 4"
                                     <<"StyleCTList 0"<<"StyleCTList 1"<<"StyleBG 0"<<"StyleBG 1"<<"StyleRG 0"<<"StyleRG 1";
    QPointer<QGroupBox> group1=new QGroupBox("Mode1");
    QPointer<QGroupBox> group2=new QGroupBox("Mode2");
    QPointer<QGridLayout> glayout1=new QGridLayout;
    QPointer<QGridLayout> glayout2=new QGridLayout;
    group1->setLayout(glayout1);
    group2->setLayout(glayout2);
    combox->setValue("FixLowCT Enable",QStringList()<<"0"<<"1"<<"2");
    combox->setMouseTracking(true);
    connect(combox,&QLabelComboBox::release,this,[=](){recvEdit(combox->Label());});
    connect(combox,&QLabelComboBox::indexChange,this,[=](){
        for(int i=0;i<combinedRegisterMap.value(combox->Label()).size();i++)
            writeCombinedRegisterValue(combinedRegisterMap.value(combox->Label()).at(i),combox->getCurrentIndex());
        updateRegisterTable(combinedRegisterMap.value(combox->Label()));
    });
    glayout->addWidget(combox,0,0);
    glayout->addItem(new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Expanding),0,2);
    glayout->addWidget(group1,2,0,1,3);
    glayout->addWidget(group2,3,0,2,3);


    QPointer<QLabelEditGroup> CurLuma      = new QLabelEditGroup();
    CurLuma->setLabelValue("Cur Luma");
    CurLuma->setValue(0);
    connect(this, &QTabWndComm::readSignal, CurLuma, [CurLuma]() {
        if (!CurLuma->isVisible())
            return;
        auto gainL     = QUiDataMiddleware("AWB", "Current Gain L").readValueFromDevice();
        auto expoL     = QUiDataMiddleware("AWB", "Current Expo L").readValueFromDevice();
        auto curLumaVal = (static_cast<uint64_t>(gainL) * (static_cast<uint64_t>(expoL))) >> 8;
        CurLuma->setValue(curLumaVal);
    });
    glayout->addWidget(CurLuma,0,2);

    CurCTEidt->setLabelValue("CurCT");
    CurCTEidt->setEditable(false);
    glayout->addWidget(CurCTEidt,1,0);


    for(int i=0;i<strl.size();i++){
        QPointer<QLabelEditGroup> edit=new QLabelEditGroup;
        edit->setEditable(false);
        edit->setMouseTracking(true);
        edit->setLabelValue(strl.at(i));
        if(i==0)
            glayout->addWidget(edit,0,1);
        if(i>0&&i<16)
            glayout1->addWidget(edit,i-(std::ceil(i/5.0)-1)*5,(std::ceil(i/5.0)-1));
        if(i>=16)
            glayout2->addWidget(edit,(i-15)-(std::ceil((i-15)/2.0)-1)*2,(std::ceil((i-15)/2.0)-1));
        Lowedit.push_back(edit);
        connect(edit,&QLabelEditGroup::releaseLabel,this,[=](){recvEdit(edit->Label());});
    }
}

void QAWBTab::initLFS(QPointer<QWidget> subTab)
{
    QPointer<QGridLayout> glayout=new QGridLayout;
    subTab->setLayout(glayout);
    QStringList strl=QStringList()<<"LFSTransMode"<<"Man B Gain"<<"Man G Gain"<<"Man R Gain"
                                     <<"MaxAWBGain 0"<<"MaxAWBGain 1"<<"MinAWBGain 0"<<"MinAWBGain 1";
    if(m_nLevel == LEVEL_ADMINISTRATOR)
    {
        strl<<"LFSTransCTList 0"<<"LFSTransCTList 1"<<"LFSTransCTList 2"<<"LFSTransCTList 3"<<"BGRatioList 0"<<"BGRatioList 1"<<"BGRatioList 2"<<"BGRatioList 3"
        <<"RGRatioList 0"<<"RGRatioList 1"<<"RGRatioList 2"<<"RGRatioList 3"
        <<"LFSTransBGList 0"<<"LFSTransBGList 1"<<"LFSTransBGList 2"<<"LFSTransBGList 3"
        <<"LFSTransRGList 0"<<"LFSTransRGList 1"<<"LFSTransRGList 2"<<"LFSTransRGList 3";
    }
    QPointer<QGroupBox> group1=new QGroupBox("Manual");
    QPointer<QGroupBox> group2=new QGroupBox("Auto");
    QPointer<QVBoxLayout> vlayout1=new QVBoxLayout;
    QPointer<QVBoxLayout> vlayout2=new QVBoxLayout;
    group1->setLayout(vlayout1);
    group2->setLayout(vlayout2);
    vlayout1->setSpacing(2);
    vlayout2->setSpacing(2);

    QPointer<QLabelCheckBox> box=new QLabelCheckBox;
    box->setMouseTracking(true);
    box->setLabel("ManualAWB En");
    LFSbox.push_back(box);
    vlayout1->addWidget(box);
    connect(box,&QLabelCheckBox::release,this,[=](){recvEdit(box->Label());});
    connect(box,&QLabelCheckBox::checkSignal,this,[=](){recvCheckBox(box->Label(),box->checkState());});

    comboxLFSTrans->setValue("LFSTransMode",QStringList()<<"0"<<"1");
    comboxLFSTrans->setMouseTracking(true);
    connect(comboxLFSTrans,&QLabelComboBox::release,this,[=](){recvEdit(comboxLFSTrans->Label());});
    connect(comboxLFSTrans,&QLabelComboBox::indexChange,this,[=](){
        for(int i=0;i<combinedRegisterMap.value(comboxLFSTrans->Label()).size();i++)
            writeCombinedRegisterValue(combinedRegisterMap.value(comboxLFSTrans->Label()).at(i),comboxLFSTrans->getCurrentIndex());
        updateRegisterTable(combinedRegisterMap.value(comboxLFSTrans->Label()));
    });
    glayout->addWidget(comboxLFSTrans,0,0);



    glayout->addItem(new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Preferred),0,1);
    glayout->addWidget(group1,1,0,4,1);
    glayout->addWidget(group2,5,0,4,1);
    for(int i=1;i<strl.size();i++){
        QPointer<QLabelEditGroup> edit=new QLabelEditGroup;
        edit->setEditable(false);
        edit->setMouseTracking(true);
        edit->setLabelValue(strl.at(i));
        if(i<1)
            glayout->addWidget(edit,0,0);
        if(i>=1&&i<4)
            vlayout1->addWidget(edit);
        if(i>=4&&i<8)
            vlayout2->addWidget(edit);
        if(i>=8&&i<20)
            glayout->addWidget(edit,(i-8)-(std::ceil((i-7)/4.0)-1)*4,(std::ceil((i-7)/4.0)));
        if(i>=20&&i<28)
            glayout->addWidget(edit,(i-15)-(std::ceil((i-19)/4.0)-1)*4,(std::ceil((i-19)/4.0)));
        LFSedit.push_back(edit);
        connect(edit,&QLabelEditGroup::releaseLabel,this,[=](){recvEdit(edit->Label()+"V");});
    }
}
void QAWBTab::updatebox()
{
    uint32_t val=0;
    for(int i=0;i<combinedRegisterMap.value(combox->Label()).size();i++)
        readCombinedRegisterValue(combinedRegisterMap.value(combox->Label()).at(i),val);
    if(val==3)
        combox->setCurrentIndex(2);
    else
        combox->setCurrentIndex(val);

    for(int i=0;i<combinedRegisterMap.value(comboxLFSTrans->Label()).size();i++)
        readCombinedRegisterValue(combinedRegisterMap.value(comboxLFSTrans->Label()).at(i),val);
    if(val==0)
        comboxLFSTrans->setCurrentIndex(0);
    else
        comboxLFSTrans->setCurrentIndex(1);

}
void QAWBTab::changeTab(int index)
{
    switch (index) {
    case 0:
        updateUIValue<std::vector<QPointer<QLabelEditGroup>>>(AWBedit);
        updateUIValue<std::vector<QPointer<QLabelCheckBox>>>(AWBbox);
        break;
    case 1:
        if(m_nLevel==LEVEL_ADMINISTRATOR)
        {
            updateUIValue<std::vector<QPointer<QLabelEditGroup>>>(Greenedit);
            updateUIValue<std::vector<QPointer<QLabelCheckBox>>>(Greenbox);
            break;
        }
        updateUIValue<std::vector<QPointer<QLabelEditGroup>>>(LFSedit,"V");
        updateUIValue<std::vector<QPointer<QLabelCheckBox>>>(LFSbox);
        break;
    case 2:
        updateUIValue<std::vector<QPointer<QLabelEditGroup>>>(Lowedit);
        break;
    case 3:
        updateUIValue<std::vector<QPointer<QLabelEditGroup>>>(LFSedit,"V");
        updateUIValue<std::vector<QPointer<QLabelCheckBox>>>(LFSbox);
        break;
    default:
        break;
    }
    updatebox();


    uint32_t gainB      = QUiDataMiddleware("AWB", "Current Gain B").readValueFromDevice();
    uint32_t gainR      = QUiDataMiddleware("AWB", "Current Gain R").readValueFromDevice();
    UINT _CTCurrent =0;
    if(gainB!=0&&gainR!=0){
        double fCurCT = ((float)gainB) / ((float)gainR);
        _CTCurrent = (UINT)(fCurCT * 256);
    }
    if(CurCTEidt)
        CurCTEidt->setValue(QString("0x%1").arg(QString::number(_CTCurrent, 16)));
}
