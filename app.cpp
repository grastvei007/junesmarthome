#include "app.h"


App::App(int argc, char *argv[]) : QApplication(argc, argv)
{
    mSmartSocket = new TpLinkSmartSocket("192.168.1.23");

    mPollTimer = new QTimer(this);
    mPollTimer->setInterval(5000);
    connect(mPollTimer, &QTimer::timeout, this, &App::onPollTimerTimeout);
    mPollTimer->start();
}


void App::onPollTimerTimeout()
{
    mSmartSocket->poll();
}
