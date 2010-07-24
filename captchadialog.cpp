#include <QImage>
#include <QPixmap>
#include <QMessageBox>
#include <QFile>
#include <QSharedMemory>
#include "captchadialog.h"
#include "ui_captchadialog.h"
//#include "JackUtils.h"




CaptchaDialog::CaptchaDialog(const QByteArray &_imgData,QString zoomFactor,QWidget *parent ):
    QDialog(parent),
    m_ui(new Ui::CaptchaDialog)
{
    m_ui->setupUi(this);

    QImage im=QImage::fromData(_imgData,"png");
    bool errorImage=false;
    if (im.isNull()){
        im=QImage::fromData(_imgData,"jpg");
        if (im.isNull()){
            im=QImage::fromData(_imgData,"gif");
            if (im.isNull()){
                errorImage=true;
            }
        }
    }
    if(errorImage){

        /*QSharedMemory sharedMemory("jacksms_result_captcha_shmem");
        sharedMemory.create(1024);
        sharedMemory.lock();
        char *to = (char*)sharedMemory.data();
        const char *text = "captcha_non_decodificato";
        memcpy(to, text, strlen(text)+1);
        sharedMemory.unlock();
*/
        QSharedMemory sharedMemory("jacksmsm_result_captcha_shmem");
        sharedMemory.attach();
        sharedMemory.lock();
        char *to = (char*)sharedMemory.data();
        memcpy(to, "captcha_non_decodificato", 24);
        sharedMemory.unlock();
        sharedMemory.detach();

        throw QString("captcha");
    }else{
        m_ui->captchalabel->setPixmap(QPixmap::fromImage(im));
        original=im;
        dim=im.size();
    }
}


CaptchaDialog::~CaptchaDialog()
{
    delete m_ui;
}

void CaptchaDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}



void CaptchaDialog::on_pushButton_clicked()
{

   QString toWrite=m_ui->TextCaptcha->text();
   if (toWrite.isEmpty())
       toWrite="captcha_non_decodificato";
   QSharedMemory memoriaCaptcha("jacksmsm_result_captcha_shmem");
   memoriaCaptcha.attach();
   memoriaCaptcha.lock();
   memcpy(memoriaCaptcha.data(),toWrite.toStdString().c_str(),toWrite.length());
   memoriaCaptcha.unlock();
   this->close();

}

void CaptchaDialog::on_zoomIn_clicked()
{
    QImage im=original;

    dim.setHeight(dim.height()+15);
    dim.setWidth(dim.width()+15);
    im=im.scaled(dim,Qt::KeepAspectRatio);
    m_ui->captchalabel->setPixmap(QPixmap::fromImage(im));


}

void CaptchaDialog::on_zoomOut_clicked()
{
    QImage im=original;

    dim.setHeight(dim.height()-15);
    dim.setWidth(dim.width()-15);
    im=im.scaled(dim,Qt::KeepAspectRatio);
    m_ui->captchalabel->setPixmap(QPixmap::fromImage(im));

}
