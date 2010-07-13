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
        login::login(const QString &_username,const QString &_password,dataTypes::proxySettings _ps ):username(_username),password(_password),ps(_ps){
            xmlResponse=new xmlParserApi::xmlParserServerApiTicpp();
        }
        bool login::doLogin(){
            webClient=new netClient::netClientQHttp();
            if (ps.useProxy()){
                webClient->setProxyServer(ps.getServer(),ps.getPort(),ps.getType());
                if (ps.useAuth())
                    webClient->setProxyAuthentication(ps.getUsername(),ps.getPassword());

            }
            webClient->setUrl("http://q.jacksms.it/"+utilities::Base64Encode(username)+"/"+utilities::Base64Encode(password)+"/fullLogin?xml,desktop");
            QString xml=webClient->readPage(true);
            if (xml.isEmpty()){
                error=QString("Il server di login non ha risposto correttamante.");
                return false;
            }else{
                xmlResponse->setXml(xml);
                if (xmlResponse->checkIsLogged()){
                    idsessione=xmlResponse->getSessionId();
                    return true;
                }else{
                    return false;
                }


            }
        }

        QString login::getLoginError() const{
            try{
                QString errore=xmlResponse->getError();
                return errore;
            }catch(libJackSMS::exceptionXmlError e){
                return "Errore sconosciuto.";
            }

        }
        QString login::getSessionId(){
            return idsessione;
            //return xmlResponse->getSessionId();
        }
        libJackSMS::dataTypesApi::clientVersion login::getVersion() const{
            QString version=xmlResponse->getVersion();
            dataTypesApi::clientVersion v(version);
            return v;
        }
        bool login::retreivePhoneBook(dataTypes::phoneBookType & _rubrica){
            return xmlResponse->loadPhoneBook(_rubrica);
        }

        bool login::retreiveAccounts(dataTypes::configuredServicesType & _serviziConfigurati){
            return xmlResponse->loadAccounts(_serviziConfigurati);
        }



        pingator::pingator(const QString &_idsessione,dataTypes::proxySettings _ps):idsessione(_idsessione),xmlResponse(new xmlParserApi::xmlParserServerApiTicpp()),ps(_ps){
        }
        bool pingator::ping(){
            webClient=new netClient::netClientQHttp();
            if (ps.useProxy()){
                webClient->setProxyServer(ps.getServer(),ps.getPort(),ps.getType());
                if (ps.useAuth())
                    webClient->setProxyAuthentication(ps.getUsername(),ps.getPassword());
            }
            webClient->setUrl("http://q.jacksms.it/"+idsessione+"/ping?xml,desktop");
            QString xml=webClient->readPage(true);
            //xmlDocument->setXml(xml);
            //std::sring xmlDocument->checkPing(this);
            return true;
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
            xmlResponse->setXml(xml);

            xmlResponse->extractImMessages(messages);
            if (messages.size()==0)
                return false;
            else
                return true;
        }
        bool instantMessenger::getMessages(libJackSMS::dataTypes::logImType &_messages){
            _messages.clear();
            _messages=messages;
            return true;
        }

        permanentJMessenger::permanentJMessenger(const QString & _username,const QString & _password,dataTypes::proxySettings _ps):

                username(_username),
                password(_password),
                ps(_ps),
                sock(new netClient::SocketClient(this)){

            }
        void permanentJMessenger::deActivatePermanentCheck(){
            sock->writeLine("E");
            sock->abort();

        }
        permanentJMessenger::~permanentJMessenger(){
            sock->writeLine("E");
            sock->abort();

        }
        bool permanentJMessenger::activatePermanentCheck(){

            if (ps.useProxy()){
                sock->setProxyServer(ps.getServer(),ps.getPort(),ps.getType());
                if (ps.useAuth())
                    sock->setProxyAuthentication(ps.getUsername(),ps.getPassword());
            }
            sock->setHost("www.jacksms.it","5551");
            if (!sock->connectToServer()){
                return false;
            }
            sock->writeLine("ID "+username+"\t"+password+"\t5551");
            QRegExp r;


            while(false||!false){ //that's the question...
               QString line=sock->readLine();
               if (line.isEmpty()){

               }else if (line.startsWith("W")){

               }else{
                   r.setPattern("^R ([^\\t]){1,} \\(([^)]+)\\)\\t([^\\t]{1,})$");
                   if (r.exactMatch(line)){
                       dataTypes::phoneNumber num;
                       num.parse(r.cap(1));
                       num.setAltName(r.cap(2));
                       dataTypes::dateTime dat(dataTypes::dateTime::getCurrentDay(),dataTypes::dateTime::getCurrentHour());

                       dataTypes::logImMessage im(num,dat,"",QString::fromUtf8(r.cap(3).toAscii(),r.cap(3).length()));
                       emit newJms(im);
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
                           dataTypes::phoneNumber num;
                           num.parse(a);
                           num.setAltName(b);
                           QDateTime dd(QDateTime::fromString(c+"/"+d+" "+e+":"+f,"dd/MM HH:mm"));
                           bool ok;
                           int current_year=QDateTime::currentDateTime().toString("yyyy").toInt(&ok,10);
                           dd.addYears(current_year-1900);
                           dataTypes::dateTime dat(dd.toString("dd/MM/yyyy HH:mm:ss"));
                           dataTypes::logImMessage im(num,dat,"",g);
                           emit newJms(im);

                       }


                   }


               }



            }
        }


        smsLogSaver::smsLogSaver(const QString _loginId,dataTypes::proxySettings _ps ):
            loginId(_loginId),
            xmlResponse(new xmlParserApi::xmlParserServerApiTicpp()),
            webClient(new netClient::netClientQHttp()),
            ps(_ps){

        }
        void smsLogSaver::setMessage(const dataTypes::logSmsMessage &_msg){
            msg=_msg;
        }
        bool smsLogSaver::save(QString &_id){
/*
            libJackSMS::encodingPercent pEncoder;
            if (ps.useProxy()){
                webClient->setProxyServer(ps.getServer(),ps.getPort(),ps.getType());
                if (ps.useAuth())
                    webClient->setProxyAuthentication(ps.getUsername(),ps.getPassword());
            }
            webClient->insertFormData("message",pEncoder.getEncodedString((msg.getText())));
            webClient->insertFormData("recipient",pEncoder.getEncodedString(msg.getPhoneNumber().getIntNum()));
            webClient->insertFormData("service_id",msg.getServiceId());
            QString xml=webClient->submitPost("http://q.jacksms.it/"+loginId+"/desktopSync?xml,desktop",true);

            xmlResponse->setXml(xml);
            return xmlResponse->checkSaved(_id);
*/

            if (ps.useProxy()){
                webClient->setProxyServer(ps.getServer(),ps.getPort(),ps.getType());
                if (ps.useAuth())
                    webClient->setProxyAuthentication(ps.getUsername(),ps.getPassword());
            }
            libJackSMS::encodingPercent pEncoder;
            webClient->insertFormData("message",pEncoder.getEncodedString(msg.getText()));
            webClient->insertFormData("recipient",msg.getPhoneNumber().getIntNum());
            webClient->insertFormData("service_id",msg.getServiceId());
            QString xml=webClient->submitPost("http://q.jacksms.it/"+loginId+"/desktopSync?xml,desktop",true);

            xmlResponse->setXml(xml);
            return xmlResponse->checkSaved(_id);

        }

        contactManager::contactManager(const QString & _loginId,dataTypes::proxySettings _ps )
            :xmlDocument(new libJackSMS::xmlParserApi::xmlParserServerApiTicpp()),
            loginId(_loginId),
            webClient(new netClient::netClientQHttp()),
            ps(_ps){
        }
        bool contactManager::addNewContact(libJackSMS::dataTypes::contact & _contatto){

            libJackSMS::encodingPercent pEncoder;

            if (ps.useProxy()){
                webClient->setProxyServer(ps.getServer(),ps.getPort(),ps.getType());
                if (ps.useAuth())
                    webClient->setProxyAuthentication(ps.getUsername(),ps.getPassword());
            }
            webClient->insertFormData("contact_name",pEncoder.getEncodedString(_contatto.getName()));
            webClient->insertFormData("contact_number",pEncoder.getEncodedString(_contatto.getPhone().toString()));
            webClient->insertFormData("account_id",pEncoder.getEncodedString(_contatto.getAccount()));
            webClient->insertFormData("preferred","1");
            QString xml=webClient->submitPost("http://q.jacksms.it/"+loginId+"/addAbook?xml,desktop",true);
            xmlDocument->setXml(xml);
            return xmlDocument->checkAddNewContact(resultId);

        }
        QString contactManager::getResultId() const{
            return resultId;
        }
        bool contactManager::updateContact(libJackSMS::dataTypes::contact & _contatto){
            libJackSMS::encodingPercent pEncoder;
            webClient->insertFormData("id",_contatto.getId());
            webClient->insertFormData("contact_name",pEncoder.getEncodedString(_contatto.getName()));
            webClient->insertFormData("contact_number",pEncoder.getEncodedString(_contatto.getPhone().toString()));
            webClient->insertFormData("account_id",_contatto.getAccount());
            QString xml=webClient->submitPost("http://q.jacksms.it/"+loginId+"/modifyAbook?xml,desktop",true);
            xmlDocument->setXml(xml);
            return xmlDocument->checkUpdateContact();
        }
        bool contactManager::deleteContact(const QString &_id){
            if (ps.useProxy()){
                webClient->setProxyServer(ps.getServer(),ps.getPort(),ps.getType());
                if (ps.useAuth())
                    webClient->setProxyAuthentication(ps.getUsername(),ps.getPassword());
            }
            libJackSMS::encodingPercent pEncoder;
            webClient->insertFormData("ids",pEncoder.getEncodedString(_id));
            QString xml=webClient->submitPost("http://q.jacksms.it/"+loginId+"/delAbook?xml,desktop",true);
            //std::cout <<xml;
            //xmlResponse->setXml(xml);
            return xmlDocument->checkDeleteContact();
        }
        accountManager::accountManager(const QString & _loginId,dataTypes::proxySettings _ps )
            :xmlDocument(new libJackSMS::xmlParserApi::xmlParserServerApiTicpp()),
            loginId(_loginId),
            webClient(new netClient::netClientQHttp()),
            ps(_ps){
        }
        QString accountManager::getResultId() const{
            return resultId;
        }
        bool accountManager::addNewAccount(libJackSMS::dataTypes::service _service,libJackSMS::dataTypes::configuredAccount & _account){
            //libJackSMS::encodingPercent encoder;
            if (ps.useProxy()){
                webClient->setProxyServer(ps.getServer(),ps.getPort(),ps.getType());
                if (ps.useAuth())
                    webClient->setProxyAuthentication(ps.getUsername(),ps.getPassword());
            }
            libJackSMS::encodingPercent pEncoder;
            webClient->insertFormData("service_id",pEncoder.getEncodedString(_account.getService()));
            webClient->insertFormData("account_name",pEncoder.getEncodedString(_account.getName()));
            int c=0;
            while(_service.nextVar()){
                   c++;
                   webClient->insertFormData("data_"+_service.currentVar().getProgressive(),pEncoder.getEncodedString(_account.getData(_service.currentVar().getName())));
            }
            for(int x=c+1;x<5;++x){
                webClient->insertFormData("data_"+QString::number(x),"");
            }

            QString xml=webClient->submitPost("http://q.jacksms.it/"+loginId+"/addService?xml,desktop",true);
            xmlDocument->setXml(xml);
            return xmlDocument->checkAddNewAccount(resultId);

        }
        bool accountManager::updateAccount(libJackSMS::dataTypes::configuredAccount & _account){
            /*
            webClient->insertFormData("XXXX",_contatto.getCampo());
            QString xml=webClient->submitPost("http://q.jacksms.it/"+loginId+"/desktopSync?xml,desktop",true);
            xmlResponse->setXml(xml);
            */
            return xmlDocument->checkUpdateAccount();
        }
        bool accountManager::deleteAccount(const QString &_id){
            if (ps.useProxy()){
                webClient->setProxyServer(ps.getServer(),ps.getPort(),ps.getType());
                if (ps.useAuth())
                    webClient->setProxyAuthentication(ps.getUsername(),ps.getPassword());
            }
            libJackSMS::encodingPercent pEncoder;
            webClient->insertFormData("ids",pEncoder.getEncodedString(_id));
            QString xml=webClient->submitPost("http://q.jacksms.it/"+loginId+"/delService?xml",true);
            //std::cout <<xml;
            //xmlResponse->setXml(xml);

            return xmlDocument->checkDeleteAccount();
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


        updateServicesManager::updateServicesManager(const QString & _loginId,dataTypes::proxySettings _ps )
            :xmlDocument(new libJackSMS::xmlParserApi::xmlParserServerApiTicpp()),
            loginId(_loginId),
            webClient(new netClient::netClientQHttp()),
            ps(_ps){
        }
        QString updateServicesManager::getMessage() const{
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
        QString updateServicesManager::getXml() const{
            return servXml;
        }
        bool updateServicesManager::downloadUpdates(libJackSMS::dataTypes::servicesType & _servizi){
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
