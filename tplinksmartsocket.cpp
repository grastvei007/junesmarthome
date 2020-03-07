#include "tplinksmartsocket.h"
#include <QNetworkAccessManager>
#include <iostream>

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
    qDebug() << "Command: " << cmd;
    QByteArray byte = encrypt(cmd);
    mTcpSocket.write(byte);
    mTcpSocket.flush();
}


void TpLinkSmartSocket::onDataReady()
{
    QByteArray data = mTcpSocket.readAll();
    QString result = decrypt(data);
    qDebug() << result;
}

void TpLinkSmartSocket::onError(QAbstractSocket::SocketError socketError)
{
    qDebug() << mTcpSocket.errorString();
}


void TpLinkSmartSocket::connectToDevice()
{
    mTcpSocket.connectToHost(mIpAdress, 9999);
}
