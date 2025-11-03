#ifndef QTABWNDCOMM_H
#define QTABWNDCOMM_H

#include "qcommondef.h"
#include "qlabelspinslidercheckboxgroup.h"
#include "qpushbuttongroup.h"
#include <QCheckBox>
#include <QDir>
#include <QHeaderView>
#include <QLayout>
#include <QPainter>
#include <QPointer>
#include <QSpacerItem>
#include <QTableView>
#include <QWidget>
#include <QtConcurrent/QtConcurrent>
#include <qfiledialog.h>
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>

namespace Ui {
class QTabWndComm;
}
enum REGISTER_TYPE{
    REGISTER_TYPE_DELETE = 0,
    REGISTER_TYPE_NORMAL,
    REGISTER_TYPE_RGB,
    REGISTER_TYPE_IR
};
#define IR_RGB_MODE_REG_ADDR 0x30082fc1
class QTabWndComm : public QWidget
{
    Q_OBJECT
public:
    explicit QTabWndComm(QWidget *parent = nullptr);
    ~QTabWndComm();
    /**
     * @brief getCurrentCamera 获取当前相机
     * @return int(0:camera0||1:camera1)
     */
    int getCurrentCamera();
    /**
     * @brief setCurrentCamera 切换当前相机
     */
    void setCurrentCamera(int cameraType);
    void updateTabCamera();
    int  getTabCamera();
    void reConnectCamera();
    /**
     * @brief addRegisterTable 在当前页面下方布局添加通用register set模块
     */
    void addRegisterTable();
    /**
     * @brief addTabWidget 在当前页面添加默认样式tab页
     */
    void addTabWidget(QPointer<QTabWidget> tabWidget);
    void updatePainter();
    /**
     * @brief setRegisterSetVisible 设置register set模块显隐
     */
    void setRegisterSetVisible(int isVisible);
    /**
     * @brief clearCurrentRigester 重置register set滑块
     */
    void clearCurrentRigester();
    /**
     * @brief getValuebyCombineBitAddr 根据高低位寄存器获取最终值
     * @return int
     */
    int getValuebyCombineBitAddr(int lowAddr, int highAddr);
    /**
     * @brief getValuebyCombineBit 根据高低位值获取最终值
     * @return int
     */
    int getValuebyCombineBit(int low, int high);
    /**
     * @brief 根据当前值自动拆分高低位并设置寄存器
     */
    void setValuebyCombinedBit(int value, int lowAddr, int highAddr);
    void setValuebyCombinedBit(int value, QString lowAddr, QString highAddr);
    /**
     * @brief getIntbyFloat CCM模块，RGB通用
     * @return int
     */
    int getIntbyFloat(float currentValue);
    /**
     * @brief getFloatbyInt CCM模块，RGB通用
     * @return float
     */
    float getFloatbyInt(int currentValue);
    /**
     * @brief updateRegisterTable 更新register set模块和列表
     */
    void updateRegisterTable(std::vector<CombinedRegister> regVector);
    /**
     * @brief updateTable 更新table内容
     */
    void updateTable(std::vector<CombinedRegister> regVector);

    void updateTableList(QList<CombinedRegister> regList);

    void writeSensorRegisterValue(std::vector<std::pair<uint32_t, uint32_t>> &regValue);
    /**
     * @brief recordCombineRegbyUIifo 绑定Ui-寄存器
     */
    void recordCombineRegbyUIifo(QString moudle_name, QString uiname, std::shared_ptr<int> cameraId);

    void setRegisterTypeMap(std::vector<CombinedRegister> regVector, std::vector<CombinedRegister> regVectorRgb, std::vector<CombinedRegister> regVectorIr);

    void setRegisterSubPage(CombinedRegister reg, QString strSubPage);

    void saveCombinedReg();
    void recvEdit(QString str);
    void recvCheckBox(QString str, bool isChecked);
    void loadDeletedRegistersFromFile(QString strDelRegListTxt);
    void saveSpecial(QString folderPath);
    template <typename T>
    void updateUIValue(T& vector,const QString &extraname="")
    {
        for(auto item:vector){
            uint32_t value=0;
            auto reg=combinedRegisterMap.value(item->Label()+extraname);
            for(int i=0;i<reg.size();i++){
                readCombinedRegisterValue(reg.at(i), value);
                // if (item->Label)
                item->setValue(QString("0x%1").arg(QString::number(value,16).toUpper().rightJustified(VALUE_HEX_INFO,'0')));
            }
        }
    }
    static void setLevel(uint Level){
        m_nLevel=Level;
    }

    static uint getLevel()
    {
        return m_nLevel;
    }

    static uint m_saveType;

protected:
    static uint m_nLevel;
private:
    void init();
    void initConnect();
signals:
    void readSignal();
    void updateValue();
    void reConnectRead();//更新camera id信号
    void registerMoudleUpdateDone();
public slots:
    void read();
    void load();
    void save();
    /**
     * @brief recValueByHighLowBits 调整set模块，用以实时接受寄存器值的变动。
     */
    void recValueByHighLowBits(uint32_t value);
    void recvUiDataMiddlewareUpdateRegisterTable(QPointer<QUiDataMiddleware>);

protected:
    void                                    paintEvent(QPaintEvent *event) override;
    QPointer<QGridLayout>                   contentLayout                = nullptr;
    QPointer<QWidget>                       contentWidget                = nullptr;
    QPointer<QWidget>                       registerSetWidget            = nullptr;
    QPointer<QGridLayout>                   registerSetLayout            = nullptr;
    QPointer<QHBoxLayout>                   titleLayout                  = nullptr;
    QPointer<QLabelSpinSliderCheckBoxGroup> labelSpinSliderCheckBoxGroup = nullptr;
    int                                     currentCamera                = 0;
    std::vector<int>                        currentAddressVector;
    std::string                             moduleName;
    std::vector<uint32_t>                   registerMoudle;
    QMap<uint32_t, std::pair<uint32_t, uint32_t>>                registerTypeMap;
    QMap<uint32_t, std::vector<QString>>                 registerSubPageMap;
    QMap<uint32_t, QString>                 subPageMap;
    // std::mutex                              mtx;
    QPointer<QPushButtonGroup>              buttonGroup = nullptr;
    QFuture<void>                           futureSaveInfoInit;
    std::vector<CombinedRegister>           regComVector;
    QMap<QString,std::vector<CombinedRegister>> combinedRegisterMap;
    int paintertype=0;
    bool                                    registerMoudleReadable = false;
    QString                                 strDelRegListTxt;
private:
    Ui::QTabWndComm                        *ui;
};

/**
 * @brief readCombinedRegisterValue 根据CombinedRegister的信息，读取当前组合寄存器的值
 * @return
 */
void readCombinedRegisterValue(CombinedRegister &combineReg, uint32_t &value);
/**
 * @brief writeCombinedRegisterValue 根据CombinedRegister的信息，将当前组合寄存器的值写入
 * @return
 */
void writeCombinedRegisterValue(CombinedRegister &combineReg, uint32_t value);

#endif // QTABWNDCOMM_H
