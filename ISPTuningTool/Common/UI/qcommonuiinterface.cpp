#include "qcommonuiinterface.h"
#include "qhuegraph.h"
#include "qlabelcheckbox.h"
#include "qlabelcombobox.h"
#include "qlabeleditgroup.h"

QCommonUiInterfaceFactory::QCommonUiInterfaceFactory()
{
    registerUiInterface<QLabelEditGroup>("QLabelEditGroup");
    registerUiInterface<QLabelCheckBox>("QLabelCheckBox");
    registerUiInterface<QHueGraph>("QHueGraph");
    registerUiInterface<QLabelComboBox>("QLabelComboBox");
}

QCommonUiInterface::QCommonUiInterface(QWidget *parent)
    : QWidget(parent)
    , uiDataMiddleware(nullptr)
{
}

QCommonUiInterface::~QCommonUiInterface() { }

QPointer<QUiDataMiddleware> QCommonUiInterface::getUiDataMiddleware() const
{
    return uiDataMiddleware;
}

void QCommonUiInterface::setUiDataMiddleware(const QPointer<QUiDataMiddleware> &value)
{
    uiDataMiddleware = new QUiDataMiddleware(*value);
}

void QCommonUiInterface::setUiDataMiddleware(const QUiDataMiddleware &value)
{
    uiDataMiddleware = new QUiDataMiddleware(value);
}

void QCommonUiInterface::setMouseTracking(bool flag) { }

void QCommonUiInterface::read()
{
    if (uiDataMiddleware != nullptr && isVisible()) {
        setValue(uiDataMiddleware->readValueFromDevice());
    }
}
