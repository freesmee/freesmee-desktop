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

#include "DataTypes.h"
#include <QSharedMemory>
#include <QSemaphore>

#include "SmsSender.h"
#include "ShortMessage.h"
#include "NetClient.h"
#include "netclientqhttp.h"
#include "Logger.h"
#include "ProxyConfig.h"
#include "EncodingUrl.h"
#include "SocketClient.h"
#include "FilesDirectory.h"
#include "Utilities.h"
#include "Exceptions.h"

namespace libJackSMS{

    /********************smssender****************/
    smsSender::smsSender(const dataTypes::servicesType & _services, const dataTypes::proxySettings &_ps):
            servizi(_services),
            ps(_ps){

    }
    void smsSender::setRecipient(const dataTypes::phoneNumber & _dest){
        destinatario=_dest;
    }
    void smsSender::setMessage(const dataTypes::shortMessage & _message){
        messaggio=_message;
    }
    void smsSender::setAccount(const dataTypes::configuredAccount &_account){
        account=_account;
    }
    void smsSender::send(){
        start();
    }
    void smsSender::run(){
        try{
            smsSenderBase sndr(servizi,ps);
            connect(this,SIGNAL(abortSignal()),&sndr,SLOT(abort()));
            sndr.setRecipient(destinatario);
            sndr.setMessage(messaggio);
            sndr.setAccount(account);
            connect(&sndr,SIGNAL(operation()),this,SLOT(slotOperation()));
            connect(&sndr,SIGNAL(operation(QString)),this,SLOT(slotError(QString)));
            connect(&sndr,SIGNAL(error(QString)),this,SLOT(slotError(QString)));
            connect(&sndr,SIGNAL(success(QString)),this,SLOT(slotSuccess(QString)));
            connect(&sndr,SIGNAL(captcha(QByteArray,QSemaphore*)),this,SLOT(slotCaptcha(QByteArray,QSemaphore*)));
            sndr.send();
        }catch(libJackSMS::netClient::abortedException e){


        }catch(libJackSMS::exceptionSharedMemory e){
            emit error("Cannot create shared memory");

        }

    }
    void smsSender::abort(){
        emit abortSignal();
    }

    void smsSender::slotOperation(){emit operation();}
    void smsSender::slotOperation(QString s){emit operation(s);}
    void smsSender::slotError(QString s){emit error(s);}
    void smsSender::slotSuccess(QString s ){emit success(s);}
    void smsSender::slotCaptcha(QByteArray a,QSemaphore* b){
        emit captcha(a,b);
    }



    /********************end smssender****************/

    smsSenderBase::smsSenderBase(const dataTypes::servicesType & _services, const dataTypes::proxySettings &_ps):
            servizi(_services),
            ps(_ps){

    }
    void smsSenderBase::setRecipient(const dataTypes::phoneNumber & _dest){
        destinatario=_dest;
    }
    void smsSenderBase::setMessage(const dataTypes::shortMessage & _message){
        messaggio=_message;
    }
    void smsSenderBase::setAccount(dataTypes::configuredAccount _account){
        account=_account;
    }

    QString smsSenderBase::substitute(QString _input,const dataTypes::contentType &_cont,bool replace_encoded){
        dataTypes::contentType::const_iterator i=_cont.begin();
        dataTypes::contentType::const_iterator i_end=_cont.end();
        for(;i!=i_end;++i){
            QString code="%%"+i->getName()+"%%";
            if (replace_encoded)
                _input=_input.replace(code,i->getValue());
            else
                _input=_input.replace(code,i->getUnEncodedValue());
        }
        return _input;
    }

    QString smsSenderBase::substitute(QString _input,const dataTypes::variousType &_cont){
        dataTypes::variousType::const_iterator i=_cont.begin();
        dataTypes::variousType::const_iterator i_end=_cont.end();
        for(;i!=i_end;++i){
            QString code="%%"+i.key()+"%%";
            _input=_input.replace(code,i.value());
        }
        return _input;
    }
    void smsSenderBase::abort(){
        webClient->interrupt();
    }

    void smsSenderBase::send(){
        libJackSMS::logger log;
         libJackSMS::dataTypes::servicesType::const_iterator service_iter=servizi.find(account.getService());
        if (service_iter==servizi.end()){
            log.addNotice("non esiste alcun servizio con questo id.");
            emit error("Servizio inesistente.");
        }else{
            bool resultSend=false;
            bool resultError=false;
            dataTypes::service servizioDaUsare=service_iter.value();
            webClient=new netClient::netClientQHttp();
            if (ps.useProxy()){
                webClient->setProxyServer(ps.getServer(),ps.getPort(),ps.getType());
                if (ps.useAuth())
                    webClient->setProxyAuthentication(ps.getUsername(),ps.getPassword());

            }


            webClient->setUserAgent("Mozilla/5.0 (Windows; U; Windows NT 5.1; it; rv:1.8.1.3) Gecko/20070309 Firefox/2.0.0.3");
            //webClient->SetCookie(directories::CookiesDirectory()+"cookie");
            webClient->setUseCookie(true);
            webClient->setCookieFile(libJackSMS::directories::concatDirectoryAndFile(libJackSMS::directories::CookiesDirectory(),account.getId()+".cookie"),true);
            //webClient->IncludeHeaders();
            dataTypes::contentType elenco_contenuti;
            dataTypes::variousType elenco_dati_vari;
            dataTypes::creditsType elenco_credenziali;


            elenco_dati_vari.insert(QString("message"),servizioDaUsare.getEncodedTextUrlEncoded(messaggio.getText()));
            elenco_dati_vari.insert(QString("intcode"),servizioDaUsare.getEncodedTextUrlEncoded(destinatario.getIntCode()));
            elenco_dati_vari.insert(QString("intpref"),servizioDaUsare.getEncodedTextUrlEncoded(destinatario.getIntPref()));
            elenco_dati_vari.insert(QString("pref"),servizioDaUsare.getEncodedTextUrlEncoded(destinatario.getPref()));
            elenco_dati_vari.insert(QString("num"),servizioDaUsare.getEncodedTextUrlEncoded(destinatario.getNum()));
            elenco_dati_vari.insert(QString("intnum"),servizioDaUsare.getEncodedTextUrlEncoded(destinatario.getIntNum()));

            while(servizioDaUsare.nextVar()){
                QString n=servizioDaUsare.currentVar().getName();
                QString aa=account.getData(n);
                elenco_credenziali.insert(n,servizioDaUsare.getEncodedTextUrlEncoded(aa));
            }



            int pageNumber=1;
            while(servizioDaUsare.nextPage()){
                if (webClient->getAborted())
                    throw netClient::abortedException();
                dataTypes::paginaServizio & paginaCorrente=servizioDaUsare.currentPage();
                bool doPage=true;
                if (paginaCorrente.hasCondition()){
                    doPage=false;
                    QString condizione=paginaCorrente.getCondition();
                    dataTypes::contentType::const_iterator i=elenco_contenuti.begin();
                    dataTypes::contentType::const_iterator i_end=elenco_contenuti.end();
                    for(;i!=i_end;++i){
                        QString code="%%"+i->getName()+"%%";
                        if (condizione==code && i->getFound()){
                            doPage=true;
                            break;
                        }

                    }
                    if (!doPage){
                        dataTypes::variousType::const_iterator i=elenco_dati_vari.begin();
                        dataTypes::variousType::const_iterator i_end=elenco_dati_vari.end();
                        for(;i!=i_end;++i){
                            QString code="%%"+i.key()+"%%";
                            if (condizione==code && (!i.value().isEmpty())){
                                doPage=true;
                                break;
                            }
                        }
                    }
                    if (!doPage){
                        dataTypes::creditsType::const_iterator i=elenco_credenziali.begin();
                        dataTypes::creditsType::const_iterator i_end=elenco_credenziali.end();
                        for(;i!=i_end;++i){
                            QString code="%%"+i.key()+"%%";
                            if (condizione==code && (!i.value().isEmpty())){
                                doPage=true;
                                break;
                            }
                        }
                    }
                }
                if (doPage){
                    QString parsedCurrentUrlPage=paginaCorrente.getUrl();

                    parsedCurrentUrlPage=substitute(parsedCurrentUrlPage,elenco_credenziali);
                    parsedCurrentUrlPage=substitute(parsedCurrentUrlPage,elenco_dati_vari);
                    parsedCurrentUrlPage=substitute(parsedCurrentUrlPage,elenco_contenuti,false);

                    //std::cout <<"[ELABORAZIONE] Pagina: "<<parsedCurrentUrlPage.toStdString()<<std::endl;
                    log.addNotice("Pagina: "+parsedCurrentUrlPage);
                    emit operation();
                    if (paginaCorrente.getIsCaptcha()){
                        log.addNotice("Eseguita come pagina captcha");

                        while(paginaCorrente.nextHeader()){
                            QString headerVal=paginaCorrente.currentHeader().getValue();

                            headerVal=substitute(headerVal,elenco_credenziali);
                            headerVal=substitute(headerVal,elenco_dati_vari);
                            headerVal=substitute(headerVal,elenco_contenuti,false);

                            webClient->addHeader(paginaCorrente.currentHeader().getName(),headerVal);
                            log.addNotice("Aggiunto header: "+paginaCorrente.currentHeader().getName()+" - "+headerVal);
                        }
                        QByteArray captchaBytes;
                        webClient->getFile(parsedCurrentUrlPage,captchaBytes);
                        log.addNotice("Captcha ricevuto: bytes="+QString::number(captchaBytes.size()));

                        QString captcha_result;

                        QSemaphore semaforoCaptcha(0);

                        /**/
                        QSharedMemory memoriaRisultatoCaptcha("jacksms_result_captcha_shmem");
                        if (!memoriaRisultatoCaptcha.create(captchaBytes.size())){

                            if (memoriaRisultatoCaptcha.error()==QSharedMemory::AlreadyExists){
                                if (!memoriaRisultatoCaptcha.attach()){
                                    throw libJackSMS::exceptionSharedMemory();
                                }
                            }else{
                                throw libJackSMS::exceptionSharedMemory();
                            }

                        }

                        emit captcha(captchaBytes,&semaforoCaptcha);
                        semaforoCaptcha.acquire(1);
                        memoriaRisultatoCaptcha.lock();
                        captcha_result=QString((const char*)memoriaRisultatoCaptcha.constData());
                        memoriaRisultatoCaptcha.unlock();
                        memoriaRisultatoCaptcha.detach();
                        /**/
                        elenco_dati_vari.insert("captcha_value",servizioDaUsare.getEncodedTextUrlEncoded(captcha_result));

                    }else{
                        if (paginaCorrente.hasRawCommands()){

                            elenco_dati_vari.insert(QString("message"),messaggio.getText());
                            elenco_dati_vari.insert(QString("intcode"),destinatario.getIntCode());
                            elenco_dati_vari.insert(QString("intpref"),destinatario.getIntPref());
                            elenco_dati_vari.insert(QString("pref"),destinatario.getPref());
                            elenco_dati_vari.insert(QString("num"),destinatario.getNum());
                            elenco_dati_vari.insert(QString("intnum"),destinatario.getIntNum());
                            elenco_dati_vari["message"]=messaggio.getText();
                            while(servizioDaUsare.nextVar()){
                                QString n=servizioDaUsare.currentVar().getName();
                                QString aa=account.getData(n);
                                elenco_credenziali.insert(n,aa);
                            }

                            netClient::SocketClient *rawClient=new netClient::SocketClient(false);
                            QString server=parsedCurrentUrlPage;
                            server=substitute(server,elenco_credenziali);
                            server=substitute(server,elenco_dati_vari);
                            server=substitute(server,elenco_contenuti,false);

                            QRegExp r;
                            r.setPattern("^socket\\:\\/\\/(.{1,})\\:([0-9]*)$");
                            if (!r.exactMatch(server)){
                                emit error("Server address malformed.");
                                resultError=true;
                                break;
                            }
                            QString host=r.cap(1);
                            QString port=r.cap(2);
                            if (port.isEmpty()){
                                emit error("Indirizzo del server non corretto. Porta mancante.");
                                resultError=true;
                                break;
                            }


                            rawClient->setHost(host,port);
                            if (ps.useProxy()){
                                rawClient->setProxyServer(ps.getServer(),ps.getPort(),ps.getType());
                                if (ps.useAuth())
                                    rawClient->setProxyAuthentication(ps.getUsername(),ps.getPassword());
                            }
                            rawClient->connectToServer();
                            int commandCount=0;
                            while(paginaCorrente.nextCommand()){
                                QString comando=paginaCorrente.currentCommand().getCommand();
                                comando=substitute(comando,elenco_credenziali);
                                comando=substitute(comando,elenco_dati_vari);
                                comando=substitute(comando,elenco_contenuti,false);
                                rawClient->writeLine(comando);
                                log.addNotice("Inviato Comando: "+comando);
                                commandCount++;
                            }
                            for(int c=0; c<commandCount;c++){
                                QString receivedData=rawClient->readLine();
                                log.addNotice("Ricevuto: "+receivedData);
                                paginaCorrente.setRetreivedHtml(receivedData);
                                if(paginaCorrente.checkErrors()){
                                    dataTypes::pageError errore=paginaCorrente.getError();
                                    log.addNotice("Ho trovato una stringa di errore: "+errore.getErrorString());
                                    log.addNotice("Errore rilevato: "+substitute(errore.getErrorMessage(),elenco_contenuti,false));
                                    log.addNotice("Codice errore: "+errore.getErrorCode());

                                    emit error(substitute(errore.getErrorMessage(),elenco_contenuti,false));
                                    resultError=true;
                                    break;

                                }
                                if(paginaCorrente.checkAccepts()){
                                    dataTypes::pageAccept accettante=paginaCorrente.getAccept();
                                    log.addNotice("Ho trovato una stringa accettante: "+accettante.getAcceptString());
                                    log.addNotice("Messaggio di conferma: "+substitute(accettante.getConfirmMessage(),elenco_contenuti,false));

                                    emit success(substitute(accettante.getConfirmMessage(),elenco_contenuti,false));
                                    resultSend=true;
                                    break;
                                }
                            }


                        }else{
                            while(paginaCorrente.nextHeader()){
                                QString headerVal=paginaCorrente.currentHeader().getValue();

                                headerVal=substitute(headerVal,elenco_credenziali);
                                headerVal=substitute(headerVal,elenco_dati_vari);
                                headerVal=substitute(headerVal,elenco_contenuti,false);

                                webClient->addHeader(paginaCorrente.currentHeader().getName(),headerVal);

                                //webClient->AddHeader(paginaCorrente.currentHeader().getName(),paginaCorrente.currentHeader().getValue());
                                log.addNotice("Aggiunto header: "+paginaCorrente.currentHeader().getName()+" - "+headerVal);
                            }
                            while(paginaCorrente.nextVariable()){
                                dataTypes::pageVariable var=paginaCorrente.currentVariable();
                                QString var_value=var.getValue();
                                QString var_name=var.getName();

                                var_value=substitute(var_value,elenco_credenziali);
                                var_value=substitute(var_value,elenco_dati_vari);
                                var_value=substitute(var_value,elenco_contenuti,false);

                                var_name=substitute(var_name,elenco_credenziali);
                                var_name=substitute(var_name,elenco_dati_vari);
                                var_name=substitute(var_name,elenco_contenuti,false);
                                webClient->insertFormData(var_name,var_value);

                                QByteArray v=QByteArray::fromPercentEncoding(var_value.toAscii());
                                v=v.toPercentEncoding();

                                log.addNotice("Aggiunta variabile: "+var_name+" - "+QString(v));

                            }

                            {

                                QString html;
                                if (paginaCorrente.getMethod()=="POST"){
                                    log.addNotice("Inviata richiesta post...");
                                    html=webClient->submitPost(parsedCurrentUrlPage,true);
                                    paginaCorrente.setRetreivedHtml(html+webClient->currentUrl());
                                }else  if (paginaCorrente.getMethod()=="GET"){
                                    log.addNotice("Inviata richiesta get...");
                                    html=webClient->submitGet(parsedCurrentUrlPage,true);
                                    paginaCorrente.setRetreivedHtml(html+webClient->currentUrl());
                                }else{
                                    log.addNotice("Lettura pagina..");
                                    webClient->setUrl(parsedCurrentUrlPage);
                                    html=webClient->readPage(true);
                                    paginaCorrente.setRetreivedHtml(html+webClient->currentUrl());

                                }
                                logger l;
                                l.addNotice(html);
                                l.save(libJackSMS::directories::DumpDirectory()+"pagina_"+QString::number(pageNumber)+".html");
                                pageNumber++;
                            }

                            while(paginaCorrente.nextContent()){
                                dataTypes::content contenuto;
                                contenuto.setName(paginaCorrente.currentContentName());
                                contenuto.setFound(paginaCorrente.currentContentFound());
                                if (contenuto.getFound()){
                                    contenuto.setValue(servizioDaUsare.getEncodedTextUrlEncoded(paginaCorrente.currentContentValue()));
                                    contenuto.setUnEncodedValue(paginaCorrente.currentContentValue());
                                    log.addNotice("Ho trovato un content: "+contenuto.getName()+" -> "+contenuto.getUnEncodedValue());

                                }else{
                                    log.addNotice("Non ho trovato il content: "+contenuto.getName());
                                }
                                elenco_contenuti.push_back(contenuto);
                            }

                            if(paginaCorrente.checkErrors()){
                                dataTypes::pageError errore=paginaCorrente.getError();
                                log.addNotice("Ho trovato una stringa di errore: "+errore.getErrorString());
                                log.addNotice("Errore rilevato: "+substitute(errore.getErrorMessage(),elenco_contenuti,false));
                                log.addNotice("Codice errore: "+errore.getErrorCode());

                                emit error(substitute(errore.getErrorMessage(),elenco_contenuti,false));
                                resultError=true;
                                break;

                            }
                            if(paginaCorrente.checkAccepts()){
                                dataTypes::pageAccept accettante=paginaCorrente.getAccept();
                                log.addNotice("Ho trovato una stringa accettante: "+accettante.getAcceptString());
                                log.addNotice("Messaggio di conferma: "+substitute(accettante.getConfirmMessage(),elenco_contenuti,false));

                                emit success(substitute(accettante.getConfirmMessage(),elenco_contenuti,false));
                                resultSend=true;
                                break;
                            }
                        }//end else of if has raw commands
                    }//end if is not a capcha page
                }//end if dopage

            }
            if ((!resultSend) && (!resultError)){
                log.addNotice("Ho raggiunto la fine delle pagine da elaborare per il servizio ma non ho ricevuto alcun errore o alcuna stringa accettante: marco come fallito l'invio.");
                emit error("Errore durante l'invio.");
            }

        }
        log.save();

    }



}


