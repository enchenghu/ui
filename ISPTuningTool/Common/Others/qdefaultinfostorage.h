#ifndef QDEFAULTINFOSTORAGE_H
#define QDEFAULTINFOSTORAGE_H

#include <QString>
#include "DatabaseOrm/databaseorm.h"
#include "qcommondef.h"

using namespace DatabaseOrm;

class QDefaultInfoStorage
{
public:
    static QDefaultInfoStorage &getInstance();
    void setCurveDefaultInfo(QString moudle,
                             int cameraID,
                             int index,
                             std::vector<std::pair<CombinedRegister, uint32_t>> gammaList);
    std::vector<std::pair<CombinedRegister, uint32_t>> getCurveDefaultInfo(QString moudle,
                                                                           int cameraID,
                                                                           int index);
    //初始化Gamma、GammaGain界面曲线的默认值，用于Reset
    void gammmaInfoInit();

private:
    std::vector<std::pair<CombinedRegister, uint32_t>>
        gammaCurveDefault[CAMERA_ID_MAX][MODULE_GAMMA_GAMMAGAIN_MAX][GAMMA_TYPE_MAX];
};

#endif // QDEFAULTINFOSTORAGE_H
