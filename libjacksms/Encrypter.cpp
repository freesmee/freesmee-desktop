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

#include "Encrypter.h"
#include <cmath>
#include <QChar>
#include "Utilities.h"
#include <iostream>

namespace libJackSMS
{
    namespace Encrypter
    {
        Encrypter::Encrypter()
        {
        }

        QString  Encrypter::base(QString _str,int multiplier)
        {
            QString fi;
            for (int i = 0, x = 0; i < _str.length(); ++i, ++x)
            {
                fi.append(_str.at(i) + 30 * multiplier);
            }

            return fi;
        }

        QString Encrypter::encrypt(QString _strToEnc)
        {
            if (_strToEnc.isEmpty())
                return _strToEnc;

            _strToEnc =_strToEnc.append("control");
            QString res = _strToEnc;
            res = utilities::Base64Encode(res);
            res = base(res,+1);
            res = utilities::Base64Encode(res);

            return res;
        }

        QString Encrypter::decrypt(const QString &_strToDec)
        {
            if (_strToDec.isEmpty())
                return _strToDec;

            QString r = utilities::Base64Decode(_strToDec);
            r = base(r, -1);
            r = utilities::Base64Decode(r);

            if (r.length() < 7)
                return "";

            QString control = r.mid(r.length() - 7, 7);
            if (control == "control")
                return r.left(r.length() - 7);

            else
                return "";

        }

    }
}
