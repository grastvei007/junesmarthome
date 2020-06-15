#include "smarthome.h"

#include "tplinkcloud.h"
#include <tagsystem/taglist.h>

SmartHome::SmartHome() : mUseWebService(false), mUseJuneServer(false)
{

}

void SmartHome::useWebservice(bool aUseWebService)
{
    mUseWebService = aUseWebService;
}

void SmartHome::useJuneServer(bool aServer, const QString aClientName)
{
    mUseJuneServer = aServer;
    mClientName = aClientName;
    if(mUseJuneServer)
    {
        TagList::sGetInstance().setClientName(mClientName);
        TagList::sGetInstance().connectToServer("localhost", 5000);
    }
}


void SmartHome::setUserName(const QString &aUserName)
{
    mUserName = aUserName;
}


void SmartHome::setPassword(const QString &aPsw)
{
    mPassword = aPsw;
}


void SmartHome::connect()
{
    if(mUseWebService)
    {
        mTpLinkCloud = new TpLinkCloud();
        mTpLinkCloud->useJuneServer(mUseJuneServer);
        QObject::connect(mTpLinkCloud, &TpLinkCloud::deviceReady, this, &SmartHome::deviceReady);
        if(mTpLinkCloud->login(mUserName, mPassword))
            mTpLinkCloud->getDeviceList();
        else
            qDebug() << "Unable to connect to cloud";
    }
    else
    {

    }
}


Hs110* SmartHome::getDevice(const QString &aDeviceName)
{
    return mTpLinkCloud->getDevice(aDeviceName);
}
