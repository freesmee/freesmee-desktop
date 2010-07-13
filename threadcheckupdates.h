#ifndef THREADCHECKUPDATES_H
#define THREADCHECKUPDATES_H

#include <QThread>
#include "libjacksms/libJackSMS.h"

class threadCheckUpdates : public QThread
{
    Q_OBJECT
private:
    libJackSMS::dataTypes::servicesType &servizi;
    const libJackSMS::dataTypes::optionsType &opzioni;
    QString loginId;
    QString result;

public:
    threadCheckUpdates(libJackSMS::dataTypes::servicesType &_servizi,const libJackSMS::dataTypes::optionsType &_opzioni,QString _loginId);
    void run();
    QString getResult() const;
signals:
    void updatesAvailable();
    void fatalError();
};

#endif // THREADCHECKUPDATES_H
