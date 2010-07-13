#ifndef SOCKETCLIENT_H
#define SOCKETCLIENT_H
#include <QString>
#include <QTcpSocket>
#include <QHostAddress>
#include <QNetworkProxy>
#include <QObject>
namespace libJackSMS{

    namespace netClient{
        class SocketClient:public QObject
        {
            Q_OBJECT
        private:
            QTcpSocket *socket;
            QString hostAddr;
            qint16 port;
            QNetworkProxy proxy;
            bool proxyConfigured;
            QList<QByteArray> pampers;
        public:
            SocketClient(QObject *parent);
            ~SocketClient();
            void setHost(QString host,QString port);
            bool connectToServer();
            QString readLine();
            bool writeLine(const QString &_data);
            bool disconnect();
            bool abort();
            bool setProxyServer(const QString & _proxyServer,const QString &_port,const QString &_proxyType);
            bool setProxyAuthentication(const QString &_proxyUsername, const QString &_proxyPassword);
        signals:
            void readyToDoSomething();
        };
   }
}
#endif // SOCKETCLIENT_H
