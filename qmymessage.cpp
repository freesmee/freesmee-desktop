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

#include "qmymessage.h"

QMyMessage::QMyMessage() :
        type(false),
        letto(true)
{
}

void QMyMessage::setReaded(bool readed) {
    letto = readed;
}

bool QMyMessage::getReaded() const {
    return letto;
}

void QMyMessage::setMessage(const QString &_msg) {
    message = _msg;
}

void QMyMessage::setData (const QDateTime &_date) {
    data = _date;
}

void QMyMessage::setAccountId(const QString &_id) {
    accountId = _id;
}

void QMyMessage::setIsReceived(bool _received) {
    type = _received;
}

void QMyMessage::setId(const QString &_id) {
    id = _id;
}

void QMyMessage::setServiceId(const QString &_id) {
    serviceId=_id;
}

void QMyMessage:: setParsedName(const QString &_pn) {
    pn = _pn;
}

QString QMyMessage::getParsedName() const {
    return pn;
}

void QMyMessage::setPhone(const libJackSMS::dataTypes::phoneNumber &_pn) {
    telefono = _pn;
}

QString QMyMessage::getMessage() const {
    return message;
}

QDateTime QMyMessage::getData() const {
    return data;
}

QString QMyMessage::getAccountId() const {
    return accountId;
}

bool QMyMessage::getIsReceived() const {
    return type;
}

QString QMyMessage::getId() const {
    return id;
}

bool QMyMessage::messageContains(QString text) const {
    return (!(-1 == pn.indexOf(text, 0, Qt::CaseInsensitive))) || (!(-1 == message.indexOf(text, 0, Qt::CaseInsensitive)));
}

QString QMyMessage::getServiceId() const {
    return serviceId;
}

libJackSMS::dataTypes::phoneNumber QMyMessage::getPhone() const {
    return telefono;
}

void QMyMessage::setAccountName(const QString &_an) {
    accountName = _an;
}

QString QMyMessage::getAccountName() const {
    return accountName;
}
