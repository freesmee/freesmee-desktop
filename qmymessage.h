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

#ifndef QMYMESSAGE_H
#define QMYMESSAGE_H

#include <libjacksms/libJackSMS.h>
#include <QDateTime>

class QMyMessage
{
    private:
        QString message;
        QDateTime data;
        QString accountId;
        bool type; //true=received, false=sended
        QString id;
        libJackSMS::dataTypes::phoneNumber telefono;
        QString serviceId;
        bool letto;
        QString accountName;
        QString upperText;
        QString pn;
    public:
        QMyMessage();
        void setMessage(const QString &_msg);
        void setData (const QDateTime &_date);
        void setAccountId(const QString &_id);
        void setIsReceived(bool _received);
        void setId(const QString &_id);
        void setPhone(const libJackSMS::dataTypes::phoneNumber & _pn);
        void setServiceId(const QString &_id);
        void setReaded(bool readed);
        bool getReaded()const;
        bool messageContains(QString text)const;
        QString getMessage()const;
        QDateTime getData()const;
        QString getAccountId()const;
        void setAccountName(const QString & _an);
        QString getAccountName() const;
        void setParsedName(const QString & _pn);
        QString getParsedName() const;
        bool getIsReceived()const;
        QString getId()const;
        libJackSMS::dataTypes::phoneNumber getPhone()const;
        QString getServiceId() const;

};

#endif // QMYMESSAGE_H
