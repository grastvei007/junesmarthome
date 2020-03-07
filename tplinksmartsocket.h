#ifndef TPLINKSMARTSOCKET_H
#define TPLINKSMARTSOCKET_H

#include <QObject>
#include <map>
#include <string>
#include <QByteArray>
#include <QTcpSocket>
#include <QAbstractSocket>
#include <QDebug>

class TpLinkSmartSocket : public QObject
{
    Q_OBJECT
public:
    explicit TpLinkSmartSocket(const QString &aIpAdress);

   //commands
    void info();
    void on();
    void off();
    void energy();

signals:

public slots:

private slots:
    void onConnected();
    void onDataReady();
    void onError(QAbstractSocket::SocketError socketError);
private:
    void connectToDevice();
    QByteArray encrypt(const QByteArray &str);
    QString decrypt(const QByteArray &msg);

private:
    std::map<QString, QByteArray> mCommands;
    QTcpSocket mTcpSocket;
    QString mIpAdress;
    QString mCurrentCommand;
};

#endif // TPLINKSMARTSOCKET_H
