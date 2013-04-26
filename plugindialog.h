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

#ifndef PLUGINDIALOG_H
#define PLUGINDIALOG_H

#include <QtGui/QDialog>
#include "plugins/JackPluginInterfacer.h"

namespace Ui {
    class pluginDialog;
}

class pluginDialog : public QDialog {
    Q_OBJECT
public:
    pluginDialog(QList<JackPluginInterface*> &_pluginsList,QWidget *parent = 0);
    ~pluginDialog();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::pluginDialog *m_ui;
    QList<JackPluginInterface*> &pluginsList;

private slots:
    void setPluginInfo();
};

#endif // PLUGINDIALOG_H
