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

#include "LogSmsMessage.h"

namespace libJackSMS{

    namespace dataTypes{


            /*********definitions for logSmsMessage****************/
            logSmsMessage::logSmsMessage(){
            }
            logSmsMessage::logSmsMessage(const phoneNumber &_destinatario,const QString &_account,const QString &_idservizio,const dateTime &_data,const QString &_id,const QString &_testo):destinatario(_destinatario),account(_account),idservizio(_idservizio),data(_data),id(_id),testo(_testo){
            }
            QString logSmsMessage::getId() const {
                return id;
            }
            void logSmsMessage::setId(const QString & _id) {
                id=_id;
            }
            const phoneNumber & logSmsMessage::getPhoneNumber() const{
                return destinatario;
            }
            QString logSmsMessage::getAccount() const{
                return account;
            }
            QString logSmsMessage::getServiceId() const{
                return idservizio;
            }
            QString logSmsMessage::getText() const{
                return testo;
            }
            dateTime logSmsMessage::getDate() const{
                return data;
            }

    }

}
