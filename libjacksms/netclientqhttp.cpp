#include "netclientqhttp.h"
#include <QEventLoop>
#include <QNetworkProxy>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QNetworkCookieJar>
namespace libJackSMS{

    namespace netClient{
        netClientQHttp::netClientQHttp(QObject *parent):
            QObject(parent),
            outputHeaders(false),
            cookies(NULL),
            userAgentSetted(false),
            useCookies(false),
            //lastSocketIsSsl(false),
            proxyConfigured(false),
            aborted(false),
            error(false)
        {
           // client=new QHttp;
            connect(&request,SIGNAL(finished(QNetworkReply*)),this,SLOT(endRequest(QNetworkReply*)));
            loop=new QEventLoop(this);
            connect(this, SIGNAL(pageDownloaded()), loop, SLOT(quit()));


        }
        netClientQHttp::~netClientQHttp(){
            //client->~QHttp();

        }

        void netClientQHttp::endRequest(QNetworkReply* reply){
            if (reply->error()==QNetworkReply::NoError){
                _lastReadedUrlCode=reply->readAll();
                error=false;
            }else{
                _lastReadedUrlCode=reply->errorString().toAscii();
                error=true;
            }
             emit pageDownloaded();

        }
        bool netClientQHttp::setTimeout(int timeout){
            return false;
        }
        void netClientQHttp::IncludeHeaders(){
            outputHeaders=true;

        }
        bool netClientQHttp::setCookieFile(const QString &_filename,bool savecookiefile){

            return false;
        }
        bool netClientQHttp::setUseCookie(bool _useCookie){
            useCookies=_useCookie;
            return false;
        }
        bool netClientQHttp::setUrl(const QUrl &_url){
            url=_url;
            return true;
        }
        void netClientQHttp::setUserAgent(const QString &_useragent){
            ua=_useragent;
            userAgentSetted=true;
        }
        QString netClientQHttp::readPage(bool _ret){

            QNetworkRequest r(url);
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
                r.setRawHeader("User-Agent",ua.toAscii());
            reply=request.get(r);

            loop->exec(QEventLoop::ExcludeUserInputEvents);

            if (error){
                error=false;
                headers.clear();
                return (!_ret)?"":QString(_lastReadedUrlCode);

            }else if (!aborted){
                //saving cookies
                if (useCookies){
                    cookies=request.cookieJar();
                }

                QVariant redirect=reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
                if (redirect.isValid()){
                    url=redirect.toUrl();
                    readPage(false);
                }

                headers.clear();

                return (!_ret)?"":QString(_lastReadedUrlCode);
            }
            return "";


        }
        bool netClientQHttp::insertFormData(const QString &_field,const QString &_value){
            queryStringFields.push_back(QPair<QString,QString>(_field,_value));
            return true;
        }

        QString netClientQHttp::submitPost(const QUrl &_url,bool _ret){
            url=_url;
            QNetworkRequest r(url);
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
                r.setRawHeader("User-Agent",ua.toAscii());
            makeQueryString(false);
            reply=request.post(r,lastQueryString.toAscii());
            loop->exec(QEventLoop::ExcludeUserInputEvents);

            if (error){
                error=false;
                headers.clear();
                return (!_ret)?"":QString(_lastReadedUrlCode);

            }else if (!aborted){
                //saving cookies
                if (useCookies){
                    cookies=request.cookieJar();
                }

                QVariant redirect=reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
                if (redirect.isValid()){
                    url=redirect.toUrl();
                    readPage(false);
                }

                headers.clear();
                return (!_ret)?"":QString(_lastReadedUrlCode);
            }
            return "";
        }
        void netClientQHttp::makeQueryString(bool urlenc){
            QList<QPair<QString, QString> >::const_iterator i=queryStringFields.begin();
            lastQueryString.clear();
            for (;i!=queryStringFields.end();++i){
                if (!lastQueryString.isEmpty()){
                    lastQueryString.append("&");
                }
                lastQueryString.append(i->first+"=");
                if (!i->second.isEmpty()){
                    if (urlenc){
                       lastQueryString.append(i->second.toAscii().toPercentEncoding());
                    }else{
                        lastQueryString.append(i->second.toAscii().constData());
                    }

                }

            }
            queryStringFields.clear();

        }
        QString netClientQHttp::submitGet(const QUrl &_url,bool _ret){
            url=_url;
            url.setQueryItems(queryStringFields);
            QNetworkRequest r(url);
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
                r.setRawHeader("User-Agent",ua.toAscii());

            reply=request.get(r);
            loop->exec(QEventLoop::ExcludeUserInputEvents);

            if (error){
                error=false;
                headers.clear();
                return (!_ret)?"":QString(_lastReadedUrlCode);

            }else if (!aborted){
                //saving cookies
                if (useCookies){
                    cookies=request.cookieJar();
                }

                QVariant redirect=reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
                if (redirect.isValid()){
                    url=redirect.toUrl();

                    readPage(false);
                }

                headers.clear();
                return (!_ret)?"":QString(_lastReadedUrlCode);
            }
            return "";
        }
        QString netClientQHttp::currentUrl(){
            return url.toString();
        }
        bool netClientQHttp::findCaptcha(const QString &_left,const QString &_right){
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
        bool netClientQHttp::getFile(const QString &_url,QByteArray &_fileBytes){
            url=_url;
            readPage(false);
            _fileBytes=_lastReadedUrlCode;
            return true;
        }

        bool netClientQHttp::addHeader(const QString &_name,const QString &_value){
            headers.push_back(qMakePair(_name.toAscii(),_value.toAscii()));

            return true;
        }
        bool netClientQHttp::setProxyServer(const QString & _proxyServer,const QString &_port,const QString &_proxyType){
            proxyConfigured=true;
            proxy.setHostName(_proxyServer);
            bool ok;
            proxy.setPort(_port.toInt(&ok,10));
            if (_proxyType.toUpper()=="HTTP")
                proxy.setType(QNetworkProxy::HttpProxy);
            else if (_proxyType.toUpper()=="SOCKS5")
                proxy.setType(QNetworkProxy::Socks5Proxy);

            return true;
        }
        bool netClientQHttp::setProxyAuthentication(const QString &_proxyUsername, const QString &_proxyPassword){
            proxy.setUser(_proxyUsername);
            proxy.setPassword(_proxyPassword);
            return true;
        }
        bool netClientQHttp::interrupt(){

            aborted=true;
            emit pageDownloaded();
            reply->abort();
            return true;
        }
        bool netClientQHttp::getAborted(){
            return aborted;
        }
    }
}
