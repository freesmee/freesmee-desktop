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

#ifndef PROXYCONFIG_H
#define PROXYCONFIG_H

#include <QString>
#include "DataTypes.h"

namespace libJackSMS
{
    namespace dataTypes
    {
        /**
            Estrae dalle opzioni i dati necessari a configurare il proxy e crea una struttura ad hoc

        */

        class proxySettings{
        private:
            QString server;
            QString port;
            QString type;
            QString username;
            QString password;
            bool usaproxy;
            bool autenticati;
        public:
            proxySettings();
            proxySettings(const dataTypes::optionsType &_opzioni);
            QString getServer() const;
            QString getPort() const;
            QString getType() const;
            QString getUsername() const;
            QString getPassword() const;
            bool useProxy() const;
            bool useAuth() const;
        };

    }


}
#endif // PROXYCONFIG_H
