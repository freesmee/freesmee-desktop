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
#include <QThread>
#include <QString>
#include "DataTypes.h"
#include <QSemaphore>
#include "ShortMessage.h"
#include "ProxyConfig.h"
#include "Encoding.h"
#include "NetClient.h"
#ifndef SMSSENDER_HH
#define SMSSENDER_HH 1



namespace libJackSMS{

    class smsSender:public QThread{
        Q_OBJECT
        private:
            dataTypes::phoneNumber destinatario;
            dataTypes::shortMessage messaggio;
            const dataTypes::servicesType & servizi;
            dataTypes::configuredAccount account;
            dataTypes::proxySettings ps;
            void run();
            int pageIndex;
            bool continueSendFlag;
            QString captchaValue;
            dataTypes::contentType contenuti;
            bool SalvaCookies;
        public:
            smsSender(const dataTypes::servicesType & _services, const dataTypes::proxySettings &_ps=dataTypes::proxySettings());
            void setRecipient(const dataTypes::phoneNumber & _dest);
            void setMessage(const dataTypes::shortMessage & _message);
            void setAccount(const dataTypes::configuredAccount &_account);
            void send();
            void continueSend(QString captcha_value);
            void abort();
            void setSalvaCookies(bool value);
        private slots:
            void slotOperation();
            void slotOperation(QString);
            void slotError(QString);
            void slotSuccess(QString);
            void slotCaptcha(QByteArray);

        signals:
            void abortSignal();
            void operation();
            void operation(QString);
            void error(QString);
            void success(QString);
            void captcha(QByteArray);



    };


    class smsSenderBase:public QObject{
        Q_OBJECT
        private:

            dataTypes::phoneNumber destinatario;
            dataTypes::shortMessage messaggio;
            QString messagiofinale;
            const dataTypes::servicesType & servizi;
            dataTypes::configuredAccount account;
            QString substitute(QString _input,const dataTypes::contentType &_cont);
            QString substitute(QString _input,const dataTypes::creditsType &_cont);
            const dataTypes::proxySettings &ps;
            netClient::netClientGeneric *webClient;
            int pageIndex;
            bool captchaInterrupt;
            dataTypes::contentType elenco_contenuti;
            bool SalvaCookies;
        public:
            smsSenderBase(const dataTypes::servicesType & _services, const dataTypes::proxySettings &_ps=dataTypes::proxySettings());
            void setNumberOfFirstPage(int _pn);
            void setRecipient(const dataTypes::phoneNumber & _dest);
            void setMessage(const dataTypes::shortMessage & _message);
            void setAccount(dataTypes::configuredAccount _account);
            void send(QString captcha_value="");
            bool isInterruptedByCaptcha()const;
            int getCaptchaPageIndex()const;
            dataTypes::contentType getContents()const;
            void setContents(dataTypes::contentType contents);
            void setSalvaCookies(bool value);
            void optionalDeleteCookies();

        public slots:
            void abort();
        signals:
            void operation();
            void operation(QString);
            void error(QString);
            void success(QString);
            void captcha(QByteArray);



    };


}
#endif //SMSSENDER_HH
