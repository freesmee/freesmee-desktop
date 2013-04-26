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

#ifndef LOGIMMESSAGE_HH
#define LOGIMMESSAGE_HH 1

#include "PhoneNumber.h"
#include "DateTime.h"
#include <QString>

namespace libJackSMS
{
    namespace dataTypes
    {
        class logImMessage
        {
            private:
                phoneNumber mittente;
                dateTime data;
                QString id;
                QString testo;
            public:
                logImMessage();
                logImMessage(const phoneNumber &_mittente,const dateTime &_data,const QString &_id,const QString &_testo);
                QString getId() const ;
                void setId(const QString &_id) ;
                const phoneNumber & getPhoneNumber() const;
                QString getText() const;
                dateTime getDate() const;

        };

    }

}

#endif //LOGIMMESSAGE_HH
