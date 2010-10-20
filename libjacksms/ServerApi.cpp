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
#include "Configuration.h"
#include "ServerApi.h"
#include "Utilities.h"
#include "netclientqhttp.h"
#include "xmlParserServerApi.h"
#include "xmlParserServerApiTicpp.h"
#include "VariabileServizio.h"
#include "LogSmsMessage.h"

#include "EncodingPercent.h"
#include "Exceptions.h"
namespace libJackSMS{

    namespace serverApi{

        login::login(const QString &_username,const QString &_password,dataTypes::proxySettings _ps ):
                username(_username),
                password(_password),
                ps(_ps),
                l(NULL){
            qRegisterMetaType<libJackSMS::dataTypes::phoneBookType>("libJackSMS::dataTypes::phoneBookType");
            qRegisterMetaType<libJackSMS::dataTypes::configuredServicesType>("libJackSMS::dataTypes::configuredServicesType");
        }
        login::~login(){
            if (l!=NULL)
                l->~loginBase();
        }

        void login::doLogin(){
            start();
        }
        void login::abort(){

        }
        void login::run(){
            emit loginStarted();
            l=new loginBase(username,password,ps);
            //connect(this,SIGNAL(abortSignal()),&man,SLOT(abort()));
            connect(l,SIGNAL(accountsReceived(libJackSMS::dataTypes::configuredServicesType)),this,SLOT(slotAccountsReceived(libJackSMS::dataTypes::configuredServicesType)));
            connect(l,SIGNAL(loginFailed(QString)),this,SLOT(slotLoginFailed(QString)));
            connect(l,SIGNAL(loginSuccess(QString)),this,SLOT(slotLoginSuccess(QString)));
            connect(l,SIGNAL(newVersionAvailable()),this,SLOT(slotNewVersionAvailable()));
            connect(l,SIGNAL(phoneBookReceived(libJackSMS::dataTypes::phoneBookType)),this,SLOT(slotPhoneBookReceived(libJackSMS::dataTypes::phoneBookType)));
            l->doLogin();

        }
        void login::slotLoginSuccess(QString loginId){
            emit this->loginSuccess(loginId);
        }
        void login::slotLoginFailed(QString error){
            emit this->loginFailed(error);
        }
        void login::slotPhoneBookReceived(libJackSMS::dataTypes::phoneBookType rubrica){
            emit this->phoneBookReceived(rubrica);

        }
        void login::slotAccountsReceived(libJackSMS::dataTypes::configuredServicesType serviziConfigurati){
            /*dataTypes::configuredAccount account;
            account.setId("0");
            account.setName("JackSMS Messenger");
            account.setService("40");
            account.setData("data1",username);
            account.setData("data2",password);
            serviziConfigurati.insert(account.getId(),account);*/
            emit this->accountsReceived(serviziConfigurati);
        }
        void login::slotNewVersionAvailable(){
            emit this->newVersionAvailable();
        }


        loginBase::loginBase(const QString &_username,const QString &_password,dataTypes::proxySettings _ps ):
                username(_username),
                password(_password),
                ps(_ps),
                xmlResponse(new xmlParserApi::xmlParserServerApiTicpp()){
        }
        loginBase::~loginBase(){
            xmlResponse->~xmlParserServerApiGeneric();
        }
      /*  void loginBase::abort(){


        }*/
        void loginBase::doLogin(){
            webClient=new netClient::netClientQHttp();
            if (ps.useProxy()){
                webClient->setProxyServer(ps.getServer(),ps.getPort(),ps.getType());
                if (ps.useAuth())
                    webClient->setProxyAuthentication(ps.getUsername(),ps.getPassword());

            }
            webClient->setUrl("http://q.jacksms.it/"+utilities::Base64Encode(username)+"/"+utilities::Base64Encode(password)+"/fullLoginJMS?xml,desktop");
            try{
                QString xml=webClient->readPage(true);
                if (xml.isEmpty())
                    emit loginFailed("Il server di login non ha risposto correttamante.");
                else if (webClient->hasError())
                    emit loginFailed("Errore: "+xml);
                else{
                    xmlResponse->setXml(xml);
                    if (xmlResponse->checkIsLogged()){
                        emit this->loginSuccess(xmlResponse->getSessionId());
                        dataTypes::phoneBookType _rubrica;
                        xmlResponse->loadPhoneBook(_rubrica);
                        emit this->phoneBookReceived(_rubrica);
                        dataTypes::configuredServicesType _serviziConfigurati;
                        xmlResponse->loadAccounts(_serviziConfigurati);
                        emit this->accountsReceived(_serviziConfigurati);
                        dataTypesApi::clientVersion v(xmlResponse->getVersion());
                        dataTypesApi::clientVersion thisVersion(JACKSMS_VERSION);
                        if (thisVersion<v)
                            emit this->newVersionAvailable();

                    }else{
                        emit loginFailed(xmlResponse->getError());
                    }
                }
            }catch(libJackSMS::exceptionXmlError e){
                emit loginFailed("Errore sconosciuto.");
            }catch(...){
                emit loginFailed("Impossibile Effettuare il Login, verificare di avere accesso alla rete.");
            }
        }



        pingator::pingator(const QString &_idsessione,dataTypes::proxySettings _ps):
                idsessione(_idsessione),
                ps(_ps),
                minutes(30){
        }
        pingator::~pingator(){
            timer.stop();
        }
        void pingator::setPingInterval(int min){
            minutes=min;

        };

        void pingator::launchPing(){
            try{
                timer.stop();
                timer.singleShot(minutes*60*1000,this,SLOT(launchPing()));
                start();
            }catch(libJackSMS::exceptionXmlError e){

            }catch(libJackSMS::exceptionSomethingWrong e){

            }catch(...){

            }
        }
        void pingator::run(){
            try{
                //xmlParserApi::xmlParserServerApiTicpp xmlResponse;

                netClient::netClientQHttp webClient;

                if (ps.useProxy()){
                    webClient.setProxyServer(ps.getServer(),ps.getPort(),ps.getType());
                    if (ps.useAuth())
                        webClient.setProxyAuthentication(ps.getUsername(),ps.getPassword());
                }
                webClient.setUrl("http://q.jacksms.it/"+idsessione+"/ping?xml,desktop");
                QString xml=webClient.readPage(true);
                emit pinged();
                //xmlDocument->setXml(xml);
                //std::sring xmlDocument->checkPing(this);
            }catch(libJackSMS::exceptionXmlError e){

            }catch(libJackSMS::exceptionSomethingWrong e){

            }catch(...){

            }
        }





        bool synchronizeVariables(dataTypes::configuredServicesType & _serviziConfigurati,const dataTypes::servicesType & _servizi){
            dataTypes::configuredServicesType::iterator i=_serviziConfigurati.begin();
            dataTypes::configuredServicesType::iterator i_end =_serviziConfigurati.end();
            for(;i!=i_end;++i){
                dataTypes::servicesType::const_iterator iter=_servizi.find(i.value().getService());
                if (iter!=_servizi.end()){
                    dataTypes::service servizio=iter.value();
                    while(servizio.nextVar()){
                        dataTypes::variabileServizio var=servizio.currentVar();

                        i.value().setData(var.getName(),i.value().getData("data"+var.getProgressive()));
                        //std::cout<<(servizio.getName()+" sostituita data"+var.getProgressive()+" con "+var.getName()+" (valore "+i.value().getData("data"+var.getProgressive())+")")<<std::endl;;
                    }
                }else{
                    return false;
                }
            }

            return true;
        }


        instantMessenger::instantMessenger(const QString &_loginId,dataTypes::proxySettings _ps ):
            loginId(_loginId),
            ps(_ps),
            webClient(new netClient::netClientQHttp()){

        }
        bool instantMessenger::checkMessages(){
            if (ps.useProxy()){
                webClient->setProxyServer(ps.getServer(),ps.getPort(),ps.getType());
                if (ps.useAuth())
                    webClient->setProxyAuthentication(ps.getUsername(),ps.getPassword());
            }
            webClient->setUrl("http://q.jacksms.it/"+loginId+"/getDeleteQueue?xml,desktop");
            QString xml=webClient->readPage(true);
            xmlParserApi::xmlParserServerApiGeneric *xmlResponse=new xmlParserApi::xmlParserServerApiTicpp();
            if (xml.isEmpty())
                return false;
            else if (webClient->hasError())
                return false;
            else{
                xmlResponse->setXml(xml);

                xmlResponse->extractImMessages(messages);
                if (messages.size()==0)
                    return false;
                else
                    return true;
            }
        }
        bool instantMessenger::getMessages(libJackSMS::dataTypes::logImType &_messages){
            _messages.clear();
            _messages=messages;
            return true;
        }



        smsLogSaver::smsLogSaver(const QString _loginId,dataTypes::proxySettings _ps ):
            loginId(_loginId),
            ps(_ps){

            qRegisterMetaType<libJackSMS::dataTypes::logSmsMessage>("libJackSMS::dataTypes::logSmsMessage");
        }
        void smsLogSaver::save(dataTypes::logSmsMessage _msg){
            msg=_msg;
            start();
        }
        void smsLogSaver::run(){
            xmlParserApi::xmlParserServerApiTicpp xmlResponse;
            netClient::netClientQHttp webClient;
            if (ps.useProxy()){
                webClient.setProxyServer(ps.getServer(),ps.getPort(),ps.getType());
                if (ps.useAuth())
                    webClient.setProxyAuthentication(ps.getUsername(),ps.getPassword());
            }
            libJackSMS::encodingPercent pEncoder;
            webClient.insertFormData("message",pEncoder.getEncodedString(msg.getText()));
            webClient.insertFormData("recipient",msg.getPhoneNumber().getIntNum());
            webClient.insertFormData("service_id",msg.getServiceId());
            webClient.insertFormData("account_id",msg.getAccountId());
            QString xml=webClient.submitPost("http://q.jacksms.it/"+loginId+"/desktopSync?xml,desktop",true);
            if (xml.isEmpty())
                emit this->smsNotSaved();
            else if (webClient.hasError())
                emit this->smsNotSaved();
            else{
                xmlResponse.setXml(xml);
                QString id;
                QString total="0";
                if(xmlResponse.checkSaved(id,total)){
                    msg.setId(id);
                    emit this->smsSaved(msg,total);
                }else
                    emit this->smsNotSaved();
            }
        }


        contactManager::contactManager(const QString & _loginId,dataTypes::proxySettings _ps )

            :loginId(_loginId),
            ps(_ps){
        }
        bool contactManager::addNewContact(libJackSMS::dataTypes::contact _contatto){
            manAdd=new contactManagerAdd(loginId,ps);
            connect(manAdd,SIGNAL(contactAdded(QString,bool)),this,SIGNAL(contactSaved(QString,bool)));
            connect(manAdd,SIGNAL(errorAdd()),this,SIGNAL(contactNotSaved()));
            manAdd->addNewContact(_contatto);

        }
        bool contactManager::updateContact(libJackSMS::dataTypes::contact  _contatto){
            manUp=new contactManagerUpdate(loginId,ps);
            connect(manUp,SIGNAL(contactUpdated(libJackSMS::dataTypes::contact)),this,SIGNAL(contactUpdated(libJackSMS::dataTypes::contact)));
            connect(manUp,SIGNAL(errorUpdate()),this,SIGNAL(contactNotUpdated()));
            manUp->updateContact(_contatto);
        }
        bool contactManager::deleteContact(QString _id){
            manDel=new contactManagerDelete(loginId,ps);
            connect(manDel,SIGNAL(contactDeleted(QString)),this,SIGNAL(contactDeleted(QString)));
            connect(manDel,SIGNAL(errorDelete()),this,SIGNAL(contactNotDeleted()));
            manDel->deleteContact(_id);
        }
        /****************************/


        void contactManagerDelete::run(){
            netClient::netClientQHttp webClient;
            libJackSMS::xmlParserApi::xmlParserServerApiTicpp xmlDocument;
            if (ps.useProxy()){
                webClient.setProxyServer(ps.getServer(),ps.getPort(),ps.getType());
                if (ps.useAuth())
                    webClient.setProxyAuthentication(ps.getUsername(),ps.getPassword());
            }
            libJackSMS::encodingPercent pEncoder;
            webClient.insertFormData("ids",pEncoder.getEncodedString(id));
            QString xml=webClient.submitPost("http://q.jacksms.it/"+loginId+"/delAbook?xml,desktop",true);
            if (xml.isEmpty())
                emit errorDelete();
            else if (webClient.hasError())
                emit errorDelete();
            else{
                xmlDocument.setXml(xml);
                if(xmlDocument.checkDeleteContact())
                    emit contactDeleted(id);
                else
                    emit errorDelete();
            }



        }

        contactManagerDelete::contactManagerDelete(const QString & _loginId,dataTypes::proxySettings _ps )
            :loginId(_loginId),
            ps(_ps)
            {


        }
        bool contactManagerDelete::deleteContact(QString _id){
            id=_id;
            start();
        }



        /******************************/
        /****************************/


        void accountManagerDelete::run(){
            netClient::netClientQHttp webClient;
            libJackSMS::xmlParserApi::xmlParserServerApiTicpp xmlDocument;
            if (ps.useProxy()){
                webClient.setProxyServer(ps.getServer(),ps.getPort(),ps.getType());
                if (ps.useAuth())
                    webClient.setProxyAuthentication(ps.getUsername(),ps.getPassword());
            }
            libJackSMS::encodingPercent pEncoder;
            webClient.insertFormData("ids",pEncoder.getEncodedString(id));
            QString xml=webClient.submitPost("http://q.jacksms.it/"+loginId+"/delService?xml",true);
            if (xml.isEmpty())
                emit errorDelete();
            else if (webClient.hasError())
                emit errorDelete();
            else{
                xmlDocument.setXml(xml);

                if(xmlDocument.checkDeleteAccount())
                    emit accountDeleted(id);
                else
                    emit errorDelete();
            }
        }

        accountManagerDelete::accountManagerDelete(const QString & _loginId,dataTypes::proxySettings _ps )
            :loginId(_loginId),
            ps(_ps)
            {


        }
        bool accountManagerDelete::deleteAccount(const QString &_id){
            id=_id;
            start();
        }



        /******************************/
        /********************************/

        void accountManagerAdd::run(){
            netClient::netClientQHttp webClient;
            libJackSMS::xmlParserApi::xmlParserServerApiTicpp xmlDocument;
            //libJackSMS::encodingPercent encoder;
            if (ps.useProxy()){
                webClient.setProxyServer(ps.getServer(),ps.getPort(),ps.getType());
                if (ps.useAuth())
                    webClient.setProxyAuthentication(ps.getUsername(),ps.getPassword());
            }
            libJackSMS::encodingPercent pEncoder;
            webClient.insertFormData("service_id",pEncoder.getEncodedString(account.getService()));
            webClient.insertFormData("account_name",pEncoder.getEncodedString(account.getName()));
            int c=0;
            while(service.nextVar()){
                   c++;
                   webClient.insertFormData("data_"+service.currentVar().getProgressive(),pEncoder.getEncodedString(account.getData(service.currentVar().getName())));
            }
            for(int x=c+1;x<5;++x){
                webClient.insertFormData("data_"+QString::number(x),"");
            }

            QString xml=webClient.submitPost("http://q.jacksms.it/"+loginId+"/addService?xml,desktop",true);
            if (xml.isEmpty())
                emit errorAdd();
            else if (webClient.hasError())
                emit errorAdd();
            else{
                xmlDocument.setXml(xml);
                QString resultId;
                if (xmlDocument.checkAddNewAccount(resultId)){
                    emit accountAdded(resultId);
                }else{
                    emit errorAdd();
                }
            }


        }


        accountManagerAdd::accountManagerAdd(const QString & _loginId,dataTypes::proxySettings _ps )
            :loginId(_loginId),
            ps(_ps)
            {


        }
        bool accountManagerAdd::addNewAccount(libJackSMS::dataTypes::service _service,libJackSMS::dataTypes::configuredAccount & _account){
            service=_service;
            account=_account;
            start();


        }


        /********************************/
        /*********************************/
        void contactManagerAdd::run(){


            libJackSMS::xmlParserApi::xmlParserServerApiTicpp xmlDocument;
            netClient::netClientQHttp webClient;

            libJackSMS::encodingPercent pEncoder;

            if (ps.useProxy()){
                webClient.setProxyServer(ps.getServer(),ps.getPort(),ps.getType());
                if (ps.useAuth())
                    webClient.setProxyAuthentication(ps.getUsername(),ps.getPassword());
            }
            webClient.insertFormData("contact_name",pEncoder.getEncodedString(contatto.getName()));
            webClient.insertFormData("contact_number",pEncoder.getEncodedString(contatto.getPhone().toString()));
            webClient.insertFormData("account_id",pEncoder.getEncodedString(contatto.getAccount()));
            webClient.insertFormData("preferred","1");
            QString xml=webClient.submitPost("http://q.jacksms.it/"+loginId+"/addAbook?xml,desktop",true);
            if (xml.isEmpty())
                emit errorAdd();
            else if (webClient.hasError())
                emit errorAdd();
            else{
                xmlDocument.setXml(xml);
                QString resultId;
                bool canReceiveJms;
                if(xmlDocument.checkAddNewContact(resultId,canReceiveJms)){
                    emit contactAdded(resultId,canReceiveJms);
                }else{
                    emit errorAdd();
                }
            }

        }
        contactManagerAdd::contactManagerAdd(const QString & _loginId,dataTypes::proxySettings _ps )
            :loginId(_loginId),
            ps(_ps){

        }
        bool contactManagerAdd::addNewContact(libJackSMS::dataTypes::contact _contatto){
            contatto=_contatto;
            start();
        }




        /***********************************/
        /*********************************/
        void contactManagerUpdate::run(){


            libJackSMS::xmlParserApi::xmlParserServerApiTicpp xmlDocument;
            netClient::netClientQHttp webClient;

            libJackSMS::encodingPercent pEncoder;
            webClient.insertFormData("id",contatto.getId());
            webClient.insertFormData("contact_name",pEncoder.getEncodedString(contatto.getName()));
            webClient.insertFormData("contact_number",pEncoder.getEncodedString(contatto.getPhone().toString()));
            webClient.insertFormData("account_id",contatto.getAccount());
            QString xml=webClient.submitPost("http://q.jacksms.it/"+loginId+"/editAbook?xml,desktop",true);
            if (xml.isEmpty())
                emit errorUpdate();
            else if (webClient.hasError())
                emit errorUpdate();
            else{
                xmlDocument.setXml(xml);

                if(xmlDocument.checkUpdateContact()){
                    emit this->contactUpdated(contatto);
                }else{
                    emit this->errorUpdate();
                }
            }
        }
        contactManagerUpdate::contactManagerUpdate(const QString & _loginId,dataTypes::proxySettings _ps )
            :loginId(_loginId),
            ps(_ps){


            qRegisterMetaType<libJackSMS::dataTypes::contact>("libJackSMS::dataTypes::contact");
        }
        bool contactManagerUpdate::updateContact(libJackSMS::dataTypes::contact _contatto){
            contatto=_contatto;
            start();
        }



        /*********************************/
        void accountManagerUpdate::run(){


            libJackSMS::xmlParserApi::xmlParserServerApiTicpp xmlDocument;
            netClient::netClientQHttp webClient;

            libJackSMS::encodingPercent pEncoder;
            webClient.insertFormData("id",account.getId());
            webClient.insertFormData("account_name",pEncoder.getEncodedString(account.getName()));
            int i=0;
            while(s.nextVar()){
                i++;
                libJackSMS::dataTypes::variabileServizio var=s.currentVar();
                QString prog=var.getProgressive();
                QString name=var.getName();
                webClient.insertFormData("data_"+prog,pEncoder.getEncodedString(account.getData(name)));
            }
            if (i<4)
                for(int x=i+1;x<=4;++x)
                    webClient.insertFormData("data_"+QString::number(x),"");
            QString xml=webClient.submitPost("http://q.jacksms.it/"+loginId+"/editService?xml,desktop",true);
            if (xml.isEmpty())
                emit errorUpdate();
            else if (webClient.hasError())
                emit errorUpdate();
            else{
                xmlDocument.setXml(xml);

                if(xmlDocument.checkUpdateAccount()){
                    emit this->accountUpdated(account);
                }else{
                    emit this->errorUpdate();
                }
            }

        }
        accountManagerUpdate::accountManagerUpdate(const QString & _loginId,libJackSMS::dataTypes::service _s,dataTypes::proxySettings _ps )
            :loginId(_loginId),
            s(_s),
            ps(_ps){
            qRegisterMetaType<libJackSMS::dataTypes::configuredAccount>("libJackSMS::dataTypes::configuredAccount");
        }
        bool accountManagerUpdate::updateAccount(libJackSMS::dataTypes::configuredAccount _account){
            account=_account;
            start();
        }
        /***********************************/
        accountManager::accountManager(const QString & _loginId,dataTypes::proxySettings _ps )

            :loginId(_loginId),

            ps(_ps){
        }

        bool accountManager::addNewAccount(libJackSMS::dataTypes::service _service,libJackSMS::dataTypes::configuredAccount & _account){
            manAdd=new accountManagerAdd(loginId,ps);
            connect(manAdd,SIGNAL(accountAdded(QString)),this,SIGNAL(accountSaved(QString)));
            connect(manAdd,SIGNAL(errorAdd()),this,SIGNAL(accountNotSaved()));
            manAdd->addNewAccount(_service,_account);




        }
        bool accountManager::updateAccount(libJackSMS::dataTypes::configuredAccount & _account,libJackSMS::dataTypes::service s){

            manUp=new accountManagerUpdate(loginId,s,ps);
            connect(manUp,SIGNAL(accountUpdated(libJackSMS::dataTypes::configuredAccount)),this,SIGNAL(accountUpdated(libJackSMS::dataTypes::configuredAccount)));
            connect(manUp,SIGNAL(errorUpdate()),this,SIGNAL(accountNotUpdated()));
            manUp->updateAccount(_account);
        }
        bool accountManager::deleteAccount(QString _id){
            manDel=new accountManagerDelete(loginId,ps);
            connect(manDel,SIGNAL(accountDeleted(QString)),this,SIGNAL(accountDeleted(QString)));
            connect(manDel,SIGNAL(errorDelete()),this,SIGNAL(accountNotDeleted()));
            manDel->deleteAccount(_id);

        }

        reloader::reloader(const QString & _loginId,dataTypes::proxySettings _ps )
            :xmlDocument(new libJackSMS::xmlParserApi::xmlParserServerApiTicpp()),
            loginId(_loginId),
            webClient(new netClient::netClientQHttp()),
            ps(_ps){
            }
        bool reloader::reloadPhoneBook(libJackSMS::dataTypes::phoneBookType & _rubrica){
            if (ps.useProxy()){
                webClient->setProxyServer(ps.getServer(),ps.getPort(),ps.getType());
                if (ps.useAuth())
                    webClient->setProxyAuthentication(ps.getUsername(),ps.getPassword());
            }
            webClient->setUrl("http://q.jacksms.it/"+loginId+"/getAbookFull?xml");
            QString xml=webClient->readPage(true);
            xmlDocument->setXml(xml);
            return xmlDocument->loadPhoneBook2(_rubrica);
        }

        conversationManager::conversationManager(const QString & _loginId,dataTypes::proxySettings _ps )
            :xmlDocument(new libJackSMS::xmlParserApi::xmlParserServerApiTicpp()),
            loginId(_loginId),
            webClient(new netClient::netClientQHttp()),
            ps(_ps){
        }
        bool conversationManager::downloadLastMessages(libJackSMS::dataTypes::logSmsType & _logSms,libJackSMS::dataTypes::logImType & _logIm){
            if (ps.useProxy()){
                webClient->setProxyServer(ps.getServer(),ps.getPort(),ps.getType());
                if (ps.useAuth())
                    webClient->setProxyAuthentication(ps.getUsername(),ps.getPassword());
            }
            webClient->setUrl("http://q.jacksms.it/"+loginId+"/getConversation?xml");
            QString xml=webClient->readPage(true);

            xmlDocument->setXml(xml);
            return xmlDocument->parseConversation(_logSms,_logIm);
        }



        void updateServicesManager::run(){

            try{
                updateServicesManagerBase man(loginId,ps);
                connect(this,SIGNAL(abortSignal()),&man,SLOT(abort()));

                if (man.downloadUpdates(servizi)){

                    if (!aborted)
                    emit updatesAvailable(servizi,man.getXml(),man.getMessage());
                }
            }catch(libJackSMS::exceptionXmlError e){
                emit criticalError(e.what());

            }catch(...){
                emit criticalError("error too critical: section 2");
            }
        }

        updateServicesManager::updateServicesManager(const QString & _loginId,dataTypes::proxySettings _ps,libJackSMS::dataTypes::servicesType _servizi):
                loginId(_loginId),
                ps(_ps),
                servizi(_servizi),
                aborted(false){


            qRegisterMetaType<libJackSMS::dataTypes::servicesType>("libJackSMS::dataTypes::servicesType");

        }

        bool updateServicesManager::checkUpdadates(){
            start();
        }

        void updateServicesManager::abort(){
            emit abortSignal();
            aborted=true;
        }




        updateServicesManagerBase::updateServicesManagerBase(const QString & _loginId,dataTypes::proxySettings _ps )
            :xmlDocument(new libJackSMS::xmlParserApi::xmlParserServerApiTicpp()),
            loginId(_loginId),
            webClient(new netClient::netClientQHttp()),
            ps(_ps),
            aborted(false){
        }
        QString updateServicesManagerBase::getMessage() const{
            QList<QPair<QString,QString> >::const_iterator i=updateResults.begin();
            QList<QPair<QString,QString> >::const_iterator i_end=updateResults.end();
            QString resUp;
            QString resAdd;
            for(;i!=i_end;++i){
                if (i->first=="a"){
                    if (resAdd.isEmpty())
                        resAdd=i->second;
                    else
                        resAdd=resAdd+", "+i->second;
                }else{
                    if (resUp.isEmpty())
                        resUp=i->second;
                    else
                        resUp=resUp+", "+i->second;
                }
            }
            QString final;
            if (!resAdd.isEmpty())
                final="Aggiunti i seguenti servizi:\n"+resAdd+"\n\n";
            if (!resUp.isEmpty())
                final=final+"Aggiornati i seguenti servizi:\n"+resUp;
            return final;
        }
        QString updateServicesManagerBase::getXml() const{
            return servXml;
        }
        void updateServicesManagerBase::abort(){
            aborted=true;
            webClient->interrupt();

        }
        bool updateServicesManagerBase::downloadUpdates(libJackSMS::dataTypes::servicesType  _servizi){
            if (ps.useProxy()){
                webClient->setProxyServer(ps.getServer(),ps.getPort(),ps.getType());
                if (ps.useAuth())
                    webClient->setProxyAuthentication(ps.getUsername(),ps.getPassword());
            }
            bool result=false;
            {
                libJackSMS::dataTypes::servicesType::iterator i=_servizi.begin();
                libJackSMS::dataTypes::servicesType::iterator i_end=_servizi.end();
                for(;i!=i_end;++i){
                    webClient->insertFormData(i.value().getId(),i.value().getVersion());
                }
            }
            QString xml=webClient->submitPost("http://q.jacksms.it/"+loginId+"/servicesFullXML",true);
            if (xml.isEmpty()){

            }else if (webClient->hasError()){

            }else{


                if (!aborted){
                    xmlDocument->setXml(xml);
                    servXml=xml;
                    libJackSMS::dataTypes::servicesType  nuoviServizi;
                    xmlDocument->parseServices(nuoviServizi);
                    {
                        libJackSMS::dataTypes::servicesType::iterator i=nuoviServizi.begin();
                        libJackSMS::dataTypes::servicesType::iterator i_end=nuoviServizi.end();
                        for(;i!=i_end;++i){
                            libJackSMS::dataTypes::servicesType::iterator x=_servizi.find(i.value().getId());
                            if (x==_servizi.end()){
                                result=true;
                                _servizi.insert(i.value().getId(),i.value());
                                //std::cout <<"aggiunto "<<i.value().getName()+" (v"+i.value().getVersion()+")";
                                updateResults.push_back(qMakePair(QString("a"),i.value().getName()+" (v"+i.value().getVersion()+")"));
                            }else{
                                if (i.value().getVersion()!=x.value().getVersion()){
                                    //std::cout <<"aggiornato "<<i.value().getName()+" (v"+i.value().getVersion()+"->v"+x.value().getVersion()+")";
                                    updateResults.push_back(qMakePair(QString("u"),i.value().getName()));
                                    x.value()=i.value();
                                    result=true;

                                }
                            }
                        }
                    }

                    return result;

                }
            }
        }









        /***********************************permanent instant messenger******************/
        permanentInstantMessenger::permanentInstantMessenger(QString _username,QString _password,dataTypes::proxySettings _ps ):
                username(_username),
                password(_password),
                ps(_ps)
        {


            qRegisterMetaType<libJackSMS::dataTypes::dateTime>("libJackSMS::dataTypes::dateTime");
            qRegisterMetaType<libJackSMS::dataTypes::logImMessage>("libJackSMS::dataTypes::logImMessage");
            qRegisterMetaType<QAbstractSocket::SocketError>("QAbstractSocket::SocketError");
            connect(&signalCountdown,SIGNAL(timeout()),this,SLOT(launchSignal()));
            connect(&sock,SIGNAL(disconnected()),this,SLOT(relaunchDisconnected()));
            connect(&sock,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(errorDisconnected(QAbstractSocket::SocketError)));
            connect(&sock,SIGNAL(readyRead()),this,SLOT(dataReceived()));
            connect(&sock,SIGNAL(connected()),this,SLOT(connectDone()));
            connect(&sock,SIGNAL(aboutToClose()),this,SLOT(relaunchDisconnected()));
            connect(&sock,SIGNAL(stateChanged(QAbstractSocket::SocketState)),this,SLOT(state(QAbstractSocket::SocketState)));
            connect(&pingTimer,SIGNAL(timeout()),this,SLOT(ping()));
            connect(&pingTimeout,SIGNAL(timeout()),this,SLOT(pingTimeoutError()));
            pingTimeout.setSingleShot(true);
            connect(&reconnectTimer,SIGNAL(timeout()),this,SLOT(tryReconnect()));
            reconnectTimer.setSingleShot(true);
            //connect(jmsgr,SIGNAL(newJms(libJackSMS::dataTypes::logImMessage)),this,SLOT(newMessage(libJackSMS::dataTypes::logImMessage)));

        }
        void permanentInstantMessenger::relaunchDisconnected() {
            emit serviceNotActive(false);
        }
        void permanentInstantMessenger::tryReconnect() {
            activateServ();
        }
        void permanentInstantMessenger::errorDisconnected(QAbstractSocket::SocketError) {
            emit serviceNotActive(true,sock.errorString());
        }
        void permanentInstantMessenger::state(QAbstractSocket::SocketState){

        }
        void permanentInstantMessenger::pingTimeoutError(){

            try{
                sock.abort();
                pingTimer.stop();
                pingTimeout.stop();
                reconnectTimer.start(3*60*1000);
                emit serviceNotActive(true,"Timeout");
            }catch(...){
                emit serviceNotActive(true,"Errore sconosciuto: section 3");

            }
        }


        void permanentInstantMessenger::stop(){
            sock.write("E\r\n");
            sock.disconnectFromHost();
            pingTimer.stop();
            pingTimeout.stop();
            sock.abort();
        }
        void permanentInstantMessenger::ping(){
            if (sock.write("\r\n")!=-1)
                pingTimeout.start(10*1000);
            else
                emit serviceNotActive(true,"Ping failed");
        }
        void permanentInstantMessenger::dataReceived(){
            dataTimeout.stop();
            this->buffer=this->buffer.append(sock.readAll());
            dataTimeout.singleShot(500,this,SLOT(parseLine()));

        }
        void  permanentInstantMessenger::parseLine(){

            try{
                QList<QByteArray> finalLines;

                QList<QByteArray> lines=buffer.split('\0');
                for (QList<QByteArray>::iterator i=lines.begin();i!=lines.end();++i){
                    QList<QByteArray> ll=i->split('\n');
                    finalLines.append(ll);
                }
                buffer.clear();
                QRegExp r;

                for (QList<QByteArray>::iterator i=finalLines.begin();i!=finalLines.end();++i){
                    QString line=QString(finalLines.first());
                    finalLines.removeFirst();
                    if (line.isEmpty()){
                        pingTimeout.stop();
                    }else if (line.startsWith("W")){
                        emit serviceActive();
                    }else{
                        r.setPattern("^R ([^\\t]+) \\(([^)]+)\\)\\t([^\\t]+)$");
                        if (r.exactMatch(line)){
                            libJackSMS::dataTypes::phoneNumber num;
                            num.parse(r.cap(1));
                            num.setAltName(r.cap(2));
                            libJackSMS::dataTypes::dateTime dat(libJackSMS::dataTypes::dateTime::getCurrentDay(),libJackSMS::dataTypes::dateTime::getCurrentHour());

                            libJackSMS::dataTypes::logImMessage im(num,dat,"",QString::fromUtf8(r.cap(3).toAscii(),r.cap(3).length()));

                            signalCountdown.stop();
                            id++;
                            im.setId(QString::number(id));
                            imLog.insert(im.getId(),im);
                            signalCountdown.start(1500);

                        }else{
                            r.setPattern("^R\\t([^ ]+) \\(([^)]+)\\)\\t([0-9]+)/([0-9]+) ([0-9]+):([0-9]+) - (.+)$");
                            if (r.exactMatch(line)){
                                QString a=r.cap(1);
                                QString b=r.cap(2);
                                QString c=r.cap(3);
                                QString d=r.cap(4);
                                QString e=r.cap(5);
                                QString f=r.cap(6);
                                QString g=QString::fromUtf8(r.cap(7).toAscii(),r.cap(7).length());
                                libJackSMS::dataTypes::phoneNumber num;
                                num.parse(a);
                                num.setAltName(b);
                                QDateTime dd(QDateTime::fromString(c+"/"+d+" "+e+":"+f,"dd/MM HH:mm"));
                                bool ok;
                                int current_year=QDateTime::currentDateTime().toString("yyyy").toInt(&ok,10);
                                dd=dd.addYears(current_year-1900);
                                libJackSMS::dataTypes::dateTime dat(dd.toString("dd/MM/yyyy HH:mm:ss"));
                                libJackSMS::dataTypes::logImMessage im(num,dat,"",g);
                                signalCountdown.stop();
                                id++;
                                im.setId(QString::number(id));
                                imLog.insert(im.getId(),im);
                                signalCountdown.start(1500);
                            }


                        }


                    }

                }
            }catch (...){
                emit serviceNotActive(true,"Unknown error");
            }


        }
        permanentInstantMessenger::~permanentInstantMessenger(){
            if (sock.state()!=QAbstractSocket::UnconnectedState)
                sock.waitForDisconnected(10000);


        }


        void permanentInstantMessenger::connectDone(){
            try{
                QString sn=QString("ID ")+username+QString("\t")+password+QString("\t5551\r\n");
                sock.write(sn.toAscii());
                pingTimer.start(1000*60);
            }catch(...){
                emit serviceNotActive(true,"Errore sconosciuto: section 6");

            }
        }
        void permanentInstantMessenger::launchSignal(){
            try{
                signalCountdown.stop();
                emit newJMS(imLog);
                imLog.clear();
                id=0;
            }catch(...){
                emit serviceNotActive(true,"Errore sconosciuto: section 5");

            }
        }

        void permanentInstantMessenger::activateServ(){
            try{
                id=0;
                reconnectTimer.stop();
                emit serviceActiving();
                if (ps.useProxy()){
                    proxy.setHostName(ps.getServer());
                    bool ok;
                    proxy.setPort(ps.getPort().toInt(&ok,10));
                    if (ps.getType().toUpper()=="HTTP")
                        proxy.setType(QNetworkProxy::HttpProxy);
                    else if (ps.getType().toUpper()=="SOCKS5")
                        proxy.setType(QNetworkProxy::Socks5Proxy);
                    if (ps.useAuth()){
                        proxy.setUser(ps.getUsername());
                        proxy.setPassword(ps.getPassword());
                    }
                    sock.setProxy(proxy);

                }
                sock.connectToHost("www.jacksms.it",5551);
            }catch(...){
                emit serviceNotActive(true,"Errore sconosciuto: section 4");

            }


        }


    }

}
