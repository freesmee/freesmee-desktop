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

#include "streamer.h"
#include "Configuration.h"
#include "json/json.h"
#include <QSslConfiguration>

namespace libJackSMS
{
    namespace serverApi
    {
        Streamer::Streamer(QString _loginId):
                loginString(_loginId),
                id(0),
                status(waitConnResponse),
                queueCount(0),
                waitResponseOfSend(false),
                stopped(false)
        {
            qRegisterMetaType<libJackSMS::dataTypes::dateTime>("libJackSMS::dataTypes::dateTime");
            qRegisterMetaType<libJackSMS::dataTypes::logImMessage>("libJackSMS::dataTypes::logImMessage");
            qRegisterMetaType<libJackSMS::dataTypes::phoneNumber>("libJackSMS::dataTypes::phoneNumber");
            qRegisterMetaType<libJackSMS::dataTypes::logSmsMessage>("libJackSMS::dataTypes::logSmsMessage");
            qRegisterMetaType<QAbstractSocket::SocketError>("QAbstractSocket::SocketError");

            connect(&signalCountdown, SIGNAL(timeout()), this, SLOT(launchSignal()));
            connect(&sock, SIGNAL(disconnected()), this, SLOT(relaunchDisconnected()));
            connect(&sock, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(errorDisconnected(QAbstractSocket::SocketError)));
            connect(&sock, SIGNAL(readyRead()), this, SLOT(dataReceived()));
            connect(&sock, SIGNAL(connected()), this, SLOT(connectDone()));
            connect(&sock, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(state(QAbstractSocket::SocketState)));
            connect(&pingTimer, SIGNAL(timeout()), this, SLOT(ping()));
            connect(&pingTimeout, SIGNAL(timeout()), this, SLOT(pingTimeoutError()));

            pingTimeout.setSingleShot(true);
        }

        qint64 Streamer::streamAPI(QString api, QString params) {
            QString fullapi = "GET /" + api + "?" + params + "&token=" + loginString + "&desktop=" + FREESMEE_VERSION + " HTTP/1.1\n\n";
            // qDebug() << fullapi;
            qint64 result = sock.write(fullapi.toUtf8());
            sock.flush();
            return result;
        }

        void Streamer::relaunchDisconnected()
        {
            pingTimer.stop();
            pingTimeout.stop();
            sock.abort();
            buffer.clear();
            emit serviceNotActive(false);
        }

        void Streamer::tryReconnect()
        {
            activateServ();
        }

        void Streamer::errorDisconnected(QAbstractSocket::SocketError e)
        {
            pingTimer.stop();
            pingTimeout.stop();
            sock.abort();
            buffer.clear();

            emit serviceNotActive(true, QString::number(e) + ": " + sock.errorString());
        }

        void Streamer::state(QAbstractSocket::SocketState)
        {
        }

        void Streamer::pingTimeoutError()
        {
            pingTimer.stop();
            pingTimeout.stop();
            buffer.clear();

            try
            {
                sock.abort();
                emit serviceNotActive(true, "Timeout");
            } catch(...) {
                emit serviceNotActive(true, "Errore sconosciuto: section 3");
            }
        }

        void Streamer::stop()
        {
            if (!stopped)
            {
                pingTimer.stop();
                pingTimeout.stop();
                sock.disconnectFromHost();

                if (sock.state() != QAbstractSocket::UnconnectedState)
                    sock.waitForDisconnected(10000);

                sock.reset();
                stopped = true;
            }
        }

        void Streamer::ping()
        {
            if (streamAPI("ping") != -1)
                pingTimeout.start(100 * 1000);
            else {
                pingTimer.stop();
                pingTimeout.stop();
                emit serviceNotActive(true, "Ping failed");
            }
        }

        void Streamer::dataReceived()
        {
            dataTimeout.stop();
            buffer = buffer.append(sock.readAll());
            dataTimeout.singleShot(500, this, SLOT(parseLine()));
            // qDebug() << buffer;
        }

        void  Streamer::parseLine()
        {
            try
            {
                QList<QByteArray> finalLines;
                QList<QByteArray> lines = buffer.split('\0');

                for (QList<QByteArray>::iterator i = lines.begin(); i != lines.end(); ++i)
                {
                    QList<QByteArray> ll = i->split('\n');
                    finalLines.append(ll);
                }

                buffer.clear();
                finalLines.removeAll(""); //remove empty lines

                while (!finalLines.isEmpty())
                {
                    QString line = QString(finalLines.takeFirst()).trimmed();
                    if (line.startsWith('{') && line.endsWith('}')) {
                        bool ok;
                        QVariantMap result = Json::parse(line, ok).toMap();
                        if (ok)
                        {
                            if (status == waitConnResponse)
                            {
                                if (result["status"].toInt() == 1) {
                                    emit serviceActive();
                                    status = connected;
                                } else {
                                    emit serviceNotActive(true, result["message"].toString());
                                }
                            }
                            else if (status == connected)
                            {
                                if (result["status"].toString() == "1" && result["type"].toString() == "Q")
                                {
                                    idList.clear();
                                    QVariantList messages = result["queue"].toList();

                                    foreach (QVariant m, messages)
                                    {
                                        QVariantMap mm = m.toMap();

                                        QString text = mm["message"].toString();
                                        text = QString::fromUtf8(text.toStdString().c_str());

                                        QString id = mm["msg_id"].toString();
                                        idList.push_back(id);

                                        QString timediff = mm["timediff"].toString();
                                        int timediffInt = (-1) * timediff.toInt();
                                        QDateTime dt = QDateTime::currentDateTime().addSecs(timediffInt);

                                        QString recipNumber = mm["sender"].toString();
                                        libJackSMS::dataTypes::phoneNumber num;
                                        num.parse(recipNumber);
                                        if (!num.getIsValid())
                                            num.parse("2727");

                                        libJackSMS::dataTypes::dateTime dat(dt);
                                        libJackSMS::dataTypes::logImMessage im(num, dat, "", text);
                                        signalCountdown.stop();
                                        im.setId(id);
                                        imLog.insert(im.getId(), im);
                                        signalCountdown.start(1500);
                                    }

                                    streamAPI("queueAck", "id=" + idList.join(","));

                                }
                                else if (result["status"].toString() == "1" && result["type"].toString() == "R")
                                {
                                    signalCountdown.stop();

                                    QString ackid = result["ack"].toString();
                                    streamAPI("ack", "id=" + ackid);

                                    QString text = result["message"].toString();
                                    text = QString::fromUtf8(text.toStdString().c_str());
                                    QString recipNumber = result["sender"].toString();
                                    QDateTime dt = QDateTime::currentDateTime();

                                    libJackSMS::dataTypes::phoneNumber num;
                                    num.parse(recipNumber);

                                    if (!num.getIsValid())
                                        num.parse("2727");

                                    libJackSMS::dataTypes::dateTime dat(dt);
                                    libJackSMS::dataTypes::logImMessage im(num, dat, "", text);
                                    im.setId(QString::number(++id));
                                    imLog.insert(im.getId(), im);
                                    signalCountdown.start(1500);

                                }
                                else if (result["status"].toString() == "1" && result["type"].toString() == "P")
                                {
                                    pingTimeout.stop();
                                }
                                else if (result["status"].toString() == "0" && result["type"].toString() == "P")
                                {
                                    pingTimer.stop();
                                    pingTimeout.stop();
                                    buffer.clear();
                                    sock.close();
                                    emit serviceNotActive(true, "Pong failed");
                                }
                            }

                        } else
                            emit serviceNotActive(true, "Unexpected response");
                    }
                }
            } catch (...) {
                pingTimer.stop();
                pingTimeout.stop();
                sock.close();
                emit serviceNotActive(true, "Errore sconosciuto");
            }
        }

        Streamer::~Streamer()
        {
            if (sock.state() != QAbstractSocket::UnconnectedState)
                sock.waitForDisconnected(10000);
        }

        void Streamer::connectDone()
        {
            try
            {
                streamAPI("stream");
                pingTimer.start(1000 * 60);
            } catch(...) {
                emit serviceNotActive(true, "Errore sconosciuto: section 6");
            }
        }

        void Streamer::launchSignal()
        {
            try
            {
                signalCountdown.stop();
                emit newJMS(imLog);
                imLog.clear();
                id = 0;
            } catch(...) {
                pingTimer.stop();
                pingTimeout.stop();
                emit serviceNotActive(true, "Errore sconosciuto: section 5");
            }
        }

        void Streamer::activateServ()
        {
            try
            {
                id = 0;
                status = waitConnResponse;
                emit serviceActivating();

                stopped = false;

                sock.setSocketOption(QAbstractSocket::KeepAliveOption, 1);
                sock.setSocketOption(QAbstractSocket::LowDelayOption, 1);
                sock.connectToHost("stream.freesmee.com", 80);
                sock.waitForConnected();
                sock.setSocketOption(QAbstractSocket::KeepAliveOption, 1);
                sock.setSocketOption(QAbstractSocket::LowDelayOption, 1);

            } catch(...) {
                emit serviceNotActive(true, "Errore sconosciuto: section 4");
            }
        }
    }
}
