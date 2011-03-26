#include "mainjacksms.h"

#ifndef MULTIPLECHECKDIALOG_H
#define MULTIPLECHECKDIALOG_H

#include <QDialog>
#include "libjacksms/libJackSMS.h"
#include "qrecipientwidget.h"
namespace Ui {
    class multipleCheckDialog;
}

class multipleCheckDialog : public QDialog {
    Q_OBJECT
public:
    multipleCheckDialog(const libJackSMS::dataTypes::phoneBookType & _rubrica,const libJackSMS::dataTypes::configuredServicesType & _elencoServiziConfigurati,const libJackSMS::dataTypes::servicesType & _elencoServizi, MainJackSMS *_padre, QWidget *parent = 0);
    ~multipleCheckDialog();

protected:
    void changeEvent(QEvent *e);

private:
    MainJackSMS *padre;
    Ui::multipleCheckDialog *ui;
    const libJackSMS::dataTypes::phoneBookType & rubrica;
    const libJackSMS::dataTypes::configuredServicesType & elencoServiziConfigurati;
    const libJackSMS::dataTypes::servicesType & elencoServizi;
    void writeToGui();

private slots:
    void on_pushButton_2_clicked();
    void on_lineEdit_textEdited(QString );
    void on_pushButton_clicked();
signals:
    void addRecipients(QList<QRecipientWidget *>);
};

#endif // MULTIPLECHECKDIALOG_H
