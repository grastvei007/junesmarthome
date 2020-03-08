#include "tplinksmartsocket.h"
#include <QNetworkAccessManager>
#include <iostream>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <tagsystem/taglist.h>


TpLinkSmartSocket::TpLinkSmartSocket(const QString &aIpAdress) : QObject(),
    mIpAdress(aIpAdress)
{
    mCommands.insert(std::make_pair(QString("energy"), QByteArray("{\"emeter\":{\"get_realtime\":{}}}")));
    mCommands.insert(std::make_pair(QString("info"), QByteArray("{\"system\":{\"get_sysinfo\":{}}}")));
    mCommands.insert(std::make_pair(QString("on"), QByteArray("{\"system\":{\"set_relay_state\":{\"state\":1}}}")));
    mCommands.insert(std::make_pair(QString("off"), QByteArray("{\"system\":{\"set_relay_state\":{\"state\":0}}}")));

    connect(&mTcpSocket, &QTcpSocket::connected, this, &TpLinkSmartSocket::onConnected);
    connect(&mTcpSocket, &QTcpSocket::readyRead, this, &TpLinkSmartSocket::onDataReady);
    connect(&mTcpSocket, qOverload<QAbstractSocket::SocketError>(&QTcpSocket::error), this, &TpLinkSmartSocket::onError);
}


void TpLinkSmartSocket::poll()
{
    if(mIsRelayStateOn)
        energy();
    else
        info();
}


void TpLinkSmartSocket::info()
{
    mCurrentCommand = "info";
    connectToDevice();
}


void TpLinkSmartSocket::on()
{
    mCurrentCommand = "on";
    connectToDevice();
}


void TpLinkSmartSocket::off()
{
    mCurrentCommand = "off";
    connectToDevice();
}


void TpLinkSmartSocket::energy()
{
    mCurrentCommand = "energy";
    connectToDevice();
}


QByteArray TpLinkSmartSocket::encrypt(const QByteArray &str)
{
    QByteArray result;
    result.append(char(0));
    result.append(char(0));
    result.append(char(0));
    result.append(char(str.size()));
    unsigned char key = 171;
    for(unsigned int i=0; i<str.size(); ++i)
    {
        unsigned char a = key ^ (unsigned char)str.at(i);
        key = a;
        result.append((unsigned char)a);
    }
    return result;
}


QString TpLinkSmartSocket::decrypt(const QByteArray &msg)
{
    QString result;
    unsigned char key = 171;
    for(int i = 4; i<msg.size(); ++i)
    {
        unsigned char a = key ^ (unsigned char)msg[i];
        key = (unsigned char)msg[i];
        result.append((unsigned char)a);
    }

    return result;
}


void TpLinkSmartSocket::onConnected()
{
    QByteArray cmd = mCommands[mCurrentCommand];
    //qDebug() << "Command: " << cmd;
    QByteArray byte = encrypt(cmd);
    mTcpSocket.write(byte);
    mTcpSocket.flush();
}


void TpLinkSmartSocket::onDataReady()
{
    QByteArray data = mTcpSocket.readAll();
    mTcpSocket.close();
    QString result = decrypt(data);
    //qDebug() << result;

    QJsonDocument json = QJsonDocument::fromJson(result.toLatin1());
    if(json.isObject())
    {
        QJsonObject object = json.object();
        if(object.contains("system"))
        {
            parseSystemResponse(object);
        }
        else if(object.contains("emeter"))
        {
            parseEmeter(object);
        }
    }
    else
    {
        qDebug() << "unknown respons: " << result;
    }
}


void TpLinkSmartSocket::parseSystemResponse(const QJsonObject &system)
{
    QJsonValue value = system.value("system");
    if(value.isObject())
    {
        QJsonObject obj = value.toObject();
        if(obj.contains("get_sysinfo"))
        {
            QJsonValue systemInfo = obj.value("get_sysinfo");
            if(systemInfo.isObject())
            {
                QJsonObject info = systemInfo.toObject();
                if(mAlias.isEmpty())
                {
                    mAlias = info.value("alias").toString();
                    createTags();
                }
                mIsRelayStateOn = info.value("relay_state").toInt() == 1 ? true : false;
                if(mRelayStateOnTag)
                    mRelayStateOnTag->setValue(mIsRelayStateOn);
            }
        }
    }
}


void TpLinkSmartSocket::parseEmeter(const QJsonObject &emeter)
{
    QJsonValue value = emeter.value("emeter");
    if(value.isObject())
    {
        QJsonObject emeterObject = value.toObject();
        if(emeterObject.contains("get_realtime"))
        {
            QJsonValue realtimeValue = emeterObject.value("get_realtime");
            if(realtimeValue.isObject())
            {
                QJsonObject realtime = realtimeValue.toObject();
                mVoltage = realtime.value("voltage_mv").toInt();
                mAmpere = realtime.value("current_ma").toInt();
                mPower = realtime.value("power_mw").toInt();

                if(mVoltageTag)
                    mVoltageTag->setValue(mVoltage);
                if(mAmpereTag)
                    mAmpereTag->setValue(mAmpere);
                if(mPowerTag)
                    mPowerTag->setValue(mPower);

                qDebug() << mVoltage << "mV " << mAmpere << "mA " << mPower << "mW";
            }
        }
    }
}


void TpLinkSmartSocket::onError(QAbstractSocket::SocketError socketError)
{
    qDebug() << mTcpSocket.errorString();
}


void TpLinkSmartSocket::connectToDevice()
{
    mTcpSocket.connectToHost(mIpAdress, 9999);
}


void TpLinkSmartSocket::createTags()
{
    if(!mVoltageTag)
    {
        mVoltageTag = TagList::sGetInstance().createTag(mAlias, "voltage", Tag::eInt);
        mVoltageTag->setValue(mVoltage);
    }
    if(!mAmpereTag)
    {
        mAmpereTag = TagList::sGetInstance().createTag(mAlias, "amphere", Tag::eInt);
        mAmpereTag->setValue(mAmpere);
    }
    if(!mPowerTag)
    {
        mPowerTag = TagList::sGetInstance().createTag(mAlias, "power", Tag::eInt);
        mPowerTag->setValue(mPower);
    }
    if(!mRelayStateOnTag)
    {
        mRelayStateOnTag = TagList::sGetInstance().createTag(mAlias, "relayStateOn", Tag::eBool);
        mRelayStateOnTag->setValue(mIsRelayStateOn);
    }
}
