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

#include "DateTime.h"
#include <QList>
#include <ctime>

namespace libJackSMS{

    namespace dataTypes{
            dateTime::dateTime(){
            }
            dateTime::dateTime(const QString &  _inputDate,const QString &  _inputTime){

                QString _input=_inputDate+" "+_inputTime;
                data=QDateTime::fromString(_input,"dd/MM/yyyy HH:mm:ss");
                if (!data.isValid())
                    data=QDateTime::fromString(_input,"dd/MM/yy HH:mm:ss");
                if (!data.isValid())
                    data=QDateTime::fromString(_input,"dd/MM/yy HH:mm");
                if (!data.isValid())
                    data=QDateTime::fromString(_input,"dd/MM/yyyy HH:mm");
                if (!data.isValid())
                    data=QDateTime::fromString(_input,"yyyy-MM-dd HH:mm:ss");
                if (!data.isValid())
                    data=QDateTime::fromString(_input,"yyyy-MM-dd HH:mm");
                if (!data.isValid())
                    data=QDateTime::fromString(_input,"yy-MM-dd HH:mm");
                if (!data.isValid())
                    data=QDateTime::fromString(_input,"yy-MM-dd HH:mm::ss");



            }
            dateTime::dateTime(const QString & _inputDate){
                data=QDateTime::fromString(_inputDate,"dd/MM/yyyy HH:mm:ss");
                if (!data.isValid())
                    data=QDateTime::fromString(_inputDate,"dd/MM/yy HH:mm:ss");
                if (!data.isValid())
                    data=QDateTime::fromString(_inputDate,"dd/MM/yy HH:mm");
                if (!data.isValid())
                    data=QDateTime::fromString(_inputDate,"dd/MM/yyyy HH:mm");
                if (!data.isValid())
                    data=QDateTime::fromString(_inputDate,"yyyy-MM-dd HH:mm:ss");
                if (!data.isValid())
                    data=QDateTime::fromString(_inputDate,"yyyy-MM-dd HH:mm");
                if (!data.isValid())
                    data=QDateTime::fromString(_inputDate,"yy-MM-dd HH:mm");
                if (!data.isValid())
                    data=QDateTime::fromString(_inputDate,"yy-MM-dd HH:mm::ss");

            }
            /*int dateTime::getDay(){return giorno;}
            int dateTime::getYear(){return anno;}
            int dateTime::getMonth(){return mese;}
            int dateTime::getMinutes(){return minuti;}
            int dateTime::getHour(){return ora;}
            int dateTime::getSecond(){return secondi;}*/
            QString dateTime::getCurrentDay(){
                return QDateTime::currentDateTime().toString("dd/MM/yyyy");
            }
            QString dateTime::getCurrentHour(){
                return QDateTime::currentDateTime().toString("HH:mm:ss");
            }
            QString dateTime::getStringDay()const{
                return data.toString("dd/MM/yyyy");
            }
            QString dateTime::getStringHour()const{
                return data.toString("HH:mm:ss");
            }
            QDateTime dateTime::getData() const{
                return data;
            }
    }

}
