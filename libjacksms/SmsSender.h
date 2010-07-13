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
            smsSender(const dataTypes::servicesType & _services, const dataTypes::proxySettings &_ps=dataTypes::proxySettings());
            void setRecipient(const dataTypes::phoneNumber & _dest);
            void setMessage(const dataTypes::shortMessage & _message);
            void setAccount(dataTypes::configuredAccount _account);
            void setReportOperation(libJackSMS::reportOperationSendSms * _op);

            void send();
        private slots:
            void abort();

    };


}
#endif //SMSSENDER_HH
