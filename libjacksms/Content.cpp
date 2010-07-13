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

#include <string>
#include "Content.h"

#ifndef DATATYPES_HH
#define DATATYPES_HH 1



namespace libJackSMS{

    namespace dataTypes{

        content::content():trovato(false){
        }
        void content::setName(const QString & _name){
            nome=_name;
        }
        QString content::getName() const{
            return nome;
        }

        QString content::getValue() const{
            return valore;
        }

        bool content::getFound() const{
            return trovato;
        }


        void content::setValue(const QString & _val){
            valore=_val;
        }
        void content::setUnEncodedValue(const QString & _val){
            unEncodedValue=_val;
        }
        QString content::getUnEncodedValue()const {
            return unEncodedValue;
        }
        void content::setFound(bool _found){
            trovato=_found;
        }


    }

}
#endif //DATATYPES_HH
