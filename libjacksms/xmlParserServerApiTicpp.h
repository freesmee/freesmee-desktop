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

#ifndef XMLPARSERSERVERAPITICPP_HH_GUARD
#define XMLPARSERSERVERAPITICPP_HH_GUARD 1

#include "DataTypes.h"
#include "xmlParserServerApi.h"
#include "ticpp.h"
#include "ticpprc.h"

namespace libJackSMS
{
    namespace xmlParserApi
    {
        class xmlParserServerApiTicpp : public xmlParserServerApiGeneric
        {
            private:
                ticpp::Document xmlResponse;
                bool alreadyUsed;
                void loadPhoneBookBase(libJackSMS::dataTypes::phoneBookType & _rubrica,ticpp::Node *root);

            public:
                xmlParserServerApiTicpp();
                ~xmlParserServerApiTicpp();
                void setXml(QString _xml);
                bool checkIsLogged();
                QString getSessionId();
                QString getError();
                bool checkGmailImport(int &num);
                QString gmailError();
                //bool loadErrors(dataTypesApi::errorsType & _errors);
                bool loadPhoneBook(libJackSMS::dataTypes::phoneBookType & _rubrica);
                bool loadPhoneBook2(libJackSMS::dataTypes::phoneBookType & _rubrica);
                bool extractImMessages(libJackSMS::dataTypes::logImType & _logIm);
                bool loadAccounts(libJackSMS::dataTypes::configuredServicesType & _serviziConfigurati);
                bool checkSaved(QString &_id,QString &totalForAccount);
                bool checkReport();
                bool checkAddNewContact(QString &_resId, bool &_canReceiveJms, int &_crr);
                bool checkUpdateContact(bool &_canReceiveJms, int &_crr);
                bool checkDeleteContact();
                bool checkAddNewAccount(QString &_resId);
                bool checkUpdateAccount();
                bool checkDeleteAccount();
                //bool parseConversation(libJackSMS::dataTypes::logSmsType & _logSms,libJackSMS::dataTypes::logImType & _logIm);
                bool parseServices(libJackSMS::dataTypes::servicesType &_servizi);
                QString getVersion();
        };

    }
}

#endif //XMLPARSERSERVERAPITICPP_HH_GUARD
