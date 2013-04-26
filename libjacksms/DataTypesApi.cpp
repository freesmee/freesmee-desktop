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

#include "DataTypesApi.h"
#include <QString>
#include <QRegExp>

namespace libJackSMS {

    namespace dataTypesApi {

        clientVersion::clientVersion(QString version) :
                client_major(0),
                client_minor(0),
                client_release('a'),
                client_build(0)
        {
            if (!version.isEmpty()) {
                QRegExp regExp;
                bool ok;

                regExp.setPattern(QString("^([0-9]{1,})\\.([0-9]{1,})\\-([abg]{1})([0-9]{1,})$"));
                if (regExp.exactMatch(version)) {

                    client_major = regExp.cap(1).toInt(&ok,10);
                    client_minor = regExp.cap(2).toInt(&ok,10);
                    client_release = regExp.cap(3).toStdString()[0];
                    client_build = regExp.cap(4).toInt(&ok,10);

                } else {

                    regExp.setPattern(QString("^([0-9]{1,})\\.([0-9]{1,})$"));
                    if (regExp.exactMatch(version)) {
                        client_major = regExp.cap(1).toInt(&ok,10);
                        client_minor = regExp.cap(2).toInt(&ok,10);
                        client_release = 'g';
                        client_build = 0;
                    }

                }
            }
        }

        bool clientVersion::operator< (const clientVersion &client)
        {

            if (client_major < client.client_major)
                return true;
            else if (client_major > client.client_major)
                return false;

            if (client_minor < client.client_minor)
                return true;
            else if (client_minor > client.client_minor)
                return false;

            if (client_build < client.client_build)
                return true;
            else if (client_build > client.client_build)
                return false;

            return false;
        }

    }

}
