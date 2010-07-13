#include "sendsmsoperations.h"
#include "libjacksms/libJackSMS.h"
#include "captchadialog.h"
#include "mainjacksms.h"
#include "threadsendsms.h"
#include <QObject>
#include <QSemaphore>
#include <QSharedMemory>
#include <iostream>

sendSmsOperations::sendSmsOperations(QSemaphore *_semaforoCaptcha,QSemaphore *_semaforoGui):
        semaforoCaptcha(_semaforoCaptcha),
        semaforoGui(_semaforoGui)
{
}


void sendSmsOperations::doOperation(){
    emit this->signalOperation();
}
void sendSmsOperations::doOperation(const QString & _text){
    emit this->signalOperation();
}
QString sendSmsOperations::doCaptchaOperation(QByteArray & data){
    //libJackSMS::netClient::MemoryStruct *captchaBytes=(libJackSMS::netClient::MemoryStruct*) data;

    /*QSharedMemory memoriaCaptcha("jacksmsm_captcha_shmem");
    if (!memoriaCaptcha.create(data.size())){

            throw;
    }*/
    QSharedMemory memoriaRisultatoCaptcha("jacksmsm_result_captcha_shmem");
    if (!memoriaRisultatoCaptcha.create(data.size())){

            throw;
    }
/*
    memoriaCaptcha.lock();
    memcpy(memoriaCaptcha.data(),captchaBytes->memory,captchaBytes->size);
    memoriaCaptcha.unlock();*/

    emit signalCaptcha(data);
    semaforoCaptcha->acquire(1);
    //memoriaCaptcha.detach();
    memoriaRisultatoCaptcha.lock();
    QString risultato=QString((const char*)memoriaRisultatoCaptcha.constData());
    memoriaRisultatoCaptcha.unlock();
    memoriaRisultatoCaptcha.detach();
    return risultato;
}
void sendSmsOperations::doOperationError(const QString & _text){
    /*QSharedMemory memoriaCaptcha("jacksmsm_error_shmem");
    if (!memoriaCaptcha.create(_text.length())){
            throw;
    }
    memoriaCaptcha.lock();
    memcpy(memoriaCaptcha.data(),_text.c_str(),_text.length());
    memoriaCaptcha.unlock();
    emit this->;
    semaforoGui->acquire(1);*/
    emit signalSendKO(_text);

}
void sendSmsOperations::doOperationSuccess(const QString & _text){

    /*if (_text.length()>0){
        QSharedMemory memoriaCaptcha("jacksms_success_shmem");
        if (!memoriaCaptcha.create(_text.length())){
            QString error=memoriaCaptcha.errorString();
            throw;
        }
        memoriaCaptcha.lock();
        memcpy(memoriaCaptcha.data(),_text.toStdString().c_str(),_text.length());
        memoriaCaptcha.unlock();
        emit this->signalSendOK();
        semaforoGui->acquire(1);
    }else{
        emit this->signalSendOK();
        semaforoGui->acquire(1);
    }*/

    emit signalSendOK(_text);
}

