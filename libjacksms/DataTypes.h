/*
    Copyright (C) <2011>

    <enrico bacis> <enrico.bacis@gmail.com>
    <ivan vaccari> <grisson@jacksms.it>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    You can't modify the adv system, to cheat it.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef DATATYPES_HH
#define DATATYPES_HH 1

#include <QMap>
#include <QList>

#include "PhoneNumber.h"
#include "DateTime.h"
#include "ConfiguredAccount.h"
#include "Service.h"
#include "LogImMessage.h"
#include "LogSmsMessage.h"
#include "Contact.h"
#include "Content.h"

namespace libJackSMS
{
    namespace dataTypes
    {
        typedef QList<content> contentType;
        typedef QMap<QString,QString> creditsType;
        typedef QMap<QString,QString> variousType;

        typedef QMap<QString,contact> phoneBookType;
        typedef QMap<QString,logSmsMessage> logSmsType;
        typedef QMap<QString,logImMessage> logImType;
        typedef QMap<QString,configuredAccount> configuredServicesType;
        typedef QMap<QString,QString> optionsType;
        typedef QMap<QString,service> servicesType;
    }
}

#endif //DATATYPES_HH
