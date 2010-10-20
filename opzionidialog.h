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
    OpzioniDialog(libJackSMS::dataTypes::optionsType & _opt,QTextEdit &_TextSms,QString _userDirectory,QWidget *parent,const bool _loggedIn);
    ~OpzioniDialog();

protected:
    void changeEvent(QEvent *e);

private:
    void translateGui();
    libJackSMS::dataTypes::optionsType &opt;

    QTextEdit & TextSms;
    QString userDirectory;
    const bool loggedIn;
    Ui::OpzioniDialog *m_ui;
private slots:
    void on_radioMultiplo_clicked();
    void on_radioSingolo_clicked();
    void on_listWidget_itemClicked(QListWidgetItem* item);
    void on_pushButton_3_clicked();
    void on_pushButton_2_clicked();
    void on_listWidget_currentItemChanged(QListWidgetItem* current, QListWidgetItem* previous);
    void on_pushButton_clicked();
signals:
    void activateImTimer();
    void deactivateImTimer();
    void reactivateImTimer();
    void translate();

};

#endif // OPZIONIDIALOG_H
