#ifndef QCOMMONUIINTERFACE_H
#define QCOMMONUIINTERFACE_H
#include "Common/UI/quidatamiddleware.h"
#include <QMap>
#include <QPointer>
#include <QSharedPointer>
#include <QString>
#include <QWidget>

class QCommonUiInterface;

class QCommonUiInterfaceCreatorBase
{
public:
    virtual ~QCommonUiInterfaceCreatorBase() { }
    virtual QPointer<QCommonUiInterface> createUiInterface(QWidget *)                    = 0;
    virtual QPointer<QCommonUiInterface> createUiInterface(QString, QString, QWidget *)  = 0;
    virtual QPointer<QCommonUiInterface> createUiInterface(QString, QString, QString, QSharedPointer<int>,
        QWidget *)                                                                       = 0;
    virtual QPointer<QCommonUiInterface> createUiInterface(QUiDataMiddleware, QWidget *) = 0;
};

template <typename T>
class QCommonUiInterfaceCreator : public QCommonUiInterfaceCreatorBase
{
public:
    QPointer<QCommonUiInterface> createUiInterface(QWidget *parent = nullptr) { return new T(parent); }
    QPointer<QCommonUiInterface> createUiInterface(QString moduleName, QString controlName, QWidget *parent = nullptr)
    {
        QPointer<QCommonUiInterface> t = new T(parent);
        dynamic_cast<T *>(t.get())->setUiDataMiddleware(QUiDataMiddleware(moduleName, controlName));
        return t;
    }
    QPointer<QCommonUiInterface> createUiInterface(QString moduleName, QString controlName, QString originModuleName,
        QSharedPointer<int> cameraId, QWidget *parent = nullptr)
    {
        QPointer<QCommonUiInterface> t = new T(parent);
        dynamic_cast<T *>(t.get())->setUiDataMiddleware(
            QUiDataMiddleware(moduleName, controlName, originModuleName, cameraId));
        return t;
    }
    QPointer<QCommonUiInterface> createUiInterface(QUiDataMiddleware uiDataMiddleware, QWidget *parent = nullptr)
    {
        QPointer<QCommonUiInterface> t = new T(parent);
        dynamic_cast<T *>(t.get())->setUiDataMiddleware(uiDataMiddleware);
        return t;
    }
};

class QCommonUiInterfaceFactory
{
public:
    inline static QCommonUiInterfaceFactory &instance()
    {
        static QCommonUiInterfaceFactory instance;
        return instance;
    }
    inline QPointer<QCommonUiInterface> createUiInterface(const QString &name, QWidget *parent = nullptr)
    {
        if (!commonUiInterfaceCreator.contains(name)) {
            return nullptr;
        }
        return commonUiInterfaceCreator[name]->createUiInterface(parent);
    }
    inline QPointer<QCommonUiInterface> createUiInterface(const QString &name, QString moduleName, QString controlName,
        QWidget *parent = nullptr)
    {
        if (!commonUiInterfaceCreator.contains(name)) {
            return nullptr;
        }
        return commonUiInterfaceCreator[name]->createUiInterface(moduleName, controlName, parent);
    }
    inline QPointer<QCommonUiInterface> createUiInterface(const QString &name, QString moduleName, QString controlName,
        QString originModuleName, QSharedPointer<int> cameraId, QWidget *parent = nullptr)
    {
        if (!commonUiInterfaceCreator.contains(name)) {
            return nullptr;
        }
        return commonUiInterfaceCreator[name]->createUiInterface(moduleName, controlName, originModuleName, cameraId,
            parent);
    }
    inline QPointer<QCommonUiInterface> createUiInterface(const QString &name, QUiDataMiddleware uiDataMiddleware,
        QWidget *parent = nullptr)
    {
        if (!commonUiInterfaceCreator.contains(name)) {
            return nullptr;
        }
        return commonUiInterfaceCreator[name]->createUiInterface(uiDataMiddleware, parent);
    }

private:
    QCommonUiInterfaceFactory();
    template <typename T>
    void registerUiInterface(const QString &name)
    {
        commonUiInterfaceCreator[name] =
            QSharedPointer<QCommonUiInterfaceCreator<T>>(new QCommonUiInterfaceCreator<T>());
    }
    QMap<QString, QSharedPointer<QCommonUiInterfaceCreatorBase>> commonUiInterfaceCreator;
};

class QCommonUiInterface : public QWidget
{
    Q_OBJECT
public:
    explicit QCommonUiInterface(QWidget *parent = nullptr);
    virtual ~QCommonUiInterface();
    virtual void                        setValue(uint32_t value) = 0;
    virtual uint32_t                    getValue()               = 0;
    virtual QPointer<QUiDataMiddleware> getUiDataMiddleware() const;
    virtual void                        setUiDataMiddleware(const QPointer<QUiDataMiddleware> &value);
    virtual void                        setUiDataMiddleware(const QUiDataMiddleware &value);
    virtual void                        setMouseTracking(bool flag);

signals:
    void signalValueChange(int);
    void releaseUiDataMiddleware(QPointer<QUiDataMiddleware>);

public slots:
    void read();

protected:
    QPointer<QUiDataMiddleware> uiDataMiddleware = nullptr;
};

#endif // QCOMMONUIINTERFACE_H
