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

#ifndef ENCRYPTER_H
#define ENCRYPTER_H

#include <QString>

namespace libJackSMS
{
    namespace Encrypter
    {
        class Encrypter
        {
        private:
            static QString base(QString _str,int multiplier);
        public:
            Encrypter();

            static QString encrypt(QString _strToEnc);
            static QString decrypt(const QString &_strToDec);
        };
    }
}

#endif // ENCRYPTER_H
