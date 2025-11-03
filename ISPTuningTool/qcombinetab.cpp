#include "qcombinetab.h"
#include "Common/UI/quidatamiddleware.h"

#include <Common/Others/qusbcommunication.h>

QCombineTab::QCombineTab(QWidget *parent)
{
    tabComm=new QTabWidget();//控制主界面唯一tab
    moduleName="Combine";
    init();
    initConnect();
}

QCombineTab::~QCombineTab()
{

}

void QCombineTab::init()
{
    addTabWidget(tabComm);
    QStringList tabList;
    if(m_nLevel==LEVEL_ADMINISTRATOR)
    {
        tabList<<"Combine"<<"H&C"<<"DarkColor"<<"DoubleEdge"<<"LFSLED"/*<<"Ghost"*/;
    }
    else
    {
        tabList<<"H&C";
    }
    // int iCount = 0;
    for(int i=0;i<tabList.size();i++){
        QPointer<QWidget> _tab=new QWidget;
        tabComm->insertTab(i,_tab,tabList.at(i));
        tabMap.insert(tabList.at(i),std::make_pair(_tab,false));
        subPageMap.insert(i, tabList[i]);
    }
    for(int i=0;i<tabList.size();i++)
        changeTab(i);
    recordUIRegister();
    recordUIRegSubPageInfo();
    addRegisterTable();
}

void QCombineTab::updateValue()
{
    changeTab(tabComm->currentIndex());
}

void QCombineTab::initConnect()
{
    connect(tabComm,&QTabWidget::currentChanged,this,&QCombineTab::changeTab);
    connect(this,&QTabWndComm::readSignal,this,[=](){updateValue();});
    connect(this,&QTabWndComm::reConnectRead,this,[=](){recordUIRegister();recordUIRegSubPageInfo();updateValue();});
    connect(this,&QTabWndComm::updateValue,this,[=](){updateValue();});
}

void QCombineTab::recordUIRegister()
{
    combinedRegisterMap.clear();
    //Combine
    recordCombineRegbyUIifo("CComb_HW_V60","Data Sel",nullptr);
    recordCombineRegbyUIifo("CComb_HW_V60","High Margin",nullptr);
    recordCombineRegbyUIifo("CComb_HW_V60","Blooming_mode_en",nullptr);
    recordCombineRegbyUIifo("CComb_HW_V60","Blooming_shift1",nullptr);
    recordCombineRegbyUIifo("CComb_HW_V60","Blooming_shift2",nullptr);
    recordCombineRegbyUIifo("CComb_HW_V60","Blooming_shift3",nullptr);
    recordCombineRegbyUIifo("COMB","ISOList0",std::make_shared<int>(getCurrentCamera()));
    recordCombineRegbyUIifo("COMB","ISOList1",std::make_shared<int>(getCurrentCamera()));
    recordCombineRegbyUIifo("ISP","Data SelCombine",nullptr);
    // recordCombineRegbyUIifo("COMB","ISOList Cur",std::make_shared<int>(getCurrentCamera()));//未找到
    //HC
    for(int i=0;i<hlist.size();i++){
        if(i==3||i==7||i==11||i==15||i==19||i==23){
            recordCombineRegbyUIifo("CComb_HW_V60",hlist.at(i),nullptr);
        }else{
            recordCombineRegbyUIifo("COMB",hlist.at(i),std::make_shared<int>(getCurrentCamera()));
        }
    }
    recordCombineRegbyUIifo("CComb_HW_V60","CompS_en",nullptr);
    for(int i=0;i<complist.size();i++)
        recordCombineRegbyUIifo("COMB",complist.at(i),std::make_shared<int>(getCurrentCamera()));
    //DarkColor
    for (auto item : dar_color_box)
        recordCombineRegbyUIifo("CComb_HW_V60",item->Label(),nullptr);
    for (auto item : dark_color_edit){
        if(item->Label()=="Dark_color_maxw"||item->Label()=="Dark_color_thrw"){
            recordCombineRegbyUIifo("CComb_HW_V60",item->Label(),nullptr);
        }else if(item->Label()=="CurGain"){
            recordCombineRegbyUIifo("PipeCtrl",item->Label(),std::make_shared<int>(getCurrentCamera()));
        }else{
            recordCombineRegbyUIifo("COMB",item->Label(),std::make_shared<int>(getCurrentCamera()));
        }
    }
    //DoubleEdge
    for (auto item : doubleEdge_box)
        recordCombineRegbyUIifo("CComb_HW_V60",item->Label(),nullptr);
    for (auto item : doubleEdge_edit){
        if(item->Label()=="DoubleEdgeGainStart"||item->Label()=="DoubleEdgeGainEnd"||item->Label()=="DoubleEdgeHFMaxThr"){
            recordCombineRegbyUIifo("COMB",item->Label(),std::make_shared<int>(getCurrentCamera()));
        }else if(item->Label()=="CurGain"){
            recordCombineRegbyUIifo("PipeCtrl",item->Label(),std::make_shared<int>(getCurrentCamera()));
        }else{
            recordCombineRegbyUIifo("CComb_HW_V60",item->Label(),nullptr);
        }
    }
    //Ghost
    for (auto item : chost_box)
        recordCombineRegbyUIifo("CComb_HW_V60",item->Label(),nullptr);
    for (auto item : chost_edit)
        recordCombineRegbyUIifo("CComb_HW_V60",item->Label(),nullptr);
    //LFS LED
    recordCombineRegbyUIifo("CComb_HW_V60","Blooming_en",nullptr);
    recordCombineRegbyUIifo("COMB","AdjustLFSWeight",std::make_shared<int>(getCurrentCamera()));
    for (auto item : LFS_edit){
        if(item->Label()=="Weight Thr"||item->Label()=="Thr1"||item->Label()=="Thr2"||item->Label()=="Thr3"||item->Label()=="Thr4"||item->Label()=="Thr7"){
            recordCombineRegbyUIifo("CComb_HW_V60",item->Label(),nullptr);
        }else{
            recordCombineRegbyUIifo("COMB",item->Label(),std::make_shared<int>(getCurrentCamera()));
        }
    }
    // saveCombinedReg();
}

void QCombineTab::initCombine()
{
    if(!tabMap.value("Combine").second){
        dataSeL=new QLabelComboBox();
        dataSeL->setMouseTracking(true);
        dataSeL->setValue("Data Sel",QStringList()<<"Combine"<<"L"<<"M"<<"S"<<"LFS");
        connect(dataSeL,&QLabelComboBox::release,this,[=](){
            auto regVector=combinedRegisterMap.value("Data Sel");
            updateRegisterTable(combinedRegisterMap.value("Data Sel"));
            readCombine(1);
        });
        connect(dataSeL,&QLabelComboBox::indexChange,this,[=](){
            uint32_t value=0;
            uint32_t val_combine=0;
            switch (dataSeL->getCurrentIndex()) {
            case 0:
                value=0;
                val_combine=240;
                break;
            case 1:
                value=0;
                val_combine=144;
                break;
            case 2:
                value=1;
                val_combine=144;
                break;
            case 3:
                value=2;
                val_combine=144;
                break;
            case 4:
                value=3;
                val_combine=144;
            default:
                break;
            }
            for(int i=0;i<combinedRegisterMap.value("Data Sel").size();i++)
                writeCombinedRegisterValue(combinedRegisterMap.value("Data Sel").at(i),value);
            updateRegisterTable(combinedRegisterMap.value("Data Sel"));
            writeCombine(val_combine);
            });
        if(m_nLevel!=LEVEL_ADMINISTRATOR)
        {
            uint32_t value=0;
            auto dataSelReg=combinedRegisterMap.value("Data Sel");
            for (int i = 0; i < dataSelReg.size(); i++){
                readCombinedRegisterValue(dataSelReg.at(i), value);
                if(value==0){
                    if(readCombine(0)==240)
                        dataSeL->setCurrentIndex(0);
                    else
                        dataSeL->setCurrentIndex(1);
                }else{
                    dataSeL->setCurrentIndex(value+1);
                }
            }
            dataSeL->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
            auto iter=tabMap.find("Combine");
            auto widget=iter.value().first;
            QPointer<QGridLayout> layout=new QGridLayout;
            widget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
            layout->setHorizontalSpacing(50);
            widget->setLayout(layout);
            layout->addWidget(dataSeL,0,0);
            return;
        }
        highMargn=new QLabelEditGroup();
        highMargn->setLabelValue("High Margin");
        highMargn->setMouseTracking(true);
        highMargn->setEditable(false);
        connect(highMargn,&QLabelEditGroup::releaseLabel,this,[=](){recvEdit(highMargn->Label());});

        blooming_mode_en=new QLabelCheckBox();
        blooming_mode_en->setLabel("Blooming_mode_en");
        blooming_mode_en->setMouseTracking(true);
        connect(blooming_mode_en,&QLabelCheckBox::release,this,[=](){recvEdit(blooming_mode_en->Label());});
        connect(blooming_mode_en,&QLabelCheckBox::checkSignal,this,[=](){recvCheckBox(blooming_mode_en->Label(),blooming_mode_en->checkState());});

        auto iter=tabMap.find("Combine");
        auto widget=iter.value().first;
        QPointer<QGridLayout> layout=new QGridLayout;
        layout->setHorizontalSpacing(50);
        widget->setLayout(layout);
        layout->addWidget(dataSeL,0,0);
        layout->addWidget(highMargn,1,0);
        layout->addWidget(blooming_mode_en,0,2);
        QSplitter* line = new QSplitter(Qt::Vertical, this);
        line->setFrameShape(QFrame::WinPanel);
        line->setFixedWidth(1);
        line->setLineWidth(1);
        layout->addWidget(line,0,1,4,1);

        for(int i=0;i<3;i++){
            QPointer<QLabelEditGroup> blooming=new QLabelEditGroup();
            blooming->setLabelValue("Blooming_shift"+QString::number(i+1));
            blooming->setEditable(false);
            blooming->setMouseTracking(true);
            layout->addWidget(blooming,i+1,2);
            connect(blooming,&QLabelEditGroup::releaseLabel,this,[=](){recvEdit(blooming->Label());});
            blooming_shif.push_back(blooming);
        }
        QSpacerItem*horizontalSpacer = new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Preferred);
        layout->addItem(horizontalSpacer,0,3);
        iter.value().second=true;
    }
    uint32_t value=0;
    auto dataSelReg=combinedRegisterMap.value("Data Sel");
    for (int i = 0; i < dataSelReg.size(); i++){
        readCombinedRegisterValue(dataSelReg.at(i), value);
        if(value==0){
            if(readCombine(0)==240)
                dataSeL->setCurrentIndex(0);
            else
                dataSeL->setCurrentIndex(1);
        }else{
            dataSeL->setCurrentIndex(value+1);
        }
    }
    auto highMargnReg=combinedRegisterMap.value("High Margin");
    for (int i = 0; i < highMargnReg.size(); i++){
        readCombinedRegisterValue(highMargnReg.at(i), value);
        highMargn->setValue(QString("0x%1").arg(QString::number(value,16).toUpper().rightJustified(VALUE_HEX_INFO,'0')));
    }
    auto Blooming_shift1Reg=combinedRegisterMap.value("Blooming_shift1");
    for (int i = 0; i < Blooming_shift1Reg.size(); i++){
        readCombinedRegisterValue(Blooming_shift1Reg.at(i), value);
        blooming_shif.at(0)->setValue(QString("0x%1").arg(QString::number(value,16).toUpper().rightJustified(VALUE_HEX_INFO,'0')));
    }
    auto Blooming_shift2Reg=combinedRegisterMap.value("Blooming_shift2");
    for (int i = 0; i < Blooming_shift2Reg.size(); i++){
        readCombinedRegisterValue(Blooming_shift2Reg.at(i), value);
        blooming_shif.at(1)->setValue(QString("0x%1").arg(QString::number(value,16).toUpper().rightJustified(VALUE_HEX_INFO,'0')));
    }
    auto Blooming_shift3Reg=combinedRegisterMap.value("Blooming_shift3");
    for (int i = 0; i < Blooming_shift3Reg.size(); i++){
        readCombinedRegisterValue(Blooming_shift3Reg.at(i), value);
        blooming_shif.at(2)->setValue(QString("0x%1").arg(QString::number(value,16).toUpper().rightJustified(VALUE_HEX_INFO,'0')));
    }
    auto Blooming_mode_en=combinedRegisterMap.value("Blooming_mode_en");
    for (int i = 0; i < Blooming_mode_en.size(); i++){
        readCombinedRegisterValue(Blooming_mode_en.at(i), value);
        if(value==0){
            blooming_mode_en->setChecked(false);
        }else if(value==1){
            blooming_mode_en->setChecked(true);
        }
    }
}

void QCombineTab::initHC()
{
    if(!tabMap.value("H&C").second){
        auto iter=tabMap.find("H&C");
        auto widget=iter.value().first;
        QPointer<QGridLayout> hglayout=new QGridLayout;
        QPointer<QVBoxLayout> vlayout=new QVBoxLayout;
        QPointer<QComboBox> hist=new QComboBox;
        widget->setLayout(hglayout);
        hglayout->setVerticalSpacing(1);
        hglayout->setContentsMargins(5,5,5,0);
        hglayout->addLayout(vlayout,0,0,2,1);
        hglayout->addWidget(hist,0,1);

        if(m_nLevel!=LEVEL_ADMINISTRATOR)
        {
            dataSeL=new QLabelComboBox();
            dataSeL->setMouseTracking(true);
            dataSeL->setValue("Data Sel",QStringList()<<"Combine"<<"L"<<"M"<<"S"<<"LFS");
            connect(dataSeL,&QLabelComboBox::release,this,[=](){
                auto regVector=combinedRegisterMap.value("Data Sel");
                updateRegisterTable(combinedRegisterMap.value("Data Sel"));
                readCombine(1);
            });
            connect(dataSeL,&QLabelComboBox::indexChange,this,[=](){
                uint32_t value=0;
                uint32_t val_combine=0;
                switch (dataSeL->getCurrentIndex()) {
                case 0:
                    value=0;
                    val_combine=240;
                    break;
                case 1:
                    value=0;
                    val_combine=144;
                    break;
                case 2:
                    value=1;
                    val_combine=144;
                    break;
                case 3:
                    value=2;
                    val_combine=144;
                    break;
                case 4:
                    value=3;
                    val_combine=144;
                default:
                    break;
                }
                for(int i=0;i<combinedRegisterMap.value("Data Sel").size();i++)
                    writeCombinedRegisterValue(combinedRegisterMap.value("Data Sel").at(i),value);
                updateRegisterTable(combinedRegisterMap.value("Data Sel"));
                writeCombine(val_combine);
            });
            uint32_t value=0;
            auto dataSelReg=combinedRegisterMap.value("Data Sel");
            for (int i = 0; i < dataSelReg.size(); i++){
                readCombinedRegisterValue(dataSelReg.at(i), value);
                if(value==0){
                    if(readCombine(0)==240)
                        dataSeL->setCurrentIndex(0);
                    else
                        dataSeL->setCurrentIndex(1);
                }else{
                    dataSeL->setCurrentIndex(value+1);
                }
            }
            dataSeL->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
            vlayout->addWidget(dataSeL);
        }


        QStringList isolist;
        if(m_nLevel==LEVEL_ADMINISTRATOR)
        {
            hist->addItems(QStringList()<<"CompS"<<"Hist");
            isolist<<"ISOList0"<<"ISOList1"/*<<"ISOList Cur"*/;
        }
        else
        {
            hist->addItems(QStringList()<<"CompS");
            isolist<<"ISOList0"<<"ISOList1";
        }
        hist->setMaximumWidth(80);
        for(int i=0;i<isolist.size();++i){
            QPointer<QLabelEditGroup> _iso=new QLabelEditGroup();
            _iso->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
            _iso->setFixedWidth(450);
            iso.push_back(_iso);
            _iso->setLabelValue(isolist.at(i));
            _iso->setEditable(false);
            _iso->setMouseTracking(true);
            vlayout->addWidget(_iso);
            HC_edit.push_back(_iso);
        }
        QPointer<QLabelEditGroup> curIso = new QLabelEditGroup(widget);
        curIso->setLabelValue("Cur Iso");
        curIso->setValue(0);
        connect(this, &QTabWndComm::readSignal, this, [=]() {
            auto gainL      = QUiDataMiddleware("Combine", "Current Gain L").readValueFromDevice();
            auto expoL      = QUiDataMiddleware("Combine", "Current Expo L").readValueFromDevice();
            auto currentIso = (static_cast<uint64_t>(gainL) * (static_cast<uint64_t>(expoL) >> 4)) >> 4;
            curIso->setValue(currentIso);
        });

        // 获取"Current Gain L"和"Current Expo L"对应的combineRegisters，并调用setRegisterSubPage函数
        auto gainLCombinedRegs = QUiDataMiddleware("Combine", "Current Gain L").getCombinedRegisterList();
        for (const auto& reg : gainLCombinedRegs) {
            setRegisterSubPage(reg, "H&C");
        }
        
        auto expoLCombinedRegs = QUiDataMiddleware("Combine", "Current Expo L").getCombinedRegisterList();
        for (const auto& reg : expoLCombinedRegs) {
            setRegisterSubPage(reg, "H&C");
        }

        curIso->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        curIso->setFixedWidth(450);
        curIso->setEditable(false);
        vlayout->addWidget(curIso);

        box=new QGroupBox();
        box2=new QGroupBox();
        hglayout->addWidget(box,1,1);
        hglayout->addWidget(box2,1,1);
        box->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
        box2->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
        histLayout=new QGridLayout();
        compLayout=new QGridLayout();
        box->setLayout(histLayout);
        box2->setLayout(compLayout);
        loadHCTab();
        box->setVisible(false);
        box2->setVisible(true);
        updateValue(HC_edit,HC_box);
        connect(hist,&QComboBox::currentIndexChanged,this,[=](){
            if(hist->currentIndex()==0){
                box2->setVisible(true);
                box->setVisible(false);
            }else{
                box2->setVisible(false);
                box->setVisible(true);
            }
            updateValue(HC_edit,HC_box);});
        iter.value().second=true;
    }
    updateValue(HC_edit,HC_box);
}

void QCombineTab::initDarkColor()
{
    if(!tabMap.value("DarkColor").second){
        auto iter=tabMap.find("DarkColor");
        auto widget=iter.value().first;
        QPointer<QGridLayout> glayout=new QGridLayout;
        glayout->setContentsMargins(20,10,20,10);
        glayout->setHorizontalSpacing(50);

        widget->setLayout(glayout);
        QStringList boxlist;
        boxlist<<"Dark_color_en"<<"Dark_color_h_en";
        for(int i=0;i<boxlist.size();i++){
            QPointer<QLabelCheckBox> darkbox=new QLabelCheckBox;
            darkbox->setLabel(boxlist.at(i));
            darkbox->setMouseTracking(true);
            darkbox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
            dar_color_box.push_back(darkbox);
            glayout->addWidget(darkbox,i,0);
        }
        QStringList editlist;
        editlist<<"Dark_color_maxw"<<"Dark_color_thrw"<<"DarkColorGainStart"
                 <<"DarkColorGainEnd"<<"CurGain";
        for(int i=0;i<editlist.size();i++){
            QPointer<QLabelEditGroup> darkedit=new QLabelEditGroup;
            darkedit->setLabelValue(editlist.at(i));
            darkedit->setEditable(false);
            darkedit->setMouseTracking(true);
            dark_color_edit.push_back(darkedit);
            darkedit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
            glayout->addWidget(darkedit,i,1);
        }
        for(QPointer<QLabelEditGroup> item:dark_color_edit)
            connect(item,&QLabelEditGroup::releaseLabel,this,[=](){recvEdit(item->Label());});
        for(QPointer<QLabelCheckBox> item:dar_color_box){
            connect(item,&QLabelCheckBox::release,this,[=](){recvEdit(item->Label());});
            connect(item,&QLabelCheckBox::checkSignal,this,[=](){recvCheckBox(item->Label(),item->checkState());});
        }
        iter.value().second=true;
    }
    updateValue(dark_color_edit,dar_color_box);
}

void QCombineTab::initDoubleEdge()
{
    if(!tabMap.value("DoubleEdge").second){
        auto iter=tabMap.find("DoubleEdge");
        auto widget=iter.value().first;
        QPointer<QGridLayout> glayout=new QGridLayout;
        widget->setLayout(glayout);
        glayout->setContentsMargins(20,10,20,10);
        glayout->setHorizontalSpacing(50);
        QStringList boxlist;
        boxlist<<"Lamp_edge_en"<<"Lamp_edge2_en"<<"Lamp_hf_en"<<"Lamp_hf2_en";
        for(int i=0;i<boxlist.size();i++){
            QPointer<QLabelCheckBox> box=new QLabelCheckBox;
            box->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
            box->setLabel(boxlist.at(i));
            box->setMouseTracking(true);
            doubleEdge_box.push_back(box);
            glayout->addWidget(box,i,0);
        }
        QStringList editlist;
        editlist<<"I_thr"<<"I_thr2"<<"DoubleEdgeGainStart"
                 <<"DoubleEdgeGainEnd"<<"DoubleEdgeHFMaxThr"
                 <<"Lamp_w"<<"Lhf_thr"<<"Lhf_thr2"<<"CurGain";
        for(int i=0;i<editlist.size();i++){
            QPointer<QLabelEditGroup> edit=new QLabelEditGroup;
            edit->setLabelValue(editlist.at(i));
            edit->setMouseTracking(true);
            edit->setEditable(false);
            doubleEdge_edit.push_back(edit);
            edit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
            if(i<=1){
                glayout->addWidget(edit,i+4,0);
            }else if(i>1&&i<8){
                glayout->addWidget(edit,i-2,1);
            }else{
                glayout->addWidget(edit,i-8,2);
            }
        }
        for(QPointer<QLabelEditGroup> item:doubleEdge_edit)
            connect(item,&QLabelEditGroup::releaseLabel,this,[=](){recvEdit(item->Label());});
        for(QPointer<QLabelCheckBox> item:doubleEdge_box){
            connect(item,&QLabelCheckBox::release,this,[=](){recvEdit(item->Label());});
            connect(item,&QLabelCheckBox::checkSignal,this,[=](){recvCheckBox(item->Label(),item->checkState());});
        }
        iter.value().second=true;
    }
    updateValue(doubleEdge_edit,doubleEdge_box);
}

void QCombineTab::initGhost()
{
    if(!tabMap.value("Ghost").second){
        auto iter=tabMap.find("Ghost");
        auto widget=iter.value().first;
        QPointer<QGridLayout> glayout=new QGridLayout;
        widget->setLayout(glayout);
        glayout->setContentsMargins(20,10,20,10);
        glayout->setHorizontalSpacing(50);
        QStringList boxlist;
        boxlist<<"Ghost_en"<<"Ghost_opt2_en"<<"Ghostcheckl_en";
        for(int i=0;i<boxlist.size();i++){
            QPointer<QLabelCheckBox> box=new QLabelCheckBox;
            box->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
            box->setLabel(boxlist.at(i));
            box->setMouseTracking(true);
            chost_box.push_back(box);
            if(i==2){
                glayout->addWidget(box,0,1);
            }else{
                glayout->addWidget(box,i,0);
            }
        }
        QStringList editlist;
        editlist<<"Ghost_thr"<<"Ghost_thr2"<<"Ghost_slop"
                 <<"Ghost_Ithr"<<"Ghost_Ithr2"<<"Ghost_Ithr3"<<"Ghost_lslop"
                 <<"Ghost_opt"<<"Ghost_mode2";
        for(int i=0;i<editlist.size();i++){
            QPointer<QLabelEditGroup> edit=new QLabelEditGroup;
            edit->setLabelValue(editlist.at(i));
            edit->setMouseTracking(true);
            edit->setEditable(false);
            chost_edit.push_back(edit);
            edit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
            if(i<=2){
                glayout->addWidget(edit,i+2,0);
            }else if(i>2&&i<7){
                glayout->addWidget(edit,i-2,1);
            }else{
                glayout->addWidget(edit,i-7,2);
            }
        }
        for(QPointer<QLabelEditGroup> item:chost_edit)
            connect(item,&QLabelEditGroup::releaseLabel,this,[=](){recvEdit(item->Label());});
        for(QPointer<QLabelCheckBox> item:chost_box){
            connect(item,&QLabelCheckBox::release,this,[=](){recvEdit(item->Label());});
            connect(item,&QLabelCheckBox::checkSignal,this,[=](){recvCheckBox(item->Label(),item->checkState());});
        }
        iter.value().second=true;
    }
    updateValue(chost_edit,chost_box);
}

void QCombineTab::initLFSLED()
{
    if(!tabMap.value("LFSLED").second){
        auto iter=tabMap.find("LFSLED");
        auto widget=iter.value().first;
        QPointer<QGridLayout> glayout=new QGridLayout;
        widget->setLayout(glayout);
        glayout->setContentsMargins(20,10,20,10);
        QStringList boxlist;
        boxlist<<"Blooming_en"<<"AdjustLFSWeight";
        for(int i=0;i<boxlist.size();i++){
            QPointer<QLabelCheckBox> box=new QLabelCheckBox;
            box->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
            box->setLabel(boxlist.at(i));
            box->setMouseTracking(true);
            LFS_box.push_back(box);
        }
        QStringList editlist;
        editlist<<"Thr1"<<"Thr2"<<"Thr3"<<"Thr4"<<"Thr7"
                 <<"ThrH_HCG[0]"<<"ThrH_HCG[1]"<<"ThrH_HCG[2]"<<"ThrL_HCG[0]"<<"ThrL_HCG[1]"<<"ThrL_HCG[2]"
                 <<"ThrH_LCG[0]"<<"ThrH_LCG[1]"<<"ThrH_LCG[2]"<<"ThrL_LCG[0]"<<"ThrL_LCG[1]"<<"ThrL_LCG[2]"
                 <<"GainPoint0"<<"GainPoint1"<<"GainPoint2"<<"Weight Thr"<<"CountRat 0"<<"CountRat 1";
        for(int i=0;i<editlist.size();i++){
            QPointer<QLabelEditGroup> edit=new QLabelEditGroup;
            edit->setLabelValue(editlist.at(i));
            edit->setMouseTracking(true);
            edit->setEditable(false);
            LFS_edit.push_back(edit);
            edit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        }
        for(int i=0;i<4;i++){
            for(int j=0;j<6;j++){
                int row;
                if(j>2){
                    row=j+1;
                }else{
                    row=j;
                }
                if(i==0){
                    if(j==0)
                        glayout->addWidget(LFS_box.at(0),row,i);
                    if(j>0)
                        glayout->addWidget(LFS_edit.at(j-1),row,i);
                }else{
                    glayout->addWidget(LFS_edit.at(j+6*i-1),row,i);
                }
            }
        }
        glayout->addWidget(LFS_box.at(1),3,3);
        iter.value().second=true;
        for(QPointer<QLabelEditGroup> item:LFS_edit)
            connect(item,&QLabelEditGroup::releaseLabel,this,[=](){recvEdit(item->Label());});
        for(QPointer<QLabelCheckBox> item:LFS_box){
            connect(item,&QLabelCheckBox::release,this,[=](){recvEdit(item->Label());});
            connect(item,&QLabelCheckBox::checkSignal,this,[=](){recvCheckBox(item->Label(),item->checkState());});
        }
    }
    updateValue(LFS_edit,LFS_box);
}

void QCombineTab::changeTab(int index)
{
    if(tabMap.contains(tabComm->tabText(index))){
        switch (index) {
        case 0:
            if(m_nLevel!=LEVEL_ADMINISTRATOR)
            {
                initHC();
                break;
            }
            initCombine();
            break;
        case 1:
            initHC();
            break;
        case 2:
            initDarkColor();
            break;
        case 3:
            initDoubleEdge();
            break;
        case 4:
            initLFSLED();
            break;
        case 5:
            //initGhost();
            break;
        default:
            break;
        }
    }
}

void QCombineTab::loadHCTab()
{
    hlist.clear();
    hlist<<"Hist10[0]"<<"Hist20[0]"<<"Hist30[0]"<<"Bright[0]"
          <<"Hist10[1]"<<"Hist20[1]"<<"Hist30[1]"<<"Bright[1]"
          <<"Hist10[2]"<<"Hist20[2]"<<"Hist30[2]"<<"Bright[2]"
          <<"Hist11[0]"<<"Hist21[0]"<<"Hist31[0]"<<"Dark[0]"
          <<"Hist11[1]"<<"Hist21[1]"<<"Hist31[1]"<<"Dark[1]"
          <<"Hist11[2]"<<"Hist21[2]"<<"Hist31[2]"<<"Dark[2]";
    for(int i=0;i<6;i++){
        for(int j=0;j<4;j++){
            QPointer<QLabelEditGroup> hist=new QLabelEditGroup();
            hist->setLabelValue(hlist.at(i*4+j));
            hist->setEditable(false);
            hist->setEditWidth(50);
            hist->setMouseTracking(true);
            if(i>2){
                histLayout->addWidget(hist,i+1,j);
            }else{
                histLayout->addWidget(hist,i,j);
            }
            histMap.insert(std::make_pair(i,j),hist);
            HC_edit.push_back(hist);
        }
    }
    QSpacerItem*verticallSpacer = new QSpacerItem(20, 50, QSizePolicy::Preferred, QSizePolicy::Preferred);
    histLayout->addItem(verticallSpacer,3,0);

    complist.clear();
    complist<<"CompS0[0]"<<"CompS1[0]"<<"CompS0[1]"<<"CompS1[1]"
             <<"CompS0[2]"<<"CompS1[2]"<<"CompS0[3]"<<"CompS1[3]"
             <<"CompS0[4]"<<"CompS1[4]"<<"CompS0[5]"<<"CompS1[5]"
             <<"CompS0[6]"<<"CompS1[6]"<<"CompS0[7]"<<"CompS1[7]";
    QPointer<QLabelCheckBox> box=new QLabelCheckBox();
    box->setLabel("CompS_en");
    box->setMouseTracking(true);
    connect(box,&QLabelCheckBox::release,this,[=](){recvEdit(box->Label());});
    connect(box,&QLabelCheckBox::checkSignal,this,[=](){recvCheckBox(box->Label(),box->checkState());});
    compLayout->addWidget(box,0,0);
    compLayout->setVerticalSpacing(0);
    compLayout->setContentsMargins(1,2,2,0);
    HC_box.push_back(box);

    for(int i=0;i<8;i++){
        for(int j=0;j<2;j++){
            QPointer<QLabelEditGroup> hist=new QLabelEditGroup();
            hist->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
            hist->setLabelValue(complist.at(i*2+j));
            hist->setEditable(false);
            hist->setMouseTracking(true);
            compLayout->addWidget(hist,i+1,j);
            histMap.insert(std::make_pair(i,j),hist);
            HC_edit.push_back(hist);
        }
    }
    auto hist =
        QCommonUiInterfaceFactory::instance().createUiInterface("QLabelCheckBox", "Combine", "ManualCompS", this);
    hist->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    hist->setMouseTracking(true);
    compLayout->addWidget(hist,0,1);
    connect(hist, &QCommonUiInterface::releaseUiDataMiddleware, this,
        &QTabWndComm::recvUiDataMiddlewareUpdateRegisterTable, Qt::QueuedConnection);
    connect(this, &QTabWndComm::readSignal, hist, &QCommonUiInterface::read, Qt::QueuedConnection);

    for(QPointer<QLabelEditGroup> item:HC_edit)
        connect(item,&QLabelEditGroup::releaseLabel,this,[=](){recvEdit(item->Label());});
}

void QCombineTab::writeCombine(int val)
{
    uint32_t addr=0;
    std::vector<CombinedRegister> Regvector=combinedRegisterMap.value("Data SelCombine");
    for(auto item:Regvector){
        std::vector<RegisterBitInfo> regBitInfo = DatabaseOrm::getRegisterBitInfoByCombinedRegisterId(item.combinedId);
        for(auto reg:regBitInfo){
            uint8_t value=0;
            value=val;
            int ret=QUsbCommunication::getInstance().writeBurst(reg.registerAddr,&value,ADDRESS_LENGTH,VALUE_LENGTH);
            addr=reg.registerAddr;
            break;
        }
        break;
    }
    appendCombineRow(addr);
}

void QCombineTab::appendCombineRow(uint32_t addr)
{
    Register reg = DatabaseOrm::getRegisterByAddress(addr);
    uint8_t  regvalue = 0;
    int row=labelSpinSliderCheckBoxGroup->currentTableRow();
    QUsbCommunication::getInstance().readBurst(addr, &regvalue, ADDRESS_LENGTH, VALUE_LENGTH);
    labelSpinSliderCheckBoxGroup->appendTableRow(
        row, 0,
        QString("0x%1").arg(QString::number(addr, 16)
                                .toUpper()
                                .rightJustified(VALUE_HEX_INFO, '0')));
    labelSpinSliderCheckBoxGroup->appendTableRow(
        row, 1,
        QString("0x%1").arg(QString::number(255, 16)
                                .toUpper()
                                .rightJustified(VALUE_HEX_INFO, '0')));
    labelSpinSliderCheckBoxGroup->appendTableRow(row, 2,
        QString::fromStdString(reg.displayName));

    labelSpinSliderCheckBoxGroup->appendTableRow(
        row, 3,
        QString("0x%1").arg(QString::number(regvalue, 16).toUpper().rightJustified(VALUE_HEX_INFO, '0')));
    labelSpinSliderCheckBoxGroup->appendTableRow(row, 4, QString::number(regvalue, 10));
    labelSpinSliderCheckBoxGroup->appendTableRow(row, 5, QString::number(0, 'f', 2));
    labelSpinSliderCheckBoxGroup->appendTableRow(row, 6, QString::fromStdString(reg.registerDescription));
}

int QCombineTab::readCombine(int flag)
{
    uint8_t value=0;
    std::vector<CombinedRegister> Regvector=combinedRegisterMap.value("Data SelCombine");
    for(auto item:Regvector){
        std::vector<RegisterBitInfo> regBitInfo = DatabaseOrm::getRegisterBitInfoByCombinedRegisterId(item.combinedId);
        for(auto reg:regBitInfo){
            int ret =
                QUsbCommunication::getInstance().readBurst(reg.registerAddr, &value, ADDRESS_LENGTH, VALUE_LENGTH);
            if(flag==1)
                appendCombineRow(reg.registerAddr);
            return value;
        }
    }
    return value;
}

void QCombineTab::updateValue(std::vector<QPointer<QLabelEditGroup> > edit, std::vector<QPointer<QLabelCheckBox> > box)
{
    updateUIValue<std::vector<QPointer<QLabelEditGroup>>>(edit);
    updateUIValue<std::vector<QPointer<QLabelCheckBox>>>(box);
}

std::vector<CombinedRegister> QCombineTab::combinedRegisterSubPageRecording(QString strModuleName, QString strUiName, std::shared_ptr<int> ptrCamera, QString strSubPage) 
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

std::vector<CombinedRegister> QCombineTab::getCombinedRegister(QString strModuleName, QString strUiName, std::shared_ptr<int> ptrCamera)
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

void QCombineTab::recordUIRegSubPageInfo()
{
    // Combine
    combinedRegisterSubPageRecording("CComb_HW_V60", "Data Sel", nullptr, "Combine");
    combinedRegisterSubPageRecording("CComb_HW_V60", "High Margin", nullptr, "Combine");
    combinedRegisterSubPageRecording("CComb_HW_V60", "Blooming_mode_en", nullptr, "Combine");
    combinedRegisterSubPageRecording("CComb_HW_V60", "Blooming_shift1", nullptr, "Combine");
    combinedRegisterSubPageRecording("CComb_HW_V60", "Blooming_shift2", nullptr, "Combine");
    combinedRegisterSubPageRecording("CComb_HW_V60", "Blooming_shift3", nullptr, "Combine");
    
    combinedRegisterSubPageRecording("ISP", "Data SelCombine", nullptr, "Combine");
    
    // H&C
    combinedRegisterSubPageRecording("COMB", "ISOList0", std::make_shared<int>(getCurrentCamera()), "H&C");
    combinedRegisterSubPageRecording("COMB", "ISOList1", std::make_shared<int>(getCurrentCamera()), "H&C");

    for(int i=0;i<hlist.size();i++){
        if(i==3||i==7||i==11||i==15||i==19||i==23){
            combinedRegisterSubPageRecording("CComb_HW_V60", hlist.at(i), nullptr, "H&C");
        }else{
            combinedRegisterSubPageRecording("COMB", hlist.at(i), std::make_shared<int>(getCurrentCamera()), "H&C");
        }
    }
    combinedRegisterSubPageRecording("CComb_HW_V60", "CompS_en", nullptr, "H&C");
    for(int i=0;i<complist.size();i++)
        combinedRegisterSubPageRecording("COMB", complist.at(i), std::make_shared<int>(getCurrentCamera()), "H&C");
    
    // DarkColor
    for (auto item : dar_color_box)
        combinedRegisterSubPageRecording("CComb_HW_V60", item->Label(), nullptr, "DarkColor");
    for (auto item : dark_color_edit){
        if(item->Label()=="Dark_color_maxw"||item->Label()=="Dark_color_thrw"){
            combinedRegisterSubPageRecording("CComb_HW_V60", item->Label(), nullptr, "DarkColor");
        }else if(item->Label()=="CurGain"){
            combinedRegisterSubPageRecording("PipeCtrl", item->Label(), std::make_shared<int>(getCurrentCamera()), "DarkColor");
        }else{
            combinedRegisterSubPageRecording("COMB", item->Label(), std::make_shared<int>(getCurrentCamera()), "DarkColor");
        }
    }
    
    // DoubleEdge
    for (auto item : doubleEdge_box)
        combinedRegisterSubPageRecording("CComb_HW_V60", item->Label(), nullptr, "DoubleEdge");
    for (auto item : doubleEdge_edit){
        if(item->Label()=="DoubleEdgeGainStart"||item->Label()=="DoubleEdgeGainEnd"||item->Label()=="DoubleEdgeHFMaxThr"){
            combinedRegisterSubPageRecording("COMB", item->Label(), std::make_shared<int>(getCurrentCamera()), "DoubleEdge");
        }else if(item->Label()=="CurGain"){
            combinedRegisterSubPageRecording("PipeCtrl", item->Label(), std::make_shared<int>(getCurrentCamera()), "DoubleEdge");
        }else{
            combinedRegisterSubPageRecording("CComb_HW_V60", item->Label(), nullptr, "DoubleEdge");
        }
    }
    
    // Ghost
    for (auto item : chost_box)
        combinedRegisterSubPageRecording("CComb_HW_V60", item->Label(), nullptr, "Ghost");
    for (auto item : chost_edit)
        combinedRegisterSubPageRecording("CComb_HW_V60", item->Label(), nullptr, "Ghost");
    
    // LFS LED
    combinedRegisterSubPageRecording("CComb_HW_V60", "Blooming_en", nullptr, "LFSLED");
    combinedRegisterSubPageRecording("COMB", "AdjustLFSWeight", std::make_shared<int>(getCurrentCamera()), "LFSLED");
    for (auto item : LFS_edit){
        if(item->Label()=="Weight Thr"||item->Label()=="Thr1"||item->Label()=="Thr2"||item->Label()=="Thr3"||item->Label()=="Thr4"||item->Label()=="Thr7"){
            combinedRegisterSubPageRecording("CComb_HW_V60", item->Label(), nullptr, "LFSLED");
        }else{
            combinedRegisterSubPageRecording("COMB", item->Label(), std::make_shared<int>(getCurrentCamera()), "LFSLED");
        }
    }
}
