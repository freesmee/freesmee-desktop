/*
    Copyright (C) <2011>

    <enrico bacis> <enrico.bacis@gmail.com>
    <ivan vaccari> <grisson@jacksms.it>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

/*!
  \file ServerApi.h
  \brief Contiene le definizioni delle classi che implementano le api lato client per l'interfacciamento al server di jacksms.it
  \ingroup apiInterface
 */

#ifndef SERVERAPI_H
#define SERVERAPI_H 1

#include <QString>
#include "xmlParserServerApi.h"
#include "NetClient.h"
#include "DataTypes.h"
#include "SocketClient.h"
#include <QObject>
#include <QTimer>
#include <QTcpSocket>
#include <QThread>

namespace libJackSMS
{
    namespace serverApi
    {
        class loginBase : public QObject
        {
            Q_OBJECT

            private:
                QString username;
                QString password;
                QString idsessione;
                xmlParserApi::xmlParserServerApiGeneric *xmlResponse;
                netClient::netClientGeneric *webClient;

            public:
                loginBase(const QString &_username, const QString &_password);
                ~loginBase();
                void doLogin();

            signals:
                void loginSuccess(QString loginId);
                void loginFailed(QString error);
                void phoneBookReceived(libJackSMS::dataTypes::phoneBookType rubrica);
                void accountsReceived(libJackSMS::dataTypes::configuredServicesType serviziConfigurati);
                void newVersionAvailable();
        };

        class login : public QThread
        {
            Q_OBJECT

            public:
                login(const QString &_username, const QString &_password);
                ~login();
                void doLogin();
                void abort();

            private:
                void run();
                QString username;
                QString password;
                loginBase *l;

            signals:
                void loginStarted();
                void loginSuccess(QString loginId);
                void loginFailed(QString error);
                void phoneBookReceived(libJackSMS::dataTypes::phoneBookType rubrica);
                void accountsReceived(libJackSMS::dataTypes::configuredServicesType serviziConfigurati);
                void newVersionAvailable();
        };

        /*!
        \brief fornisce l'implementazione delle api per pingare il server e non far scadere la sessione.
        \ingroup serverApi

        Esempio
        \code
            //si suppone client oggetto di tipo libJackSMS::serverApi::login
            QString sessionId=client.getSessionId();
            libJackSMS::serverApi::pingator ping(sessionId);
            if (ping.ping()){
                //ping ok

            }else{
                //ping fallito


            }
        \endcode

          */
        class pingator : public QThread
        {
            Q_OBJECT

            private:
                QString idsessione;
                void run();
                int minutes;
                QTimer timer;

            public:
                pingator(const QString &_idsessione);
                ~pingator();
                void setPingInterval(int minutes);

            public slots:
                void launchPing();

            signals:
                void pinged();
        };

        void synchronizeVariables(dataTypes::configuredServicesType &_serviziConfigurati, const dataTypes::servicesType &_servizi);

        /*! \brief riceve i messaggi JMS in coda
          \ingroup serverApi
          Riceve i messaggi JMS in coda e li marca come letti.
          */
        class instantMessenger
        {

            private:
                QString loginId;
                libJackSMS::dataTypes::logImType messages;
                netClient::netClientGeneric *webClient;

            public:
                /*! instantMessenger

                   \param _loginId id della sessione corrente
                  */
                instantMessenger(const QString &_loginId);

                /*! checkMessages

                  \return true se sono presenti messaggi in coda, false altrimenti

                  */
                bool checkMessages();

                /*! getMessages
                  \param _messages Oggetto di tipo libJackSMS::dataTypes::logImType dove verranno salvati i messaggi in coda.
                  \return true se il parsing ha avuto successo, false altrimenti

                  */
                bool getMessages(libJackSMS::dataTypes::logImType &_messages);
        };


        /*! \brief Implementa le api per salvare un sms sul server di jacksms.it
          \ingroup serverApi

          */
        class smsLogSaver : public QThread
        {
            Q_OBJECT

            private:
                QString loginId;
                dataTypes::logSmsMessage msg;
                void run();

            public:
                smsLogSaver(QString _loginId);
                void save(dataTypes::logSmsMessage _msg);

            signals:
                void smsSaved(libJackSMS::dataTypes::logSmsMessage,QString);
                void smsNotSaved();
        };

        class smsLogFailed : public QThread
        {
            Q_OBJECT

            private:
                QString loginId;
                dataTypes::logSmsMessage msg;
                QString error;
                void run();

            public:
                smsLogFailed(QString _loginId);
                void reportFail(dataTypes::logSmsMessage _msg,QString _error);

            signals:
                void reportSuccess();
                void reportFailed();
        };

        class contactManagerAdd : public QThread
        {
            Q_OBJECT

            private:
                QString loginId;
                void run();
                libJackSMS::dataTypes::contact contatto;

            public:
                contactManagerAdd(const QString &_loginId);
                void addNewContact(libJackSMS::dataTypes::contact _contatto);

            signals:
                void contactAdded(QString, bool, int);
                void errorAdd();
        };

        class contactManagerUpdate : public QThread
        {
            Q_OBJECT

            private:
                QString loginId;
                void run();
                libJackSMS::dataTypes::contact contatto;

            public:
                contactManagerUpdate(const QString & _loginId);
                void updateContact(libJackSMS::dataTypes::contact _contatto);

            signals:
                void contactUpdated(libJackSMS::dataTypes::contact);
                void errorUpdate();
        };

        class contactManagerDelete : public QThread
        {
            Q_OBJECT

            private:
                QString loginId;
                QString id;
                void run();

            public:
                contactManagerDelete(const QString &_loginId);
                void deleteContact(QString _id);

            signals:
                void contactDeleted(QString);
                void errorDelete();
        };

        class contactManager : public QObject
        {
            Q_OBJECT

            private:
                QString loginId;
                contactManagerAdd* manAdd;
                contactManagerDelete *manDel;
                contactManagerUpdate * manUp;

            public:
                contactManager(const QString &_loginId);
                void addNewContact(libJackSMS::dataTypes::contact _contatto);
                void updateContact(libJackSMS::dataTypes::contact _contatto);
                void deleteContact(QString _id);

            signals:
                void contactDeleted(QString);
                void contactNotDeleted();
                void contactUpdated(libJackSMS::dataTypes::contact);
                void contactNotUpdated();
                void contactSaved(QString, bool, int);
                void contactNotSaved();
        };

        class accountManagerUpdate : public QThread
        {
            Q_OBJECT

            private:
                QString loginId;
                libJackSMS::dataTypes::service s;
                void run();
                libJackSMS::dataTypes::configuredAccount account;

            public:
                accountManagerUpdate(const QString & _loginId, libJackSMS::dataTypes::service _s);
                void updateAccount(libJackSMS::dataTypes::configuredAccount _account);

            signals:
                void accountUpdated(libJackSMS::dataTypes::configuredAccount);
                void errorUpdate();
        };

        class accountManagerDelete : public QThread
        {
            Q_OBJECT

            private:
                QString loginId;
                QString id;
                void run();

            public:
                accountManagerDelete(const QString &_loginId);
                void deleteAccount(const QString &_id);

            signals:
                void accountDeleted(QString);
                void errorDelete();
        };

        class accountManagerAdd : public QThread
        {
            Q_OBJECT

            private:
                QString loginId;
                void run();
                libJackSMS::dataTypes::service service;
                libJackSMS::dataTypes::configuredAccount account;

            public:
                accountManagerAdd(const QString &_loginId);
                void addNewAccount(libJackSMS::dataTypes::service _service,libJackSMS::dataTypes::configuredAccount &_account);

            signals:
                void accountAdded(QString);
                void errorAdd();
        };

        class accountManager : public QObject
        {
            Q_OBJECT

            private:
                QString loginId;
                accountManagerDelete *manDel;
                accountManagerAdd *manAdd;
                accountManagerUpdate *manUp;

            public:
                accountManager(const QString &_loginId);
                void addNewAccount(libJackSMS::dataTypes::service _service, libJackSMS::dataTypes::configuredAccount &_account);
                void updateAccount(libJackSMS::dataTypes::configuredAccount &_account, libJackSMS::dataTypes::service s);
                void deleteAccount(QString _id);

            signals:
                void accountDeleted(QString);
                void accountNotDeleted();
                void accountUpdated(libJackSMS::dataTypes::configuredAccount);
                void accountNotUpdated();
                void accountSaved(QString);
                void accountNotSaved();
            };


        //class conversationManager {
        //
        //   private:
        //       xmlParserApi::xmlParserServerApiGeneric *xmlDocument;
        //       QString loginId;
        //       netClient::netClientGeneric *webClient;
        //
        //   public:
        //       /*!
        //          \param _loginId id della sessione corrente
        //         */
        //       conversationManager(const QString & _loginId);
        //       /*!
        //          \param _logSms Struttura dati dove verranno salvati gli sms inviati( JMS compresi)
        //          \param _logIm Struttura dati dove verranno salvati i JMS ricevuti
        //          \return true se il parsing ha avuto successo, false altrimenti
        //
        //          Riceve dal server gli ultimi 100 messaggi inviati e ricevuti
        //
        //         */
        //       bool downloadLastMessages(libJackSMS::dataTypes::logSmsType &_logSms, libJackSMS::dataTypes::logImType &_logIm);
        //};

        class updateServicesManager : public QThread
        {
            Q_OBJECT

            private:
                QString loginId;
                libJackSMS::dataTypes::servicesType servizi;
                void run();
                bool aborted;

            public:
                updateServicesManager(const QString &_loginId, libJackSMS::dataTypes::servicesType _servizi);
                void checkUpdates();
                void abort();

            signals:
                void abortSignal();
                void criticalError(QString);
                void error(QString);
                void updatesAvailable(libJackSMS::dataTypes::servicesType _servizi,QString xml,QString message);
        };

        class updateServicesManagerBase : public QObject
        {
            Q_OBJECT

            private:
                xmlParserApi::xmlParserServerApiGeneric *xmlDocument;
                QString loginId;
                netClient::netClientGeneric *webClient;
                QList<QPair<QString,QString> > updateResults;
                QString servXml;
                bool aborted;

            public slots:
                void abort();

            public:
                updateServicesManagerBase(const QString &_loginId);
                bool downloadUpdates(libJackSMS::dataTypes::servicesType &_servizi);
                QString getMessage() const;
                QString getXml() const;
        };


        class reloader :public QObject
        {
            Q_OBJECT

            private:
                xmlParserApi::xmlParserServerApiGeneric *xmlDocument;
                QString loginId;
                netClient::netClientGeneric *webClient;

            public:
                /*!
                   \param _loginId id della sessione corrente
                  */
                reloader(const QString & _loginId);
                bool reloadPhoneBook();
        signals:
                void errorReload();
                void phoneBookReloaded(libJackSMS::dataTypes::phoneBookType);

        };

        /*class permanentInstantMessenger : public QObject
        {
            Q_OBJECT

        private:
            QString username;
            QString password;
            QString error;
            QString userDir;
            QTimer pingTimer;
            QTimer signalCountdown;
            QTimer pingTimeout;
            QTimer reconnectTimer;
            QTimer dataTimeout;
            QTcpSocket sock;
            libJackSMS::dataTypes::logImType imLog;
            int id;
            QByteArray buffer;

        public:
            permanentInstantMessenger(QString _username, QString _password);
            ~permanentInstantMessenger();
            void activateServ();
            void stop();

        private slots:
            void launchSignal();
            void connectDone();
            void parseLine();
            void relaunchDisconnected();
            void errorDisconnected(QAbstractSocket::SocketError);
            void state(QAbstractSocket::SocketState e);
            void ping();
            void pingTimeoutError();
            void tryReconnect();
            void dataReceived();

        signals:
            void newJMS(libJackSMS::dataTypes::logImType);
            void serviceActive();
            void serviceNotActive(bool err = false, QString errStr = "");
            void serviceActivating();
        };*/

        class cyclicMessengerChecker :
                public QThread
        {
            Q_OBJECT

        private:
            QString loginString;
            QTimer timeout;

        public:
            cyclicMessengerChecker(QString _loginString);
            ~cyclicMessengerChecker();
            void activateServ();
            void stop();

        private slots:
             void run();

        signals:
            void newJMS(libJackSMS::dataTypes::logImType);
            void serviceActive();
            void serviceNotActive();
        };

        class gmailAddressBookImporter : public QThread
        {
            Q_OBJECT

            private:
                QString loginId;
                QString gmailuser;
                QString gmailpsw;
                void run();

            public:
                gmailAddressBookImporter(QString _loginId);
                void import(QString _user, QString _psw);

            signals:
                void importDone(int numberOfimports);
                void importError(QString error);
                void wrongCredentials();
        };
    }
}

#endif //SERVERAPI_H
