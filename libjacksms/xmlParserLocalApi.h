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

#ifndef XMLPARSERLOCALAPI_HH_GUARD
#define XMLPARSERLOCALAPI_HH_GUARD 1

#include "DataTypes.h"
#include "DataTypesApi.h"
#include "StringTriplet.h"
#include <QList>

namespace libJackSMS
{
    namespace xmlParserApi
    {
        class xmlParserLocalApiGeneric
        {
            public:
                xmlParserLocalApiGeneric(const QString&)
                {
                }

                virtual ~xmlParserLocalApiGeneric()
                {
                }

                virtual bool appendSmsToLogfile(dataTypes::logSmsMessage &_msg) = 0;
                virtual bool appendImToLogfile(dataTypes::logImMessage &_msg) = 0;
                virtual bool saveAllSmsToLogFile(const dataTypes::logSmsType &_smsContainer) = 0;
                virtual bool saveAllImToLogFile(const dataTypes::logImType &_smsContainer) = 0;
                virtual bool loadServices(dataTypes::servicesType &_servizi) = 0;
                virtual bool loadAccounts(dataTypes::configuredServicesType &_serviziConfigurati) = 0;
                virtual bool loadOptions(dataTypes::optionsType &_opzioni, bool overwriteExisting = true) = 0;
                virtual bool loadSmsLog(dataTypes::logSmsType &_logSms) = 0;
                virtual bool loadImLog(dataTypes::logImType &_logIm) = 0;
                virtual bool loadUsers(QList<dataTypes::stringTriplet> &_utenti) = 0;
                virtual bool saveOptions(const dataTypes::optionsType &_opzioni) = 0;
                virtual bool userDirectoryExists(const QString &_user) = 0;
                virtual bool createUser(const QString &_user, const QString &_directory) = 0;
                virtual bool createUsersFile() = 0;
                virtual bool saveServices(QString _xml) = 0;
                virtual bool updateStat(const QString &_accountId, const QString &_statName, const QString &_statVal) = 0;
                //virtual bool updateStat(libJackSMS::dataTypes::configuredAccount &_service) = 0;
                virtual bool loadStats(libJackSMS::dataTypes::configuredServicesType &_services) = 0;
                virtual bool mergeServices(QString _xml) = 0;
                virtual bool deleteSmsMessage(const QList<QString> &idList) = 0;
                virtual bool deleteImMessage(const QList<QString> &idList) = 0;

                // funzioni commentate dato che ora sono state spostate sul server
                /*virtual bool loadPhoneBook(dataTypes::phoneBookType &_rubrica) = 0;
                virtual bool addNewAccount(libJackSMS::dataTypes::configuredAccount &_account) = 0;
                virtual bool updateAccount(libJackSMS::dataTypes::configuredAccount &_account) = 0;
                virtual bool deleteAccount(const QString &_id) = 0;
                virtual bool addNewContact(libJackSMS::dataTypes::contact &_contatto) = 0;
                virtual bool updateContact(libJackSMS::dataTypes::contact &_contatto) = 0;
                virtual bool deleteContact(const QString &_name) = 0;*/

        };
    }
}

#endif //XMLPARSERLOCALAPI_HH_GUARD
