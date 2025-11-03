#ifndef QPIPICLIENT_H
#define QPIPICLIENT_H

#include <QObject>
#include <QThread>
#include <QString>
#include <QMutex>
#include <QDebug>
#include <windows.h>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUdpSocket>
#include <QIODevice>
class QISPClient : public QObject
{
    Q_OBJECT

public:

    static QISPClient &getInstance()
    {
        static QISPClient instance;
        return instance;
    }

    // 消息发送接口
    int sendMessage(const QJsonObject& msgJson,QJsonObject& recvJson);

    int sendI2CWrite(const QJsonObject& msgJson);
    int sendI2CRead(const QJsonObject& msgJson, unsigned char *pBuf);
    int sendSPIWrite(const QJsonObject& msgJson);
    int sendSPIRead(const QJsonObject& msgJson, unsigned char *pBuf);


private:
    QString m_pipeName;       // 管道名称
    DWORD processId;
    QMutex m_mutex;           // 用于线程安全

     QISPClient( QObject* parent = nullptr);
    ~QISPClient();

};

#endif // QPIPICLIENT_H
