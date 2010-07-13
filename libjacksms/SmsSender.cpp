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
#include "ReportOperationSendSmsNothing.h"
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

namespace libJackSMS{


    smsSender::smsSender(const dataTypes::servicesType & _services, const dataTypes::proxySettings &_ps):servizi(_services),ps(_ps){
        operazione=new libJackSMS::reportOperationSendSmsNothing();

    }
    void smsSender::setRecipient(const dataTypes::phoneNumber & _dest){
        destinatario=_dest;
    }
    void smsSender::setMessage(const dataTypes::shortMessage & _message){
        messaggio=_message;
    }
    void smsSender::setAccount(dataTypes::configuredAccount _account){
        account=_account;
    }
    void smsSender::setReportOperation(libJackSMS::reportOperationSendSms * _op){
        operazione->~reportOperationSendSms();
        operazione=_op;
    }
    QString smsSender::substitute(QString _input,const dataTypes::contentType &_cont,bool replace_encoded){
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

    QString smsSender::substitute(QString _input,const dataTypes::variousType &_cont){
        dataTypes::variousType::const_iterator i=_cont.begin();
        dataTypes::variousType::const_iterator i_end=_cont.end();
        for(;i!=i_end;++i){
            QString code="%%"+i.key()+"%%";
            _input=_input.replace(code,i.value());
        }
        return _input;
    }
    void smsSender::abort(){
        webClient->interrupt();
    }
    void smsSender::send(){
        libJackSMS::logger log;
        libJackSMS::dataTypes::servicesType::const_iterator service_iter=servizi.find(account.getService());
        if (service_iter==servizi.end()){
            log.addNotice("non esiste alcun servizio con questo id.");
            operazione->doOperationError("Servizio inesistente.");
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
            //webClient->IncludeHeaders();
            dataTypes::contentType elenco_contenuti;
            dataTypes::variousType elenco_dati_vari;
            dataTypes::creditsType elenco_credenziali;


            elenco_dati_vari.insert(QString("message"),servizioDaUsare.getEncodedText(messaggio.getText()));
            elenco_dati_vari.insert(QString("intcode"),destinatario.getIntCode());
            elenco_dati_vari.insert(QString("intpref"),destinatario.getIntPref());
            elenco_dati_vari.insert(QString("pref"),destinatario.getPref());
            elenco_dati_vari.insert(QString("num"),destinatario.getNum());
            elenco_dati_vari.insert(QString("intnum"),destinatario.getIntNum());

            while(servizioDaUsare.nextVar()){
                //std::cout<<servizioDaUsare.currentVar().getName()<<"-"<<account.getData(servizioDaUsare.currentVar().getName());
                elenco_credenziali.insert(servizioDaUsare.currentVar().getName(),account.getData(servizioDaUsare.currentVar().getName()));
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
                    operazione->doOperation();
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
                        QString captcha_result=operazione->doCaptchaOperation(captchaBytes);
                        if (captcha_result=="captcha_non_decodificato"){
                            operazione->doOperationError("Captcha failed.");
                            resultError=true;
                            break;
                        }
                        elenco_dati_vari.insert("captcha_value",captcha_result);

                    }else{
                        if (paginaCorrente.hasRawCommands()){

                            elenco_dati_vari["message"]=messaggio.getText();

                            netClient::SocketClient *rawClient=new netClient::SocketClient(false);
                            QString server=parsedCurrentUrlPage;
                            server=substitute(server,elenco_credenziali);
                            server=substitute(server,elenco_dati_vari);
                            server=substitute(server,elenco_contenuti,false);

                            QRegExp r;
                            r.setPattern("^socket\\:\\/\\/(.{1,})\\:([0-9]*)$");
                            if (!r.exactMatch(server)){
                                operazione->doOperationError("Server address malformed.");
                                resultError=true;
                                break;
                            }
                            QString host=r.cap(1);
                            QString port=r.cap(2);
                            if (port.isEmpty()){
                                operazione->doOperationError("Indirizzo del server non corretto. Porta mancante.");
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

                                    operazione->doOperationError(substitute(errore.getErrorMessage(),elenco_contenuti,false));
                                    resultError=true;
                                    break;

                                }
                                if(paginaCorrente.checkAccepts()){
                                    dataTypes::pageAccept accettante=paginaCorrente.getAccept();
                                    log.addNotice("Ho trovato una stringa accettante: "+accettante.getAcceptString());
                                    log.addNotice("Messaggio di conferma: "+substitute(accettante.getConfirmMessage(),elenco_contenuti,false));

                                    operazione->doOperationSuccess(substitute(accettante.getConfirmMessage(),elenco_contenuti,false));
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
                                log.addNotice("Aggiunta variabile: "+var_name+" - "+var_value);

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

                                operazione->doOperationError(substitute(errore.getErrorMessage(),elenco_contenuti,false));
                                resultError=true;
                                break;

                            }
                            if(paginaCorrente.checkAccepts()){
                                dataTypes::pageAccept accettante=paginaCorrente.getAccept();
                                log.addNotice("Ho trovato una stringa accettante: "+accettante.getAcceptString());
                                log.addNotice("Messaggio di conferma: "+substitute(accettante.getConfirmMessage(),elenco_contenuti,false));

                                operazione->doOperationSuccess(substitute(accettante.getConfirmMessage(),elenco_contenuti,false));
                                resultSend=true;
                                break;
                            }
                        }//end else of if has raw commands
                    }//end if is not a capcha page
                }//end if dopage

            }
            if ((!resultSend) && (!resultError)){
                log.addNotice("Ho raggiunto la fine delle pagine da elaborare per il servizio ma non ho ricevuto alcun errore o alcuna stringa accettante: marco come fallito l'invio.");
                operazione->doOperationError("Conferma di invio non trovata.");
            }

        }
        log.save();

    }



}


