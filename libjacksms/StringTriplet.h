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

#ifndef STRINGTRIPLET_HH
#define STRINGTRIPLET_HH 1

#include <QString>

namespace libJackSMS
{
    namespace dataTypes
    {
        class stringTriplet
        {
            private:
                QString one;
                QString two;
                QString three;

            public:
                stringTriplet(const QString &_first, const QString &_second, const QString &_third);

                QString getFirst() const;
                void setFirst(const QString &_str);

                QString getSecond() const;
                void setSecond(const QString &_str);

                QString getThird() const;
                void setThird(const QString &_str);

        };

    }

}
#endif //STRINGTRIPLET_HH
