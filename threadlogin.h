#ifndef THREADLOGIN_H
#define THREADLOGIN_H
#include <QThread>
#include "libjacksms/libJackSMS.h"
#include "Types.h"
class ThreadLogin : public QThread
 {
    Q_OBJECT
     private:
        QString error;
        libJackSMS::serverApi::login * signin;
        libJackSMS::dataTypes::servicesType &ElencoServizi;
        libJackSMS::dataTypes::optionsType &Opzioni;

        libJackSMS::dataTypes::logImType ArchivioIm;
        libJackSMS::dataTypes::logSmsType ArchivioMessaggi;

        libJackSMS::dataTypes::configuredServicesType &ElencoServiziConfigurati;

        libJackSMS::dataTypes::phoneBookType &Rubrica;
        QString &userDirectory;
        QString userName;
        types::QMessageListType &messaggi;

     public:
         void run();
         ThreadLogin(libJackSMS::serverApi::login * _signin,libJackSMS::dataTypes::servicesType & _ElencoServizi,libJackSMS::dataTypes::optionsType &_Opzioni,libJackSMS::dataTypes::configuredServicesType &_ElencoServiziConfigurati,libJackSMS::dataTypes::phoneBookType &_rubrica, QString &_userDirectory,QString _userName,types::QMessageListType &_messaggi);
         ~ThreadLogin();
         QString getError();
         bool result;
 signals:
         void endLogin();

 };



#endif // THREADLOGIN_H
