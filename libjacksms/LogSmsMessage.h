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


#include "PhoneNumber.h"
#include "DateTime.h"
#ifndef LOGSMSMESSAGE_HH
#define LOGSMSMESSAGE_HH 1



namespace libJackSMS{

    namespace dataTypes{
        class logSmsMessage{
            private:
                phoneNumber destinatario;
                QString account;
                QString idservizio;
                dateTime data;
                QString id;
                QString testo;
                QString accountId;
            public:
                logSmsMessage();
                logSmsMessage(const phoneNumber &_destinatario,const QString &_account,const QString &_idservizio,const dateTime &_data,const QString &_id,const QString &_testo);
                QString getId() const ;
                void setId(const QString & _id);
                const phoneNumber & getPhoneNumber() const;
                QString getAccount() const;
                QString getAccountId() const;
                void setAccountId(QString _id);
                QString getServiceId() const;
                QString getText() const;
                dateTime getDate() const;

        };
    }

}
#endif //LOGSMSMESSAGE_HH
