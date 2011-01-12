/*
    Copyright (C) <2009>  <ivan vaccari> <grisson@jacksms.it>

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

#include <QString>
#include "xmlParserServerApi.h"
#include "NetClient.h"
#include "DataTypes.h"
#include "ProxyConfig.h"
#include "SocketClient.h"
#include <QObject>
#include <QTimer>
#include <QTcpSocket>

#include <QThread>
#ifndef SERVERAPI_H
#define SERVERAPI_H 1




namespace libJackSMS{

    namespace serverApi{


        class loginBase:public QObject{
            Q_OBJECT
            private:
                QString username;
                QString password;
                QString idsessione;
                dataTypes::proxySettings ps;
                xmlParserApi::xmlParserServerApiGeneric *xmlResponse;
                netClient::netClientGeneric *webClient;

            public:
                loginBase(const QString &_username,const QString &_password,dataTypes::proxySettings _ps );
                ~loginBase();
                void doLogin();
            signals:
                void loginSuccess(QString loginId);
                void loginFailed(QString error);
                void phoneBookReceived(libJackSMS::dataTypes::phoneBookType rubrica);
                void accountsReceived(libJackSMS::dataTypes::configuredServicesType serviziConfigurati);
                void newVersionAvailable();


        };

        class login:public QThread{
            Q_OBJECT
            public:
                login(const QString &_username,const QString &_password,dataTypes::proxySettings _ps );
                ~login();
                void doLogin();
                void abort();
            private:
                void run();
                QString username;
                QString password;
                dataTypes::proxySettings ps;
                loginBase *l;
            private slots:
                void slotLoginSuccess(QString loginId);
                void slotLoginFailed(QString error);
                void slotPhoneBookReceived(libJackSMS::dataTypes::phoneBookType rubrica);
                void slotAccountsReceived(libJackSMS::dataTypes::configuredServicesType serviziConfigurati);
                void slotNewVersionAvailable();
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
        class pingator:public QThread{
            Q_OBJECT
            private:
                QString idsessione;
                dataTypes::proxySettings ps;
                void run();
                int minutes;
                QTimer timer;
            public:
                pingator(const QString &_idsessione,dataTypes::proxySettings _ps );
                ~pingator();
                void setPingInterval(int minutes);
            public slots:
                void launchPing();

            signals:
                void pinged();
        };
        bool synchronizeVariables(dataTypes::configuredServicesType & _serviziConfigurati,const dataTypes::servicesType & _servizi);

        /*! \brief riceve i messaggi JMS in coda
          \ingroup serverApi
          Riceve i messaggi JMS in coda e li marca come letti.
          */
        class instantMessenger{
            private:
                QString loginId;
                libJackSMS::dataTypes::logImType messages;
                dataTypes::proxySettings ps;
                netClient::netClientGeneric *webClient;
            public:
                /*! instantMessenger

                   \param _loginId id della sessione corrente
                   \param _ps oggetto di tipo dataTypes::proxySettings che mantiene le impostazioni del proxy

                  */
                instantMessenger(const QString & _loginId,dataTypes::proxySettings _ps );
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
        class smsLogSaver:public QThread{
            Q_OBJECT
            private:
                QString loginId;
                dataTypes::logSmsMessage msg;
                dataTypes::proxySettings ps;
                void run();
            public:
                smsLogSaver(QString _loginId,dataTypes::proxySettings _ps );
                void save(dataTypes::logSmsMessage _msg);
            signals:
                void smsSaved(libJackSMS::dataTypes::logSmsMessage,QString);
                void smsNotSaved();

        };

        class smsLogFailed:public QThread{
            Q_OBJECT
            private:
                QString loginId;

                dataTypes::logSmsMessage msg;
                QString error;
                dataTypes::proxySettings ps;
                void run();
            public:
                smsLogFailed(QString _loginId,dataTypes::proxySettings _ps );
                void reportFail(dataTypes::logSmsMessage _msg,QString _error);
            signals:
                void reportSuccess();
                void reportFailed();

        };



        class contactManagerAdd:public QThread{
            Q_OBJECT
            private:

                QString loginId;
                dataTypes::proxySettings ps;
                void run();
                libJackSMS::dataTypes::contact contatto;
            public:
                contactManagerAdd(const QString & _loginId,dataTypes::proxySettings _ps );
                void addNewContact(libJackSMS::dataTypes::contact _contatto);
            signals:
                void contactAdded(QString,bool);
                void errorAdd();


        };
        class contactManagerUpdate:public QThread{
            Q_OBJECT
            private:

                QString loginId;
                dataTypes::proxySettings ps;
                void run();
                libJackSMS::dataTypes::contact contatto;
            public:
                contactManagerUpdate(const QString & _loginId,dataTypes::proxySettings _ps );
                void updateContact(libJackSMS::dataTypes::contact _contatto);
            signals:
                void contactUpdated(libJackSMS::dataTypes::contact);
                void errorUpdate();


        };
        class contactManagerDelete:public QThread{
            Q_OBJECT
            private:
                QString loginId;
                dataTypes::proxySettings ps;
                QString id;
                void run();
            public:
                contactManagerDelete(const QString & _loginId,dataTypes::proxySettings _ps );
                void deleteContact(QString _id);
            signals:
                void contactDeleted(QString);
                void errorDelete();


        };
        class contactManager:public QObject{
            Q_OBJECT
            private:

                QString loginId;

                dataTypes::proxySettings ps;
                contactManagerAdd* manAdd;
                contactManagerDelete *manDel;
                contactManagerUpdate * manUp;
            public:

                contactManager(const QString & _loginId,dataTypes::proxySettings _ps );
                void addNewContact(libJackSMS::dataTypes::contact _contatto);
                void updateContact(libJackSMS::dataTypes::contact _contatto);
                void deleteContact(QString _id);

            signals:
                void contactDeleted(QString);
                void contactNotDeleted();
                void contactUpdated(libJackSMS::dataTypes::contact);
                void contactNotUpdated();
                void contactSaved(QString,bool);
                void contactNotSaved();



        };


        class accountManagerUpdate:public QThread{
            Q_OBJECT
            private:

                QString loginId;
                libJackSMS::dataTypes::service s;
                dataTypes::proxySettings ps;
                void run();
                libJackSMS::dataTypes::configuredAccount account;

            public:
                accountManagerUpdate(const QString & _loginId,libJackSMS::dataTypes::service _s,dataTypes::proxySettings _ps );
                void updateAccount(libJackSMS::dataTypes::configuredAccount _account);
            signals:
                void accountUpdated(libJackSMS::dataTypes::configuredAccount);
                void errorUpdate();

        };
        class accountManagerDelete:public QThread{
            Q_OBJECT
            private:
                QString loginId;
                dataTypes::proxySettings ps;
                QString id;
                void run();
            public:
                accountManagerDelete(const QString & _loginId,dataTypes::proxySettings _ps );
                void deleteAccount(const QString &_id);
            signals:
                void accountDeleted(QString);
                void errorDelete();


        };
        class accountManagerAdd:public QThread{
            Q_OBJECT
            private:

                QString loginId;
                dataTypes::proxySettings ps;
                void run();
                libJackSMS::dataTypes::service service;
                libJackSMS::dataTypes::configuredAccount account;
            public:
                accountManagerAdd(const QString & _loginId,dataTypes::proxySettings _ps );
                void addNewAccount(libJackSMS::dataTypes::service _service,libJackSMS::dataTypes::configuredAccount & _account);
            signals:
                void accountAdded(QString);
                void errorAdd();


        };
        class accountManager:public QObject{
            Q_OBJECT
            private:
                QString loginId;
                dataTypes::proxySettings ps;
                accountManagerDelete *manDel;
                accountManagerAdd *manAdd;
                accountManagerUpdate *manUp;
            public:
                accountManager(const QString & _loginId,dataTypes::proxySettings _ps );
                void addNewAccount(libJackSMS::dataTypes::service _service,libJackSMS::dataTypes::configuredAccount & _account);
                void updateAccount(libJackSMS::dataTypes::configuredAccount & _account,libJackSMS::dataTypes::service s);
                void deleteAccount(QString _id);

            signals:
                void accountDeleted(QString);
                void accountNotDeleted();
                void accountUpdated(libJackSMS::dataTypes::configuredAccount);
                void accountNotUpdated();
                void accountSaved(QString);
                void accountNotSaved();

        };


        class conversationManager{
            private:
                xmlParserApi::xmlParserServerApiGeneric *xmlDocument;
                QString loginId;
                netClient::netClientGeneric *webClient;
                dataTypes::proxySettings ps;
            public:
                /*!
                   \param _loginId id della sessione corrente
                   \param _ps oggetto di tipo dataTypes::proxySettings che mantiene le impostazioni del proxy

                  */
                conversationManager(const QString & _loginId,dataTypes::proxySettings _ps );
                /*!
                   \param _logSms Struttura dati dove verranno salvati gli sms inviati( JMS compresi)
                   \param _logIm Struttura dati dove verranno salvati i JMS ricevuti
                   \return true se il parsing ha avuto successo, false altrimenti

                   Riceve dal server gli ultimi 100 messaggi inviati e ricevuti

                  */
                bool downloadLastMessages(libJackSMS::dataTypes::logSmsType & _logSms,libJackSMS::dataTypes::logImType & _logIm);

        };

        class updateServicesManager:public QThread{
            Q_OBJECT
            private:

                QString loginId;
                dataTypes::proxySettings ps;
                libJackSMS::dataTypes::servicesType servizi;
                void run();
                bool aborted;
            public:
                updateServicesManager(const QString & _loginId,dataTypes::proxySettings _ps,libJackSMS::dataTypes::servicesType _servizi);
                void checkUpdates();
                void abort();
            signals:
                void abortSignal();
                void criticalError(QString);
                void error(QString);
                void updatesAvailable(libJackSMS::dataTypes::servicesType _servizi,QString xml,QString message);


        };
        class updateServicesManagerBase:public QObject{
            Q_OBJECT
            private:
                xmlParserApi::xmlParserServerApiGeneric *xmlDocument;
                QString loginId;
                netClient::netClientGeneric *webClient;
                QList<QPair<QString,QString> > updateResults;
                QString servXml;
                dataTypes::proxySettings ps;
                bool aborted;
            public slots:
                void abort();
            public:

                updateServicesManagerBase(const QString & _loginId,dataTypes::proxySettings _ps );
                bool downloadUpdates(libJackSMS::dataTypes::servicesType _servizi);
                QString getMessage() const;
                QString getXml() const;


        };


        class reloader{
            private:
                xmlParserApi::xmlParserServerApiGeneric *xmlDocument;
                QString loginId;
                netClient::netClientGeneric *webClient;
                dataTypes::proxySettings ps;
            public:
                /*!
                   \param _loginId id della sessione corrente
                   \param _ps oggetto di tipo dataTypes::proxySettings che mantiene le impostazioni del proxy

                  */
                reloader(const QString & _loginId,dataTypes::proxySettings _ps );
                bool reloadPhoneBook(libJackSMS::dataTypes::phoneBookType & _rubrica);
        };

        class permanentInstantMessenger:public QObject
        {
            Q_OBJECT
        private:


            QString username;
            QString password;
            QString error;
            QString  userDir;
            QTimer pingTimer;
            QTimer signalCountdown;
            QTimer pingTimeout;
            QTimer reconnectTimer;
            QTimer dataTimeout;
            QTcpSocket sock;
            QNetworkProxy proxy;
            dataTypes::proxySettings ps;
            libJackSMS::dataTypes::logImType imLog;
            int id;
            QByteArray buffer;
        public:

            permanentInstantMessenger(QString _username,QString _password,dataTypes::proxySettings _ps );
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
            void serviceNotActive(bool err=false,QString errStr="");
            void serviceActiving();

    };

        class Streamer:public QObject
        {
            Q_OBJECT
        private:


            QString username;
            QString password;
            QString error;
            QString  userDir;
            QTimer pingTimer;
            QTimer signalCountdown;
            QTimer pingTimeout;
            QTimer reconnectTimer;
            QTimer dataTimeout;
            QTcpSocket sock;
            QNetworkProxy proxy;
            dataTypes::proxySettings ps;
            libJackSMS::dataTypes::logImType imLog;
            int id;
            QByteArray buffer;
            QString loginString;
            QStringList idList;

            enum streamerState{waitConnResponse=1,receivingQueue=2,queueProcessed=3,waitForMessages=4};
            //waitformessages e' solo un nome mnemonico.. 3 e 4 indicano la stessa cosa.
            streamerState status;
            int queueCount;
        public:

            Streamer(QString _username,QString _password,QString _loginString,dataTypes::proxySettings _ps );
            ~Streamer();
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
            void serviceNotActive(bool err=false,QString errStr="");
            void serviceActiving();

        };
    }
}
#endif //SERVERAPI_H
