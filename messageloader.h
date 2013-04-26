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

#ifndef MESSAGELOADER_H
#define MESSAGELOADER_H

#include <QThread>
#include "mainjacksms.h"

class messageLoader : public QThread
{
    Q_OBJECT
private:
    void run();
    QString ud;
    MainJackSMS* mainjack;
    bool aborted;
    static bool compareMessages(QMyMessage &s1, QMyMessage &s2);
public:
    messageLoader(QString _userDir, MainJackSMS* _mainjack);
    void loadMessages();
    void abort();
signals:
    void messagesLoaded(QList<QMyMessage>);
};

#endif // MESSAGELOADER_H
