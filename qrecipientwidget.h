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

#ifndef QRECIPIENTWIDGET_H
#define QRECIPIENTWIDGET_H

#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QString>
#include "qlabelresult.h"
#include <QListWidgetItem>
#include "libjacksms/libJackSMS.h"

class QRecipientWidget : public QWidget
{
    Q_OBJECT

private:
    QHBoxLayout *hLayout;
    QLabel *name;
    QFrame *f1;
    QLabel *iconStatus;
    QLabel *iconService;
    QString contactName;
    QString accountId;
    libJackSMS::dataTypes::phoneNumber number;
    QLabelResult *removeLabel;
    QListWidgetItem *pIt;
    QLabel *iconLabel;
    QPixmap original;
    QString nameStr;

public:
    void setStatusFailed(QString message);
    void setStatusWorking();
    void setStatusSuccess(QString message);
    QRecipientWidget(const QString & contactName,const QString & accId,const libJackSMS::dataTypes::phoneNumber &number,QPixmap icon,QWidget *parent = 0);
    libJackSMS::dataTypes::phoneNumber getPhone() const;
    QString getAccountId() const;
    void setParentItem(QListWidgetItem * it);
    void setIcon(QPixmap Icon);
    void restoreOriginalIcon();
    QString getName() const;
    void clearStatus();

signals:
    void removed(QListWidgetItem*);

private slots:
    void remove();

};

#endif // QRECIPIENTWIDGET_H
