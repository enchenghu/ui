#ifndef QTABLELISTGROUP_H
#define QTABLELISTGROUP_H
#include "DatabaseOrm/databaseorm.h"
#include <QAbstractItemModel>
#include <QDebug>
#include <QHeaderView>
#include <QPointer>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QStringList>
#include <QTableView>

using namespace DatabaseOrm;
class QTableListGroup : public QTableView
{
    Q_OBJECT
public:
    explicit QTableListGroup(QTableView *parent = nullptr);
    ~QTableListGroup();
    /**
     * @brief setHeader 设置水平表头属性
     * @param  QStringList 表头属性名称（从左到右）
     * @return
     */
    void setHeader(QStringList strlist);
    /**
     * @brief setVerticalHeader 设置垂直表头属性
     * @param  QStringList 表头属性名称（从上到下）
     * @return
     */
    void setVerticalHeader(QStringList strlist);
    /**
     * @brief setFont 设置表头属性
     * @param  QFont
     * @return
     */
    void setFont(QFont font);
    /**
     * @brief appendRow 添加表格单元文字类
     * @param  (int,int, QString)
     * @return
     */
    void appendRow(int row,int column,QString str,Qt::Alignment alignment=Qt::AlignCenter);
    /**
     * @brief appendRowWidget 添加表格单元控件类
     * @param  (int,int, QWidget*)
     * @return
     */
    void appendRowWidget(int row,int column,QWidget*widget);
    /**
     * @brief recordValue 记录寄存器当前值
     * @param  (当前寄存器Register,当前列表的DATA列,进制位(2,8,10,16))
     * @return
     */
    void recordValue(Register reg, int valCol, int bit);
    /**
     * @brief setValue 更新寄存器当前值
     * @param  (寄存器地址, 寄存器值)
     * @return
     */
    void setValue(int addr,int val);
    /**
     * @brief setValueByBitMask 更新寄存器当前值
     * @param  (寄存器地址, 寄存器值)
     * @return
     */
    void setValueByBitMask(int addr,int val);
    /**
     * @brief getValue 更新寄存器当前值
     * @param  (寄存器地址, 寄存器值)
     * @return
     */
    int getValue(int addr);
    /**
     * @brief getItem 获取当前表格单元
     * @param  (行,列)
     * @return
     */
    QStandardItem* getItem(int row,int col);
    /**
     * @brief setTableMenuRequested 设置右击菜单
     * @return
     */
    void setTableMenuRequested(bool flag);
    /**
     * @brief setStyle 添加表格样式
     * @return
     */
    void setStyle(QString qss);
    int rowCount();
    int columnCount();
    void clearAll();
    void clearRows();
    void setVerticalHeaderVisible(bool isVisible);
    void setHorizontalHeaderVisible(bool isVisible);


signals:
    void itemClicked(QString addr);//表格之间通讯，可传递模块、寄存器地址
    void sendCurrentItem(QStandardItem* citem,int row,int col);//表格之间通讯，可传递模块、寄存器地址

public slots:
    void recvValueChanged(int addr,int val);
    void recvValueChangedbyBitMask(int addr,int val);
    void receiveitemClicked(QString address);//表格之间通讯，可处理模块、寄存器地址
private slots:
    void on_table_doubleClicked(const QModelIndex &index);
    void on_customContextMenuRequested(const QPoint &pos);
    void deleteItem(QModelIndex index);

private:
    /**
     * @brief initTable 初始化图表
     * @return
     */
    void initTable();

private:
    QPointer<QStandardItemModel>                                    tableModel = nullptr; // 控制主界面唯一tree
    QMap<uint32_t,QMap<std::pair<uint32_t,int>,std::pair<int,int>>> addrMap;
    QString currentAddress;
    QMap<QString,std::vector<QStandardItem*>> propertyItemMap;


};

#endif // QTABLELISTGROUP_H
