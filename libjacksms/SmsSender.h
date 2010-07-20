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
  \file SmsSender.h
  \brief Contiene la definizione della classe da utilizzare per inviare un messaggio.
  \ingroup smsSender
 */
#include <QObject>
#include <QString>
#include "DataTypes.h"
#include "ReportOperationSendSmsNothing.h"
#include "ShortMessage.h"
#include "ProxyConfig.h"
#include "Encoding.h"
#include "NetClient.h"
#ifndef SMSSENDER_HH
#define SMSSENDER_HH 1



namespace libJackSMS{


    /*! \brief fornisce la funzionalità di invio di un messaggio sms
      \ingroup smsSender
      Questa classe implementa tutto ciò che è necessario per inviare un sms.

      */
    class smsSender:public QObject{
        Q_OBJECT
        private:

            dataTypes::phoneNumber destinatario;
            dataTypes::shortMessage messaggio;
            QString messagiofinale;
            libJackSMS::reportOperationSendSms * operazione;
            const dataTypes::servicesType & servizi;
            dataTypes::configuredAccount account;
            QString substitute(QString _input,const dataTypes::contentType &_cont,bool replace_encoded=true);
            QString substitute(QString _input,const dataTypes::creditsType &_cont);
            const dataTypes::proxySettings &ps;
            netClient::netClientGeneric *webClient;
        public:
            /*!
               \param _services la struttura dati contenente la definizione dei servizi
               \param _ps L'oggetto contenente le impostazioni per il proxy
              */
            smsSender(const dataTypes::servicesType & _services, const dataTypes::proxySettings &_ps=dataTypes::proxySettings());
            /*!
               \param _dest numero telefonico del destinatario del messaggio

               Imposta il destinatario del messaggio
              */
            void setRecipient(const dataTypes::phoneNumber & _dest);
            /*!
               \param _message Il messaggio da inviare

               Imposta il messaggio da inviare
              */
            void setMessage(const dataTypes::shortMessage & _message);
            /*!
               \param _account L'account da utilizzare per l'invio del messaggio

               Imposta l'account da utilizzare per l'invio del messaggio
              */
            void setAccount(dataTypes::configuredAccount _account);
            /*!
               \param _op La classe che gestisce le operazioni da eseguire

               Imposta le operazioni sincrone utilizzate dalla classe per notificare gli eventi che accadono durante l'invio
              */
            void setReportOperation(libJackSMS::reportOperationSendSms * _op);
            /*!

               Avvia l'operazione di invio del messaggio sms.
              */
            void send();
        private slots:
            /*!

               slot utilizzato per annullare l'invio.
              */
            void abort();

    };


}
#endif //SMSSENDER_HH
