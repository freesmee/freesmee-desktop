#ifndef THREADPINGSERVER_H
#define THREADPINGSERVER_H
#include "libjacksms/libJackSMS.h"
#include <QThread>

class threadPingServer : public QThread
{
    Q_OBJECT
private:


    QString sessId;
    const libJackSMS::dataTypes::optionsType &opzioni;
    bool result;
    QString error;
public:
    threadPingServer(QString _sessId,const libJackSMS::dataTypes::optionsType &_opzioni);
    void run();
signals:
    void pinged();
};

#endif // THREADPINGSERVER_H
