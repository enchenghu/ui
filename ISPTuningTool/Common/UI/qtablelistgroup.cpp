#include "qtablelistgroup.h"
#include "Common/Others/qconfigfiletrans.h"
#include "Common/Others/qusbcommunication.h"
#include "qcommondef.h"
#include <QGridLayout>
#include <QMenu>
#include <Qlabel>

QTableListGroup::QTableListGroup(QTableView *parent)
{
    tableModel = new QStandardItemModel();
    initTable();
}

void QTableListGroup::initTable()
{
    //设置不可编辑
    this->setEditTriggers(QAbstractItemView::EditKeyPressed);
    //设置每次选择为选择一行
    this->setSelectionBehavior(QAbstractItemView::SelectItems);
    //设置交替颜色
    this->setAlternatingRowColors(true);
    //隐藏行号
    this->setVerticalHeaderVisible(false);
    this->setMouseTracking(true);
    this->setContextMenuPolicy(Qt::CustomContextMenu);
    this->setModel(tableModel.get());
    this->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    this->setMinimumHeight(500);
    connect(this,&QTableView::doubleClicked,this,&QTableListGroup::on_table_doubleClicked);
    connect(this,&QTableView::customContextMenuRequested,this,&QTableListGroup::on_customContextMenuRequested);

    setStyle("");
    this->setSortingEnabled(true);
}

void QTableListGroup::setHeader(QStringList strlist)
{
    tableModel->setHorizontalHeaderLabels(strlist);
}

void QTableListGroup::setVerticalHeader(QStringList strlist)
{
    tableModel->setVerticalHeaderLabels(strlist);
}

void QTableListGroup::setFont(QFont font)
{

}

void QTableListGroup::appendRow(int row, int column, QString str, Qt::Alignment alignment)
{
    QStandardItem*_item=new QStandardItem(str);
    tableModel->setItem(row, column, _item);
    _item->setData(str,Qt::UserRole);
    tableModel->item(row, column)->setTextAlignment(alignment);
}



void QTableListGroup::appendRowWidget(int row, int column, QWidget *widget)
{
    this->setIndexWidget(tableModel->index(row, column),widget);
}

void QTableListGroup::recordValue(Register reg,int valCol,int bit)
{
    int currentRow=tableModel->rowCount();
    if(!addrMap.contains(reg.address)){
        QMap<std::pair<uint32_t,int>,std::pair<int,int>> registerItemValMap;
        addrMap.insert(reg.address,registerItemValMap);
    }
    auto iter=addrMap.find(reg.address);
    iter.value().insert(std::make_pair(reg.address,bit),std::make_pair(currentRow-1, valCol));
}

void QTableListGroup::setValue(int addr, int val)
{
    if(!addrMap.contains(addr))
        return;
    auto registerItemValMap=addrMap.value(addr);
    for(auto iter=registerItemValMap.begin();iter!=registerItemValMap.end();iter++){
        int currentBit=iter.key().second;
        auto item=tableModel->item(registerItemValMap.value(std::make_pair(addr,currentBit)).first,registerItemValMap.value(std::make_pair(addr,currentBit)).second);
        if(item){
            if(currentBit==16){
                item->setText(QString("0x%1").arg(QString::number(val,currentBit).toUpper().rightJustified(VALUE_HEX_INFO,'0')));
            }else{
                item->setText(QString::number(val,currentBit).toUpper());
            }
        }
    }
}

void QTableListGroup::setValueByBitMask(int addr, int val)
{
    if(!addrMap.contains(addr))
        return;
    std::vector<RegisterBitInfo> bitVector=DatabaseOrm::getRegisterBitInfoByRegisterAddress(addr);
    for(int i=0;i<bitVector.size();i++){
        auto registerItemValMap=addrMap.value(addr);
        for(auto iter=registerItemValMap.begin();iter!=registerItemValMap.end();iter++){
            int currentBit=iter.key().second;
            uint32_t  bitValue=getBitValue(bitVector.at(i).bitMask,val);
            auto item=tableModel->item(i,registerItemValMap.value(std::make_pair(addr,currentBit)).second);
            if(item){
                if(currentBit==16){
                    item->setText(QString("0x%1").arg(QString::number(bitValue,currentBit).toUpper().rightJustified(VALUE_HEX_INFO,'0')));
                }else{
                    item->setText(QString::number(bitValue,currentBit).toUpper());
                }
            }
        }
    }

}

int QTableListGroup::getValue(int addr)
{
    int value=0;
    if(addrMap.contains(addr)){
        auto registerItemValMap=addrMap.value(addr);
        auto item=tableModel->item(registerItemValMap.value(std::make_pair(addr,16)).first,registerItemValMap.value(std::make_pair(addr,16)).second);
        value= item->text().toInt(nullptr,16);
    }
    return value;
}

QStandardItem* QTableListGroup::getItem(int row, int col)
{
    if(tableModel->item(row,col)){
        QStandardItem*_item=tableModel->item(row,col);
        return _item;
     }
    return new QStandardItem();
}

void QTableListGroup::setTableMenuRequested(bool flag)
{
    if(flag){
        connect(this,&QTableView::customContextMenuRequested,this,&QTableListGroup::on_customContextMenuRequested);
    }else{
        disconnect(this,&QTableView::customContextMenuRequested,this,&QTableListGroup::on_customContextMenuRequested);
    }
}

void QTableListGroup::setStyle(QString qss)
{
    /*if(qss==""){
        this->setStyleSheet("QTableView{border:0px solid rgb(255,255,255);gridline-color:white;outline:0px;background-color:rgb(225,225,225);alternate-background-color:rgba(243,243,243);selection-background-color: rgba(0, 0, 255,50);}"
                            "QHeaderView::section{border:2px solid rgb(255,255,255);background-color:rgb(165,165,165);color:rgb(255,255,255);font:bold};"
                            );
    }else{
        this->setStyleSheet(qss);
    }*/
    this->setStyleSheet("QTableView{border:0px solid rgb(255,255,255);gridline-color:white;outline:0px;selection-background-color: rgba(0, 0, 255,50);}"
                        "QHeaderView::section{border:2px solid rgb(255,255,255);font:bold};"
        );
}

int QTableListGroup::rowCount()
{
    return tableModel->rowCount();
}

int QTableListGroup::columnCount()
{
    return tableModel->columnCount();
}

void QTableListGroup::clearAll()
{
    tableModel->clear();
    addrMap.clear();
    propertyItemMap.clear();
}

void QTableListGroup::clearRows()
{
    tableModel->removeRows(0, tableModel->rowCount());
    addrMap.clear();
    propertyItemMap.clear();
}

void QTableListGroup::setHorizontalHeaderVisible(bool isVisible)
{
    this->horizontalHeader()->setVisible(isVisible);
}

void QTableListGroup::setVerticalHeaderVisible(bool isVisible)
{
    this->verticalHeader()->setVisible(isVisible);
}

void QTableListGroup::recvValueChanged(int addr, int val)
{
    setValue(addr,val);
}

void QTableListGroup::recvValueChangedbyBitMask(int addr, int val)
{
    std::vector<RegisterBitInfo> bitVector=DatabaseOrm::getRegisterBitInfoByRegisterAddress(addr);
    if(bitVector.empty())
        return;
    if(!propertyItemMap.contains(QString::fromStdString(bitVector.at(0).registerBitName)))
        return;
    for(int i=0;i<bitVector.size();i++){
        uint32_t  bitValue=getBitValue(bitVector.at(i).bitMask,val);
        auto iter=propertyItemMap.find(QString::fromStdString(bitVector.at(i).registerBitName));
        if(iter.value().size()>0){
            iter.value().at(2)->setText(QString("0x%1").arg(QString::number(bitValue,16).toUpper().rightJustified(VALUE_HEX_INFO,'0')));
            iter.value().at(3)->setText(QString::number(bitValue,10).toUpper());
        }
    }
}

void QTableListGroup::receiveitemClicked(QString address)
{
    //此处应请求数据库接口，展示模块相关的寄存器bits
    std::vector<RegisterBitInfo> bitVector=DatabaseOrm::getRegisterBitInfoByRegisterAddress(address.toInt(nullptr,16));
    Register _register=DatabaseOrm::getRegisterByAddress(address.toInt(nullptr,16));
    int currentRow=tableModel->rowCount();
    for(int i=0;i<bitVector.size();i++)
    {
        if(propertyItemMap.contains(QString::fromStdString(bitVector.at(i).registerBitName)))
            return;
        QStandardItem*_item0=new QStandardItem(QString("0x%1").arg(QString::number(bitVector.at(i).bitMask,16).toUpper()));
        tableModel->setItem(i+currentRow, 0, _item0);
        _item0->setTextAlignment(Qt::AlignCenter);
        QStandardItem*_item1=new QStandardItem(QString::fromStdString(bitVector.at(i).registerBitName));
        tableModel->setItem(i+currentRow, 1, _item1);
        _item1->setData(address,Qt::UserRole);
        _item1->setTextAlignment(Qt::AlignLeft|Qt::AlignVCenter);
        uint8_t value = 0;
        QUsbCommunication::getInstance().readBurst(_register.address, &value, ADDRESS_LENGTH, VALUE_LENGTH);
        uint32_t  bitValue=getBitValue(bitVector.at(i).bitMask,(uint32_t)value);
        QStandardItem*_item2=new QStandardItem(QString("0x%1").arg(QString::number(bitValue,16).toUpper().rightJustified(VALUE_HEX_INFO,'0')));
        tableModel->setItem(i+currentRow, 2, _item2);
        recordValue(_register,2,16);
        _item2->setTextAlignment(Qt::AlignCenter);
        QStandardItem*_item3=new QStandardItem(QString::number(bitValue));
        tableModel->setItem(i+currentRow, 3, _item3);
        recordValue(_register,3,10);
        _item3->setTextAlignment(Qt::AlignCenter);
        QStandardItem*_item4=new QStandardItem(QString::fromStdString(bitVector.at(i).registerBitDescription));
        tableModel->setItem(i+currentRow, 4, _item4);
        _item4->setTextAlignment(Qt::AlignLeft|Qt::AlignVCenter);

        std::vector<QStandardItem*> itemVector;
        itemVector.push_back(_item0);
        itemVector.push_back(_item1);
        itemVector.push_back(_item2);
        itemVector.push_back(_item3);
        itemVector.push_back(_item4);
        propertyItemMap.insert(QString::fromStdString(bitVector.at(i).registerBitName),itemVector);
    }
}

void QTableListGroup::on_table_doubleClicked(const QModelIndex &index)
{
    if(!index.isValid())
        return;
    QStandardItem *cItem=tableModel->itemFromIndex(index);
    int row=cItem->row();
    int col=cItem->column();
    QString addr =tableModel->item(row,1)->data(Qt::UserRole).toString();
    emit itemClicked(addr);
    emit sendCurrentItem(cItem,row,col);
}

void QTableListGroup::on_customContextMenuRequested(const QPoint &pos)
{
    QMenu*itemMenu=new QMenu(this);
    QAction*list0=new QAction(QString::fromUtf8("Delete"),itemMenu);
    itemMenu->addAction(list0);

    connect(list0,&QAction::triggered,this,[=](){deleteItem(this->indexAt(pos));});
    QAction*List=new QAction(QString::fromUtf8("Delete All"),itemMenu);
    itemMenu->addAction(List);
    itemMenu->adjustSize();
    itemMenu->setVisible(true);
    itemMenu->move(this->mapToGlobal(pos));
    connect(List,&QAction::triggered,this,[=](){clearRows();});
}

void QTableListGroup::deleteItem(QModelIndex index)
{
    if(!index.isValid())
        return;
    propertyItemMap.remove(tableModel->item(index.row(),1)->text());
    tableModel->removeRow(index.row());
}

QTableListGroup::~QTableListGroup()
{
}
