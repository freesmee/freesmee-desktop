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
#ifndef SERVERAPI_H
#define SERVERAPI_H 1




namespace libJackSMS{

    namespace serverApi{
        /*! \brief login: Fornisce un'interfaccia per effettuare l'accesso al server di jacksms.it
          \ingroup serverApi
          Questa classe implementa le api per l'accesso al server di jacksms.it
          Esegue l'accesso e fornisce l'identificatore univoco dell'accesso.
          Permette inoltre di scaricare dal server la lista dei contatti e dei servizi configurati.
          */


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
                /*! login
                   \param _username Il nome utente
                   \param _username La password utente
                   \param _ps oggetto di tipo dataTypes::proxySettings che mantiene le impostazioni del proxy

                   Inizializza l'oggetto "login"
                  */
                login(const QString &_username,const QString &_password,dataTypes::proxySettings _ps );

                /*! doLogin
                   \return true in caso di login avvenuto con successo, false altrimenti

                   Esegue il login sul server di jacksms.it
                  */
                bool doLogin();

                /*! getLoginError
                   \return Stringa contenente la descrizione dell'errore.

                   Ritorna l'errore avvenuto durante il login. Se nessun errore è avvenuto, viene ritornata una stringa vuota.
                  */
                QString getLoginError() const;

                /*! getSessionId
                   \return Stringa contenente il codice della sessione di login.

                   Ritorna l'id della sessione di login attualmente in corso.
                  */
                QString getSessionId();

                /*! retreivePhoneBook
                   \param _rubrica oggetto di tipo dataTypes::phoneBookType dove verranno salvati i contatti ricevuti durante la fase di login
                   \return true se la fase di parsing ha avuto successo, false altrimenti.

                   Preleva i dati ricevuti durante la fase di login sui contatti registrati, e li salva nella struttura dati _rubrica.
                  */
                bool retreivePhoneBook(dataTypes::phoneBookType & _rubrica);
                /*! retreiveAccounts
                   \param _rubrica oggetto di tipo dataTypes::configuredServicesType dove verranno salvate le configurazioni dei servizi registrati sul server e associati a questo account
                   \return true se la fase di parsing ha avuto successo, false altrimenti.

                   Preleva i dati ricevuti durante la fase di login sui servizi configurati, e li salva nella struttura dati _serviziConfigurati.
                  */
                bool retreiveAccounts(dataTypes::configuredServicesType & _serviziConfigurati);
                /*! getVersion

                   \return Un oggetto di tipo libJackSMS::dataTypesApi::clientVersion che contiene informazioni sull'ultima versione del software.

                   Preleva dai dati processati del login la versione del software registrata sul server. Utilizzata poi per confrontare e segnalare gli aggiornamenti del client.
                  */
                libJackSMS::dataTypesApi::clientVersion getVersion() const;

        };
        /*! \brief pingator: fornisce l'implementazione delle api per pingare il server e non far scadere la sessione.
          \ingroup serverApi

          */
        class pingator{
            private:
                QString idsessione;
                xmlParserApi::xmlParserServerApiGeneric *xmlResponse;
                netClient::netClientGeneric *webClient;
                QString error;
                dataTypes::proxySettings ps;
            public:
                /*! pingator

                   \param _idsessione id della sessione corrente
                   \param _ps oggetto di tipo dataTypes::proxySettings che mantiene le impostazioni del proxy

                  */
                pingator(const QString &_idsessione,dataTypes::proxySettings _ps );
                /*! ping

                   \return true se il ping ha avuto successo, false altrimenti.

                   Esegue il ping sul server

                  */
                bool ping();
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
        class smsLogSaver{
            private:
                QString loginId;
                xmlParserApi::xmlParserServerApiGeneric *xmlResponse;
                netClient::netClientGeneric *webClient;
                dataTypes::logSmsMessage msg;
                dataTypes::proxySettings ps;
            public:
                /*! smsLogSaver

                   \param _loginId id della sessione corrente
                   \param _ps oggetto di tipo dataTypes::proxySettings che mantiene le impostazioni del proxy

                  */
                smsLogSaver(const QString _loginId,dataTypes::proxySettings _ps );
                /*! setMessage

                   \param _msg Il messaggio da salvare.

                   imposta il messaggio da salvare

                  */
                void setMessage(const dataTypes::logSmsMessage &_msg);
                /*! save

                   \param _id La variabile dove verrà memorizzato l'id con cui il messaggio è stato salvato sul server.

                   \return true se il messaggio è stato salvato, false altrimenti

                   avvia la procedura per salvare il messaggio sul server

                  */
                bool save(QString &_id);
        };

        /*! \brief Implementa le api per salvare un sms sul server di jacksms.it
          \ingroup serverApi

          */
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
