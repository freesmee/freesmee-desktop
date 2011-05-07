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

#include "PageContent.h"
#include "StringTriplet.h"


namespace libJackSMS{

    namespace dataTypes{


            /****************definitions for pageContent**********/
            pageContent::pageContent(){
            }
            pageContent::pageContent(const QString &_name,const QString &_left,const QString &_right):n(_name),l(_left),r(_right),encode(false){
            }
            QString pageContent::getName() const{
                return n;
            }
            QString pageContent::getLeft() const{
                return l;
            }
            QString pageContent::getRight() const{
                return r;
            }
            /*void pageContent::setToEncode(bool _e){
                encode=_e;
            }
            bool pageContent::getToEncode()const{
                return encode;
            }*/

    }

}
