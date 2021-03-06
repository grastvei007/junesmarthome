#ifndef HS110_H
#define HS110_H
#include "smarthome_global.h"

#include <QObject>
#include <QJsonObject>
#include <QString>

#include <tagsystem/tag.h>

class QTimer;
class QNetworkReply;
class QNetworkAccessManager;


class SMARTHOMESHARED_EXPORT Hs110 : public QObject
{
    Q_OBJECT
public:
    explicit Hs110(const QJsonObject& json, const QString &token);

    const QString& getAlias() const;

    void listenPowerUsage(int pollMs);
    void useJuneServer(bool aServer);

public slots:
    void getPowerUsage(); ///< get power usage one time.

signals:
    void powerReady(double);
    void voltageReady(double);
    void amphereReady(double);

private slots:
    void onPowerUsageReply(QNetworkReply *reply);

private:
    QString mAlias;
    QString mAppServerUrl;
    QString mDeviceId;
    bool mIsRelayOn;
    bool mUseJuneServer;


    QString mToken;
    QTimer *mPollPowerUsageTimer = nullptr;
    QNetworkAccessManager *mNetworkAccessManager = nullptr;

    Tag *mPowerTag;
    Tag *mVoltageTag;
    Tag *mAmphereTag;
};

#endif // HS110_H
