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

#ifndef MULTIPLECHECKDIALOG_H
#define MULTIPLECHECKDIALOG_H

#include "mainjacksms.h"
#include <QDialog>
#include "libjacksms/libJackSMS.h"
#include "qrecipientwidget.h"

namespace Ui {
    class multipleCheckDialog;
}

class multipleCheckDialog : public QDialog {
    Q_OBJECT
public:
    multipleCheckDialog(const libJackSMS::dataTypes::phoneBookType & _rubrica,const libJackSMS::dataTypes::configuredServicesType & _elencoServiziConfigurati,const libJackSMS::dataTypes::servicesType & _elencoServizi, MainJackSMS *_padre, QWidget *parent = 0);
    ~multipleCheckDialog();

protected:
    void changeEvent(QEvent *e);

private:
    MainJackSMS *padre;
    Ui::multipleCheckDialog *ui;
    const libJackSMS::dataTypes::phoneBookType & rubrica;
    const libJackSMS::dataTypes::configuredServicesType & elencoServiziConfigurati;
    const libJackSMS::dataTypes::servicesType & elencoServizi;
    void writeToGui();

private slots:
    void on_pushButton_2_clicked();
    void on_lineEdit_textEdited(QString );
    void on_pushButton_clicked();
    void on_button_all_clicked();

signals:
    void addRecipients(QList<QRecipientWidget *>);
};

#endif // MULTIPLECHECKDIALOG_H
