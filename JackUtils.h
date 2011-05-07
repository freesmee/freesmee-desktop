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

#ifndef JACKUTILS_H
#define JACKUTILS_H

#include <string>
#include <sstream>
#include <QString>

namespace jackUtils
{
/*QString Base64Encode(const QString &toEncode);

QString Base64Decode(const QString &toDecode);
*/

template <typename SRCTYPE>
QString ToQString(SRCTYPE src){
    std::stringstream ss(std::stringstream::in | std::stringstream::out);
    ss<< src;
    std::string tmp;
    ss>>tmp;
    return QString(tmp.c_str());
}
}

#endif // JACKUTILS_H
