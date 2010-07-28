#include "mainjacksms.h"
#include "captchadialog.h"
#include "servicesdialog.h"
#include "ui_mainjacksms.h"
#include <QListWidgetItem>
#include <QThread>
#include <QMessageBox>
#include <QTableWidget>
#include "Types.h"
#include "Configuration.h"
#include "aggiungicontatto.h"
#include "opzionidialog.h"
#include "JackUtils.h"
#include <QTime>
#include <QDate>
#include <QDesktopServices>
#include <QMultiMap>
#include <QSystemTrayIcon>
#include <QTimer>
#include "editcontattodialog.h"
#include <QHeaderView>
#include <QMap>
#include "inviomultiplo.h"
#include "libjacksms/libJackSMS.h"
#include <QSharedMemory>
#include <QSemaphore>
#include "smswidget.h"
#include "contactwidget.h"


#include "contactwidgetfastbook.h"
#include "accountwidget.h"
#include "Types.h"


#include <cmath>
#include "statsdialog.h"
#include "plugindialog.h"
#include "qlabelresult.h"
#include "messageloader.h"
#define COUNTDOWNTOGUICOUNTDEFINE 5
MainJackSMS::MainJackSMS(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainJackSMS)
{

    //abilito la gui quando  countdownToGuiCount==0
    //1-fine caricamento servizi
    //2-fine login
    //3-fine caricamento messaggi
    //4-fine caricamento account di lofin
    //5 -fine caricamento rubrica login
    countdownToGuiCount=COUNTDOWNTOGUICOUNTDEFINE;
    imChecker=NULL;
    updateChecker=NULL;

    ui->setupUi(this);
    ui->widgetSchermate->setCurrentIndex(0);
    ui->tabWidget->setCurrentIndex(0);
    loggedIn=false;
    currentMaxLength=160;
    countReceivedUnreaded=0;
    setWindowTitle(QString("JackSMS ")+QString(JACKSMS_VERSION));
    /*imposto lo spinner nella schermata di caricamento*/
    spinner=new QMovie(":/resource/loading-spinner.gif",QByteArray("gif"),this);
    spinner->start();
    ui->labelSpinner->setMovie(spinner);

    spinnerDC=new QMovie(":/resource/loading-spinner.gif",QByteArray("gif"),this);
    spinnerDC->setScaledSize(QSize(16,16));
    spinnerDC->start();
    ui->labelSpinDelContact->setMovie(spinnerDC);
    ui->labelSpinDelContact->hide();
    ui->labelSpinDelAccount->setMovie(spinnerDC);
    ui->labelSpinDelAccount->hide();
    ui->progressBar->setMovie(spinnerDC);

    QPalette p = ui->smsListWidget->palette();
    p.setColor(QPalette::Highlight,QColor(209,215,231));
    ui->smsListWidget->setPalette(p);
    ui->listServiziConfigurati->setPalette(p);
    ui->rubricaListWidget->setPalette(p);
    ui->imRicevutiWidget->setPalette(p);
    ui->RubricaVeloce->setPalette(p);
    ui->LabelEsito->~QWidget();
    ui->LabelEsito=new QLabelResult(this);




    ui->horizontalLayout_16->addWidget(ui->LabelEsito);
    ui->LabelEsito->hide();
    /*imposto la tray icon*/
    trayIco=new QSystemTrayIcon(this);
    #ifdef __APPLE__
       trayIco->setIcon(QIcon(":/resource/JackIcon_bn.png"));
    #else
       trayIco->setIcon(QIcon(":/resource/tray_icon.png"));
    #endif
    connect(trayIco,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),this,SLOT(TrayClicked()));
    connect(trayIco,SIGNAL(messageClicked()),this,SLOT(ClickBaloon()));
    trayIco->show();

    imServiceActive=false;
    invioInCorso=false;


    /*imposto il menu di scelta del tipo di invio*/
    menu=new QMenu(this);
    menu->setDefaultAction(menu->addAction(QIcon(),"Invio singolo"));
    menu->addAction(QIcon(),"Invio multiplo");
    connect(menu,SIGNAL(triggered(QAction* )),this,SLOT(gestiscimenu(QAction*)));
    ui->bottoneinviomultiplo->setMenu(menu);





    //nascondo alcuni bottoni nella schermata dei messaggi
    ui->InoltraButton->setVisible(false);
    ui->CitaButton->setVisible(false);





    /////////nascondo alcune cose che ancora non sono state implementate... da rimuovere poi...
    ui->ModificaServizioButton->hide();
    //ui->RubricaModifica->hide();
    ui->tabWidget->removeTab(5);
    ui->tabWidget->removeTab(5);
    ui->bottoneinviomultiplo->hide();

    ui->actionImporta_Backup->setVisible(false);
    ui->actionCrea_backup_configurazione->setVisible(false);

    ui->actionPlugins->setVisible(false);






    //carica i plugin
    //loadPlugins();
    jphi = new JackPluginHostInterface();



    ui->username->setFocus();
    ui->progressBar->hide();
    ui->LabelCountChars->show();
    ui->AnnullaSMS->hide();

    disableUibeforeLogin();


    ////////////////////////////




    libJackSMS::localApi::userDirectoryManager man("");

    if(!man.initializeDirectory()){
        QMessageBox::critical(this,"JackSMS","Non posso inizializzare JackSMS: Impossibile creare le cartelle necessarie.\nJacksms verrà chiuso.\n\nErrore: "+man.getError());

        exit(-1);
    }



    /*questo carica le opzioni globali*/
    initialXmlLoader=new libJackSMS::localApi::xmlLoader("");
    connect(initialXmlLoader,SIGNAL(optionsLoaded(libJackSMS::dataTypes::optionsType)),this,SLOT(initialOptionsLoaded(libJackSMS::dataTypes::optionsType)));
    initialXmlLoader->loadOptions();




    try{
        finder=new libJackSMS::localApi::userFinder;
        finder->findUsers();
        while(finder->nextUser()){
            ui->username->addItem(QIcon(),finder->currentUsername());
        }
    }catch(libJackSMS::exceptionXmlError){
        QMessageBox::critical(this,"JackSMS","Il file degli utenti sembra essere corrotto. Non posso caricare la lista degli utenti e le loro opzioni");
    }catch(libJackSMS::exceptionXmlNotFound){
        /*non trovare il file significa che non ci sono utenti(tipico dell'installazione fresca) */
    }
    translateGui();

}
void MainJackSMS::initialOptionsLoaded(libJackSMS::dataTypes::optionsType o){
    Opzioni=o;
    GlobalOptions=o;
    if (GlobalOptions["save-passwd"]=="yes"){
        ui->ricordaPassword->setChecked(true);
    }
}
MainJackSMS::~MainJackSMS()
{
    if (updateChecker!=NULL)
        if (updateChecker->isRunning()){
            updateChecker->abort();

        }
    if (imChecker!=NULL && Opzioni["receive-im"]=="yes"){

        imChecker->stop();
        delete imChecker;

    }

    delete trayIco;
    delete ui;
}
void MainJackSMS::translateGui(){
    libJackSMS::dataTypes::optionsType::const_iterator iter=Opzioni.find("language");
    if (iter!=Opzioni.end()){
        libJackSMS::LanguageManager *lm=libJackSMS::LanguageManager::getIstance();

        if(lm->setLanguage(iter.value())){
            ui->InviaSMS->setText(lm->getString("1"));
            ui->AnnullaSMS->setText(lm->getString("2"));
            ui->loginButton->setText(lm->getString("3"));




            ui->label_3->setText("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN" "http://www.w3.org/TR/REC-html40/strict.dtd\"><html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">p, li { white-space: pre-wrap; } </style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400; font-style:normal;\">            <p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><a href=\"http://www.jacksms.it/registrazione.html\"><span style=\" text-decoration: underline; color:#000000;\">"+lm->getString("4")+"</span></a></p></body></html>");
            ui->label_16->setText("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN" "http://www.w3.org/TR/REC-html40/strict.dtd\"><html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">p, li { white-space: pre-wrap; } </style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400; font-style:normal;\">            <p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><a href=\"http://www.jacksms.it/recupera-password.html\"><span style=\" text-decoration: underline; color:#000000;\">"+lm->getString("5")+"</span></a></p></body></html>");
            ui->label_19->setText(lm->getString("7"));
            ui->ricordaPassword->setText(lm->getString("6"));
            ui->label->setText(lm->getString("8"));
            ui->label_2->setText(lm->getString("9"));
            ui->label_7->setText(lm->getString("10"));
            ui->label_5->setText(lm->getString("11"));
            ui->InviaSMS->setText(lm->getString("12"));
            ui->AnnullaSMS->setText(lm->getString("13"));
            ui->tabWidget->setTabText(2,lm->getString("14"));
            ui->tabWidget->setTabText(3,lm->getString("15"));
            ui->tabWidget->setTabText(4,lm->getString("16"));
            ui->RispondiButton->setText(lm->getString("17"));
            ui->InoltraButton->setText(lm->getString("18"));
            ui->CitaButton->setText(lm->getString("20"));
            ui->EliminaButton->setText(lm->getString("19"));
            ui->label_14->setText(lm->getString("21"));
            ui->label_11->setText(lm->getString("22"));
            ui->RubricaAggiungi->setText(lm->getString("23"));
            ui->RubricaModifica->setText(lm->getString("24"));
            ui->RubricaElimina->setText(lm->getString("25"));

            ui->AggiungiServizioButton->setText(lm->getString("23"));
            ui->ModificaServizioButton->setText(lm->getString("24"));
            ui->EliminaServizioButton->setText(lm->getString("25"));
            ui->label_15->setText(lm->getString("26"));
            ui->label_9->setText(lm->getString("27"));
            ui->RispondiIm->setText(lm->getString("17"));


        }

    }

}


void MainJackSMS::gestiscimenu(QAction* act){

    menu->setDefaultAction(act);

}


void MainJackSMS::WriteAddressBookToGui(){

    int section=1;
    try{
        libJackSMS::dataTypes::phoneBookType::const_iterator i=Rubrica.begin();
        libJackSMS::dataTypes::phoneBookType::const_iterator i_end=Rubrica.end();

        QMultiMap<QString,contactWidgetFastBook*> fastList;
        QMultiMap<QString,ContactWidget*> list;

        for(;i!=i_end;++i){
            QString accountName="Nessun servizio associato";
            QIcon ico;
            {
                libJackSMS::dataTypes::configuredServicesType::const_iterator x=ElencoServiziConfigurati.find(i->getAccount());
                if (x==ElencoServiziConfigurati.end()){
                    ico=QIcon(":/resource/ico_contact.png");

                }else{
                    ico=ElencoServizi[x.value().getService()].getIcon();
                    //ico=createIcon(QString::fromStdString(x->second.getService()));

                }
                contactWidgetFastBook *ww=new contactWidgetFastBook(
                       i->getId(),
                       i->getName(),
                       i->getPhone().toString(),
                       ico.pixmap(16,16),
                       i->getAccount()
                       );
                fastList.insert(i->getName().toUpper(),ww);
            }

            {
                libJackSMS::dataTypes::configuredServicesType::const_iterator x=ElencoServiziConfigurati.find(i->getAccount());
                if (x!=ElencoServiziConfigurati.end())
                    accountName=x.value().getName();
            }


            ContactWidget *ww=new ContactWidget(
                   i->getId(),
                   i->getName(),
                   i->getPhone().toString(),
                   ico.pixmap(16,16),
                   i->getGroup(),
                   accountName
                   );
            list.insert(i->getName().toUpper(),ww);
        }
        section++;
        if (list.size()>0){
            QMultiMap<QString,ContactWidget*>::ConstIterator xx=list.begin();
            QMultiMap<QString,ContactWidget*>::ConstIterator xx_end=list.end();
            for(;xx!=xx_end;++xx){
                QListWidgetItem *item = new QListWidgetItem;
                item->setSizeHint(xx.value()->size());
                ui->rubricaListWidget->addItem(item);
                ui->rubricaListWidget->setItemWidget(item, xx.value());
            }
        }
        section++;
        if (fastList.size()>0){
            QMultiMap<QString,contactWidgetFastBook*>::ConstIterator xx=fastList.end();
            QMultiMap<QString,contactWidgetFastBook*>::ConstIterator xx_end=fastList.begin();
            do{
               --xx;
               QListWidgetItem *item = new QListWidgetItem;
               item->setSizeHint(xx.value()->size());
               ui->RubricaVeloce->addItem(item);
               ui->RubricaVeloce->setItemWidget(item, xx.value());
            }while(xx!=xx_end);
        }

    }catch(...){
        QMessageBox::critical(this,"JackSMS","JackSMS ha rilevato un errore grave durante la procedura WriteAddressBookToGui().\nDettagli: unknown error, section: "+QString::number(section));
    }
}

void MainJackSMS::WriteConfiguredServicesToGui(){
    int section=1;
    try{
        libJackSMS::dataTypes::configuredServicesType::const_iterator i=ElencoServiziConfigurati.begin();
        libJackSMS::dataTypes::configuredServicesType::const_iterator i_end=ElencoServiziConfigurati.end();
        for(;i!=i_end;++i){
            //QIcon ico=createIcon(i->second.getService());
            QIcon ico=ElencoServizi[i.value().getService()].getIcon();

            ui->comboServizio->addItem(ico,i.value().getName());

            accountWidget *ww=new accountWidget(i.value().getId(),i.value().getName(),ico.pixmap(16,16));
            QListWidgetItem *item = new QListWidgetItem;
            item->setSizeHint(ww->size());
            ui->listServiziConfigurati->addItem(item);
            ui->listServiziConfigurati->setItemWidget(item, ww);

        }
    }catch(...){
        QMessageBox::critical(this,"JackSMS","JackSMS ha rilevato un errore grave durante la procedura WriteConfiguredServicesToGui().\nDettagli: unknown error, section: "+QString::number(section));
    }

}

QString MainJackSMS::phone2name(const libJackSMS::dataTypes::phoneNumber &_number){
    QString user_num_name=_number.getAltName();
    try{
        if (_number.getIsValid()){
            user_num_name=_number.toString();
            libJackSMS::dataTypes::phoneBookType::const_iterator x=Rubrica.begin();
            libJackSMS::dataTypes::phoneBookType::const_iterator x_end=Rubrica.end();
            bool found=false;
            for(;x!=x_end;++x){

                if(x->getPhone().toString()==user_num_name){
                    user_num_name=x->getName();
                    found=true;
                    break;
                }

            }
            if (!found){
                if (_number.getAltName()!=""){
                    user_num_name=user_num_name+QString(" (")+_number.getAltName()+QString(")");
                }
            }
        }
    }catch(...){


    }
    return user_num_name;
}


void MainJackSMS::WriteMessagesToGui(){

    int section=1;
    try{
        ui->smsListWidget->clear();
        mapWidgets.clear();

        //QPixmap icon_jack=createIcon("40");
        QPixmap icon_jack=ElencoServizi["40"].getIcon().pixmap(16,16);
        types::QMessageListType::const_iterator i=Messaggi.begin();
        types::QMessageListType::const_iterator i_end=Messaggi.end();
        for(;i!=i_end;++i){
            QString user_num_name=phone2name(i->getPhone());
            if (i->getIsReceived()){
                SmsWidget *wid=new SmsWidget(
                                             i->getMessage(),
                                             icon_jack,
                                             true,
                                             i->getData(),
                                             user_num_name,
                                             QString("JackSMS Messenger"),
                                             i->getId(),
                                             i->getPhone()
                                             );
                mapWidgets.insert(i->getData(),wid);
            }else{
                QPixmap ico=ElencoServizi[i->getServiceId()].getIcon().pixmap(16,16);

                SmsWidget *wid=new SmsWidget(
                                         i->getMessage(),
                                         ico,
                                         false,
                                         i->getData(),
                                         user_num_name,
                                         i->getAccountName(),
                                         i->getId(),
                                         i->getPhone()
                                         );
                mapWidgets.insert(i->getData(),wid);
            }
         }
         section++;
         if (mapWidgets.size()>0){
            QMultiMap<QDateTime,SmsWidget*>::ConstIterator xx=mapWidgets.end();
            QMultiMap<QDateTime,SmsWidget*>::ConstIterator xx_end=mapWidgets.begin();
            do{
                --xx;
                QListWidgetItem *item = new QListWidgetItem;
                item->setSizeHint(xx.value()->size());
                ui->smsListWidget->addItem(item);
                ui->smsListWidget->setItemWidget(item, xx.value());
            }while(xx!=xx_end);
        }

     }catch(...){
         QMessageBox::critical(this,"JackSMS","JackSMS ha rilevato un errore grave durante la procedura WriteMessagesToGui().\nDettagli: unknown error, section: "+QString::number(section));
     }
}
void MainJackSMS::WriteImToGui(){

    ui->imRicevutiWidget->clear();
    mapWidgetsReceived.clear();

    //QPixmap icon_jack=createIcon("40");
    QPixmap icon_jack=ElencoServizi["40"].getIcon().pixmap(16,16);
    types::QMessageListType::const_iterator i=MessaggiRicevuti.begin();
    types::QMessageListType::const_iterator i_end=MessaggiRicevuti.end();
    for(;i!=i_end;++i){
        QString user_num_name=phone2name(i->getPhone());

        SmsWidget *wid=new SmsWidget(
                                     i->getMessage(),
                                     icon_jack,
                                     true,
                                     i->getData(),
                                     user_num_name,
                                     QString("JackSMS Messenger"),
                                     i->getId(),
                                     i->getPhone(),
                                     i->getReaded()
                                     );
        this->mapWidgetsReceived.insert(i->getData(),wid);

    }

     if (mapWidgetsReceived.size()>0){
        QMultiMap<QDateTime,SmsWidget*>::ConstIterator xx=mapWidgetsReceived.end();
        QMultiMap<QDateTime,SmsWidget*>::ConstIterator xx_end=mapWidgetsReceived.begin();
        do{
            --xx;
            QListWidgetItem *item = new QListWidgetItem;
            item->setSizeHint(xx.value()->size());
            ui->imRicevutiWidget->addItem(item);
            ui->imRicevutiWidget->setItemWidget(item, xx.value());
        }while(xx!=xx_end);
    }


}





void MainJackSMS::on_RubricaAggiungi_clicked()
{
    AggiungiContatto *diag=new AggiungiContatto(this,this,ElencoServiziConfigurati,Rubrica,ElencoServizi,Opzioni);
    diag->exec();
    diag->deleteLater();

}
void MainJackSMS::displayCaptcha(QByteArray data,QSemaphore* sem){
    /*
            //cerca se c'è un plugin captcha per questo servizio
            JackCaptchaPluginInterface* availablePlugin = 0;
            if(Opzioni["use-captcha"]=="yes"){
                jackTypes::ConfiguredServicesMap::const_iterator i=ElencoServiziConfigurati.begin();
                jackTypes::ConfiguredServicesMap::const_iterator i_end=ElencoServiziConfigurati.end();
                QString serviceName;
                for(;i!=i_end;++i){
                    if (ui->comboServizio->currentText()==i->nome){
                        serviceName=ElencoServizi[i->idServizio].nome;
                        break;
                    }

                }

                foreach(JackPluginInterface* iPlugin, pluginsList){
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
                SendSmsProcess->write(result.toStdString().c_str());
            }
            //non abbiamo un plugin disponibile o l'opzione è disattivata
            else{
                CaptchaDialog *diag=new CaptchaDialog(jackDirectories::DumpDirectory()+"captcha.png",SendSmsProcess,Opzioni["captcha-zoom"],this);
                diag->exec();
                diag->deleteLater();
            }
*/

    try{
        CaptchaDialog *dial=new CaptchaDialog(data,QString("100%"),this);
        dial->exec();
        dial->deleteLater();

    }catch (QString s){
        QMessageBox::critical(this,"JackSMS","Si e' verificato un errore.\nImpossibile caricare l'immagine captcha. L'invio verra' interrotto.");
    }
    sem->release(1);
}

void MainJackSMS::eseguiPassoInvio(){

    //ui->progressBar->setValue(ui->progressBar->value()+1);

}
void MainJackSMS::popupInvio(){
    QMessageBox::information(this,"JackSMS",esitoInvio);



}

void MainJackSMS::invioSuccesso(const QString & _text){

    if (!_text.isEmpty()){
        if (_text.length()<45){
            ui->LabelEsito->setText("Messaggio inviato: "+_text);
            if (esitoInvio.length()>=45)
                disconnect(ui->LabelEsito,SIGNAL(clicked()),this,SLOT(popupInvio()));

        }else{
            if (esitoInvio.length()<45)
                connect(ui->LabelEsito,SIGNAL(clicked()),this,SLOT(popupInvio()));
            esitoInvio=_text;
            ui->LabelEsito->setText("Messaggio inviato: "+_text.left(45)+"...");
        }


    }else{
        if (esitoInvio.length()>=45)
            disconnect(ui->LabelEsito,SIGNAL(clicked()),this,SLOT(popupInvio()));
        ui->LabelEsito->setText("Messaggio inviato!");
    }




    invioInCorso=false;
    QString a=ultimoSms.getData().toString("dd/MM/yyyy");
    QString b=ultimoSms.getData().toString("HH:mm:ss");
    libJackSMS::dataTypes::dateTime dd(a+" "+b);

    libJackSMS::dataTypes::logSmsMessage us(ultimoSms.getPhone(),
                                           ElencoServiziConfigurati[ultimoSms.getAccountId()].getName(),
                                           ultimoSms.getServiceId(),//ElencoServiziConfigurati[idAccount].getService(),
                                           dd,
                                           "",
                                           ultimoSms.getMessage()
                                           );

    libJackSMS::dataTypes::optionsType::const_iterator iter=Opzioni.find("save-local");

    if (iter!=Opzioni.end())
        if ("yes"==iter.value()){
            libJackSMS::localApi::smsLogSaver sav(current_user_directory);
            sav.setMessage(us);
            sav.appendToLogFile();
            ultimoSms.setId(sav.getSavedId());
            //std::cout<<"[SAVE] messaggio sms salvato in locale"<<sav.getSavedId()<<std::endl;
            Messaggi.push_back(ultimoSms);

            //ReWriteMessagesToGui();
            QString user_num_name=phone2name(ultimoSms.getPhone());
            QPixmap icona=ElencoServizi[ultimoSms.getServiceId()].getIcon().pixmap(16,16);
            SmsWidget *wid=new SmsWidget(
                                         ultimoSms.getMessage(),
                                         icona,
                                         false,
                                         ultimoSms.getData(),

                                         user_num_name,
                                         ultimoSms.getAccountName(),
                                         ultimoSms.getId(),
                                         ultimoSms.getPhone(),
                                         true
                                         );
            mapWidgets.insert(ultimoSms.getData(),wid);
            QListWidgetItem *item = new QListWidgetItem;
            item->setSizeHint(wid->size());
            ui->smsListWidget->insertItem(0,item);
            ui->smsListWidget->setItemWidget(item, wid);
        }
    AbilitaUi();

    ElencoServiziConfigurati[ultimoSms.getAccountId()].increaseStatValue("sent");
    //QString v=ElencoServiziConfigurati[ultimoSms.getAccountId()].getStat("sent");
    libJackSMS::localApi::statsManager man(current_user_directory);
    man.increaseSentStat(ElencoServiziConfigurati[ultimoSms.getAccountId()]);
    libJackSMS::localApi::optionManager man2(current_user_directory,Opzioni);
    man2.increaseTotalSent();



    onlineSmsSaver=new libJackSMS::serverApi::smsLogSaver(current_login_id,Opzioni);
    onlineSmsSaver->save(us);




}
void MainJackSMS::invioFallito(const QString & _text){

    if (!_text.isEmpty()){
        ui->LabelEsito->setText("Messaggio non inviato: "+_text);
    }else{
        ui->LabelEsito->setText("Messaggio non inviato!");
    }

    AbilitaUi();
    invioInCorso=false;

}
void MainJackSMS::on_InviaSMS_clicked()
{

    try{
        invioInCorso=true;
        DisabilitaUi();

        libJackSMS::dataTypes::shortMessage messaggio(ui->TestoSMS->toPlainText());
        libJackSMS::dataTypes::phoneNumber destinatario;
        destinatario.parse(ui->NumeroDestinatario->text());
        QString idAccount;
        {


            libJackSMS::dataTypes::configuredServicesType::const_iterator i=ElencoServiziConfigurati.begin();
            libJackSMS::dataTypes::configuredServicesType::const_iterator i_end=ElencoServiziConfigurati.end();

            for(;i!=i_end;++i){
                if (i.value().getName()==ui->comboServizio->currentText()){
                    idAccount=i.key();
                    break;
                }
            }

        }

        ultimoSms=QMyMessage();
        ultimoSms.setMessage(ui->TestoSMS->toPlainText());
        ultimoSms.setAccountId(idAccount);
        ultimoSms.setAccountName(ui->comboServizio->currentText());
        ultimoSms.setIsReceived(false);
        ultimoSms.setData(QDateTime::currentDateTime());
        ultimoSms.setPhone(destinatario);
        ultimoSms.setServiceId(ElencoServiziConfigurati[idAccount].getService());

        smsSender=new libJackSMS::smsSender(ElencoServizi,Opzioni);
        smsSender->setRecipient(destinatario);
        smsSender->setMessage(messaggio);
        smsSender->setAccount(ElencoServiziConfigurati[idAccount]);

        connect(smsSender,SIGNAL(captcha(QByteArray,QSemaphore*)),this,SLOT(displayCaptcha(QByteArray,QSemaphore*)));
        connect(smsSender,SIGNAL(error(QString)),this,SLOT(invioFallito(QString)));
        connect(smsSender,SIGNAL(success(QString)),this,SLOT(invioSuccesso(QString )));


        smsSender->send();


    }catch(libJackSMS::exceptionPhoneNumber e){
        QMessageBox::information(this,"JackSMS","Numero in formato sconosciuto");
        invioInCorso=false;
    }


}



void MainJackSMS::on_RubricaModifica_clicked()
{
    QListWidgetItem *wid=ui->rubricaListWidget->currentItem();
    if (wid!=NULL){
            QWidget * wi=ui->rubricaListWidget->itemWidget(wid);
            ContactWidget* cw=static_cast<ContactWidget*>(wi);
            QString id=cw->getContactId();
            editcontattodialog *dial=new editcontattodialog(this,this,ElencoServizi,ElencoServiziConfigurati,Rubrica,id,Opzioni);
            dial->exec();
            dial->deleteLater();
    }
}


void MainJackSMS::on_actionEsci_triggered()
{
    this->close();
}


void MainJackSMS::on_AggiungiServizioButton_clicked()
{
    ServicesDialog *sd=new ServicesDialog(this,this,ElencoServizi,ElencoServiziConfigurati,Opzioni);
    sd->exec();
    sd->deleteLater();


}



void MainJackSMS::on_EliminaServizioButton_clicked()
{

    QListWidgetItem *wid=ui->listServiziConfigurati->currentItem();
    if (wid!=NULL){
        if (QMessageBox::Yes==QMessageBox::question(this,"JackSMS","Eliminare questo account?",QMessageBox::Yes,QMessageBox::No)){
            QWidget * wi=ui->listServiziConfigurati->itemWidget(wid);
            accountWidget* cw=static_cast<accountWidget*>(wi);
            //QMessageBox::information(this,"JackSMS",cw->getName());
            QString id=cw->getAccountId();
            ui->labelSpinDelAccount->show();
            ui->EliminaServizioButton->setEnabled(false);
            accountManager=new libJackSMS::serverApi::accountManager(current_login_id,Opzioni);
            connect(accountManager,SIGNAL(accountDeleted(QString)),this,SLOT(deleteAccountOk(QString)));
            connect(accountManager,SIGNAL(accountNotDeleted()),this,SLOT(deleteAccountKo()));
            accountManager->deleteAccount(id);

        }


    }



}

void MainJackSMS::on_actionOpzioni_triggered()
{
   //QMessageBox::information(this,"asd",current_user_directory);
   OpzioniDialog * dial=new OpzioniDialog(Opzioni,*ui->TestoSMS,current_user_directory,this,loggedIn);
   connect(dial,SIGNAL(activateImTimer()),this,SLOT(startIm()));
   connect(dial,SIGNAL(deactivateImTimer()),this,SLOT(stopIm()));
   //connect(dial,SIGNAL(reactivateImTimer()),this,SLOT(restartIm()));
   connect(dial,SIGNAL(translate()),this,SLOT(translateGui()));
   dial->exec();
   dial->deleteLater();

}
void MainJackSMS::ReWriteAddressBookToGui(){
    ui->rubricaListWidget->clear();
    ui->RubricaVeloce->clear();
    WriteAddressBookToGui();
}
void MainJackSMS::ReWriteConfiguredServicesToGui(){
    ui->listServiziConfigurati->clear();
    ui->comboServizio->clear();
    WriteConfiguredServicesToGui();
}
void MainJackSMS::ReloadRubrica(){


    /*try{
        libJackSMS::localApi::xmlLoader caricatore(this->current_user_directory.toStdString());
        Rubrica.clear();
        caricatore.loadPhoneBook(cRubrica);
        ui->rubricaListWidget->clear();
        ui->RubricaVeloce->clear();
        WriteAddressBookToGui();
    }catch(libJackSMS::exceptionXmlError e){
        QMessageBox::critical(this,"JackSMS","Si e' verificato un errore durante il caricamento della rubrica.");
    }catch(libJackSMS::exceptionPhoneNumber e){
        QMessageBox::critical(this,"JackSMS","Si e' verificato un errore durante il caricamento della rubrica.\nAlcuni numeri telefonici non sono riconoscibili.");
    }
*/



}

void MainJackSMS::ReadStdoutReloadRubrica(){

}
void MainJackSMS::ReloadConfiguredServices(){
    ElencoServiziConfigurati.clear();
    libJackSMS::localApi::xmlLoader load(current_user_directory);
    load.loadAccounts(ElencoServiziConfigurati);
    ui->listServiziConfigurati->clear();
    ui->comboServizio->clear();
    WriteConfiguredServicesToGui();
}



void MainJackSMS::on_RicercaVeloce_textChanged(QString )
{
    ui->RubricaVeloce->clear();
    QMultiMap<QString,contactWidgetFastBook*> fastList;

    libJackSMS::dataTypes::phoneBookType::const_iterator i=Rubrica.begin();
    libJackSMS::dataTypes::phoneBookType::const_iterator i_end=Rubrica.end();
    QString newText=ui->RicercaVeloce->text().toUpper();
    for(;i!=i_end;++i){
        if (i->getName().toUpper().contains(newText) || i->getPhone().toString().toUpper().contains(newText)){
            //QString accountName="Nessun servizio associato";
            QIcon ico;
            libJackSMS::dataTypes::configuredServicesType::const_iterator x=ElencoServiziConfigurati.find(i->getAccount());
            if (x==ElencoServiziConfigurati.end())
                ico=QIcon(":/resource/ico_contact.png");
            else
                ico=ElencoServizi[x.value().getService()].getIcon();
                //ico=createIcon(QString::fromStdString(x->second.getService()));

            contactWidgetFastBook *ww=new contactWidgetFastBook(
                   i->getId(),
                   i->getName(),
                   i->getPhone().toString(),
                   ico.pixmap(16,16),
                   i->getAccount()
                   );
            fastList.insert(i->getName().toUpper(),ww);
        }
    }
    if (fastList.size()>0){
       QMultiMap<QString,contactWidgetFastBook*>::ConstIterator xx=fastList.end();
       QMultiMap<QString,contactWidgetFastBook*>::ConstIterator xx_end=fastList.begin();
       do{
           --xx;
           QListWidgetItem *item = new QListWidgetItem;
           item->setSizeHint(xx.value()->size());
           ui->RubricaVeloce->addItem(item);
           ui->RubricaVeloce->setItemWidget(item, xx.value());
       }while(xx!=xx_end);
   }

}








void MainJackSMS::on_InoltraButton_clicked()
{

    QList<QListWidgetItem *> ls=ui->smsListWidget->selectedItems();

    if (!ls.isEmpty()){
             ui->labelNomeDest->setText("");
             QListWidgetItem *it=ls.front();
             SmsWidget * ww=static_cast< SmsWidget*>(ui->smsListWidget->itemWidget(it));
             ui->TestoSMS->setText(ww->getText());
             ui->tabWidget->setCurrentIndex(0);

    }

}



void MainJackSMS::on_AnnullaSMS_clicked()
{
    smsSender->abort();

    ui->LabelEsito->setText("Invio annullato dall'utente.");
    invioInCorso=false;
    AbilitaUi();
}



void MainJackSMS::on_CitaButton_clicked()
{
    QList<QListWidgetItem *> ls=ui->smsListWidget->selectedItems();



    if (!ls.isEmpty()){

        QListWidgetItem *it=ls.front();
        SmsWidget * ww=static_cast< SmsWidget*>(ui->smsListWidget->itemWidget(it));
        QString phone=ww->getPhoneNum().toString();
        ui->NumeroDestinatario->setText(phone);
        ui->TestoSMS->setText("[ "+ww->getText()+" ]");
        libJackSMS::dataTypes::phoneBookType::const_iterator a=Rubrica.begin();
        libJackSMS::dataTypes::phoneBookType::const_iterator a_end=Rubrica.end();
        ui->labelNomeDest->setText("");
        for (;a!=a_end;++a){
            if (a.value().getPhone().toString()==phone){
                ui->labelNomeDest->setText(a.value().getName());
                break;
            }
        }

        libJackSMS::dataTypes::configuredServicesType::const_iterator x=ElencoServiziConfigurati.begin();
        libJackSMS::dataTypes::configuredServicesType::const_iterator x_end=ElencoServiziConfigurati.end();
        for(;x!=x_end;x++){
            if (x.value().getService()=="40"){
                int index=ui->comboServizio->findText(x.value().getName());
                ui->comboServizio->setCurrentIndex(index);
                break;
            }
        }
         ui->tabWidget->setCurrentIndex(0);


    }
}

void MainJackSMS::on_RispondiButton_clicked()
{
    QList<QListWidgetItem *> ls=ui->smsListWidget->selectedItems();



    if (!ls.isEmpty()){

        QListWidgetItem *it=ls.front();
        SmsWidget * ww=static_cast< SmsWidget*>(ui->smsListWidget->itemWidget(it));
        libJackSMS::dataTypes::phoneNumber phone=ww->getPhoneNum();
        ui->NumeroDestinatario->setText(phone.toString());
        ui->TestoSMS->setText("");
        libJackSMS::dataTypes::phoneBookType::const_iterator a=Rubrica.begin();
        libJackSMS::dataTypes::phoneBookType::const_iterator a_end=Rubrica.end();
        ui->labelNomeDest->setText("");
        for (;a!=a_end;++a){
            if (a.value().getPhone().toString()==phone.toString()){
                ui->labelNomeDest->setText(a.value().getName());
                break;
            }
        }

        libJackSMS::dataTypes::configuredServicesType::const_iterator x=ElencoServiziConfigurati.begin();
        libJackSMS::dataTypes::configuredServicesType::const_iterator x_end=ElencoServiziConfigurati.end();
        for(;x!=x_end;x++){
            if (x.value().getService()=="40"){
                int index=ui->comboServizio->findText(x.value().getName());
                ui->comboServizio->setCurrentIndex(index);
                break;
            }
        }
         ui->tabWidget->setCurrentIndex(0);
    }
}

void MainJackSMS::ClickBaloon(){

    raise();
    activateWindow();
    showNormal();
    ui->tabWidget->setCurrentIndex(1);

}

void MainJackSMS::on_NumeroDestinatario_textEdited(QString )
{
    ui->labelNomeDest->setText("");
}



void MainJackSMS::on_TextRapidRubrica_textChanged(QString )
{
    ui->rubricaListWidget->clear();
     QMultiMap<QString,ContactWidget*> list;

    QString newText=ui->TextRapidRubrica->text().toUpper();
    libJackSMS::dataTypes::phoneBookType::const_iterator i=Rubrica.begin();
    libJackSMS::dataTypes::phoneBookType::const_iterator i_end=Rubrica.end();

    for(;i!=i_end;++i){
        if (i->getName().toUpper().contains(newText) || i->getPhone().toString().toUpper().contains(newText)){
            QString accountName="Nessun servizio associato";
            QIcon ico;
            {
                libJackSMS::dataTypes::configuredServicesType::const_iterator x=ElencoServiziConfigurati.find(i->getAccount());
                if (x==ElencoServiziConfigurati.end()){
                    ico=QIcon(":/resource/ico_contact.png");

                }else{
                    ico=this->ElencoServizi[x.value().getService()].getIcon();
                    //ico=createIcon(QString::fromStdString(x->second.getService()));

                }

            }

            {
                libJackSMS::dataTypes::configuredServicesType::const_iterator x=ElencoServiziConfigurati.find(i->getAccount());
                if (x!=ElencoServiziConfigurati.end())
                    accountName=x.value().getName();
            }

            ContactWidget *ww=new ContactWidget(
                   i->getId(),
                   i->getName(),
                   i->getPhone().toString(),
                   ico.pixmap(16,16),
                   i->getGroup(),
                   accountName
                   );

            list.insert(i->getName().toUpper(),ww);

        }
    }
    if (list.size()>0){
       QMultiMap<QString,ContactWidget*>::ConstIterator xx=list.begin();
       QMultiMap<QString,ContactWidget*>::ConstIterator xx_end=list.end();
       for(;xx!=xx_end;++xx){
           QListWidgetItem *item = new QListWidgetItem;
           item->setSizeHint(xx.value()->size());
           ui->rubricaListWidget->addItem(item);
           ui->rubricaListWidget->setItemWidget(item, xx.value());
       }
   }

}


void MainJackSMS::on_actionInfo_su_JackSMS_triggered()
{

    //QMessageBox::about(this,"JackSMS","JackSMS beta 1.2\n\nComunicazione libera e gratuita per tutti: questo e' il proposito di JackSMS.\nComunicare, chiacchierare, scambiarci idee e' una nostra necessita', un nostro diritto, un bisogno a cui nessuno puo' fare a meno.\nNon sarebbe bello se la piu' minuta delle forme di comunicazione, gli sms, potesse diventare gratuita per tutti?\nIn nome di quest'idea e' nato JackSMS.\nE quel gigantesco contatore che ogni giorno cresce e' il simbolo che, in fondo, quest'idea non e' poi cosi' assurda come potrebbe sembrare.");
    QDesktopServices::openUrl(QUrl("http://www.jacksms.it", QUrl::TolerantMode));

}

void MainJackSMS::on_RubricaElimina_clicked()
{
    QListWidgetItem *wid=ui->rubricaListWidget->currentItem();
    if (wid!=NULL){
        if (QMessageBox::Yes==QMessageBox::question(this,"JackSMS","Eliminare questo contatto?",QMessageBox::Yes,QMessageBox::No)){
            QWidget * wi=ui->rubricaListWidget->itemWidget(wid);
            ContactWidget* cw=static_cast<ContactWidget*>(wi);

            QString id=cw->getContactId();

            ui->labelSpinDelContact->show();
            ui->RubricaElimina->setEnabled(false);

            deleteContect=new libJackSMS::serverApi::contactManager(current_login_id,Opzioni);
            connect(deleteContect,SIGNAL(contactDeleted(QString)),this,SLOT(deleteContactOk(QString)));
            connect(deleteContect,SIGNAL(contactNotDeleted()),this,SLOT(deleteContactKo()));
            deleteContect->deleteContact(id);

        }


    }

}

void MainJackSMS::deleteContactKo(){
    ui->RubricaElimina->setEnabled(true);
    QMessageBox::critical(this,"JackSMS","Errore durante l'eliminazione del contatto.");
}
void MainJackSMS::deleteContactOk(QString id){
    Rubrica.remove(id);
    ui->RubricaElimina->setEnabled(true);
    this->ReWriteAddressBookToGui();
    ui->labelSpinDelContact->hide();

}

void MainJackSMS::on_RicercaVeloceIM_textChanged(QString )
{



    QString text=ui->RicercaVeloceIM->text().toUpper();
    ui->smsListWidget->clear();
    mapWidgets.clear();

    QPixmap icon_jack=ElencoServizi["40"].getIcon().pixmap(16,16);

    types::QMessageListType::const_iterator i=Messaggi.begin();
    types::QMessageListType::const_iterator i_end=Messaggi.end();
    for(;i!=i_end;++i){
        QString user_num_name=phone2name(i->getPhone());
        if (user_num_name.toUpper().contains(text) || i->getMessage().toUpper().contains(text)){
            if (i->getIsReceived()){
                SmsWidget *wid=new SmsWidget(
                                         i->getMessage(),
                                         icon_jack,
                                         true,
                                         i->getData(),
                                         user_num_name,
                                         QString("JackSMS Messenger"),
                                         i->getId(),
                                         i->getPhone()
                                         );
                mapWidgets.insert(i->getData(),wid);
            }else{
                QPixmap ico=ElencoServizi[i->getServiceId()].getIcon().pixmap(16,16);

                SmsWidget *wid=new SmsWidget(
                                         i->getMessage(),
                                         ico,
                                         false,
                                         i->getData(),
                                         user_num_name,
                                         i->getAccountName(),
                                         i->getId(),
                                         i->getPhone()
                                         );
                mapWidgets.insert(i->getData(),wid);
            }
        }

    }
    if (mapWidgets.size()>0){
        QMultiMap<QDateTime,SmsWidget*>::ConstIterator xx=mapWidgets.end();
        QMultiMap<QDateTime,SmsWidget*>::ConstIterator xx_end=mapWidgets.begin();
        do{
            --xx;
            QListWidgetItem *item = new QListWidgetItem;
            item->setSizeHint(xx.value()->size());
            ui->smsListWidget->addItem(item);
            ui->smsListWidget->setItemWidget(item, xx.value());
        }while(xx!=xx_end);
    }
}
void MainJackSMS::on_EliminaButton_clicked()
{
    QList<QListWidgetItem *> ls=ui->smsListWidget->selectedItems();
        QList<QString> smsIds;
        QList<QString> imIds;

          if (!ls.isEmpty()){
              if (QMessageBox::Yes==QMessageBox::question(this,"JackSMS","Eliminare questo/i messaggio/i?",QMessageBox::Yes,QMessageBox::No)){

                  while (!ls.isEmpty()){
                      QListWidgetItem *it=ls.front();

                      SmsWidget * ww=static_cast<SmsWidget*>(ui->smsListWidget->itemWidget(it));
                      if (ww->isReceived()){
                          imIds.push_back(ww->getId());
                      }else{
                          smsIds.push_back(ww->getId());
                      }
                      ls.removeFirst();
                      delete it;
                  }


                  types::QMessageListType::iterator i=Messaggi.begin();

                  for (;i!=Messaggi.end();++i){
                      QString id=i->getId();
                      int found=-1;
                      if (!i->getIsReceived())
                          found=smsIds.indexOf(id);
                      else
                          found=imIds.indexOf(id);
                      if (found>=0){
                            i=Messaggi.erase(i);
                            i--;
                      }

                  }
                  bool val=true;
                  if (!imIds.empty()){
                       libJackSMS::localApi::imLogSaver man(current_user_directory);
                      val=val && man.deleteIm(imIds);
                  }
                  if (!smsIds.empty()){
                      libJackSMS::localApi::smsLogSaver man(current_user_directory);
                      val=val && man.deleteSms(smsIds);
                  }



            }


        }
}







void MainJackSMS::on_actionCrea_backup_configurazione_triggered()
{
     /*QDateTime today=QDateTime::currentDateTime();
     QString data=today.toString("dd-MM-yyyy");

     QString fileName =QFileDialog::getSaveFileName(this,"Salva backup configurazione",jackDirectories::DataDirectory()+"JackSMS_Backup_"+data+".7z","Archivio backup JackSMS (*.7z)");
     if (!fileName.isEmpty()){
        SaveBackupProcess = new QProcess( this );
        QStringList arguments;
        arguments <<"-backup"<<fileName;
        connect(SaveBackupProcess, SIGNAL(finished(int)), this, SLOT(ProcessSaveBackup()));
        SaveBackupProcess->start(JACKSMS_BINARY_FILENAME,arguments);

     }*/
}



void MainJackSMS::on_actionImporta_Backup_triggered()
{


   /*  QString fileName =QFileDialog::getOpenFileName(this,"Importa backup configurazione",jackDirectories::DataDirectory(),"Archivio backup JackSMS (*.7z)");
     if (!fileName.isEmpty()){
        ImportBackupProcess = new QProcess( this );
        QStringList arguments;
        arguments <<"-importbackup"<<fileName;
        connect(ImportBackupProcess, SIGNAL(finished(int)), this, SLOT(ProcessImportBackup()));
        ImportBackupProcess->start(JACKSMS_BINARY_FILENAME,arguments);

     }*/
}






 bool MainJackSMS::eventFilter( QObject *obj, QEvent *ev )
  {

    return false;
  }



void MainJackSMS::on_TestoSMS_textChanged()
{
    QString txt=ui->TestoSMS->toPlainText();

    ui->LabelCountChars->show();

    ui->LabelEsito->hide();

    QString cSms=QString::number(ceil((float)txt.length()/currentSingleLength));
    QString nSms=QString::number(ceil((float)currentMaxLength/currentSingleLength));


    ui->LabelCountChars->setText("Testo: "+QString::number(txt.length())+"/"+QString::number(currentMaxLength)+", SMS: "+cSms+"/"+nSms);

}

void MainJackSMS::on_comboServizio_activated(QString )
{

}

void MainJackSMS::on_comboServizio_currentIndexChanged(int index)
{
    on_TestoSMS_textChanged();
}


void MainJackSMS::on_bottoneinviomultiplo_clicked()
{
    /*if (!ui->TestoSMS->toPlainText().isEmpty()){
        InvioMultiplo *inv=new InvioMultiplo(this,ElencoServiziConfigurati,Icone,Rubrica,Opzioni,ui->TestoSMS->toPlainText());
        inv->exec();
        inv->deleteLater();
    }else{
        QMessageBox::information(this,"JackSMS","Il testo del messaggio è vuoto.");
    }
    */
}

//carica i plugin
void MainJackSMS::loadPlugins(){

#ifdef __APPLE__
    QDir pluginsDir = getJackPluginsPath();
#else
    #ifdef  WIN32
        QDir pluginsDir = QDir(qApp->applicationDirPath() + "/plugins");
    #else
        QDir pluginsDir = QDir("/opt/jacksms2/plugins");
    #endif

#endif

        foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {

            QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
                 QObject *plugin = loader.instance();
                 if (plugin) {

                     JackPluginInterface *iPlugin = dynamic_cast<JackPluginInterface *>(plugin);
                        if (iPlugin){
                              pluginsList.push_back(iPlugin);
                              //QMessageBox::information(this,"asd","asd");
                              //ui->listPlugins->addItem(new PluginListItem(0, iPlugin));
                          }
                    }
             }

        //connect(ui->listPlugins, SIGNAL(itemSelectionChanged()), this, SLOT(setPluginInfo()));

}



void MainJackSMS::DisabilitaUi(){
    ui->LabelEsito->hide();
    ui->LabelCountChars->hide();
    ui->progressBar->show();
    ui->InviaSMS->hide();
    ui->AnnullaSMS->show();
    ui->AnnullaSMS->setEnabled(true);
    ui->RubricaVeloce->setEnabled(false);
    ui->TestoSMS->setEnabled(false);
    ui->comboServizio->setEnabled(false);
    ui->NumeroDestinatario->setEnabled(false);
    ui->RicercaVeloce->setEnabled(false);
    //ui->progressBar->setValue(0);

}
void MainJackSMS::AbilitaUi(){
    ui->progressBar->hide();
    ui->LabelEsito->show();
    //ui->LabelCountChars->show();
    ui->InviaSMS->show();
    ui->AnnullaSMS->hide();
    ui->AnnullaSMS->setEnabled(false);
    ui->RubricaVeloce->setEnabled(true);
    ui->TestoSMS->setEnabled(true);
    ui->comboServizio->setEnabled(true);
    ui->NumeroDestinatario->setEnabled(true);
    ui->RicercaVeloce->setEnabled(true);
}

void MainJackSMS::TrayClicked(){
    if ((isHidden())||(isMinimized())){
        raise();
        activateWindow();
        showNormal();

    }else{
        hide();
    }




}
void MainJackSMS::ReWriteMessagesToGui(){
    ui->smsListWidget->clear();
    WriteMessagesToGui();
}
void MainJackSMS::ReWriteImToGui(){
    ui->imRicevutiWidget->clear();
    WriteImToGui();
}
void MainJackSMS::stopIm(){
    imChecker->stop();
}

void MainJackSMS::jmsActive(){
    //libJackSMS::LanguageManager *lm=libJackSMS::LanguageManager::getIstance();
    //ui->buttonStatusJms->setText(lm->getString(28));
    ui->buttonStatusJms->setText("Attivo");
    ui->buttonStatusJms->setIcon(QIcon(":/resource/jms-active.png"));
    imServiceActive=true;
}
void MainJackSMS::jmsNotActive(bool err,QString str){
    //libJackSMS::LanguageManager *lm=libJackSMS::LanguageManager::getIstance();
    if (err)
        trayIco->showMessage("JackSMS Messenger","Il servizio e' stato disattivato temporaneamente a causa di un errore.\nDettagli: "+str);


    //ui->buttonStatusJms->setText(lm->getString(30));
    ui->buttonStatusJms->setText("Disattivo");
    ui->buttonStatusJms->setIcon(QIcon(":/resource/jms-not-active.png"));
    imServiceActive=false;
}
void MainJackSMS::jmsActiving(){
    //libJackSMS::LanguageManager *lm=libJackSMS::LanguageManager::getIstance();
    //ui->buttonStatusJms->setText(lm->getString(29));
    ui->buttonStatusJms->setText("Connessione");
    ui->buttonStatusJms->setIcon(QIcon(":/resource/jms-activing.png"));
    imServiceActive=false;
}
void MainJackSMS::startIm(){

        messaggiRicevuti.clear();
        imServiceActive=false;


        imChecker=new libJackSMS::serverApi::permanentInstantMessenger(current_user_username,ui->password->text(),Opzioni);
        connect(imChecker,SIGNAL(newJMS(libJackSMS::dataTypes::logImType)),this,SLOT(checkInstantMessengerReceived(libJackSMS::dataTypes::logImType)));

        connect(imChecker,SIGNAL(serviceActive()),this,SLOT(jmsActive()));
        connect(imChecker,SIGNAL(serviceNotActive(bool,QString)),this,SLOT(jmsNotActive(bool,QString)));
        connect(imChecker,SIGNAL(serviceActiving()),this,SLOT(jmsActiving()));
        imChecker->activateServ();


}

/*
void MainJackSMS::endLogin(){
    if (loginThread->result){
       loggedIn=true;

       WriteConfiguredServicesToGui();

       WriteAddressBookToGui();

       WriteMessagesToGui();
       ui->TestoSMS->setFont(QFont(ui->TestoSMS->font().family(),Opzioni["textsize"].toInt(NULL,10),ui->TestoSMS->font().weight(),false));
       onlineLogin=true;


       if (Opzioni["receive-im"]=="yes"){
           startIm();
       }
       pingServerTimer=new QTimer();
       connect(pingServerTimer,SIGNAL(timeout()),this,SLOT(pingServer()));
       pingServerTimer->start(1000*60*40);
       enableUiAfterLogin();
       ui->widgetSchermate->setCurrentIndex(2);

       checkUpdatesThread=new threadCheckUpdates(ElencoServizi,Opzioni,signin->getSessionId());
       connect(checkUpdatesThread,SIGNAL(updatesAvailable()),this,SLOT(updatesAvailable()));
       checkUpdatesThread->start();

       if (ui->ricordaPassword->isChecked()){
           GlobalOptions["save-passwd"]="yes";
           Opzioni["save-passwd"]="yes";
           Opzioni["password"]=ui->password->text();
       }else{
           GlobalOptions["save-passwd"]="no";        
           Opzioni["save-passwd"]="no";
           Opzioni["password"]="";



       }
       libJackSMS::localApi::optionManager man("",GlobalOptions);
       man.save();
       libJackSMS::localApi::optionManager man2(current_user_directory,Opzioni);
       man2.save();

       //if (Opzioni["check-version"]=="yes"){
           libJackSMS::dataTypesApi::clientVersion thisVersion(JACKSMS_VERSION);
           if (thisVersion<signin->getVersion())
               QMessageBox::information(this,"JackSMS","E' disponibile una nuova versione di JackSMS.\nVisita www.jacksms.it");
       //}
    }else{
        loggedIn=false;
        ui->widgetSchermate->setCurrentIndex(0);
        QMessageBox::critical(this,"JackSMS",loginThread->getError());
    }
}*/

/*
void MainJackSMS::appendImToGui(){
    mapWidgetsReceivedNew.clear();
    mapWidgetsNew.clear();
    QPixmap icon_jack=ElencoServizi["40"].getIcon().pixmap(16,16);
    libJackSMS::dataTypes::logImType::const_iterator i=nuoviMessaggiRicevuti.begin();
    libJackSMS::dataTypes::logImType::const_iterator i_end=nuoviMessaggiRicevuti.end();
    for(;i!=i_end;++i){
        QString user_num_name=phone2name(i.value().getPhoneNumber());
        SmsWidget *wid=new SmsWidget(
                                     i.value().getText(),
                                     icon_jack,
                                     true,
                                     i.value().getDate().getData(),
                                     user_num_name,
                                     QString("JackSMS Messenger"),
                                     i.value().getId(),
                                     i.value().getPhoneNumber(),
                                     false
                                     );
        mapWidgetsReceived.insert(i.value().getDate().getData(),wid);
        mapWidgetsReceivedNew.insert(i.value().getDate().getData(),wid);

        SmsWidget *wid2=new SmsWidget(
                                     i.value().getText(),
                                     icon_jack,
                                     true,
                                     i.value().getDate().getData(),
                                     user_num_name,
                                     QString("JackSMS Messenger"),
                                     i.value().getId(),
                                     i.value().getPhoneNumber(),
                                     true
                                     );
        mapWidgets.insert(i.value().getDate().getData(),wid2);
        mapWidgetsNew.insert(i.value().getDate().getData(),wid2);
    }

    if (mapWidgetsReceivedNew.size()>0){
        QMultiMap<QDateTime,SmsWidget*>::ConstIterator xx=mapWidgetsReceivedNew.begin();
        QMultiMap<QDateTime,SmsWidget*>::ConstIterator xx_end=mapWidgetsReceivedNew.end();
        for(;xx!=xx_end;++xx){

            QListWidgetItem *item = new QListWidgetItem;
            item->setSizeHint(xx.value()->size());
            ui->imRicevutiWidget->insertItem(0,item);
            ui->imRicevutiWidget->setItemWidget(item, xx.value());
        }
    }
    if (mapWidgetsNew.size()>0){
        QMultiMap<QDateTime,SmsWidget*>::ConstIterator xx=mapWidgetsNew.begin();
        QMultiMap<QDateTime,SmsWidget*>::ConstIterator xx_end=mapWidgetsNew.end();
        for(;xx!=xx_end;++xx){

            QListWidgetItem *item = new QListWidgetItem;
            item->setSizeHint(xx.value()->size());
            ui->smsListWidget->insertItem(0,item);
            ui->smsListWidget->setItemWidget(item, xx.value());
        }
    }
}*/
void MainJackSMS::checkInstantMessengerReceived(libJackSMS::dataTypes::logImType jmsList){
    int section=1;
    try{
        libJackSMS::dataTypes::logImType::iterator i=jmsList.begin();
        libJackSMS::dataTypes::logImType::iterator i_end=jmsList.end();
        //warning da escludere, non può essere usato non inizializzato. è qui per la visibilità nel for
        libJackSMS::localApi::imLogSaver *manager;
        bool save=false;
        if( Opzioni["save-local"]=="yes"){
            save=true;
            manager=new libJackSMS::localApi::imLogSaver(current_user_directory);
        }else{
            Opzioni["save-local"]="no";
        }
        for(;i!=i_end;++i){
                if( save){
                    manager->setMessage(i.value());
                    manager->appendToLogFile();
                    i.value().setId(manager->getSavedId());
                }else{
                    //generating temporary id
                    i.value().setId(QString::number(Messaggi.size()+1));
                }

                QMyMessage msg;
                msg.setAccountId(QString::number(0));
                msg.setData( i.value().getDate().getData());
                msg.setMessage(i.value().getText());
                msg.setId(i.value().getId());
                msg.setIsReceived(true);
                msg.setPhone(i.value().getPhoneNumber());
                Messaggi.push_back(msg);
                msg.setReaded(false);
                MessaggiRicevuti.push_back(msg);
                countReceivedUnreaded++;

                //relays to listening plugins the received message
                //jphi->emitJIMreceived(*i);

        }
        section++;

        /*********************append im to gui*********************************/

        QMultiMap<QDateTime,SmsWidget*> mapWidgetsReceivedNew;

        QMultiMap<QDateTime,SmsWidget*> mapWidgetsNew;

        QPixmap icon_jack=ElencoServizi["40"].getIcon().pixmap(16,16);
        i=jmsList.begin();
        i_end=jmsList.end();
        for(;i!=i_end;++i){
            QString user_num_name=phone2name(i.value().getPhoneNumber());
            SmsWidget *wid=new SmsWidget(
                                         i.value().getText(),
                                         icon_jack,
                                         true,
                                         i.value().getDate().getData(),
                                         user_num_name,
                                         QString("JackSMS Messenger"),
                                         i.value().getId(),
                                         i.value().getPhoneNumber(),
                                         false
                                         );
            mapWidgetsReceived.insert(i.value().getDate().getData(),wid);
            mapWidgetsReceivedNew.insert(i.value().getDate().getData(),wid);

            SmsWidget *wid2=new SmsWidget(
                                         i.value().getText(),
                                         icon_jack,
                                         true,
                                         i.value().getDate().getData(),
                                         user_num_name,
                                         QString("JackSMS Messenger"),
                                         i.value().getId(),
                                         i.value().getPhoneNumber(),
                                         true
                                         );
            mapWidgets.insert(i.value().getDate().getData(),wid2);
            mapWidgetsNew.insert(i.value().getDate().getData(),wid2);
        }
        section++;

        if (mapWidgetsReceivedNew.size()>0){
            QMultiMap<QDateTime,SmsWidget*>::ConstIterator xx=mapWidgetsReceivedNew.begin();
            QMultiMap<QDateTime,SmsWidget*>::ConstIterator xx_end=mapWidgetsReceivedNew.end();
            for(;xx!=xx_end;++xx){

                QListWidgetItem *item = new QListWidgetItem;
                item->setSizeHint(xx.value()->size());
                ui->imRicevutiWidget->insertItem(0,item);
                ui->imRicevutiWidget->setItemWidget(item, xx.value());
            }
        }
        section++;

        if (mapWidgetsNew.size()>0){
            QMultiMap<QDateTime,SmsWidget*>::ConstIterator xx=mapWidgetsNew.begin();
            QMultiMap<QDateTime,SmsWidget*>::ConstIterator xx_end=mapWidgetsNew.end();
            for(;xx!=xx_end;++xx){

                QListWidgetItem *item = new QListWidgetItem;
                item->setSizeHint(xx.value()->size());
                ui->smsListWidget->insertItem(0,item);
                ui->smsListWidget->setItemWidget(item, xx.value());
            }
        }
        /*****************************end append im to gui***************************/


        section++;

        setTrayIcon();
        section++;
        if (jmsList.size()==1)
            trayIco->showMessage("JackSMS Messenger","Ricevuto un nuovo JMS");
        else
            trayIco->showMessage("JackSMS Messenger","Ricevuti "+QString::number(jmsList.size())+" nuovi JMS");
        //jmsList.clear();
        section++;
    }catch(...){
        QMessageBox::critical(this,"JackSMS","JackSMS ha rilevato un errore grave durante la procedura di aggiornamento dei messaggi JMS ricevuti.\nDettagli: unknown error, section: "+QString::number(section));
    }

}
void MainJackSMS::errorUpdates(QString err){
    QMessageBox::critical(this,"JackSMS","si e' verificato un errore grave durante l'aggiornamnto dei servizi.\nErrore: "+err);
}
void MainJackSMS::countdownToGui(){
    countdownToGuiCount--;
    if (countdownToGuiCount==0){
       this->WriteAddressBookToGui();
       this->WriteConfiguredServicesToGui();
       this->WriteMessagesToGui();

       enableUiAfterLogin();
       ui->widgetSchermate->setCurrentIndex(2);
       updateChecker=new libJackSMS::serverApi::updateServicesManager(this->current_login_id,Opzioni,ElencoServizi);
       connect(updateChecker,SIGNAL(updatesAvailable(libJackSMS::dataTypes::servicesType,QString,QString)),this,SLOT(updatesAvailable(libJackSMS::dataTypes::servicesType,QString,QString)));
       connect(updateChecker,SIGNAL(criticalError(QString)),this,SLOT(errorUpdates(QString)));
       updateChecker->checkUpdadates();
       countdownToGuiCount=COUNTDOWNTOGUICOUNTDEFINE;
    
    }

}
void MainJackSMS::setTrayIcon(){
    if (countReceivedUnreaded>0){
        trayIco->setIcon(QIcon(":/resource/tray_icon_notify.png"));
    }else{
        trayIco->setIcon(QIcon(":/resource/tray_icon.png"));
    }
}
void MainJackSMS::messagesLoaded(QList<QMyMessage> msgs){
    Messaggi=msgs;
    countdownToGui();
}
void MainJackSMS::servicesLoaded(libJackSMS::dataTypes::servicesType s){
    ElencoServizi=s;
    libJackSMS::serverApi::synchronizeVariables(ElencoServiziConfigurati,ElencoServizi);

    loaderMessages=new messageLoader(current_user_directory);
    connect(loaderMessages,SIGNAL(messagesLoaded(QList<QMyMessage>)),this,SLOT(messagesLoaded(QList<QMyMessage>)));
    loaderMessages->loadMessages();
    countdownToGui();


}
void MainJackSMS::optionsLoaded(libJackSMS::dataTypes::optionsType op){
    Opzioni=op;

    ui->TestoSMS->setFont(QFont(ui->TestoSMS->font().family(),Opzioni["textsize"].toInt(NULL,10),ui->TestoSMS->font().weight(),false));

    if (Opzioni["receive-im"]=="yes"){
        startIm();
    }


    if (ui->ricordaPassword->isChecked()){
        GlobalOptions["save-passwd"]="yes";
        Opzioni["save-passwd"]="yes";
        Opzioni["password"]=ui->password->text();
    }else{
        GlobalOptions["save-passwd"]="no";
        Opzioni["save-passwd"]="no";
        Opzioni["password"]="";



    }
    libJackSMS::localApi::optionManager man("",GlobalOptions);
    man.save();
    libJackSMS::localApi::optionManager man2(current_user_directory,Opzioni);
    man2.save();
}
void MainJackSMS::loginSuccess(QString sessionId){

    libJackSMS::localApi::userDirectoryManager man(current_user_username);
    if (!man.userDirectoryExists()){
        man.createUser();
        current_user_directory=man.getUserDir();
    }


    current_login_id=sessionId;

    xmlLoader=new libJackSMS::localApi::xmlLoader(current_user_directory);
    connect(xmlLoader,SIGNAL(servicesLoaded(libJackSMS::dataTypes::servicesType)),this,SLOT(servicesLoaded(libJackSMS::dataTypes::servicesType)));
    connect(xmlLoader,SIGNAL(optionsLoaded(libJackSMS::dataTypes::optionsType)),this,SLOT(optionsLoaded(libJackSMS::dataTypes::optionsType)));

    xmlLoader->loadServices();
    xmlLoader->loadOptions();

    loggedIn=true;

    pingator=new libJackSMS::serverApi::pingator(current_login_id,Opzioni);
    connect(pingator,SIGNAL(pinged()),this,SLOT(serverPinged()));
    pingator->launchPing();
    countdownToGui();








}
void MainJackSMS::serverPinged(){
    std::cout<<"[PING] pingato server"<<std::endl;
}
void MainJackSMS::updatesAvailable(libJackSMS::dataTypes::servicesType serv,QString xml,QString msg){
    QMessageBox::information(this,"Aggiornamenti JackSMS",msg);
    ElencoServizi=serv;

}
void MainJackSMS::newVersionAvailable(){

    //if (Opzioni["check-version"]=="yes"){
        QMessageBox::information(this,"JackSMS","E' disponibile una nuova versione di JackSMS.\nVisita www.jacksms.it");
    //}
}

void MainJackSMS::accountsReceived(libJackSMS::dataTypes::configuredServicesType accounts){
    ElencoServiziConfigurati=accounts;
    libJackSMS::localApi::statsManager l(current_user_directory);
    l.loadStats(ElencoServiziConfigurati);
    countdownToGui();


}
void MainJackSMS::phoneBookReceived(libJackSMS::dataTypes::phoneBookType r){
    Rubrica=r;
    countdownToGui();


}
void MainJackSMS::loginStarted(){
    ui->widgetSchermate->setCurrentIndex(1);
}
void MainJackSMS::loginFailed(QString err){
    ui->widgetSchermate->setCurrentIndex(0);
    QMessageBox::critical(this,"JackSMS",err);
    countdownToGuiCount=COUNTDOWNTOGUICOUNTDEFINE;
}

void MainJackSMS::on_loginButton_clicked()
{

    if (ui->password->text().isEmpty()){
        QMessageBox::critical(this,"JackSMS","La tua password non puo' essere vuota!");
        return;
    }
    if (ui->username->currentText().isEmpty()){
        QMessageBox::critical(this,"JackSMS","Il tuo username non puo' essere vuoto!");
        return;
    }


    current_user_directory=finder->getDataDirectory(ui->username->currentText());
    current_user_username=ui->username->currentText();

    loginClient=new libJackSMS::serverApi::login(current_user_username,ui->password->text(),Opzioni);
    connect(loginClient,SIGNAL(loginSuccess(QString)),this,SLOT(loginSuccess(QString)));
    connect(loginClient,SIGNAL(newVersionAvailable()),this,SLOT(newVersionAvailable()));
    connect(loginClient,SIGNAL(accountsReceived(libJackSMS::dataTypes::configuredServicesType)),this,SLOT(accountsReceived(libJackSMS::dataTypes::configuredServicesType)));
    connect(loginClient,SIGNAL(phoneBookReceived(libJackSMS::dataTypes::phoneBookType)),this,SLOT(phoneBookReceived(libJackSMS::dataTypes::phoneBookType)));
    connect(loginClient,SIGNAL(loginFailed(QString)),this,SLOT(loginFailed(QString)));
    connect(loginClient,SIGNAL(loginStarted()),this,SLOT(loginStarted()));
    loginClient->doLogin();

}

void MainJackSMS::on_username_currentIndexChanged(int index)
{
    QString password=finder->getPassword(ui->username->currentText());
    ui->password->setText(password);

}


void MainJackSMS::disableUibeforeLogin(){
    //ui->actionOpzioni->setEnabled(false);
    ui->actionStatistiche->setEnabled(false);
    ui->actionCrea_backup_configurazione->setEnabled(false);
    ui->actionImporta_Backup->setEnabled(false);
    ui->actionLogout->setEnabled(false);

}
void MainJackSMS::enableUiAfterLogin(){
    ui->actionLogout->setEnabled(true);
    //ui->actionOpzioni->setEnabled(true);
    ui->actionStatistiche->setEnabled(true);
    ui->actionCrea_backup_configurazione->setEnabled(true);
    ui->actionImporta_Backup->setEnabled(true);
}
void MainJackSMS::on_smsListWidget_currentItemChanged(QListWidgetItem* current, QListWidgetItem* previous)
{

    if (current!=NULL){
        SmsWidget *www=static_cast<SmsWidget *>(ui->smsListWidget->itemWidget(current));
        if (www->isReceived()){
            ui->InoltraButton->setVisible(false);
            ui->RispondiButton->setVisible(true);
            ui->CitaButton->setVisible(true);
        }else{
            ui->InoltraButton->setVisible(true);
            ui->RispondiButton->setVisible(false);
            ui->CitaButton->setVisible(false);
        }
    }


}

void MainJackSMS::on_RubricaVeloce_currentItemChanged(QListWidgetItem* item, QListWidgetItem*old)
{
    if (!invioInCorso){



        if (item!=NULL){
            contactWidgetFastBook *it=static_cast<contactWidgetFastBook*>(ui->RubricaVeloce->itemWidget(item));
            ui->labelNomeDest->setText(it->getName());

            QString accountId=it->getContactId();
            {
                libJackSMS::dataTypes::phoneBookType::const_iterator i=Rubrica.begin();
                libJackSMS::dataTypes::phoneBookType::const_iterator i_end=Rubrica.end();
                while(i!=i_end){
                    if (i->getId()==accountId){
                        ui->NumeroDestinatario->setText(i->getPhone().toString());
                        accountId=i->getAccount();
                        break;
                    }

                    ++i;
                }
            }
            if (Opzioni["set-account"]=="yes"){

                libJackSMS::dataTypes::configuredServicesType::const_iterator i=ElencoServiziConfigurati.find(accountId);

                if (i!=ElencoServiziConfigurati.end()){
                    int index=ui->comboServizio->findText(i.value().getName());
                    if (index>=0)
                        ui->comboServizio->setCurrentIndex(index);
                }


            }
        }
    }
}



void MainJackSMS::deleteAccountKo(){
    ui->EliminaServizioButton->setEnabled(true);
    ui->labelSpinDelAccount->hide();
    QMessageBox::critical(this,"JackSMS","Errore durante l'eliminazione dell'account.");
}
void MainJackSMS::deleteAccountOk(QString id){
    ElencoServiziConfigurati.remove(id);
    ui->EliminaServizioButton->setEnabled(true);
    ui->labelSpinDelAccount->hide();
    ReWriteConfiguredServicesToGui();
}

void MainJackSMS::on_actionLogout_triggered()
{
    loggedIn=false;
    loginClient->deleteLater();
    loaderMessages->deleteLater();
    pingator->deleteLater();
    Rubrica.clear();
    if (Opzioni["receive-im"]=="yes"){
       imChecker->stop();
       imChecker->deleteLater();
    }

    ElencoServizi.clear();
    ElencoServiziConfigurati.clear();
    if (GlobalOptions["save-passwd"]!="yes"){
        ui->password->clear();
    }

    if (updateChecker->isRunning()){
        updateChecker->abort();

        //updateChecker->deleteLater();

    }
    pingator->~QThread();
    ui->tabWidget->setCurrentIndex(0);
    Messaggi.clear();
    Opzioni.clear();
    Opzioni=GlobalOptions;
    ui->RubricaVeloce->clear();
    ui->comboServizio->clear();
    ui->smsListWidget->clear();
    ui->rubricaListWidget->clear();
    ui->listServiziConfigurati->clear();
    disableUibeforeLogin();

    ui->widgetSchermate->setCurrentIndex(0);
}

void MainJackSMS::on_TextRapidServizi_textEdited(QString )
{
    QString text=ui->TextRapidServizi->text().toUpper();
    ui->listServiziConfigurati->clear();
    libJackSMS::dataTypes::configuredServicesType::const_iterator i=ElencoServiziConfigurati.begin();
    libJackSMS::dataTypes::configuredServicesType::const_iterator i_end=ElencoServiziConfigurati.end();


    for(;i!=i_end;++i){
        if (i.value().getName().toUpper().contains(text)){
            QIcon ico=this->ElencoServizi[i.value().getService()].getIcon();
            accountWidget *ww=new accountWidget(
                   i.value().getId(),
                   i.value().getName(),
                   ico.pixmap(16,16)
                   );
            QListWidgetItem *item = new QListWidgetItem;
            item->setSizeHint(ww->size());
            ui->listServiziConfigurati->addItem(item);
            ui->listServiziConfigurati->setItemWidget(item, ww);
        }
    }
}




void MainJackSMS::on_RispondiIm_clicked()
{
 QList<QListWidgetItem *> ls=ui->imRicevutiWidget->selectedItems();



    if (!ls.isEmpty()){

              QListWidgetItem *it=ls.front();
              SmsWidget * ww=static_cast< SmsWidget*>(ui->imRicevutiWidget->itemWidget(it));
              QString phone=ww->getPhoneNum().toString();
              ui->NumeroDestinatario->setText(phone);
              ui->TestoSMS->setText("");
              libJackSMS::dataTypes::phoneBookType::const_iterator a=Rubrica.begin();
              libJackSMS::dataTypes::phoneBookType::const_iterator a_end=Rubrica.end();
              ui->labelNomeDest->setText("");
              for (;a!=a_end;++a){
                  if (a.value().getPhone().toString()==phone){
                      ui->labelNomeDest->setText(a.value().getName());
                      break;
                  }
              }

              libJackSMS::dataTypes::configuredServicesType::const_iterator x=ElencoServiziConfigurati.begin();
              libJackSMS::dataTypes::configuredServicesType::const_iterator x_end=ElencoServiziConfigurati.end();
              for(;x!=x_end;x++){
                  if (x.value().getService()=="40"){
                      int index=ui->comboServizio->findText(x.value().getName());
                      ui->comboServizio->setCurrentIndex(index);
                      break;
                  }
              }
               ui->tabWidget->setCurrentIndex(0);

             

    }
}

void MainJackSMS::on_comboServizio_currentIndexChanged(QString currentServiceName)
{

    if (!currentServiceName.isEmpty()){
        libJackSMS::dataTypes::configuredServicesType::const_iterator x=ElencoServiziConfigurati.begin();
        libJackSMS::dataTypes::configuredServicesType::const_iterator x_end=ElencoServiziConfigurati.end();
        for(;x!=x_end;x++){
            if (x.value().getName()==currentServiceName){
                QString serviceId=x.value().getService();
                libJackSMS::dataTypes::servicesType::const_iterator i=ElencoServizi.find(serviceId);
                currentMaxLength=i.value().getIntMaxLength();
                currentSingleLength=i.value().getIntSmsDivisor();
                break;
            }
        }
        on_TestoSMS_textChanged();
    }

}




void MainJackSMS::on_RubricaVeloce_itemClicked(QListWidgetItem* item)
{
    on_RubricaVeloce_currentItemChanged(item,NULL);
}

void MainJackSMS::on_RicercaVeloceIM_2_textChanged(QString )
{



    QString text=ui->RicercaVeloceIM_2->text().toUpper();
    ui->imRicevutiWidget->clear();
    mapWidgets.clear();

    QPixmap icon_jack=ElencoServizi["40"].getIcon().pixmap(16,16);

    types::QMessageListType::const_iterator i=MessaggiRicevuti.begin();
    types::QMessageListType::const_iterator i_end=MessaggiRicevuti.end();
    for(;i!=i_end;++i){
        QString user_num_name=phone2name(i->getPhone());
        if (user_num_name.toUpper().contains(text) || i->getMessage().toUpper().contains(text)){
            SmsWidget *wid=new SmsWidget(
                                     i->getMessage(),
                                     icon_jack,
                                     true,
                                     i->getData(),
                                     user_num_name,
                                     QString("JackSMS Messenger"),
                                     i->getId(),
                                     i->getPhone(),
                                     i->getReaded()
                                     );
            mapWidgets.insert(i->getData(),wid);
        }
    }
    if (mapWidgets.size()>0){
        QMultiMap<QDateTime,SmsWidget*>::ConstIterator xx=mapWidgets.end();
        QMultiMap<QDateTime,SmsWidget*>::ConstIterator xx_end=mapWidgets.begin();
        do{
            --xx;
            QListWidgetItem *item = new QListWidgetItem;
            item->setSizeHint(xx.value()->size());
            ui->imRicevutiWidget->addItem(item);
            ui->imRicevutiWidget->setItemWidget(item, xx.value());
        }while(xx!=xx_end);
    }
}

void MainJackSMS::on_password_returnPressed()
{
    this->ui->loginButton->click();
}

void MainJackSMS::on_actionTutti_i_messaggi_triggered()
{
    QDesktopServices::openUrl(QUrl("http://www.jacksms.it/area-personale.html", QUrl::TolerantMode));
}

void MainJackSMS::on_actionStatistiche_triggered()
{
    StatsDialog *sd=new StatsDialog(Opzioni,ElencoServiziConfigurati,ElencoServizi,this);
    sd->exec();
    sd->deleteLater();
}

void MainJackSMS::on_actionPlugins_triggered()
{
   pluginDialog *pd=new pluginDialog(pluginsList);
   pd->exec();
   pd->deleteLater();
}

void MainJackSMS::on_imRicevutiWidget_itemClicked(QListWidgetItem* current)
{
    if (current!=NULL){
        SmsWidget * w=static_cast<SmsWidget*>(ui->imRicevutiWidget->itemWidget(current));
        if (!w->isReaded()){
            if (countReceivedUnreaded>0)
                countReceivedUnreaded--;
            w->setReaded(true);

            types::QMessageListType::iterator i=MessaggiRicevuti.begin();
            types::QMessageListType::iterator i_end=MessaggiRicevuti.end();
            for(;i!=i_end;++i){
                if (i->getId()==w->getId()){
                    i->setReaded(true);
                    break;
                }
            }
            setTrayIcon();
        }


    }
}

void MainJackSMS::on_buttonStatusJms_clicked()
{
    if (imServiceActive)
        imChecker->stop();
    else
        startIm();
}

void MainJackSMS::on_actionRicarica_servizi_triggered()
{
    xmlReLoader=new libJackSMS::localApi::xmlLoader(current_user_directory);
    connect(xmlReLoader,SIGNAL(servicesLoaded(libJackSMS::dataTypes::servicesType)),this,SLOT(servicesReLoaded(libJackSMS::dataTypes::servicesType)));


    xmlReLoader->loadServices();



}
void MainJackSMS::servicesReLoaded(libJackSMS::dataTypes::servicesType s){
    ElencoServizi=s;
    QMessageBox::information(this,"JackSMS","Servizi ricaricati");
    xmlReLoader->deleteLater();

}

void MainJackSMS::on_actionElimina_cookies_triggered()
{
    libJackSMS::localApi::cookieManager m(current_user_directory);
    if (m.deleteCookies())
        QMessageBox::information(this,"JackSMS","Cookies eliminati.");
    else
        QMessageBox::critical(this,"JackSMS","Errore durante l'eliminazione dei cookie.");
}
