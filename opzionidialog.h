#include <QListWidgetItem>
#include <QTextEdit>
#include "libjacksms/libJackSMS.h"
#include "Types.h"
#ifndef OPZIONIDIALOG_H
#define OPZIONIDIALOG_H

#include <QtGui/QDialog>

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

private slots:
    void on_pushButton_3_clicked();
    void on_pushButton_2_clicked();
    void on_listWidget_currentItemChanged(QListWidgetItem* current, QListWidgetItem* previous);
    void on_pushButton_clicked();

signals:
    void translate();


};

#endif // OPZIONIDIALOG_H
