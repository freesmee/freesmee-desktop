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

#ifndef QCONTACT_H
#define QCONTACT_H

#include <QString>
#include <libjacksms/libJackSMS.h>

class QContact
{

private:
    QString nome;
    libJackSMS::dataTypes::phoneNumber telefono;
    QString gruppo;
    QString account;
    QString id;
public:
    QContact(const QString & _nome,const libJackSMS::dataTypes::phoneNumber & _telefono,const QString & _gruppo, const QString & _account);
    QString getName()const;
    QString getGroup()const;
    QString getAccount()const;
    libJackSMS::dataTypes::phoneNumber getPhone()const;
    void setId(const QString & _id);
    void setName(const QString & _name);
    void setAccount(QString _account);
    QString getId()const;
};

#endif // QCONTACT_H
