#include "qtabwndcommfactory.h"
#include "qcactab.h"
#include "qccmtab.h"
#include "qcombinetab.h"
#include "qcontrast.h"
#include "qdpc.h"
#include "qgamma.h"
#include "qgbalance.h"
#include "qimgfmttab.h"
#include "qlsctab.h"
#include "qpreccmtab.h"
#include "qrgbirtab.h"
#include "qsaturation.h"
#include "qsharpnesstab.h"
#include "qtonemappingtab.h"
#include "qaectab.h"
#include "qdnstab.h"
#include "qawbtab.h"

uint QTabWndComm::m_nLevel=0;
uint QTabWndComm::m_saveType = 1;

QTabWndCommFactory &QTabWndCommFactory::instance()
{
    static QTabWndCommFactory factory;
    return factory;
}

QPointer<QTabWndComm> QTabWndCommFactory::createTabWndComm(const QString &name, QWidget *parent)
{
    if (m_map.contains(name)) {
        return m_map[name](parent);
    }
    return new QTabWndComm(parent);
}

void QTabWndCommFactory::QTabWndUpdateUserLevel(uint level)
{
    QCacTab::setLevel(level);
    QCCMTab::setLevel(level);
    QCombineTab::setLevel(level);
    QContrast::setLevel(level);
    QDPC::setLevel(level);
    QGammaGainTab::setLevel(level);
    QGammaTab::setLevel(level);
    QGBalance::setLevel(level);
    QImgFmtTab::setLevel(level);
    QLscTab::setLevel(level);
    QPreCCMTab::setLevel(level);
    QRGBIRTab::setLevel(level);
    QSaturation::setLevel(level);
    QSharpnessTab::setLevel(level);
    QToneMappingTab::setLevel(level);
    QDNSTab::setLevel(level);
    QAECTab::setLevel(level);
    QAWBTab::setLevel(level);
}

QTabWndCommFactory::QTabWndCommFactory()
{
    registerTabWndComm<QCacTab>("CAC");
    registerTabWndComm<QCCMTab>("CCM");
    registerTabWndComm<QCombineTab>("Combine");
    registerTabWndComm<QContrast>("Contrast");
    registerTabWndComm<QDPC>("DPC");
    registerTabWndComm<QGammaGainTab>("GammaGain");
    registerTabWndComm<QGammaTab>("Gamma");
    registerTabWndComm<QGBalance>("GBalance");
    registerTabWndComm<QImgFmtTab>("ImgFmt");
    registerTabWndComm<QLscTab>("LSC");
    registerTabWndComm<QPreCCMTab>("PRECCM");
    registerTabWndComm<QRGBIRTab>("RGBIR");
    registerTabWndComm<QSaturation>("Saturation");
    registerTabWndComm<QSharpnessTab>("Sharpness");
    registerTabWndComm<QToneMappingTab>("TM");
    registerTabWndComm<QDNSTab>("DNS");
    registerTabWndComm<QAECTab>("AEC");
    registerTabWndComm<QAWBTab>("AWB");
}
