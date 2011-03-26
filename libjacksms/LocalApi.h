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
#include <QThread>
#ifndef LOCALAPI_H
#define LOCALAPI_H 1




namespace libJackSMS {

    namespace localApi {

        class userFinder {

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
                    QString getPassword(const QString &_username) const;
                    QString getDataDirectory(const QString &_username) const;
        };

        class smsLogSaver {

            private:
                xmlParserApi::xmlParserLocalApiGeneric *xmlDocument;
                dataTypes::logSmsMessage msg;

            public:
                smsLogSaver(const QString & _currentUserDirectory);
                void setMessage(const dataTypes::logSmsMessage &_msg);
                bool appendToLogFile();
                QString getSavedId() const;
                bool saveAllToFile(const dataTypes::logSmsType &_smsContainer);
                bool deleteSms(const QString  &_id);
                bool deleteSms(const QList<QString>  &_idList);
        };

        class imLogSaver {

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

        class optionLoader : public QThread {
            Q_OBJECT

            private:
                xmlParserApi::xmlParserLocalApiGeneric *xmlDocument;

            protected:
                void run();

            public:
                void load();
                optionLoader(QString _userDir = "");

            signals:
                void endLoad(libJackSMS::dataTypes::optionsType);
                void criticalError(QString);
        };

        class synchronousOptionLoader : public QObject {
            Q_OBJECT

            private:
                xmlParserApi::xmlParserLocalApiGeneric *xmlDocument;

            public:
                bool load(libJackSMS::dataTypes::optionsType &dest);
                synchronousOptionLoader(QString _userDir = "");
        };

        class serviceLoader : public QThread {
            Q_OBJECT

            private:
                xmlParserApi::xmlParserLocalApiGeneric *xmlDocument;

            protected:
                void run();

            public:
                void load();
                serviceLoader();

            signals:
                void endLoad(libJackSMS::dataTypes::servicesType);
                void criticalError(QString);
        };

        class xmlLoader : public QObject {
            Q_OBJECT

            private:
                xmlParserApi::xmlParserLocalApiGeneric *xmlDocument;
                serviceLoader *lo;
                optionLoader *l;
                QString currentUserDirectory;

            public:
                xmlLoader(const QString &_currentUserDirectory);
                //bool loadPhoneBook(libJackSMS::dataTypes::phoneBookType &_rubrica);
                void loadServices();
                bool loadAccounts(libJackSMS::dataTypes::configuredServicesType &_serviziConfigurati);
                void loadOptions();
                bool loadSmsLog(libJackSMS::dataTypes::logSmsType &_logSms);
                bool loadImLog(libJackSMS::dataTypes::logImType &_logIm);

           signals:
                void servicesLoaded(libJackSMS::dataTypes::servicesType);
                void optionsLoaded(libJackSMS::dataTypes::optionsType);
                void criticalError(QString);
        };

        class serviceManagerBase : public QThread {
            Q_OBJECT

            private:
                QString xml;
                xmlParserApi::xmlParserLocalApiGeneric *xmlDocument;
                void run();

            public:
                serviceManagerBase(const QString &_xml);
                void merge();

            signals:
                void merged();
        };

        class serviceManager : public QObject {
            Q_OBJECT

            private:
                xmlParserApi::xmlParserLocalApiGeneric *xmlDocument;
            public:

                serviceManager();
                bool saveServices(QString _xml);
                void mergeServices(const QString &_xml);

            signals:
                void merged();
        };

        /*class accountManager {

            private:
                xmlParserApi::xmlParserLocalApiGeneric *xmlDocument;
            public:

                accountManager(const QString & _currentUserDirectory);
                bool addNewAccount(libJackSMS::dataTypes::configuredAccount &_account);
                bool updateAccount(libJackSMS::dataTypes::configuredAccount &_account);
                bool deleteAccount(const QString &_id);
        };

        class contactManager {

            private:
                xmlParserApi::xmlParserLocalApiGeneric *xmlDocument;

            public:
                contactManager(const QString &_currentUserDirectory);
                bool addNewContact(libJackSMS::dataTypes::contact &_contatto);
                bool updateContact(libJackSMS::dataTypes::contact &_contatto);
                bool deleteContact(const QString &_id);
        };*/

        class optionManager {

            private:
                xmlParserApi::xmlParserLocalApiGeneric *xmlDocument;
                dataTypes::optionsType &opzioni;

            public:
                optionManager(const QString & _currentUserDirectory,dataTypes::optionsType &_opzioni);
                bool save();
                bool increaseTotalSent();
        };

        class cookieManager {

            private:
                QString userDir;

            public:
                cookieManager(const QString &_currentUserDirectory);
                bool deleteCookies();
        };

        class userDirectoryManager {

            private:
                xmlParserApi::xmlParserLocalApiGeneric *xmlDocument;
                QString user;
                bool mymkdir(const QString &_dir);
                bool copyFile(const QString &_in, const QString &_out);
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

        class statsManager {

            private:
                xmlParserApi::xmlParserLocalApiGeneric *xmlDocument;
                QString userDir;

            public:
                statsManager(const QString &_currentUserDirectory);
                //bool updateStatsOfAccount(const dataTypes::configuredAccount & _account);

                bool updateStatsOfAccount(const QString &_accountId, const QString &_statName, const QString &_statVal);
                bool loadStats(libJackSMS::dataTypes::configuredServicesType &_servizi);
                bool increaseSentStat(const libJackSMS::dataTypes::configuredAccount &_servizio);
        };

    }
}

#endif //LOCALAPI_H
