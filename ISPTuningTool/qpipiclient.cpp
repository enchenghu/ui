#include "qpipiclient.h"
#include "SSUSBLib/SSUSBLib/InnerMethod.h"


QISPClient::QISPClient(QObject* parent)
    : QObject(parent)
{}

QISPClient::~QISPClient()
{}

int QISPClient::sendMessage(const QJsonObject& msgJson, QJsonObject &recvJson)
{
    QUdpSocket socket;
    QHostAddress serverAddress = QHostAddress::LocalHost; // 服务端地址（本机）
    quint16 serverPort = 12345; // 服务端端口
    QByteArray requestData = QJsonDocument(msgJson).toJson(QJsonDocument::Compact);

    //发送数据到服务端
    socket.writeDatagram(requestData, serverAddress, serverPort);

    // 等待服务端响应
    if (socket.waitForReadyRead(1000))
    { // 等待最多 2 秒
        QByteArray responseData;
        responseData.resize(socket.pendingDatagramSize());
        socket.readDatagram(responseData.data(), responseData.size());

        // 解析响应 JSON 数据
        QJsonDocument responseDoc = QJsonDocument::fromJson(responseData);
        if (!responseDoc.isNull() && responseDoc.isObject()) {
            recvJson=responseDoc.object();
            return 0;
        }
    }
    return 1;
}

int QISPClient::sendI2CWrite(const QJsonObject &msgJson)
{
    int verifyCode=-1;
    if (msgJson.contains("verifyCode"))
        verifyCode = msgJson.value("verifyCode").toInt();
    else
        return -1;

    QUdpSocket socket;
    QHostAddress serverAddress = QHostAddress::LocalHost; // 服务端地址（本机）
    quint16 serverPort = 12345; // 服务端端口
    QByteArray requestData = QJsonDocument(msgJson).toJson(QJsonDocument::Compact);

    //发送数据到服务端
    socket.writeDatagram(requestData, serverAddress, serverPort);
    // 等待服务端响应最多 2 秒
    if (socket.waitForReadyRead(1000))
    {
        QByteArray responseData;
        responseData.resize(socket.pendingDatagramSize());
        socket.readDatagram(responseData.data(), responseData.size());

        QJsonDocument responseDoc = QJsonDocument::fromJson(responseData);
        if (!responseDoc.isNull() && responseDoc.isObject()) {
            QJsonObject recvJson=responseDoc.object();
            if (recvJson.contains("verifyCode")) {
                if(verifyCode==(recvJson.value("verifyCode").toInt())){
                     // qDebug() << "I2CWrite Recv JSON:" << QJsonDocument(recvJson).toJson(QJsonDocument::Indented);
                    if (recvJson.contains("error_code"))
                        return (recvJson.value("error_code").toInt());
                }
            }
        }
    }

    return -1;
}

int QISPClient::sendI2CRead(const QJsonObject &msgJson, unsigned char *pBuf)
{
    int verifyCode=-1;
    if (msgJson.contains("verifyCode"))
        verifyCode = msgJson.value("verifyCode").toInt();
    else
        return -1;

    int size=-1;
    if (msgJson.contains("dataSize"))
        size = msgJson.value("dataSize").toInt();
    else
        return -1;

    QUdpSocket socket;
    QHostAddress serverAddress = QHostAddress::LocalHost; // 服务端地址（本机）
    quint16 serverPort = 12345; // 服务端端口
    QByteArray requestData = QJsonDocument(msgJson).toJson(QJsonDocument::Compact);

    //发送数据到服务端
    socket.writeDatagram(requestData, serverAddress, serverPort);
    // 等待服务端响应最多 1 秒
    if (socket.waitForReadyRead(1000))
    {
        QByteArray responseData;
        responseData.resize(socket.pendingDatagramSize());
        socket.readDatagram(responseData.data(), responseData.size());

        QJsonDocument responseDoc = QJsonDocument::fromJson(responseData);
        if (!responseDoc.isNull() && responseDoc.isObject()) {
            QJsonObject recvJson=responseDoc.object();
            if (recvJson.contains("verifyCode")) {
                if(verifyCode==(recvJson.value("verifyCode").toInt())){
                    // 解析字段并检查是否存在
                    if (recvJson.contains("result_data") && recvJson["result_data"].isObject()) {
                        QJsonObject resultData = recvJson["result_data"].toObject();
                        if (resultData.contains("vals") && resultData["vals"].isArray()) {
                            QJsonArray valsArray = resultData["vals"].toArray();
                            if(size==(valsArray.size())){
                                for(int i=0;i<(valsArray.size());i++)
                                    *(pBuf+i) = static_cast<uint8_t>(valsArray[i].toInt());
                            }
                        }
                        // // 打印 JSON 格式的字符串
                         // qDebug() << "I2CRead Recv JSON:" << responseDoc.toJson(QJsonDocument::Indented);
                        if (recvJson.contains("error_code"))
                            return (recvJson.value("error_code").toInt());
                    }
                }
            }
        }
    }

    return -1;
}

int QISPClient::sendSPIWrite(const QJsonObject &msgJson)
{
    int verifyCode=-1;
    if (msgJson.contains("verifyCode"))
        verifyCode = msgJson.value("verifyCode").toInt();
    else
        return -1;

    QUdpSocket socket;
    QHostAddress serverAddress = QHostAddress::LocalHost; // 服务端地址（本机）
    quint16 serverPort = 12345; // 服务端端口
    QByteArray requestData = QJsonDocument(msgJson).toJson(QJsonDocument::Compact);

    //发送数据到服务端
    socket.writeDatagram(requestData, serverAddress, serverPort);
    // 等待服务端响应最多 2 秒
    if (socket.waitForReadyRead(1000))
    {
        QByteArray responseData;
        responseData.resize(socket.pendingDatagramSize());
        socket.readDatagram(responseData.data(), responseData.size());

        QJsonDocument responseDoc = QJsonDocument::fromJson(responseData);
        if (!responseDoc.isNull() && responseDoc.isObject()) {
            QJsonObject recvJson=responseDoc.object();
            if (recvJson.contains("verifyCode")) {
                if(verifyCode==(recvJson.value("verifyCode").toInt())){
                    if (recvJson.contains("error_code"))
                        return (recvJson.value("error_code").toInt());
                }
            }
        }
    }

    return -1;
}

int QISPClient::sendSPIRead(const QJsonObject &msgJson, unsigned char *pBuf)
{
    int verifyCode=-1;
    if (msgJson.contains("verifyCode"))
        verifyCode = msgJson.value("verifyCode").toInt();
    else
        return -1;

    int size=-1;
    if (msgJson.contains("dataSize"))
        size = msgJson.value("dataSize").toInt();
    else
        return -1;

    QUdpSocket socket;
    QHostAddress serverAddress = QHostAddress::LocalHost; // 服务端地址（本机）
    quint16 serverPort = 12345; // 服务端端口
    QByteArray requestData = QJsonDocument(msgJson).toJson(QJsonDocument::Compact);

    //发送数据到服务端
    socket.writeDatagram(requestData, serverAddress, serverPort);
    // 等待服务端响应最多 1 秒
    if (socket.waitForReadyRead(1000))
    {
        QByteArray responseData;
        responseData.resize(socket.pendingDatagramSize());
        socket.readDatagram(responseData.data(), responseData.size());

        QJsonDocument responseDoc = QJsonDocument::fromJson(responseData);
        if (!responseDoc.isNull() && responseDoc.isObject()) {
            QJsonObject recvJson=responseDoc.object();
            if (recvJson.contains("verifyCode")) {
                if(verifyCode==(recvJson.value("verifyCode").toInt())){

                           // 解析字段并检查是否存在
                    if (recvJson.contains("result_data") && recvJson["result_data"].isObject()) {
                        QJsonObject resultData = recvJson["result_data"].toObject();
                        if (resultData.contains("vals") && resultData["vals"].isArray()) {
                            QJsonArray valsArray = resultData["vals"].toArray();
                            if(size==(valsArray.size())){
                                for(int i=0;i<(valsArray.size());i++)
                                    *(pBuf+i) = static_cast<uint8_t>(valsArray[i].toInt());
                            }
                        }

                        if (recvJson.contains("error_code"))
                            return (recvJson.value("error_code").toInt());
                    }
                }
            }
        }
    }

    return -1;
}
