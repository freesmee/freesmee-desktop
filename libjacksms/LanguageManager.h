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

#ifndef LANGUAGEMANAGER_H
#define LANGUAGEMANAGER_H

#include <QMap>
#include <QList>
#include <QString>

/*using singleton design pattern*/
namespace libJackSMS
{
    namespace dataTypes
    {
        class Language
        {
        private:
            QMap<QString,QString> coppie;
            QString langName;
        public:
            Language(QString _lname);
            void setString(const QString _id, const QString _string);
            QString getString(const QString _id)const;
            QString getLangName() const;
        };

    }
}

namespace libJackSMS
{
    class LanguageManager
    {
    private:
        LanguageManager();
        static LanguageManager* istance;
        QList<dataTypes::Language> languages;
        QList<dataTypes::Language>::const_iterator currentLanguage;
        QList<dataTypes::Language>::const_iterator lanIter;
        bool isSet;
        int lan_iterator_counter;

    public:

        static LanguageManager* getIstance();
        QString getString(const QString &_id);
        bool setLanguage(QString _name);
        bool nextLanguage();
        QString currentLang() const;

    };
}

#endif // LANGUAGEMANAGER_H
