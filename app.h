#ifndef APP_H
#define APP_H

#include <QApplication>
#include <QTimer>
#include "tplinksmartsocket.h"

class App : public QApplication
{
    Q_OBJECT
public:
    App(int argc, char *argv[]);

signals:

private slots:
    void onPollTimerTimeout();
private:
    TpLinkSmartSocket *mSmartSocket;
    QTimer *mPollTimer;
};

#endif // APP_H
