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

#ifndef RECIPIENTCOMPLETER_H
#define RECIPIENTCOMPLETER_H

#include <QCompleter>

class RecipientCompleter : public QCompleter
{
    Q_OBJECT
public:
    explicit RecipientCompleter(QObject *parent = 0);
    RecipientCompleter(const QStringList& completions, QObject *parent = 0);

signals:

public slots:

};

#endif // RECIPIENTCOMPLETER_H
