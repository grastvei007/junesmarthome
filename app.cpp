#include "app.h"


App::App(int argc, char *argv[]) : QApplication(argc, argv)
{
    mSmartSocket = new TpLinkSmartSocket("192.168.1.23");
}
