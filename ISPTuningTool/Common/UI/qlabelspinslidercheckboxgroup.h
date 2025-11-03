#ifndef QLABELSPINSLIDERCHECKBOXGROUP_H
#define QLABELSPINSLIDERCHECKBOXGROUP_H

#include "qbitcheckboxgroup.h"
#include "qlabeleditgroup.h"
#include "qspinslidergroup.h"
#include "qtablelistgroup.h"
#include <QPushButton>
#include <QWidget>
#include <QDateTime>
#include "qcommondef.h"
namespace Ui {
class QLabelSpinSliderCheckBoxGroup;
}

class QLabelSpinSliderCheckBoxGroup : public QWidget
{
    Q_OBJECT

public:
    explicit QLabelSpinSliderCheckBoxGroup(QWidget *parent = nullptr);
    ~QLabelSpinSliderCheckBoxGroup();
    void init();
    void initConnect();
    void setValue(uint32_t value);
    int getValue();
    void updateValue();
    /**
     * @brief setReadBtnVisible 设置read按钮显隐
     */
    void setReadBtnVisible(bool isVisible);
    /**
     * @brief setTableListVisible 设置table表格显隐
     */
    void setTableListVisible(bool isVisible);
    /**
     * @brief setTableHeader 设置table表头Header内容
     */
    void setTableHeader(QStringList strList);
    /**
     * @brief appendTableRow 添加table内容
     */
    void appendTableRow(int row,int column,QString str,Qt::Alignment alignment=Qt::AlignCenter);
    int currentTableRow();
    /**
     * @brief changeGroupLayout重构布局
     * @param 网格布局中(table行、列;spinSlider行、列;bitBox行、列;labelEdit行、列)
     * @param 不传入参数位默认布局
     */
    void changeGroupLayout(int tableRow=0,int tableCol=0,int spinSliderRow=1,int spinSliderCol=0,int bitBoxRow=1,int bitBoxCol=2,int labelEditRow=1,int labelEditCol=1);
    void clearTableRows();
    /**
     * @brief setCurrentRegisterPair 设置当前绑定的高低位一组寄存器，记录当前值
     */
    void setCurrentRegisterPair(std::pair<Register,Register> registerPair,int cValue);
    /**
     * @brief setCurrentBitRange 设置显示范围
     */
    void setCurrentRange(uint32_t minVal=0, uint32_t maxVal=255);
    void setcurrentValue(uint32_t value);
    std::vector<CombinedRegister> getCombinedRegister();
    void setCombinedRegister(std::vector<CombinedRegister> combineVector);
    void setCombinedRegister2(std::vector<CombinedRegister> combineVector);
    void clearRegister();
    void setRealEdit(bool flag);
    float getCurrentRealVal();
    void  setSpinSliderEnable(bool flag);
    void                          setBitCheckBoxEnable(bool flag);
    void                          setFloatIntType(FloatIntType type);

signals:
    void signalValueChange(int,int);//register相关
    void signalValueChange(QString);//register相关
    void updateRegisterModule(int);//register相关
    void changeValueByHLBits(uint32_t);
    void ctrlTriggered(uint32_t);
public slots:
    void recvValueChanged(QString addr);//register相关
    void on_readButton_clicked();//register相关
protected:
    void paintEvent(QPaintEvent*event) override;

private slots:
    void recvValueChanged(uint32_t value);
    void recvValueChanged(uint32_t value,bool flag);
    void recFloatValue(float val);

public:
    std::pair<Register, Register> currentRegisterPair;

private:
    uint32_t                           currentValue = 0;
    Ui::QLabelSpinSliderCheckBoxGroup *ui;
    std::vector<CombinedRegister>      combinedRegister;
    QPointer<QSpinSliderGroup>         spinSliderGroup  = nullptr;
    QPointer<QBitCheckBoxGroup>        bitCheckBoxGroup = nullptr;
    QPointer<QLabelEditGroup>          labelEditGroup   = nullptr;
    QPointer<QPushButton>              readButton       = nullptr;
    QPointer<QLabelEditGroup>          labelEditGroup2  = nullptr;
    QPointer<QTableListGroup>          tableListGroup   = nullptr;
    Register                           currentRegister;
    Module                             currentModule;
    FloatIntType                       m_type=FloatIntType::Type_1_0x100;

    std::function<float(uint32_t)>     valueToReal;
};
#endif // QLABELSPINSLIDERCHECKBOXGROUP_H
