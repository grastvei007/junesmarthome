#include "smarthome.h"

#include "tplinkcloud.h"

SmartHome::SmartHome() : mUseWebService(false)
{

}

void SmartHome::useWebservice(bool aUseWebService)
{
    mUseWebService = aUseWebService;
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
