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

#include "streamer.h"
#include "Configuration.h"
#include "json/json.h"
#include <QSslConfiguration>

namespace libJackSMS
{
    namespace serverApi
    {
        Streamer::Streamer(QString _loginId, dataTypes::proxySettings _ps):
                loginString(_loginId),
                ps(_ps),
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
            if (sock.write((QString("GET /ping HTTP/1.1\n\n")).toUtf8()) != -1)
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
                    QString line = QString(finalLines.first()).trimmed();
                    if (line.startsWith("HTTP/1.1"))
                    {
                        while (true)
                        {
                            finalLines.removeFirst();
                            line = QString(finalLines.first()).trimmed();

                            if (line.isEmpty())
                            {
                                //remove empty line
                                finalLines.removeFirst();

                                //skip length
                                line = QString(finalLines.first()).trimmed();
                                finalLines.removeFirst();
                                //get interesting line :D

                                if (finalLines.count() > 0)
                                    line = QString(finalLines.first()).trimmed();
                                else
                                    line = "";

                                break;
                            }
                        }
                    }

                    finalLines.removeFirst();

                    if (status == waitConnResponse)
                    {
                        bool ok;
                        line.toInt(&ok, 16);

                        if (ok)
                        {
                            line = QString(finalLines.first()).trimmed();
                            finalLines.removeFirst();
                        }

                        QVariantMap result = Json::parse(line, ok).toMap();
                        if (ok)
                        {
                            if (result["status"].toInt() == 1)
                            {
                                emit serviceActive();
                                status = connected;
                            } else {
                                emit serviceNotActive(true, result["message"].toString());
                            }

                        } else {
                            emit serviceNotActive(true, "Unexpected response");
                        }

                    } else if (status == connected){
                        bool ok;
                        line.toInt(&ok, 16);

                        if (ok)
                        {
                            line = QString(finalLines.first()).trimmed();
                            finalLines.removeFirst();
                        }

                        QVariantMap result = Json::parse(line, ok).toMap();
                        if (ok)
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

                                QString idl = idList.join(",");
                                QString sn = QString("GET /queueAck?id=") + idl + QString(" HTTP/1.1\n\n");
                                sock.write(sn.toUtf8());

                            } else if (result["status"].toString() == "1" && result["type"].toString() == "R") {
                                signalCountdown.stop();

                                QString text = result["message"].toString();
                                text = QString::fromUtf8(text.toStdString().c_str());

                                QString ackid = result["ack"].toString();

                                QString recipNumber = result["sender"].toString();

                                QDateTime dt = QDateTime::currentDateTime();

                                QString sn = QString("GET /ack?id=") + ackid + QString(" HTTP/1.1\n\n");
                                sock.write(sn.toUtf8());

                                libJackSMS::dataTypes::phoneNumber num;
                                num.parse(recipNumber);
                                if (!num.getIsValid())
                                    num.parse("2727");

                                libJackSMS::dataTypes::dateTime dat(dt);
                                libJackSMS::dataTypes::logImMessage im(num, dat, "", text);
                                im.setId(QString::number(id++));
                                imLog.insert(im.getId(), im);
                                signalCountdown.start(1500);

                            } else if (result["status"].toString() == "1" && result["type"].toString() == "P") {
                                pingTimeout.stop();
                            } else if (result["status"].toString() == "0" && result["type"].toString() == "P") {
                                pingTimer.stop();
                                pingTimeout.stop();
                                buffer.clear();
                                sock.close();
                                emit serviceNotActive(true, "Pong failed");
                            }
                        }
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
                QString sn = QString("GET /stream?token=" + loginString + "&desktop=") + QString(FREESMEE_VERSION) + QString(" HTTP/1.1\n\n");
                sock.write(sn.toUtf8());

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
            }catch(...){
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
                if (ps.useProxy())
                {
                    proxy.setHostName(ps.getServer());
                    bool ok;
                    proxy.setPort(ps.getPort().toInt(&ok, 10));

                    if (ps.getType().toUpper() == "HTTP")
                        proxy.setType(QNetworkProxy::HttpProxy);
                    else if (ps.getType().toUpper() == "SOCKS5")
                        proxy.setType(QNetworkProxy::Socks5Proxy);

                    if (ps.useAuth()){
                        proxy.setUser(ps.getUsername());
                        proxy.setPassword(ps.getPassword());
                    }

                    sock.setProxy(proxy);
                }

                stopped = false;
                sock.connectToHost("stream.freesmee.com", 80);

            } catch(...) {
                emit serviceNotActive(true,"Errore sconosciuto: section 4");
            }
        }
    }
}
