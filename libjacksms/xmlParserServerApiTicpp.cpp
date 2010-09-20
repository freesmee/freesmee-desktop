
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

#include "ticpp.h"
#include "ticpprc.h"
#include "DataTypes.h"
#include "xmlParserServerApiTicpp.h"
#include "Utilities.h"
#include "Exceptions.h"
#include "Contact.h"
#include <algorithm>
#include <string>

namespace libJackSMS{

    namespace xmlParserApi{
        xmlParserServerApiTicpp::xmlParserServerApiTicpp(){
        }
        xmlParserServerApiTicpp::~xmlParserServerApiTicpp(){
        }

        void xmlParserServerApiTicpp::setXml(QString _xml){

            _xml=_xml.replace("&egrave;","&#232;");
            _xml=_xml.replace("&eacute;","&#233;");
            _xml=_xml.replace("&ograve;","&#242;");
            _xml=_xml.replace("&agrave;","&#224;");
            _xml=_xml.replace("&ugrave;","&#249;");
            _xml=_xml.replace("&igrave;","&#236;");
            _xml=_xml.replace("\\\"","&#34;");
            _xml=_xml.replace("&quot;","&#34;");
            _xml=_xml.replace("&euro;","&#x20ac;");
            xmlResponse.Parse(_xml.toStdString());

        }
        bool xmlParserServerApiTicpp::checkIsLogged(){
            try{
                ticpp::Node * subRoot=xmlResponse.FirstChild("JackSMS");
                ticpp::Node * sessIdNode=subRoot->FirstChild("session_id",false);
                if (sessIdNode==NULL)
                    return false;
                else
                    return true;
            }catch(ticpp::Exception e){
                throw libJackSMS::exceptionXmlError(e.what());
            }
            return false;
        }
        QString xmlParserServerApiTicpp::getVersion(){
             try{
                 ticpp::Node * subRoot=xmlResponse.FirstChild("JackSMS");
                 ticpp::Node * verIdNode=subRoot->FirstChild("version_id",false);
                 if (verIdNode==NULL)
                     return "";
                 else{
                     return QString::fromStdString(verIdNode->ToElement()->GetText());
                 }
             }catch(ticpp::Exception e){
                 throw libJackSMS::exceptionXmlError(e.what());
             }
             return false;
        }
        QString xmlParserServerApiTicpp::getSessionId(){
            try{
                ticpp::Node * subRoot=xmlResponse.FirstChild("JackSMS");
                ticpp::Node * sessIdNode=subRoot->FirstChild("session_id",false);
                if (sessIdNode==NULL)
                    return "";
                else{
                    return QString::fromStdString(sessIdNode->ToElement()->GetText());
                }
            }catch(ticpp::Exception e){
                throw libJackSMS::exceptionXmlError(e.what());
            }
            return false;
        }
        QString xmlParserServerApiTicpp::getError(){
            try{
                ticpp::Node * subRoot=xmlResponse.FirstChild("JackSMS");
                ticpp::Node * errNode=subRoot->FirstChild("error",false);
                if (errNode==NULL)
                    return "";
                else{
                    return QString::fromStdString(errNode->ToElement()->GetText());
                }
            }catch(ticpp::Exception e){
                throw libJackSMS::exceptionXmlError(e.what());
            }
            return false;
        }
        bool xmlParserServerApiTicpp::loadErrors(dataTypesApi::errorsType & _errors){
            /*try{
                ticpp::Node * subRoot=xmlResponse.FirstChild("JackSMS");
                unsigned int codice=1;
                bool last=false;
                while(!last){

                    ticpp::Node * errNode=subRoot->FirstChild(("e"+utilities::toString(codice)),false);
                    if (errNode==NULL){
                        last=true;

                    }else{

                        _errors.insert(utilities::toString(codice),errNode->ToElement()->GetText()));
                        codice++;
                    }
                }
            }catch(ticpp::Exception e){
                throw libJackSMS::exceptionXmlError(e.what());
            }*/
            return true;
        }
        void xmlParserServerApiTicpp::loadPhoneBookBase(libJackSMS::dataTypes::phoneBookType & _rubrica,ticpp::Node *root){
            ticpp::Node *child=NULL;
            while( child = root->IterateChildren( child ) ){
                ticpp::Element * curElem= child->ToElement();

                std::string tmp=curElem->GetAttribute("name");
                QString name=QString::fromUtf8(tmp.c_str(),tmp.length());

                tmp=curElem->GetAttribute("number");
                dataTypes::phoneNumber num;
                num.parse(QString::fromUtf8(tmp.c_str(),tmp.length()));

                tmp=curElem->GetAttribute("service");
                QString srv=QString::fromUtf8(tmp.c_str(),tmp.length());

                dataTypes::contact contatto(name,num,"Contatti", srv);
                contatto.setId(QString::fromStdString(curElem->GetAttribute("id")));
                if (curElem->GetAttributeOrDefault("jms","0")=="1")
                    contatto.setCanReceiveJms(true);

                _rubrica.insert(contatto.getId(),contatto);
            }


        }
        bool xmlParserServerApiTicpp::loadPhoneBook2(libJackSMS::dataTypes::phoneBookType & _rubrica){
            /*try{
                ticpp::Node * subRoot=xmlResponse.FirstChild("JackSMS");
                loadPhoneBookBase(_rubrica,subRoot);
            }catch(ticpp::Exception e){
                throw libJackSMS::exceptionXmlError(e.what());
            }*/
            return true;
        }
        bool xmlParserServerApiTicpp::loadPhoneBook(libJackSMS::dataTypes::phoneBookType & _rubrica){
            try{
                ticpp::Node * subRoot=xmlResponse.FirstChild("JackSMS");
                ticpp::Node * pbNode=subRoot->FirstChild("AddressBook");
                loadPhoneBookBase(_rubrica,pbNode);
            }catch(ticpp::Exception e){
                throw libJackSMS::exceptionXmlError(e.what());
            }
            return true;

        }
        bool xmlParserServerApiTicpp::loadAccounts(libJackSMS::dataTypes::configuredServicesType & _serviziConfigurati){
            try{
                ticpp::Node * subRoot=xmlResponse.FirstChild("JackSMS");
                ticpp::Node * accountsNode=subRoot->FirstChild("Services");

                ticpp::Node *child=NULL;
                while( child = accountsNode->IterateChildren( child ) ){
                    ticpp::Element * curElem= child->ToElement();
                    dataTypes::configuredAccount account;
                    account.setId(QString::fromStdString(curElem->GetAttribute("account_id")));
                    std::string tmp=curElem->GetAttribute("account_name");
                    account.setName(QString::fromUtf8(tmp.c_str(),tmp.length()));
                    account.setService(QString::fromStdString(curElem->GetAttribute("service_id")));
                    {
                        int counter=1;
                        bool attributeNotFound=false;
                        while(!attributeNotFound){
                            if (curElem->HasAttribute("data"+QString::number(counter).toStdString())){
                                account.setData(QString("data")+QString::number(counter),utilities::Base64Decode(QString::fromStdString(curElem->GetAttribute("data"+QString::number(counter).toStdString()))));
                                counter++;
                            }else{
                                attributeNotFound=true;
                            }
                        }
                    }

                    _serviziConfigurati.insert(account.getId(),account);
                }

            }catch(ticpp::Exception e){
                throw libJackSMS::exceptionXmlError(e.what());

            }


            return true;

        }
        bool xmlParserServerApiTicpp::checkSaved(QString &_id){
            ticpp::Node *root=xmlResponse.FirstChild("JackSMS");
            ticpp::Node *child=root->FirstChild("sync");
            _id=QString::fromStdString(child->ToElement()->GetText());
            if((_id=="0") || (_id=="-1"))
                return false;
            else
                return true;
        }
        bool xmlParserServerApiTicpp::extractImMessages(libJackSMS::dataTypes::logImType & _logIm){
            ticpp::Node *root=xmlResponse.FirstChild("JackSMS");
            ticpp::Node *child=NULL;
            //std::cout <<"parsing";
            int tmpId=0;
            while( child = root->IterateChildren( child ) ){
                ticpp::Element *childElem=child->ToElement();
                std::string tmp;
                libJackSMS::dataTypes::phoneNumber nn;
                try{
                    nn.parse(QString::fromStdString(childElem->GetAttribute("number")));
                    tmp=childElem->GetAttribute("sender");
                    nn.setAltName(QString::fromUtf8(tmp.c_str(),tmp.length()));
                    nn.setIsValid(true);
                }catch(exceptionPhoneNumber e){
                    nn.setIsValid(false);
                    nn.setAltName(QString::fromStdString(childElem->GetAttribute("number")));
                }


                QString data=QString::fromStdString(childElem->GetAttribute("time"));
                libJackSMS::dataTypes::dateTime date(data);
                tmp=childElem->GetAttribute("sms");
                libJackSMS::dataTypes::logImMessage imMsg(nn,date,QString::number(tmpId),QString::fromUtf8(tmp.c_str(),tmp.length()));
                _logIm.insert(QString::number(tmpId),imMsg);
                tmpId++;

            }
            return (tmpId==0)?false:true;

        }

        bool xmlParserServerApiTicpp::checkAddNewContact(QString &_resId){
            ticpp::Node *root=xmlResponse.FirstChild("JackSMS");
            ticpp::Node *child=root->FirstChild("id");
            _resId=QString::fromStdString(child->ToElement()->GetText());
            if(_resId=="0" || _resId=="-1")
                return false;
            else
                return true;
        }
        bool xmlParserServerApiTicpp::checkUpdateContact(){
            ticpp::Node *root=xmlResponse.FirstChild("JackSMS");
            ticpp::Node *child=root->FirstChild("id");
            QString _resId=QString::fromStdString(child->ToElement()->GetText());
            if( _resId=="-1" || _resId=="0")
                return false;
            else
                return true;
        }
        bool xmlParserServerApiTicpp::checkDeleteContact(){
            return true;
        }
        bool xmlParserServerApiTicpp::checkAddNewAccount(QString &_resId){
            ticpp::Node *root=xmlResponse.FirstChild("JackSMS");
            ticpp::Node *child=root->FirstChild("id");
            _resId=QString::fromStdString(child->ToElement()->GetText());
            if(_resId=="0" || _resId=="-1")
                return false;
            else
                return true;
        }
        bool xmlParserServerApiTicpp::checkUpdateAccount(){
            ticpp::Node *root=xmlResponse.FirstChild("JackSMS");
            ticpp::Node *child=root->FirstChild("id");
            QString _resId=QString::fromStdString(child->ToElement()->GetText());
            if( _resId=="-1" || _resId=="0")
                return false;
            else
                return true;
        }
        bool xmlParserServerApiTicpp::checkDeleteAccount(){
            return true;
        }
        bool xmlParserServerApiTicpp::parseConversation(libJackSMS::dataTypes::logSmsType & _logSms,libJackSMS::dataTypes::logImType & _logIm){
            /*ticpp::Node *root=xmlResponse.FirstChild("JackSMS");
            ticpp::Node *child=NULL;
            int count=0;

            while( child = root->IterateChildren( child ) ){
                ticpp::Element *childElem=child->ToElement();

                libJackSMS::dataTypes::phoneNumber nn;
                try{
                    nn.parse(childElem->GetAttribute("who"));
                }catch(exceptionPhoneNumber e){
                    nn.setAltName(childElem->GetAttribute("who"));
                }
                std::string id=childElem->GetAttribute("id");
                std::string data=childElem->GetAttribute("sent_on");
                libJackSMS::dataTypes::dateTime date(data.substr(8,2)+"/"+data.substr(5,2)+"/"+data.substr(0,4),data.substr(11,2)+":"+data.substr(14,2)+":"+data.substr(17,2));
                std::string type=childElem->GetAttribute("type");
                std::transform(type.begin(), type.end(), type.begin(), toupper);
                if (type=="RECV"){
                    libJackSMS::dataTypes::logImMessage imMsg(nn,date,id,childElem->GetAttribute("body"));
                    _logIm.insert(std::make_pair(id,imMsg));
                    count++;
                }else{
                    libJackSMS::dataTypes::logSmsMessage smsMsg(nn,"","",date,id,childElem->GetAttribute("body"));
                    _logSms.insert(std::make_pair(id,smsMsg));
                    count++;
                }

            }
            return ((count)==0)?false:true;*/
            return false;
        }


        bool xmlParserServerApiTicpp::parseServices(libJackSMS::dataTypes::servicesType &_servizi){
            try{

                ticpp::Node *subRoot=xmlResponse.FirstChild("JackSMS");

                ticpp::Node *child=NULL;
                while( child = subRoot->IterateChildren( child ) ){
                    ticpp::Element * thisService=child->ToElement();
                    QString serviceId=QString::fromStdString(thisService->GetAttribute("id"));
                    libJackSMS::dataTypes::service servizio;
                    servizio.setId(QString::fromStdString(thisService->GetAttribute("id")));
                    /************************/
                    //std::cout <<std::endl<<thisService->GetAttribute("name")<<std::endl;
                    servizio.setName(QString::fromStdString(thisService->GetAttribute("name")));
                    servizio.setVersion(QString::fromStdString(thisService->GetAttribute("v")));
                    servizio.setEncoding(QString::fromStdString(thisService->GetAttributeOrDefault("output_encoding","auto")));

                    servizio.setMaxSms(QString::fromStdString(thisService->GetAttribute("maxsms")));
                    servizio.setReset(QString::fromStdString(thisService->GetAttribute("reset")));
                    servizio.setMaxLength(QString::fromStdString(thisService->GetAttribute("maxlen")));
                    servizio.setSingleLength(QString::fromStdString(thisService->GetAttribute("singlelen")));
                    servizio.setSmsDivisor(QString::fromStdString(thisService->GetAttribute("sms_divisor")));


                    servizio.setIcon(QImage::fromData(utilities::Base64DecodeByte(QString::fromStdString(thisService->GetAttribute("icon")))));
                    {
                        ticpp::Node * descriptionNode=child->FirstChild("description");
                        ticpp::Node * infoNode=descriptionNode->FirstChild("info");
                        ticpp::Element * infoNodeElem= infoNode->ToElement();
                        std::string tmp=infoNodeElem->GetTextOrDefault("");
                        servizio.setDescription(QString::fromUtf8(tmp.c_str(),tmp.length()));
                        ticpp::Node * configNode=descriptionNode->FirstChild("config");
                        ticpp::Node * thisVar=NULL;
                        int c=1;
                        while( thisVar = configNode->IterateChildren( thisVar ) ){
                            ticpp::Element * thisVarElem= thisVar->ToElement();
                            std::string tmp=thisVarElem->GetAttribute("desc");
                            QString qTmp=QString::fromUtf8(tmp.c_str(),tmp.length());
                            tmp=thisVarElem->GetAttribute("name");
                            QString qName=QString::fromUtf8(tmp.c_str(),tmp.length());

                            if (thisVar->Value()=="required"){
                                libJackSMS::dataTypes::variabileServizio var(qName,qTmp,QString::fromStdString(thisVarElem->GetAttributeOrDefault("default","")),QString::fromStdString(thisVarElem->GetAttributeOrDefault("n",QString::number(c).toStdString())),true);

                                servizio.setVariable(var);
                            }else{
                                libJackSMS::dataTypes::variabileServizio var(qName,qTmp,QString::fromStdString(thisVarElem->GetAttributeOrDefault("default","")),QString::fromStdString(thisVarElem->GetAttributeOrDefault("n",QString::number(c).toStdString())),false);

                                servizio.setVariable(var);

                            }
                            c++;
                        }
                    }
                    {
                        ticpp::Node * procedureNode=child->FirstChild("procedure");
                        ticpp::Node * thisPage=NULL;
                        while( thisPage = procedureNode->IterateChildren( thisPage ) ){
                            ticpp::Element * thisPageElem= thisPage->ToElement();
                            libJackSMS::dataTypes::paginaServizio page;
                            page.setUrl(QString::fromStdString(thisPageElem->GetAttribute("uri")));
                            page.setMethod(QString::fromStdString(thisPageElem->GetAttributeOrDefault("method","")));
                            if (thisPageElem->HasAttribute("captcha"))
                                if (thisPageElem->GetAttribute("captcha")=="1")
                                    page.setIsCaptcha(true);
                            if (thisPageElem->HasAttribute("condition"))
                                page.setCondition(QString::fromStdString(thisPageElem->GetAttribute("condition")));

                            /*************************/
                            {
                                ticpp::Node *varNode=thisPage->FirstChild("vars",false);
                                if (varNode!=NULL){
                                    ticpp::Node *currentVar=NULL;
                                    while(  currentVar = varNode->IterateChildren( currentVar ) ){
                                        ticpp::Element * thisVar= currentVar->ToElement();
                                        std::string tmp=thisVar->GetAttributeOrDefault("desktop_encode","0");
                                        dataTypes::pageVariable var(QString::fromStdString(thisVar->GetAttribute("name")),QString::fromStdString(thisVar->GetAttribute("value")),QString::fromStdString(thisVar->GetAttributeOrDefault("value","")));
                                        var.setToEncode((tmp=="1")?true:false);
                                        page.setVariable(var);
                                    }
                                }
                            }
                            /**************************/
                            /*************************/
                            {
                                ticpp::Node *contentNode=thisPage->FirstChild("contents",false);
                                if (contentNode!=NULL){
                                    ticpp::Node *currentContent=NULL;
                                    while(  currentContent = contentNode->IterateChildren( currentContent ) ){
                                        ticpp::Element * thisContent= currentContent->ToElement();
                                        /********************/


                                        dataTypes::pageContent con(QString::fromStdString(thisContent->GetAttribute("name")),QString::fromStdString(thisContent->GetAttribute("left")),QString::fromStdString(thisContent->GetAttribute("right")));

                                        page.setContent(con);
                                    }
                                }
                            }
                            /**************************/

                            /*************************/
                            {
                                ticpp::Node *errNode=thisPage->FirstChild("errors",false);
                                if (errNode!=NULL){
                                    ticpp::Node *currentErr=NULL;
                                    while(  currentErr = errNode->IterateChildren( currentErr ) ){
                                        ticpp::Element * thisErr= currentErr->ToElement();
                                        dataTypes::pageError err(QString::fromStdString(thisErr->GetAttributeOrDefault("errcode","0")),QString::fromStdString(thisErr->GetAttribute("errstr")),QString::fromStdString(thisErr->GetAttribute("errmsg")));
                                        page.setError(err);
                                    }
                                }
                            }
                            /**************************/
                            /*************************/
                            {
                                ticpp::Node *accNode=thisPage->FirstChild("accept",false);
                                if (accNode!=NULL){
                                    ticpp::Node *currentAcc=NULL;
                                    while(  currentAcc = accNode->IterateChildren( currentAcc ) ){
                                        ticpp::Element * thisAcc= currentAcc->ToElement();
                                        dataTypes::pageAccept acc(QString::fromStdString(thisAcc->GetAttribute("acceptstr")),QString::fromStdString(thisAcc->GetAttributeOrDefault("acceptmsg","")));
                                        page.setAccept(acc);
                                    }
                                }
                            }
                            /**************************/

                            /*************************/
                            {
                                ticpp::Node *headerNode=thisPage->FirstChild("headers",false);
                                if (headerNode!=NULL){
                                    ticpp::Node *currentHeader=NULL;
                                    while(  currentHeader = headerNode->IterateChildren( currentHeader ) ){
                                        ticpp::Element * thisHeader= currentHeader->ToElement();
                                        dataTypes::pageHeader head(QString::fromStdString(thisHeader->GetAttribute("name")),QString::fromStdString(thisHeader->GetAttribute("value")));
                                        page.setHeader(head);
                                    }
                                }
                            }
                            /**************************/

                            /*************************/
                            {
                                ticpp::Node *commandsNode=thisPage->FirstChild("commands",false);
                                if (commandsNode!=NULL){
                                    ticpp::Node *currentCommand=NULL;
                                    while(  currentCommand = commandsNode->IterateChildren( currentCommand ) ){
                                        ticpp::Element * thisCommand= currentCommand->ToElement();
                                        dataTypes::pageCommand cmd(QString::fromStdString(thisCommand->GetAttribute("cmd")));
                                        page.setCommand(cmd);
                                    }
                                }
                            }
                            /**************************/
                            servizio.setPage(page);
                        }

                    }

                _servizi.insert(servizio.getId(),servizio);
                }


            }catch(ticpp::Exception e){
                throw libJackSMS::exceptionXmlError(e.what());

            }
            return true;
        }

    }
}

