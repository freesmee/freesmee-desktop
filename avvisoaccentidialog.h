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

#ifndef AVVISOACCENTIDIALOG_H
#define AVVISOACCENTIDIALOG_H

#include <QtGui/QDialog>

namespace Ui
{
    class avvisoaccentidialog;
}

class avvisoaccentidialog : public QDialog {
    Q_OBJECT
public:
    avvisoaccentidialog(QString & scelta,QWidget *parent = 0);
    ~avvisoaccentidialog();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::avvisoaccentidialog *ui;
    QString &sc;
private slots:
    void on_saveButton_clicked();
    void on_radioButton_2_clicked();
    void on_radioButton_clicked();
    void on_pushButton_2_clicked();
};

#endif // AVVISOACCENTIDIALOG_H
