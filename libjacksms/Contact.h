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



#ifndef CONTACT_HH
#define CONTACT_HH 1
#include "PhoneNumber.h"
#include <QString>


namespace libJackSMS{

    namespace dataTypes{

        /**
         * Definisce un contatto
         */

        class contact{
            private:
                QString nome;
                phoneNumber telefono;
                QString gruppo;
                QString account;
                QString id;
                bool canReceiveJms;
            public:
                contact();
                contact(const QString & _nome,const phoneNumber & _telefono,const QString & _gruppo, const QString & _account);

                /**
                 * Preleva il nome corrente del contatto
                 * \return Il nome del contatto
                 */
                QString getName()const;

                /**
                 * Preleva il gruppo del contatto
                 * \return Il gruppo del contatto
                 */
                QString getGroup()const;

                /**
                 * Preleva l'account di default associato al contatto
                 * \return l'id dell'account
                 */
                QString getAccount()const;

                /**
                 * Preleva il numero telefonico del contatto
                 * \return il numero telefonico
                 */
                phoneNumber getPhone()const;

                /**
                 * Imposta l'id per il contatto
                 * \param _id l'id del contatto
                 */

                void setId(const QString & _id);

                /**
                 * Imposta l'id per il contatto
                 * \param _id l'id del contatto
                 */
                void setName(const QString & _name);
                /**
                 * Preleva l'id del contatto
                 * \return l'id del contatto
                 */
                QString getId()const;

                void setCanReceiveJms(bool can);
                /**
                 * Preleva l'id del contatto
                 * \return l'id del contatto
                 */
                bool getCanReceiveJms()const;

                /**
                 * Imposta l'account per il contatto
                 * \param _account l'account del contatto
                 */
                void setAccount(QString _account);

        };

    }

}
#endif //CONTACT_HH
