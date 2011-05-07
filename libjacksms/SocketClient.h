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

#ifndef SOCKETCLIENT_H
#define SOCKETCLIENT_H

#include <QString>
#include <QTcpSocket>
#include <QHostAddress>
#include <QNetworkProxy>
#include <QObject>

namespace libJackSMS
{
    namespace netClient
    {
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
