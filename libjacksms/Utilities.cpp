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

#include "Utilities.h"
#include <QDir>
#include <QFile>

namespace libJackSMS {

    namespace utilities {

        QString Base64Encode(const QString &toEncode)
        {
            QByteArray data = toEncode.toUtf8();
            return QString(data.toBase64());
        }

        QString Base64Decode(const QString &toDecode)
        {
            return QString(QByteArray::fromBase64(toDecode.toUtf8()));
        }

        QByteArray Base64DecodeByte(const QString &toDecode)
        {
            return QByteArray::fromBase64(toDecode.toUtf8());
        }

        void deleteDirectory(QString dirPath, bool deleteParent)
        {
            QDir dir(dirPath);

            //First delete any files in the current directory
            QFileInfoList files = dir.entryInfoList(QDir::NoDotAndDotDot | QDir::Files);
            for(int file = 0; file < files.count(); ++file)
                dir.remove(files.at(file).fileName());

            //Now recursively delete any child directories
            QFileInfoList dirs = dir.entryInfoList(QDir::NoDotAndDotDot | QDir::Dirs);
            for(int subdir = 0; subdir < dirs.count(); ++subdir)
                deleteDirectory(dirs.at(subdir).absoluteFilePath(), true);

            //Finally, remove empty parent directory
            if(deleteParent)
                dir.rmdir(dir.path());
        }

    } //end namespace utilities
} //end namespace libJackSMS
