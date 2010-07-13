#include "libjacksms/libJackSMS.h"
#include <QThread>

#ifndef THREADSENDSMS_H
#define THREADSENDSMS_H


class threadsendsms : public QThread
{
Q_OBJECT
private:
    const libJackSMS::dataTypes::optionsType & opzioni;
    libJackSMS::reportOperationSendSms * op;
    const libJackSMS::dataTypes::configuredAccount &Account;
    const libJackSMS::dataTypes::servicesType &ElencoServizi;

    libJackSMS::dataTypes::shortMessage msg;
    libJackSMS::dataTypes::phoneNumber recipient;


public:
    threadsendsms(const libJackSMS::dataTypes::optionsType & _opzioni,libJackSMS::reportOperationSendSms * _op,const libJackSMS::dataTypes::configuredAccount &_Account,const libJackSMS::dataTypes::servicesType &_ElencoServizi,libJackSMS::dataTypes::shortMessage _msg,libJackSMS::dataTypes::phoneNumber _recipient);
    void run();
    void cancelSend();
private slots:
    void abort();
signals:
    void signalAbort();
    void fatalError();

};

#endif // THREADSENDSMS_H
