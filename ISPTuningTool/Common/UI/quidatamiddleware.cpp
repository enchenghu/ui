#include "quidatamiddleware.h"
#include "Common/Others/qcameratype.h"
#include "Common/Others/qusbcommunication.h"
#include "qcommondef.h"
#include <QSet>
#include <cstdint>
#include <memory>

QUiDataMiddleware::QUiDataMiddleware(const QUiDataMiddleware &other)
    : uiModuleName(other.uiModuleName)
    , uiControlName(other.uiControlName)
    , uiControlIndex(other.uiControlIndex == nullptr ? nullptr : QSharedPointer<int>(new int(*other.uiControlIndex)))
    , originalModuleName(other.originalModuleName)
    , cameraId(other.cameraId == nullptr ? nullptr : QSharedPointer<int>(new int(*other.cameraId)))
{
    if (originalModuleName.isEmpty()) {
        uiInfoRegModuleList = DatabaseOrm::getUiInfoRegModuleByUiModuleNameAndUiControlName(uiModuleName.toStdString(),
            uiControlName.toStdString());
        if (!uiInfoRegModuleList.empty()) {
            originalModuleName = QString::fromStdString(uiInfoRegModuleList[0].regModuleName);
            cameraId           = uiInfoRegModuleList[0].hasCameraId
                          ? QSharedPointer<int>(new int(CameraType::instance()->getCurrentCamera()))
                          : nullptr;
        }
    }
}

QUiDataMiddleware::QUiDataMiddleware(QString _uiModuleName, QString _uiControlName)
    : QUiDataMiddleware(_uiModuleName, _uiControlName, "", nullptr)
{
}

QUiDataMiddleware::QUiDataMiddleware(QString uiModuleName, QString uiControlName, QSharedPointer<int> uiControlIndex)
    : uiModuleName(uiModuleName)
    , uiControlName(uiControlName)
    , uiControlIndex(uiControlIndex == nullptr ? nullptr : QSharedPointer<int>(new int(*uiControlIndex)))
    , originalModuleName()
    , cameraId(nullptr)
{
    if (originalModuleName.isEmpty()) {
        uiInfoRegModuleList = DatabaseOrm::getUiInfoRegModuleByUiModuleNameAndUiControlName(uiModuleName.toStdString(),
            uiControlName.toStdString());
        if (!uiInfoRegModuleList.empty()) {
            originalModuleName = QString::fromStdString(uiInfoRegModuleList[0].regModuleName);
            cameraId           = uiInfoRegModuleList[0].hasCameraId
                          ? QSharedPointer<int>(new int(CameraType::instance()->getCurrentCamera()))
                          : nullptr;
        }
    }
}

QUiDataMiddleware::QUiDataMiddleware(QString uiModuleName, QString uiControlName, QString originalModuleName,
    QSharedPointer<int> cameraId)
    : uiModuleName(uiModuleName)
    , uiControlName(uiControlName)
    , uiControlIndex(nullptr)
    , originalModuleName(originalModuleName)
    , cameraId(nullptr)
{
    if (originalModuleName.isEmpty()) {
        uiInfoRegModuleList = DatabaseOrm::getUiInfoRegModuleByUiModuleNameAndUiControlName(uiModuleName.toStdString(),
            uiControlName.toStdString());
        if (!uiInfoRegModuleList.empty()) {
            originalModuleName = QString::fromStdString(uiInfoRegModuleList[0].regModuleName);
            cameraId           = uiInfoRegModuleList[0].hasCameraId
                          ? QSharedPointer<int>(new int(CameraType::instance()->getCurrentCamera()))
                          : nullptr;
        }
    }
}

QUiDataMiddleware::~QUiDataMiddleware() { }

QList<DatabaseOrm::CombinedRegister> QUiDataMiddleware::getCombinedRegisterList()
{
    auto uiInfoList = (uiControlIndex == nullptr)
        ? DatabaseOrm::getUiInfoByModuleNameAndControlName(uiModuleName.toStdString(), uiControlName.toStdString())
        : DatabaseOrm::getUiInfoByModuleNameAndControlNameAndControlIndex(uiModuleName.toStdString(),
            uiControlName.toStdString(), *uiControlIndex);

    QList<DatabaseOrm::CombinedRegister> combinedRegisterList;
    for (auto &uiInfo : uiInfoList) {
        if (originalModuleName.isEmpty() && uiInfo.combinedName) {
            for (auto &uiInfoRegModule : uiInfoRegModuleList) {
                std::shared_ptr<int> currentCameraId = uiInfoRegModule.hasCameraId
                    ? std::make_shared<int>(CameraType::instance()->getCurrentCamera())
                    : nullptr;

                auto cr = DatabaseOrm::getCombinedRegisterByModuleNameAndCameraIdAndCombinedName(
                    uiInfoRegModule.regModuleName, currentCameraId, *(uiInfo.combinedName));
                combinedRegisterList.append(cr);
            }
        } else if (!originalModuleName.isEmpty() && uiInfo.combinedName) {
            std::shared_ptr<int> currentCameraId =
                cameraId != nullptr ? std::make_shared<int>(CameraType::instance()->getCurrentCamera()) : nullptr;
            auto cr = DatabaseOrm::getCombinedRegisterByModuleNameAndCameraIdAndCombinedName(
                originalModuleName.toStdString(), currentCameraId, *(uiInfo.combinedName));
            combinedRegisterList.append(cr);
        } else if (uiInfo.registerAddr) {
            auto crList = DatabaseOrm::getCombinedRegisterByRegisterAddress(*uiInfo.registerAddr);
            for (auto &cr : crList) {
                if (originalModuleName.isEmpty()) {
                    combinedRegisterList.append(cr);
                } else if (cr.moduleName == originalModuleName.toStdString()
                    && ((!cr.cameraId && !cameraId) || (cr.cameraId && cameraId && *cr.cameraId == *cameraId))) {
                    combinedRegisterList.append(cr);
                }
            }
        }
    }
    return combinedRegisterList;
}

QList<QPair<DatabaseOrm::CombinedRegister, uint32_t>> QUiDataMiddleware::getCombinedRegisterValueList()
{
    QList<QPair<DatabaseOrm::CombinedRegister, uint32_t>> retList;
    auto                                                  crList = getCombinedRegisterList();
    uint32_t                                              value  = 0;
    for (auto &cr : crList) {
        readCombinedRegisterValue(cr, value);
        retList.append({ cr, value });
    }
    return retList;
}

QList<QPair<DatabaseOrm::CombinedRegister, uint32_t>> QUiDataMiddleware::getCombinedRegisterValueListByCameraId(
    std::shared_ptr<int> cameraId)
{
    QList<QPair<DatabaseOrm::CombinedRegister, uint32_t>> retList;
    auto crList    = DatabaseOrm::getCombinedRegisterByUiModuleNameAndControlNameAndCameraId(uiModuleName.toStdString(),
           uiControlName.toStdString(), cameraId);
    uint32_t value = 0;
    for (auto &cr : crList) {
        readCombinedRegisterValue(cr, value);
        retList.append({ cr, value });
    }
    return retList;
}

bool QUiDataMiddleware::setCombinedRegisterValueList(
    QList<QPair<DatabaseOrm::CombinedRegister, uint32_t>> combinedRegisterValueList)
{
    for (auto &crv : combinedRegisterValueList) {
        writeCombinedRegisterValue(crv.first, crv.second);
    }
    return true;
}

QList<DatabaseOrm::RegisterBitInfo> QUiDataMiddleware::getRegisterBitInfoList()
{
    auto                                crList = getCombinedRegisterList();
    QList<DatabaseOrm::RegisterBitInfo> registerBitInfoList;
    for (auto &cr : crList) {
        auto rbiList = DatabaseOrm::getRegisterBitInfoByCombinedRegister(cr);
        for (auto &rbi : rbiList) {
            registerBitInfoList.append(rbi);
        }
    }
    return registerBitInfoList;
}

QList<uint32_t> QUiDataMiddleware::getRegisterAddressList()
{
    auto            rbiList = getRegisterBitInfoList();
    QSet<uint32_t>  registerAddressSet;
    QList<uint32_t> registerAddressList;
    for (auto &rbi : rbiList) {
        if (registerAddressSet.find(rbi.registerAddr) == registerAddressSet.end()) {
            registerAddressSet.insert(rbi.registerAddr);
            registerAddressList.append(rbi.registerAddr);
        }
    }
    return registerAddressList;
}

QList<QPair<uint32_t, uint32_t>> QUiDataMiddleware::getRegisterAddressValueListFromDbAndDevice()
{
    auto                             rbiList = getRegisterBitInfoList();
    QList<QPair<uint32_t, uint32_t>> registerAddressValueList;
    for (auto &rbi : rbiList) {
        uint32_t value = 0;
        if (0
            != QUsbCommunication::getInstance().readBurst(rbi.registerAddr, (unsigned char *)&value, ADDRESS_LENGTH,
                VALUE_LENGTH)) {
            return {};
        }
        registerAddressValueList.append(QPair<uint32_t, uint32_t>(rbi.registerAddr, value));
    }
    return registerAddressValueList;
}

bool QUiDataMiddleware::getValueFromDevice(uint32_t &registerValue)
{
    registerValue = 0;
    auto rbiList  = getRegisterBitInfoList();
    for (auto &rbi : rbiList) {
        if (rbi.bitMask == 0xff) {
            uint32_t tempValue = 0;
            if (0
                != QUsbCommunication::getInstance().readBurst(rbi.registerAddr, (unsigned char *)&tempValue,
                    ADDRESS_LENGTH, VALUE_LENGTH)) {
                return false;
            }
            // calculate shift
            int shift = 0;
            for (int i = 0; i < 32; i++) {
                if (rbi.combinedBitMask & (1 << i)) {
                    shift = i;
                    break;
                }
            }
            tempValue = tempValue << shift;
            registerValue |= tempValue;
        } else {
            uint32_t tempValue         = 0;
            uint32_t tempRegisterValue = 0;
            if (0
                != QUsbCommunication::getInstance().readBurst(rbi.registerAddr, (unsigned char *)&tempValue,
                    ADDRESS_LENGTH, VALUE_LENGTH)) {
                return false;
            }
            int originShift = -1;
            for (int i = 0; i < 32; i++) {
                if (rbi.combinedBitMask & (1 << i)) {
                    for (int j = originShift + 1; j < 32; j++) {
                        if (rbi.bitMask & (1 << j)) {
                            originShift = j;
                            break;
                        }
                    }
                    tempRegisterValue |= ((tempValue >> originShift) & 1) << i;
                }
            }
            registerValue |= tempRegisterValue;
        }
    }
    return true;
}

bool QUiDataMiddleware::setValueToDevice(uint32_t registerValue)
{
    auto rbiList = getRegisterBitInfoList();
    for (auto &rbi : rbiList) {
        if (rbi.bitMask == 0xff) {
            auto value = registerValue & rbi.combinedBitMask;
            // calculate shift
            int shift = 0;
            for (int i = 0; i < 32; i++) {
                if (rbi.combinedBitMask & (1 << i)) {
                    shift = i;
                    break;
                }
            }
            value = value >> shift;
            QUsbCommunication::getInstance().writeBurst(rbi.registerAddr, (unsigned char *)&value, ADDRESS_LENGTH,
                VALUE_LENGTH);
        } else {
            auto value = 0;
            QUsbCommunication::getInstance().readBurst(rbi.registerAddr, (unsigned char *)&value, ADDRESS_LENGTH,
                VALUE_LENGTH);
            int shift = -1;
            for (int i = 0; i < 32; i++) {
                if (rbi.combinedBitMask & (1 << i)) {
                    for (int j = shift + 1; j < 32; j++) {
                        if (rbi.bitMask & (1 << j)) {
                            shift = j;
                            break;
                        }
                    }
                    value &= ~(1 << shift);
                    value |= ((registerValue >> i) & 1) << shift;
                }
            }
            QUsbCommunication::getInstance().writeBurst(rbi.registerAddr, (unsigned char *)&value, ADDRESS_LENGTH,
                VALUE_LENGTH);
        }
    }
    return true;
}

void QUiDataMiddleware::writeValueToDevice(uint32_t registerValue)
{
    setValueToDevice(registerValue);
}

uint32_t QUiDataMiddleware::readValueFromDevice()
{
    uint32_t value = 0;
    getValueFromDevice(value);
    return value;
}

bool QUiDataMiddleware::writeValueToDevice(QList<QPair<uint32_t, uint32_t>> registerAddressValueList)
{
    for (auto &registerAddressValue : registerAddressValueList) {
        QUsbCommunication::getInstance().writeBurst(registerAddressValue.first,
            (unsigned char *)&registerAddressValue.second, ADDRESS_LENGTH, VALUE_LENGTH);
    }
    return true;
}

QString QUiDataMiddleware::getUiModuleName() const
{
    return uiModuleName;
}

void QUiDataMiddleware::setUiModuleName(const QString &value)
{
    uiModuleName = value;
}

QString QUiDataMiddleware::getUiControlName() const
{
    return uiControlName;
}

void QUiDataMiddleware::setUiControlName(const QString &value)
{
    uiControlName = value;
}

QSharedPointer<int> QUiDataMiddleware::getUiControlIndex() const
{
    return uiControlIndex;
}

void QUiDataMiddleware::setUiControlIndex(const QSharedPointer<int> value)
{
    if (value) {
        uiControlIndex = QSharedPointer<int>(new int(*value));
    } else {
        uiControlIndex = nullptr;
    }
}

QString QUiDataMiddleware::getOriginalModuleName() const
{
    return originalModuleName;
}

void QUiDataMiddleware::setOriginalModuleName(const QString &value)
{
    originalModuleName = value;
}

QSharedPointer<int> QUiDataMiddleware::getCameraId() const
{
    return cameraId;
}

void QUiDataMiddleware::setCameraId(const QSharedPointer<int> value)
{
    if (value) {
        QSharedPointer<int>(new int(*value)).swap(cameraId);
    } else {
        QSharedPointer<int>(nullptr).swap(cameraId);
    }
}

QList<uint32_t> QUiDataMiddleware::getRegisterAddressByUiModule(QString uiModuleName)
{
    auto            regListWithCameraId = DatabaseOrm::getRegistersByUiModuleNameAndCameraId(uiModuleName.toStdString(),
                   std::make_shared<int>(CameraType::instance()->getCurrentCamera()));
    QList<uint32_t> registerAddressList;
    QSet<uint32_t>  registerAddressSet;
    for (auto &reg : regListWithCameraId) {
        if (registerAddressSet.find(reg.address) == registerAddressSet.end()) {
            registerAddressSet.insert(reg.address);
            registerAddressList.append(reg.address);
        }
    }
    // sort
    std::sort(registerAddressList.begin(), registerAddressList.end());
    return registerAddressList;
}

void readCombinedRegisterValue(DatabaseOrm::CombinedRegister &combineReg, uint32_t &value)
{
    int                                       index   = 1;
    int                                       offset  = 0;
    uint8_t                                   valTemp = 0;
    std::vector<DatabaseOrm::RegisterBitInfo> regBitInfo =
        DatabaseOrm::getRegisterBitInfoByCombinedRegister(combineReg);

    value = 0;
    for (auto item : regBitInfo) {
        QUsbCommunication::getInstance().readBurst(item.registerAddr, &valTemp, ADDRESS_LENGTH, VALUE_LENGTH);
        // 取有效比特位
        index  = 1;
        offset = 0;
        while ((item.bitMask & index) == 0) {
            offset++;
            index = (index << 1);
            // 异常保护，防止死循环
            if (offset > COMBINE_VALUE_BIT_MAX) {
                offset = 0;
                break;
            }
        }
        valTemp = ((valTemp & (item.bitMask)) >> offset);

        // 将有效比特位拼入组合寄存器
        index  = 1;
        offset = 0;
        while ((item.combinedBitMask & index) == 0) {
            offset++;
            index = (index << 1);
            // 异常保护，防止死循环
            if (offset > COMBINE_VALUE_BIT_MAX) {
                offset = 0;
                break;
            }
        }
        value = (value & (~item.combinedBitMask)) + (valTemp << offset);
    }
}

void writeCombinedRegisterValue(DatabaseOrm::CombinedRegister &combineReg, uint32_t value)
{
    int                                       index    = 1;
    int                                       offset   = 0;
    uint8_t                                   valWrite = 0;
    uint8_t                                   valRead  = 0;
    std::vector<DatabaseOrm::RegisterBitInfo> regBitInfo =
        DatabaseOrm::getRegisterBitInfoByCombinedRegister(combineReg);

    for (auto item : regBitInfo) {
        QUsbCommunication::getInstance().readBurst(item.registerAddr, &valRead, ADDRESS_LENGTH, VALUE_LENGTH);

        // 取出当前寄存器的值
        index  = 1;
        offset = 0;
        while ((item.combinedBitMask & index) == 0) {
            offset++;
            index = (index << 1);
            // 异常保护，防止死循环
            if (offset > COMBINE_VALUE_BIT_MAX) {
                offset = 0;
                break;
            }
        }
        valWrite = ((value & item.combinedBitMask) >> offset);

        // 取有效比特位
        index  = 1;
        offset = 0;
        while ((item.bitMask & index) == 0) {
            offset++;
            index = (index << 1);
            // 异常保护，防止死循环
            if (offset > COMBINE_VALUE_BIT_MAX) {
                offset = 0;
                break;
            }
        }
        valWrite = (valRead & (~item.bitMask)) + (valWrite << offset);
        QUsbCommunication::getInstance().writeBurst(item.registerAddr, &valWrite, ADDRESS_LENGTH, VALUE_LENGTH);
    }
}
