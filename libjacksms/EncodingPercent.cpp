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

#include "EncodingPercent.h"
#include <QString>
#include <QUrl>
namespace libJackSMS{

    encodingPercent::encodingPercent(){
    }
    encodingPercent::~encodingPercent(){
    }

    /*the input string _text is supposed to be in utf*/
    QString encodingPercent::getEncodedString(QString _text){
         return QString(QUrl::toPercentEncoding(_text));


    }
}
