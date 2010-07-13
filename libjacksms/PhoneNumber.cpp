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

#include <QString>
#include "PhoneNumber.h"
#include <QRegExp>

namespace libJackSMS{
    namespace dataTypes{
        phoneNumber::phoneNumber():valid(false),specialNumber(false){
        }

        void
        phoneNumber::parse(QString _phoneNum){

            QRegExp  regExp;
            regExp.setPattern(QString("^\\+([0-9]{1,4})\\.([0-9]{1,4})\\.([0-9]{1,7})$"));
            if (regExp.exactMatch(_phoneNum)){
                intcode=regExp.cap(1);
                intpref="+"+intcode;
                pref=regExp.cap(2);
                num=regExp.cap(3);
                valid=true;
                return;
            }
            regExp.setPattern("^([0-9]{10})$");
            if (regExp.exactMatch(_phoneNum)){
                intcode="39";
                intpref="+39";
                pref=_phoneNum.left(3);
                num=_phoneNum.right(7);
                valid=true;
                return;
            }

            regExp.setPattern("^\\+([0-9]{12})$");
            if (regExp.exactMatch(_phoneNum)){
                intcode=_phoneNum.mid(1,2);
                intpref="+"+intcode;
                pref=_phoneNum.mid(3,3);
                num=_phoneNum.mid(6);
                valid=true;
                return;
            }
            regExp.setPattern("^([0-9]{4,5})$");
            if (regExp.exactMatch(_phoneNum)){
                intcode="";
                intpref="";
                pref="";
                num=_phoneNum;
                valid=true;
                specialNumber=true;
                return;
            }
            valid=false;
            alternativeName=_phoneNum;
        }


        QString
        phoneNumber::getIntCode() const {
                return intcode;
        }

        QString
        phoneNumber::getIntNum() const {
            return pref+num;
        }

        QString
        phoneNumber::getNum() const {
            return num;
        }

        QString
        phoneNumber::getIntPref()const {
            return intpref;
        }

        QString
        phoneNumber::getPref() const {
            return pref;
        }

        QString
        phoneNumber::toString() const {
            if (specialNumber)
                return (valid)?num:QString("");
            else
                return (valid)?intpref+QString(".")+pref+QString(".")+num:QString("");
        }


        void phoneNumber::setAltName(const QString &_name){
            alternativeName=_name;

        }
        QString phoneNumber::getAltName() const{
            return alternativeName;


        }
        void phoneNumber::setIsValid(bool v){
            valid=v;
        }
        bool phoneNumber::getIsValid() const{
            return valid;
        }
    }
}

