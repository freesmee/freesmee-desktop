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

#ifndef IMPORTGMAILDIALOG_H
#define IMPORTGMAILDIALOG_H

#include <QDialog>
#include "libjacksms/libJackSMS.h"
#include <QMovie>

namespace Ui {
    class importGmailDialog;
}

class importGmailDialog : public QDialog {
    Q_OBJECT

public:
    importGmailDialog(QString _lid,const libJackSMS::dataTypes::optionsType &o,libJackSMS::dataTypes::phoneBookType &_phoneBook,QWidget *parent = 0);
    ~importGmailDialog();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::importGmailDialog *ui;
    QString logid;
    const libJackSMS::dataTypes::optionsType &opt;
    libJackSMS::serverApi::gmailAddressBookImporter *importer;
    libJackSMS::serverApi::reloader *reloader;
    libJackSMS::dataTypes::phoneBookType &phoneBook;
    QMovie* spinner;

private slots:
    void on_importButton_clicked();
    void on_pushButton_clicked();
    void importDone(int);
    void importError(QString e);
    void wrongCredentials();
    void phoneBookReloaded(libJackSMS::dataTypes::phoneBookType);
    void errorReload();

signals:
    void rewritePhoneBook();

};

#endif // IMPORTGMAILDIALOG_H
