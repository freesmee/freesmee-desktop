#ifndef NETCLIENTQHTTP_H
#define NETCLIENTQHTTP_H

#include <QObject>
#include "NetClient.h"
#include <QNetworkAccessManager>
#include <QUrl>
#include <QEventLoop>
#include <QList>
#include <QNetworkProxy>
#include <QNetworkReply>
#include <QNetworkCookieJar>
namespace libJackSMS{

    namespace netClient{
        class netClientQHttp : public QObject,public netClientGeneric
        {
        Q_OBJECT
        private:
//            QHttp *client;
            QNetworkAccessManager request;
            QNetworkReply *reply;
            bool outputHeaders;
            QUrl url;
            QNetworkCookieJar *cookies;
            QList<QPair <QByteArray,QByteArray> > headers;
            bool userAgentSetted;
            QByteArray _lastReadedUrlCode;
            bool useCookies;
            QEventLoop *loop;
//            bool lastSocketIsSsl;
            QList<QPair <QString,QString> > queryStringFields;
            void setSocket();
            QString lastQueryString;
            void makeQueryString(bool urlenc);
            QNetworkProxy proxy;
            bool proxyConfigured;
//            QString queryItems();
            bool aborted;
            bool error;
//            void parseCookies();
//            bool checkRedirects();
//            //hostname,mapOfCookieForThisHost
//            QMap<QString,QString> cookies;
//            void addCookies();
           QString ua;
        public:
            netClientQHttp(QObject *parent = 0);
            ~netClientQHttp();
            bool setTimeout(int timeout);
            void IncludeHeaders();
            bool setCookieFile(const QString &_filename,bool savecookiefile=true);
            bool setUseCookie(bool _useCookie);
            bool setUrl(const QUrl &_url);
            void setUserAgent(const QString &_useragent);
            QString readPage(bool _ret=false);
            bool insertFormData(const QString &_field,const QString &_value);
            QString submitPost(const QUrl &_url,bool _ret=false);
            QString submitGet(const QUrl &_url,bool _ret=false);
            QString currentUrl();
            bool findCaptcha(const QString &_left,const QString &_right);
            bool getFile(const QString &_url,QByteArray &_fileBytes);
            bool addHeader(const QString &_name,const QString &_value);
            bool setProxyServer(const QString & _proxyServer,const QString &_port,const QString &_proxyType);
            bool setProxyAuthentication(const QString &_proxyUsername, const QString &_proxyPassword);
            bool interrupt();
            bool getAborted();
        signals:
            void pageDownloaded();

        private slots:
            void endRequest(QNetworkReply*);
            /*void MainWindow::action(int id);
            void MainWindow::action2(int id,bool a);
*/


        };
    }
}
#endif // NETCLIENTQHTTP_H
