#ifndef QUIDATAMIDDLEWARE_H
#define QUIDATAMIDDLEWARE_H
#include "DatabaseOrm/databaseorm.h"
#include <QObject>
#include <QSharedPointer>

class QUiDataMiddleware : public QObject
{
    Q_OBJECT
public:
    Q_PROPERTY(QString uiModuleName READ getUiModuleName WRITE setUiModuleName)
    Q_PROPERTY(QString uiControlName READ getUiControlName WRITE setUiControlName)
    Q_PROPERTY(QSharedPointer<int> uiControlIndex READ getUiControlIndex WRITE setUiControlIndex)
    Q_PROPERTY(QString originalModuleName READ getOriginalModuleName WRITE setOriginalModuleName)
    Q_PROPERTY(QSharedPointer<int> cameraId READ getCameraId WRITE setCameraId)

    QUiDataMiddleware(const QUiDataMiddleware &other);
    QUiDataMiddleware(QString uiModuleName, QString uiControlName);
    QUiDataMiddleware(QString uiModuleName, QString uiControlName, QSharedPointer<int> uiControlIndex);
    QUiDataMiddleware(QString uiModuleName, QString uiControlName, QString originalModuleName,
        QSharedPointer<int> cameraId);
    ~QUiDataMiddleware();

    QList<DatabaseOrm::CombinedRegister>                  getCombinedRegisterList();
    QList<QPair<DatabaseOrm::CombinedRegister, uint32_t>> getCombinedRegisterValueList();
    QList<QPair<DatabaseOrm::CombinedRegister, uint32_t>> getCombinedRegisterValueListByCameraId(
        std::shared_ptr<int> cameraId);
    bool setCombinedRegisterValueList(QList<QPair<DatabaseOrm::CombinedRegister, uint32_t>> combinedRegisterValueList);
    QList<DatabaseOrm::RegisterBitInfo>                   getRegisterBitInfoList();
    QList<uint32_t>                                       getRegisterAddressList();
    QList<QPair<uint32_t, uint32_t>>                      getRegisterAddressValueListFromDbAndDevice();
    bool                                                  getValueFromDevice(uint32_t &registerValue);
    bool                                                  setValueToDevice(uint32_t registerValue);
    void                                                  writeValueToDevice(uint32_t registerValue);
    uint32_t                                              readValueFromDevice();
    bool writeValueToDevice(QList<QPair<uint32_t, uint32_t>> registerAddressValueList);

    QString             getUiModuleName() const;
    void                setUiModuleName(const QString &value);
    QString             getUiControlName() const;
    void                setUiControlName(const QString &value);
    QSharedPointer<int> getUiControlIndex() const;
    void                setUiControlIndex(const QSharedPointer<int> value);
    QString             getOriginalModuleName() const;
    void                setOriginalModuleName(const QString &value);
    QSharedPointer<int> getCameraId() const;
    void                setCameraId(const QSharedPointer<int> value);

    static QList<uint32_t> getRegisterAddressByUiModule(QString uiModuleName);

private:
    QString                                   uiModuleName;
    QString                                   uiControlName;
    QSharedPointer<int>                       uiControlIndex;
    QString                                   originalModuleName;
    std::vector<DatabaseOrm::UiInfoRegModule> uiInfoRegModuleList;
    QSharedPointer<int>                       cameraId;
};

/**
 * @brief readCombinedRegisterValue 根据CombinedRegister的信息，读取当前组合寄存器的值
 * @return
 */
void readCombinedRegisterValue(DatabaseOrm::CombinedRegister &combineReg, uint32_t &value);
/**
 * @brief writeCombinedRegisterValue 根据CombinedRegister的信息，将当前组合寄存器的值写入
 * @return
 */
void writeCombinedRegisterValue(DatabaseOrm::CombinedRegister &combineReg, uint32_t value);

#endif // QUIDATAMIDDLEWARE_H
