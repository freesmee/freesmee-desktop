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

#include "PageVariable.h"



namespace libJackSMS{

    namespace dataTypes{


            /****************definitions for pageVariable**********/
        pageVariable::pageVariable(const QString &_name,const QString &_value,const QString &_condition):n(_name),v(_value),c(_condition),encode(false){
            }
            QString pageVariable::getName() const{
                return n;
            }
            QString pageVariable::getValue() const{
                return v;
            }
            QString pageVariable::getCondition() const{
                return c;
            }
            void pageVariable::setToEncode(bool _e){
                encode=_e;
            }
            bool pageVariable::getToEncode()const{
                return encode;
            }


    }

}
