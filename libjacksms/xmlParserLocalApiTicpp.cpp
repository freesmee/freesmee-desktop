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

#include "ticpp.h"
#include "ticpprc.h"
#include "DataTypes.h"
#include "xmlParserLocalApiTicpp.h"
#include "Utilities.h"
#include "Exceptions.h"
#include "Contact.h"
#include "FilesDirectory.h"
#include "LogSmsMessage.h"
#include <QPixmap>
#include <QImage>
#include <iostream>
#include <QFile>

namespace libJackSMS {

    namespace xmlParserApi {

        xmlParserLocalApiTicpp::xmlParserLocalApiTicpp(const QString &_userDirectory) :
                xmlParserLocalApiGeneric(_userDirectory),
                userDirectory(_userDirectory)
        {
        }

        xmlParserLocalApiTicpp::~xmlParserLocalApiTicpp()
        {
        }

        bool xmlParserLocalApiTicpp::appendSmsToLogfile(dataTypes::logSmsMessage &_msg)
        {
            try {
                ticpp::Document log;
                QString path = libJackSMS::directories::concatDirectoryAndFile(libJackSMS::directories::XmlDirectory(), userDirectory);
                path = libJackSMS::directories::concatDirectoryAndFile(path, "log.xml");

                log.LoadFile(path.toStdString(),TIXML_ENCODING_UTF8);
                ticpp::Node *subRoot = log.FirstChild("log");
                int id = 0;
                {
                    ticpp::Node *child = NULL;
                    while ((child = subRoot->IterateChildren(child))) {
                        ticpp::Element *curElem = child->ToElement();
                        std::string strId = curElem->GetAttribute("id");
                        int intId = QString::fromStdString(strId).toInt();
                        if (id < intId)
                            id = intId;
                    }
                    ++id;
                }

                _msg.setId(QString::number(id));

                ticpp::Element *elemToInsert = new ticpp::Element("msg");
                elemToInsert->SetAttribute("dest", _msg.getPhoneNumber().toString().toStdString());
                elemToInsert->SetAttribute("account", _msg.getAccount().toStdString());
                elemToInsert->SetAttribute("serviceid", _msg.getServiceId().toStdString());
                elemToInsert->SetAttribute("id", _msg.getId().toStdString());
                elemToInsert->SetText(_msg.getText().toUtf8().constData());
                elemToInsert->SetAttribute("hour", _msg.getDate().getStringHour().toStdString());
                elemToInsert->SetAttribute("date", _msg.getDate().getStringDay().toStdString());

                subRoot->LinkEndChild(elemToInsert);
                log.SaveFile();

            } catch(ticpp::Exception e) {
                throw libJackSMS::exceptionXmlError(e.what());
            }

            return true;
        }

        bool xmlParserLocalApiTicpp::appendImToLogfile(dataTypes::logImMessage &_msg)
        {
            try {
                ticpp::Document log;
                QString path = libJackSMS::directories::concatDirectoryAndFile(libJackSMS::directories::XmlDirectory(), userDirectory);
                path = libJackSMS::directories::concatDirectoryAndFile(path, "imlog.xml");

                log.LoadFile(path.toStdString(),TIXML_ENCODING_UTF8);
                ticpp::Node *subRoot = log.FirstChild("log");
                int id = 0;
                {
                    ticpp::Node *child = NULL;
                    while((child = subRoot->IterateChildren(child))) {
                        ticpp::Element *curElem = child->ToElement();
                        std::string strId = curElem->GetAttribute("id");
                        int intId=QString::fromStdString(strId).toInt();
                        if (id < intId)
                            id = intId;
                    }
                    ++id;
                }

                _msg.setId(QString::number(id));

                ticpp::Element *elemToInsert = new ticpp::Element("msg");
                elemToInsert->SetAttribute("sender", _msg.getPhoneNumber().toString().toStdString());
                elemToInsert->SetAttribute("id", _msg.getId().toStdString());
                elemToInsert->SetText(_msg.getText().toUtf8().constData());
                elemToInsert->SetAttribute("hour", _msg.getDate().getStringHour().toStdString());
                elemToInsert->SetAttribute("date", _msg.getDate().getStringDay().toStdString());

                subRoot->LinkEndChild(elemToInsert);
                log.SaveFile();

            } catch(ticpp::Exception e) {
                throw libJackSMS::exceptionXmlError(e.what());
            }
            return true;

        }

        bool xmlParserLocalApiTicpp::saveAllSmsToLogFile(const dataTypes::logSmsType &_smsContainer) {
            try {
                ticpp::Document log;
                QString path = libJackSMS::directories::concatDirectoryAndFile(libJackSMS::directories::XmlDirectory(), userDirectory);
                path = libJackSMS::directories::concatDirectoryAndFile(path, "log.xml");

                log.LoadFile(path.toStdString(), TIXML_ENCODING_UTF8);
                ticpp::Element *radice = new ticpp::Element("log");

                for (dataTypes::logSmsType::const_iterator i = _smsContainer.begin(); i != _smsContainer.end(); ++i) {
                    ticpp::Element *elemToInsert = new ticpp::Element("msg");
                    elemToInsert->SetAttribute("dest", i.value().getPhoneNumber().toString().toStdString());
                    elemToInsert->SetAttribute("account", i.value().getAccount().toStdString());
                    elemToInsert->SetAttribute("serviceid", i.value().getServiceId().toStdString());
                    elemToInsert->SetAttribute("id", i.value().getId().toStdString());
                    elemToInsert->SetText(i.value().getText().toUtf8().constData());
                    elemToInsert->SetAttribute("hour", i.value().getDate().getStringHour().toStdString());
                    elemToInsert->SetAttribute("date", i.value().getDate().getStringDay().toStdString());

                    radice->LinkEndChild(elemToInsert);
                }

                log.LinkEndChild(radice);
                log.SaveFile();

            } catch(ticpp::Exception e) {
                throw libJackSMS::exceptionXmlError(e.what());
            }

            return true;
        }

        bool xmlParserLocalApiTicpp::saveAllImToLogFile(const dataTypes::logImType &_smsContainer) {
            try {
                ticpp::Document log;
                QString path=libJackSMS::directories::concatDirectoryAndFile(libJackSMS::directories::XmlDirectory(),userDirectory);
                path=libJackSMS::directories::concatDirectoryAndFile(path,"imlog.xml");

                log.LoadFile(path.toStdString(),TIXML_ENCODING_UTF8);
                ticpp::Element *radice = new ticpp::Element("log");

                for (dataTypes::logImType::const_iterator i=_smsContainer.begin(); i != _smsContainer.end(); ++i) {
                    ticpp::Element *elemToInsert = new ticpp::Element("msg");
                    elemToInsert->SetAttribute("sender", i.value().getPhoneNumber().toString().toStdString());
                    elemToInsert->SetAttribute("id", i.value().getId().toStdString());
                    elemToInsert->SetText(i.value().getText().toUtf8().constData());
                    elemToInsert->SetAttribute("hour", i.value().getDate().getStringHour().toStdString());
                    elemToInsert->SetAttribute("date", i.value().getDate().getStringDay().toStdString());

                    radice->LinkEndChild(elemToInsert);
                }

                log.LinkEndChild(radice);
                log.SaveFile();

            } catch(ticpp::Exception e) {
                throw libJackSMS::exceptionXmlError(e.what());
            }

            return true;
        }

        /*bool xmlParserLocalApiTicpp::loadPhoneBook(libJackSMS::dataTypes::phoneBookType &_rubrica) {
            try {
                ticpp::Document phonebook;
                QString path=libJackSMS::directories::concatDirectoryAndFile(libJackSMS::directories::XmlDirectory(),userDirectory);
                path=libJackSMS::directories::concatDirectoryAndFile(path,"abook.xml");
                phonebook.LoadFile(path.toStdString(),TIXML_ENCODING_UTF8);


                ticpp::Node * subRoot=phonebook.FirstChild("addressbook");
                ticpp::Node * thisGroup=NULL;
                while( (thisGroup = subRoot->IterateChildren( thisGroup )) ){
                    ticpp::Node *thisContact=NULL;
                    while( (thisContact = thisGroup->IterateChildren( thisContact )) ){
                        ticpp::Element *thisContactElem=thisContact->ToElement();

                        libJackSMS::dataTypes::phoneNumber telefono;
                        telefono.parse(QString::fromStdString(thisContactElem->GetAttribute("number")));

                        libJackSMS::dataTypes::contact contatto(QString::fromStdString(thisContactElem->GetAttribute("name")),telefono,QString::fromStdString(thisGroup->Value()),QString::fromStdString(thisContactElem->GetAttribute("serviceid")));
                        _rubrica.insert(QString::fromStdString(thisContactElem->GetAttribute("name")),contatto);

                    }
                }
            }catch(ticpp::Exception e){
                throw libJackSMS::exceptionXmlError(e.what());
            }
            return true;
        }*/

        bool xmlParserLocalApiTicpp::loadServices(libJackSMS::dataTypes::servicesType & _servizi)
        {
            try
            {
                ticpp::Document services;

                QString path=libJackSMS::directories::concatDirectoryAndFile(libJackSMS::directories::XmlDirectory(),"services.xml");
                services.LoadFile(path.toStdString(),TIXML_ENCODING_UTF8);


                ticpp::Node *subRoot = services.FirstChild("services",false);
                if (subRoot == NULL)
                    subRoot = services.FirstChild("Freesmee");
                ticpp::Node *child = NULL;
                while ((child = subRoot->IterateChildren(child))) {
                    ticpp::Element *thisService = child->ToElement();
                    libJackSMS::dataTypes::service servizio;
                    servizio.setId(QString::fromStdString(thisService->GetAttribute("id")));

                    /************************/

                    servizio.setName(QString::fromStdString(thisService->GetAttribute("name")));
                    servizio.setVersion(QString::fromStdString(thisService->GetAttribute("v")));
                    servizio.setEncoding(QString::fromStdString(thisService->GetAttributeOrDefault("output_encoding", "auto")));

                    servizio.setMaxSms(QString::fromStdString(thisService->GetAttribute("maxsms")));
                    servizio.setReset(QString::fromStdString(thisService->GetAttribute("reset")));
                    servizio.setMaxLength(QString::fromStdString(thisService->GetAttribute("maxlen")));
                    servizio.setSingleLength(QString::fromStdString(thisService->GetAttribute("singlelen")));
                    servizio.setSmsDivisor(QString::fromStdString(thisService->GetAttribute("sms_divisor")));
                    servizio.setServiceType(QString::fromStdString(thisService->GetAttributeOrDefault("service_type", "other")));

                    if (thisService->GetAttributeOrDefault("icon", "") != "")
                        servizio.setIcon(QImage::fromData(utilities::Base64DecodeByte(QString::fromStdString(thisService->GetAttribute("icon")))));
                    else
                        servizio.setIcon(QImage(":/resource/Mail-icon.png"));

                    {
                        ticpp::Node *descriptionNode = child->FirstChild("description");
                        ticpp::Node *infoNode = descriptionNode->FirstChild("info");
                        ticpp::Element *infoNodeElem = infoNode->ToElement();
                        std::string tmp = infoNodeElem->GetTextOrDefault("");
                        servizio.setDescription(QString::fromUtf8(tmp.c_str(), tmp.length()));

                        ticpp::Node *configNode=descriptionNode->FirstChild("config");
                        ticpp::Node *thisVar = NULL;
                        int c = 1;

                        while ((thisVar = configNode->IterateChildren(thisVar))) {
                            ticpp::Element *thisVarElem = thisVar->ToElement();
                            std::string tmp = thisVarElem->GetAttribute("desc");
                            QString qTmp = QString::fromUtf8(tmp.c_str(), tmp.length());
                            tmp = thisVarElem->GetAttribute("name");
                            QString qName = QString::fromUtf8(tmp.c_str(), tmp.length());

                            if (thisVar->Value() == "required") {
                                libJackSMS::dataTypes::variabileServizio var(qName, qTmp, QString::fromStdString(thisVarElem->GetAttributeOrDefault("default", "")), QString::fromStdString(thisVarElem->GetAttributeOrDefault("n", QString::number(c).toStdString())), true);
                                servizio.setVariable(var);
                            } else {
                                libJackSMS::dataTypes::variabileServizio var(qName, qTmp, QString::fromStdString(thisVarElem->GetAttributeOrDefault("default", "")), QString::fromStdString(thisVarElem->GetAttributeOrDefault("n", QString::number(c).toStdString())), false);
                                servizio.setVariable(var);
                            }

                            ++c;
                        }
                    }

                    {
                        ticpp::Node *procedureNode = child->FirstChild("procedure");
                        ticpp::Node *thisPage = NULL;
                        while ((thisPage = procedureNode->IterateChildren(thisPage))) {
                            ticpp::Element *thisPageElem = thisPage->ToElement();
                            libJackSMS::dataTypes::paginaServizio page;
                            page.setUrl(QString::fromStdString(thisPageElem->GetAttribute("uri")));
                            page.setMethod(QString::fromStdString(thisPageElem->GetAttributeOrDefault("method", "")));
                            if (thisPageElem->HasAttribute("captcha"))
                                if (thisPageElem->GetAttribute("captcha")=="1")
                                    page.setIsCaptcha(true);
                            if (thisPageElem->HasAttribute("condition"))
                                page.setCondition(QString::fromStdString(thisPageElem->GetAttribute("condition")));
                            if (thisPageElem->HasAttribute("sleepbefore")) {
                                bool intConversionCheck = false;
                                int intConversion = QString::fromStdString(thisPageElem->GetAttribute("sleepbefore")).toInt(&intConversionCheck);
                                if (intConversionCheck)
                                    page.setSleepbefore(intConversion);
                            }

                            /*************************/
                            {
                                ticpp::Node *varNode=thisPage->FirstChild("vars",false);
                                if (varNode!=NULL){
                                    ticpp::Node *currentVar=NULL;
                                    while( (currentVar = varNode->IterateChildren( currentVar )) ){
                                        ticpp::Element * thisVar= currentVar->ToElement();
                                        std::string tmp=thisVar->GetAttributeOrDefault("desktop_encode","0");
                                        bool condition=false;
                                        if (thisVar->HasAttribute("condition"))
                                            condition=true;
                                        dataTypes::pageVariable var(QString::fromStdString(thisVar->GetAttribute("name")),QString::fromStdString(thisVar->GetAttribute("value")),QString::fromStdString(thisVar->GetAttributeOrDefault("condition","")),condition);
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
                                    while( (currentContent = contentNode->IterateChildren( currentContent )) ){
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
                                    while( (currentErr = errNode->IterateChildren( currentErr )) ){
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
                                    while( (currentAcc = accNode->IterateChildren( currentAcc )) ){
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
                                    while( (currentHeader = headerNode->IterateChildren( currentHeader )) ){
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
                                    while( (currentCommand = commandsNode->IterateChildren( currentCommand )) ){
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

                    {
                        ticpp::Node *thisPage = child->FirstChild("postprocedure", false);
                        if (thisPage != NULL) {

                            libJackSMS::dataTypes::paginaServizio page;
                            ticpp::Element *thisPageElem = thisPage->ToElement();

                            page.setUrl(QString::fromStdString(thisPageElem->GetAttribute("uri")));
                            page.setMethod(QString::fromStdString(thisPageElem->GetAttributeOrDefault("method", "")));
                            if (thisPageElem->HasAttribute("condition"))
                                page.setCondition(QString::fromStdString(thisPageElem->GetAttribute("condition")));
                            if (thisPageElem->HasAttribute("sleepbefore")) {
                                bool intConversionCheck = false;
                                int intConversion = QString::fromStdString(thisPageElem->GetAttribute("sleepbefore")).toInt(&intConversionCheck);
                                if (intConversionCheck)
                                    page.setSleepbefore(intConversion);
                            }

                            /*************************/
                            {
                                ticpp::Node *varNode=thisPage->FirstChild("vars",false);
                                if (varNode!=NULL){
                                    ticpp::Node *currentVar=NULL;
                                    while( (currentVar = varNode->IterateChildren( currentVar )) ){
                                        ticpp::Element * thisVar= currentVar->ToElement();
                                        std::string tmp = thisVar->GetAttributeOrDefault("desktop_encode","0");
                                        bool condition=false;
                                        if (thisVar->HasAttribute("condition"))
                                            condition=true;
                                        dataTypes::pageVariable var(QString::fromStdString(thisVar->GetAttribute("name")),QString::fromStdString(thisVar->GetAttribute("value")),QString::fromStdString(thisVar->GetAttributeOrDefault("condition","")),condition);
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
                                    while( (currentContent = contentNode->IterateChildren( currentContent )) ){
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
                                    while( (currentErr = errNode->IterateChildren( currentErr )) ){
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
                                    while( (currentAcc = accNode->IterateChildren( currentAcc )) ){
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
                                    while( (currentHeader = headerNode->IterateChildren( currentHeader )) ){
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
                                    while( (currentCommand = commandsNode->IterateChildren( currentCommand )) ){
                                        ticpp::Element * thisCommand= currentCommand->ToElement();
                                        dataTypes::pageCommand cmd(QString::fromStdString(thisCommand->GetAttribute("cmd")));
                                        page.setCommand(cmd);
                                    }
                                }
                            }
                            /**************************/
                            servizio.setPostprocedurePage(page);
                        }
                    }

                    _servizi.insert(servizio.getId(), servizio);
                }




            }catch(ticpp::Exception e){
                throw libJackSMS::exceptionXmlError(e.what());

            }

            return true;

        }

        bool xmlParserLocalApiTicpp::loadAccounts(libJackSMS::dataTypes::configuredServicesType &_serviziConfigurati) {
            try {
                ticpp::Document accounts;
                QString path=libJackSMS::directories::concatDirectoryAndFile(libJackSMS::directories::XmlDirectory(),userDirectory);
                path=libJackSMS::directories::concatDirectoryAndFile(path,"accounts.xml");
                if (!QFile::exists(path))
                    throw libJackSMS::exceptionXmlNotFound();
                accounts.LoadFile(path.toStdString(),TIXML_ENCODING_UTF8);


                ticpp::Node *subRoot=accounts.FirstChild("profile");
                ticpp::Node *child=NULL;
                while( (child = subRoot->IterateChildren( child )) ){
                    ticpp::Element * curElem= child->ToElement();
                    libJackSMS::dataTypes::configuredAccount account;
                    account.setId(QString::fromStdString(curElem->GetAttribute("id")));
                    account.setName(QString::fromStdString(curElem->GetAttribute("name")));
                    account.setService(QString::fromStdString(curElem->GetAttribute("service")));
                    {
                        ticpp::Node *node=child->FirstChild("data");
                        ticpp::Node *thisNode=NULL;
                        while((thisNode = node->IterateChildren(thisNode))) {
                            account.setData(QString::fromStdString(thisNode->Value()),utilities::Base64Decode(QString::fromStdString(thisNode->ToElement()->GetTextOrDefault(""))));
                        }
                    }
                    {
                        ticpp::Node *node=child->FirstChild("stats");
                        ticpp::Node *thisNode=NULL;
                        while( (thisNode = node->IterateChildren( thisNode )) ){
                            account.setStat(QString::fromStdString(thisNode->Value()),QString::fromStdString(thisNode->ToElement()->GetTextOrDefault("")));
                        }
                    }
                    {
                        ticpp::Node *node=child->FirstChild("options",false);
                        if (node!=NULL){
                            ticpp::Node *thisNode=NULL;
                            while( (thisNode = node->IterateChildren( thisNode )) ){
                                account.setOption(QString::fromStdString(thisNode->Value()),QString::fromStdString(thisNode->ToElement()->GetTextOrDefault("")));
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

        bool xmlParserLocalApiTicpp::loadOptions(libJackSMS::dataTypes::optionsType & _opzioni,bool overwriteExisting){
            try {
                ticpp::Document config;

                QString path = libJackSMS::directories::concatDirectoryAndFile(libJackSMS::directories::XmlDirectory(), userDirectory);
                path = libJackSMS::directories::concatDirectoryAndFile(path, "config.xml");

                if (!QFile::exists(path)) {
                    //throw libJackSMS::exceptionXmlNotFound();
                    ticpp::Document doc;
                    ticpp::Element *newRoot = new ticpp::Element("config");
                    doc.LinkEndChild(newRoot);
                    doc.SaveFile(path.toStdString());
                }

                config.LoadFile(path.toStdString(), TIXML_ENCODING_UTF8);
                
                ticpp::Node *subRoot = config.FirstChild("config");
                ticpp::Node *child = NULL;

                while( (child = subRoot->IterateChildren( child )) ){
                    ticpp::Element * curElem = child->ToElement();

                    if (overwriteExisting)
                        _opzioni[QString::fromStdString(child->Value())] = QString::fromStdString(curElem->GetTextOrDefault(""));
                    else if (_opzioni.find(QString::fromStdString(child->Value())) == _opzioni.end())
                        _opzioni.insert(QString::fromStdString(child->Value()),QString::fromStdString(curElem->GetTextOrDefault("")));


                }
            }catch(ticpp::Exception e){
                throw libJackSMS::exceptionXmlError(e.what());

            }


            return true;
        }

        /*bool xmlParserLocalApiTicpp::deleteAccount(const QString &_id){
            try{
                ticpp::Document acc;
                QString path=libJackSMS::directories::concatDirectoryAndFile(libJackSMS::directories::XmlDirectory(),userDirectory);
                path=libJackSMS::directories::concatDirectoryAndFile(path,"accounts.xml");
                if (!QFile::exists(path))
                    throw libJackSMS::exceptionXmlNotFound();
                acc.LoadFile(path.toStdString(),TIXML_ENCODING_UTF8);

                ticpp::Node *subRoot=acc.FirstChild("profile");

                ticpp::Node *nodeToDelete=NULL;
                bool found=false;
                while( (nodeToDelete = subRoot->IterateChildren( nodeToDelete )) ){
                    if (QString::fromStdString(nodeToDelete->ToElement()->GetAttribute("id"))==_id){
                        found=true;
                        break;
                    }
                }
                if (!found){
                    throw exceptionSomethingWrong("Non posso eliminare l'account.\nAccount "+_id+" non trovato.");
                }else{
                    subRoot->RemoveChild(nodeToDelete);
                    acc.SaveFile();
                }


            }catch(ticpp::Exception e){
                throw libJackSMS::exceptionXmlError(e.what());
            }
            return true;
        }

        bool xmlParserLocalApiTicpp::updateAccount(libJackSMS::dataTypes::configuredAccount & _account){
            try{
                ticpp::Document acc;
                std::string path=libJackSMS::directories::concatDirectoryAndFile(libJackSMS::directories::XmlDirectory(),userDirectory);
                path=libJackSMS::directories::concatDirectoryAndFile(path,"accounts.xml");
                acc.LoadFile(path,TIXML_ENCODING_UTF8);

                ticpp::Node *subRoot=acc.FirstChild("profile");

                ticpp::Node *nodeToDelete=NULL;
                bool found=false;
                while( nodeToDelete = subRoot->IterateChildren( nodeToDelete ) ){
                    if (nodeToDelete->ToElement()->GetAttribute("id")==_account.getId()){
                        found=true;
                        break;
                    }
                }
                if (!found){
                        throw exceptionSomethingWrong("Non posso aggiornare l'account.\nAccount "+_account.getId()+" ("+_account.getName()+") non trovato.");
                }else{
                    subRoot->RemoveChild(nodeToDelete);

                    ticpp::Element *service=new ticpp::Element("account");
                    service->SetAttribute("name",_account.getName());
                    service->SetAttribute("id",_account.getId());
                    service->SetAttribute("service",_account.getService());

                    ticpp::Element *dataRoot=new ticpp::Element("data");

                    while(_account.nextData()){
                        ticpp::Element *data=new ticpp::Element(_account.currentData());
                        data->SetText(utilities::Base64Encode(_account.getData(_account.currentData())));
                        dataRoot->LinkEndChild(data);
                    }
                    ticpp::Element *statRoot=new ticpp::Element("stats");
                    while(_account.nextStat()){
                        ticpp::Element *stat=new ticpp::Element(_account.currentStat());
                        stat->SetText(utilities::Base64Encode(_account.getStat(_account.currentStat())));
                        statRoot->LinkEndChild(stat);
                    }

                    service->LinkEndChild(dataRoot);
                    service->LinkEndChild(statRoot);
                    subRoot->LinkEndChild(service);
                    acc.SaveFile();
                }


            }catch(ticpp::Exception e){
                throw libJackSMS::exceptionXmlError(e.what());
            }
            return true;
        }*/

        /*bool xmlParserLocalApiTicpp::addNewAccount(libJackSMS::dataTypes::configuredAccount & _account){
            try{
                ticpp::Document acc;
                QString path=libJackSMS::directories::concatDirectoryAndFile(libJackSMS::directories::XmlDirectory(),userDirectory);
                path=libJackSMS::directories::concatDirectoryAndFile(path,"accounts.xml");
                if (!QFile::exists(path))
                    throw libJackSMS::exceptionXmlNotFound();
                acc.LoadFile(path.toStdString(),TIXML_ENCODING_UTF8);

                ticpp::Node *subRoot=acc.FirstChild("profile");
                int id=0;
                {
                    ticpp::Node *child=NULL;
                    while( (child = subRoot->IterateChildren( child )) ){
                        ticpp::Element * curElem= child->ToElement();
                        QString strId=QString::fromStdString(curElem->GetAttribute("id"));
                        bool ok;
                        int intId=strId.toInt(&ok,10);
                        if (id<intId)
                            id=intId;
                    }
                    ++id;


                }
                ticpp::Element *service=new ticpp::Element("account");
                service->SetAttribute("name",_account.getName().toStdString());
                service->SetAttribute("id",QString::number(id).toStdString());
                service->SetAttribute("service",_account.getService().toStdString());

                ticpp::Element *dataRoot=new ticpp::Element("data");

                while(_account.nextData()){
                    ticpp::Element *data=new ticpp::Element(_account.currentData().toStdString());
                    data->SetText(utilities::Base64Encode(_account.getData(_account.currentData())).toStdString());
                    dataRoot->LinkEndChild(data);
                }
                ticpp::Element *statRoot=new ticpp::Element("stats");
                while(_account.nextStat()){
                    ticpp::Element *stat=new ticpp::Element(_account.currentStat().toStdString());
                    stat->SetText(utilities::Base64Encode(_account.getStat(_account.currentStat())).toStdString());
                    statRoot->LinkEndChild(stat);
                }

                service->LinkEndChild(dataRoot);
                service->LinkEndChild(statRoot);
                subRoot->LinkEndChild(service);
                acc.SaveFile();

            }catch(ticpp::Exception e){
                throw libJackSMS::exceptionXmlError(e.what());
            }
            return true;
        }*/

        bool xmlParserLocalApiTicpp::loadUsers(QList<dataTypes::stringTriplet> &_utenti) {

            QString oldDirectory = userDirectory;

            try{
                ticpp::Document log;
                QString path = libJackSMS::directories::concatDirectoryAndFile(libJackSMS::directories::XmlDirectory(), "users.xml");
                if (!QFile::exists(path))
                    throw libJackSMS::exceptionXmlNotFound();
                log.LoadFile(path.toStdString(),TIXML_ENCODING_UTF8);

                ticpp::Node *subRoot = log.FirstChild("users");
                ticpp::Node *child = NULL;

                while((child = subRoot->IterateChildren( child ))) {
                    ticpp::Element *childElem = child->ToElement();

                    dataTypes::optionsType opzioni;
                    userDirectory = QString::fromStdString(childElem->GetAttribute("directory"));

                    try {
                        loadOptions(opzioni);
                    } catch(libJackSMS::exceptionXmlNotFound){

                    }
                    QString password = opzioni["password"];

                    dataTypes::stringTriplet trip(utilities::Base64Decode(QString::fromStdString(childElem->GetAttribute("username"))), password, QString::fromStdString(childElem->GetAttribute("directory")));
                    _utenti.push_back(trip);
                }

            }catch(ticpp::Exception e){

                userDirectory=oldDirectory;
                throw libJackSMS::exceptionXmlError(e.what());
            }
            userDirectory=oldDirectory;
            return true;
        }

        bool xmlParserLocalApiTicpp::loadSmsLog(libJackSMS::dataTypes::logSmsType & _logSms){
            try{
                ticpp::Document log;
                QString path=libJackSMS::directories::concatDirectoryAndFile(libJackSMS::directories::XmlDirectory(),userDirectory);
                path=libJackSMS::directories::concatDirectoryAndFile(path,"log.xml");
                if (!QFile::exists(path)){
                    ticpp::Document doc;
                    ticpp::Element *newRoot=new ticpp::Element("log");
                    doc.LinkEndChild(newRoot);
                    QString path=libJackSMS::directories::concatDirectoryAndFile(libJackSMS::directories::XmlDirectory(),userDirectory);
                    path=libJackSMS::directories::concatDirectoryAndFile(path,"log.xml");
                    doc.SaveFile(path.toStdString());
                    return true;
                }
                log.LoadFile(path.toStdString(),TIXML_ENCODING_UTF8);


                ticpp::Node *subRoot=log.FirstChild("log");
                ticpp::Node *child=NULL;
                while( (child = subRoot->IterateChildren( child )) ){
                    ticpp::Element * childElem=child->ToElement();
                    libJackSMS::dataTypes::phoneNumber numero;
                    numero.parse(QString::fromStdString(childElem->GetAttribute("dest")));
                    std::string tmp=childElem->GetTextOrDefault("");
                    QString qSms=QString::fromUtf8(tmp.c_str(),tmp.length());
                    libJackSMS::dataTypes::dateTime data(QString::fromStdString(childElem->GetAttribute("date")),QString::fromStdString(childElem->GetAttribute("hour")));
                    libJackSMS::dataTypes::logSmsMessage sms(numero,QString::fromStdString(childElem->GetAttribute("account")),QString::fromStdString(childElem->GetAttribute("serviceid")),data,QString::fromStdString(childElem->GetAttribute("id")),qSms);
                    _logSms.insert(sms.getId(),sms);
                }
            }catch(ticpp::Exception e){
                throw libJackSMS::exceptionXmlError(e.what());
            }
            return true;
        }

        bool xmlParserLocalApiTicpp::loadImLog(libJackSMS::dataTypes::logImType & _logIm){
            try{
                ticpp::Document log;
                QString path=libJackSMS::directories::concatDirectoryAndFile(libJackSMS::directories::XmlDirectory(),userDirectory);
                path=libJackSMS::directories::concatDirectoryAndFile(path,"imlog.xml");
                if (!QFile::exists(path)){
                    ticpp::Document doc;
                    ticpp::Element *newRoot=new ticpp::Element("log");
                    doc.LinkEndChild(newRoot);
                    QString path=libJackSMS::directories::concatDirectoryAndFile(libJackSMS::directories::XmlDirectory(),userDirectory);
                    path=libJackSMS::directories::concatDirectoryAndFile(path,"imlog.xml");
                    doc.SaveFile(path.toStdString());
                    return true;
                }
                log.LoadFile(path.toStdString(),TIXML_ENCODING_UTF8);



                ticpp::Node *subRoot=log.FirstChild("log");
                ticpp::Node *child=NULL;
                while( (child = subRoot->IterateChildren( child )) ){
                    ticpp::Element * childElem=child->ToElement();
                    libJackSMS::dataTypes::phoneNumber numero;
                    numero.parse(QString::fromStdString(childElem->GetAttribute("sender")));
                    std::string tmp=childElem->GetText();
                    QString qSms=QString::fromUtf8(tmp.c_str(),tmp.length());
                    libJackSMS::dataTypes::dateTime data(QString::fromStdString(childElem->GetAttribute("date")),QString::fromStdString(childElem->GetAttribute("hour")));
                    libJackSMS::dataTypes::logImMessage sms(numero,data,QString::fromStdString(childElem->GetAttribute("id")),qSms);
                    _logIm.insert(sms.getId(),sms);
                }
            }catch(ticpp::Exception e){
                throw libJackSMS::exceptionXmlError(e.what());
            }
            return true;
        }

        bool xmlParserLocalApiTicpp::updateStat(const QString &_accountId, const QString &_statName, const QString &_statVal)
        {
            try {

                QString path = libJackSMS::directories::concatDirectoryAndFile(libJackSMS::directories::XmlDirectory(), userDirectory);
                path = libJackSMS::directories::concatDirectoryAndFile(path, "stats.xml");

                if (!QFile::exists(path))
                    throw libJackSMS::exceptionXmlNotFound();

                ticpp::Document log;
                log.LoadFile(path.toStdString(), TIXML_ENCODING_UTF8);

                bool nodeExist = false;
                bool idExist = false;
                ticpp::Node *subRoot = log.FirstChild("stats");
                ticpp::Node *child = NULL;
                ticpp::Node *childStat = NULL;

                while ((child = subRoot->IterateChildren(child))) {
                    ticpp::Element *childElem = child->ToElement();
                    if (child->Value() == "account-stat") {
                        QString id = QString::fromStdString(childElem->GetAttribute("account"));
                        if (id == _accountId) {
                            idExist = true;
                            childStat = NULL;
                            while ((childStat = child->IterateChildren(childStat))) {
                                if (childStat->Value() == _statName.toStdString()) {
                                    childStat->ToElement()->SetText(_statVal.toStdString());
                                    nodeExist = true;
                                    log.SaveFile();
                                }
                            }
                        }
                    }
                }

                if (!idExist) {
                    ticpp::Element *newNode = new ticpp::Element("account-stat");
                    newNode->SetAttribute("account", _accountId.toStdString());
                    subRoot->LinkEndChild(newNode);

                    ticpp::Element *newStat = new ticpp::Element(_statName.toStdString());
                    newStat->SetText(_statVal.toStdString());
                    newNode->LinkEndChild(newStat);
                    log.SaveFile();

                } else if (!nodeExist) {
                    ticpp::Element *newStat = new ticpp::Element(_statName.toStdString());
                    newStat->SetText(_statVal.toStdString());
                    child->LinkEndChild(newStat);
                    log.SaveFile();
                }

            }catch(ticpp::Exception e){
                throw libJackSMS::exceptionXmlError(e.what());
            }
            return true;
        }

        bool xmlParserLocalApiTicpp::loadStats(libJackSMS::dataTypes::configuredServicesType &_services) {
            try{
                ticpp::Document log;
                QString path = libJackSMS::directories::concatDirectoryAndFile(libJackSMS::directories::XmlDirectory(), userDirectory);
                path = libJackSMS::directories::concatDirectoryAndFile(path, "stats.xml");
                try {
                    log.LoadFile(path.toStdString(), TIXML_ENCODING_UTF8);
                } catch(ticpp::Exception e) {
                    ticpp::Document doc;
                    ticpp::Element *newRoot = new ticpp::Element("stats");
                    doc.LinkEndChild(newRoot);
                    QString path = libJackSMS::directories::concatDirectoryAndFile(libJackSMS::directories::XmlDirectory(), userDirectory);
                    path = libJackSMS::directories::concatDirectoryAndFile(path, "stats.xml");
                    doc.SaveFile(path.toStdString());
                    return true;
                }

                ticpp::Node *subRoot = log.FirstChild("stats");
                ticpp::Node *child = NULL;
                while ((child = subRoot->IterateChildren(child))) {
                    ticpp::Element *childElem = child->ToElement();
                    if (child->Value() == "account-stat") {
                        QString id = QString::fromStdString(childElem->GetAttribute("account"));
                        dataTypes::configuredServicesType::iterator i = _services.find(id);
                        if (i != _services.end()){
                            std::string r = childElem->GetText(false);
                            if (!r.empty()) {
                                /*this statement is only for compatibility from old stat xml format to new stat xml format*/
                                i.value().setStat("sent", QString::fromStdString(r));
                                childElem->SetText("");
                                ticpp::Element *newEl = new ticpp::Element("sent");
                                newEl->SetText(r);
                                child->LinkEndChild(newEl);
                                log.SaveFile();

                            } else {
                                ticpp::Node *se = child->FirstChild("sent", false);
                                QString r;
                                if (se == NULL)
                                    r = "0";
                                else
                                    r = QString::fromStdString(se->ToElement()->GetTextOrDefault("0"));
                                i.value().setStat("sent", r);
                            }
                        }
                    }
                }
            }catch(ticpp::Exception e){
                throw libJackSMS::exceptionXmlError(e.what());
            }
            return true;
        }


        /*bool xmlParserLocalApiTicpp::addNewContact(libJackSMS::dataTypes::contact & _contatto){
            try {
                ticpp::Document phonebook;
                QString path=libJackSMS::directories::concatDirectoryAndFile(libJackSMS::directories::XmlDirectory(),userDirectory);
                path=libJackSMS::directories::concatDirectoryAndFile(path,"abook.xml");
                phonebook.LoadFile(path.toStdString(),TIXML_ENCODING_UTF8);

                ticpp::Node * subRoot=phonebook.FirstChild("addressbook");

                {
                    ticpp::Node *thisGroup=NULL;
                    while( (thisGroup = subRoot->IterateChildren( thisGroup )) ){
                        ticpp::Node *thisContact=NULL;
                        while( (thisContact = thisGroup->IterateChildren( thisContact )) ){
                            ticpp::Element *thisContactElem=thisContact->ToElement();

                            if (_contatto.getName()==QString::fromStdString(thisContactElem->GetAttribute("name")))
                                throw exceptionSomethingWrong("Il nome scelto e' gia  utilizzato da un'altro contatto.");
                        }
                    }
                }

                ticpp::Node *GroupRoot = subRoot->FirstChild(_contatto.getGroup().toStdString(), false);
                if (GroupRoot == NULL){
                    ticpp::Element *toInsert = new ticpp::Element(_contatto.getGroup().toStdString());
                    subRoot->LinkEndChild(toInsert);
                    GroupRoot = subRoot->FirstChild(_contatto.getGroup().toStdString());
                }

                ticpp::Element *newContact = new ticpp::Element("contact");
                newContact->SetAttribute("name", _contatto.getName().toStdString());
                newContact->SetAttribute("number",  _contatto.getPhone().toString().toStdString());
                newContact->SetAttribute("serviceid",  _contatto.getAccount().toStdString());
                GroupRoot->LinkEndChild(newContact);
                phonebook.SaveFile();

            }catch(ticpp::Exception e){
                throw exceptionXmlError(e.what());
                return false;
            }
            return true;
        }

        bool xmlParserLocalApiTicpp::updateContact(libJackSMS::dataTypes::contact & _contatto) {
            return false;
        }

        bool xmlParserLocalApiTicpp::deleteContact(const QString &_name){
            bool removed=false;
            try{
                ticpp::Document phonebook;
                QString path=libJackSMS::directories::concatDirectoryAndFile(libJackSMS::directories::XmlDirectory(),userDirectory);
                path=libJackSMS::directories::concatDirectoryAndFile(path,"abook.xml");
                phonebook.LoadFile(path.toStdString(),TIXML_ENCODING_UTF8);


                ticpp::Node * subRoot=phonebook.FirstChild("addressbook");
                ticpp::Node * GroupRoot=NULL;
                while( (GroupRoot = subRoot->IterateChildren( GroupRoot )) ){
                    ticpp::Node * contatto=NULL;
                    while( (contatto = GroupRoot->IterateChildren( contatto )) ){
                        ticpp::Element *contattoElem=contatto->ToElement();
                        if (QString::fromStdString(contattoElem->GetAttribute("name"))==_name){
                            GroupRoot->RemoveChild(contatto);
                            removed=true;
                            break;
                        }
                    }
                    if (removed)
                        break;
                }
                phonebook.SaveFile();
            }catch(ticpp::Exception e){
                throw exceptionXmlError(e.what());
                return false;
            }
            return removed;
        }*/

        bool xmlParserLocalApiTicpp::userDirectoryExists(const QString &_user){
            try{
                ticpp::Document log;
                QString path=libJackSMS::directories::concatDirectoryAndFile(libJackSMS::directories::XmlDirectory(),"users.xml");
                log.LoadFile(path.toStdString(),TIXML_ENCODING_UTF8);

                ticpp::Node *subRoot=log.FirstChild("users");
                ticpp::Node *child=NULL;

                while( (child = subRoot->IterateChildren( child )) ){
                    ticpp::Element * childElem=child->ToElement();

                    if (utilities::Base64Decode(QString::fromStdString(childElem->GetAttribute("username")))==_user)
                        return true;
                }

            }catch (ticpp::Exception e) {

                throw libJackSMS::exceptionXmlError(e.what());
            }
            return false;
        }

        bool xmlParserLocalApiTicpp::createUser(const QString &_user,const QString &_directory){
            try{
                {
                    ticpp::Document log;
                    QString path=libJackSMS::directories::concatDirectoryAndFile(libJackSMS::directories::XmlDirectory(),"users.xml");
                    log.LoadFile(path.toStdString(),TIXML_ENCODING_UTF8);
                    ticpp::Node *subRoot=log.FirstChild("users");
                    ticpp::Element *el=new ticpp::Element("user");
                    el->SetAttribute("username",utilities::Base64Encode(_user).toStdString());
                    el->SetAttribute("directory",_directory.toStdString());
                    subRoot->LinkEndChild(el);
                    log.SaveFile();
                }
                {
                    ticpp::Document doc;
                    ticpp::Element *newRoot=new ticpp::Element("config");
                    doc.LinkEndChild(newRoot);
                    QString path=libJackSMS::directories::concatDirectoryAndFile(libJackSMS::directories::XmlDirectory(),_directory);
                    path=libJackSMS::directories::concatDirectoryAndFile(path,"config.xml");
                    doc.SaveFile(path.toStdString());
                }

                {
                    ticpp::Document doc;
                    ticpp::Element *newRoot=new ticpp::Element("log");
                    doc.LinkEndChild(newRoot);
                    QString path=libJackSMS::directories::concatDirectoryAndFile(libJackSMS::directories::XmlDirectory(),_directory);
                    path=libJackSMS::directories::concatDirectoryAndFile(path,"log.xml");
                    doc.SaveFile(path.toStdString());
                }
                {
                    ticpp::Document doc;
                    ticpp::Element *newRoot=new ticpp::Element("log");
                    doc.LinkEndChild(newRoot);
                    QString path=libJackSMS::directories::concatDirectoryAndFile(libJackSMS::directories::XmlDirectory(),_directory);
                    path=libJackSMS::directories::concatDirectoryAndFile(path,"imlog.xml");
                    doc.SaveFile(path.toStdString());
                }
                {
                    ticpp::Document doc;
                    ticpp::Element *newRoot=new ticpp::Element("stats");
                    doc.LinkEndChild(newRoot);
                    QString path=libJackSMS::directories::concatDirectoryAndFile(libJackSMS::directories::XmlDirectory(),_directory);
                    path=libJackSMS::directories::concatDirectoryAndFile(path,"stats.xml");
                    doc.SaveFile(path.toStdString());
                }
                return true;


            }catch(ticpp::Exception e){
                throw libJackSMS::exceptionXmlError(e.what());
            }
            return false;
        }

        bool xmlParserLocalApiTicpp::createUsersFile() {
            try{
                ticpp::Document doc;
                ticpp::Element *newRoot = new ticpp::Element("users");
                doc.LinkEndChild(newRoot);
                QString path = libJackSMS::directories::concatDirectoryAndFile(libJackSMS::directories::XmlDirectory(), "users.xml");
                doc.SaveFile(path.toStdString());
            }catch(ticpp::Exception e){
                return false;
            }
            return true;
        }

        bool xmlParserLocalApiTicpp::saveOptions(const dataTypes::optionsType &_opzioni) {

            try {
                ticpp::Document options;
                QString path = libJackSMS::directories::concatDirectoryAndFile(libJackSMS::directories::XmlDirectory(), userDirectory);
                path = libJackSMS::directories::concatDirectoryAndFile(path, "config.xml");

                try {
                    options.LoadFile(path.toStdString(), TIXML_ENCODING_UTF8);
                } catch(ticpp::Exception e) {
                    ticpp::Document doc;
                    ticpp::Element *newRoot = new ticpp::Element("config");
                    doc.LinkEndChild(newRoot);
                    doc.SaveFile(path.toStdString());
                    doc.LoadFile(path.toStdString(), TIXML_ENCODING_UTF8);
                    options = ticpp::Document(doc);
                }

                ticpp::Node *subRoot = options.FirstChild("config");
                subRoot->Clear();

                for (dataTypes::optionsType::const_iterator i = _opzioni.begin(); i != _opzioni.end(); ++i) {
                    ticpp::Element *el = new ticpp::Element(i.key().toStdString());

                    if (!i.value().isEmpty())
                        el->SetText(i.value().toStdString());
                    subRoot->LinkEndChild(el);
                }

                options.SaveFile();

            }catch(ticpp::Exception e){
                throw exceptionXmlError(e.what());
                return false;
            }

            return true;
        }

        bool xmlParserLocalApiTicpp::deleteSmsMessage(const QList<QString> &idList){
            try{
                QString path=libJackSMS::directories::concatDirectoryAndFile(libJackSMS::directories::XmlDirectory(),userDirectory);
                path=libJackSMS::directories::concatDirectoryAndFile(path,"log.xml");
                ticpp::Document log(path.toStdString());
                log.LoadFile();

                ticpp::Node *subRoot=log.FirstChild("log");
                ticpp::Node *child=NULL;
                std::list<ticpp::Node*> nodeToDelete;
                while( (child = subRoot->IterateChildren( child )) ){
                    ticpp::Element * childElem=child->ToElement();
                    std::string currentMessageId=childElem->GetAttribute("id");

                    QList<QString>::const_iterator i=idList.begin();
                    QList<QString>::const_iterator i_end=idList.end();
                    for(;i!=i_end;++i){
                        if (i->toStdString()==currentMessageId){
                            nodeToDelete.push_back(child);
                            break;
                        }
                    }
                }

                std::list<ticpp::Node*>::const_iterator i=nodeToDelete.begin();
                std::list<ticpp::Node*>::const_iterator i_end=nodeToDelete.end();
                for(;i!=i_end;++i){
                    subRoot->RemoveChild(*i);
                }
                log.SaveFile();
            }catch(ticpp::Exception e){
                throw libJackSMS::exceptionXmlError(e.what());
            }
            return true;
        }

        bool xmlParserLocalApiTicpp::deleteImMessage(const QList<QString> &idList){
            try{
                QString path=libJackSMS::directories::concatDirectoryAndFile(libJackSMS::directories::XmlDirectory(),userDirectory);
                path=libJackSMS::directories::concatDirectoryAndFile(path,"imlog.xml");
                ticpp::Document log(path.toStdString());
                log.LoadFile();

                ticpp::Node *subRoot=log.FirstChild("log");
                ticpp::Node *child=NULL;
                std::list<ticpp::Node*> nodeToDelete;
                while( (child = subRoot->IterateChildren( child )) ){
                    ticpp::Element * childElem=child->ToElement();
                    std::string currentMessageId=childElem->GetAttribute("id");

                    QList<QString>::const_iterator i=idList.begin();
                    QList<QString>::const_iterator i_end=idList.end();
                    for(;i!=i_end;++i){
                        if (i->toStdString()==currentMessageId){
                            nodeToDelete.push_back(child);
                            break;
                        }
                    }
                }

                std::list<ticpp::Node*>::const_iterator i=nodeToDelete.begin();
                std::list<ticpp::Node*>::const_iterator i_end=nodeToDelete.end();
                for(;i!=i_end;++i){
                    subRoot->RemoveChild(*i);
                }
                log.SaveFile();
            }catch(ticpp::Exception e){
                throw libJackSMS::exceptionXmlError(e.what());
            }
            return true;
        }

        bool xmlParserLocalApiTicpp::saveServices(QString _xml){
            try{
                ticpp::Document doc;
                doc.Parse(_xml.toStdString());
                QString path=libJackSMS::directories::concatDirectoryAndFile(libJackSMS::directories::XmlDirectory(),"services.xml");
                doc.SaveFile(path.toStdString());
            }catch(ticpp::Exception e){
                throw exceptionXmlError(e.what());
                return false;
            }
            return true;

        }

        bool xmlParserLocalApiTicpp::mergeServices(QString _xml){
            try{
                ticpp::Document localServices;
                ticpp::Document newServices;
                newServices.Parse(_xml.toStdString());
                QString path=libJackSMS::directories::concatDirectoryAndFile(libJackSMS::directories::XmlDirectory(),"services.xml");
                localServices.LoadFile(path.toStdString(),TIXML_ENCODING_UTF8);


                ticpp::Node *subRootNew=newServices.FirstChild("Freesmee");
                ticpp::Node *childNew=NULL;
                while( (childNew = subRootNew->IterateChildren( childNew )) ){
                    ticpp::Element * thisServiceNew=childNew->ToElement();
                    std::string serviceIdNew=thisServiceNew->GetAttribute("id");
                    std::string serviceVersionNew=thisServiceNew->GetAttribute("v");
                    ticpp::Node *subRootLocal=localServices.FirstChild("Freesmee");
                    ticpp::Node *childLocal=NULL;
                    bool found=false;
                    while( (childLocal = subRootLocal->IterateChildren( childLocal )) ){

                        ticpp::Element * thisServiceLocal=childLocal->ToElement();
                        //std::cout <<thisServiceLocal->GetAttribute("id");
                        std::string serviceIdLocal=thisServiceLocal->GetAttribute("id");
                        std::string serviceVersionLocal=thisServiceLocal->GetAttribute("v");
                        if (serviceIdLocal==serviceIdNew){
                            if (serviceVersionLocal!=serviceVersionNew){
                                subRootLocal->RemoveChild(childLocal);

                                childLocal=subRootLocal->InsertEndChild(*childNew);


                            }
                            found=true;

                        }
                    }
                    if (!found){
                        subRootLocal=subRootLocal->InsertEndChild(*childNew);

                    }
                }
                localServices.SaveFile();
            }catch(ticpp::Exception e){
                throw exceptionXmlError(e.what());
                return false;
            }
            return true;
        }
    }
}

