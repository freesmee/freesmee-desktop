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

#ifndef AGGIUNGICONTATTO_H
#define AGGIUNGICONTATTO_H

#include "libjacksms/libJackSMS.h"
#include "Types.h"
#include <QMovie>
#include <QtGui/QDialog>

namespace Ui
{
class AggiungiContatto;
}

class AggiungiContatto : public QDialog
{
    Q_OBJECT
public:
    AggiungiContatto(QWidget *parent, libJackSMS::dataTypes::configuredServicesType &_ElencoServiziConfigurati, libJackSMS::dataTypes::phoneBookType &_Rubrica, libJackSMS::dataTypes::servicesType &_ElencoServizi, QString _current_login_id);
    AggiungiContatto(QWidget *parent, libJackSMS::dataTypes::configuredServicesType &_ElencoServiziConfigurati, libJackSMS::dataTypes::phoneBookType &_Rubrica, libJackSMS::dataTypes::servicesType &_ElencoServizi, QString _current_login_id, libJackSMS::dataTypes::phoneNumber numero);
    ~AggiungiContatto();

protected:
    void changeEvent(QEvent *e);

private:
    libJackSMS::serverApi::contactManager *saver;
    Ui::AggiungiContatto *m_ui;
    libJackSMS::dataTypes::configuredServicesType &ElencoServiziConfigurati;
    libJackSMS::dataTypes::phoneBookType &Rubrica;
    libJackSMS::dataTypes::servicesType &ElencoServizi;
    QMovie *spinner;
    QString current_login_id;
    libJackSMS::dataTypes::contact contatto;

private slots:
    void on_salva_clicked();
    void on_annulla_clicked();
    void salvataggioOk(QString, bool, int);
    void salvataggioKo();

signals:
    void contactAdded(QString);
};

#endif // AGGIUNGICONTATTO_H
