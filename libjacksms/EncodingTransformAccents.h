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

#ifndef ENCODINGUTRANSFORMACCENTS_HH
#define ENCODINGUTRANSFORMACCENTS_HH 1

#include "Encoding.h"

namespace libJackSMS
{

    class encodingTransformAccents : public encodingGeneric
    {
        public:
            encodingTransformAccents();
            ~encodingTransformAccents();
            QString getEncodedString(QString _text);
            QString getEncodedAndUrlString(QString _text);
    };


}
#endif //ENCODINGUTRANSFORMACCENTS_HH

