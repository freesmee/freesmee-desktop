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

#ifndef CAMBIAACCOUNT_H
#define CAMBIAACCOUNT_H

#include <QDialog>
#include <libjacksms/libJackSMS.h>
#include "mainjacksms.h"

namespace Ui
{
    class cambiaaccount;
}

class cambiaaccount : public QDialog
{
    Q_OBJECT

public:
    cambiaaccount(QWidget *parent, MainJackSMS * _padre, const libJackSMS::dataTypes::servicesType & _ElencoServizi, const libJackSMS::dataTypes::configuredServicesType &_ElencoServiziConfigurati, libJackSMS::dataTypes::phoneBookType &_Rubrica, QString _id, int _found);
    ~cambiaaccount();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::cambiaaccount *m_ui;

    MainJackSMS * padre;
    const libJackSMS::dataTypes::servicesType & ElencoServizi;
    const libJackSMS::dataTypes::configuredServicesType &ElencoServiziConfigurati;
    libJackSMS::dataTypes::phoneBookType &Rubrica;
    libJackSMS::serverApi::contactManager *saver;
    QMovie *spinner;
    QString id;
    int found;
    int changed;
    bool stopsearch;

private slots:
    void on_buttonAnnulla_clicked();
    void on_buttonSalva_clicked();
    void salvataggioKo();
    void salvataggioOk(libJackSMS::dataTypes::contact c);
};

#endif // CAMBIAACCOUNT_H
