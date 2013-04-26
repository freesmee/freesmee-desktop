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

#include "VariabileServizio.h"
#include <QString>

namespace libJackSMS{

    namespace dataTypes{

            /****************definitions for variabileservizio**********/
            variabileServizio::variabileServizio():vuota(true){
            }
            variabileServizio::variabileServizio(const QString &_name,const QString &_descr,const QString &_default,const QString & _progressivo,bool _required):nome(_name),descrizione(_descr),valoredefault(_default),progressivo(_progressivo),required(_required),vuota(false){
            }
            QString variabileServizio::getName() const{
                return nome;
            }
            QString variabileServizio::getDescription() const{
                return descrizione;
            }
            QString variabileServizio::getDefaultValue() const{
                return valoredefault;
            }
            QString variabileServizio::getProgressive() const{
                return progressivo;
            }




    }

}
