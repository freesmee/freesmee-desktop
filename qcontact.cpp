/*
    Copyright (C) <2011>

    <enrico bacis> <enrico.bacis@gmail.com>
    <ivan vaccari> <grisson@jacksms.it>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    You can't modify the adv system, to cheat it.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include <QString>
#include "qcontact.h"

QContact::QContact(const QString & _nome,const libJackSMS::dataTypes::phoneNumber & _telefono,const QString & _gruppo, const QString & _account):nome(_nome),telefono(_telefono),gruppo(_gruppo),account(_account){}

QString QContact::getName() const{
    return nome;
}
QString QContact::getGroup()const{
    return gruppo;
}
QString QContact::getAccount()const{
    return account;
}
libJackSMS::dataTypes::phoneNumber QContact::getPhone()const{
    return telefono;
}
void QContact::setId(const QString & _id){
    id=_id;
}
QString QContact::getId()const{
    return id;
}
void QContact::setName(const QString & _name){
    nome=_name;
}
void QContact::setAccount(QString _account){
    account=_account;
}
