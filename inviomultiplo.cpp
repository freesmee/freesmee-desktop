
#include "inviomultiplo.h"
#include "ui_inviomultiplo.h"
#include "mainjacksms.h"
#include "Types.h"
#include "JackUtils.h"
#include "Configuration.h"

#include "captchadialog.h"


InvioMultiplo::InvioMultiplo(MainJackSMS * _padre,const libJackSMS::dataTypes::configuredServicesType &_ElencoServiziConfigurati,const libJackSMS::dataTypes::phoneBookType &_Rubrica,const libJackSMS::dataTypes::optionsType & _opt,const QString &_smsTxt,QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::InvioMultiplo),
    padre(_padre),
    ElencoServiziConfigurati(_ElencoServiziConfigurati),
    Rubrica(_Rubrica),
    Opzioni(_opt),
    testoSms(_smsTxt)
{
    m_ui->setupUi(this);
    /*m_ui->buttonAnnullaInvio->setVisible(false);
    m_ui->tabellaEsito->setColumnWidth(0,120);
    m_ui->tabellaEsito->setColumnWidth(1,200);
    m_ui->tabellaEsito->setColumnWidth(2,140);

    {
        jackTypes::ConfiguredServicesMap::const_iterator i=ElencoServiziConfigurati.begin();
        jackTypes::ConfiguredServicesMap::const_iterator i_end=ElencoServiziConfigurati.end();
        for(;i!=i_end;++i){
            jackTypes::IconsMap::const_iterator x=Icone.find(i->idServizio);
            QIcon ico;
            if (x!=Icone.end())
                ico=x.value();
            m_ui->comboServizio->addItem(ico,i->nome);
        }
    }

    {
        m_ui->comboBox->addItem("");
        jackTypes::AbookMap::const_iterator i=Rubrica.begin();
        jackTypes::AbookMap::const_iterator i_end=Rubrica.end();

        for(;i!=i_end;++i){

            QIcon ico;

            QListWidgetItem *it=new QListWidgetItem();
            it->setText(i.key());
            QString idServizio;
            {
                jackTypes::ConfiguredServicesMap::const_iterator x=ElencoServiziConfigurati.find(i->servizioAssociato);
                if (x==ElencoServiziConfigurati.end())
                    idServizio="0";
                else
                    idServizio=x->idServizio;
            }
            {
                jackTypes::IconsMap::const_iterator x=Icone.find(idServizio);
                if (x!=Icone.end())
                    ico=x.value();
            }
            it->setIcon(ico);
            m_ui->rubricaVeloce->addItem(it);
            if (m_ui->comboBox->findText(i->gruppo)==-1)
                m_ui->comboBox->addItem(i->gruppo);

        }
    }*/
}

InvioMultiplo::~InvioMultiplo()
{

    delete m_ui;
}


void InvioMultiplo::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void InvioMultiplo::on_pushButton_2_clicked()
{
   this->close();
}

void InvioMultiplo::on_rubricaVeloce_itemDoubleClicked(QListWidgetItem* item)
{
    QListWidgetItem *it=new QListWidgetItem(*item);
    m_ui->destinatari->addItem(it);
}

void InvioMultiplo::on_destinatari_itemDoubleClicked(QListWidgetItem* item)
{
    //m_ui->destinatari->removeItemWidget(item);
    item->~QListWidgetItem();

}

void InvioMultiplo::on_comboBox_currentIndexChanged(QString )
{
   /* m_ui->rubricaVeloce->clear();
    jackTypes::AbookMap::const_iterator i=Rubrica.begin();
    jackTypes::AbookMap::const_iterator i_end=Rubrica.end();
    for(;i!=i_end;++i){
        if (i->gruppo==m_ui->comboBox->currentText()){
            QListWidgetItem *it=new QListWidgetItem();
            QIcon ico;
            it->setText(i.key());
            QString idServizio;
            {
                jackTypes::ConfiguredServicesMap::const_iterator x=ElencoServiziConfigurati.find(i->servizioAssociato);
                idServizio=x.value().idServizio;
            }
            {
                jackTypes::IconsMap::const_iterator x=Icone.find(idServizio);
                if (x!=Icone.end())
                    ico=x.value();
            }
            it->setIcon(ico);
            m_ui->rubricaVeloce->addItem(it);

        }
    }*/

}

void InvioMultiplo::on_pushButton_clicked()
{

    /*m_ui->tabellaEsito->clearContents();
    m_ui->tabellaEsito->setRowCount(0);
    sendProcess = new QProcess( this );
    //sendProcess->deleteLater();
    QStringList arguments;
    QString testo=testoSms.replace("\n",". ");
    arguments <<"-sms64"<<jackUtils::Base64Encode(testo.toUtf8());
    QString idServ;


    bool almenoUnDestinatario=false;
    if (m_ui->radiounico->isChecked()){

        jackTypes::ConfiguredServicesMap::const_iterator i=ElencoServiziConfigurati.begin();
        jackTypes::ConfiguredServicesMap::const_iterator i_end=ElencoServiziConfigurati.end();

        for(;i!=i_end;++i){
            if (i->nome==m_ui->comboServizio->currentText()){
                idServ=i.key();
                break;

            }
        }

        for(int x=0;x<m_ui->destinatari->count();++x){
            QString nome=m_ui->destinatari->item(x)->text();
            phoneList<<Rubrica[nome].numero;
            nameList <<nome;
            idList<<idServ;
            almenoUnDestinatario=true;
        }





    }else{

        for(int x=0;x<m_ui->destinatari->count();++x){
            QString nome=m_ui->destinatari->item(x)->text();
            phoneList<<Rubrica[nome].numero;
            idList<<Rubrica[nome].servizioAssociato;
            nameList <<nome;
            almenoUnDestinatario=true;
        }


    }
    if (almenoUnDestinatario){


        arguments <<"-dest"<<phoneList.first();
        arguments <<"-serv"<<idList.first();

        if (Opzioni["save-local"]=="yes" && Opzioni["save-online"]=="yes")
            arguments<<"-save"<<"onlineandlocal";
        else if (Opzioni["save-local"]!="yes" && Opzioni["save-online"]=="yes")
            arguments<<"-save"<<"online";
        else if (Opzioni["save-local"]=="yes" && Opzioni["save-online"]!="yes")
            arguments<<"-save"<<"local";
        else
            arguments<<"-save"<<"no";


        padre->LastSms.second.destinatario=phoneList.first();
        padre->LastSms.second.testo=testo;
        padre->LastSms.second.nomeAccount=ElencoServiziConfigurati[idList.first()].nome;
        QDate oggi=QDate::currentDate();
        QTime adesso=QTime::currentTime();
        padre->LastSms.second.data=oggi.toString("dd/MM/yyyy");
        padre->LastSms.second.ora=adesso.toString("H:m");
        padre->LastSms.second.idServizio=ElencoServiziConfigurati[idList.first()].idServizio;
        padre->LastSms.second.idAccount=idList.first();



        connect(sendProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(readProcessStdOut()));
        sendProcess->start(JACKSMS_BINARY_FILENAME,arguments);

        int index=m_ui->tabellaEsito->rowCount();
        m_ui->tabellaEsito->insertRow(index);
        {
            QTableWidgetItem *it = new QTableWidgetItem();
            it->setText(nameList.first());
            it->setIcon(Icone[ElencoServiziConfigurati[idList.first()].idServizio]);
            m_ui->tabellaEsito->setItem(index,0,it);
            it = new QTableWidgetItem();
            it->setText("Invio in corso");
            m_ui->tabellaEsito->setItem(index,1,it);
        }
        m_ui->pushButton->setVisible(false);
        m_ui->buttonAnnullaInvio->setVisible(true);

        phoneList.removeFirst();
        idList.removeFirst();
        nameList.removeFirst();

    }else{
        QMessageBox::critical(this, "JackSMS","Devi selezionare almeno un destinatario.");
    }
    */
}

void  InvioMultiplo::SendNext(){
    /*//sendProcess->~QProcess();
    if (!phoneList.isEmpty()){

        sendProcess = new QProcess( this );
        //sendProcess->deleteLater();
        QStringList arguments;
        QString testo=testoSms.replace("\n",". ");
        arguments <<"-sms64"<<jackUtils::Base64Encode(testo.toUtf8());
        arguments <<"-dest"<<phoneList.first();
        arguments <<"-serv"<<idList.first();

        if (Opzioni["save-local"]=="yes" && Opzioni["save-online"]=="yes")
            arguments<<"-save"<<"onlineandlocal";
        else if (Opzioni["save-local"]!="yes" && Opzioni["save-online"]=="yes")
            arguments<<"-save"<<"online";
        else if (Opzioni["save-local"]=="yes" && Opzioni["save-online"]!="yes")
            arguments<<"-save"<<"local";
        else
            arguments<<"-save"<<"no";

        padre->LastSms.second.destinatario=phoneList.first();
        padre->LastSms.second.testo=testo;
        padre->LastSms.second.nomeAccount=ElencoServiziConfigurati[idList.first()].nome;
        QDate oggi=QDate::currentDate();
        QTime adesso=QTime::currentTime();
        padre->LastSms.second.data=oggi.toString("dd/MM/yyyy");
        padre->LastSms.second.ora=adesso.toString("H:m");
        padre->LastSms.second.idServizio=ElencoServiziConfigurati[idList.first()].idServizio;
        padre->LastSms.second.idAccount=idList.first();



        connect(sendProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(readProcessStdOut()));
        sendProcess->start(JACKSMS_BINARY_FILENAME,arguments);
        int index=m_ui->tabellaEsito->rowCount();
        m_ui->tabellaEsito->insertRow(index);
        {
            QTableWidgetItem *it = new QTableWidgetItem();
            it->setText(nameList.first());
            it->setIcon(Icone[ElencoServiziConfigurati[idList.first()].idServizio]);
            m_ui->tabellaEsito->setItem(index,0,it);

            it = new QTableWidgetItem();
            it->setText("Invio in corso");
            m_ui->tabellaEsito->setItem(index,1,it);
        }
        m_ui->pushButton->setVisible(false);
        m_ui->buttonAnnullaInvio->setVisible(true);

        phoneList.removeFirst();
        idList.removeFirst();
        nameList.removeFirst();
    }
    */
}
void InvioMultiplo::readProcessStdOut(){
    /*static QString endMessage;
    static bool esito=true;
    QString strOut=sendProcess->readLine();
    strOut.resize(strOut.length()-NEWLINE_CHAR_COUNT);


    {
        int index=m_ui->tabellaEsito->rowCount();
        QTableWidgetItem *it = new QTableWidgetItem();
        it->setText(m_ui->tabellaEsito->item(index-1,1)->text()+".");
        m_ui->tabellaEsito->setItem(index-1,1,it);
    }
    //m_ui->textBrowser->setText(m_ui->textBrowser->toPlainText()+".");

    if (strOut.contains("ESITO: messaggio inviato correttamente")){
        {
            int index=m_ui->tabellaEsito->rowCount();
            QTableWidgetItem *it = new QTableWidgetItem();
            it->setText("Messaggio inviato.");
            m_ui->tabellaEsito->setItem(index-1,2,it);
            it = new QTableWidgetItem();
            if (endMessage.isEmpty())
                it->setText("Concluso");
            else
                it->setText("Concluso: "+endMessage);
            m_ui->tabellaEsito->setItem(index-1,1,it);
        }


        endMessage.clear();

        m_ui->pushButton->setVisible(true);
        m_ui->buttonAnnullaInvio->setVisible(false);
        this->SendNext();
    }else if (strOut.contains("INTERROTTO: Invio interrotto dall'utente")){

        endMessage="";
        phoneList.clear();
        idList.clear();
        nameList.clear();

        m_ui->pushButton->setVisible(true);
        m_ui->buttonAnnullaInvio->setVisible(false);
    }else if (strOut.contains("ERRORE:")){
        endMessage=strOut;

    }else if (strOut.contains("MESSAGGIO:")){
        endMessage=strOut.right(strOut.length()-11);
    }else if (strOut.contains("ESITO: messaggio non inviato")){

        {
            int index=m_ui->tabellaEsito->rowCount();
            QTableWidgetItem *it = new QTableWidgetItem();
            it->setText("Messaggio non inviato.");
            m_ui->tabellaEsito->setItem(index-1,2,it);

            if (!endMessage.isEmpty()){
                it = new QTableWidgetItem();
                it->setText(endMessage);
                m_ui->tabellaEsito->setItem(index-1,1,it);
                endMessage.clear();
            }


        }

        endMessage="";

        m_ui->pushButton->setVisible(true);
        m_ui->buttonAnnullaInvio->setVisible(false);
        this->SendNext();
    }else if (strOut.contains("INFO: sms salvato in locale con id ")){
        QString tmp="INFO: sms salvato in locale con id ";
        int pos=strOut.indexOf(" ",tmp.length()+1);
        QString id;
        if (pos >0){
            id=strOut.mid(tmp.length(),pos-tmp.length());
        }else{
            id=strOut.right(tmp.length());
        }
        //m_ui->textBrowser->append("id: "+id+"\n");
        padre->LastSms.first=id;
        padre->WriteSmsToLog();



    }else if (strOut.contains("INFO CAPTCHA: captcha salvato in")){

        //cerca se c'è un plugin captcha per questo servizio
        JackCaptchaPluginInterface* availablePlugin = 0;
        if(Opzioni["use-captcha"]=="yes"){
            QString serviceName=padre->ElencoServizi[padre->LastSms.second.idServizio].nome;

            foreach(JackPluginInterface* iPlugin, padre->pluginsList){
                if(iPlugin->getPluginInfo().pluginType == CaptchaDecoder){
                    JackCaptchaPluginInterface* plugin = dynamic_cast<JackCaptchaPluginInterface*>(iPlugin);
                    if(plugin->getPluginStatus() == Ready && plugin->getServiceName() == serviceName){
                        availablePlugin = plugin;
                        break;
                    }
                }
            }
        }
        //il plugin c'è
        if((Opzioni["use-captcha"]=="yes") && availablePlugin){
            QString  result = availablePlugin->decodeCaptcha(jackDirectories::DumpDirectory()+"captcha.png") + "\n";
            //sends result to process
            sendProcess->write(result.toStdString().c_str());
        }
        //non abbiamo un plugin disponibile o l'opzione è disattivata
        else{
            CaptchaDialog *diag=new CaptchaDialog(jackDirectories::DumpDirectory()+"captcha.png",sendProcess,Opzioni["captcha-zoom"],this);
            //CaptchaDialog *diag=new CaptchaDialog(jackDirectories::DumpDirectory()+"captcha.png",SendSmsProcess,Opzioni["captcha-zoom"],this);
            diag->exec();
            diag->deleteLater();
        }



    }*/




}

void InvioMultiplo::on_lineEdit_textChanged(QString )
{

}

void InvioMultiplo::on_RicercaVeloce_textChanged(QString )
{
    /*m_ui->rubricaVeloce->clear();
    jackTypes::AbookMap::const_iterator i=Rubrica.begin();
    jackTypes::AbookMap::const_iterator i_end=Rubrica.end();
    for(;i!=i_end;++i){

        if (i.key().toUpper().indexOf(m_ui->RicercaVeloce->text().toUpper())!=(-1)){

            QListWidgetItem *it=new QListWidgetItem();
            QIcon ico;
            it->setText(i.key());
            QString idServizio;
            {
                jackTypes::ConfiguredServicesMap::const_iterator x=ElencoServiziConfigurati.find(i->servizioAssociato);
                idServizio=x.value().idServizio;
            }
            {
                jackTypes::IconsMap::const_iterator x=Icone.find(idServizio);
                if (x!=Icone.end())
                    ico=x.value();
            }
            it->setIcon(ico);
            m_ui->rubricaVeloce->addItem(it);

        }
    }*/
}

void InvioMultiplo::on_pushButton_3_clicked()
{

}

void InvioMultiplo::on_ButtonAddAll_clicked()
{
    /*for(int x=0;x<m_ui->rubricaVeloce->count();++x){
        QListWidgetItem *it=new QListWidgetItem(*m_ui->rubricaVeloce->item(x));
        m_ui->destinatari->addItem(it);
    }*/
}

void InvioMultiplo::on_radiounico_clicked()
{

}

void InvioMultiplo::on_buttonAnnullaInvio_clicked()
{
    /*CancelSendSmsProcess = new QProcess( this );
    QStringList arguments;
    arguments <<"-stopsend";
    m_ui->buttonAnnullaInvio->setEnabled(false);
    CancelSendSmsProcess->start(JACKSMS_BINARY_FILENAME,arguments);
    */
}

void InvioMultiplo::on_InvioMultiplo_destroyed()
{

}

void InvioMultiplo::on_InvioMultiplo_finished(int result)
{

}
