#ifndef APP_H
#define APP_H

#include <QApplication>
#include "tplinksmartsocket.h"

class App : public QApplication
{
    Q_OBJECT
public:
    App(int argc, char *argv[]);

signals:

public slots:

private:
    TpLinkSmartSocket *mSmartSocket;
};

#endif // APP_H
