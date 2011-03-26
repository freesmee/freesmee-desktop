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

#include <QMap>
#include <QString>
#ifndef DATATYPESAPI_HH
#define DATATYPESAPI_HH 1



namespace libJackSMS{

    namespace dataTypesApi{


        //typedef QString sessionId;
        typedef QMap<QString,QString> errorsType;
        class clientVersion {
            protected:
                int major;
                int minor;
                char release;
                int build;

            public:
                clientVersion(QString _v);
                bool  operator< ( const clientVersion & client);


        };


    }

}
#endif //DATATYPESAPI_HH
