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
#include <QNetworkCookie>
namespace libJackSMS{

    namespace netClient{
        class QMyNetworkCookieJar:public QNetworkCookieJar{
            Q_OBJECT


            public:
                void clear();
                void saveToDisk(QString filename);
                void loadFromDisk(QString filename);
                QByteArray getRawData();

        };
        class netClientQHttp : public QObject,public netClientGeneric
        {
        Q_OBJECT
        private:
            
            QNetworkAccessManager request;
            QNetworkReply *reply;
            bool outputHeaders;
            QUrl url;
            QMyNetworkCookieJar *cookies;
            QList<QPair <QByteArray,QByteArray> > headers;
            bool userAgentSetted;
            QByteArray _lastReadedUrlCode;
            bool useCookies;
            QString cookieFilename;
            QEventLoop *loop;
            QList<QPair <QString,QString> > queryStringFields;
            void setSocket();
            QString lastQueryString;
            void makeQueryString(bool urlenc);
            QNetworkProxy proxy;
            bool proxyConfigured;
            bool aborted;
            bool error;

            QString ua;
            bool errorPage;
        public:
            netClientQHttp(QObject *parent = 0);
            ~netClientQHttp();
            bool setTimeout(int timeout);
            void IncludeHeaders();
            bool setCookieFile(const QString &_filename);
            bool setUseCookie(bool _useCookie);
            bool setUrl(const QString &_url);
            void setUserAgent(const QString &_useragent);
            QString readPage(bool _ret=false);
            bool insertFormData(const QString &_field,const QString &_value);
            QString submitPost(const QString &_url,bool _ret=false);
            QString submitGet(const QString &_url,bool _ret=false);
            QString currentUrl();
            bool findCaptcha(const QString &_left,const QString &_right);
            bool getFile(const QString &_url,QByteArray &_fileBytes);
            bool addHeader(const QString &_name,const QString &_value);
            bool setProxyServer(const QString & _proxyServer,const QString &_port,const QString &_proxyType);
            bool setProxyAuthentication(const QString &_proxyUsername, const QString &_proxyPassword);
            bool interrupt();
            bool getAborted();
            void clearCookies();
            bool hasError();
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
