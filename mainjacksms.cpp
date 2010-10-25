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
#include "libjacksms/libJackSMS.h"
#include "smswidget.h"
#include "contactwidget.h"
#include "editaccountdialog.h"
#include "contactwidgetfastbook.h"
#include "accountwidget.h"
#include "Types.h"
#include "statsdialog.h"
#include "plugindialog.h"
#include "qlabelresult.h"
#include "messageloader.h"


/*#include <phonon/audiooutput.h>
#include <Phonon/MediaSource>
#include <phonon/mediaobject.h>
*/

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
    popupJms=false;
    imChecker=NULL;
    updateChecker=NULL;
    messageType=TYPE_SMS;
    ui->setupUi(this);
    ui->widgetSchermate->setCurrentIndex(0);
    ui->tabWidget->setCurrentIndex(0);
    loggedIn=false;
    recipientAutoEdited=false;
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
    ui->destinatariListWidget->setPalette(p);
    
    ui->LabelEsito->~QWidget();
    ui->LabelEsito=new QLabelResult(this);
    ui->horizontalLayout_16->addWidget(ui->LabelEsito);
    ui->LabelEsito->hide();
    ui->LabelEsito->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Minimum);
    connect(ui->LabelEsito,SIGNAL(clicked()),this,SLOT(popupInvio()));

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

    //nascondo alcuni bottoni nella schermata dei messaggi
    ui->InoltraButton->setVisible(false);
    ui->CitaButton->setVisible(false);

    /////////nascondo alcune cose che ancora non sono state implementate... da rimuovere poi...
    //ui->ModificaServizioButton->hide();
    //ui->RubricaModifica->hide();
    ui->tabWidget->removeTab(5);
    ui->tabWidget->removeTab(5);
    //ui->bottoneinviomultiplo->hide();

    //ui->label_16->hide();
    //ui->numArchivio->hide();



    ui->actionImporta_Backup->setVisible(false);
    ui->actionCrea_backup_configurazione->setVisible(false);

    ui->actionPlugins->setVisible(false);

    //carica i plugin
    loadPlugins();
    jphi = new JackPluginHostInterface();

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

    /*initialXmlLoader=new libJackSMS::localApi::xmlLoader("");
    connect(initialXmlLoader,SIGNAL(optionsLoaded(libJackSMS::dataTypes::optionsType)),this,SLOT(initialOptionsLoaded(libJackSMS::dataTypes::optionsType)));
    initialXmlLoader->loadOptions();
*/

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
    firstResize=true;
    connect(&resizeTimer,SIGNAL(timeout()),this,SLOT(resized()));


    /*questo carica le opzioni globali*/
    libJackSMS::localApi::synchronousOptionLoader l("");
    if(l.load(GlobalOptions)){
        Opzioni=GlobalOptions;

        /*l'utente di default potrebbe non essere impostato, caso tipico del primo avvio*/
        libJackSMS::dataTypes::optionsType::const_iterator i= GlobalOptions.find("default-user");
        if (i!=GlobalOptions.end()) {
            bool tointcheck;
            int userindex = i.value().toInt(&tointcheck, 10);
            if (tointcheck)
                ui->username->setCurrentIndex(userindex);

        }

        if (GlobalOptions["save-passwd"]=="yes") {
            ui->ricordaPassword->setChecked(true);
            ui->loginButton->setDefault(true);
            ui->loginButton->setFocus();

            if (GlobalOptions["auto-login"]=="yes"){
                ui->autoLogin->setChecked(true);
                ui->loginButton->animateClick();
            }

        } else {
            ui->username->setFocus();
        }
        i= GlobalOptions.find("window-height");
        if (i!=GlobalOptions.end()){
            bool ok;
            int h=i.value().toInt(&ok,10);
            bool ok2;
            int w=GlobalOptions["window-width"].toInt(&ok2,10);
            if (ok&&ok2)
               this->resize(w,h);
        }
        i= GlobalOptions.find("window-maximized");
        if (i!=GlobalOptions.end()){
            if (i.value()=="yes")
                showMaximized();

        }

        firstResize=false;

        connect( ui->username->lineEdit(), SIGNAL(returnPressed()), this, SLOT(username_returnPressed()));

    }

}

void MainJackSMS::resized(){
    resizeTimer.stop();

    if (isMaximized())
        GlobalOptions["window-maximized"] ="yes";
    else{
        GlobalOptions["window-maximized"] ="no";
        GlobalOptions["window-height"]=QString::number(size().height());
        GlobalOptions["window-width"]=QString::number(size().width());
    }


    libJackSMS::localApi::optionManager man("",GlobalOptions);
    man.save();
}
void MainJackSMS::resizeEvent ( QResizeEvent * s){
    if (!firstResize){
        resizeTimer.stop();
        resizeTimer.start(1000);
    }
}
MainJackSMS::~MainJackSMS()
{


    if (updateChecker!=NULL)
        delete updateChecker;

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




            ui->buttonNoAccount->setText(lm->getString("4"));
            ui->buttonLostPassword->setText(lm->getString("5"));
            ui->label_19->setText(lm->getString("7"));
            ui->ricordaPassword->setText(lm->getString("6"));
            ui->autoLogin->setText(lm->getString("31"));
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

void MainJackSMS::gestiscimenuSingolo(bool starting){
    if(invioMultiplo || starting){
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/resource/ico_contact2.png"), QSize(), QIcon::Normal, QIcon::Off);
        ui->label_3->hide();
        ui->destinatariListWidget->hide();

        ui->bottoneinviomultiplo->setIcon(icon1);
        ui->bottoneinviomultiplo->setIconSize(QSize(12, 12));
        ui->label->setText("Destinatario");

        invioMultiplo = false;
        if(usaAssociatiPresent)
            ReWriteConfiguredServicesToGui();
    }else{
        invioMultiplo = false;
    }
}
void MainJackSMS::gestiscimenuMultiplo(bool starting){
    if(!invioMultiplo || starting){
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/resource/ico_contact.png"), QSize(), QIcon::Normal, QIcon::Off);
        ui->label_3->show();
        ui->destinatariListWidget->show();

        ui->bottoneinviomultiplo->setIcon(icon1);
        ui->bottoneinviomultiplo->setIconSize(QSize(12, 12));
        ui->label->setText("Destinatari");

        invioMultiplo=true;
        if(!usaAssociatiPresent)
            ReWriteConfiguredServicesToGui();
    }else{
        invioMultiplo=true;
    }
}

void MainJackSMS::showContactByTypeInFastAbook(){
    ui->RubricaVeloce->clear();

    int section=1;
    try{
        libJackSMS::dataTypes::phoneBookType::const_iterator i=Rubrica.begin();
        libJackSMS::dataTypes::phoneBookType::const_iterator i_end=Rubrica.end();

        QMultiMap<QString,contactWidgetFastBook*> fastList;
        QString filter=ui->RicercaVeloce->text();
        for(;i!=i_end;++i){
            if (i->getName().contains(filter,Qt::CaseInsensitive)){
                QIcon ico;
                libJackSMS::dataTypes::configuredServicesType::const_iterator x=ElencoServiziConfigurati.find(i->getAccount());
                if (x==ElencoServiziConfigurati.end()){
                    ico=QIcon(":/resource/ico_contact.png");

                }else{
                    ico=ElencoServizi[x.value().getService()].getIcon();


                }
                contactWidgetFastBook *ww=new contactWidgetFastBook(i.value(),ico.pixmap(16,16));
                if ((i->getCanReceiveJms()) && (messageType==TYPE_JMS))
                    fastList.insert(i->getName().toUpper(),ww);
                else if ((!i->getPhone().getVirtual()) && (messageType==TYPE_SMS))
                    fastList.insert(i->getName().toUpper(),ww);
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
        QMessageBox::critical(this,"JackSMS","JackSMS ha rilevato un errore grave durante la procedura showContactByTypeInFastAbook().\nDettagli: unknown error, section: "+QString::number(section));
    }
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

                }
                contactWidgetFastBook *ww=new contactWidgetFastBook(i.value(),ico.pixmap(16,16));
                /*contactWidgetFastBook *ww=new contactWidgetFastBook(
                       i->getId(),
                       i->getName(),
                       i->getPhone().toString(),
                       ico.pixmap(16,16),
                       i->getAccount()
                       );*/

                if ((i->getCanReceiveJms()) && (messageType==TYPE_JMS))
                    fastList.insert(i->getName().toUpper(),ww);
                else if ((!i->getPhone().getVirtual()) && (messageType==TYPE_SMS))
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

    mapWidgetsAccount.clear();
    try{
        libJackSMS::dataTypes::configuredServicesType::const_iterator i=ElencoServiziConfigurati.begin();
        libJackSMS::dataTypes::configuredServicesType::const_iterator i_end=ElencoServiziConfigurati.end();
        for(;i!=i_end;++i){
            if (i.value().getId()!="0"){
                QIcon ico=ElencoServizi[i.value().getService()].getIcon();
                accountWidget *ww=new accountWidget(i.value().getId(),i.value().getName(),ico.pixmap(16,16));

                mapWidgetsAccount.insert(i.value().getName().toUpper(),ww);
            }
        }

       QMap<QString,accountWidget*>::ConstIterator xx=mapWidgetsAccount.begin();
       QMap<QString,accountWidget*>::ConstIterator xx_end=mapWidgetsAccount.end();
       for(;xx!=xx_end;++xx){

           QListWidgetItem *item = new QListWidgetItem;
           item->setSizeHint(xx.value()->size());
           ui->listServiziConfigurati->addItem(item);
           ui->listServiziConfigurati->setItemWidget(item, xx.value());
           QString sa=ElencoServiziConfigurati[xx.value()->getAccountId()].getStat("sent-all");
           if (sa=="0")
               sa=xx.value()->getName();
           else
               sa=xx.value()->getName()+" ["+sa+"]";
           ui->comboServizio->addItem(xx.value()->getIcon(),sa,xx.value()->getName());

       }

       if(invioMultiplo){
           ui->comboServizio->insertItem(0, QIcon(":/resource/freccia.png"), "Usa Account Associati", "Usa Account Associati");
           ui->comboServizio->insertSeparator(1);
           ui->comboServizio->setCurrentIndex(0);
           usaAssociatiPresent = true;
       }else{
           usaAssociatiPresent = false;
       }

    }catch(...){
        QMessageBox::critical(this,"JackSMS","JackSMS ha rilevato un errore grave durante la procedura WriteConfiguredServicesToGui().\nDettagli: unknown error, section: "+QString::number(section));
    }

}

void MainJackSMS::updateAccountCountComboBox(QString id){

    libJackSMS::dataTypes::configuredServicesType::const_iterator i=ElencoServiziConfigurati.begin();
    libJackSMS::dataTypes::configuredServicesType::const_iterator i_end=ElencoServiziConfigurati.end();
    for(;i!=i_end;++i){
        if (i.value().getId()==id){
            QString name=i.value().getName();
            int index=ui->comboServizio->findData(name,Qt::UserRole);
            ui->comboServizio->setItemText(index,name+" ["+ElencoServiziConfigurati[id].getStat("sent-all")+"]");
            break;

        }
    }

}

QString MainJackSMS::phone2name(const libJackSMS::dataTypes::phoneNumber &_number){
    QString user_num_name=_number.getAltName();
    static QMap<QString,QString> buf;

    try{
        if (_number.getIsValid()){
            QMap<QString,QString>::iterator i=buf.find(_number.toString());
            if (i!=buf.end()){
                user_num_name=i.value();
            }else{
                user_num_name=_number.toString();
                libJackSMS::dataTypes::phoneBookType::const_iterator x=Rubrica.begin();
                libJackSMS::dataTypes::phoneBookType::const_iterator x_end=Rubrica.end();
                bool found=false;
                for(;x!=x_end;++x){

                    if(x->getPhone().toString()==user_num_name){
                        user_num_name=x->getName();
                        found=true;
                        buf[_number.toString()]=user_num_name;
                        break;
                    }

                }
                if (!found){
                    if (_number.getAltName()!=""){
                        user_num_name=user_num_name+QString(" (")+_number.getAltName()+QString(")");
                    }
                }
            }
        }
    }catch(...){


    }
    return user_num_name;
}


void MainJackSMS::WriteMessagesToGui(bool limit100){

    int section=1;
    try{
        ui->smsListWidget->clear();
        mapWidgets.clear();


        types::QMessageListType::const_iterator i=Messaggi.begin();
        types::QMessageListType::const_iterator i_end=Messaggi.end();

        for(;i!=i_end;++i){
            QString user_num_name=phone2name(i->getPhone());
            if (i->getIsReceived()){

                SmsWidget *wid=new SmsWidget(*i,icon_jack,true);
                mapWidgets.insert(i->getData(),wid);
            }else{
                QPixmap ico=ElencoServizi[i->getServiceId()].getIcon().pixmap(16,16);
                SmsWidget *wid=new SmsWidget(*i,ico,false);
                mapWidgets.insert(i->getData(),wid);
            }
         }
         section++;
         if (mapWidgets.size()>0){
            QMultiMap<QDateTime,SmsWidget*>::ConstIterator xx=mapWidgets.end();
            QMultiMap<QDateTime,SmsWidget*>::ConstIterator xx_end=mapWidgets.begin();

            if(limit100){
                for(int c=0; c<100; c++){
                    --xx;
                    QListWidgetItem *item = new QListWidgetItem;
                    item->setSizeHint(xx.value()->getSize());
                    ui->smsListWidget->addItem(item);
                    ui->smsListWidget->setItemWidget(item, xx.value());
                    if(xx==xx_end)
                        break;
                }
            }else{
                do{
                    --xx;
                    QListWidgetItem *item = new QListWidgetItem;
                    item->setSizeHint(xx.value()->getSize());
                    ui->smsListWidget->addItem(item);
                    ui->smsListWidget->setItemWidget(item, xx.value());
                }while(xx!=xx_end);
            }
        }

     }catch(...){
         QMessageBox::critical(this,"JackSMS","JackSMS ha rilevato un errore grave durante la procedura WriteMessagesToGui().\nDettagli: unknown error, section: "+QString::number(section));
     }
}
void MainJackSMS::WriteImToGui(){

    ui->imRicevutiWidget->clear();
    mapWidgetsReceived.clear();


    types::QMessageListType::const_iterator i=MessaggiRicevuti.begin();
    types::QMessageListType::const_iterator i_end=MessaggiRicevuti.end();
    for(;i!=i_end;++i){
        QString user_num_name=phone2name(i->getPhone());
        SmsWidget *wid=new SmsWidget(*i,icon_jack,true);
        /*SmsWidget *wid=new SmsWidget(
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
        */this->mapWidgetsReceived.insert(i->getData(),wid);

    }

     if (mapWidgetsReceived.size()>0){
        QMultiMap<QDateTime,SmsWidget*>::ConstIterator xx=mapWidgetsReceived.end();
        QMultiMap<QDateTime,SmsWidget*>::ConstIterator xx_end=mapWidgetsReceived.begin();
        do{
            --xx;
            QListWidgetItem *item = new QListWidgetItem;
            /* QSize s=xx.value()->size();
             s.setHeight(s.height()/2+40);
             item->setSizeHint(s);*/
             item->setSizeHint(xx.value()->getSize());
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
void MainJackSMS::displayCaptcha(QByteArray data/*,QSemaphore* sem*/){
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
        if (Opzioni["display-captcha-popup"]=="yes"){
            popupJms=false;
            trayIco->showMessage("JackSMS","JackSMS richiede l'inserimento del codice ottico.",QSystemTrayIcon::Information);

        }
        CaptchaDialog *dial=new CaptchaDialog(data,QString("100%"),this);
        dial->exec();

        this->smsSender->continueSend(dial->result);
        dial->deleteLater();
    }catch (QString s){
        QMessageBox::critical(this,"JackSMS","Si e' verificato un errore.\nImpossibile caricare l'immagine captcha. L'invio verra' interrotto.");
        this->invioFallito("Errore durante il caricamento dell'immagine capptcha.");
    }

    //sem->release(1);
}

void MainJackSMS::eseguiPassoInvio(){

    //ui->progressBar->setValue(ui->progressBar->value()+1);

}
void MainJackSMS::popupInvio(){
    QMessageBox::information(this,"JackSMS",esitoInvio);



}
void MainJackSMS::clickText(QString _text,QString defaultStr){

    //QFontMetrics fm(ui->LabelEsito->font());
    QSize s=this->size();
    int maxlenght=((s.width()-330)/5)-5; //calcolo molto ma molto approssimativo e fatto a tentativi

    if (!_text.isEmpty()){
        esitoInvio=defaultStr+": "+_text;
        if (esitoInvio.length()<maxlenght){
            ui->LabelEsito->setText(esitoInvio);
        }else{
            ui->LabelEsito->setText(esitoInvio.left(maxlenght)+"...");
        }
    }else{
        esitoInvio=defaultStr;
        ui->LabelEsito->setText(defaultStr+"!");
    }
    /*if (!_text.isEmpty()){

        esitoInvio=defaultStr+": "+_text;
        ui->LabelEsito->setText(fm.elidedText(esitoInvio,Qt::ElideRight,ui->LabelEsito->size().width()));
    }else{
        esitoInvio=defaultStr+"!";
        ui->LabelEsito->setText(fm.elidedText(esitoInvio,Qt::ElideRight,ui->LabelEsito->size().width()));
    }*/
}
void MainJackSMS::smsSaved(libJackSMS::dataTypes::logSmsMessage sms,QString t){
    ElencoServiziConfigurati[sms.getAccountId()].setStat("sent-all",t);

    libJackSMS::localApi::statsManager man(current_user_directory);
    man.updateStatsOfAccount(ultimoSms.getAccountId(),"sent-all",t);
    updateAccountCountComboBox(sms.getAccountId());
}

void MainJackSMS::invioSuccesso(QString _text){

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
    us.setAccountId(ultimoSms.getAccountId());

    libJackSMS::dataTypes::optionsType::const_iterator iter=Opzioni.find("save-local");

    if (iter!=Opzioni.end())
        if ("yes"==iter.value()){
            libJackSMS::localApi::smsLogSaver sav(current_user_directory);
            sav.setMessage(us);
            sav.appendToLogFile();
            ultimoSms.setId(sav.getSavedId());
            ultimoSms.setParsedName(phone2name(ultimoSms.getPhone()));
            Messaggi.push_back(ultimoSms);


            QPixmap icona=ElencoServizi[ultimoSms.getServiceId()].getIcon().pixmap(16,16);
            SmsWidget *wid=new SmsWidget(ultimoSms,icona,false);
            mapWidgets.insert(ultimoSms.getData(),wid);

            QListWidgetItem *item = new QListWidgetItem;
            item->setSizeHint(wid->getSize());
            ui->smsListWidget->insertItem(0,item);
            ui->smsListWidget->setItemWidget(item, wid);
        }
    AbilitaUi();

    libJackSMS::localApi::statsManager man(current_user_directory);
    man.increaseSentStat(ElencoServiziConfigurati[ultimoSms.getAccountId()]);

    ElencoServiziConfigurati[ultimoSms.getAccountId()].increaseStatValue("sent");

    libJackSMS::localApi::optionManager man2(current_user_directory,Opzioni);
    man2.increaseTotalSent();



    onlineSmsSaver=new libJackSMS::serverApi::smsLogSaver(current_login_id,Opzioni);
    connect(onlineSmsSaver,SIGNAL(smsSaved(libJackSMS::dataTypes::logSmsMessage,QString)),this,SLOT(smsSaved(libJackSMS::dataTypes::logSmsMessage,QString)));
    onlineSmsSaver->save(us);

    if (invioMultiplo){
        sendNextMessage(false,true,"Messaggio inviato" + (_text.isEmpty() ? "!" : ": " + _text));
    }else{
        if (Opzioni["successfull-send-popup"]=="yes"){
            popupJms=false;
            this->trayIco->showMessage("JackSMS","Messaggio inviato!" + (_text.isEmpty() ? "" : "\n" + _text),QSystemTrayIcon::Information);
        }

        if (Opzioni["opz-svuota-invio-corretto"]=="yes"){
            svuotaTabSms();
            ui->LabelCountChars->hide();
            ui->LabelEsito->show();
        }
        clickText(_text,"Messaggio inviato");
    }

}
void MainJackSMS::invioFallito(QString _text){

    if(!invioMultiplo)
        clickText(_text,"Messaggio non inviato");

    AbilitaUi();
    invioInCorso=false;

    if (invioMultiplo){
        sendNextMessage(false,false,"Messaggio non inviato" + (_text.isEmpty() ? "" : ": " + _text));
    }else{
        if (Opzioni["error-send-popup"]=="yes"){
            popupJms=false;
            trayIco->showMessage("JackSMS","Messaggio non inviato" + (_text.isEmpty() ? "" : "\n" + _text),QSystemTrayIcon::Critical);
        }
    }
}
void MainJackSMS::sendNextMessage(bool first, bool result, QString _text){

    if ((!first) && (multipleSendRecipients.size()>0)){
        QPair<contactWidgetFastBook*,libJackSMS::dataTypes::phoneNumber> pair=multipleSendRecipients.first();
        if (result){
            pair.first->setInfoIcon(QIcon(":/resource/jms-active.png").pixmap(16,16),_text);
            libJackSMS::dataTypes::optionsType::const_iterator iter=Opzioni.find("opz-svuota-invio-corretto");
            if (iter!=Opzioni.end())
                if ("yes"==iter.value())
                    on_destinatariListWidget_itemDoubleClicked(ui->destinatariListWidget->item(errorSentCounter));
        } else {
            pair.first->setInfoIcon(QIcon(":/resource/jms-not-active.png").pixmap(16,16), _text);
            errorSentCounter++;
        }
        multipleSendRecipients.removeFirst();
    }
    if (multipleSendRecipients.size()>0){
        try{
            QPair<contactWidgetFastBook*,libJackSMS::dataTypes::phoneNumber> pair=this->multipleSendRecipients.first();
            pair.first->setInfoIcon(QIcon(":/resource/jms-activing.png").pixmap(16,16), "Invio in corso...");
            invioInCorso=true;
            DisabilitaUi();

            libJackSMS::dataTypes::shortMessage messaggio(ui->TestoSMS->toPlainText());
            QString idAccount="0";
            if (messageType!=TYPE_JMS){

                if(ui->comboServizio->currentIndex() == 0){
                    // Usa Account Associati
                    libJackSMS::dataTypes::configuredServicesType::const_iterator i = ElencoServiziConfigurati.find(pair.first->getContact().getAccount());
                    if (i!=ElencoServiziConfigurati.end())
                        idAccount = i.key();

                } else {
                    libJackSMS::dataTypes::configuredServicesType::const_iterator i=ElencoServiziConfigurati.begin();
                    libJackSMS::dataTypes::configuredServicesType::const_iterator i_end=ElencoServiziConfigurati.end();

                    for(;i!=i_end;++i){
                        if (i.value().getName()==ui->comboServizio->itemData(ui->comboServizio->currentIndex(),Qt::UserRole).toString()){
                            idAccount=i.key();
                            break;
                        }
                    }
                }

            }

            ultimoSms=QMyMessage();
            ultimoSms.setMessage(ui->TestoSMS->toPlainText());
            ultimoSms.setAccountId(idAccount);
            if (messageType==TYPE_JMS){
                ultimoSms.setAccountName("JackSMS Messenger");
                ultimoSms.setServiceId("40");
            }
            else if (messageType==TYPE_SMS){
                ultimoSms.setAccountName(ui->comboServizio->itemData(ui->comboServizio->currentIndex(),Qt::UserRole).toString());
                ultimoSms.setServiceId(ElencoServiziConfigurati[idAccount].getService());

            }
            ultimoSms.setIsReceived(false);
            ultimoSms.setData(QDateTime::currentDateTime());
            ultimoSms.setPhone(pair.second);


            smsSender=new libJackSMS::smsSender(ElencoServizi,Opzioni);
            smsSender->setRecipient(pair.second);
            smsSender->setMessage(messaggio);
            if (messageType==TYPE_JMS){

                smsSender->setAccount(ElencoServiziConfigurati["0"]);
            }else if (messageType==TYPE_SMS)
                smsSender->setAccount(ElencoServiziConfigurati[idAccount]);

            connect(smsSender,SIGNAL(captcha(QByteArray)),this,SLOT(displayCaptcha(QByteArray)));
            connect(smsSender,SIGNAL(error(QString)),this,SLOT(invioFallito(QString)));
            connect(smsSender,SIGNAL(success(QString)),this,SLOT(invioSuccesso(QString )));

            libJackSMS::dataTypes::optionsType::const_iterator iter=Opzioni.find("dont-cookies");
            if (iter!=Opzioni.end()){
                if ("yes"==iter.value()){
                    smsSender->setSalvaCookies(false);
                }
            }

            smsSender->send();


        }catch(libJackSMS::exceptionPhoneNumber){
            QMessageBox::information(this,"JackSMS","Numero in formato sconosciuto");
            invioInCorso=false;
        }
    } else {
        //fine dell'invio dei messaggi multipli

        //inviati tutti correttamente
        if(errorSentCounter==0){

            if (Opzioni["opz-svuota-invio-corretto"]=="yes"){
                ui->comboServizio->setCurrentIndex(0);
                svuotaTabSms();
                ui->LabelCountChars->hide();
                ui->LabelEsito->show();
            }

            if (Opzioni["successfull-send-popup"]=="yes"){
                popupJms=false;
                this->trayIco->showMessage("JackSMS","Messaggi inviati!",QSystemTrayIcon::Information);
            }

            clickText("","Messaggi inviati");

        //alcuni messaggi non inviati
        }else{
            if (Opzioni["error-send-popup"]=="yes"){
                popupJms=false;
                trayIco->showMessage("JackSMS", "Attenzione: " + QString::number(errorSentCounter) + ((errorSentCounter==1) ? " messaggio non inviato!" : " messaggi non inviati!"), QSystemTrayIcon::Critical);
            }

            clickText(QString::number(errorSentCounter) + ((errorSentCounter==1) ? " messaggio non inviato!" : " messaggi non inviati!"),"Attenzione");
        }
    }


}
void MainJackSMS::on_InviaSMS_clicked()
{

    if (ui->TestoSMS->toPlainText().isEmpty()){
        QMessageBox::information(this,"JackSMS","Il testo del messaggio e' vuoto");
        return;
    }

    if (ui->NumeroDestinatario->text().isEmpty()){
        QMessageBox::information(this,"JackSMS","Il destinatario non e' stato specificato.");
        return;
    }
    if (!invioMultiplo){

        try{
            invioInCorso=true;
            DisabilitaUi();

            libJackSMS::dataTypes::shortMessage messaggio(ui->TestoSMS->toPlainText());
            libJackSMS::dataTypes::phoneNumber destinatario;
            destinatario.parse(ui->NumeroDestinatario->text());
            QString idAccount="0";
            if (messageType!=TYPE_JMS){


                libJackSMS::dataTypes::configuredServicesType::const_iterator i=ElencoServiziConfigurati.begin();
                libJackSMS::dataTypes::configuredServicesType::const_iterator i_end=ElencoServiziConfigurati.end();

                for(;i!=i_end;++i){
                    if (i.value().getName()==ui->comboServizio->itemData(ui->comboServizio->currentIndex(),Qt::UserRole).toString()){
                        idAccount=i.key();
                        break;
                    }
                }

            }

            ultimoSms=QMyMessage();
            ultimoSms.setMessage(ui->TestoSMS->toPlainText());
            ultimoSms.setAccountId(idAccount);
            if (messageType==TYPE_JMS){
                ultimoSms.setAccountName("JackSMS Messenger");
                ultimoSms.setServiceId("40");
            }
            else if (messageType==TYPE_SMS){
                ultimoSms.setAccountName(ui->comboServizio->itemData(ui->comboServizio->currentIndex(),Qt::UserRole).toString());
                ultimoSms.setServiceId(ElencoServiziConfigurati[idAccount].getService());

            }
            ultimoSms.setIsReceived(false);
            ultimoSms.setData(QDateTime::currentDateTime());
            ultimoSms.setPhone(destinatario);


            smsSender=new libJackSMS::smsSender(ElencoServizi,Opzioni);
            smsSender->setRecipient(destinatario);
            smsSender->setMessage(messaggio);
            if (messageType==TYPE_JMS){

                smsSender->setAccount(ElencoServiziConfigurati["0"]);
            }else if (messageType==TYPE_SMS)
                smsSender->setAccount(ElencoServiziConfigurati[idAccount]);

            connect(smsSender,SIGNAL(captcha(QByteArray)),this,SLOT(displayCaptcha(QByteArray)));
            connect(smsSender,SIGNAL(error(QString)),this,SLOT(invioFallito(QString)));
            connect(smsSender,SIGNAL(success(QString)),this,SLOT(invioSuccesso(QString)));

            libJackSMS::dataTypes::optionsType::const_iterator iter=Opzioni.find("dont-cookies");
            if (iter!=Opzioni.end()){
                if ("yes"==iter.value()){
                    smsSender->setSalvaCookies(false);
                }
            }

            smsSender->send();

        }catch(libJackSMS::exceptionPhoneNumber){
            QMessageBox::information(this,"JackSMS","Numero in formato sconosciuto");
            invioInCorso=false;
        }

    }else{

        int c=ui->destinatariListWidget->count();
        for(int i=0;i<c;++i){
            QListWidgetItem *item=ui->destinatariListWidget->item(i);
            contactWidgetFastBook * w=static_cast<contactWidgetFastBook*>(ui->destinatariListWidget->itemWidget(item));
            multipleSendRecipients.push_back(qMakePair(w,w->getContact().getPhone()));
        }

        DisabilitaUi();
        errorSentCounter = 0;
        sendNextMessage(true);

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
    connect(sd,SIGNAL(rewriteAccounts()),this,SLOT(ReWriteConfiguredServicesToGui()));
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
   //connect(dial,SIGNAL(activateImTimer()),this,SLOT(startIm()));
   //connect(dial,SIGNAL(deactivateImTimer()),this,SLOT(stopIm()));
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


            contactWidgetFastBook *ww=new contactWidgetFastBook(i.value(),ico.pixmap(16,16));
            if ((i->getCanReceiveJms()) && (messageType==TYPE_JMS))
                fastList.insert(i->getName().toUpper(),ww);
            else if ((!i->getPhone().getVirtual()) && (messageType==TYPE_SMS))
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
             on_radioSMS_clicked();
             ui->labelNomeDest->clear();
             ui->NumeroDestinatario->clear();
             QListWidgetItem *it=ls.front();
             SmsWidget * ww=static_cast< SmsWidget*>(ui->smsListWidget->itemWidget(it));
             ui->TestoSMS->setText(ww->getText());
             ui->tabWidget->setCurrentIndex(0);

    }

}

void MainJackSMS::on_AnnullaSMS_clicked()
{
    smsSender->abort();
    if (invioMultiplo)
        multipleSendRecipients.clear();

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
        ui->TestoSMS->setText("<<[ "+ww->getText()+" ]>>");
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

                int index=ui->comboServizio->findData(x.value().getName(),Qt::UserRole);
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
            on_radioJMS_clicked();
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

            ui->tabWidget->setCurrentIndex(0);



    }








}

void MainJackSMS::ClickBaloon(){

    raise();
    activateWindow();
    showNormal();
    if (popupJms){
        ui->tabWidget->setCurrentIndex(1);
        popupJms=false;
    }


}
bool MainJackSMS::checkDoubleRecipients(libJackSMS::dataTypes::phoneNumber &_n) const{
    int c=ui->destinatariListWidget->count();
    for(int i=0;i<c;++i){
        QListWidgetItem *item=ui->destinatariListWidget->item(i);
        contactWidgetFastBook * w=static_cast<contactWidgetFastBook*>(ui->destinatariListWidget->itemWidget(item));
        if (_n==w->getContact().getPhone())
            return true;


    }
    return false;

}
void MainJackSMS::on_NumeroDestinatario_textEdited(QString str)
{
    ui->labelNomeDest->setText("");
    if(!recipientAutoEdited){
        QStringList strings=str.split(",",QString::SkipEmptyParts);
        if (strings.count()>1)
            gestiscimenuMultiplo();
        else if (invioMultiplo)
            gestiscimenuSingolo();

        if(invioMultiplo)
            ricaricaDestinatariList(strings);

    }
    recipientAutoEdited=false;
}



void MainJackSMS::on_TextRapidRubrica_textChanged(QString txt)
{
    //ui->rubricaListWidget->clear();
     //QMultiMap<QString,ContactWidget*> list;


     int c=ui->rubricaListWidget->count();
     for(int x=0;x<c;x++){
         QListWidgetItem *item=ui->rubricaListWidget->item(x);
         ContactWidget *w=static_cast<ContactWidget*>(ui->rubricaListWidget->itemWidget(item));
         if (w->searchMatch(txt))
             ui->rubricaListWidget->setItemHidden(item,false);
         else
             ui->rubricaListWidget->setItemHidden(item,true);
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



void MainJackSMS::on_RicercaVeloceIM_textChanged(QString text)
{

    //QString text=ui->RicercaVeloceIM->text().toUpper();

    int c=ui->smsListWidget->count();
    for(int x=0;x<c;x++){
        QListWidgetItem *item=ui->smsListWidget->item(x);
        SmsWidget *w=static_cast<SmsWidget*>(ui->smsListWidget->itemWidget(item));
        if (w->searchMatch(text))
            ui->smsListWidget->setItemHidden(item,false);
        else
            ui->smsListWidget->setItemHidden(item,true);
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

    if(invioMultiplo && (ui->comboServizio->currentIndex()==0)){
        ui->LabelCountChars->setText("Testo: "+QString::number(txt.length())+" caratteri");
    } else {
        QString cSms=QString::number(ceil((float)txt.length()/currentSingleLength));
        QString nSms=QString::number(ceil((float)currentMaxLength/currentSingleLength));
        ui->LabelCountChars->setText("Testo: "+QString::number(txt.length())+"/"+QString::number(currentMaxLength)+", SMS: "+cSms+"/"+nSms);
    }
}

void MainJackSMS::on_comboServizio_activated(QString )
{

}

void MainJackSMS::on_comboServizio_currentIndexChanged(int index)
{
    on_TestoSMS_textChanged();

    if(invioMultiplo){
        int c=ui->destinatariListWidget->count();
        for(int i=0;i<c;++i){
            if(ui->comboServizio->currentIndex() != 0){
                //working
                static_cast<contactWidgetFastBook*>(ui->destinatariListWidget->itemWidget(ui->destinatariListWidget->item(i)))->showIcon(false);
                ui->destinatariListWidget->item(i)->setIcon(ui->comboServizio->itemIcon(ui->comboServizio->currentIndex()));
            } else {
                //rimettere quelli di default
                QPixmap px(0,0);
                px.fill(Qt::transparent);
                ui->destinatariListWidget->item(i)->setIcon(QIcon(px));
                static_cast<contactWidgetFastBook*>(ui->destinatariListWidget->itemWidget(ui->destinatariListWidget->item(i)))->showIcon(true);
            }
        }
    }
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

            QString afp=pluginsDir.absoluteFilePath(fileName);
            QPluginLoader loader(afp);
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
    ui->bottoneinviomultiplo->setEnabled(false);
    ui->tastoNuovoSMS->setEnabled(false);
    if (invioMultiplo)
        ui->destinatariListWidget->disconnect(this,SLOT(on_destinatariListWidget_itemDoubleClicked(QListWidgetItem*)));


}
void MainJackSMS::AbilitaUi(){
    ui->progressBar->hide();
    ui->LabelEsito->show();
    ui->InviaSMS->show();
    ui->AnnullaSMS->hide();
    ui->AnnullaSMS->setEnabled(false);
    ui->RubricaVeloce->setEnabled(true);
    ui->TestoSMS->setEnabled(true);
    ui->comboServizio->setEnabled(true);
    ui->NumeroDestinatario->setEnabled(true);
    ui->RicercaVeloce->setEnabled(true);
    ui->bottoneinviomultiplo->setEnabled(true);
    ui->tastoNuovoSMS->setEnabled(true);
    if (invioMultiplo)
        ui->destinatariListWidget->connect(ui->destinatariListWidget,SIGNAL(itemDoubleClicked(QListWidgetItem*)),this,SLOT(on_destinatariListWidget_itemDoubleClicked(QListWidgetItem*)));
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
void MainJackSMS::ReWriteMessagesToGui(bool limit100){
    ui->smsListWidget->clear();
    WriteMessagesToGui(limit100);
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
    if (err){
        popupJms=false;
        trayIco->showMessage("JackSMS Messenger","Il servizio e' stato disattivato temporaneamente a causa di un errore.\nDettagli: "+str);
    }

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

        QMultiMap<QDateTime,SmsWidget*> mapWidgetsReceivedNew;

        QMultiMap<QDateTime,SmsWidget*> mapWidgetsNew;

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
                msg.setParsedName(phone2name(i.value().getPhoneNumber()));


                msg.setAccountId(QString::number(0));
                msg.setData( i.value().getDate().getData());
                msg.setMessage(i.value().getText());
                msg.setId(i.value().getId());
                msg.setIsReceived(true);
                msg.setPhone(i.value().getPhoneNumber());
                Messaggi.push_back(msg);
                msg.setReaded(false);
                msg.setAccountName("JackSMS Messenger");
                MessaggiRicevuti.push_back(msg);
                countReceivedUnreaded++;
                ui->tabWidget->setTabText(1,"Ricevuti ("+QString::number(countReceivedUnreaded)+")");

                SmsWidget *wid=new SmsWidget(msg,icon_jack, true);

                mapWidgetsReceived.insert(i.value().getDate().getData(),wid);
                mapWidgetsReceivedNew.insert(i.value().getDate().getData(),wid);
                msg.setReaded(true);
                SmsWidget *wid2=new SmsWidget(msg,icon_jack, true);
                mapWidgets.insert(i.value().getDate().getData(),wid2);
                mapWidgetsNew.insert(i.value().getDate().getData(),wid2);


                //relays to listening plugins the received message
                jphi->emitJIMreceived(*i);

        }
        section++;


        if (mapWidgetsReceivedNew.size()>0){
            QMultiMap<QDateTime,SmsWidget*>::ConstIterator xx=mapWidgetsReceivedNew.begin();
            QMultiMap<QDateTime,SmsWidget*>::ConstIterator xx_end=mapWidgetsReceivedNew.end();
            for(;xx!=xx_end;++xx){

                QListWidgetItem *item = new QListWidgetItem;
                item->setSizeHint(xx.value()->getSize());
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
                item->setSizeHint(xx.value()->getSize());
                ui->smsListWidget->insertItem(0,item);
                ui->smsListWidget->setItemWidget(item, xx.value());
            }
        }
        /*****************************end append im to gui***************************/


        section++;

        setTrayIcon();
        section++;
        popupJms=true;
        if (jmsList.size()==1)
            trayIco->showMessage("JackSMS Messenger","Ricevuto un nuovo JMS");
        else
            trayIco->showMessage("JackSMS Messenger","Ricevuti "+QString::number(jmsList.size())+" nuovi JMS");
        //jmsList.clear();

        section++;
/*
        if( Opzioni["play-im-sound"]=="yes"){
            Phonon::MediaObject *mediaObject= new Phonon::MediaObject(this);
            Phonon::AudioOutput *audioOutput= new Phonon::AudioOutput(Phonon::MusicCategory, this);
            Phonon::createPath(mediaObject, audioOutput);
            mediaObject->setCurrentSource(Phonon::MediaSource("C:\\Users\\Ivan\\Music\\Toto - Legend - The Best Of (Mp3 320)\\02 - Hold the Line.mp3"));
            mediaObject->play();
        }*/
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
        libJackSMS::serverApi::synchronizeVariables(ElencoServiziConfigurati,ElencoServizi);
       this->WriteAddressBookToGui();
       this->WriteConfiguredServicesToGui();
       this->WriteMessagesToGui();

       enableUiAfterLogin();
       ui->widgetSchermate->setCurrentIndex(2);

       usaAssociatiPresent = false;
       libJackSMS::dataTypes::optionsType::const_iterator iter=Opzioni.find("opz-radio-singolo");
       if (iter!=Opzioni.end()){
           if ("no"==iter.value()){
               gestiscimenuMultiplo(true);
           }else{
               gestiscimenuSingolo(true);
           }
       }else{
           gestiscimenuSingolo(true);
       }


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
    types::QMessageListType::iterator i=msgs.begin();
    types::QMessageListType::iterator i_end=msgs.end();
    for(;i!=i_end;++i){
        if (i->getIsReceived())
            i->setAccountName("JackSMS Messenger");
        i->setParsedName(phone2name(i->getPhone()));
    }
    Messaggi=msgs;
    countdownToGui();
}
void MainJackSMS::servicesLoaded(libJackSMS::dataTypes::servicesType s){
    ElencoServizi=s;
    icon_jack=ElencoServizi["40"].getIcon().pixmap(16,16);

    loaderMessages=new messageLoader(current_user_directory);
    connect(loaderMessages,SIGNAL(messagesLoaded(QList<QMyMessage>)),this,SLOT(messagesLoaded(QList<QMyMessage>)));
    loaderMessages->loadMessages();

    countdownToGui();


}
void MainJackSMS::optionsLoaded(libJackSMS::dataTypes::optionsType op){

    Opzioni=op;

    ui->TestoSMS->setFont(QFont(ui->TestoSMS->font().family(),Opzioni["textsize"].toInt(NULL,10),ui->TestoSMS->font().weight(),false));

    //questo mi permette di abilitare il servizio jms sempre e non abilitarlo solo se è esplicitamente "no"
    if (Opzioni["receive-im"]!="no"){
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

    if (ui->autoLogin->isChecked()){
        GlobalOptions["auto-login"]="yes";
        Opzioni["auto-login"]="yes";
    } else {
        GlobalOptions["auto-login"]="no";
        Opzioni["auto-login"]="no";
    }

    GlobalOptions["default-user"]=QString::number(ui->username->currentIndex());

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
    libJackSMS::localApi::cookieManager m(current_user_directory);
    m.deleteCookies();


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

    libJackSMS::localApi::serviceManager man;
    man.mergeServices(xml);
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


    libJackSMS::dataTypes::configuredAccount *acc=new libJackSMS::dataTypes::configuredAccount();
    acc->setId("0");
    acc->setName("JackSMS Messenger");
    acc->setService("40");
    acc->setData("data1",current_user_username);
    acc->setData("data2",ui->password->text());
    ElencoServiziConfigurati.insert("0",*acc);

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

    if(ui->autoLogin->isChecked()){
        GlobalOptions["auto-login"]="yes";
        Opzioni["auto-login"]="yes";
    }else{
        GlobalOptions["auto-login"]="no";
        Opzioni["auto-login"]="no";
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
    ui->actionRicarica_servizi->setEnabled(false);
    ui->actionElimina_cookies->setEnabled(false);
    ui->actionPlugins->setEnabled(false);
    ui->actionCsv->setEnabled(false);

}
void MainJackSMS::enableUiAfterLogin(){
    ui->actionLogout->setEnabled(true);
    //ui->actionOpzioni->setEnabled(true);
    ui->actionStatistiche->setEnabled(true);
    ui->actionCrea_backup_configurazione->setEnabled(true);
    ui->actionImporta_Backup->setEnabled(true);
    ui->actionRicarica_servizi->setEnabled(true);
    ui->actionElimina_cookies->setEnabled(true);
    ui->actionPlugins->setEnabled(true);
    ui->actionCsv->setEnabled(true);
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
            ui->RispondiButton->setVisible(false);
            ui->CitaButton->setVisible(false);
            ui->InoltraButton->setVisible(true);

        }
    }


}

void MainJackSMS::on_RubricaVeloce_currentItemChanged(QListWidgetItem* item, QListWidgetItem*old)
{
    if (!invioInCorso){



        if (!invioMultiplo && item!=NULL ){
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
            if (Opzioni["set-account"]=="yes" && messageType==TYPE_SMS){

                libJackSMS::dataTypes::configuredServicesType::const_iterator i=ElencoServiziConfigurati.find(accountId);

                if (i!=ElencoServiziConfigurati.end()){
                    int index=ui->comboServizio->findData(i.value().getName(),Qt::UserRole);
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
    ReWriteAddressBookToGui();
    ricaricaDestinatariList();
}
void MainJackSMS::closeEvent(QCloseEvent *event)
 {

    if (loggedIn){
        loginClient->deleteLater();
        loaderMessages->deleteLater();
        pingator->deleteLater();
        if (imChecker!=NULL && Opzioni["receive-im"]!="no"){
           imChecker->stop();
           imChecker->deleteLater();

        }
        if (updateChecker!=NULL){
            if (updateChecker->isRunning()){
                updateChecker->abort();



            }


        }
    }


 }

void MainJackSMS::on_actionLogout_triggered()
{
    loggedIn=false;
    loginClient->deleteLater();
    loaderMessages->deleteLater();
    pingator->deleteLater();
    Rubrica.clear();
    if (imChecker!=NULL && Opzioni["receive-im"]!="no"){
       imChecker->stop();
       imChecker->deleteLater();
    }

    ElencoServizi.clear();
    ElencoServiziConfigurati.clear();
    if (GlobalOptions["save-passwd"]!="yes"){
        ui->password->clear();
    }

    GlobalOptions["auto-login"]="no";
    Opzioni["auto-login"]="no";
    ui->autoLogin->setChecked(false);

    if (updateChecker->isRunning()){
        updateChecker->abort();



    }


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

void MainJackSMS::on_TextRapidServizi_textEdited(QString text )
{
    int c=ui->listServiziConfigurati->count();
    for(int x=0;x<c;x++){
        QListWidgetItem *item=ui->listServiziConfigurati->item(x);
        accountWidget *w=static_cast<accountWidget*>(ui->listServiziConfigurati->itemWidget(item));
        if (w->getName().contains(text,Qt::CaseInsensitive))
            ui->listServiziConfigurati->setItemHidden(item,false);
        else
            ui->listServiziConfigurati->setItemHidden(item,true);
    }
}

void MainJackSMS::on_TextRapidServizi_textChanged(QString){
}


void MainJackSMS::on_RispondiIm_clicked()
{
    QList<QListWidgetItem *> ls=ui->imRicevutiWidget->selectedItems();



    if (!ls.isEmpty()){
            on_radioJMS_clicked();
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
                      int index=ui->comboServizio->findData(x.value().getName(),Qt::UserRole);
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

void MainJackSMS::on_RicercaVeloceIM_2_textChanged(QString text)
{



    //QString text=ui->RicercaVeloceIM_2->text().toUpper();
    int c=ui->imRicevutiWidget->count();
    for(int x=0;x<c;x++){
        QListWidgetItem *item=ui->imRicevutiWidget->item(x);
        SmsWidget *w=static_cast<SmsWidget*>(ui->imRicevutiWidget->itemWidget(item));
        if (w->searchMatch(text))
            ui->imRicevutiWidget->setItemHidden(item,false);
        else
            ui->imRicevutiWidget->setItemHidden(item,true);
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
    /*if (current!=NULL){
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


    }*/
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




void MainJackSMS::on_radioSMS_clicked()
{
    messageType=TYPE_SMS;
    ui->comboServizio->show();
    ui->label_2->show();
    showContactByTypeInFastAbook();
    ui->radioSMS->setChecked(true);
    on_comboServizio_currentIndexChanged(ui->comboServizio->itemData(ui->comboServizio->currentIndex(),Qt::UserRole).toString());


}

void MainJackSMS::on_radioJMS_clicked()
{
    messageType=TYPE_JMS;
    ui->comboServizio->hide();
    ui->label_2->hide();
    ui->radioJMS->setChecked(true);
    showContactByTypeInFastAbook();

    libJackSMS::dataTypes::configuredServicesType::const_iterator x=ElencoServiziConfigurati.find("0");
    if (x!=ElencoServiziConfigurati.end()){
        QString serviceId=x.value().getService();
        libJackSMS::dataTypes::servicesType::const_iterator i=ElencoServizi.find(serviceId);
        currentMaxLength=i.value().getIntMaxLength();
        currentSingleLength=i.value().getIntSmsDivisor();
    }
    on_TestoSMS_textChanged();

}

void MainJackSMS::on_CitaButton_2_clicked()
{
    QList<QListWidgetItem *> ls=ui->imRicevutiWidget->selectedItems();



    if (!ls.isEmpty()){
        on_radioJMS_clicked();
        QListWidgetItem *it=ls.front();
        SmsWidget * ww=static_cast< SmsWidget*>(ui->imRicevutiWidget->itemWidget(it));
        QString phone=ww->getPhoneNum().toString();
        ui->NumeroDestinatario->setText(phone);
        ui->TestoSMS->setText("<<[ "+ww->getText()+" ]>>");
        libJackSMS::dataTypes::phoneBookType::const_iterator a=Rubrica.begin();
        libJackSMS::dataTypes::phoneBookType::const_iterator a_end=Rubrica.end();

        for (;a!=a_end;++a){
            if (a.value().getPhone().toString()==phone){
                ui->labelNomeDest->setText(a.value().getName());
                break;
            }
        }


        ui->tabWidget->setCurrentIndex(0);


    }
}

void MainJackSMS::on_buttonLostPassword_clicked()
{
    QDesktopServices::openUrl(QUrl("http://www.jacksms.it/recupera-password.html", QUrl::TolerantMode));

}

void MainJackSMS::on_buttonNoAccount_clicked()
{
    QDesktopServices::openUrl(QUrl("http://www.jacksms.it/registrazione.html", QUrl::TolerantMode));
}

void MainJackSMS::on_imRicevutiWidget_itemPressed(QListWidgetItem* current)
{
    if (current!=NULL){
            SmsWidget * w=static_cast<SmsWidget*>(ui->imRicevutiWidget->itemWidget(current));
            if (!w->isReaded()){
                if (countReceivedUnreaded>0)
                    countReceivedUnreaded--;

                if (countReceivedUnreaded>0)
                    ui->tabWidget->setTabText(1,"Ricevuti ("+QString::number(countReceivedUnreaded)+")");
                else
                    ui->tabWidget->setTabText(1,"Ricevuti");
                
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

void MainJackSMS::on_ModificaServizioButton_clicked()
{

    QListWidgetItem *wid=ui->listServiziConfigurati->currentItem();
    if (wid!=NULL){
            QWidget * wi=ui->listServiziConfigurati->itemWidget(wid);
            accountWidget* aw=static_cast<accountWidget*>(wi);
            QString id=aw->getAccountId();
            editAccountDialog *dialog=new editAccountDialog(ElencoServiziConfigurati,ElencoServizi,id,current_login_id,Opzioni,this);
            connect(dialog,SIGNAL(rewriteAccunts()),this,SLOT(ReWriteConfiguredServicesToGui()));
            dialog->exec();
            dialog->deleteLater();
    }

}

void MainJackSMS::on_RubricaVeloce_itemDoubleClicked(QListWidgetItem* item)
{

    if(invioMultiplo){
        contactWidgetFastBook * w=static_cast<contactWidgetFastBook*>(ui->RubricaVeloce->itemWidget(item))->clone();
        libJackSMS::dataTypes::phoneNumber num=w->getContact().getPhone();

        if (!checkDoubleRecipients(num)){
            QListWidgetItem *n = new QListWidgetItem;
            n->setSizeHint(w->size());

            if(ui->comboServizio->currentIndex() != 0){
                w->showIcon(false);
                n->setIcon(ui->comboServizio->itemIcon(ui->comboServizio->currentIndex()));
            } else {
                w->showIcon(true);
            }

            ui->destinatariListWidget->addItem(n);
            ui->destinatariListWidget->setItemWidget(n, w);

        }else{
            QMessageBox::critical(this,"JackSMS","Questo numero e' gia' presente nella lista dei destinatari.");
        }
        recipientStringCalculate();

    }

}
void MainJackSMS::recipientStringCalculate(){

    QString r;
    int c=ui->destinatariListWidget->count();
    for(int i=0;i<c;++i){
        QListWidgetItem *item=ui->destinatariListWidget->item(i);
        contactWidgetFastBook * w=static_cast<contactWidgetFastBook*>(ui->destinatariListWidget->itemWidget(item));
        if (!r.isEmpty())
            r.append(", ");
        r.append(w->getName()+QString(" <")+w->getContact().getPhone().toString()+QString(">"));


    }
    recipientAutoEdited=true;
    ui->NumeroDestinatario->setText(r);


}
void MainJackSMS::on_destinatariListWidget_itemDoubleClicked(QListWidgetItem* item)
{
    ui->destinatariListWidget->takeItem(ui->destinatariListWidget->row(item));

    item->~QListWidgetItem();
    recipientStringCalculate();
}

void MainJackSMS::on_rubricaListWidget_itemDoubleClicked(QListWidgetItem* item)
{
    QWidget * wi=ui->rubricaListWidget->itemWidget(item);
    ContactWidget* cw=static_cast<ContactWidget*>(wi);
    QString id=cw->getContactId();
    editcontattodialog *dial=new editcontattodialog(this,this,ElencoServizi,ElencoServiziConfigurati,Rubrica,id,Opzioni);
    dial->exec();
    dial->deleteLater();
}

void MainJackSMS::on_listServiziConfigurati_itemDoubleClicked(QListWidgetItem* item)
{
    QWidget * wi=ui->listServiziConfigurati->itemWidget(item);
    accountWidget* aw=static_cast<accountWidget*>(wi);
    QString id=aw->getAccountId();
    editAccountDialog *dialog=new editAccountDialog(ElencoServiziConfigurati,ElencoServizi,id,current_login_id,Opzioni,this);
    connect(dialog,SIGNAL(rewriteAccunts()),this,SLOT(ReWriteConfiguredServicesToGui()));
    dialog->exec();
    dialog->deleteLater();
}

void MainJackSMS::on_actionCsv_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this,"JackSMS - Esporta rubrica","rubrica-jacksms.csv");
    if (!fileName.isEmpty()){
    libJackSMS::dataTypes::phoneBookType::const_iterator i=Rubrica.begin();
    libJackSMS::dataTypes::phoneBookType::const_iterator i_end=Rubrica.end();
    QFile out(fileName);
    if (!out.open(QIODevice::WriteOnly | QIODevice::Text))
        QMessageBox::critical(this,"JackSMS","Impossibile salvare la rubrica in questa posizione.");
    else{
        for(;i!=i_end;++i){
            QString riga=i->getName()+";"+i->getPhone().toString();
            if (riga.contains("\"")){
                riga=riga.replace("\"","\"\"");
                riga="\""+riga+"\"";
            }
            riga=riga+"\n";
            out.write(riga.toAscii());
        }
        out.close();
        QMessageBox::information(this,"JackSMS","Rubrica esportata.");
    }
    }
}

void MainJackSMS::on_ricordaPassword_stateChanged(int stato)
{
    if (stato == Qt::Checked) {
        ui->autoLogin->setEnabled(true);
    } else {
        ui->autoLogin->setChecked(false);
        ui->autoLogin->setEnabled(false);
    }
    ui->loginButton->setFocus();
}


void MainJackSMS::username_returnPressed(){
    ui->loginButton->animateClick();
}


void MainJackSMS::on_autoLogin_stateChanged(int )
{
    ui->loginButton->setFocus();
}


void MainJackSMS::on_bottoneinviomultiplo_clicked()
{
    svuotaDestinatari();

    if(invioMultiplo)
        gestiscimenuSingolo();
    else
        gestiscimenuMultiplo();
}

void MainJackSMS::on_tastoNuovoSMS_clicked()
{
    svuotaTabSms();
}

void MainJackSMS::svuotaTabSms(){
    svuotaDestinatari();
    ui->TestoSMS->clear();
    ui->LabelCountChars->clear();
}

void MainJackSMS::svuotaDestinatari(){

    if(invioMultiplo)
        ui->label->setText("Destinatari");
    else
        ui->label->setText("Destinatario");

    ui->destinatariListWidget->clear();
    ui->NumeroDestinatario->clear();
    ui->labelNomeDest->clear();
}

void MainJackSMS::ricaricaDestinatariList(QStringList strings){

    ui->destinatariListWidget->clear();
    while(!strings.isEmpty()){
        QString s=strings.first();
        s=s.trimmed();
        strings.removeFirst();
        QRegExp r;
        r.setPattern("^([^<]+) <([\\+\\.0-9]+)>$");
        bool ok=false;
        if (r.exactMatch(s)){
            libJackSMS::dataTypes::phoneBookType::const_iterator i=Rubrica.begin();
            for(;i!=Rubrica.end();++i){
                libJackSMS::dataTypes::phoneNumber num;
                num.parse(r.cap(2));
                if (i->getPhone()==num){
                    if (!checkDoubleRecipients(num)){
                        QListWidgetItem *n = new QListWidgetItem;
                        libJackSMS::dataTypes::contact cc(i->getName(),i->getPhone(),"",i->getAccount());
                        contactWidgetFastBook * w=new contactWidgetFastBook(cc,ElencoServizi[ElencoServiziConfigurati[i->getAccount()].getService()].getIcon().pixmap(16,16));
                        n->setSizeHint(w->size());
                        if(ui->comboServizio->currentIndex() != 0){
                            w->showIcon(false);
                            n->setIcon(ui->comboServizio->itemIcon(ui->comboServizio->currentIndex()));
                        } else {
                            w->showIcon(true);
                        }
                        ui->destinatariListWidget->addItem(n);
                        ui->destinatariListWidget->setItemWidget(n, w);
                    }else{
                        QMessageBox::critical(this,"JackSMS","Questo numero e' gia' presente nella lista dei destinatari.");
                    }

                    ok=true;
                    break;
                }
            }
        }
        if (!ok){
            r.setPattern("^([0-9]{10})$");
            if (r.exactMatch(s)){
                libJackSMS::dataTypes::phoneBookType::const_iterator i=Rubrica.begin();
                libJackSMS::dataTypes::phoneNumber num;
                num.parse(r.cap(1));
                for(;i!=Rubrica.end();++i){

                    if (i->getPhone()==num){
                        if (!checkDoubleRecipients(num)){
                            QListWidgetItem *n = new QListWidgetItem;
                            libJackSMS::dataTypes::contact cc(i->getName(),i->getPhone(),"",i->getAccount());
                            contactWidgetFastBook * w=new contactWidgetFastBook(cc,ElencoServizi[ElencoServiziConfigurati[i->getAccount()].getService()].getIcon().pixmap(16,16));
                            n->setSizeHint(w->size());
                            if(ui->comboServizio->currentIndex() != 0){
                                w->showIcon(false);
                                n->setIcon(ui->comboServizio->itemIcon(ui->comboServizio->currentIndex()));
                            } else {
                                w->showIcon(true);
                            }
                            ui->destinatariListWidget->addItem(n);
                            ui->destinatariListWidget->setItemWidget(n, w);
                        }else{
                            QMessageBox::critical(this,"JackSMS","Questo numero e' gia' presente nella lista dei destinatari.");
                        }

                        ok=true;
                        break;
                    }
                }
                if (!ok){
                    if (!checkDoubleRecipients(num)){
                        QListWidgetItem *n = new QListWidgetItem;
                        libJackSMS::dataTypes::contact cc(r.cap(1),num,"","0");
                        contactWidgetFastBook * w=new contactWidgetFastBook(cc,icon_jack);
                        n->setSizeHint(w->size());
                        if(ui->comboServizio->currentIndex() != 0){
                            w->showIcon(false);
                            n->setIcon(ui->comboServizio->itemIcon(ui->comboServizio->currentIndex()));
                        } else {
                            w->showIcon(true);
                        }
                        ui->destinatariListWidget->addItem(n);
                        ui->destinatariListWidget->setItemWidget(n, w);
                    }else{
                        QMessageBox::critical(this,"JackSMS","Questo numero e' gia' presente nella lista dei destinatari.");
                    }

                    ok=true;
                }

            }
        }
        if (!ok){
            r.setPattern("^([0-9]{4,5})$");
            if (r.exactMatch(s)){
                libJackSMS::dataTypes::phoneNumber num;
                num.parse(r.cap(1));
                if (!checkDoubleRecipients(num)){
                    QListWidgetItem *n = new QListWidgetItem;
                    libJackSMS::dataTypes::contact cc(r.cap(1),num,"","0");
                    contactWidgetFastBook * w=new contactWidgetFastBook(cc,icon_jack);
                    n->setSizeHint(w->size());
                    if(ui->comboServizio->currentIndex() != 0){
                        w->showIcon(false);
                        n->setIcon(ui->comboServizio->itemIcon(ui->comboServizio->currentIndex()));
                    } else {
                        w->showIcon(true);
                    }
                    ui->destinatariListWidget->addItem(n);
                    ui->destinatariListWidget->setItemWidget(n, w);
                }else{
                    QMessageBox::critical(this,"JackSMS","Questo numero e' gia' presente nella lista dei destinatari.");
                }

                ok=true;

            }
        }
        if (!ok){
            r.setPattern("^([0-9]{4,5}) <([0-9]{4,5})>$");
            if (r.exactMatch(s)){
                libJackSMS::dataTypes::phoneNumber num;
                num.parse(r.cap(2));
                if (!checkDoubleRecipients(num)){
                    QListWidgetItem *n = new QListWidgetItem;
                    libJackSMS::dataTypes::contact cc(r.cap(1),num,"","0");
                    contactWidgetFastBook * w=new contactWidgetFastBook(cc,icon_jack);
                    n->setSizeHint(w->size());
                    if(ui->comboServizio->currentIndex() != 0){
                        w->showIcon(false);
                        n->setIcon(ui->comboServizio->itemIcon(ui->comboServizio->currentIndex()));
                    } else {
                        w->showIcon(true);
                    }
                    ui->destinatariListWidget->addItem(n);
                    ui->destinatariListWidget->setItemWidget(n, w);
                }else{
                    QMessageBox::critical(this,"JackSMS","Questo numero e' gia' presente nella lista dei destinatari.");
                }

                ok=true;

            }
        }
        if (!ok){
            libJackSMS::dataTypes::phoneBookType::const_iterator i=Rubrica.begin();
            for(;i!=Rubrica.end();++i){
                libJackSMS::dataTypes::phoneNumber num;
                num.parse(s);
                libJackSMS::dataTypes::phoneNumber num2=i->getPhone();
                if ((num2==num)||(i->getName().toUpper()==s.toUpper())){

                    if (!checkDoubleRecipients(num2)){
                        QListWidgetItem *n = new QListWidgetItem;
                        libJackSMS::dataTypes::contact cc(i->getName(),num2,"",i->getAccount());
                        QIcon ico=ElencoServizi[ElencoServiziConfigurati[i->getAccount()].getService()].getIcon();
                        contactWidgetFastBook * w=new contactWidgetFastBook(cc,ico.pixmap(16,16));
                        n->setSizeHint(w->size());
                        if(ui->comboServizio->currentIndex() != 0){
                            w->showIcon(false);
                            n->setIcon(ui->comboServizio->itemIcon(ui->comboServizio->currentIndex()));
                        } else {
                            w->showIcon(true);
                        }
                        ui->destinatariListWidget->addItem(n);
                        ui->destinatariListWidget->setItemWidget(n, w);
                    }else{
                        QMessageBox::critical(this,"JackSMS","Questo numero e' gia' presente nella lista dei destinatari.");
                    }

                    break;
                }
            }
        }
    }
}

void MainJackSMS::ricaricaDestinatariList(QString str){
    ricaricaDestinatariList(str.split(",",QString::SkipEmptyParts));
}

void MainJackSMS::ricaricaDestinatariList(){
    ricaricaDestinatariList(ui->NumeroDestinatario->text());
}

void MainJackSMS::on_numArchivio_currentIndexChanged(int index)
{
    //Ultimi 100
    if(index == 0){
        ReWriteMessagesToGui(true);

    //Tutti
    }else if(index == 1){
        ReWriteMessagesToGui(false);
    }
}

void MainJackSMS::on_tabWidget_currentChanged(int index)
{
    if(index==2)
        ui->RicercaVeloceIM->setFocus();
}
