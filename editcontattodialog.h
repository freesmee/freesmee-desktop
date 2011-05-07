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

#ifndef EDITCONTATTODIALOG_H
#define EDITCONTATTODIALOG_H

#include "libjacksms/libJackSMS.h"
#include <QtGui/QDialog>
#include <QMovie>

namespace Ui {
    class editcontattodialog;
}

class editcontattodialog : public QDialog {
    Q_OBJECT
public:
    editcontattodialog(QWidget *parent, const libJackSMS::dataTypes::servicesType & _ElencoServizi, const libJackSMS::dataTypes::configuredServicesType &_ElencoServiziConfigurati, libJackSMS::dataTypes::phoneBookType &_Rubrica, QString contactId, const libJackSMS::dataTypes::optionsType _Opzioni, QString _current_login_id);
    ~editcontattodialog();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::editcontattodialog *m_ui;
    const libJackSMS::dataTypes::servicesType &ElencoServizi;
    const libJackSMS::dataTypes::configuredServicesType &ElencoServiziConfigurati;
    libJackSMS::dataTypes::phoneBookType &Rubrica;
    libJackSMS::serverApi::contactManager *saver;
    QMovie *spinner;
    const libJackSMS::dataTypes::optionsType Opzioni;
    QString id;
    QString current_login_id;

private slots:
    void on_salva_clicked();
    void on_annulla_clicked();
    void salvataggioOk(libJackSMS::dataTypes::contact);
    void salvataggioKo();

signals:
    void contactEdited(QString);

};

#endif // EDITCONTATTODIALOG_H
