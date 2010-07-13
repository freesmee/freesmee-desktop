#include "mainjacksms.h"
#include <QList>
#include "libjacksms/libJackSMS.h"
#ifndef INVIOMULTIPLO_H
#define INVIOMULTIPLO_H

#include <QtGui/QDialog>

namespace Ui {
    class InvioMultiplo;
}

class InvioMultiplo : public QDialog {
    Q_OBJECT
public:
    //InvioMultiplo(QWidget *parent = 0);
    InvioMultiplo( MainJackSMS * _padre,const libJackSMS::dataTypes::configuredServicesType &_ElencoServiziConfigurati,const libJackSMS::dataTypes::phoneBookType &_Rubrica,const libJackSMS::dataTypes::optionsType & _opt,const QString &_smsTxt,QWidget *parent = 0);
    ~InvioMultiplo();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::InvioMultiplo *m_ui;

     MainJackSMS * padre;
    const libJackSMS::dataTypes::configuredServicesType &ElencoServiziConfigurati;
    const libJackSMS::dataTypes::phoneBookType &Rubrica;
    const libJackSMS::dataTypes::optionsType &Opzioni;
    QString testoSms;
    QList <QString> idList;
    QList <QString> nameList;
    QList <QString> phoneList;
    void SendNext();
private slots:
    void on_InvioMultiplo_finished(int result);
    void on_InvioMultiplo_destroyed();
    void on_buttonAnnullaInvio_clicked();
    void on_radiounico_clicked();
    void on_ButtonAddAll_clicked();
    void on_pushButton_3_clicked();
    void on_RicercaVeloce_textChanged(QString );
    void on_lineEdit_textChanged(QString );
    void on_pushButton_clicked();
    void on_comboBox_currentIndexChanged(QString );
    void on_destinatari_itemDoubleClicked(QListWidgetItem* item);
    void on_rubricaVeloce_itemDoubleClicked(QListWidgetItem* item);
    void on_pushButton_2_clicked();
    void readProcessStdOut();
};

#endif // INVIOMULTIPLO_H
