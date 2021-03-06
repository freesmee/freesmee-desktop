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

#include "PhoneNumber.h"
#include "Contact.h"

namespace libJackSMS
{
    namespace dataTypes
    {
        contact::contact()
        {
        }

        contact::contact(const QString &_nome, const phoneNumber &_telefono ,const QString &_gruppo, const QString &_account) :
            nome(_nome),
            telefono(_telefono),
            gruppo(_gruppo),
            account(_account),
            canReceiveJms(false),
            carrier(0)
        {
        }

        QString contact::getId() const
        {
            return id;
        }

        QString contact::getName() const
        {
            return nome;
        }

        QString contact::getGroup() const
        {
            return gruppo;
        }

        QString contact::getAccount() const
        {
            return account;
        }

        phoneNumber contact::getPhone() const
        {
            return telefono;
        }

        bool contact::getCanReceiveJms() const
        {
            return canReceiveJms;
        }

        void contact::setId(const QString &_id)
        {
            id = _id;
        }

        void contact::setName(const QString &_name)
        {
            nome = _name;
        }

        void contact::setCanReceiveJms(bool can)
        {
            canReceiveJms = can;
        }

        void contact::setAccount(QString _account)
        {
            account = _account;
        }

        void contact::setCarrier(int _carrier)
        {
            carrier = _carrier;
        }

        int contact::getCarrier() const
        {
            return carrier;
        }

        QString contact::getCarrierString() const
        {
            switch (carrier)
            {
            case 1:
                return "Vodafone";
            case 2:
                return "Tim";
            case 3:
                return "Tre";
            case 4:
                return "Wind";

            default:
                return "";
            }
        }

    }

}
