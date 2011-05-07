/*
*/

#include "SmsSender.h"
#include <QString>
#include "DataTypes.h"
#include <QSharedMemory>
#include <QSemaphore>
#include "ShortMessage.h"
#include "NetClient.h"
#include "netclientqhttp.h"
#include "ProxyConfig.h"
#include "EncodingUrl.h"
#include "SocketClient.h"
#include "FilesDirectory.h"
#include "Utilities.h"
#include "Exceptions.h"
#include <QWaitCondition>
#include "libjacksms/Utilities.h"
#include "libjacksms/libJackSMS.h"

namespace libJackSMS
{
    QWaitCondition waitOnSleepBefore;

    /********************smsSender****************/

    smsSender::smsSender(const QString &_loginId, const dataTypes::servicesType &_services, const dataTypes::proxySettings &_ps) :
            loginId(_loginId),
            servizi(_services),
            ps(_ps),
            continueSendFlag(false),
            SalvaCookies(true),
            getAdv(false)
    {
    }

    void smsSender::setRecipient(const dataTypes::phoneNumber &_dest)
    {
        destinatario = _dest;
    }

    void smsSender::setMessage(const dataTypes::shortMessage &_message)
    {
        messaggio = _message;
    }

    void smsSender::setAccount(const dataTypes::configuredAccount &_account)
    {
        account = _account;
    }

    void smsSender::send()
    {
        run();
    }

    void smsSender::continueSend(QString captcha_value)
    {
        captchaValue = captcha_value;
        run();
    }

    void smsSender::run()
    {
        try
        {
            smsSenderBase sndr(loginId, servizi, ps);
            connect(&sleepClockTimer, SIGNAL(timeout()), this, SLOT(slotTimerClocked()));
            connect(this, SIGNAL(abortSignal()), &sndr, SLOT(abort()));

            sndr.setSalvaCookies(SalvaCookies);
            sndr.setRecipient(destinatario);
            sndr.setMessage(messaggio);
            sndr.setAccount(account);
            sndr.setGetAdv(getAdv);

            connect(&sndr, SIGNAL(operation()), this, SLOT(slotOperation()));
            connect(&sndr, SIGNAL(operation(QString)), this, SLOT(slotError(QString)));
            connect(&sndr, SIGNAL(error(QString)), this, SLOT(slotError(QString)));
            connect(&sndr, SIGNAL(success(QString)), this, SLOT(slotSuccess(QString)));
            connect(&sndr, SIGNAL(captcha(QByteArray)), this, SLOT(slotCaptcha(QByteArray)));
            connect(&sndr, SIGNAL(sleepBeforeFound(int)), this, SLOT(slotSleepBeforeFound(int)));
            connect(&sndr, SIGNAL(adv(QString)), this, SIGNAL(adv(QString)));

            if (!continueSendFlag)
            {
                sndr.send();
                if (sndr.isInterruptedByCaptcha()) {
                    continueSendFlag = true;
                    pageIndex = sndr.getCaptchaPageIndex();
                    contenuti = sndr.getContents();
                }

            } else {

                sndr.setContents(contenuti);
                sndr.setNumberOfFirstPage(pageIndex);
                sndr.send(captchaValue);
            }

            sndr.disconnect(this);

        } catch(libJackSMS::netClient::abortedException e) {
        } catch(libJackSMS::exceptionSharedMemory e) {
            emit error("Cannot create shared memory");
        }
    }

    void smsSender::abort()
    {
        sleepClockTimer.stop();
        waitOnSleepBefore.wakeAll();
        emit abortSignal();
    }

    void smsSender::slotOperation()
    {
        emit operation();
    }

    void smsSender::slotOperation(QString s)
    {
        emit operation(s);
    }

    void smsSender::slotError(QString s)
    {
        emit error(s);
        connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
    }

    void smsSender::slotSuccess(QString s)
    {
        emit success(s);
        connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
    }

    void smsSender::slotCaptcha(QByteArray a)
    {
        emit captcha(a);
    }

    void smsSender::slotSleepBeforeFound(int i)
    {
        if (i >= 0) {
            secondsToGo = i;
            emit sleepBeforeFound(i);
            sleepClockTimer.start(1000);
        }
    }

    void smsSender::slotTimerClocked()
    {
        --secondsToGo;
        emit sleepBeforeFound(secondsToGo);

        // condizione di uscita
        if (secondsToGo <= 0)
            sleepClockTimer.stop();
    }

    void smsSender::setSalvaCookies(bool value)
    {
        SalvaCookies = value;
    }

    void smsSender::setGetAdv(bool value)
    {
        getAdv = value;
    }

    /********************smsSenderBase****************/

    smsSenderBase::smsSenderBase(const QString &_loginId, const dataTypes::servicesType &_services, const dataTypes::proxySettings &_ps) :
            loginId(_loginId),
            servizi(_services),
            ps(_ps),
            pageIndex(-1),
            captchaInterrupt(false),
            hasAborted(false),
            getAdv(false)
    {
    }

    bool smsSenderBase::isInterruptedByCaptcha() const
    {
        return captchaInterrupt;
    }

    int smsSenderBase::getCaptchaPageIndex() const
    {
        return pageIndex;
    }

    void smsSenderBase::setSalvaCookies(bool value)
    {
        SalvaCookies = value;
    }

    void smsSenderBase::optionalDeleteCookies()
    {
        if (!SalvaCookies)
            webClient->clearCookies();
    }

    void smsSenderBase::setNumberOfFirstPage(int _pn)
    {
        pageIndex = _pn;
    }

    void smsSenderBase::setRecipient(const dataTypes::phoneNumber &_dest)
    {
        destinatario = _dest;
    }

    void smsSenderBase::setMessage(const dataTypes::shortMessage &_message)
    {
        messaggio = _message;
    }

    void smsSenderBase::setAccount(dataTypes::configuredAccount _account)
    {
        account = _account;
    }

    QString smsSenderBase::substitute(QString _input, const dataTypes::contentType &_cont)
    {
        for (dataTypes::contentType::const_iterator i = _cont.begin(); i != _cont.end(); ++i)
        {
            QString code = "%%" + i->getName() + "%%";
            /*if (i->getToEncode())
                _input=_input.replace(code,QString(i->getValue().toAscii().toPercentEncoding()));
            else*/
            _input = _input.replace(code,i->getValue());
        }
        return _input;
    }

    QString smsSenderBase::substitute(QString _input, const dataTypes::variousType &_cont)
    {
        for (dataTypes::variousType::const_iterator i = _cont.begin(); i != _cont.end(); ++i) {
            QString code = "%%" + i.key() + "%%";
            _input = _input.replace(code, i.value());
        }
        return _input;
    }

    void smsSenderBase::abort()
    {
        hasAborted = true;
        webClient->interrupt();
        webClient->clearCookies();
    }

    dataTypes::contentType smsSenderBase::getContents() const
    {
        return elenco_contenuti;
    }

    void smsSenderBase::setContents(dataTypes::contentType contents)
    {
        elenco_contenuti = contents;
    }

    void smsSenderBase::send(QString captcha_value)
    {
        try
        {
            if (account.getId() == "1")
            {
                // JMS

                if (getAdv) {
                    libJackSMS::serverApi::advChecker *advcheck = new libJackSMS::serverApi::advChecker(loginId, messaggio.getText(), ps);
                    connect(advcheck, SIGNAL(adv(QString)), this, SIGNAL(adv(QString)));
                    advcheck->start();
                }

                webClient = new netClient::netClientQHttp();
                webClient->setUseCookie(true);

                if (ps.useProxy()) {
                    webClient->setProxyServer(ps.getServer(), ps.getPort(), ps.getType());
                    if (ps.useAuth())
                        webClient->setProxyAuthentication(ps.getUsername(), ps.getPassword());
                }

                webClient->insertFormData("account_id", "1");
                webClient->insertFormData("recipient", destinatario.toString().toAscii().toPercentEncoding());
                webClient->insertFormData("message", messaggio.getText().toAscii().toPercentEncoding());

                QString resultOfApi = webClient->submitPost("http://stream.freesmee.com/send?desktop=" + QString(FREESMEE_VERSION) + "&token=" + loginId, true);

                if (webClient->getAborted())
                    throw netClient::abortedException();

                if (resultOfApi.isEmpty())
                {
                    emit error("Empty Response");

                } else {
                    QList<QString> lines = resultOfApi.split('\n');

                    while (!lines.isEmpty())
                    {
                        QString l(lines.first().trimmed());
                        if (l.startsWith("HTTP/1.1"))
                        {
                            while (true)
                            {
                                lines.removeFirst();
                                l = QString(lines.first().trimmed());

                                if (l.isEmpty())
                                {
                                    //remove empty line
                                    lines.removeFirst();

                                    //skip length
                                    l = QString(lines.first().trimmed());
                                    lines.removeFirst();

                                    //get interesting line :D
                                    if (lines.count() > 0)
                                        l = QString(lines.first().trimmed());
                                    else
                                        l = "";

                                    break;
                                }
                            }
                        }

                        lines.removeFirst();

                        bool ok;
                        QVariantMap result = Json::parse(l, ok).toMap();

                        if (ok)
                        {
                            if (result["status"].toInt() == 1)
                            {
                                int res = result["result"].toInt();

                                if (res == 1)
                                    emit success(result["message"].toString().isEmpty() ? "" : result["message"].toString());
                                else if (res == 0)
                                    emit error(result["message"].toString().isEmpty() ? "Errore durante l'invio" : result["message"].toString());

                            } else if (result["status"].toString() == "0") {
                                emit error(result["message"].toString().isEmpty() ? "Errore durante l'invio" : result["message"].toString());
                            }
                        }
                    }
                }

                webClient->~netClientGeneric();

            } else {
                // SMS

                libJackSMS::logger log(directories::DataDirectory() + QString("debug_log.txt"));

                if (captcha_value.isEmpty())
                    log.cancelLog();

                libJackSMS::dataTypes::servicesType::const_iterator service_iter = servizi.find(account.getService());

                if (service_iter == servizi.end())
                {
                    log.addNotice("non esiste alcun servizio con questo id.");
                    emit error("Servizio inesistente.");

                } else {

                    if (getAdv) {
                        libJackSMS::serverApi::advChecker *advcheck = new libJackSMS::serverApi::advChecker(loginId, messaggio.getText(), ps);
                        connect(advcheck, SIGNAL(adv(QString)), this, SIGNAL(adv(QString)));
                        advcheck->start();
                    }

                    bool resultSend = false;
                    bool resultError = false;
                    dataTypes::service servizioDaUsare = service_iter.value();
                    webClient = new netClient::netClientQHttp();

                    if (ps.useProxy()) {
                        webClient->setProxyServer(ps.getServer(), ps.getPort(), ps.getType());
                        if (ps.useAuth())
                            webClient->setProxyAuthentication(ps.getUsername(), ps.getPassword());
                    }

                    webClient->setUserAgent("Mozilla/5.0 (Windows; U; Windows NT 5.1; it; rv:1.8.1.3) Gecko/20070309 Firefox/2.0.0.3");
                    //webClient->SetCookie(directories::CookiesDirectory() + "cookie");
                    webClient->setUseCookie(true);
                    webClient->setCookieFile(libJackSMS::directories::concatDirectoryAndFile(libJackSMS::directories::CookiesDirectory(), account.getId() + ".cookie"));
                    //webClient->IncludeHeaders();

                    dataTypes::variousType elenco_dati_vari;
                    dataTypes::creditsType elenco_credenziali;

                    elenco_dati_vari.insert(QString("message"), servizioDaUsare.getEncodedTextUrlEncoded(messaggio.getText()));
                    elenco_dati_vari.insert(QString("intcode"), servizioDaUsare.getEncodedTextUrlEncoded(destinatario.getIntCode()));
                    elenco_dati_vari.insert(QString("intpref"), servizioDaUsare.getEncodedTextUrlEncoded(destinatario.getIntPref()));
                    elenco_dati_vari.insert(QString("pref"), servizioDaUsare.getEncodedTextUrlEncoded(destinatario.getPref()));
                    elenco_dati_vari.insert(QString("num"), servizioDaUsare.getEncodedTextUrlEncoded(destinatario.getNum()));
                    elenco_dati_vari.insert(QString("intnum"), servizioDaUsare.getEncodedTextUrlEncoded(destinatario.getIntNum()));

                    if (!captcha_value.isEmpty())
                        elenco_dati_vari.insert("captcha_value", servizioDaUsare.getEncodedTextUrlEncoded(captcha_value));

                    while (servizioDaUsare.nextVar()) {
                        QString n = servizioDaUsare.currentVar().getName();
                        QString aa = account.getData(n);
                        elenco_credenziali.insert(n,servizioDaUsare.getEncodedText(aa));
                    }

                    //svuoto la cartella Dump
                    utilities::deleteDirectory(libJackSMS::directories::DumpDirectory(), false);

                    //inizializzo le variabili
                    int pageNumber = 1;
                    int pageCounter = -1;
                    bool hasPostprocedurePage = servizioDaUsare.getHasPostprocedurePage();
                    bool isPostprocedurePage = false;
                    QString resultString = "";
                    bool forceDeleteCookies = false;

                    while (servizioDaUsare.nextPage() || isPostprocedurePage) {

                        if (webClient->getAborted())
                            throw netClient::abortedException();

                        dataTypes::paginaServizio paginaCorrente;

                        if (isPostprocedurePage) {
                            log.addNotice("Fine Procedure - Inizio Postprocedure");
                            paginaCorrente = servizioDaUsare.getPostprocedurePage();
                        } else {
                            pageCounter++;
                            paginaCorrente = servizioDaUsare.currentPage();
                        }

                        // se la pagina dice di aspettare si aspetta.
                        if (paginaCorrente.getSleepbefore() > 0) {
                            mutex.lock();

                            // ricordando che lo sleepbefore è espresso in secondi si moltiplica per 1000 per ottenere i millisecondi
                            int secs = paginaCorrente.getSleepbefore();
                            emit sleepBeforeFound(secs);
                            waitOnSleepBefore.wait(&mutex, secs*1000);
                            mutex.unlock();
                        }

                        bool doPage = false;

                        if (pageCounter >= pageIndex)
                            doPage = true;
                        if (doPage && paginaCorrente.hasCondition()) {
                            doPage = false;
                            QString condizione=paginaCorrente.getCondition();

                            if (!doPage) {
                                for (dataTypes::contentType::const_iterator i = elenco_contenuti.begin();i != elenco_contenuti.end(); ++i) {
                                    QString code = "%%" + i->getName() + "%%";
                                    if (condizione == code && i->getFound()) {
                                        doPage = true;
                                        break;
                                    }
                                }
                            }
                            if (!doPage) {
                                for (dataTypes::variousType::const_iterator i = elenco_dati_vari.begin(); i != elenco_dati_vari.end(); ++i) {
                                    QString code = "%%" + i.key() + "%%";
                                    if (condizione == code && (!i.value().isEmpty())) {
                                        doPage = true;
                                        break;
                                    }
                                }
                            }
                            if (!doPage) {
                                for (dataTypes::creditsType::const_iterator i = elenco_credenziali.begin(); i != elenco_credenziali.end(); ++i) {
                                    QString code = "%%" + i.key() + "%%";
                                    if (condizione == code && (!i.value().isEmpty())) {
                                        doPage = true;
                                        break;
                                    }
                                }
                            }
                        }

                        if (doPage){
                            QString parsedCurrentUrlPage = paginaCorrente.getUrl();

                            parsedCurrentUrlPage = substitute(parsedCurrentUrlPage, elenco_credenziali);
                            parsedCurrentUrlPage = substitute(parsedCurrentUrlPage, elenco_dati_vari);
                            parsedCurrentUrlPage = substitute(parsedCurrentUrlPage, elenco_contenuti);

                            log.addNotice("Pagina: " + parsedCurrentUrlPage);
                            emit operation();
                            if (paginaCorrente.getIsCaptcha()) {
                                log.addNotice("Eseguita come pagina captcha");

                                while (paginaCorrente.nextHeader()) {
                                    QString headerVal = paginaCorrente.currentHeader().getValue();

                                    headerVal = substitute(headerVal, elenco_credenziali);
                                    headerVal = substitute(headerVal, elenco_dati_vari);
                                    headerVal = substitute(headerVal, elenco_contenuti);

                                    webClient->addHeader(paginaCorrente.currentHeader().getName(), headerVal);
                                    log.addNotice("Aggiunto header: " + paginaCorrente.currentHeader().getName() + " - " + headerVal);
                                }

                                QByteArray captchaBytes;
                                webClient->getFile(parsedCurrentUrlPage, captchaBytes);
                                log.addNotice("Captcha ricevuto: bytes=" + QString::number(captchaBytes.size()));
                                pageIndex = pageCounter + 1;
                                captchaInterrupt = true;
                                emit captcha(captchaBytes);
                                return;

                            } else {
                                if (paginaCorrente.hasRawCommands()) {
                                    elenco_dati_vari.insert(QString("message"), messaggio.getText());
                                    elenco_dati_vari.insert(QString("intcode"), destinatario.getIntCode());
                                    elenco_dati_vari.insert(QString("intpref"), destinatario.getIntPref());
                                    elenco_dati_vari.insert(QString("pref"), destinatario.getPref());
                                    elenco_dati_vari.insert(QString("num"), destinatario.getNum());
                                    elenco_dati_vari.insert(QString("intnum"), destinatario.getIntNum());
                                    elenco_dati_vari["message"] = messaggio.getText();
                                    while(servizioDaUsare.nextVar()) {
                                        QString n = servizioDaUsare.currentVar().getName();
                                        QString aa = account.getData(n);
                                        elenco_credenziali.insert(n, aa);
                                    }

                                    netClient::SocketClient *rawClient = new netClient::SocketClient(false);
                                    QString server = parsedCurrentUrlPage;
                                    server = substitute(server, elenco_credenziali);
                                    server = substitute(server, elenco_dati_vari);
                                    server = substitute(server, elenco_contenuti);

                                    QRegExp r;
                                    r.setPattern("^socket\\:\\/\\/(.{1,})\\:([0-9]*)$");
                                    if (!r.exactMatch(server)) {

                                        resultString = "Server address malformed.";
                                        resultError = true;
                                        if (manageErrorBreak(resultString, resultError, resultSend, forceDeleteCookies, isPostprocedurePage, hasPostprocedurePage, log))
                                            break;
                                        else
                                            continue;

                                    }

                                    QString host = r.cap(1);
                                    QString port = r.cap(2);
                                    if (port.isEmpty()) {
                                        resultString = "Indirizzo del server non corretto. Porta mancante.";
                                        resultError = true;
                                        if (manageErrorBreak(resultString, resultError, resultSend, forceDeleteCookies, isPostprocedurePage, hasPostprocedurePage, log))
                                            break;
                                        else
                                            continue;
                                    }

                                    rawClient->setHost(host, port);
                                    if (ps.useProxy()) {
                                        rawClient->setProxyServer(ps.getServer(), ps.getPort(), ps.getType());
                                        if (ps.useAuth())
                                            rawClient->setProxyAuthentication(ps.getUsername(), ps.getPassword());
                                    }

                                    rawClient->connectToServer();

                                    int commandCount = 0;
                                    while (paginaCorrente.nextCommand()) {
                                        QString comando = paginaCorrente.currentCommand().getCommand();
                                        comando = substitute(comando, elenco_credenziali);
                                        comando = substitute(comando, elenco_dati_vari);
                                        comando = substitute(comando, elenco_contenuti);
                                        rawClient->writeLine(comando);
                                        log.addNotice("Inviato Comando: " + comando);
                                        commandCount++;
                                    }

                                    for (int c = 0; c < commandCount; c++) {
                                        QString receivedData = rawClient->readLine();
                                        log.addNotice("Ricevuto: " + receivedData);
                                        paginaCorrente.setRetreivedHtml(receivedData);

                                        if (!isPostprocedurePage) {
                                            if (paginaCorrente.checkErrors()) {
                                                dataTypes::pageError errore = paginaCorrente.getError();
                                                log.addNotice("Ho trovato una stringa di errore: " + errore.getErrorString());
                                                log.addNotice("Errore rilevato: " + substitute(errore.getErrorMessage(), elenco_contenuti));
                                                log.addNotice("Codice errore: " + errore.getErrorCode());

                                                resultString = substitute(errore.getErrorMessage(), elenco_contenuti);
                                                resultError = true;
                                                if (hasPostprocedurePage) {
                                                    isPostprocedurePage = true;
                                                    continue;
                                                } else {
                                                    emit error(resultString);
                                                    optionalDeleteCookies();
                                                    break;
                                                }
                                            }

                                            if (paginaCorrente.checkAccepts()) {
                                                dataTypes::pageAccept accettante = paginaCorrente.getAccept();
                                                log.addNotice("Ho trovato una stringa accettante: " + accettante.getAcceptString());
                                                log.addNotice("Messaggio di conferma: " + substitute(accettante.getConfirmMessage(), elenco_contenuti));

                                                resultString = substitute(accettante.getConfirmMessage(),elenco_contenuti);
                                                resultSend = true;
                                                if (hasPostprocedurePage) {
                                                    isPostprocedurePage = true;
                                                    continue;
                                                } else {
                                                    emit success(resultString);
                                                    optionalDeleteCookies();
                                                    break;
                                                }
                                            }
                                        }
                                    }
                                } else {
                                    while (paginaCorrente.nextHeader()) {
                                        QString headerVal = paginaCorrente.currentHeader().getValue();

                                        headerVal = substitute(headerVal, elenco_credenziali);
                                        headerVal = substitute(headerVal, elenco_dati_vari);
                                        headerVal = substitute(headerVal, elenco_contenuti);

                                        webClient->addHeader(paginaCorrente.currentHeader().getName(), headerVal);
                                        log.addNotice("Aggiunto header: " + paginaCorrente.currentHeader().getName() + " - " + headerVal);
                                    }

                                    while (paginaCorrente.nextVariable()) {
                                        dataTypes::pageVariable var = paginaCorrente.currentVariable();
                                        bool insert = !var.hasCondition();
                                        if (!insert) {
                                            QString condition = var.getCondition();
                                            for (dataTypes::contentType::const_iterator i = elenco_contenuti.begin(); i != elenco_contenuti.end(); ++i) {
                                                QString code = "%%" + i->getName() + "%%";
                                                if (condition == code && i->getFound()) {
                                                    insert = true;
                                                    break;
                                                }
                                            }
                                        }

                                        if (insert) {
                                            QString var_value = var.getValue();
                                            QString var_name = var.getName();
                                            var_value = substitute(var_value, elenco_credenziali);
                                            var_value = substitute(var_value, elenco_dati_vari);
                                            var_value = substitute(var_value, elenco_contenuti);

                                            var_name = substitute(var_name, elenco_credenziali);
                                            var_name = substitute(var_name, elenco_dati_vari);
                                            var_name = substitute(var_name, elenco_contenuti);

                                            if (var.getToEncode())
                                                var_value = QString(var_value.toAscii().toPercentEncoding());

                                            webClient->insertFormData(var_name, var_value);
                                            log.addNotice("Aggiunta variabile: " + var_name + " - " + var_value);
                                        }
                                    }

                                    {
                                        QString html;
                                        if (paginaCorrente.getMethod() == "POST") {
                                            log.addNotice("Inviata richiesta post...");
                                            html = webClient->submitPost(parsedCurrentUrlPage, true);
                                            paginaCorrente.setRetreivedHtml(html + webClient->currentUrl());
                                        }else if (paginaCorrente.getMethod() == "GET") {
                                            log.addNotice("Inviata richiesta get...");
                                            html = webClient->submitGet(parsedCurrentUrlPage, true);
                                            paginaCorrente.setRetreivedHtml(html + webClient->currentUrl());
                                        } else {
                                            log.addNotice("Lettura pagina..");
                                            webClient->setUrl(parsedCurrentUrlPage);
                                            html=webClient->readPage(true);
                                            paginaCorrente.setRetreivedHtml(html + webClient->currentUrl());
                                        }

                                        logger l(libJackSMS::directories::DumpDirectory() + "pagina_" + QString::number(pageCounter) + ".html");
                                        l.addNotice(html);
                                        pageNumber++;
                                    }

                                    while (paginaCorrente.nextContent()) {
                                        dataTypes::content contenuto;
                                        contenuto.setName(paginaCorrente.currentContentName());
                                        contenuto.setFound(paginaCorrente.currentContentFound());
                                        if (contenuto.getFound()){

                                            contenuto.setValue(paginaCorrente.currentContentValue());
                                            //contenuto.setToEncode(paginaCorrente.currentContentToEncode());
                                            //contenuto.setUnEncodedValue(paginaCorrente.currentContentValue());
                                            log.addNotice("Ho trovato un content: "+contenuto.getName()+" -> "+contenuto.getValue());

                                        }else{
                                            log.addNotice("Non ho trovato il content: "+contenuto.getName());
                                        }
                                        elenco_contenuti.push_back(contenuto);
                                    }

                                    if (isPostprocedurePage) {
                                        managePostProcedureSignals(resultString, resultError, resultSend, forceDeleteCookies, isPostprocedurePage);
                                        log.addNotice("Fine Postprocedure");
                                        break;
                                    } else {

                                        if (paginaCorrente.checkErrors()) {
                                            dataTypes::pageError errore = paginaCorrente.getError();
                                            log.addNotice("Ho trovato una stringa di errore: " + errore.getErrorString());
                                            log.addNotice("Errore rilevato: " + substitute(errore.getErrorMessage(), elenco_contenuti));
                                            log.addNotice("Codice errore: " + errore.getErrorCode());

                                            resultString = substitute(errore.getErrorMessage(), elenco_contenuti);
                                            resultError = true;

                                            if (hasPostprocedurePage) {
                                                isPostprocedurePage = true;
                                                forceDeleteCookies = true;
                                                continue;
                                            } else {
                                                emit error(resultString);
                                                webClient->clearCookies();
                                                break;
                                            }
                                        }

                                        if (paginaCorrente.checkAccepts()) {
                                            dataTypes::pageAccept accettante = paginaCorrente.getAccept();
                                            log.addNotice("Ho trovato una stringa accettante: " + accettante.getAcceptString());
                                            log.addNotice("Messaggio di conferma: " + substitute(accettante.getConfirmMessage(), elenco_contenuti));

                                            resultString = substitute(accettante.getConfirmMessage(), elenco_contenuti);
                                            resultSend = true;
                                            if (hasPostprocedurePage) {
                                                isPostprocedurePage = true;
                                                continue;
                                            } else {
                                                emit success(resultString);
                                                optionalDeleteCookies();
                                                break;
                                            }
                                        }
                                    }
                                } //end else of if has raw commands
                            } //end if is not a capcha page
                        } //end if dopage

                    }
                    if ((!resultSend) && (!resultError)) {
                        log.addNotice("Ho raggiunto la fine delle pagine da elaborare per il servizio ma non ho ricevuto alcun errore o alcuna stringa accettante: marco come fallito l'invio.");
                        webClient->clearCookies();
                        if (!hasAborted)
                            emit error("Errore durante l'invio.");
                    }
                    webClient->~netClientGeneric();
                }
            }

        } catch(...) {
            if (!hasAborted)
                emit error("Errore Generico. Verificare la Connessione.");

            //cancello il webclient se è già stato creato
            if(webClient != NULL)
                webClient->~netClientGeneric();
        }
    }

    void smsSenderBase::managePostProcedureSignals(QString resultString, bool resultError, bool resultSend, bool forceDeleteCookies, bool &isPostprocedurePage)
    {
        isPostprocedurePage = false;

        if (resultError) {
            emit error(resultString);
            if (forceDeleteCookies)
                webClient->clearCookies();
            else
                optionalDeleteCookies();
        } else if (resultSend) {
            emit success(resultString);
            optionalDeleteCookies();
        }
    }

    bool smsSenderBase::manageErrorBreak(QString resultString, bool resultError, bool resultSend, bool forceDeleteCookies, bool &isPostprocedurePage, bool hasPostprocedurePage, logger &log)
    {
        if (isPostprocedurePage) {
            managePostProcedureSignals(resultString, resultError, resultSend, forceDeleteCookies, isPostprocedurePage);
            log.addNotice("Errore nella PostProcedure: " + resultString);
            return true;

        } else {

            if (hasPostprocedurePage) {
                isPostprocedurePage = true;
                return false;
            } else {
                emit error(resultString);
                optionalDeleteCookies();
                return true;
            }
        }
    }

    void smsSenderBase::setGetAdv(bool value)
    {
        getAdv = value;
    }

}
