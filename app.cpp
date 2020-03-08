#include "app.h"

#include <tagsystem/taglist.h>

App::App(int argc, char *argv[]) : QApplication(argc, argv)
{
    mSmartSocket = new TpLinkSmartSocket("192.168.1.23");

    TagList::sGetInstance().setClientName("JuneSmartHome");
    TagList::sGetInstance().connectToServer("localhost", 5000);

    mPollTimer = new QTimer(this);
    mPollTimer->setInterval(5000);
    connect(mPollTimer, &QTimer::timeout, this, &App::onPollTimerTimeout);
    mPollTimer->start();
}


void App::onPollTimerTimeout()
{
    mSmartSocket->poll();
}
