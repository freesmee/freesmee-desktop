#include <QListWidgetItem>
#include <QTextEdit>
#include "libjacksms/libJackSMS.h"
#include "Types.h"
#ifndef OPZIONIDIALOG_H
#define OPZIONIDIALOG_H

#include <QtGui/QDialog>
#include <QCheckBox>
#include <QLineEdit>

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
    void setCheckboxStatusFromYesNoOption(libJackSMS::dataTypes::optionsType &currentWorkingOpt, QCheckBox* checkbox, QString optionName);
    void setTextFromOption(libJackSMS::dataTypes::optionsType &currentWorkingOpt, QLineEdit* lineedit, QString optionName);

private slots:
    void on_okButton_clicked();
    void on_applicaButton_clicked();
    void on_listOpzioni_currentItemChanged(QListWidgetItem* current, QListWidgetItem* previous);
    void on_annullaButton_clicked();

signals:
    void translate();


};

#endif // OPZIONIDIALOG_H
