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


#include "ProxyConfig.h"
#include <QString>

namespace libJackSMS {

    namespace dataTypes {

        proxySettings::proxySettings() :
                usaproxy(false),
                autenticati(false)
        {
        }

        proxySettings::proxySettings(const dataTypes::optionsType &_opzioni) :
                usaproxy(false),
                autenticati(false)
        {
            usaproxy = false;

            if (_opzioni["use-proxy"] == "yes") {

                usaproxy = true;
                dataTypes::optionsType::const_iterator iter;

                iter = _opzioni.find("proxy");
                if (iter != _opzioni.end())
                    server = iter.value();

                iter = _opzioni.find("proxy-port");
                if (iter != _opzioni.end())
                    port = iter.value();

                iter = _opzioni.find("proxy-type");
                if (iter != _opzioni.end())
                    type = iter.value();

                iter = _opzioni.find("proxy-username");
                if (iter != _opzioni.end())
                    username = iter.value();

                iter = _opzioni.find("proxy-password");
                if (iter != _opzioni.end())
                    password = iter.value();

                if (_opzioni["use-proxy-auth"] == "yes")
                    autenticati = true;
            }
        }

        QString proxySettings::getServer() const {
            return server;
        }

        QString proxySettings::getPort() const {
            return port;
        }

        QString proxySettings::getType() const {
            return type;
        }

        QString proxySettings::getUsername() const {
            return username;
        }

        QString proxySettings::getPassword() const {
            return password;
        }

        bool proxySettings::useProxy() const {
            return usaproxy;
        }

        bool proxySettings::useAuth() const {
            return autenticati;
        }
    }

}
