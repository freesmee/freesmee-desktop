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

#include "SocketClient.h"
#include <QString>

namespace libJackSMS
{
    namespace netClient
    {
        SocketClient::SocketClient(QObject *parent) :
                QObject(parent),
                socket(new QTcpSocket)
        {
        }

        SocketClient::~SocketClient()
        {
                delete socket;
        }

        void SocketClient::setHost(QString host, QString _port)
        {
            hostAddr = host;
            port = _port.toUInt();
        }

        bool SocketClient::connectToServer()
        {
            socket->connectToHost(hostAddr, port);
            return socket->waitForConnected(30000);
        }

        bool SocketClient::abort()
        {
            socket->abort();
            return true;
        }

        QString SocketClient::readLine()
        {
            if (!pampers.isEmpty())
            {
                QByteArray p = pampers.first();
                pampers.removeFirst();
                return QString(p);
            }
            else
            {
                socket->waitForReadyRead(-1);
                QByteArray a = socket->read(20000);

                if (a.at(0) == '\0')
                    a = a.right(a.length() - 1);

                a = a.left(a.length() - 2);
                QList<QByteArray> lines = a.split('\0');

                for (QList<QByteArray>::iterator i = lines.begin(); i != lines.end(); ++i)
                {
                    QList<QByteArray> ll = i->split('\n');
                    pampers.append(ll);
                }

                QByteArray p = pampers.first();
                pampers.removeFirst();
                return QString(p);
            }
        }

        bool SocketClient::writeLine(const QString &_data)
        {
            if (_data.length() + 2 == socket->write((_data+QString("\r\n")).toUtf8()))
                return true;
            else
                return false;
        }

        bool SocketClient::disconnect()
        {
            socket->disconnectFromHost();
            return socket->waitForDisconnected(30000);
        }
    }
}
