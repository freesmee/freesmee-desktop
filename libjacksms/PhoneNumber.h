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


#ifndef PHONENUMBER_HH_GUARD
#define PHONENUMBER_HH_GUARD 1
#include <QString>



namespace libJackSMS{
    namespace dataTypes{

        /**
        * descrive un numero telefonico
        **/
        class phoneNumber{
            private:
                QString intcode;
                QString intpref;
                QString num;
                QString intnum;
                QString pref;
                QString alternativeName;
                bool valid;
                bool specialNumber;
            public:
                phoneNumber();
                /**
                 * Esegue una scansione del numero telefonico. \n\n
                 * Formati supportati: \n
                 *\b aa.bb.cc dove
                 *                     \li \c aa indica il prefisso internazionale (es +39)
                 *                     \li \c bb indica il prefisso operatore (es 339)
                 *                     \li \c cc indica il numero telefonico (es 1234567)
                 *
                 *\b aabb dove
                 *                     \li \c aa indica il prefisso operatore di 3 cifre (es 339)
                 *                     \li \c bb indica il numero telefonico di 7 cifre (es 1234567)
                 *                     \li In questo caso, al numero sar√  applicato il prefisso internazionale \b +39\n
                 *
                 * \param _phoneNum la stringa contenente il numero telefonico
                 *
                 */

                void parse(QString _phoneNum);

                /**
                 * Imposta
                 * \param _name il nome alternativo
                 */
                void setAltName(const QString &_name);
                /**
                 * Preleva il nome alternativo di questo contatto
                 * \return il nome alternativo
                 */
                QString getAltName() const;
                /**
                 * Preleva il numero telefonico composto da prefisso operatore concatenato a numero telefonico
                 * \return il numero telefonico
                 */
                QString getIntNum()const ;
                /**
                 * Preleva il numero telefonico senza alcun prefisso
                 * \return il numero telefonico
                 */
                QString getNum()const;
                /**
                 * Preleva il prefisso internazionale con +
                 * \return il prefisso telefonico
                 */
                QString getIntPref()const;
                /**
                 * Preleva il prefisso operatore
                 * \return il prefisso telefonico
                 */
                QString getPref()const;

                /**
                 * Preleva il prefisso internazionale senza +
                 * \return il  numero internazionale
                 */
                QString getIntCode() const;




                /**
                 * Preleva il numero telefonico completo di prefisso internazionale (con +) e prefisso operatore. I campi sono separati da "." (punto)
                 * \return il prefisso telefonico
                 */
                QString toString() const;

                void setIsValid(bool v);
                bool getIsValid() const;



        };
    }

}

#endif //PHONENUMBER_HH_GUARD
