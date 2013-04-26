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

#ifndef SYSTEMDIRECTORY_HH_GUARD
#define SYSTEMDIRECTORY_HH_GUARD 1

#include <QString>

namespace libJackSMS
{
    namespace directories
    {
        QString concatDirectoryAndFile(const QString & _dir,const QString &_file);
        QString DataDirectory();
        QString XmlDirectory();
        QString ImageDirectory();
        QString DumpDirectory();
        QString CookiesDirectory();
        //QString FilesDirectory();
    }

}

#endif //SYSTEMDIRECTORY_HH_GUARD
