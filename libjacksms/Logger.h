/*
    Copyright (C) <2009>  <ivan vaccari> <grisson@jacksms.it>

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
#include <QList>
#include <QString>
#ifndef LOGGER_HH
#define LOGGER_HH 1



namespace libJackSMS{

        class logger{
            private:
                QList<QString> noticeList;
            public:
                logger();
                void addNotice(const QString &_notice);
                bool save();
                bool save(const QString &_filename);
       };

}
#endif //LOGGER_HH
