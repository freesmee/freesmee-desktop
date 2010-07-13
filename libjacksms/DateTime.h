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
#include <QString>
#include <QDateTime>
#ifndef DATETIME_HH
#define DATETIME_HH 1


namespace libJackSMS{

    namespace dataTypes{
        /**
        * Descrive la data.
        **/
        class dateTime{
            private:
                QDateTime data;

            public:
                dateTime();
                /**
                 * costruisce l'oggetto data inizializzandolo alla data passata dai parametri
                 * \param _inputDate La data in input (formato dd/mm/yy)
                 * \param _inputTime L'ora in input (formato hh::mm)
                 */
                dateTime(const QString &  _inputDate,const QString &  _inputTime);

                /**
                 * costruisce l'oggetto data inizializzandolo alla data passata dai parametri
                 * \param _inputDate La data in input (formato dd/mm/yy hh:mm:ss)
                 */
                dateTime(const QString &  _inputDate);
                /**
                 * Preleva il giorno associato all'oggetto corrente
                 * \return il giorno
                 */
                //int getDay();
                /**
                 * Preleva l'anno associato all'oggetto corrente
                 * \return l'anno
                 */
                //int getYear();
                /**
                 * Preleva il mese associato all'oggetto corrente
                 * \return il mese
                 */
                //int getMonth();
                /**
                 * Preleva il minuto associato all'oggetto corrente
                 * \return il minuto
                 */
                //int getMinutes();
                /**
                 * Preleva l'ora associata all'oggetto corrente
                 * \return l'ora
                 */
                //int getHour();
                /**
                 * Preleva il secondo associato all'oggetto corrente
                 * \return il secondo
                 */
                //int getSecond();
                /**
                 * ritorna una stringa contenente la data corrente (formato dd/mm/yy)
                 * \return la data corrente
                 */
                static QString getCurrentDay();
                /**
                 * ritorna una stringa contenente l'ora corrente (formato hh::mm)
                 * \return la data corrente
                 */
                static QString getCurrentHour();
                /**
                 * ritorna una stringa contenente la data associata all'oggetto corrente (formato dd/mm/yy)
                 * \return la data dell'oggetto corrente
                 */
                QString getStringDay()const;
                /**
                 * ritorna una stringa contenente l'ora associata all'oggetto corrente (formato hh:mm)
                 * \return l'ora dell'oggetto corrente
                 */
                QString getStringHour()const;

                QDateTime getData() const;
        };
    }
}
#endif //DATETIME_HH
