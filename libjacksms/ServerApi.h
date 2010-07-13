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

#include <QString>
#include "xmlParserServerApi.h"
#include "NetClient.h"
#include "DataTypes.h"
#include "ProxyConfig.h"
#include "SocketClient.h"
#include <QObject>
#ifndef SERVERAPI_H
#define SERVERAPI_H 1




namespace libJackSMS{

    namespace serverApi{
        class login{
            private:
                QString username;
                QString password;
                QString idsessione;
                xmlParserApi::xmlParserServerApiGeneric *xmlResponse;
                netClient::netClientGeneric *webClient;
                QString error;
                dataTypes::proxySettings ps;
            public:
                login(const QString &_username,const QString &_password,dataTypes::proxySettings _ps );
                bool doLogin();
                QString getLoginError() const;
                QString getSessionId();
                bool retreivePhoneBook(dataTypes::phoneBookType & _rubrica);
                bool retreiveAccounts(dataTypes::configuredServicesType & _serviziConfigurati);
                libJackSMS::dataTypesApi::clientVersion getVersion() const;

        };
        class pingator{
            private:
                QString idsessione;
                xmlParserApi::xmlParserServerApiGeneric *xmlResponse;
                netClient::netClientGeneric *webClient;
                QString error;
                dataTypes::proxySettings ps;
            public:
                pingator(const QString &_idsessione,dataTypes::proxySettings _ps );
                bool ping();
        };
        bool synchronizeVariables(dataTypes::configuredServicesType & _serviziConfigurati,const dataTypes::servicesType & _servizi);

        class instantMessenger{
            private:
                QString loginId;
                libJackSMS::dataTypes::logImType messages;
                dataTypes::proxySettings ps;
                netClient::netClientGeneric *webClient;
            public:
                instantMessenger(const QString & _loginId,dataTypes::proxySettings _ps );
                bool checkMessages();
                bool getMessages(libJackSMS::dataTypes::logImType &_messages);
        };

        class permanentJMessenger:public QObject{
            Q_OBJECT
            private:
                QString username;
                QString password;
                libJackSMS::dataTypes::logImType messages;
                dataTypes::proxySettings ps;
                netClient::SocketClient *sock;

            public:
                ~permanentJMessenger();
                permanentJMessenger(const QString & _username,const QString & _password,dataTypes::proxySettings _ps);
                bool activatePermanentCheck();
                void deActivatePermanentCheck();
            signals:
                void newJms(libJackSMS::dataTypes::logImMessage jms);

        };
        class smsLogSaver{
            private:
                QString loginId;
                xmlParserApi::xmlParserServerApiGeneric *xmlResponse;
                netClient::netClientGeneric *webClient;
                dataTypes::logSmsMessage msg;
                dataTypes::proxySettings ps;
            public:
                smsLogSaver(const QString _loginId,dataTypes::proxySettings _ps );
                void setMessage(const dataTypes::logSmsMessage &_msg);
                bool save(QString &_id);
        };

        class contactManager{
            private:
                xmlParserApi::xmlParserServerApiGeneric *xmlDocument;
                QString loginId;
                netClient::netClientGeneric *webClient;
                QString resultId;
                dataTypes::proxySettings ps;
            public:
                contactManager(const QString & _loginId,dataTypes::proxySettings _ps );
                bool addNewContact(libJackSMS::dataTypes::contact & _contatto);
                bool updateContact(libJackSMS::dataTypes::contact & _contatto);
                bool deleteContact(const QString &_id);
                QString getResultId() const;
        };

        class accountManager{
            private:
                xmlParserApi::xmlParserServerApiGeneric *xmlDocument;
                QString loginId;
                netClient::netClientGeneric *webClient;
                QString resultId;
                dataTypes::proxySettings ps;
            public:
                accountManager(const QString & _loginId,dataTypes::proxySettings _ps );
                bool addNewAccount(libJackSMS::dataTypes::service _service,libJackSMS::dataTypes::configuredAccount & _account);
                bool updateAccount(libJackSMS::dataTypes::configuredAccount & _account);
                bool deleteAccount(const QString &_id);
                QString getResultId() const;
        };
        class conversationManager{
            private:
                xmlParserApi::xmlParserServerApiGeneric *xmlDocument;
                QString loginId;
                netClient::netClientGeneric *webClient;
                dataTypes::proxySettings ps;
            public:
                conversationManager(const QString & _loginId,dataTypes::proxySettings _ps );
                bool downloadLastMessages(libJackSMS::dataTypes::logSmsType & _logSms,libJackSMS::dataTypes::logImType & _logIm);

        };
        class updateServicesManager{
            private:
                xmlParserApi::xmlParserServerApiGeneric *xmlDocument;
                QString loginId;
                netClient::netClientGeneric *webClient;
                QList<QPair<QString,QString> > updateResults;
                QString servXml;
                dataTypes::proxySettings ps;
            public:
                updateServicesManager(const QString & _loginId,dataTypes::proxySettings _ps );
                bool downloadUpdates(libJackSMS::dataTypes::servicesType & _servizi);
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
                reloader(const QString & _loginId,dataTypes::proxySettings _ps );
                bool reloadPhoneBook(libJackSMS::dataTypes::phoneBookType & _rubrica);
        };
    }
}
#endif //SERVERAPI_H
