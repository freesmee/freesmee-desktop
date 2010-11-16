#include "qtsingleapplication/qtsingleapplication.h"
#include "mainjacksms.h"

int main(int argc, char **argv)
{
    QtSingleApplication app(argc, argv);
    if (app.isRunning()) {
        app.sendMessage("Duplicate");
        return 0;
    }

    MainJackSMS w;
    app.setActivationWindow(&w, false);
    w.show();

    QObject::connect(&app, SIGNAL(messageReceived(const QString&)), &w, SLOT(anotherInstanceOpened(const QString&)));

    return app.exec();
}
