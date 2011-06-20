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

#include <QImage>
#include <QPixmap>
#include <QMessageBox>
#include <QFile>
#include <QSharedMemory>
#include "captchadialog.h"
#include "ui_captchadialog.h"
//#include "JackUtils.h"

CaptchaDialog::CaptchaDialog(const QByteArray &_imgData, QString zoomFactor, QWidget *parent) :
    QDialog(parent, Qt::WindowStaysOnTopHint),
    m_ui(new Ui::CaptchaDialog)
{
    Q_UNUSED(zoomFactor);

    m_ui->setupUi(this);

    QImage im = QImage::fromData(_imgData, "png");
    bool errorImage = false;
    if (im.isNull())
    {
        im = QImage::fromData(_imgData, "jpg");
        if (im.isNull())
        {
            im = QImage::fromData(_imgData, "gif");
            if (im.isNull())
                errorImage = true;
        }
    }

    if(errorImage)
    {
        throw QString("captcha");
    }
    else
    {
        m_ui->captchalabel->setPixmap(QPixmap::fromImage(im));
        original = im;
        dim = im.size();
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
    result = m_ui->TextCaptcha->text();
    if (result.isEmpty())
           result="captcha_non_decodificato";
/*
   QString toWrite=m_ui->TextCaptcha->text();
   if (toWrite.isEmpty())
       toWrite="captcha_non_decodificato";
   QSharedMemory memoriaCaptcha("jacksms_result_captcha_shmem");
   memoriaCaptcha.attach();
   memoriaCaptcha.lock();
   memcpy(memoriaCaptcha.data(),toWrite.toStdString().c_str(),toWrite.length());
   memoriaCaptcha.unlock();
*/
   close();
}

void CaptchaDialog::on_zoomIn_clicked()
{
    QImage im = original;

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
