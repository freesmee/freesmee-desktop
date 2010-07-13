#ifndef THREADSAVEACCOUNTONLINE_H
#define THREADSAVEACCOUNTONLINE_H

#include <QThread>
#include "libjacksms/libJackSMS.h"
class threadSaveAccountOnline : public QThread
{
      Q_OBJECT

private:
    libJackSMS::dataTypes::configuredServicesType & Esc;
    libJackSMS::dataTypes::servicesType & S;
    QString sessId;
    libJackSMS::dataTypes::configuredAccount account;
    const libJackSMS::dataTypes::optionsType & opzioni;
public:

    threadSaveAccountOnline(libJackSMS::dataTypes::configuredServicesType & _Esc,libJackSMS::dataTypes::servicesType & _S,QString _sessId,libJackSMS::dataTypes::configuredAccount &_account,const libJackSMS::dataTypes::optionsType &_opzioni);
    void run();
signals:
    void saveOk();
    void saveKo();
};

#endif // THREADSAVEACCOUNTONLINE_H
