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

#include "Logger.h"
#include "FilesDirectory.h"
#include <QFile>
#include <QString>

namespace libJackSMS
{
    logger::logger(QString _filename) : filename(_filename)
    {
    }

    void logger::addNotice(const QString &_notice)
    {
        QFile fi(filename);
        if (fi.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
        {
            fi.write(_notice.toUtf8());
            fi.write("\n");
            fi.close();
        }
    }

    void logger::cancelLog()
    {
        QFile fi(filename);
        if (fi.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
            fi.close();
    }
}
