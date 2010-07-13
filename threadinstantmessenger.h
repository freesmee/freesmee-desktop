#ifndef THREADINSTANTMESSENGER_H
#define THREADINSTANTMESSENGER_H

#include <QTimer>

#include <QTcpSocket>
#include "libjacksms/libJackSMS.h"
#include <QNetworkProxy>
class threadInstantMessenger:public QObject
{
    Q_OBJECT
private:

    libJackSMS::dataTypes::logImType &imLog;
    QString username;
    QString password;
    QString error;
    const libJackSMS::dataTypes::optionsType & opzioni;
    QString  userDir;
    QTimer signalCountdown;
    QTimer pingTimer;
    QTimer pingTimeout;
    QTimer reconnectTimer;
    QTcpSocket sock;
    QNetworkProxy proxy;

public:

    threadInstantMessenger(libJackSMS::dataTypes::logImType &_imLog,QString _username,QString _password,const libJackSMS::dataTypes::optionsType &_opzioni,const QString & _userDir);
    ~threadInstantMessenger();
    void activateServ();
    QString getError() const;
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
signals:
    void newMessages();
    void stopService();
    void serviceActive();
    void serviceNotActive(bool err);
    void serviceActiving();



};

#endif // THREADINSTANTMESSENGER_H
