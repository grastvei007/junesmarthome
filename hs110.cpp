#include "hs110.h"
#include <QTimer>
#include <QDebug>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QJsonDocument>
#include <QJsonArray>
#include <QApplication>

Hs110::Hs110(const QJsonObject& json, const QString &token) : mToken(token),
    mPollPowerUsageTimer(nullptr)
{
    mNetworkAccessManager = new QNetworkAccessManager(this);
    //"alias":"coffeemaker","appServerUrl":"https://eu-wap.tplinkcloud.com",
    //"deviceHwVer":"4.0","deviceId":"80062A0B8B8C70B98EB2BBA94CE85EFD1C8EDAD5",
    //"deviceMac":"1C3BF3650C82",
    //"deviceModel":"HS110(EU)",
    //"deviceName":"Smart Wi-Fi Plug With Energy Monitoring",
    //"deviceRegion":"eu-west-1","deviceType":"IOT.SMARTPLUGSWITCH",
    //"fwId":"00000000000000000000000000000000",
    //"fwVer":"1.0.4 Build 191111 Rel.143500",
    //"hwId":"06D9793BF7C4C3E37A386CB6C5D4A929","isSameRegion":true,
    //"oemId":"BC7DF59F436483CD3D8396111011B83E","role":0,"status":1})

    mAlias = json.value("alias").toString();
    mAppServerUrl = json.value("appServerUrl").toString();
    mDeviceId = json.value("deviceId").toString();
    mIsRelayOn = json.value("status").toInt() == 1 ? true : false;
    qDebug() << "Alias" << mAlias;
}


const QString& Hs110::getAlias() const
{
    return mAlias;
}


void Hs110::listenPowerUsage(int pollMs)
{
    if(!mPollPowerUsageTimer)
        mPollPowerUsageTimer = new QTimer(this);
    if(pollMs <= 0)
    {
        mPollPowerUsageTimer->deleteLater();
        mPollPowerUsageTimer = nullptr;
        return;
    }
    connect(mPollPowerUsageTimer, &QTimer::timeout, this, &Hs110::getPowerUsage);
    mPollPowerUsageTimer->setInterval(pollMs);
    mPollPowerUsageTimer->start();

}


void Hs110::getPowerUsage()
{
    qDebug() << __FUNCTION__;
    QNetworkRequest request(QUrl(QString("%1?token=%2").arg(mAppServerUrl).arg(mToken)));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject pass;
    pass.insert("method", "passthrough");

    QJsonObject realtime;
    realtime.insert("get_realtime", QJsonValue());

    QJsonObject emeter;
    emeter.insert( "emeter", realtime);

    QJsonObject val;
    val.insert("deviceId", mDeviceId);
    val.insert("requestData", emeter);


    pass.insert("params", val);

    QNetworkReply *reply = mNetworkAccessManager->post(request, QJsonDocument(pass).toJson(QJsonDocument::Compact));
    while (!reply->isFinished())
    {
        qApp->processEvents();
    }
    onPowerUsageReply(reply);
}


void Hs110::onPowerUsageReply(QNetworkReply *reply)
{
    qDebug() << __FUNCTION__;
    //qDebug() << reply->readAll();
    //"{\"error_code\":0,\"result\":{\"responseData\":{\"emeter\":{\"get_realtime\":{\"voltage_mv\":225331,\"power_mw\":26463,\"err_code\":0,\"total_wh\":1473,\"current_ma\":278}}}}}"
    QJsonDocument jsonReply = QJsonDocument::fromJson(reply->readAll());
    reply->deleteLater();
    if(jsonReply.isObject())
    {
        QJsonObject replyObj = jsonReply.object();
        if(!replyObj.contains("error_code"))
            qDebug() << "error recieving emeter";
        else
        {
            int errorcode = replyObj.value("error_code").toInt();
            if(errorcode != 0)
            {
                qDebug() << "Error(" << errorcode << ")";
                return;
            }
            QJsonValue result = replyObj.value("result");
            if(result.isObject())
            {
                QJsonObject resultObj = result.toObject();
                //qDebug() << resultObj;
                QJsonValue responseData = resultObj.value("responseData");
                if(responseData.isObject())
                {

                    QJsonObject responseDataObj = responseData.toObject();
                    //qDebug() << emeterObj;
                    QJsonObject emeter = responseDataObj.value("emeter").toObject();
                    QJsonObject realtime = emeter.value("get_realtime").toObject();
                    qDebug() << realtime;
                    double voltage = realtime.value("voltage_mv").toInt();
                    double amphere = realtime.value("current_ma").toInt();
                    double power = realtime.value("power_mw").toInt();

                    qDebug() << voltage << "mV";
                    qDebug() << amphere << "mA";
                    qDebug() << power << "mW";

                    emit voltageReady(voltage);
                    emit amphereReady(amphere);
                    emit powerReady(power);
                }
            }
            else
                qDebug() << "Result is not an object";
        }
    }
    else
        qDebug() << "No object";
}
