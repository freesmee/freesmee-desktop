#ifndef STREAMER_H
#define STREAMER_H

#include <QString>
#include "DataTypes.h"
#include "ProxyConfig.h"
#include "SocketClient.h"
#include <QObject>
#include <QTimer>
#include <QTcpSocket>
#include <QStringList>
#include "json/json.h"
#include <QNetworkConfigurationManager>
#include "ServerApi.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>

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
                //QString  userDir;
                QTimer pingTimer;
                QTimer signalCountdown;
                QTimer pingTimeout;
                //QTimer reconnectTimer;
                QTimer dataTimeout;
                QTcpSocket sock;
                //QTcpSocket sendSock;
                QNetworkProxy proxy;
                dataTypes::proxySettings ps;
                libJackSMS::dataTypes::logImType imLog;
                libJackSMS::dataTypes::logSmsMessage _msg;
                int id;
                QByteArray buffer;
                //QByteArray bufferSend;
                //QTimer dataTimeoutSend;

                QStringList idList;
                enum streamerState{waitConnResponse=1,connected=2};

                streamerState status;
                int queueCount;
                bool waitResponseOfSend;
                //QString sendSessionId;
                //int multiplier;
                bool stopped;
                //libJackSMS::dataTypes::configuredAccount ac;
                //QNetworkConfigurationManager manager;
                //QNetworkSession* networkSession;
                //QNetworkConfiguration networkConfig;
                //QString captchaSolution;

            public:
                Streamer(QString _loginId, dataTypes::proxySettings _ps);
                ~Streamer();
                void activateServ();
                void stop();
                //void cancelSend();
                //bool sendSms(QString _text,libJackSMS::dataTypes::phoneNumber _dest,libJackSMS::dataTypes::configuredAccount);
                //void continueSendSms(QString _captchaSolution);
                //libJackSMS::dataTypes::logSmsMessage getSendedMessage()const;

            private slots:
                //void connectSend();
                //void continueSendConnectDone();
                //void sendConnectDone();
                //void sendDataReceived();
                //void parseLineSend();
                //void sendErrorDisconnected(QAbstractSocket::SocketError e);
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
                //void smsSendCompleted(bool smsSended,QString _respText,QString _queueSize,QString _smsSendedbyAccount,QString _destCarrier,QString _destCanReceiveJMS);
                void newJMS(libJackSMS::dataTypes::logImType);
                void serviceActive();
                void serviceNotActive(bool err=false,QString errStr="");
                void serviceActivating();
                //void captchaRequest(QByteArray _bytes);

        };
    }
}

#endif // STREAMER_H
