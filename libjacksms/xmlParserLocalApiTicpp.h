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

#ifndef XMLPARSERLOCALAPITICPP_HH_GUARD
#define XMLPARSERLOCALAPITICPP_HH_GUARD 1

#include "DataTypes.h"
#include "xmlParserLocalApi.h"

namespace libJackSMS
{
    namespace xmlParserApi
    {
        class xmlParserLocalApiTicpp : public xmlParserLocalApiGeneric
        {
            private:
                QString userDirectory;
            public:
                xmlParserLocalApiTicpp(const QString &_userDirectory);
                ~xmlParserLocalApiTicpp();
                bool appendSmsToLogfile(dataTypes::logSmsMessage &_msg);
                bool appendImToLogfile(dataTypes::logImMessage &_msg );
                bool saveAllSmsToLogFile(const dataTypes::logSmsType &_smsContainer);
                bool saveAllImToLogFile(const dataTypes::logImType &_smsContainer);
                bool loadServices(libJackSMS::dataTypes::servicesType & _servizi);
                bool loadAccounts(libJackSMS::dataTypes::configuredServicesType & _serviziConfigurati);
                bool loadOptions(libJackSMS::dataTypes::optionsType & _opzioni,bool overwriteExisting=true);
                bool loadSmsLog(libJackSMS::dataTypes::logSmsType & _logSms);
                bool loadImLog(libJackSMS::dataTypes::logImType & _logIm);
                bool loadUsers(QList<dataTypes::stringTriplet> &_utenti);
                bool saveOptions(const dataTypes::optionsType &_opzioni);
                bool userDirectoryExists(const QString &_user);
                bool createUser(const QString &_user,const QString &_directory);
                bool createUsersFile();
                bool saveServices(QString _xml);
                bool updateStat(const QString &_accountId,const QString& _statName,const QString & _statVal);
                //bool updateStat(libJackSMS::dataTypes::configuredAccount & _service);
                bool loadStats(libJackSMS::dataTypes::configuredServicesType & _services);
                bool mergeServices(QString _xml);
                bool deleteSmsMessage(const QList<QString> & idList);
                bool deleteImMessage(const QList<QString> & idList);

                // funzioni commentate dato che ora sono state spostate sul server
                /*bool loadPhoneBook(libJackSMS::dataTypes::phoneBookType & _rubrica);
                bool addNewAccount(libJackSMS::dataTypes::configuredAccount & _account);
                bool updateAccount(libJackSMS::dataTypes::configuredAccount & _account);
                bool deleteAccount(const QString &_id);
                bool addNewContact(libJackSMS::dataTypes::contact & _contatto);
                bool updateContact(libJackSMS::dataTypes::contact & _contatto);
                bool deleteContact(const QString &_name);*/
        };
    }
}

#endif //XMLPARSERLOCALAPITICPP_HH_GUARD
