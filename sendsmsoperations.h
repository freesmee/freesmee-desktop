#include <QObject>
#include "libjacksms/libJackSMS.h"
#include "threadsendsms.h"
#include <QSemaphore>
#include <QString>
#include <QByteArray>

#ifndef SENDSMSOPERATIONS_H
#define SENDSMSOPERATIONS_H
class sendSmsOperations: public QObject,public libJackSMS::reportOperationSendSms
{
    Q_OBJECT
 private:
    QSemaphore *semaforoCaptcha;
    QSemaphore *semaforoGui;
public:

    sendSmsOperations(QSemaphore *_semaforoCaptcha,QSemaphore *_semaforoGui);
    void doOperation();
    void doOperation(const QString & _text);
    QString doCaptchaOperation(QByteArray & data);
    void doOperationError(const QString & _text);
    void doOperationSuccess(const QString & _text);
signals:
    void signalCaptcha(const QByteArray & data);
    void signalSendOK(const QString & _text);
    void signalSendKO(const QString & _text);
    void signalOperation();
};

#endif // SENDSMSOPERATIONS_H
