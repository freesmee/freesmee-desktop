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
#ifndef VARIABILESERVIZIO_HH
#define VARIABILESERVIZIO_HH 1



namespace libJackSMS{

    namespace dataTypes{


        class variabileServizio{
            private:
                QString nome;
                QString descrizione;
                QString valoredefault;
                QString progressivo;
                bool required;
                bool vuota;
            public:
                variabileServizio();
                variabileServizio(const QString &_name,const QString &_descr,const QString &_default,const QString & _progressivo,bool _required);
                QString getName() const;
                QString getDescription() const;
                QString getDefaultValue() const;
                QString getProgressive() const;
                bool isRequired() const;
                bool isEmpty() const;
        };


    }

}
#endif //VARIABILESERVIZIO_HH
