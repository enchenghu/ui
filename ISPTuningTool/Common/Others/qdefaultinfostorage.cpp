#include "qdefaultinfostorage.h"
#include "../UI/qtabwndcomm.h"

QDefaultInfoStorage &QDefaultInfoStorage::getInstance()
{
    static QDefaultInfoStorage instance;
    return instance;
}

void QDefaultInfoStorage::setCurveDefaultInfo(
    QString moudle,
    int cameraID,
    int index,
    std::vector<std::pair<CombinedRegister, uint32_t>> gammaList)
{
    int moudleID = 0;
    if (moudle == "Gamma")
        moudleID = MODULE_GAMMA;
    else
        moudleID = MODULE_GAMMAGAIN;
    gammaCurveDefault[cameraID][moudleID][index].swap(gammaList);
}

std::vector<std::pair<CombinedRegister, uint32_t>> QDefaultInfoStorage::getCurveDefaultInfo(
    QString moudle, int cameraID, int index)
{
    std::vector<std::pair<CombinedRegister, uint32_t>> gammaList;
    int moudleID = 0;
    if (moudle == "Gamma")
        moudleID = MODULE_GAMMA;
    else
        moudleID = MODULE_GAMMAGAIN;
    gammaList.assign(gammaCurveDefault[cameraID][moudleID][index].begin(),
                     gammaCurveDefault[cameraID][moudleID][index].end());
    return gammaList;
}

void QDefaultInfoStorage::gammmaInfoInit()
{
    std::vector<UiInfo> uiInfo;
    std::vector<std::pair<CombinedRegister, uint32_t>> gammaList; //用于存储曲线点的信息
    QStringList uiName = {"Gamma A", "Gamma B"};
    QStringList moduleName = {"Gamma", "GammaGain"};
    QString regMoudleName = "CURVE";

    for (int cameraId = 0; cameraId < CAMERA_ID_MAX; cameraId++) {
        for (int m = 0; m < MODULE_GAMMA_GAMMAGAIN_MAX; m++) {
            for (int t = 0; t < GAMMA_TYPE_MAX; t++) {
                gammaList.clear();
                QPointer<QUiDataMiddleware> data             = new QUiDataMiddleware(moduleName.at(m), uiName.at(t));
                auto combineListValue = data->getCombinedRegisterValueListByCameraId(std::make_shared<int>(cameraId));
                gammaList = std::vector<std::pair<CombinedRegister, uint32_t>>(combineListValue.constBegin(),
                    combineListValue.constEnd());
                //保存Default值
                setCurveDefaultInfo(moduleName.at(m), cameraId, t, gammaList);
            }
        }
    }
}
