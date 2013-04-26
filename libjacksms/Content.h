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

#ifndef CONTENT_HH
#define CONTENT_HH 1

#include <QString>

namespace libJackSMS
{
    namespace dataTypes
    {
        /**
        *   Descrive un contenuto prelevato da una pagina web
        *
        **/
        class content{
            private:
                QString nome;
                QString valore;
                QString unEncodedValue;
                bool trovato;
                bool encode;
            public:
                content();
                /**
                 * Impoista il nome del contenuto
                 * \param _name Il nome del contenuto
                 */
                void setName(const QString & _name);
                /**
                 * Impoista il valore del contenuto codificato
                 * \param _val Il valore del contenuto
                 */
                void setValue(const QString & _val);
                /**
                 * Impoista il valore del contenuto non codificato
                 * \param _val Il valore del contenuto
                 */
                //void setUnEncodedValue(const QString & _val);
                /**
                 * Impoista il flag che mi dice se il contenuto era stato trovato o meno nella pagina
                 * \param _fount L'esito della ricerca
                 */
                void setFound(bool _found);
                /**
                 * Preleva il nome del contenuto
                 * \return Il nome del contenuto
                 */
                QString getName() const ;
                /**
                 * Preleva il valore del contenuto codificato
                 * \return Il valore del contenuto
                 */
                QString getValue() const;
                /**
                 * Preleva il valore del contenuto non codificato
                 * \return Il valore del contenuto
                 */
                //QString getUnEncodedValue() const;
                /**
                 * Preleva l'esito della ricerca del contenuto
                 * \return l'esito della ricerca (true= trovato, false=non trovato)
                 */
                bool getFound() const;

                /*void setToEncode(bool _e);
                bool getToEncode()const;*/

        };

    }

}

#endif //CONTENT_HH
