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

#include "importgmaildialog.h"
#include "ui_importgmaildialog.h"

importGmailDialog::importGmailDialog(QString _lid,const libJackSMS::dataTypes::optionsType &o,libJackSMS::dataTypes::phoneBookType &_phoneBook,QWidget *parent) :
        QDialog(parent),
        ui(new Ui::importGmailDialog),
        logid(_lid),
        opt(o),
        phoneBook(_phoneBook)
{
    ui->setupUi(this);
    ui->labelImport->setPixmap(QPixmap(":/resource/ball-grey.png","png"));
    ui->labelUpdateBook->setPixmap(QPixmap(":/resource/ball-grey.png","png"));
    spinner=new QMovie(":/resource/loading-spinner.gif",QByteArray("gif"),this);
    spinner->setScaledSize(QSize(16,16));
    spinner->start();
}

importGmailDialog::~importGmailDialog()
{
    delete ui;
}

void importGmailDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void importGmailDialog::on_pushButton_clicked()
{
    close();
}
void importGmailDialog::phoneBookReloaded(libJackSMS::dataTypes::phoneBookType _r){
    phoneBook=_r;
     ui->labelUpdateBook->setPixmap(QPixmap(":/resource/jms-active.png","png"));
     emit rewritePhoneBook();
}
void importGmailDialog::errorReload(){
    //ui->labelUpdateBook->setText(e);
    ui->labelUpdateBook->setPixmap(QPixmap(":/resource/jms-not-active.png","png"));
}

void importGmailDialog::importDone(int count){
    ui->labelCount->setText(QString::number(count)+" contatti importati");
    ui->labelImport->setPixmap(QPixmap(":/resource/jms-active.png","png"));
    reloader = new libJackSMS::serverApi::reloader(logid, opt);
    connect(reloader,SIGNAL(phoneBookReloaded(libJackSMS::dataTypes::phoneBookType)),this,SLOT(phoneBookReloaded(libJackSMS::dataTypes::phoneBookType)));
    connect(reloader,SIGNAL(errorReload()),this,SLOT(errorReload()));
    ui->labelUpdateBook->setMovie(spinner);
    reloader->reloadPhoneBook();
}

void importGmailDialog::importError(QString e){
    ui->labelCount->setText(e);
    ui->labelImport->setPixmap(QPixmap(":/resource/jms-not-active.png","png"));
}
void importGmailDialog::wrongCredentials(){
    ui->labelCount->setText("Username o password errate");
    ui->labelImport->setPixmap(QPixmap(":/resource/jms-not-active.png","png"));
}
void importGmailDialog::on_importButton_clicked()
{
    ui->labelImport->setPixmap(QPixmap(":/resource/ball-grey.png","png"));
    ui->labelUpdateBook->setPixmap(QPixmap(":/resource/ball-grey.png","png"));
    ui->labelCount->setText("");
    importer = new libJackSMS::serverApi::gmailAddressBookImporter(logid, opt);

    connect(importer,SIGNAL(importDone(int)),this,SLOT(importDone(int)));
    connect(importer,SIGNAL(importError(QString)),this,SLOT(importError(QString)));
    connect(importer,SIGNAL(wrongCredentials()),this,SLOT(wrongCredentials()));
    ui->labelImport->setMovie(spinner);
    importer->import(ui->user->text(), ui->password->text());
}
