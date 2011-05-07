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

#ifndef SERVICESDIALOG_H
#define SERVICESDIALOG_H

#include <QtGui/QDialog>
#include <QListWidgetItem>
#include <QProcess>
#include "mainjacksms.h"
#include "Types.h"
#include "libjacksms/libJackSMS.h"
#include <QMovie>

namespace Ui {
    class ServicesDialog;
}

class ServicesDialog : public QDialog {
    Q_OBJECT
public:
    ServicesDialog(QWidget *parent,MainJackSMS * _padre,libJackSMS::dataTypes::servicesType &_ElencoServizi,libJackSMS::dataTypes::configuredServicesType &_ElencoServiziConfigurati,const libJackSMS::dataTypes::optionsType &_opzioni);
    ~ServicesDialog();

protected:
    void changeEvent(QEvent *e);

private:
    MainJackSMS *padre;
    Ui::ServicesDialog *m_ui;
    libJackSMS::dataTypes::servicesType &ElencoServizi;
    libJackSMS::dataTypes::configuredServicesType &ElencoServiziConfigurati;
    QString currentId;

    QMovie *spinner;
    libJackSMS::serverApi::accountManager *saver;
    libJackSMS::dataTypes::configuredAccount newAcc;
    const libJackSMS::dataTypes::optionsType &opzioni;
private slots:

    void on_Salva_clicked();
    void on_ListServizi_currentItemChanged(QListWidgetItem* current, QListWidgetItem* previous);
    void addAccountKo();
    void addAccountOk(QString);
signals:
    void rewriteAccounts();
};

#endif // SERVICESDIALOG_H
