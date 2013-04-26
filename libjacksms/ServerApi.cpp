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

#include <QString>
#include "Configuration.h"
#include "ServerApi.h"
#include "Utilities.h"
#include "netclientqhttp.h"
#include "xmlParserServerApi.h"
#include "xmlParserServerApiTicpp.h"
#include "VariabileServizio.h"
#include "LogSmsMessage.h"
#include <QTimer>
#include "Exceptions.h"
#include <QFile>

namespace libJackSMS {

    namespace serverApi {

        login::login(const QString &_username, const QString &_password) :
                username(_username),
                password(_password),
                l(NULL)
        {
            qRegisterMetaType<libJackSMS::dataTypes::phoneBookType>("libJackSMS::dataTypes::phoneBookType");
            qRegisterMetaType<libJackSMS::dataTypes::configuredServicesType>("libJackSMS::dataTypes::configuredServicesType");
        }

        login::~login()
        {
            if (l != NULL)
                l->~loginBase();
        }

        void login::doLogin()
        {
            start();
        }

        void login::abort()
        {
        }

        void login::run()
        {
            emit loginStarted();
            l = new loginBase(username, password);
            //connect(this,SIGNAL(abortSignal()),&man,SLOT(abort()));
            connect(l, SIGNAL(accountsReceived(libJackSMS::dataTypes::configuredServicesType)), this, SIGNAL(accountsReceived(libJackSMS::dataTypes::configuredServicesType)));
            connect(l, SIGNAL(loginFailed(QString)), this, SIGNAL(loginFailed(QString)));
            connect(l, SIGNAL(loginSuccess(QString)), this, SIGNAL(loginSuccess(QString)));
            connect(l, SIGNAL(newVersionAvailable()), this, SIGNAL(newVersionAvailable()));
            connect(l, SIGNAL(phoneBookReceived(libJackSMS::dataTypes::phoneBookType)), this, SIGNAL(phoneBookReceived(libJackSMS::dataTypes::phoneBookType)));
            l->doLogin();
        }

        loginBase::loginBase(const QString &_username, const QString &_password) :
                username(_username),
                password(_password),
                xmlResponse(new xmlParserApi::xmlParserServerApiTicpp())
        {
        }

        loginBase::~loginBase()
        {
            xmlResponse->~xmlParserServerApiGeneric();
        }

        void loginBase::doLogin()
        {
            webClient = new netClient::netClientQHttp();
            webClient->insertFormData("user",     username.toUtf8().toPercentEncoding());
            webClient->insertFormData("password", password.toUtf8().toPercentEncoding());

            QString xml = webClient->submitPost("http://api.freesmee.com/desktopLogin?desktop=" + QString(FREESMEE_VERSION) + "&o=xml", true);

            try {
                if (xml.isEmpty())
                    emit loginFailed("Il server di login non ha risposto correttamante.");
                else if (webClient->hasError())
                    emit loginFailed("Errore: " + xml);
                else {
                    xmlResponse->setXml(xml);
                    if (xmlResponse->checkIsLogged()) {
                        emit loginSuccess(xmlResponse->getSessionId());
                        dataTypes::phoneBookType _rubrica;
                        xmlResponse->loadPhoneBook(_rubrica);
                        emit phoneBookReceived(_rubrica);
                        dataTypes::configuredServicesType _serviziConfigurati;
                        xmlResponse->loadAccounts(_serviziConfigurati);
                        emit accountsReceived(_serviziConfigurati);
                        dataTypesApi::clientVersion v(xmlResponse->getVersion());
                        dataTypesApi::clientVersion thisVersion(FREESMEE_VERSION);
                        if (thisVersion < v) {
                            emit newVersionAvailable();
                        }

                    } else {
                        emit loginFailed(xmlResponse->getError());
                    }
                }

            } catch(libJackSMS::exceptionXmlError e) {
                emit loginFailed("Errore sconosciuto.");
            } catch(...) {
                emit loginFailed("Impossibile Effettuare il Login, verificare di avere accesso alla rete.");
            }
        }

        pingator::pingator(const QString &_idsessione) :
                idsessione(_idsessione),
                minutes(30)
        {
        }

        pingator::~pingator()
        {
            timer.stop();
        }

        void pingator::setPingInterval(int min)
        {
            minutes = min;
        }

        void pingator::launchPing()
        {
            try
            {
                timer.stop();
                timer.singleShot(minutes * 60 * 1000, this, SLOT(launchPing()));
                start();
            } catch(libJackSMS::exceptionXmlError e) {
            } catch(libJackSMS::exceptionSomethingWrong e) {
            } catch(...) {}
        }

        void pingator::run()
        {
            try
            {
                //xmlParserApi::xmlParserServerApiTicpp xmlResponse;

                netClient::netClientQHttp webClient;

                webClient.setUrl("http://api.freesmee.com/ping?desktop=" + QString(FREESMEE_VERSION) + "&token=" + idsessione + "&o=xml");
                QString xml = webClient.readPage(true);
                emit pinged();
                //xmlDocument->setXml(xml);
                //std::sring xmlDocument->checkPing(this);

            } catch(libJackSMS::exceptionXmlError e) {
            } catch(libJackSMS::exceptionSomethingWrong e) {
            } catch(...) {}
        }

        void synchronizeVariables(dataTypes::configuredServicesType &_serviziConfigurati, const dataTypes::servicesType &_servizi)
        {
            for (dataTypes::configuredServicesType::iterator i = _serviziConfigurati.begin(); i != _serviziConfigurati.end(); ++i)
            {
                dataTypes::servicesType::const_iterator iter = _servizi.find(i.value().getService());
                if (iter != _servizi.end()) {
                    dataTypes::service servizio = iter.value();
                    while (servizio.nextVar()) {
                        dataTypes::variabileServizio var = servizio.currentVar();
                        i.value().setData(var.getName(),i.value().getData("data"+var.getProgressive()));
                        //std::cout<<(servizio.getName()+" sostituita data"+var.getProgressive()+" con "+var.getName()+" (valore "+i.value().getData("data"+var.getProgressive())+")")<<std::endl;;
                    }
                }
            }
        }

        instantMessenger::instantMessenger(const QString &_loginId) :
            loginId(_loginId),
            webClient(new netClient::netClientQHttp())
        {
        }

        bool instantMessenger::checkMessages()
        {
            try
            {
                webClient->setUrl("http://api.freesmee.com/getDeleteQueue?desktop=" + QString(FREESMEE_VERSION) + "&token=" + loginId + "&o=xml");
                QString xml = webClient->readPage(true);

                xmlParserApi::xmlParserServerApiGeneric *xmlResponse = new xmlParserApi::xmlParserServerApiTicpp();

                if (xml.isEmpty())
                    return false;
                else if (webClient->hasError())
                    return false;
                else {
                    xmlResponse->setXml(xml);

                    // Controllo che non abbia trovato errori
                    QString error = xmlResponse->getError();

                    if (!error.isEmpty())
                    {
                        // errori trovati

                        // working
                        // emit
                        return false;
                    }

                    xmlResponse->extractImMessages(messages);
                    if (messages.size() == 0)
                        return false;
                    else
                        return true;
                }
                return false;
            } catch (...) {
                throw exceptionGeneric();
            }
        }

        bool instantMessenger::getMessages(libJackSMS::dataTypes::logImType &_messages)
        {
            _messages.clear();
            _messages = messages;
            return true;
        }

        smsLogFailed::smsLogFailed(const QString _loginId) :
            loginId(_loginId)
        {
        }

        void smsLogFailed::reportFail(dataTypes::logSmsMessage _msg, QString _error)
        {
            msg = _msg;
            error = _error;
            start();
        }

        void smsLogFailed::run()
        {
            xmlParserApi::xmlParserServerApiTicpp xmlResponse;
            netClient::netClientQHttp webClient;

            webClient.insertFormData("message",     msg.getText().toUtf8().toPercentEncoding());
            webClient.insertFormData("recipient",   msg.getPhoneNumber().toString().toUtf8().toPercentEncoding());
            webClient.insertFormData("service_id",  msg.getServiceId());
            webClient.insertFormData("account_id",  msg.getAccountId());
            webClient.insertFormData("fail_reason", error.toUtf8().toPercentEncoding());

            QString xml = webClient.submitPost("http://api.freesmee.com/desktopSyncFail?desktop=" + QString(FREESMEE_VERSION) + "&token=" + loginId + "&o=xml", true);

            if (xml.isEmpty())
                emit reportFailed();
            else if (webClient.hasError())
                emit reportFailed();
            else {
                xmlResponse.setXml(xml);

                if (xmlResponse.checkReport())
                    emit reportSuccess();
                else
                    emit reportFailed();
            }
        }

        smsLogSaver::smsLogSaver(const QString _loginId) :
            loginId(_loginId)
        {
            qRegisterMetaType<libJackSMS::dataTypes::logSmsMessage>("libJackSMS::dataTypes::logSmsMessage");
        }

        void smsLogSaver::save(dataTypes::logSmsMessage _msg) {
            msg = _msg;
            start();
        }

        void smsLogSaver::run()
        {
            xmlParserApi::xmlParserServerApiTicpp xmlResponse;
            netClient::netClientQHttp webClient;

            webClient.insertFormData("message",    msg.getText().toUtf8().toPercentEncoding());
            webClient.insertFormData("recipient",  msg.getPhoneNumber().toString().toUtf8().toPercentEncoding());
            webClient.insertFormData("service_id", msg.getServiceId());
            webClient.insertFormData("account_id", msg.getAccountId());

            QString xml = webClient.submitPost("http://api.freesmee.com/desktopSync?desktop=" + QString(FREESMEE_VERSION) + "&token=" + loginId + "&o=xml", true);

            if (xml.isEmpty())
                emit smsNotSaved();
            else if (webClient.hasError())
                emit smsNotSaved();
            else {
                xmlResponse.setXml(xml);
                QString id;
                QString total = "0";
                if (xmlResponse.checkSaved(id, total)) {
                    msg.setId(id);
                    emit smsSaved(msg,total);
                } else
                    emit smsNotSaved();
            }
        }

        contactManager::contactManager(const QString &_loginId) :
                loginId(_loginId)
        {
        }

        void contactManager::addNewContact(libJackSMS::dataTypes::contact _contatto)
        {
            manAdd = new contactManagerAdd(loginId);
            connect(manAdd, SIGNAL(contactAdded(QString, bool, int)), this, SIGNAL(contactSaved(QString, bool, int)));
            connect(manAdd, SIGNAL(errorAdd()), this, SIGNAL(contactNotSaved()));
            manAdd->addNewContact(_contatto);
        }

        void contactManager::updateContact(libJackSMS::dataTypes::contact _contatto)
        {
            manUp = new contactManagerUpdate(loginId);
            connect(manUp, SIGNAL(contactUpdated(libJackSMS::dataTypes::contact)), this, SIGNAL(contactUpdated(libJackSMS::dataTypes::contact)));
            connect(manUp, SIGNAL(errorUpdate()), this, SIGNAL(contactNotUpdated()));
            manUp->updateContact(_contatto);
        }

        void contactManager::deleteContact(QString _id)
        {
            manDel = new contactManagerDelete(loginId);
            connect(manDel, SIGNAL(contactDeleted(QString)), this, SIGNAL(contactDeleted(QString)));
            connect(manDel, SIGNAL(errorDelete()), this, SIGNAL(contactNotDeleted()));
            manDel->deleteContact(_id);
        }

        /****************************/

        void contactManagerDelete::run()
        {
            netClient::netClientQHttp webClient;
            libJackSMS::xmlParserApi::xmlParserServerApiTicpp xmlDocument;

            webClient.insertFormData("ids", id.toUtf8().toPercentEncoding());

            QString xml = webClient.submitPost("http://api.freesmee.com/delAbook?desktop=" + QString(FREESMEE_VERSION) + "&token=" + loginId + "&o=xml", true);

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

        contactManagerDelete::contactManagerDelete(const QString &_loginId) :
                loginId(_loginId)
        {
        }

        void contactManagerDelete::deleteContact(QString _id)
        {
            id = _id;
            start();
        }

        /******************************/
        /****************************/

        void accountManagerDelete::run()
        {
            netClient::netClientQHttp webClient;
            libJackSMS::xmlParserApi::xmlParserServerApiTicpp xmlDocument;

            webClient.insertFormData("ids", id.toUtf8().toPercentEncoding());

            QString xml = webClient.submitPost("http://api.freesmee.com/delService?desktop=" + QString(FREESMEE_VERSION) + "&token=" + loginId + "&o=xml", true);

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

        accountManagerDelete::accountManagerDelete(const QString &_loginId) :
                loginId(_loginId)
        {
        }

        void accountManagerDelete::deleteAccount(const QString &_id)
        {
            id = _id;
            start();
        }

        /******************************/
        /********************************/

        void accountManagerAdd::run()
        {
            netClient::netClientQHttp webClient;
            libJackSMS::xmlParserApi::xmlParserServerApiTicpp xmlDocument;

            webClient.insertFormData("service_id",   account.getService().toUtf8().toPercentEncoding());
            webClient.insertFormData("account_name", account.getName().toUtf8().toPercentEncoding());

            int c = 0;

            while(service.nextVar())
            {
                   ++c;
                   webClient.insertFormData("data_" + service.currentVar().getProgressive(), account.getData(service.currentVar().getName()).toUtf8().toPercentEncoding());
            }

            for (int x = c + 1; x < 5; ++x) {
                webClient.insertFormData("data_" + QString::number(x), "");
            }

            QString xml = webClient.submitPost("http://api.freesmee.com/addService?desktop=" + QString(FREESMEE_VERSION) + "&token=" + loginId + "&o=xml", true);
            if (xml.isEmpty()) {
                emit errorAdd();
            } else if (webClient.hasError()) {
                emit errorAdd();
            } else {
                xmlDocument.setXml(xml);
                QString resultId;
                if (xmlDocument.checkAddNewAccount(resultId))
                    emit accountAdded(resultId);
                else
                    emit errorAdd();
            }
        }

        accountManagerAdd::accountManagerAdd(const QString &_loginId) :
                loginId(_loginId)
        {
        }

        void accountManagerAdd::addNewAccount(libJackSMS::dataTypes::service _service, libJackSMS::dataTypes::configuredAccount &_account)
        {
            service = _service;
            account = _account;
            start();
        }

        /********************************/

        void contactManagerAdd::run()
        {
            libJackSMS::xmlParserApi::xmlParserServerApiTicpp xmlDocument;
            netClient::netClientQHttp webClient;

            webClient.insertFormData("contact_name",   contatto.getName().toUtf8().toPercentEncoding());
            webClient.insertFormData("contact_number", contatto.getPhone().toString().toUtf8().toPercentEncoding());
            webClient.insertFormData("account_id",     contatto.getAccount().toUtf8().toPercentEncoding());
            webClient.insertFormData("preferred",      "1");

            QString xml = webClient.submitPost("http://api.freesmee.com/addAbook?desktop=" + QString(FREESMEE_VERSION) + "&token=" + loginId + "&o=xml", true);

            if (xml.isEmpty())
                emit errorAdd();
            else if (webClient.hasError())
                emit errorAdd();
            else {
                xmlDocument.setXml(xml);
                QString resultId;
                bool canReceiveJms;
                int carrier;

                if(xmlDocument.checkAddNewContact(resultId, canReceiveJms, carrier))
                    emit contactAdded(resultId, canReceiveJms, carrier);
                else
                    emit errorAdd();
            }
        }

        contactManagerAdd::contactManagerAdd(const QString &_loginId) :
                loginId(_loginId)
        {
        }

        void contactManagerAdd::addNewContact(libJackSMS::dataTypes::contact _contatto)
        {
            contatto = _contatto;
            start();
        }

        /***********************************/

        void contactManagerUpdate::run()
        {
            libJackSMS::xmlParserApi::xmlParserServerApiTicpp xmlDocument;
            netClient::netClientQHttp webClient;

            webClient.insertFormData("id",             contatto.getId());
            webClient.insertFormData("contact_name",   contatto.getName().toUtf8().toPercentEncoding());
            webClient.insertFormData("contact_number", contatto.getPhone().toString().toUtf8().toPercentEncoding());
            webClient.insertFormData("account_id",     contatto.getAccount());

            QString xml = webClient.submitPost("http://api.freesmee.com/editAbook?desktop=" + QString(FREESMEE_VERSION) + "&token=" + loginId + "&o=xml", true);

            if (xml.isEmpty() || webClient.hasError())
            {
                emit errorUpdate();
            }
            else
            {
                xmlDocument.setXml(xml);
                int carrier;
                bool canReceiveJms;

                if(xmlDocument.checkUpdateContact(canReceiveJms, carrier))
                {
                    contatto.setCarrier(carrier);
                    contatto.setCanReceiveJms(canReceiveJms);

                    emit contactUpdated(contatto);
                }
                else
                {
                    emit errorUpdate();
                }
            }
        }

        contactManagerUpdate::contactManagerUpdate(const QString &_loginId)
            :loginId(_loginId)
        {
            qRegisterMetaType<libJackSMS::dataTypes::contact>("libJackSMS::dataTypes::contact");
        }

        void contactManagerUpdate::updateContact(libJackSMS::dataTypes::contact _contatto)
        {
            contatto=_contatto;
            start();
        }

        /*********************************/

        void accountManagerUpdate::run()
        {
            libJackSMS::xmlParserApi::xmlParserServerApiTicpp xmlDocument;
            netClient::netClientQHttp webClient;

            webClient.insertFormData("id",           account.getId());
            webClient.insertFormData("account_name", account.getName().toUtf8().toPercentEncoding());
            int i = 0;

            while (s.nextVar())
            {
                ++i;
                libJackSMS::dataTypes::variabileServizio var = s.currentVar();
                QString prog = var.getProgressive();
                QString name = var.getName();
                webClient.insertFormData("data_" + prog, account.getData(name).toUtf8().toPercentEncoding());
            }

            if (i < 4)
            {
                for (int x = i + 1; x <= 4; ++x)
                    webClient.insertFormData("data_" + QString::number(x), "");
            }

            QString xml = webClient.submitPost("http://api.freesmee.com/editService?desktop=" + QString(FREESMEE_VERSION) + "&token=" + loginId + "&o=xml", true);

            if (xml.isEmpty())
            {
                emit errorUpdate();
            }
            else if (webClient.hasError())
            {
                emit errorUpdate();
            }
            else
            {
                xmlDocument.setXml(xml);

                if(xmlDocument.checkUpdateAccount())
                    emit accountUpdated(account);
                else
                    emit errorUpdate();
            }
        }

        accountManagerUpdate::accountManagerUpdate(const QString &_loginId, libJackSMS::dataTypes::service _s) :
                loginId(_loginId),
                s(_s)
        {
            qRegisterMetaType<libJackSMS::dataTypes::configuredAccount>("libJackSMS::dataTypes::configuredAccount");
        }

        void accountManagerUpdate::updateAccount(libJackSMS::dataTypes::configuredAccount _account)
        {
            account = _account;
            start();
        }

        /***********************************/

        accountManager::accountManager(const QString &_loginId) :
                loginId(_loginId)
        {
        }

        void accountManager::addNewAccount(libJackSMS::dataTypes::service _service,libJackSMS::dataTypes::configuredAccount & _account){
            manAdd = new accountManagerAdd(loginId);
            connect(manAdd,SIGNAL(accountAdded(QString)),this,SIGNAL(accountSaved(QString)));
            connect(manAdd,SIGNAL(errorAdd()),this,SIGNAL(accountNotSaved()));
            manAdd->addNewAccount(_service,_account);
        }

        void accountManager::updateAccount(libJackSMS::dataTypes::configuredAccount &_account, libJackSMS::dataTypes::service s)
        {
            manUp = new accountManagerUpdate(loginId, s);
            connect(manUp, SIGNAL(accountUpdated(libJackSMS::dataTypes::configuredAccount)), this, SIGNAL(accountUpdated(libJackSMS::dataTypes::configuredAccount)));
            connect(manUp, SIGNAL(errorUpdate()), this, SIGNAL(accountNotUpdated()));
            manUp->updateAccount(_account);
        }

        void accountManager::deleteAccount(QString _id)
        {
            manDel = new accountManagerDelete(loginId);
            connect(manDel, SIGNAL(accountDeleted(QString)), this, SIGNAL(accountDeleted(QString)));
            connect(manDel, SIGNAL(errorDelete()), this, SIGNAL(accountNotDeleted()));
            manDel->deleteAccount(_id);
        }

        reloader::reloader(const QString &_loginId) :
                xmlDocument(new libJackSMS::xmlParserApi::xmlParserServerApiTicpp()),
                loginId(_loginId),
                webClient(new netClient::netClientQHttp())
        {
            qRegisterMetaType<libJackSMS::dataTypes::phoneBookType>("libJackSMS::dataTypes::phoneBookType");
        }

        bool reloader::reloadPhoneBook()
        {
            webClient->setUrl("http://api.freesmee.com/getAbookFullJMS?desktop=" + QString(FREESMEE_VERSION) + "&token=" + loginId + "&o=xml");
            QString xml = webClient->readPage(true);
            xmlDocument->setXml(xml);
            libJackSMS::dataTypes::phoneBookType _rubrica;
            if (xmlDocument->loadPhoneBook2(_rubrica)) {
                emit phoneBookReloaded(_rubrica);
                return true;
            } else {
                emit errorReload();
                return false;
            }

        }

//        conversationManager::conversationManager(const QString &_loginId) :
//                xmlDocument(new libJackSMS::xmlParserApi::xmlParserServerApiTicpp()),
//                loginId(_loginId),
//                webClient(new netClient::netClientQHttp())
//        {
//        }

//        bool conversationManager::downloadLastMessages(libJackSMS::dataTypes::logSmsType & _logSms, libJackSMS::dataTypes::logImType &_logIm)
//        {
//            webClient->setUrl("http://api.freesmee.com/getConversation?desktop=" + QString(FREESMEE_VERSION) + "&token=" + idsessione + "&o=xml");
//            QString xml=webClient->readPage(true);

//            xmlDocument->setXml(xml);
//            return xmlDocument->parseConversation(_logSms,_logIm);
//        }

        void updateServicesManager::run()
        {
            try {
                updateServicesManagerBase man(loginId);
                connect(this, SIGNAL(abortSignal()), &man, SLOT(abort()));

                if (man.downloadUpdates(servizi))
                    if (!aborted)
                        emit updatesAvailable(servizi, man.getXml(), man.getMessage());

            } catch(libJackSMS::exceptionXmlError e) {
                emit criticalError(e.what());
            } catch(...) {
                emit criticalError("Errore nella procedura updateServicesManager::run()");
            }
        }

        updateServicesManager::updateServicesManager(const QString &_loginId, libJackSMS::dataTypes::servicesType _servizi) :
                loginId(_loginId),
                servizi(_servizi),
                aborted(false)
        {
            qRegisterMetaType<libJackSMS::dataTypes::servicesType>("libJackSMS::dataTypes::servicesType");
        }

        void updateServicesManager::checkUpdates()
        {
            start();
        }

        void updateServicesManager::abort()
        {
            emit abortSignal();
            aborted = true;
        }

        updateServicesManagerBase::updateServicesManagerBase(const QString &_loginId) :
                xmlDocument(new libJackSMS::xmlParserApi::xmlParserServerApiTicpp()),
                loginId(_loginId),
                webClient(new netClient::netClientQHttp()),
                aborted(false)
        {
        }

        QString updateServicesManagerBase::getMessage() const
        {
            QString resAdd, resUp, resDel;

            for (QList< QPair<QString,QString> >::const_iterator i = updateResults.begin(); i != updateResults.end(); ++i)
            {
                if (i->first == "a")
                    resAdd = (resAdd.isEmpty() ? "" : ", ") + i->second;

                else if (i->first == "u")
                    resUp = (resUp.isEmpty() ? "" : ", ") + i->second;

                else if (i->first == "d")
                    resDel = (resDel.isEmpty() ? "" : ", ") + i->second;
            }

            QString final;

            if (!resAdd.isEmpty())
                final = "Aggiunti i seguenti servizi:\n" + resAdd + "\n\n";

            if (!resUp.isEmpty())
                final = final + "Aggiornati i seguenti servizi:\n" + resUp;

            return final;
        }

        QString updateServicesManagerBase::getXml() const
        {
            return servXml;
        }

        void updateServicesManagerBase::abort()
        {
            aborted = true;
            webClient->interrupt();
        }

        bool updateServicesManagerBase::downloadUpdates(libJackSMS::dataTypes::servicesType &_servizi)
        {
            bool result = false;
            for (libJackSMS::dataTypes::servicesType::iterator i = _servizi.begin(); i != _servizi.end(); ++i)
            {
                QString id = i.value().getId();

                // gli id oltre il mille sono per usi custom
                if (id.toInt() <= 1000)
                    webClient->insertFormData(id, i.value().getVersion());
            }

            QString xml = webClient->submitPost("http://api.freesmee.com/servicesFullXML?desktop=" + QString(FREESMEE_VERSION), true);

            if (!xml.isEmpty() && !webClient->hasError())
            {
                if (!aborted)
                {
                    xmlDocument->setXml(xml);
                    servXml = xml;
                    libJackSMS::dataTypes::servicesType  nuoviServizi;
                    xmlDocument->parseServices(nuoviServizi);

                    {
                        for (libJackSMS::dataTypes::servicesType::iterator i = nuoviServizi.begin(); i != nuoviServizi.end(); ++i)
                        {
                            libJackSMS::dataTypes::servicesType::iterator x = _servizi.find(i.value().getId());

                            if (x == _servizi.end())
                            {
                                result = true;
                                _servizi.insert(i.value().getId(), i.value());
                                updateResults.push_back(qMakePair(QString("a"), i.value().getName() + " (v" + i.value().getVersion() + ")"));
                            }
                            else
                            {
                                if (i.value().getVersion() != x.value().getVersion())
                                {
                                    updateResults.push_back(qMakePair(QString("u"), i.value().getName()));
                                    x.value() = i.value();
                                    result = true;
                                }
                            }
                        }

                        // TODO: controllare se il servizio è stato cancellato
                    }

                    return result;
                }
            }

            //prevent compile warning.
            //the execution shouldn't reach this.
            return false;
        }

        void cyclicMessengerChecker::run()
        {
            try
            {
                libJackSMS::serverApi::instantMessenger im(loginString);

                if (im.checkMessages()) {
                    libJackSMS::dataTypes::logImType msgs;
                    im.getMessages(msgs);
                    emit newJMS(msgs);
                }

                emit serviceActive();

            } catch(...) {
                emit serviceNotActive();
            }
        }

        cyclicMessengerChecker::cyclicMessengerChecker(QString _loginString) :
                loginString(_loginString)
        {
            qRegisterMetaType<libJackSMS::dataTypes::logImType>("libJackSMS::dataTypes::logImType");
            connect(&timeout, SIGNAL(timeout()), this, SLOT(run()));
        }

        cyclicMessengerChecker::~cyclicMessengerChecker() {
        }

        void cyclicMessengerChecker::activateServ() {
            start();
            timeout.start(60 * 1000);
        }

        void cyclicMessengerChecker::stop() {
            timeout.stop();
            emit serviceNotActive();
        }

        /**********************gmail importer***********************/

        void gmailAddressBookImporter::run()
        {
            libJackSMS::xmlParserApi::xmlParserServerApiTicpp *xmlDocument = new libJackSMS::xmlParserApi::xmlParserServerApiTicpp();
            netClient::netClientQHttp *webClient = new netClient::netClientQHttp();

            webClient->setUserAgent("Mozilla/5.0 (Windows; U; Windows NT 5.1; it; rv:1.8.1.3) Gecko/20070309 Firefox/2.0.0.3");

            webClient->insertFormData("email",    gmailuser.toUtf8().toPercentEncoding());
            webClient->insertFormData("password", gmailpsw.toUtf8().toPercentEncoding());

            QString xml = webClient->submitPost("http://api.freesmee.com/importAbookFromGoogle?desktop=" + QString(FREESMEE_VERSION) + "&token=" + loginId + "&o=xml", true);

            if (xml.isEmpty())
            {
                emit importError("Empty response");
            }
            else if (webClient->hasError())
            {
                emit importError("Unexpected response");
            }
            else
            {
                xmlDocument->setXml(xml);
                int count;
                if (xmlDocument->checkGmailImport(count))
                {
                    emit importDone(count);
                } else {
                    QString e = xmlDocument->gmailError();
                    if (e == "1")
                        emit wrongCredentials();
                    else
                        emit importError("Unexpected XML");
                }
            }
        }

        gmailAddressBookImporter::gmailAddressBookImporter(QString _loginId) :
                loginId(_loginId)
        {
        }

        void gmailAddressBookImporter::import(QString _user, QString _psw)
        {
            gmailuser = _user;
            gmailpsw = _psw;
            start();
        }

    }
}
