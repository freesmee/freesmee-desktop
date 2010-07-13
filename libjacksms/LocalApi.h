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
#include <QList>
#include "xmlParserLocalApi.h"
#include "DataTypes.h"
#ifndef LOCALAPI_H
#define LOCALAPI_H 1




namespace libJackSMS{

    namespace localApi{
        class userFinder{
                private:
                    xmlParserApi::xmlParserLocalApiGeneric *xmlDocument;
                    QList<dataTypes::stringTriplet> utenti;
                    int indexUser;
                    QList<dataTypes::stringTriplet>::const_iterator iterUser;
                public:
                    userFinder();
                    bool findUsers();
                    bool nextUser();
                    QString currentUsername();
                    QString getPassword(const QString &_username) const ;
                    QString getDataDirectory(const QString &_username) const ;


        };

        class smsLogSaver{
            private:
                xmlParserApi::xmlParserLocalApiGeneric *xmlDocument;
                dataTypes::logSmsMessage msg;
            public:
                smsLogSaver(const QString & _currentUserDirectory);
                void setMessage(const dataTypes::logSmsMessage &_msg);
                bool appendToLogFile();
                bool saveAllToFile(const dataTypes::logSmsType &_smsContainer);
                bool deleteSms(const QString  &_id);
                bool deleteSms(const QList<QString>  &_idList);
        };
        class imLogSaver{
            private:
                xmlParserApi::xmlParserLocalApiGeneric *xmlDocument;
                dataTypes::logImMessage msg;
            public:
                imLogSaver(const QString & _currentUserDirectory);
                void setMessage(const dataTypes::logImMessage &_msg);
                bool appendToLogFile();
                QString getSavedId() const;
                bool saveAllToFile(const dataTypes::logImType &_smsContainer);
                bool deleteIm(const QString  &_id);
                bool deleteIm(const QList<QString>  &_idList);

        };
        class xmlLoader{
            private:
                xmlParserApi::xmlParserLocalApiGeneric *xmlDocument;
            public:
                xmlLoader(const QString & _currentUserDirectory);
                bool loadPhoneBook(libJackSMS::dataTypes::phoneBookType & _rubrica);
                bool loadServices(libJackSMS::dataTypes::servicesType & _servizi);
                bool loadAccounts(libJackSMS::dataTypes::configuredServicesType & _serviziConfigurati);
                bool loadOptions(libJackSMS::dataTypes::optionsType & _opzioni,bool overwriteExisting=true);
                bool loadSmsLog(libJackSMS::dataTypes::logSmsType & _logSms);
                bool loadImLog(libJackSMS::dataTypes::logImType & _logIm);
                //bool loadGlobalOptions(libJackSMS::dataTypes::optionsType & _opzioni,bool overwriteExisting=true);
        };
        class serviceManager{
            private:
                xmlParserApi::xmlParserLocalApiGeneric *xmlDocument;
            public:
                serviceManager();
                bool saveServices(QString _xml);
                bool mergeServices(QString _xml);
        };

        class accountManager{
            private:
                xmlParserApi::xmlParserLocalApiGeneric *xmlDocument;
            public:
                accountManager(const QString & _currentUserDirectory);
                bool addNewAccount(libJackSMS::dataTypes::configuredAccount & _account);
                bool updateAccount(libJackSMS::dataTypes::configuredAccount & _account);
                bool deleteAccount(const QString &_id);
        };

        class contactManager{
            private:
                xmlParserApi::xmlParserLocalApiGeneric *xmlDocument;
            public:
                contactManager(const QString & _currentUserDirectory);
                bool addNewContact(libJackSMS::dataTypes::contact & _contatto);
                bool updateContact(libJackSMS::dataTypes::contact & _contatto);
                bool deleteContact(const QString &_id);
        };
        class optionManager{
            private:
                xmlParserApi::xmlParserLocalApiGeneric *xmlDocument;
                dataTypes::optionsType &opzioni;
            public:
                optionManager(const QString & _currentUserDirectory,dataTypes::optionsType &_opzioni);
                bool save();
                bool increaseTotalSent();
        };

        class userDirectoryManager{
            private:
                xmlParserApi::xmlParserLocalApiGeneric *xmlDocument;
                QString user;
                bool mymkdir(const QString &_dir);
                bool copyFile(const QString &_in,const QString &_out);
                bool fileOrDirExists(const QString &_entry);
                QString userDir;
                QString error;
            public:
                userDirectoryManager(const QString _user);
                bool userDirectoryExists();
                bool createUser();
                QString getUserDir() const;
                QString getError() const;
                bool initializeDirectory();
        };

        class statsManager{
            private:
                xmlParserApi::xmlParserLocalApiGeneric *xmlDocument;
                QString userDir;
            public:
                statsManager(const QString & _currentUserDirectory);
                //bool updateStatsOfAccount(const dataTypes::configuredAccount & _account);

                bool updateStatsOfAccount(const QString &_accountId,const QString& _statName,const QString & _statVal);
                bool loadStats(libJackSMS::dataTypes::configuredServicesType & _servizi);
                bool increaseSentStat(const libJackSMS::dataTypes::configuredAccount & _servizio);
        };





    }
}
#endif //LOCALAPI_H
