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

#ifndef STREAMER_H
#define STREAMER_H

#include <QString>
#include <QNetworkReply>
#include <QObject>
#include <QTimer>
#include <QTcpSocket>
#include <QStringList>
#include "DataTypes.h"
#include "SocketClient.h"
#include "json/json.h"
#include "ServerApi.h"

namespace libJackSMS
{
    namespace serverApi
    {
            class Streamer : public QObject
            {
                Q_OBJECT

            private:
                QString loginString;
                QString error;
                QTimer pingTimer;
                QTimer signalCountdown;
                QTimer pingTimeout;
                QTimer dataTimeout;
                QTcpSocket sock;
                libJackSMS::dataTypes::logImType imLog;
                libJackSMS::dataTypes::logSmsMessage _msg;
                int id;
                QByteArray buffer;

                QStringList idList;
                enum streamerState{waitConnResponse = 1,connected = 2};

                streamerState status;
                int queueCount;
                bool waitResponseOfSend;
                bool stopped;

                qint64 streamAPI(QString api, QString params = "");
                QString callAPI(QString api, QString params = "");

            public:
                Streamer(QString _loginId);
                ~Streamer();
                void activateServ();
                void stop();

            private slots:
                void launchSignal();
                void connectDone();
                void parseLine();
                void relaunchDisconnected();
                void errorDisconnected(QAbstractSocket::SocketError);
                void state(QAbstractSocket::SocketState e);
                void ping();
                void pingTimeoutError();
                void tryReconnect();
                void dataReceived();

            signals:
                void newJMS(libJackSMS::dataTypes::logImType);
                void serviceActive();
                void serviceNotActive(bool err = false, QString errStr = "");
                void serviceActivating();
        };
    }
}

#endif // STREAMER_H
