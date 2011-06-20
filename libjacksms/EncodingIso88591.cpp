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

#include "EncodingIso88591.h"

namespace libJackSMS
{
    encodingISO88591::encodingISO88591()
    {
    }

    QString encodingISO88591::getEncodedString(QString _text)
    {
        QByteArray _t = _text.toLatin1();
        return QString(_t);
    }

    QString encodingISO88591::getEncodedAndUrlString(QString _text)
    {
        QByteArray _t = _text.toLatin1().toPercentEncoding();
        return QString(_t);

    }
}
