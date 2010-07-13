#ifndef THREADDELETEACCOUNTONLINE_H
#define THREADDELETEACCOUNTONLINE_H

#include <QThread>

#include "libjacksms/libJackSMS.h"

class threadDeleteAccountOnline : public QThread
{
    Q_OBJECT
private:
    libJackSMS::dataTypes::configuredServicesType & Esc;
    QString sessId;
    QString accountId;
    const libJackSMS::dataTypes::optionsType & opzioni;
public:
    threadDeleteAccountOnline(libJackSMS::dataTypes::configuredServicesType & _Esc,QString _sessId,QString _accountId,const libJackSMS::dataTypes::optionsType &_opzioni);
    void run();
signals:
    void deleteOk();
    void deleteKo();
};

#endif // THREADDELETEACCOUNTONLINE_H
