/*
    Copyright (C) <2011>

    <enrico bacis> <enrico.bacis@gmail.com>
    <ivan vaccari> <grisson@jacksms.it>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    You can't modify the adv system, to cheat it.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "netclientqhttp.h"
#include <QEventLoop>
#include <QNetworkProxy>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QNetworkCookieJar>
#include <QFile>
#include <QDateTime>
#include <QNetworkCookie>
#include <QSslSocket>
#include <QSslConfiguration>

namespace libJackSMS{

    namespace netClient
    {

        void QMyNetworkCookieJar::saveToDisk(QString filename)
        {
            QList<QNetworkCookie> myCookieList = allCookies();
            QFile file(filename);
            if (file.open(QIODevice::ReadWrite | QIODevice::Text))
            {
                for(QList<QNetworkCookie>::const_iterator i = myCookieList.begin(); i != myCookieList.end(); ++i)
                {
                    file.write(i->toRawForm()+"\n");
                }

                file.close();
            }
        }

        void QMyNetworkCookieJar::loadFromDisk(QString filename)
        {
            QList<QNetworkCookie> myCookieList;
            QFile file(filename);
            if (file.open(QIODevice::ReadOnly | QIODevice::Text))
            {
                while (!file.atEnd())
                {
                    QByteArray line = file.readLine();
                    line.resize(line.length() - 1);
                    QList<QNetworkCookie> c = QNetworkCookie::parseCookies(line);

                    for (QList<QNetworkCookie>::iterator i = c.begin(); i != c.end(); ++i)
                    {
                        //QDateTime dd = i->expirationDate();
                        //QDateTime ct = QDateTime::currentDateTime();
                        //QDateTime say: if the cookie is a session cookie , epirationDate()
                        //will return an invalid date.. but i want the session cookie!
                        //if (!dd.isValid())
                        myCookieList.push_back(*i);
                        //else if (dd>ct)
                        //    myCookieList.push_back(*i);
                    }
                }

                setAllCookies(myCookieList);
                file.close();
            }
        }

        QByteArray QMyNetworkCookieJar::getRawData()
        {
            QByteArray raw;
            QList<QNetworkCookie> myCookieList = allCookies();
            for (QList<QNetworkCookie>::const_iterator i = myCookieList.begin(); i != myCookieList.end(); ++i)
            {
                raw.append(i->toRawForm()+"\n");
            }

            return raw;
        }

        void QMyNetworkCookieJar::clear()
        {
            setAllCookies(QList<QNetworkCookie>());
        }

        netClientQHttp::netClientQHttp(QObject *parent):
                QObject(parent),
                reply(NULL),
                outputHeaders(false),
                cookies(NULL),
                userAgentSetted(false),
                useCookies(false),
                //lastSocketIsSsl(false),
                proxyConfigured(false),
                aborted(false),
                error(false),
                errorPage(false)
        {
            // client=new QHttp;
            connect(&request, SIGNAL(finished(QNetworkReply*)), this, SLOT(endRequest(QNetworkReply*)));
            loop = new QEventLoop(this);
            connect(this, SIGNAL(pageDownloaded()), loop, SLOT(quit()));
        }

        netClientQHttp::~netClientQHttp()
        {
            if (reply != NULL)
                reply->deleteLater();

            if (cookies != NULL)
                cookies->deleteLater();

            loop->deleteLater();
        }

        void netClientQHttp::clearCookies()
        {
            QFile::remove(cookieFilename);
            if (cookies != NULL)
                cookies->clear();
        }

        void netClientQHttp::endRequest(QNetworkReply* reply)
        {
            if (reply->error() == QNetworkReply::NoError)
            {
                _lastReadedUrlCode = reply->readAll();
                error = false;
            }else{
                _lastReadedUrlCode=reply->errorString().toUtf8();

                error=true;
            }
            emit pageDownloaded();

        }

        bool netClientQHttp::setTimeout(int timeout)
        {
            //todo
            return false;
        }

        void netClientQHttp::IncludeHeaders()
        {
            //todo
            outputHeaders=true;

        }

        bool netClientQHttp::setCookieFile(const QString &_filename)
        {
            cookieFilename=_filename;
            if (QFile::exists(_filename)){
                cookies=new QMyNetworkCookieJar;
                cookies->loadFromDisk(_filename);
            }
            return false;
        }
        bool netClientQHttp::setUseCookie(bool _useCookie){
            useCookies=_useCookie;
            return true;
        }
        bool netClientQHttp::setUrl(const QString &_url){
            url=QUrl(QString::fromUtf8(_url.toStdString().c_str(),_url.length()));
            return true;
        }
        void netClientQHttp::setUserAgent(const QString &_useragent){
            ua=_useragent;
            userAgentSetted=true;
        }
        QString netClientQHttp::readPage(bool _ret){

            QNetworkRequest r(url);
            QSslConfiguration config = r.sslConfiguration();
            config.setPeerVerifyMode(QSslSocket::VerifyNone);
            r.setSslConfiguration(config);
            if (proxyConfigured)
                request.setProxy(proxy);
            if (useCookies && cookies!=NULL){
                request.setCookieJar(cookies);
            }

            QList<QPair<QByteArray, QByteArray> >::const_iterator i=headers.begin();
            bool overwrite=false;
            for (;i!=headers.end();++i){
                if (i->first.toUpper()==QByteArray("USER-AGENT"))
                    overwrite=true;
                r.setRawHeader(i->first,i->second);

            }
            if (userAgentSetted && !overwrite)
                r.setRawHeader("User-Agent",ua.toUtf8());
            reply=request.get(r);

            loop->exec(QEventLoop::ExcludeUserInputEvents);
            errorPage=error;
            if (error){
                error=false;
                headers.clear();
                return (!_ret)?"":QString(_lastReadedUrlCode);

            }else if (!aborted){
                //saving cookies
                if (useCookies){
                    cookies=static_cast<QMyNetworkCookieJar*>(request.cookieJar());
                    cookies->saveToDisk(cookieFilename);
                }

                QVariant redirect = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
                if (redirect.isValid()) {

                    QUrl redurl = redirect.toUrl();
                    if (redurl.isRelative())
                        redurl = url.resolved(redurl);

                    url = redurl;
                    readPage(false);
                }

                headers.clear();

                return (!_ret)?"":QString(_lastReadedUrlCode);
            }
            return "";


        }
        bool netClientQHttp::insertFormData(const QString &_field,const QString &_value){
            //QByteArray v=QByteArray::fromPercentEncoding(_value.toUtf8());
            //v=v.toPercentEncoding();

            queryStringFields.push_back(QPair<QString,QString>(_field,_value));
            //queryStringFields.push_back(QPair<QString,QString>(_field,QString(v)));
            return true;
        }

        QString netClientQHttp::submitPost(const QString &_url, bool _ret)
        {
            url = QUrl(QString::fromUtf8(_url.toStdString().c_str(),_url.length()));
            QNetworkRequest r(url);
            QSslConfiguration config = r.sslConfiguration();
            config.setPeerVerifyMode(QSslSocket::VerifyNone);
            r.setSslConfiguration(config);
            if (proxyConfigured)
                request.setProxy(proxy);
            if (useCookies && cookies!=NULL){
                request.setCookieJar(cookies);
            }
            QList<QPair<QByteArray, QByteArray> >::const_iterator i=headers.begin();
            bool overwrite=false;
            for (;i!=headers.end();++i){
                if (i->first.toUpper()==QByteArray("USER-AGENT"))
                    overwrite=true;
                r.setRawHeader(i->first,i->second);

            }
            if (userAgentSetted && !overwrite)
                r.setRawHeader("User-Agent",ua.toUtf8());
            makeQueryString(false);
            reply=request.post(r, lastQueryString.toUtf8());
            loop->exec(QEventLoop::ExcludeUserInputEvents);
            errorPage=error;
            if (error){
                error=false;
                headers.clear();
                return (!_ret)?"":QString(_lastReadedUrlCode);

            }else if (!aborted){
                //saving cookies
                if (useCookies){
                    cookies=static_cast<QMyNetworkCookieJar*>(request.cookieJar());
                    cookies->saveToDisk(cookieFilename);
                }

                QVariant redirect=reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
                if (redirect.isValid()){

                    QUrl redurl = redirect.toUrl();
                    if (redurl.isRelative())
                        redurl = url.resolved(redurl);

                    url = redurl;
                    readPage(false);
                }

                headers.clear();
                return (!_ret) ? "" : QString(_lastReadedUrlCode);
            }
            return "";
        }

        void netClientQHttp::makeQueryString(bool urlenc)
        {
            lastQueryString.clear();
            for (QList< QPair<QString, QString> >::const_iterator i = queryStringFields.begin(); i != queryStringFields.end(); ++i)
            {
                if (!lastQueryString.isEmpty())
                    lastQueryString.append("&");

                lastQueryString.append(i->first + "=");
                if (!i->second.isEmpty())
                {
                    if (urlenc)
                        lastQueryString.append(QByteArray::fromPercentEncoding(i->second.toUtf8()).toPercentEncoding());
                    else
                        lastQueryString.append(i->second.toUtf8().constData());

                }
            }

            queryStringFields.clear();
        }

        QString netClientQHttp::submitGet(const QString &_url, bool _ret)
        {
            QUrl _url2=QUrl(QString::fromUtf8(_url.toStdString().c_str(),_url.length()));
            QList<QPair <QString,QString> > fields=_url2.queryItems();
            fields.append(queryStringFields);
            url=_url2;
            url.setQueryItems(fields);

            QNetworkRequest r(url);
            QSslConfiguration config = r.sslConfiguration();
            config.setPeerVerifyMode(QSslSocket::VerifyNone);
            r.setSslConfiguration(config);

            if (proxyConfigured)
                request.setProxy(proxy);
            if (useCookies && cookies!=NULL){
                request.setCookieJar(cookies);
            }

            QList<QPair<QByteArray, QByteArray> >::const_iterator i=headers.begin();
            bool overwrite=false;
            for (;i!=headers.end();++i){
                if (i->first.toUpper()==QByteArray("USER-AGENT"))
                    overwrite=true;
                r.setRawHeader(i->first,i->second);

            }
            if (userAgentSetted && !overwrite)
                r.setRawHeader("User-Agent",ua.toUtf8());

            reply=request.get(r);
            loop->exec(QEventLoop::ExcludeUserInputEvents);
            errorPage=error;
            if (error){
                error=false;
                headers.clear();
                return (!_ret)?"":QString(_lastReadedUrlCode);

            }else if (!aborted){
                //saving cookies
                if (useCookies){
                    cookies=static_cast<QMyNetworkCookieJar*>(request.cookieJar());
                    cookies->saveToDisk(cookieFilename);
                }

                QVariant redirect=reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
                if (redirect.isValid()){

                    QUrl redurl = redirect.toUrl();
                    if (redurl.isRelative())
                        redurl = url.resolved(redurl);

                    url = redurl;
                    readPage(false);
                }

                headers.clear();
                return (!_ret)?"":QString(_lastReadedUrlCode);
            }
            return "";
        }

        QString netClientQHttp::currentUrl()
        {
            return url.toString();
        }

        bool netClientQHttp::findCaptcha(const QString &_left,const QString &_right)
        {
            int _pleft=_lastReadedUrlCode.indexOf(_left);
            if (_pleft!= -1){
                int _pright=_lastReadedUrlCode.indexOf(_right,_pleft);
                if (_pright!=-1){
                    if (_pleft <_pright)
                        return true;
                    else
                        return false;
                }else{
                    return false;
                }
            }else
                return false;
        }

        bool netClientQHttp::getFile(const QString &_url, QByteArray &_fileBytes)
        {
            url=_url;
            readPage(false);
            _fileBytes=_lastReadedUrlCode;
            return true;
        }

        bool netClientQHttp::addHeader(const QString &_name, const QString &_value)
        {
            headers.push_back(qMakePair(_name.toUtf8(), _value.toUtf8()));
            return true;
        }

        bool netClientQHttp::setProxyServer(const QString & _proxyServer, const QString &_port, const QString &_proxyType)
        {
            proxyConfigured = true;
            proxy.setHostName(_proxyServer);
            bool ok;
            proxy.setPort(_port.toInt(&ok, 10));
            if (_proxyType.toUpper() == "HTTP")
                proxy.setType(QNetworkProxy::HttpProxy);
            else if (_proxyType.toUpper() == "SOCKS5")
                proxy.setType(QNetworkProxy::Socks5Proxy);

            return true;
        }

        bool netClientQHttp::setProxyAuthentication(const QString &_proxyUsername, const QString &_proxyPassword)
        {
            proxy.setUser(_proxyUsername);
            proxy.setPassword(_proxyPassword);
            return true;
        }

        bool netClientQHttp::interrupt()
        {
            aborted = true;

            emit pageDownloaded();

            if (reply != NULL)
                reply->abort();

            return true;
        }

        bool netClientQHttp::getAborted()
        {
            return aborted;
        }

        bool netClientQHttp::hasError()
        {
            return errorPage;
        }

    }

}
