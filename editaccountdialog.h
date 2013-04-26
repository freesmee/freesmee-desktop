/*
    Copyright (C) <2011>

    <enrico bacis> <enrico.bacis@gmail.com>
    <ivan vaccari> <grisson@jacksms.it>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef EDITACCOUNTDIALOG_H
#define EDITACCOUNTDIALOG_H

#include <QDialog>
#include <QMovie>
#include <libjacksms/libJackSMS.h>

namespace Ui
{
    class editAccountDialog;
}

class editAccountDialog : public QDialog {
    Q_OBJECT
public:
    editAccountDialog(libJackSMS::dataTypes::configuredServicesType &_acc, libJackSMS::dataTypes::servicesType &_services, QString id, QString current_login_id, QWidget *parent = 0);
    ~editAccountDialog();

protected:
    void changeEvent(QEvent *e);

private:
    QMovie *spinner;
    libJackSMS::dataTypes::configuredServicesType &accounts;
    libJackSMS::dataTypes::servicesType &services;
    QString id;
    QString current_login_id;
    libJackSMS::dataTypes::configuredAccount currentAccount;
    Ui::editAccountDialog *ui;
    libJackSMS::serverApi::accountManager *saver;

private slots:
    void on_pushButton_clicked();
    void on_buttonAnnulla_clicked();
    void updateOk(libJackSMS::dataTypes::configuredAccount);
    void accountNotUpdated();

signals:
    void rewriteAccunts();
};

#endif // EDITACCOUNTDIALOG_H
