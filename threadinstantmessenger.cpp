#include "libjacksms/libJackSMS.h"
#include "threadinstantmessenger.h"
#include <QMetaType>

threadInstantMessenger::threadInstantMessenger(libJackSMS::dataTypes::logImType &_imLog,QString _username,QString _password,const libJackSMS::dataTypes::optionsType &_opzioni,const QString & _userDir ):
        imLog(_imLog),username(_username),password(_password),opzioni(_opzioni),userDir(_userDir)
{

    qRegisterMetaType<libJackSMS::dataTypes::dateTime>("libJackSMS::dataTypes::dateTime");
    qRegisterMetaType<libJackSMS::dataTypes::logImMessage>("libJackSMS::dataTypes::logImMessage");
    qRegisterMetaType<QAbstractSocket::SocketError>("QAbstractSocket::SocketError");
    connect(&signalCountdown,SIGNAL(timeout()),this,SLOT(launchSignal()));
    connect(&sock,SIGNAL(disconnected()),this,SLOT(relaunchDisconnected()));
    connect(&sock,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(errorDisconnected(QAbstractSocket::SocketError)));
    connect(&sock,SIGNAL(readyRead()),this,SLOT(parseLine()));
    connect(&sock,SIGNAL(connected()),this,SLOT(connectDone()));
    connect(&sock,SIGNAL(aboutToClose()),this,SLOT(relaunchDisconnected()));
    connect(&sock,SIGNAL(stateChanged(QAbstractSocket::SocketState)),this,SLOT(state(QAbstractSocket::SocketState)));
    connect(&pingTimer,SIGNAL(timeout()),this,SLOT(ping()));
    connect(&pingTimeout,SIGNAL(timeout()),this,SLOT(pingTimeoutError()));
    pingTimeout.setSingleShot(true);
    connect(&reconnectTimer,SIGNAL(timeout()),this,SLOT(tryReconnect()));
    reconnectTimer.setSingleShot(true);
    //connect(jmsgr,SIGNAL(newJms(libJackSMS::dataTypes::logImMessage)),this,SLOT(newMessage(libJackSMS::dataTypes::logImMessage)));

}
void threadInstantMessenger::relaunchDisconnected() {
    emit serviceNotActive(false);
}
void threadInstantMessenger::tryReconnect() {
    activateServ();
}
void threadInstantMessenger::errorDisconnected(QAbstractSocket::SocketError) {
    emit serviceNotActive(false);
}
void threadInstantMessenger::state(QAbstractSocket::SocketState){

}
void threadInstantMessenger::pingTimeoutError(){

    sock.abort();
    pingTimer.stop();
    pingTimeout.stop();
    reconnectTimer.start(3*60*1000);
    emit serviceNotActive(true);
}

QString threadInstantMessenger::getError() const{
    return error;
}
void threadInstantMessenger::stop(){
    sock.write("E\r\n");
    sock.disconnectFromHost();
    pingTimer.stop();
    pingTimeout.stop();
}
void threadInstantMessenger::ping(){
    if (sock.write("\r\n")!=-1)
        pingTimeout.start(10*1000);
    else
        emit serviceNotActive(true);
}
void  threadInstantMessenger::parseLine(){
    QList<QByteArray> finalLines;
    QByteArray a=sock.read(20000);
    if (a.at(0)=='\0')
        a=a.right(a.length()-1);
    a=a.left(a.length()-2);
    QList<QByteArray> lines=a.split('\0');
    for (QList<QByteArray>::iterator i=lines.begin();i!=lines.end();++i){
        QList<QByteArray> ll=i->split('\n');
        finalLines.append(ll);
    }
    QRegExp r;

    for (QList<QByteArray>::iterator i=finalLines.begin();i!=finalLines.end();++i){
        QString line=QString(finalLines.first());
        finalLines.removeFirst();
        if (line.isEmpty()){
            pingTimeout.stop();
        }else if (line.startsWith("W")){
            emit this->serviceActive();
        }else{
            r.setPattern("^R ([^\\t]+) \\(([^)]+)\\)\\t([^\\t]+)$");
            if (r.exactMatch(line)){
                libJackSMS::dataTypes::phoneNumber num;
                num.parse(r.cap(1));
                num.setAltName(r.cap(2));
                libJackSMS::dataTypes::dateTime dat(libJackSMS::dataTypes::dateTime::getCurrentDay(),libJackSMS::dataTypes::dateTime::getCurrentHour());

                libJackSMS::dataTypes::logImMessage im(num,dat,"",QString::fromUtf8(r.cap(3).toAscii(),r.cap(3).length()));
                signalCountdown.stop();
                libJackSMS::localApi::imLogSaver saver(userDir);
                std::cout <<"[JMS] ricevuto jms"<<std::endl;
                saver.setMessage(im);
                saver.appendToLogFile();
                im.setId(saver.getSavedId());
                std::cout <<"[SAVE] salvato im "<<im.getId().toStdString()<<" sul log locale"<<std::endl;


                imLog.insert(im.getId(),im);
                signalCountdown.start(2000);
            }else{
                r.setPattern("^R\\t([^ ]+) \\(([^)]+)\\)\\t([0-9]+)/([0-9]+) ([0-9]+):([0-9]+) - (.+)$");
                if (r.exactMatch(line)){
                    QString a=r.cap(1);
                    QString b=r.cap(2);
                    QString c=r.cap(3);
                    QString d=r.cap(4);
                    QString e=r.cap(5);
                    QString f=r.cap(6);
                    QString g=QString::fromUtf8(r.cap(7).toAscii(),r.cap(7).length());
                    libJackSMS::dataTypes::phoneNumber num;
                    num.parse(a);
                    num.setAltName(b);
                    QDateTime dd(QDateTime::fromString(c+"/"+d+" "+e+":"+f,"dd/MM HH:mm"));
                    bool ok;
                    int current_year=QDateTime::currentDateTime().toString("yyyy").toInt(&ok,10);
                    dd=dd.addYears(current_year-1900);
                    libJackSMS::dataTypes::dateTime dat(dd.toString("dd/MM/yyyy HH:mm:ss"));
                    libJackSMS::dataTypes::logImMessage im(num,dat,"",g);
                    signalCountdown.stop();

                    libJackSMS::localApi::imLogSaver saver(userDir);
                    std::cout <<"[JMS] ricevuto jms"<<std::endl;
                    saver.setMessage(im);
                    saver.appendToLogFile();
                    im.setId(saver.getSavedId());
                    std::cout <<"[SAVE] salvato im "<<im.getId().toStdString()<<" sul log locale"<<std::endl;

                    imLog.insert(im.getId(),im);
                    signalCountdown.start(2000);

                }


            }


        }

    }


}
threadInstantMessenger::~threadInstantMessenger(){
    sock.waitForDisconnected(1000);
}


void threadInstantMessenger::launchSignal(){
    signalCountdown.stop();
    emit newMessages();
}
void threadInstantMessenger::connectDone(){
    QString sn=QString("ID ")+username+QString("\t")+password+QString("\t5551\r\n");
    sock.write(sn.toAscii());
    pingTimer.start(1000*60);
}

void threadInstantMessenger::activateServ(){


    reconnectTimer.stop();
    emit serviceActiving();
    libJackSMS::dataTypes::proxySettings ps(opzioni);
    if (ps.useProxy()){
        proxy.setHostName(ps.getServer());
        bool ok;
        proxy.setPort(ps.getPort().toInt(&ok,10));
        if (ps.getType().toUpper()=="HTTP")
            proxy.setType(QNetworkProxy::HttpProxy);
        else if (ps.getType().toUpper()=="SOCKS5")
            proxy.setType(QNetworkProxy::Socks5Proxy);
        if (ps.useAuth()){
            proxy.setUser(ps.getUsername());
            proxy.setPassword(ps.getPassword());
        }
        sock.setProxy(proxy);

    }

    sock.connectToHost("www.jacksms.it",5551);



}

