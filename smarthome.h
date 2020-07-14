#ifndef SMARTHOME_H
#define SMARTHOME_H
#include "smarthome_global.h"

#include <QObject>
#include <QString>

#include "hs110.h"

class TpLinkCloud;

class SMARTHOMESHARED_EXPORT SmartHome : public QObject
{
    Q_OBJECT
public:
    SmartHome();
    //webservice
    void useWebservice(bool aUseWebService);
    void useJuneServer(const QString aIp, const QString aClientName);
    void setUserName(const QString &aUserName);
    void setPassword(const QString &aPsw);
    void connect();

    //local lan

    //
    Hs110* getDevice(const QString &aDeviceName);
signals:
    void deviceReady(QString);
private:
    QString mUserName;
    QString mPassword;
    QString mClientName;
    bool mUseWebService;
    bool mUseJuneServer;

    TpLinkCloud *mTpLinkCloud;
};

#endif // SMARTHOME_H
