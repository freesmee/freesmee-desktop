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

#include "avvisoaccentidialog.h"
#include "ui_avvisoaccentidialog.h"

avvisoaccentidialog::avvisoaccentidialog(QString & scelta,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::avvisoaccentidialog),
    sc(scelta)


{
    ui->setupUi(this);
}

avvisoaccentidialog::~avvisoaccentidialog()
{
    delete ui;
}

void avvisoaccentidialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void avvisoaccentidialog::on_pushButton_2_clicked()
{
    sc="-1";
    close();
}

void avvisoaccentidialog::on_radioButton_clicked()
{
   sc="1";
}

void avvisoaccentidialog::on_radioButton_2_clicked()
{
    sc="2";
}

void avvisoaccentidialog::on_saveButton_clicked()
{
    close();
}
