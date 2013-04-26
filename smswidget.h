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

#ifndef SMSWIDGET_H
#define SMSWIDGET_H

#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDateTime>
#include <QListWidgetItem>
#include "libjacksms/libJackSMS.h"
#include <QResizeEvent>
#include <qmymessage.h>

class SmsWidget : public QFrame
{
Q_OBJECT

private:
    QString originalText;
    QLabel *labelTxt,*labelGroup,*labelTime,*labelIco,*labelIcoReceived;
    QVBoxLayout *vLayout;
    QHBoxLayout *hLayout;
    QMyMessage msg;
    bool type;
    QString id;
    QString name;
    bool nameFiltered;
    libJackSMS::dataTypes::phoneNumber number;
    bool readed;
    QString parseLinks(QString _s);
    QString parseAts(QString _s);
    QString parseQuotes(QString _s);
    void createBubble(bool isread, bool isreceived);
    const QDateTime dateTim;
    bool caricaAltri;

private slots:
     void openUrl(QString _url);

public:
    SmsWidget(QString _txt,QPixmap _ico,bool received,QDateTime time,QString user,QString service,QString id,libJackSMS::dataTypes::phoneNumber _number,bool _letto=true);
    SmsWidget(QMyMessage _sms,QPixmap _ico,bool received);
    //solo per il "Carica Altri Messaggi"
    SmsWidget(QString _txt);

    bool isCaricaAltri()const;
    QString getId()const;
    bool isReceived()const;
    bool searchMatch(QString _txt);
    bool isReaded()const;
    void setReaded(bool _r);
    QString getText()const;
    QString getName()const;
    void setName(QString newname);
    libJackSMS::dataTypes::phoneNumber getPhoneNum()const;
    //void resizeEvent (QResizeEvent*);
    QSize getSize();
    void setNameFilteredHidden(bool _nf);
    bool isNameFilteredHidden();
    QDateTime getDateTime()const;
};

#endif // SMSWIDGET_H
