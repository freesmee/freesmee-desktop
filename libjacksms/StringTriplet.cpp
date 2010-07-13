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

#include "StringTriplet.h"
#include <QString>
namespace libJackSMS{

    namespace dataTypes{


            /*********definitions for stringTriplet****************/
            stringTriplet::stringTriplet(const QString &_first,const QString &_second,const QString &_third):one(_first),two(_second),three(_third){
            }
            QString stringTriplet::getFirst() const{
                return one;
            }
            QString stringTriplet::getSecond() const{
                return two;
            }
            void stringTriplet::setSecond(const QString &_str){
                two=_str;
            }
            QString stringTriplet::getThird() const{
                return three;
            }


    }

}
