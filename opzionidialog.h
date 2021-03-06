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

#ifndef OPZIONIDIALOG_H
#define OPZIONIDIALOG_H

#include <QListWidgetItem>
#include <QTextEdit>
#include "libjacksms/libJackSMS.h"
#include "Types.h"
#include <QtGui/QDialog>
#include <QCheckBox>
#include <QLineEdit>
#include <QComboBox>

namespace Ui {
    class OpzioniDialog;
}

class OpzioniDialog : public QDialog {
    Q_OBJECT

public:
    OpzioniDialog(libJackSMS::dataTypes::optionsType &_opt, libJackSMS::dataTypes::optionsType &_globopt, QString _userDirectory, QWidget *parent, const bool _loggedIn, QString _pass);
    ~OpzioniDialog();

protected:
    void changeEvent(QEvent *e);

private:
    void translateGui();
    libJackSMS::dataTypes::optionsType &opt;
    libJackSMS::dataTypes::optionsType &globopt;
    QString userDirectory;
    const bool loggedIn;
    QString pass;
    Ui::OpzioniDialog *m_ui;
    void setCheckboxStatusFromYesNoOption(libJackSMS::dataTypes::optionsType &Options, QCheckBox* checkbox, QString optionName);
    void setTextFromOption(libJackSMS::dataTypes::optionsType &Options, QLineEdit* lineedit, QString optionName);
    void setComboBoxStatusFromOption(libJackSMS::dataTypes::optionsType &Options, QComboBox* combobox, QString optionName);
    void enableFreePlusFields(bool enable);
    void enableProxyFields(bool enable);

private slots:
    void on_okButton_clicked();
    void on_listOpzioni_currentItemChanged(QListWidgetItem* current, QListWidgetItem* previous);
    void on_annullaButton_clicked();
    void on_enableJmsService_stateChanged(int state);
    void on_CheckUsaProxy_stateChanged(int state);

signals:
    void translate();
    void updateProxy();
    void UpdateGuiFromOptions();

};

#endif // OPZIONIDIALOG_H
