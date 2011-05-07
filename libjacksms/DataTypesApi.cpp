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

#include "DataTypesApi.h"
#include <QString>
#include <QRegExp>

namespace libJackSMS {

    namespace dataTypesApi {

        clientVersion::clientVersion(QString version) :
                major(0),
                minor(0),
                release('a'),
                build(0)
        {
            if (!version.isEmpty()) {
                QRegExp regExp;
                bool ok;

                regExp.setPattern(QString("^([0-9]{1,})\\.([0-9]{1,})\\-([abg]{1})([0-9]{1,})$"));
                if (regExp.exactMatch(version)) {

                    major = regExp.cap(1).toInt(&ok,10);
                    minor = regExp.cap(2).toInt(&ok,10);
                    release = regExp.cap(3).toStdString()[0];
                    build = regExp.cap(4).toInt(&ok,10);

                } else {

                    regExp.setPattern(QString("^([0-9]{1,})\\.([0-9]{1,})$"));
                    if (regExp.exactMatch(version)) {
                        major = regExp.cap(1).toInt(&ok,10);
                        minor = regExp.cap(2).toInt(&ok,10);
                    }

                }
            }
        }

        bool clientVersion::operator< (const clientVersion &client)
        {

            if (major < client.major)
                return true;
            else if (major > client.major)
                return false;

            if (minor < client.minor)
                return true;
            else if (minor > client.minor)
                return false;

            if (build < client.build)
                return true;
            else if (build > client.build)
                return false;

            if (release == 'a' && client.release == 'b')
                return true;
            else if (release == 'b' && client.release == 'g')
                return true;
            else
                return false;
        }

    }

}
