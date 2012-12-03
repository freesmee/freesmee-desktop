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

#ifndef CONTACTWIDGETFASTBOOK_H
#define CONTACTWIDGETFASTBOOK_H

#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>
#include "libjacksms/libJackSMS.h"
#include <QCheckBox>

class contactWidgetFastBook : public QWidget
{
private:
    QLabel *nome,*icon;
    QHBoxLayout *hLayout;
    QString id;
    QString nomeContatto;
    QString accountId;
    QString numero;
    libJackSMS::dataTypes::contact c;
    QLabel *labelIconInfo;
    QCheckBox *chbox;

public:
    contactWidgetFastBook(const libJackSMS::dataTypes::contact &_c, QPixmap _icoServ, bool addCheckBox = false);
    QString getName() const;
    QString getContactId() const;
    QString getAccountId() const;
    contactWidgetFastBook *clone() const;
    libJackSMS::dataTypes::contact getContact() const;
    QPixmap getIcon() const;
    void setIcon(QPixmap _pix);
    void showIcon(bool value);
    bool isChecked() const;
    void setChecked(bool value);
    bool searchMatch(QString _txt) const;
};

#endif // CONTACTWIDGETFASTBOOK_H
