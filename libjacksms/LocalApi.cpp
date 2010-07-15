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
#include "xmlParserLocalApiTicpp.h"
#include "DataTypes.h"
#include "LocalApi.h"
#include "Utilities.h"
#include "FilesDirectory.h"
#include "Encrypter.h"
#include <QDir>
#ifdef WIN32
    #include <dirent.h>
#else
    #ifdef WIN64
        #include <dirent.h>
    #else
        #include <sys/stat.h>
        #include <sys/types.h>
        #include <cstdlib>
    #endif
#endif




namespace libJackSMS{

    namespace localApi{

        smsLogSaver::smsLogSaver(const QString & _currentUserDirectory):
            xmlDocument(new libJackSMS::xmlParserApi::xmlParserLocalApiTicpp(_currentUserDirectory)){

        }
        void smsLogSaver::setMessage(const dataTypes::logSmsMessage &_msg){
            msg=_msg;
        }
        bool smsLogSaver::appendToLogFile(){
            return xmlDocument->appendSmsToLogfile(msg);
        }
        QString smsLogSaver::getSavedId() const{
            return msg.getId();
        }
        bool smsLogSaver::saveAllToFile(const dataTypes::logSmsType &_smsContainer){
            return xmlDocument->saveAllSmsToLogFile(_smsContainer);

        }

        imLogSaver::imLogSaver(const QString & _currentUserDirectory):
            xmlDocument(new xmlParserApi::xmlParserLocalApiTicpp(_currentUserDirectory)){
        }
        void imLogSaver::setMessage(const dataTypes::logImMessage &_msg){
            msg=_msg;
        }
        bool imLogSaver::appendToLogFile(){
            return xmlDocument->appendImToLogfile(msg);
        }
        QString imLogSaver::getSavedId() const{
            return msg.getId();
        }
        bool imLogSaver::saveAllToFile(const dataTypes::logImType &_smsContainer){
            return xmlDocument->saveAllImToLogFile(_smsContainer);
        }
        bool smsLogSaver::deleteSms(const QString  &_id){
            QList<QString> _idList;
            _idList.push_back(_id);
            return xmlDocument->deleteSmsMessage(_idList);
        }
        bool smsLogSaver::deleteSms(const  QList<QString>  &_idList){
            return xmlDocument->deleteSmsMessage(_idList);
        }
        bool imLogSaver::deleteIm(const QString  &_id){
            QList<QString> _idList;
            _idList.push_back(_id);
            return xmlDocument->deleteImMessage(_idList);
        }
        bool imLogSaver::deleteIm(const  QList<QString>  &_idList){
            return xmlDocument->deleteImMessage(_idList);
        }
        xmlLoader::xmlLoader(const QString & _currentUserDirectory):
            xmlDocument(new libJackSMS::xmlParserApi::xmlParserLocalApiTicpp(_currentUserDirectory)){

        }
        bool xmlLoader::loadPhoneBook(libJackSMS::dataTypes::phoneBookType & _rubrica){
            return xmlDocument->loadPhoneBook(_rubrica);
        }
        bool xmlLoader::loadServices(libJackSMS::dataTypes::servicesType & _servizi){
            return xmlDocument->loadServices(_servizi);
        }
        bool xmlLoader::loadAccounts(libJackSMS::dataTypes::configuredServicesType & _serviziConfigurati){
            return xmlDocument->loadAccounts(_serviziConfigurati);
        }
        bool xmlLoader::loadOptions(libJackSMS::dataTypes::optionsType & _opzioni,bool overwriteExisting){
            if(xmlDocument->loadOptions(_opzioni,overwriteExisting)){
                libJackSMS::dataTypes::optionsType::iterator i=_opzioni.begin();
                libJackSMS::dataTypes::optionsType::iterator i_end=_opzioni.end();
                for(;i!=i_end;++i){

                    if (i.key().indexOf("password")!=-1){
                        i.value()=Encrypter::Encrypter::decrypt(i.value());
                    }
                }
                return true;
            }else
                return false;
        }

        bool xmlLoader::loadSmsLog(libJackSMS::dataTypes::logSmsType & _logSms){
            return xmlDocument->loadSmsLog(_logSms);
        }
        bool xmlLoader::loadImLog(libJackSMS::dataTypes::logImType & _logIm){
            return xmlDocument->loadImLog(_logIm);
        }



        accountManager::accountManager(const QString & _currentUserDirectory):
            xmlDocument(new libJackSMS::xmlParserApi::xmlParserLocalApiTicpp(_currentUserDirectory)){

        }
        bool accountManager::addNewAccount(libJackSMS::dataTypes::configuredAccount & _account){
            return xmlDocument->addNewAccount(_account);
        }
        bool accountManager::updateAccount(libJackSMS::dataTypes::configuredAccount & _account){
            return xmlDocument->updateAccount(_account);
        }
        bool accountManager::deleteAccount(const QString &_id){
            return xmlDocument->deleteAccount(_id);
        }

        userFinder::userFinder():xmlDocument(new libJackSMS::xmlParserApi::xmlParserLocalApiTicpp("")),indexUser(0){
        }
        bool userFinder::findUsers(){
            bool res=xmlDocument->loadUsers(utenti);
            QList<dataTypes::stringTriplet>::iterator i=utenti.begin();
            QList<dataTypes::stringTriplet>::iterator i_end=utenti.end();
            for(;i!=i_end;++i){
                i->setSecond(Encrypter::Encrypter::decrypt(i->getSecond()));
            }
            return res;
        }
        bool userFinder::nextUser(){
            QList<dataTypes::stringTriplet>::const_iterator i_end=utenti.end();
            if (indexUser==0){
                iterUser=utenti.begin();
            }else{
                ++iterUser;
            }
            ++indexUser;
            if (iterUser==i_end){
                return false;
                indexUser=0;
            }else
                return true;
        }
        QString userFinder::currentUsername(){

            return iterUser->getFirst();

        }
        QString userFinder::getPassword(const QString &_username) const{
            QList<dataTypes::stringTriplet>::const_iterator i=utenti.begin();
            QList<dataTypes::stringTriplet>::const_iterator i_end=utenti.end();
            for(;i!=i_end;++i){
                    if (i->getFirst()==_username)
                        return i->getSecond();
            }
            return "";

        }
        QString userFinder::getDataDirectory(const QString &_username) const{
            QList<dataTypes::stringTriplet>::const_iterator i=utenti.begin();
            QList<dataTypes::stringTriplet>::const_iterator i_end=utenti.end();
            for(;i!=i_end;++i){
                    if (i->getFirst()==_username)
                        return i->getThird();
            }
            return "";
        }

        contactManager::contactManager(const QString & _currentUserDirectory)
            :xmlDocument(new libJackSMS::xmlParserApi::xmlParserLocalApiTicpp(_currentUserDirectory)){
        }
        bool contactManager::addNewContact(libJackSMS::dataTypes::contact & _contatto){
            return xmlDocument->addNewContact(_contatto);
        }
        bool contactManager::updateContact(libJackSMS::dataTypes::contact & _contatto){
            return xmlDocument->updateContact(_contatto);
        }
        bool contactManager::deleteContact(const QString &_name){
            return xmlDocument->deleteContact(_name);
        }

        optionManager::optionManager(const QString & _currentUserDirectory,dataTypes::optionsType &_opzioni)
            :xmlDocument(new libJackSMS::xmlParserApi::xmlParserLocalApiTicpp(_currentUserDirectory)),
            opzioni(_opzioni){
        }

        bool optionManager::save(){
            libJackSMS::dataTypes::optionsType::iterator i=opzioni.begin();
            libJackSMS::dataTypes::optionsType::iterator i_end=opzioni.end();
            for(;i!=i_end;++i){
                if (i.key().indexOf("password")!=-1){
                    i.value()=Encrypter::Encrypter::encrypt(i.value());
                }
            }
            bool r= xmlDocument->saveOptions(opzioni);
            i=opzioni.begin();
            i_end=opzioni.end();
            for(;i!=i_end;++i){
                if (i.key().indexOf("password")!=-1){
                    i.value()=Encrypter::Encrypter::decrypt(i.value());
                }
            }
            return r;

        }
        bool optionManager::increaseTotalSent(){
            dataTypes::optionsType::const_iterator i=opzioni.find("sent-count");
            int n=1;
            if (i!=opzioni.end()){
                bool ok;
                n=i.value().toInt(&ok,10)+1;
            }

            opzioni["sent-count"]=QString::number(n);
            return xmlDocument->saveOptions(opzioni);

        }
        statsManager::statsManager(const QString & _currentUserDirectory)
            :xmlDocument(new libJackSMS::xmlParserApi::xmlParserLocalApiTicpp(_currentUserDirectory))
            {
        }

       /* bool statsManager::updateStatsOfAccount(const dataTypes::configuredAccount & _account){
            return xmlDocument->updateStat(_account);

        }*/
        bool statsManager::updateStatsOfAccount(const QString &_accountId,const QString& _statName,const QString & _statVal){
            return xmlDocument->updateStat(_accountId,_statName,_statVal);
        }

        bool statsManager::increaseSentStat(const libJackSMS::dataTypes::configuredAccount & _servizio){
            bool ok;
            int n=_servizio.getStat("sent").toInt(&ok,10)+1;
            return xmlDocument->updateStat(_servizio.getId(),"sent",QString::number(n));
        }
        bool statsManager::loadStats(libJackSMS::dataTypes::configuredServicesType & _servizi){
            return xmlDocument->loadStats(_servizi);
        }

        userDirectoryManager::userDirectoryManager(const QString _user)
            :xmlDocument(new libJackSMS::xmlParserApi::xmlParserLocalApiTicpp("")),user(_user){
        }
        bool userDirectoryManager::fileOrDirExists(const QString &_entry){
            QDir d(_entry);
            if (d.exists()){
                return true;
            }else{
                QFile f(_entry);
                return f.exists();

            }
            /*if ( access( _entry.toStdString().c_str(), 0 ) == 0 )
                return true;
            else
                return false;*/
        }

        bool userDirectoryManager::mymkdir(const QString &_dir){
        QDir d;
        return d.mkpath(_dir);
        /*#ifdef WIN32
            return 0==mkdir(_dir.c_str());
        #else
            #ifdef WIN64
                return 0==mkdir(_dir.c_str());
            #else
                return 0==mkdir(_dir.c_str(),S_IRWXU | S_IRWXG | S_IROTH); //775
            #endif
        #endif*/
        }

        bool userDirectoryManager::copyFile(const QString &_in,const QString &_out){
            /*std::ifstream ifs(_in.c_str(), std::ios::binary);
            if (ifs.is_open()){
                std::ofstream ofs(_out.c_str(), std::ios::binary);
                if (ofs.is_open()){
                    ofs << ifs.rdbuf();
                    return true;
                }
            }
            return false;*/
            return QFile::copy(_in,_out);


        }

        bool userDirectoryManager::userDirectoryExists(){
            return xmlDocument->userDirectoryExists(user);

        }
        QString userDirectoryManager::getUserDir() const{
            return userDir;
        }
        bool userDirectoryManager::initializeDirectory(){

            QString path=directories::concatDirectoryAndFile(directories::XmlDirectory(),"users.xml");
            
            if (fileOrDirExists(path)){
                path=directories::DumpDirectory();
                if (!fileOrDirExists(path))
                    if(!mymkdir(path)){
                        error="cannot create "+path;
                        return false;
                    }
                path=directories::concatDirectoryAndFile(directories::XmlDirectory(),"services.xml");
                if (fileOrDirExists(path)){
                    return true;
                }else{
                    bool r;
                    #ifdef WIN32
                        r= copyFile("services.xml",path);
                    #else
                        #ifdef WIN64
                            r= copyFile("services.xml",path);
                        #else
                            r= copyFile("/var/jacksms2/services.xml",path);
                        #endif
                    #endif
                    if (!r){
                        error="cannot copy "+path+" file";

                    }
                    return r;

                }
            }else{
                QString dir=directories::DataDirectory();
                if (!fileOrDirExists(dir))
                    if(!mymkdir(dir)){
                        error="cannot create "+dir;
                        return false;
                    }

                path=directories::DumpDirectory();
                if (!fileOrDirExists(path))
                    if(!mymkdir(path)){
                        error="cannot create "+path;
                        return false;
                    }
                dir=directories::XmlDirectory();
                if (!fileOrDirExists(dir))
                    if(!mymkdir(dir)){
                        error="cannot create "+dir;
                        return false;
                    }
                if (!fileOrDirExists(directories::concatDirectoryAndFile(dir,"users.xml")))
                    if(!xmlDocument->createUsersFile()){
                        error="cannot create users.xml file";
                        return false;
                    }

                path=directories::concatDirectoryAndFile(directories::XmlDirectory(),"services.xml");
                if (!fileOrDirExists(path)){
                    bool r;
                    #ifdef WIN32
                        r= copyFile("services.xml",path);
                    #else
                        #ifdef WIN64
                            r= copyFile("services.xml",path);
                        #else
                            r= copyFile("/var/jacksms2/services.xml",path);
                        #endif
                    #endif
                    if (!r){
                        error="cannot copy "+path+" file (2nd opt)";

                    }
                    return r;
                }
                
                
                return true;

            }
        }
        bool userDirectoryManager::createUser(){
            QString path=directories::concatDirectoryAndFile(directories::XmlDirectory(),utilities::Base64Encode(user));
            if (!fileOrDirExists(path))
                if (!mymkdir(path)){
                        error="cannot create "+path;
                        return false;
                }
            userDir=utilities::Base64Encode(user);

            if(xmlDocument->createUser(user,utilities::Base64Encode(user))){
               return true;
           }else{
               error="cannot create user files";
               return false;
           }
        }

        QString userDirectoryManager::getError() const{
            return error;
        }
        serviceManager::serviceManager()
            :xmlDocument(new libJackSMS::xmlParserApi::xmlParserLocalApiTicpp("")){

        }
        bool serviceManager::saveServices(QString _xml){
            return xmlDocument->saveServices(_xml);
        }
        bool serviceManager::mergeServices(QString _xml){
            return xmlDocument->mergeServices(_xml);
        }



    }

}

