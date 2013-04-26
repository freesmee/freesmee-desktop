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

#ifdef WIN32
    #include <dirent.h>
#else
    #include <sys/stat.h>
    #include <sys/types.h>
#endif

#include <QDir>
#include <cstdlib>
#include <QString>
#include "FilesDirectory.h"

namespace libJackSMS
{
    namespace directories
    {
        QString concatDirectoryAndFile(const QString & _dir, const QString &_file)
        {
            #ifdef WIN32
            QString newPath = _dir + QString("\\") + _file;
            newPath = newPath.replace("\\\\", "\\");

            #else
            QString newPath = _dir + QString("/") + _file;
            newPath = newPath.replace("//", "/");

            #endif

            return newPath;
        }

        QString DataDirectory()
        {
            #ifndef PORTABLE

            #ifdef WIN32
            char* path = getenv("appdata");
            return concatDirectoryAndFile(QString(path), "Freesmee\\");

            #else
            char* path = getenv("HOME");
            return concatDirectoryAndFile(QString(path), ".Freesmee/");
            #endif

            #else

            #ifdef WIN32
            return concatDirectoryAndFile(QDir::currentPath(), "data\\");

            #else
            return concatDirectoryAndFile(QDir::currentPath(), "data/");
            #endif

            #endif
        }

        QString XmlDirectory()
        {
            #ifdef WIN32
            return DataDirectory() + QString("xml\\");
            #else
            return DataDirectory() + QString("xml/");
            #endif
        }

//        QString FilesDirectory()
//        {
//            #ifdef WIN32
//            return QString("files\\");
//            #else
//            return QString("/tmp/JackSMS/");
//            #endif
//        }

        QString ImageDirectory()
        {
            #ifdef WIN32
            return DataDirectory()+QString("img\\");
            #else
            return DataDirectory()+QString("img/");
            #endif
        }

        QString DumpDirectory()
        {
            #ifdef WIN32
            return DataDirectory()+QString("onlinedump\\");
            #else
            return DataDirectory()+QString("onlinedump/");
            #endif
        }

        QString CookiesDirectory()
        {
            #ifdef WIN32
            return DataDirectory()+QString("cookies\\");
            #else
            return DataDirectory()+QString("cookies/");
            #endif
        }
    }
}
