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
  \file SmsSender.h
  \brief Contiene la definizione della classe da utilizzare per inviare un messaggio.
  \ingroup smsSender
 */

#ifndef SMSSENDER_HH
#define SMSSENDER_HH 1

#include <QThread>
#include <QString>
#include <QSemaphore>
#include <QMutex>
#include <QWaitCondition>
#include "DataTypes.h"
#include "ShortMessage.h"
#include "Encoding.h"
#include "NetClient.h"
#include "Logger.h"
#include <QTimer>
#include "libjacksms/json/json.h"
#include "Configuration.h"


namespace libJackSMS
{
    class smsSender : public QThread
    {
        Q_OBJECT

        private:
            dataTypes::phoneNumber destinatario;
            dataTypes::shortMessage messaggio;
            const QString loginId;
            const dataTypes::servicesType &servizi;
            dataTypes::configuredAccount account;
            void run();
            int pageIndex;
            bool continueSendFlag;
            QString captchaValue;
            dataTypes::contentType contenuti;
            bool SalvaCookies;
            QTimer sleepClockTimer;
            int secondsToGo;

        public:
            smsSender(const QString &_loginId, const dataTypes::servicesType &_services);
            void setRecipient(const dataTypes::phoneNumber &_dest);
            void setMessage(const dataTypes::shortMessage &_message);
            void setAccount(const dataTypes::configuredAccount &_account);
            void send();
            void continueSend(QString captcha_value);
            void abort();
            void setSalvaCookies(bool value);

        private slots:
            void slotError(QString);
            void slotSuccess(QString, int);
            void slotSleepBeforeFound(int);
            void slotTimerClocked();

        signals:
            void abortSignal();
            void operation();
            void operation(QString);
            void error(QString);
            void success(QString, int);
            void captcha(QByteArray);
            void sleepBeforeFound(int);
    };

    class smsSenderBase : public QObject
    {
        Q_OBJECT

    private:
        dataTypes::phoneNumber destinatario;
        dataTypes::shortMessage messaggio;
        QString messagiofinale;
        const QString loginId;
        const dataTypes::servicesType &servizi;
        dataTypes::configuredAccount account;
        QString substitute(QString _input, const dataTypes::contentType &_cont);
        QString substitute(QString _input, const dataTypes::creditsType &_cont);
        netClient::netClientGeneric *webClient;
        int pageIndex;
        bool captchaInterrupt;
        dataTypes::contentType elenco_contenuti;
        bool SalvaCookies;
        QMutex mutex;
        bool hasAborted;
        void managePostProcedureSignals(QString resultString, bool resultError, bool resultSend, bool forceDeleteCookies, bool &isPostprocedurePage);
        bool manageErrorBreak(QString resultString, bool resultError, bool resultSend, bool forceDeleteCookies, bool &isPostprocedurePage, bool hasPostprocedurePage, logger &log);

    public:
        smsSenderBase(const QString &_loginId, const dataTypes::servicesType &_services);
        void setNumberOfFirstPage(int _pn);
        void setRecipient(const dataTypes::phoneNumber &_dest);
        void setMessage(const dataTypes::shortMessage &_message);
        void setAccount(dataTypes::configuredAccount _account);
        void send(QString captcha_value = "");
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
        void success(QString, int);
        void captcha(QByteArray);
        void sleepBeforeFound(int);
    };
}

#endif //SMSSENDER_HH
