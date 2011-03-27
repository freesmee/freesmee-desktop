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
#include "qrecipientwidget.h"
#include "editcontattodialog.h"
#include <QHeaderView>
#include <QMap>
#include "libjacksms/libJackSMS.h"
#include "smswidget.h"
#include "importgmaildialog.h"
#include "contactwidget.h"
#include "editaccountdialog.h"
#include "contactwidgetfastbook.h"
#include "accountwidget.h"
#include "Types.h"
#include "statsdialog.h"
#include "plugindialog.h"
#include "qlabelresult.h"
#include "messageloader.h"
#include <QSslSocket>
#include <QTabBar>
#include "cambiaaccount.h"
#include "multiplecheckdialog.h"
#include "smstextedit.h"

#define COUNTDOWNTOGUICOUNTDEFINE 5
#define MILLISECONDSDELTASMSLOAD 25
MainJackSMS::MainJackSMS(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainJackSMS)
{

    //abilito la gui quando  countdownToGuiCount==0
    //1-fine caricamento servizi
    //2-fine login
    //3-fine caricamento messaggi
    //4-fine caricamento account di login
    //5-fine caricamento rubrica login
    countdownToGuiCount = COUNTDOWNTOGUICOUNTDEFINE;

    popupJms = false;
    imChecker = NULL;
    updateChecker = NULL;
    messageType = TYPE_SMS;
    altriMessaggi = true;
    nascondiProssimoDialogRicarica = false;
    lastJmsError = "";

    ui->setupUi(this);

    ui->verticalLayout_12->setAlignment(ui->comboServizio,Qt::AlignTop);
    ui->widgetSchermate->setCurrentIndex(0);
    ui->tabWidget->setCurrentIndex(0);

    loggedIn = false;
    recipientAutoEdited = false;
    invioMultiplo = false;
    currentMaxLength = 160;
    countReceivedUnreaded = 0;
    setWindowTitle(QString("JackSMS ") + QString(JACKSMS_VERSION));

    /*imposto lo spinner nella schermata di caricamento*/
    spinner = new QMovie(":/resource/loading-spinner.gif", QByteArray("gif"), this);
    spinner->start();
    ui->labelSpinner->setMovie(spinner);

    spinnerDC = new QMovie(":/resource/loading-spinner.gif", QByteArray("gif"), this);
    spinnerDC->setScaledSize(QSize(16, 16));
    spinnerDC->start();
    ui->labelSpinDelContact->setMovie(spinnerDC);
    ui->labelSpinDelContact->hide();
    ui->labelSpinDelAccount->setMovie(spinnerDC);
    ui->labelSpinDelAccount->hide();
    ui->progressBar->setMovie(spinnerDC);

    QPalette p = ui->smsListWidget->palette();
    p.setColor(QPalette::Highlight,QColor(209, 215, 231));
    ui->smsListWidget->setPalette(p);
    ui->listServiziConfigurati->setPalette(p);
    ui->rubricaListWidget->setPalette(p);
    ui->RubricaVeloce->setPalette(p);

    ui->rubricaBar->addTab("Tutti");
    ui->rubricaBar->addTab("JMS");
    ui->rubricaBar->setShape(QTabBar::RoundedSouth);
    ui->rubricaBar->setMovable(false);
    connect(ui->rubricaBar, SIGNAL(currentChanged(int)), this, SLOT(rubricaBarCurrentChanged(int)));

    ui->LabelEsito->~QWidget();
    ui->LabelEsito=new QLabelResult(this);
    ui->horizontalLayout_16->addWidget(ui->LabelEsito);
    ui->LabelEsito->setText("");
    ui->LabelEsito->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Minimum);
    connect(ui->LabelEsito, SIGNAL(clicked()), this, SLOT(popupInvio()));
    connect(ui->TestoSMS, SIGNAL(richiestoInvio()), this, SLOT(testoSmsRichiestoInvio()));
    connect(ui->smsListWidget, SIGNAL(smsListCanc()), this, SLOT(catchSmsListCanc()));
    connect(ui->recipientLine, SIGNAL(tabKeyPressed()), this, SLOT(RecipientTabPressed()));

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
    ui->tabWidget->removeTab(5);
    ui->tabWidget->removeTab(5);
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

    if(!man.initializeDirectory()) {
        QMessageBox::critical(this, "JackSMS", "Non posso inizializzare JackSMS: Impossibile creare le cartelle necessarie.\nJacksms verrà chiuso.\n\nErrore: "+man.getError());
        exit(-1);
    }

    menu = new QMenu(this);
    menu->addAction(QIcon(), "Pulisci tutti i campi", this, SLOT(svuotaTabSms()));
    ui->menuInstruments->setMenu(menu);

    caricaUtenti();
    translateGui();

    //firstResize = true;
    //connect(&resizeTimer, SIGNAL(timeout()), this, SLOT(resized()));

    ui->ricordaPassword->setChecked(false);
    ui->autoLogin->setChecked(false);

    /*questo carica le opzioni globali*/
    libJackSMS::localApi::synchronousOptionLoader l("");
    if(l.load(GlobalOptions)) {

        manageUserPassAutoLogin();

        libJackSMS::dataTypes::optionsType::const_iterator i = GlobalOptions.find("window-height");
        if (i != GlobalOptions.end()) {
            bool ok, ok2;
            int h = i.value().toInt(&ok, 10);
            int w = GlobalOptions["window-width"].toInt(&ok2, 10);
            if (ok && ok2)
                resize(w,h);
        }

        i = GlobalOptions.find("window-xpos");
        if (i != GlobalOptions.end()) {
            bool ok, ok2;
            int xpos = GlobalOptions["window-xpos"].toInt(&ok, 10);
            int ypos = GlobalOptions["window-ypos"].toInt(&ok2, 10);
            if (ok && ok2)
                move(xpos, ypos);
        }

        i = GlobalOptions.find("window-maximized");
        if (i != GlobalOptions.end()){
            if (i.value() == "yes")
                showMaximized();
        }

        //firstResize = false;
    }

    connect(ui->username->lineEdit(), SIGNAL(returnPressed()), this, SLOT(username_returnPressed()));

    QDateTime midnight;
    midnight.setTime(QTime(23,59,59,999));
    midnight.setDate(QDate::currentDate());
    int secToMidnight=QDateTime::currentDateTime().secsTo(midnight);  //directly from the Maiden Album "powerslave"....2 minutes to midnight! :P
    resetCounterTimer.singleShot(secToMidnight*1000,this,SLOT(resetCounters()));
    ui->recipientListWidget->setVisible(false);
    ui->rubricaBar->setCurrentIndex(0);
    rubricaBarCurrentChanged(0);

    if (!QSslSocket::supportsSsl()){
        QMessageBox::critical(this,"JackSMS","Il sistema in uso non supporta la modalità di connessione sicura (SSL).\nIn queste condizioni, alcuni servizi potrebbero non funzionare correttamente.");
    }

    backupImChecker = NULL;
}

void MainJackSMS::on_recipientLine_returnPressed()
{
    elaboraRecipientLine();
}

void MainJackSMS::resizeRecipientBox() {
    if (ui->recipientListWidget->count() > 0) {
        ui->recipientListWidget->show();

        int rCount = 1;
        int sum = 0;
        for (int a = 0; a < ui->recipientListWidget->count(); ++a) {
            sum = sum + ui->recipientListWidget->itemWidget(ui->recipientListWidget->item(a))->width() + 9;
            if (sum > ui->recipientListWidget->width()) {
                sum = ui->recipientListWidget->itemWidget(ui->recipientListWidget->item(a))->width();
                rCount++;
            }
        }

        rCount = ((rCount < 5) ? rCount : 4);
        ui->recipientListWidget->setMinimumHeight(22*rCount);
        ui->recipientListWidget->setMaximumHeight(22*rCount);
    } else {
        ui->recipientListWidget->hide();
    }
}

void MainJackSMS::recipientRemove(QListWidgetItem* w) {

    ui->RubricaVeloce->clearSelection();

    if (!invioInCorso) {
        ui->recipientListWidget->takeItem(ui->recipientListWidget->row(w));

        if(ui->recipientListWidget->count() < 1)
            ui->recipientListWidget->hide();
        else
           resizeRecipientBox();

        if (ui->recipientListWidget->count() == 1) {
            gestiscimenuSingolo();

            QRecipientWidget *widget = dynamic_cast<QRecipientWidget*>(ui->recipientListWidget->itemWidget(ui->recipientListWidget->item(0)));
            //ui->recipientLine->setText(widget->getName()+" <"+widget->getPhone().toString()+">");

            if (Opzioni["set-account"] != "no" && messageType == TYPE_SMS) {
                libJackSMS::dataTypes::configuredServicesType::const_iterator i = ElencoServiziConfigurati.find(widget->getAccountId());
                if (i != ElencoServiziConfigurati.end()) {
                    int index = ui->comboServizio->findData(i.value().getName(), Qt::UserRole);
                    if (index >= 0)
                        ui->comboServizio->setCurrentIndex(index);
                }
            }

        }
    }
}

// fa già tutto questo alla chiusura
/*void MainJackSMS::resized() {
    resizeTimer.stop();

    if (isMaximized()) {
        GlobalOptions["window-maximized"] = "yes";
    } else {
        GlobalOptions["window-maximized"] = "no";
        GlobalOptions["window-height"] = QString::number(size().height());
        GlobalOptions["window-width"] = QString::number(size().width());
        GlobalOptions["window-xpos"] = QString::number(pos().x());
        GlobalOptions["window-ypos"] = QString::number(pos().y());
    }

    libJackSMS::localApi::optionManager man("", GlobalOptions);
    man.save();
}

void MainJackSMS::resizeEvent(QResizeEvent * s) {
    resizeRecipientBox();
    if (!firstResize){
        resizeTimer.stop();
        resizeTimer.start(1000);
    }
}*/

MainJackSMS::~MainJackSMS()
{
    if (updateChecker!=NULL)
        delete updateChecker;

    delete trayIco;
    delete ui;
}

void MainJackSMS::resetCounters() {

    for (libJackSMS::dataTypes::configuredServicesType::iterator i = ElencoServiziConfigurati.begin(); i != ElencoServiziConfigurati.end(); ++i) {
        i->setSentAll(0);
    }

    for(libJackSMS::dataTypes::configuredServicesType::iterator i = ElencoServiziConfigurati.begin(); i != ElencoServiziConfigurati.end(); ++i) {
        QString name = i.value().getName();
        int index = ui->comboServizio->findData(name, Qt::UserRole);
        ui->comboServizio->setItemText(index, name);
    }
}

void MainJackSMS::translateGui() {
    // rimuovo intanto che non sono ancora implementate
    /*
    libJackSMS::dataTypes::optionsType::const_iterator iter = GlobalOptions.find("language");
    if (iter != GlobalOptions.end()) {
        libJackSMS::LanguageManager *lm = libJackSMS::LanguageManager::getIstance();

        if (lm->setLanguage(iter.value())) {
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
        }
    }
    */
}

void MainJackSMS::gestiscimenuSingolo() {
    if (invioMultiplo) {
        ui->label->setText("Destinatario");
        invioMultiplo = false;
        if(usaAssociatiPresent)
            ReWriteConfiguredServicesToGui();
    } else {
        invioMultiplo = false;
    }
}

void MainJackSMS::gestiscimenuMultiplo() {
    if (!invioMultiplo) {
        ui->label->setText("Destinatari");
        invioMultiplo = true;
        if(!usaAssociatiPresent)
            ReWriteConfiguredServicesToGui();
    } else {
        invioMultiplo = true;
    }
}

void MainJackSMS::showContactByTypeInFastAbook(){
    ui->RubricaVeloce->clear();

    int section = 1;
    try{
        libJackSMS::dataTypes::phoneBookType::const_iterator i_end = Rubrica.end();

        QMultiMap<QString,contactWidgetFastBook*> fastList;
        for (libJackSMS::dataTypes::phoneBookType::const_iterator i = Rubrica.begin(); i != i_end; ++i) {
            QIcon ico;
            libJackSMS::dataTypes::configuredServicesType::const_iterator x = ElencoServiziConfigurati.find(i->getAccount());
            if (x == ElencoServiziConfigurati.end())
                ico = QIcon(":/resource/ico_contact.png");
            else
                ico = ElencoServizi[x.value().getService()].getIcon();

            contactWidgetFastBook *ww = new contactWidgetFastBook(i.value(), ico.pixmap(16, 16));
            if ((i->getCanReceiveJms()) && (messageType == TYPE_JMS))
                fastList.insert(i->getName().toUpper(), ww);
            else if (messageType == TYPE_SMS)
                fastList.insert(i->getName().toUpper(), ww);
        }

        section++;
        if (fastList.size() > 0) {
            QMultiMap<QString, contactWidgetFastBook*>::ConstIterator xx = fastList.end();
            QMultiMap<QString,contactWidgetFastBook*>::ConstIterator xx_end = fastList.begin();
            do {
                --xx;
                QListWidgetItem *item = new QListWidgetItem;
                item->setSizeHint(xx.value()->size());
                ui->RubricaVeloce->addItem(item);
                ui->RubricaVeloce->setItemWidget(item, xx.value());
            } while(xx != xx_end);
        }
    } catch(...) {
        QMessageBox::critical(this, "JackSMS", "JackSMS ha rilevato un errore grave durante la procedura showContactByTypeInFastAbook().\nDettagli: unknown error, section: " + QString::number(section));
    }
}

void MainJackSMS::WriteAddressBookToGui() {

    int section = 1;
    try {
        libJackSMS::dataTypes::phoneBookType::const_iterator i_end=Rubrica.end();

        QMultiMap<QString, contactWidgetFastBook*> fastList;
        QMultiMap<QString, ContactWidget*> list;

        for (libJackSMS::dataTypes::phoneBookType::const_iterator i = Rubrica.begin(); i != i_end; ++i) {

            QString accountName = "Nessun servizio associato";
            QIcon ico;
            {
                libJackSMS::dataTypes::configuredServicesType::const_iterator x=ElencoServiziConfigurati.find(i->getAccount());
                if (x == ElencoServiziConfigurati.end())
                    ico = QIcon(":/resource/ico_contact.png");
                else
                    ico = ElencoServizi[x.value().getService()].getIcon();

                contactWidgetFastBook *ww = new contactWidgetFastBook(i.value(), ico.pixmap(16, 16));


                if ((i->getCanReceiveJms()) && (messageType == TYPE_JMS))
                    fastList.insert(i->getName().toUpper(), ww);
                else if (messageType == TYPE_SMS)
                    fastList.insert(i->getName().toUpper(), ww);
            }

            {
                libJackSMS::dataTypes::configuredServicesType::const_iterator x=ElencoServiziConfigurati.find(i->getAccount());
                if (x != ElencoServiziConfigurati.end())
                    accountName = x.value().getName();
            }

            ContactWidget *ww = new ContactWidget(
                   i->getId(),
                   i->getName(),
                   i->getPhone().toString(),
                   ico.pixmap(16,16),
                   i->getGroup(),
                   accountName);

            list.insert(i->getName().toUpper(), ww);
        }

        section++;
        if (list.size() > 0) {
            QMultiMap<QString, ContactWidget*>::ConstIterator xx = list.begin();
            QMultiMap<QString, ContactWidget*>::ConstIterator xx_end = list.end();
            for(;xx != xx_end; ++xx){
                QListWidgetItem *item = new QListWidgetItem;
                item->setSizeHint(xx.value()->size());
                ui->rubricaListWidget->addItem(item);
                ui->rubricaListWidget->setItemWidget(item, xx.value());
            }
        }

        section++;
        if (fastList.size() > 0) {
            QMultiMap<QString, contactWidgetFastBook*>::ConstIterator xx = fastList.end();
            QMultiMap<QString, contactWidgetFastBook*>::ConstIterator xx_end = fastList.begin();
            do {
               --xx;
               QListWidgetItem *item = new QListWidgetItem;
               item->setSizeHint(xx.value()->size());
               ui->RubricaVeloce->addItem(item);
               ui->RubricaVeloce->setItemWidget(item, xx.value());
            }while(xx != xx_end);
        }

    } catch(...) {
        QMessageBox::critical(this, "JackSMS", "JackSMS ha rilevato un errore grave durante la procedura WriteAddressBookToGui().\nDettagli: unknown error, section: " + QString::number(section));
    }
}

void MainJackSMS::WriteConfiguredServicesToGui() {

    mapWidgetsAccount.clear();
    try {
        for (libJackSMS::dataTypes::configuredServicesType::const_iterator i = ElencoServiziConfigurati.begin(); i != ElencoServiziConfigurati.end(); ++i) {
            if (i.value().getId() != "1") {
                QIcon ico = ElencoServizi[i.value().getService()].getIcon();
                accountWidget *ww = new accountWidget(i.value().getId(), i.value().getName(), ico.pixmap(16, 16));
                mapWidgetsAccount.insert(i.value().getName().toUpper(), ww);
            }
        }

       for (QMap<QString,accountWidget*>::ConstIterator xx = mapWidgetsAccount.begin(); xx != mapWidgetsAccount.end(); ++xx) {
           addServiceToServiceComboBox(xx.value(), false);
       }

       // aggiungo il servizio "Jacksms Messenger"
       libJackSMS::dataTypes::configuredAccount jmsacc = ElencoServiziConfigurati["1"];
       accountWidget *ww = new accountWidget(jmsacc.getId(), jmsacc.getName(), ElencoServizi[jmsacc.getService()].getIcon().pixmap(16, 16));
       addServiceToServiceComboBox(ww, true);

       // aggiungo il servizio "Usa Account Associati"
       if (invioMultiplo) {
           ui->comboServizio->insertItem(0, QIcon(":/resource/freccia.png"), "Usa Account Associati", "Usa Account Associati");
           ui->comboServizio->insertSeparator(1);
           usaAssociatiPresent = true;
       } else {
           usaAssociatiPresent = false;
       }

       ui->comboServizio->setCurrentIndex(0);

    } catch(...) {
        QMessageBox::critical(this, "JackSMS","JackSMS ha rilevato un errore grave durante la procedura WriteConfiguredServicesToGui().");
    }

}

void MainJackSMS::updateAccountCountComboBox(QString id)
{
    for (libJackSMS::dataTypes::configuredServicesType::const_iterator i = ElencoServiziConfigurati.begin(); i != ElencoServiziConfigurati.end(); ++i) {
        if (i.value().getId() == id) {
            QString name = i.value().getName();
            int index = ui->comboServizio->findData(name, Qt::UserRole);
            ui->comboServizio->setItemText(index, name + " [" + QString::number(ElencoServiziConfigurati[id].getSentAll()) + "]");
            break;
        }
    }
}

QString MainJackSMS::phone2name(const libJackSMS::dataTypes::phoneNumber &_number) {
    QString user_num_name = _number.getAltName();
    static QMap<QString, QString> buf;

    try {
        if (_number.getIsValid()) {
            QMap<QString, QString>::iterator i = buf.find(_number.toString());
            if (i != buf.end()) {
                user_num_name = i.value();
            } else {
                user_num_name = _number.toString();
                bool found = false;
                for(libJackSMS::dataTypes::phoneBookType::const_iterator x = Rubrica.begin(); x != Rubrica.end(); ++x) {

                    if(x->getPhone().toString() == user_num_name) {
                        user_num_name = x->getName();
                        found = true;
                        buf[_number.toString()] = user_num_name;
                        break;
                    } else if(x->getCanReceiveJms() == true){
                        if(x->getVirtualNumber().toString() == user_num_name) {
                            user_num_name = x->getName();
                            found = true;
                            buf[_number.toString()] = user_num_name;
                            break;
                        }
                    }
                }
                if (!found) {
                    if (_number.getAltName() != ""){
                        user_num_name = user_num_name + QString(" (") + _number.getAltName() + QString(")");
                    }
                }
            }
        }
    }catch(...){
    }
    return user_num_name;
}

bool MainJackSMS::isInRubrica(const libJackSMS::dataTypes::phoneNumber &_number) {
    try {
        if (_number.getIsValid()) {
            for(libJackSMS::dataTypes::phoneBookType::const_iterator x = Rubrica.begin(); x != Rubrica.end(); ++x) {
                if(x->getPhone().toString() == _number.toString()) {
                    return true;
                } else if(x->getCanReceiveJms() == true) {
                    if(x->getVirtualNumber().toString() == _number.toString()) {
                        return true;
                    }
                }
            }
        }
    }catch(...){}
    return false;
}

bool MainJackSMS::isInRecipients(const libJackSMS::dataTypes::phoneNumber &_number) {
    try {
        if (_number.getIsValid()) {
            for (int a = 0; a < ui->recipientListWidget->count(); ++a) {
                QListWidgetItem* current = ui->recipientListWidget->item(a);
                QRecipientWidget* widget = dynamic_cast<QRecipientWidget*>(ui->recipientListWidget->itemWidget(current));
                if (_number.toString() == widget->getPhone().toString())
                    return true;
            }
            return false;
        }
    }catch(...){}
    return false;
}

// aggiunge 100 sms alla lista degli SmsWidget ogni volta che la chiamo
void MainJackSMS::WriteMessagesToGui() {

    if(!stopWriteMessagesToGui) {

        ui->smsListWidget->hideCaricaAltri(true);

        if (Messaggi.isEmpty())
            return;

        if (!altriMessaggi)
            return;

        tempCount = 0;
        QTimer::singleShot(MILLISECONDSDELTASMSLOAD, this, SLOT(stepWriteMessageToGui()));

    } else {
        ui->smsListWidget->hideCaricaAltri(true);
    }
}

void MainJackSMS::stepWriteMessageToGui() {

    if (!stopWriteMessagesToGui) {

        --iterMess;
        tempCount++;

        try {

            SmsWidget *wid = NULL;
            if (iterMess->getIsReceived())
                wid = new SmsWidget(*iterMess, icon_jack, true);
            else
                wid = new SmsWidget(*iterMess, ElencoServizi[iterMess->getServiceId()].getIcon().pixmap(16, 16), false);

            QListWidgetItem *item = new QListWidgetItem;
            item->setSizeHint(wid->getSize());
            ui->smsListWidget->addItem(item);
            ui->smsListWidget->setItemWidget(item, wid);

            //controllo se è filtrato dal nome
            if (ui->listSmsNames->currentRow() > 0) {
                if( static_cast<NameWidget*>(ui->listSmsNames->itemWidget(ui->listSmsNames->item(ui->listSmsNames->currentRow())))->getName() == phone2name(wid->getPhoneNum())){
                    wid->setNameFilteredHidden(false);
                } else {
                    ui->smsListWidget->setItemHidden(item, true);
                    wid->setNameFilteredHidden(true);
                }
            } else {
                wid->setNameFilteredHidden(false);
            }

            //controllo se è filtrato dal cerca
            if(ui->RicercaVeloceIM->text() != "") {
                if (wid->isNameFilteredHidden() == false){
                    if (wid->searchMatch(ui->RicercaVeloceIM->text()))
                        ui->smsListWidget->setItemHidden(item, false);
                    else
                        ui->smsListWidget->setItemHidden(item, true);
                }
            }

            ui->listSmsNames->refreshOneBottom(this, wid);

            if (iterMess <= Messaggi.begin()) {
                //era l'ultimo
                ui->smsListWidget->hideCaricaAltri(true);
                altriMessaggi = false;
                setCursor(Qt::ArrowCursor);
                return;
            }

            if (tempCount >= 100) {
                tempCount = 0;
                setCursor(Qt::ArrowCursor);
                ui->smsListWidget->hideCaricaAltri(false);
                return;
            }

            QTimer::singleShot(MILLISECONDSDELTASMSLOAD, this, SLOT(stepWriteMessageToGui()));

        }catch(...){
            QMessageBox::critical(this,"JackSMS","JackSMS ha rilevato un errore grave durante la procedura stepWriteMessagesToGui.");
        }

    } else {
        ui->smsListWidget->hideCaricaAltri(true);
    }
}

void MainJackSMS::on_RubricaAggiungi_clicked()
{
    AggiungiContatto *diag = new AggiungiContatto(this, ElencoServiziConfigurati, Rubrica, ElencoServizi, GlobalOptions, current_login_id);
    connect(diag, SIGNAL(contactAdded(QString)), this, SLOT(contactAdded(QString)));
    diag->exec();
    diag->deleteLater();
}

void MainJackSMS::contactAdded(QString id) {
    ReWriteAddressBookToGui();
    updateSmsListAfterContactAdded(Rubrica[id]);
    ui->RicercaVeloce->setText("");
    ui->TextRapidRubrica->setText("");
    refreshCompleter();
}

void MainJackSMS::displayCaptcha(QByteArray data) {
    /*
            //cerca se c'è un plugin captcha per questo servizio
            JackCaptchaPluginInterface* availablePlugin = 0;
            if(Opzioni["use-captcha"] == "yes"){
                QString serviceName;
                for (jackTypes::ConfiguredServicesMap::const_iterator i = ElencoServiziConfigurati.begin(); i != ElencoServiziConfigurati.end(); ++i) {
                    if (ui->comboServizio->currentText() == i->nome) {
                        serviceName = ElencoServizi[i->idServizio].nome;
                        break;
                    }
                }

                foreach(JackPluginInterface* iPlugin, pluginsList) {
                    if(iPlugin->getPluginInfo().pluginType == CaptchaDecoder) {
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
        if (Opzioni["display-captcha-popup"] == "yes") {
            popupJms = false;
            trayIco->showMessage("JackSMS", "JackSMS richiede l'inserimento del codice ottico.", QSystemTrayIcon::Information);

        }
        CaptchaDialog *dial=new CaptchaDialog(data,QString("100%"),this);
        dial->exec();

        smsSender->continueSend(dial->result);
        dial->deleteLater();
    }catch (QString s) {
        QMessageBox::critical(this,"JackSMS","Si e' verificato un errore.\nImpossibile caricare l'immagine captcha. L'invio verra' interrotto.");
        invioFallito("Errore durante il caricamento del codice ottico.");
    }
}

void MainJackSMS::eseguiPassoInvio() {
}

void MainJackSMS::popupInvio(){
    QMessageBox::information(this,"JackSMS",esitoInvio);
}

void MainJackSMS::clickText(QString _text, QString defaultStr) {

    QSize s = size();
    int maxlenght = ((s.width()-330)/5) - 5; //calcolo molto ma molto approssimativo e fatto a tentativi

    if (!_text.isEmpty()){
        esitoInvio = defaultStr + ": " + _text;
        if (esitoInvio.length() < maxlenght) {
            ui->LabelEsito->setText(esitoInvio);
        } else {
            ui->LabelEsito->setText(esitoInvio.left(maxlenght) + "...");
        }
    } else {
        esitoInvio = defaultStr;
        ui->LabelEsito->setText(defaultStr);
    }
}

void MainJackSMS::smsSaved(libJackSMS::dataTypes::logSmsMessage sms, QString t)
{
    ElencoServiziConfigurati[sms.getAccountId()].setSentAll(t.toInt());
    updateAccountCountComboBox(sms.getAccountId());
}

void MainJackSMS::invioSuccesso(QString _text) {

    QString a = ultimoSms.getData().toString("dd/MM/yyyy");
    QString b = ultimoSms.getData().toString("HH:mm:ss");
    libJackSMS::dataTypes::dateTime dd(a + " " + b);

    libJackSMS::dataTypes::logSmsMessage us(ultimoSms.getPhone(),
                                           ElencoServiziConfigurati[ultimoSms.getAccountId()].getName(),
                                           ultimoSms.getServiceId(), //ElencoServiziConfigurati[idAccount].getService(),
                                           dd,
                                           "",
                                           ultimoSms.getMessage());
    us.setAccountId(ultimoSms.getAccountId());

    if (Opzioni["save-local"] != "no") {
        libJackSMS::localApi::smsLogSaver sav(current_user_directory);
        sav.setMessage(us);
        sav.appendToLogFile();
        ultimoSms.setId(sav.getSavedId());
        ultimoSms.setParsedName(phone2name(ultimoSms.getPhone()));
        Messaggi.push_back(ultimoSms);

        QPixmap icona = ElencoServizi[ultimoSms.getServiceId()].getIcon().pixmap(16, 16);
        SmsWidget *wid = new SmsWidget(ultimoSms,icona, false);

        QListWidgetItem *item = new QListWidgetItem;
        item->setSizeHint(wid->getSize());

        ui->smsListWidget->insertItem(0, item);
        ui->smsListWidget->setItemWidget(item, wid);
        ui->listSmsNames->itemAdded(wid, false);

        //nascondi se non è la conversazione corretta o se non è "tutte" (dovrebbe essere su tutte per controlli precedenti)
        if (ui->listSmsNames->currentRow() > 0)
            if (static_cast<NameWidget*>(ui->listSmsNames->itemWidget(ui->listSmsNames->currentItem()))->getName() != wid->getName())
                ui->smsListWidget->setItemHidden(item, true);

        //nascondi se è filtrato dal cerca (il cerca dovrebbe essere vuoto dati controlli precedenti)
        if (ui->RicercaVeloceIM->text() != "")
            if(!wid->searchMatch(ui->RicercaVeloceIM->text()))
                ui->smsListWidget->setItemHidden(item, true);
    }

    AbilitaUi();

    libJackSMS::localApi::statsManager man(current_user_directory);
    man.increaseSentStat(ElencoServiziConfigurati[ultimoSms.getAccountId()]);

    ElencoServiziConfigurati[ultimoSms.getAccountId()].increaseStatValue("sent");

    onlineSmsSaver = new libJackSMS::serverApi::smsLogSaver(current_login_id, GlobalOptions);
    connect(onlineSmsSaver, SIGNAL(smsSaved(libJackSMS::dataTypes::logSmsMessage, QString)), this, SLOT(smsSaved(libJackSMS::dataTypes::logSmsMessage, QString)));
    onlineSmsSaver->save(us);

    if (iterateSendSms(false, true, "Messaggio inviato" + (_text.isEmpty() ? "" : ": " + _text)) == 0) {
        if (Opzioni["successfull-send-popup"] == "yes") {
            popupJms = false;
            trayIco->showMessage("JackSMS","Messaggio inviato!" + (_text.isEmpty() ? "" : "\n" + _text), QSystemTrayIcon::Information);
        }

        if (Opzioni["svuota-invio-corretto"] == "yes") {
            svuotaTabSms();
            ui->LabelCountChars->hide();
            //ui->LabelEsito->show();
        }

        clickText(_text, "Messaggio inviato");
        invioInCorso = false;
        ui->recipientLine->setFocus();
    }
}

void MainJackSMS::invioFallito(QString _text) {

    if(!invioMultiplo)
        clickText(_text,"Messaggio non inviato");

    AbilitaUi();

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
    onlineSmsSaverFailCase = new libJackSMS::serverApi::smsLogFailed(current_login_id, GlobalOptions);
    onlineSmsSaverFailCase->reportFail(us,_text);

    if(iterateSendSms(false, false, "Messaggio non inviato" + (_text.isEmpty() ? "" : ": " + _text)) == 0) {
        if (Opzioni["error-send-popup"] != "no") {
            popupJms = false;
            trayIco->showMessage("JackSMS", "Messaggio non inviato" + (_text.isEmpty() ? "" : "\n" + _text), QSystemTrayIcon::Critical);
        }

        clickText(_text, "Messaggio non inviato");
        invioInCorso = false;
        ui->InviaSMS->setFocus();
    }
}

int MainJackSMS::iterateSendSms(bool first, bool result, QString _text) {

    clickText("", "Invio in corso");
    if ((!first) && (multipleSendRecipients.size() > 0)) {
        QListWidgetItem* current = multipleSendRecipients.first();
        QRecipientWidget* widget = dynamic_cast<QRecipientWidget*>(ui->recipientListWidget->itemWidget(current));
        if (result) {
            widget->setStatusSuccess(_text);
            if (Opzioni["svuota-invio-corretto"] == "yes") {
                ui->recipientListWidget->takeItem(ui->recipientListWidget->row(current));
                resizeRecipientBox();
            }
        } else {
            widget->setStatusFailed(_text);
            errorSentCounter++;
        }
        multipleSendRecipients.removeFirst();
    }

    if (multipleSendRecipients.size() > 0) {
        QListWidgetItem* current = multipleSendRecipients.first();
        QRecipientWidget* widget = dynamic_cast<QRecipientWidget*>(ui->recipientListWidget->itemWidget(current));
        widget->setStatusWorking();

        DisabilitaUi();

        libJackSMS::dataTypes::shortMessage messaggio(ui->TestoSMS->toPlainText());
        QString idAccount="1";
        bool found = false;

        if((messageType != TYPE_JMS) && usaAssociatiPresent && (ui->comboServizio->currentIndex() == 0)){
            // Usa Account Associati
            libJackSMS::dataTypes::configuredServicesType::const_iterator i = ElencoServiziConfigurati.find(widget->getAccountId());

            if (i != ElencoServiziConfigurati.end()) {
                found = true;
                idAccount = i.key();
            }

        } else {
            // Usa account Selezionato
            if (messageType != TYPE_JMS) {

                for(libJackSMS::dataTypes::configuredServicesType::const_iterator i = ElencoServiziConfigurati.begin(); i != ElencoServiziConfigurati.end(); ++i){
                    if (i.value().getName() == ui->comboServizio->itemData(ui->comboServizio->currentIndex(),Qt::UserRole).toString()){
                        found = true;
                        idAccount = i.key();
                        break;
                    }
                }
            }
        }

        if (!found) {
            for(libJackSMS::dataTypes::configuredServicesType::const_iterator i=ElencoServiziConfigurati.begin(); i != ElencoServiziConfigurati.end(); ++i){
                if (i.value().getName() == "JackSMS Messenger") {
                    found = true;
                    idAccount = i.key();
                    break;
                }
            }
        }

        ultimoSms = QMyMessage();
        ultimoSms.setMessage(ui->TestoSMS->toPlainText());
        ultimoSms.setAccountId(idAccount);

        if (messageType == TYPE_JMS) {
            ultimoSms.setAccountName("JackSMS Messenger");
            ultimoSms.setServiceId("40");
        } else if (messageType == TYPE_SMS) {
            ultimoSms.setAccountName(ElencoServiziConfigurati[idAccount].getName());
            ultimoSms.setServiceId(ElencoServiziConfigurati[idAccount].getService());
        }

        ultimoSms.setIsReceived(false);
        ultimoSms.setData(QDateTime::currentDateTime());
        ultimoSms.setPhone(widget->getPhone());

        smsSender = new libJackSMS::smsSender(ElencoServizi, GlobalOptions);
        smsSender->setRecipient(widget->getPhone());
        smsSender->setMessage(messaggio);

        smsSender->setAccount(ElencoServiziConfigurati[idAccount]);

        connect(smsSender, SIGNAL(captcha(QByteArray)), this, SLOT(displayCaptcha(QByteArray)));
        connect(smsSender, SIGNAL(error(QString)), this, SLOT(invioFallito(QString)));
        connect(smsSender, SIGNAL(success(QString)), this, SLOT(invioSuccesso(QString)));
        connect(smsSender, SIGNAL(sleepBeforeFound(int)), this, SLOT(sleepBeforeFound(int)));

        if (Opzioni["dont-cookies"] == "yes")
            smsSender->setSalvaCookies(false);

        smsSender->send();

    } else if (smsCount > 1) {
        invioInCorso = false;
        //fine invio messaggi
        if(errorSentCounter == 0){

            if (Opzioni["svuota-invio-corretto"] == "yes") {
                ui->comboServizio->setCurrentIndex(0);
                svuotaTabSms();
                ui->LabelCountChars->hide();
                //ui->LabelEsito->show();
            }

            if (Opzioni["successfull-send-popup"] == "yes"){
                popupJms = false;
                trayIco->showMessage("JackSMS", "Messaggi inviati!", QSystemTrayIcon::Information);
            }

            clickText("","Messaggi inviati");
            ui->recipientLine->setFocus();

        }else{
            //alcuni messaggi non inviati
            if (Opzioni["error-send-popup"] != "no") {
                popupJms = false;
                trayIco->showMessage("JackSMS", "Attenzione: " + QString::number(errorSentCounter) + ((errorSentCounter==1) ? " messaggio non inviato!" : " messaggi non inviati!"), QSystemTrayIcon::Critical);
            }

            clickText(QString::number(errorSentCounter) + ((errorSentCounter==1) ? " messaggio non inviato!" : " messaggi non inviati!"), "Attenzione");
            ui->InviaSMS->setFocus();
        }

        return -1;

        ui->listSmsNames->clearSelection();
        ui->smsListWidget->clearSelection();
        if (ui->RicercaVeloceIM->text() != "")
            ui->RicercaVeloceIM->setText("");
        ui->listSmsNames->setCurrentRow(0);

    }

    return multipleSendRecipients.size();
}

void MainJackSMS::on_InviaSMS_clicked()
{
    invioInCorso = true;
    ui->listSmsNames->clearSelection();
    ui->smsListWidget->clearSelection();
    if (ui->RicercaVeloceIM->text() != "")
        ui->RicercaVeloceIM->setText("");
    ui->listSmsNames->setCurrentRow(0);

    clearRecipientsStatus();
    elaboraRecipientLine();
    multipleSendRecipients.clear();

    if (ui->TestoSMS->toPlainText().isEmpty()) {
        QMessageBox::information(this, "JackSMS", "Il testo del messaggio e' vuoto");
        invioInCorso = false;
        ui->TestoSMS->setFocus();
        return;
    }

    if (ui->recipientListWidget->count() == 0) {
        QMessageBox::information(this, "JackSMS", "Il destinatario non e' stato specificato.");
        invioInCorso = false;
        ui->recipientLine->setFocus();
        return;
    }

    DisabilitaUi();

    //libJackSMS::dataTypes::shortMessage messaggio(ui->TestoSMS->toPlainText());

    QString idAccount = "1";
    if (messageType != TYPE_JMS) {
        for (libJackSMS::dataTypes::configuredServicesType::const_iterator i = ElencoServiziConfigurati.begin(); i != ElencoServiziConfigurati.end(); i++) {
            if (i.value().getName() == ui->comboServizio->itemData(ui->comboServizio->currentIndex(), Qt::UserRole).toString()) {
                idAccount = i.key();
                break;
            }
        }
    }

    for (int a = 0; a < ui->recipientListWidget->count(); ++a) {
        multipleSendRecipients.append(ui->recipientListWidget->item(a));
    }

    smsCount = multipleSendRecipients.size();
    errorSentCounter = 0;
    iterateSendSms(true);
}

void MainJackSMS::on_RubricaModifica_clicked()
{
    QListWidgetItem *wid = ui->rubricaListWidget->currentItem();
    if (wid != NULL)
        on_rubricaListWidget_itemDoubleClicked(wid);
}

void MainJackSMS::on_actionEsci_triggered()
{
    close();
}

void MainJackSMS::on_AggiungiServizioButton_clicked()
{
    ServicesDialog *sd = new ServicesDialog(this, this, ElencoServizi, ElencoServiziConfigurati, GlobalOptions);
    connect(sd, SIGNAL(rewriteAccounts()), this,SLOT(ReWriteConfiguredServicesToGui()));
    sd->exec();
    sd->deleteLater();
}

void MainJackSMS::on_EliminaServizioButton_clicked()
{
    QListWidgetItem *wid = ui->listServiziConfigurati->currentItem();
    if (wid != NULL) {
        if (0 == QMessageBox::question(this, "JackSMS", "Eliminare questo account?", tr("&Si"), tr("&No"))){
            QWidget *wi = ui->listServiziConfigurati->itemWidget(wid);
            accountWidget *cw = static_cast<accountWidget*>(wi);
            //QMessageBox::information(this,"JackSMS",cw->getName());
            QString id = cw->getAccountId();
            ui->labelSpinDelAccount->show();
            ui->EliminaServizioButton->setEnabled(false);

            accountManager = new libJackSMS::serverApi::accountManager(current_login_id, GlobalOptions);
            connect(accountManager, SIGNAL(accountDeleted(QString)), this, SLOT(deleteAccountOk(QString)));
            connect(accountManager, SIGNAL(accountNotDeleted()), this, SLOT(deleteAccountKo()));
            accountManager->deleteAccount(id);
        }
    }
}

void MainJackSMS::on_actionOpzioni_triggered()
{
   OpzioniDialog *dial = new OpzioniDialog(Opzioni, GlobalOptions, current_user_directory, this, loggedIn, ui->password->text());
   connect(dial, SIGNAL(translate()), this, SLOT(translateGui()));
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
        libJackSMS::localApi::xmlLoader caricatore(current_user_directory.toStdString());
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

void MainJackSMS::on_InoltraButton_clicked() {

    QList<QListWidgetItem*> ls = ui->smsListWidget->selectedItems();

    if (!ls.isEmpty()) {
        ui->RubricaVeloce->clearSelection();
        ui->recipientListWidget->clear();
        ui->recipientLine->setText("");
        QListWidgetItem* it = ls.front();

        SmsWidget* ww = static_cast<SmsWidget*>(ui->smsListWidget->itemWidget(it));
        if(ww->isCaricaAltri())
            return;

        gestiscimenuSingolo();
        svuotaDestinatari();
        ui->TestoSMS->setText(ww->getText());
        ui->recipientLine->setFocus();
        ui->tabWidget->setCurrentIndex(0);
    }
}

void MainJackSMS::on_AnnullaSMS_clicked()
{
    smsSender->abort();
    if (invioMultiplo)
        multipleSendRecipients.clear();

    ui->LabelEsito->setText("Invio annullato dall'utente.");
    invioInCorso = false;
    ui->InviaSMS->setFocus();
    AbilitaUi();
}

void MainJackSMS::on_CitaButton_clicked()
{
    QList<QListWidgetItem*> ls = ui->smsListWidget->selectedItems();

    if (!ls.isEmpty()) {
        ui->RubricaVeloce->clearSelection();
        ui->recipientListWidget->clear();
        gestiscimenuSingolo();
        QListWidgetItem *it = ls.front();

        SmsWidget* ww = static_cast<SmsWidget*>(ui->smsListWidget->itemWidget(it));
        if(ww->isCaricaAltri())
            return;

        QString phone = ww->getPhoneNum().toString();
        ui->TestoSMS->setText("<<[ " + ww->getText() + " ]>>");

        bool found = false;
        for (libJackSMS::dataTypes::phoneBookType::const_iterator a = Rubrica.begin(); a != Rubrica.end(); ++a) {
            if (a.value().getPhone().toString() == phone) {
                ui->recipientLine->setText(a.value().getName() + " <" + phone + ">");
                found = true;
                break;
            }
        }

        if (!found)
            ui->recipientLine->setText(phone);

        for(libJackSMS::dataTypes::configuredServicesType::const_iterator x = ElencoServiziConfigurati.begin(); x != ElencoServiziConfigurati.end(); x++) {
            if (x.value().getService() == "40") {
                int index = ui->comboServizio->findData(x.value().getName(),Qt::UserRole);
                ui->comboServizio->setCurrentIndex(index);
                break;
            }
        }
        elaboraRecipientLine(false);
        ui->TestoSMS->setFocus();
        QTextCursor cursor(ui->TestoSMS->textCursor());
        cursor.movePosition(QTextCursor::End);
        ui->TestoSMS->setTextCursor(cursor);
        ui->tabWidget->setCurrentIndex(0);
    }
}

void MainJackSMS::on_RispondiButton_clicked()
{
    QList<QListWidgetItem*> ls = ui->smsListWidget->selectedItems();

    if (!ls.isEmpty()) {
        ui->RubricaVeloce->clearSelection();
        ui->recipientListWidget->clear();
        gestiscimenuSingolo();
        QListWidgetItem *it = ls.front();

        SmsWidget* ww = static_cast<SmsWidget*>(ui->smsListWidget->itemWidget(it));
        if (ww->isCaricaAltri())
            return;

        libJackSMS::dataTypes::phoneNumber phone = ww->getPhoneNum();

        ui->TestoSMS->setText("");

        bool found = false;
        for (libJackSMS::dataTypes::phoneBookType::const_iterator a = Rubrica.begin(); a != Rubrica.end(); ++a) {
            if (a.value().getPhone() == phone) {
                ui->recipientLine->setText(a.value().getName() + " <" + phone.toString() + ">");
                found = true;
                break;
            }
        }

        if (!found)
            ui->recipientLine->setText(phone.toString());

        for (libJackSMS::dataTypes::configuredServicesType::const_iterator x = ElencoServiziConfigurati.begin(); x != ElencoServiziConfigurati.end(); x++) {
            if (x.value().getService() == "40") {
                int index = ui->comboServizio->findData(x.value().getName(), Qt::UserRole);
                ui->comboServizio->setCurrentIndex(index);
                break;
            }
        }

        elaboraRecipientLine(false);
        ui->tabWidget->setCurrentIndex(0);
        ui->TestoSMS->setFocus();
    }
}

void MainJackSMS::ClickBaloon() {

    raise();
    activateWindow();
    showNormal();

    if (popupJms) {
        ui->tabWidget->setCurrentIndex(1);
        popupJms = false;
    }
}

bool MainJackSMS::checkDoubleRecipients(libJackSMS::dataTypes::phoneNumber &_n) const {
    /*int c=ui->destinatariListWidget->count();
    for(int i=0;i<c;++i){
        QListWidgetItem *item=ui->destinatariListWidget->item(i);
        contactWidgetFastBook * w=static_cast<contactWidgetFastBook*>(ui->destinatariListWidget->itemWidget(item));
        if (_n==w->getContact().getPhone())
            return true;
    }*/
    return false;
}

void MainJackSMS::on_TextRapidRubrica_textChanged(QString txt)
{
    if (txt == "") {
        ui->RicercaButton->setEnabled(false);
        ui->RicercaButton->setIcon(QIcon(":/resource/findd.png"));
        ui->RicercaButton->setCursor(Qt::ArrowCursor);
    } else {
        ui->RicercaButton->setEnabled(true);
        ui->RicercaButton->setIcon(QIcon(":/resource/finde.png"));
        ui->RicercaButton->setCursor(Qt::PointingHandCursor);
    }

    //ui->rubricaListWidget->clear();
     //QMultiMap<QString,ContactWidget*> list;

     int c = ui->rubricaListWidget->count();
     for (int x = 0; x < c; x++) {
         QListWidgetItem *item = ui->rubricaListWidget->item(x);
         ContactWidget *w = static_cast<ContactWidget*>(ui->rubricaListWidget->itemWidget(item));
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
    QListWidgetItem* wid = ui->rubricaListWidget->currentItem();
    if (wid != NULL) {
        if (0 == QMessageBox::question(this, "JackSMS", "Eliminare questo contatto?", tr("&Si"), tr("&No"))) {
            QWidget* wi = ui->rubricaListWidget->itemWidget(wid);
            ContactWidget* cw = static_cast<ContactWidget*>(wi);

            QString id = cw->getContactId();
            ui->labelSpinDelContact->show();
            ui->RubricaElimina->setEnabled(false);

            deleteContect = new libJackSMS::serverApi::contactManager(current_login_id, GlobalOptions);
            connect(deleteContect, SIGNAL(contactDeleted(QString)), this, SLOT(deleteContactOk(QString)));
            connect(deleteContect, SIGNAL(contactNotDeleted()), this, SLOT(deleteContactKo()));
            deleteContect->deleteContact(id);
        }
    }
}

void MainJackSMS::deleteContactKo()
{
    ui->RubricaElimina->setEnabled(true);
    QMessageBox::critical(this,"JackSMS","Errore durante l'eliminazione del contatto.");
}

void MainJackSMS::deleteContactOk(QString id)
{
    libJackSMS::dataTypes::contact c = Rubrica[id];
    Rubrica.remove(id);
    ReWriteAddressBookToGui();
    updateSmsListAfterContactRemoved(c);
    ui->labelSpinDelContact->hide();
    ui->RubricaElimina->setEnabled(true);
    ui->RicercaVeloce->setText("");
    ui->TextRapidRubrica->setText("");
    refreshCompleter();
}

void MainJackSMS::on_RicercaVeloceIM_textChanged(QString text)
{
    if (text == "") {
        ui->RicercaImButton->setEnabled(false);
        ui->RicercaImButton->setIcon(QIcon(":/resource/findd.png"));
        ui->RicercaImButton->setCursor(Qt::ArrowCursor);
    } else {
        ui->RicercaImButton->setEnabled(true);
        ui->RicercaImButton->setIcon(QIcon(":/resource/finde.png"));
        ui->RicercaImButton->setCursor(Qt::PointingHandCursor);
    }

    int c = ui->smsListWidget->count();
    if (altriMessaggi)
        c--;

    for (int x = 0; x < c; x++) {
        QListWidgetItem *item = ui->smsListWidget->item(x);
        SmsWidget* w = static_cast<SmsWidget*>(ui->smsListWidget->itemWidget(item));
        if (!w->isCaricaAltri() && (w->isNameFilteredHidden() == false)) {
            if (w->searchMatch(text))
                ui->smsListWidget->setItemHidden(item, false);
            else
                ui->smsListWidget->setItemHidden(item, true);
        }
    }
}

void MainJackSMS::on_EliminaButton_clicked() {

    QList<QListWidgetItem*> ls = ui->smsListWidget->selectedItems();
    QList<QListWidgetItem*>::const_iterator i;

    if (!ls.isEmpty()) {
        for (i = ls.begin(); i < ls.end(); ++i) {
            if(static_cast<SmsWidget*>(ui->smsListWidget->itemWidget(*i))->isCaricaAltri())
                ls.removeOne(*i);
        }
    }

    if (!ls.isEmpty()) {
        QList<QString> smsIds;
        QList<QString> imIds;
        bool needRefresh = false;

        QString questionmessage = (ls.count() == 1 ? "Eliminare questo messaggio?" : "Eliminare questi messaggi?");
        if (0 == QMessageBox::question(this, "JackSMS", questionmessage, tr("&Si"), tr("&No"))){

            while (!ls.isEmpty()){

                QListWidgetItem *it = ls.front();

                SmsWidget * ww = static_cast<SmsWidget*>(ui->smsListWidget->itemWidget(it));
                if(!ww->isCaricaAltri()){

                    if (ww->isReceived()){
                        imIds.push_back(ww->getId());
                    }else{
                        smsIds.push_back(ww->getId());
                    }

                    ui->smsListWidget->takeItem(ui->smsListWidget->row(it));

                    if(!needRefresh)
                        needRefresh = ui->listSmsNames->checkNeedRefresh(ww->getId(), ui->smsListWidget);

                }
                ls.removeFirst();
            }



            for (types::QMessageListType::iterator i = Messaggi.begin(); i != Messaggi.end(); ++i){
                QString id = i->getId();
                int found = -1;
                if (!i->getIsReceived())
                    found = smsIds.indexOf(id);
                else
                    found = imIds.indexOf(id);
                if (found >= 0){
                    i = Messaggi.erase(i);
                    i--;
                }
            }

            bool val = true;
            if (!imIds.empty()){
                libJackSMS::localApi::imLogSaver man(current_user_directory);
                val=val && man.deleteIm(imIds);
            }
            if (!smsIds.empty()){
                libJackSMS::localApi::smsLogSaver man(current_user_directory);
                val=val && man.deleteSms(smsIds);
            }

            if (needRefresh) {
                QString nameBefore;
                if (ui->listSmsNames->currentRow() > 0)
                    nameBefore = static_cast<NameWidget*>(ui->listSmsNames->itemWidget(ui->listSmsNames->item(ui->listSmsNames->currentRow())))->getName();
                else
                    nameBefore = "_AllContacts_";

                ui->listSmsNames->clearSelection();
                ui->listSmsNames->refreshAll(this, ui->smsListWidget);

                if (nameBefore == "_AllContacts_") {
                    ui->listSmsNames->setCurrentRow(0);
                } else {
                    bool found = false;
                    for (int i = 1; i < ui->listSmsNames->count(); i++) {
                        if (static_cast<NameWidget*>(ui->listSmsNames->itemWidget(ui->listSmsNames->item(i)))->getName() == nameBefore) {
                            found = true;
                            ui->listSmsNames->setCurrentRow(i);
                            break;
                        }
                    }

                    if (!found)
                        ui->listSmsNames->setCurrentRow(0);

                }
                ui->smsListWidget->scrollToTop();
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

bool MainJackSMS::eventFilter(QObject *obj, QEvent *ev)
{
    return false;
}

void MainJackSMS::on_TestoSMS_textChanged()
{
    QString txt = ui->TestoSMS->toPlainText();
    ui->LabelCountChars->show();
    ui->LabelEsito->setText("");

    if(invioMultiplo && (ui->comboServizio->currentIndex() == 0)) {

        ui->LabelCountChars->setText("Testo: " + QString::number(txt.length()) + " caratteri");

    } else {
        QString cSms = "";
        if (txt.length() > currentSingleLength)
            cSms = QString::number(ceil((float)txt.length()/currentSmsDivisor));
        else
            cSms = QString::number(ceil((float)txt.length()/currentSingleLength));

        QString nSms = QString::number(ceil((float)currentMaxLength/currentSingleLength));
        ui->LabelCountChars->setText("Testo: " + QString::number(txt.length()) + "/" + QString::number(currentMaxLength) + ", SMS: " + cSms + "/" + nSms);
    }
}

void MainJackSMS::on_comboServizio_currentIndexChanged(int index)
{
    if (invioMultiplo && (index == 0)) {

        for(int i = 0; i < ui->recipientListWidget->count(); ++i) {
            dynamic_cast<QRecipientWidget*>(ui->recipientListWidget->itemWidget(ui->recipientListWidget->item(i)))->restoreOriginalIcon();
        }

    } else {

        QIcon ico = ui->comboServizio->itemIcon(index);
        for (int i = 0; i < ui->recipientListWidget->count(); ++i) {
            dynamic_cast<QRecipientWidget*>(ui->recipientListWidget->itemWidget(ui->recipientListWidget->item(i)))->setIcon(ico.pixmap(16, 16));
        }

        for (libJackSMS::dataTypes::configuredServicesType::const_iterator x = ElencoServiziConfigurati.begin(); x != ElencoServiziConfigurati.end(); x++) {
            if (x.value().getName() == ui->comboServizio->itemData(index, Qt::UserRole).toString()) {
                QString serviceId = x.value().getService();
                libJackSMS::dataTypes::servicesType::const_iterator i = ElencoServizi.find(serviceId);
                currentMaxLength = i.value().getIntMaxLength();
                currentSingleLength = i.value().getIntSingleLength();
                currentSmsDivisor = i.value().getIntSmsDivisor();
                break;
            }
        }

    }

    on_TestoSMS_textChanged();
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
    ui->LabelEsito->setEnabled(false);
    ui->LabelCountChars->hide();
    ui->progressBar->show();
    ui->InviaSMS->hide();
    ui->AnnullaSMS->show();
    ui->AnnullaSMS->setEnabled(true);
    ui->RubricaVeloce->setEnabled(false);
    ui->TestoSMS->setEnabled(false);
    ui->comboServizio->setEnabled(false);
    ui->recipientLine->setEnabled(false);
    ui->buttonAddContacts->setEnabled(false);
    //ui->NumeroDestinatario->setEnabled(false);

    ui->menuInstruments->setEnabled(false);
    ui->rubricaBar->setEnabled(false);
    ui->ModificaServizioButton->setEnabled(false);
    ui->EliminaServizioButton->setEnabled(false);
    ui->RubricaElimina->setEnabled(false);
    ui->RubricaModifica->setEnabled(false);
    ui->CitaButton->setEnabled(false);
    ui->InoltraButton->setEnabled(false);
    ui->RispondiButton->setEnabled(false);

}
void MainJackSMS::AbilitaUi(){
    ui->progressBar->hide();
    ui->LabelEsito->setEnabled(true);
    ui->InviaSMS->show();
    ui->AnnullaSMS->hide();
    ui->AnnullaSMS->setEnabled(false);
    ui->RubricaVeloce->setEnabled(true);
    ui->TestoSMS->setEnabled(true);
    ui->comboServizio->setEnabled(true);
    ui->menuInstruments->setEnabled(true);
    ui->rubricaBar->setEnabled(true);
    ui->ModificaServizioButton->setEnabled(true);
    ui->EliminaServizioButton->setEnabled(true);
    ui->RubricaElimina->setEnabled(true);
    ui->RubricaModifica->setEnabled(true);
    ui->CitaButton->setEnabled(true);
    ui->InoltraButton->setEnabled(true);
    ui->RispondiButton->setEnabled(true);
    ui->recipientLine->setEnabled(true);
    ui->buttonAddContacts->setEnabled(true);
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

void MainJackSMS::stopIm() {
    imChecker->stop();
}

void MainJackSMS::jmsActive() {
    //libJackSMS::LanguageManager *lm=libJackSMS::LanguageManager::getIstance();
    //ui->buttonStatusJms->setText(lm->getString(28));
    if (backupImChecker != NULL) {
        backupImChecker->stop();
        connect(imChecker, SIGNAL(serviceNotActive(bool, QString)), this, SLOT(jmsNotActive(bool, QString)));
        connect(imChecker, SIGNAL(serviceActiving()), this, SLOT(jmsActiving()));
        backupImChecker->deleteLater();
        backupImChecker = NULL;
    }

    if (Opzioni["show-popup-jms-status"] != "no")
        if (ui->buttonStatusJms->text() == "Disattivo")
            trayIco->showMessage("JackSMS Messenger", "Servizio Riattivato");

    ui->buttonStatusJms->setText("Attivo");
    ui->buttonStatusJms->setIcon(QIcon(":/resource/jms-active.png"));
    imServiceActive = true;
}

void MainJackSMS::jmsNotActive(bool err, QString str) {

    //libJackSMS::LanguageManager *lm = libJackSMS::LanguageManager::getIstance();
    if (err) {

        disconnect(imChecker, SIGNAL(serviceNotActive(bool, QString)), this, SLOT(jmsNotActive(bool, QString)));
        disconnect(imChecker, SIGNAL(serviceActiving()), this, SLOT(jmsActiving()));

        popupJms = false;
        imServiceActive = false;
        lastJmsError = str;

        backupImChecker = new libJackSMS::serverApi::cyclicMessengerChecker(current_login_id, GlobalOptions);
        connect(backupImChecker, SIGNAL(newJMS(libJackSMS::dataTypes::logImType)), this, SLOT(checkInstantMessengerReceived(libJackSMS::dataTypes::logImType)));
        connect(backupImChecker, SIGNAL(serviceNotActive()), this, SLOT(jmsBackupNotActive()));
        connect(backupImChecker, SIGNAL(serviceActive()), this, SLOT(jmsBackupActive()));
        backupImChecker->activateServ();

    } else {

        //ui->buttonStatusJms->setText(lm->getString(30));
        ui->buttonStatusJms->setText("Disattivo");
        ui->buttonStatusJms->setIcon(QIcon(":/resource/jms-not-active.png"));
        imServiceActive = false;

    }
}

void MainJackSMS::jmsBackupNotActive()
{
    if (ui->buttonStatusJms->text() != "Disattivo") {
        ui->buttonStatusJms->setText("Disattivo");
        ui->buttonStatusJms->setIcon(QIcon(":/resource/jms-not-active.png"));

        if (Opzioni["show-popup-jms-status"] != "no")
            trayIco->showMessage("JackSMS Messenger", "Servizio Disattivato\n" + lastJmsError);
    }
}

void MainJackSMS::jmsBackupActive()
{

    if (Opzioni["show-popup-jms-status"] != "no")
        if (ui->buttonStatusJms->text() == "Disattivo")
            trayIco->showMessage("JackSMS Messenger", "Servizio Riattivato");

    ui->buttonStatusJms->setText("Attivo");
    ui->buttonStatusJms->setIcon(QIcon(":/resource/jms-active.png"));
}

void MainJackSMS::jmsActiving()
{
    //libJackSMS::LanguageManager *lm=libJackSMS::LanguageManager::getIstance();
    //ui->buttonStatusJms->setText(lm->getString(29));
    ui->buttonStatusJms->setText("Connessione");
    ui->buttonStatusJms->setIcon(QIcon(":/resource/jms-activing.png"));
    imServiceActive = false;
}

void MainJackSMS::startIm()
{
        messaggiRicevuti.clear();
        imServiceActive = false;

        imChecker = new libJackSMS::serverApi::Streamer(current_user_username,ui->password->text(), "", GlobalOptions);
        connect(imChecker, SIGNAL(newJMS(libJackSMS::dataTypes::logImType)), this, SLOT(checkInstantMessengerReceived(libJackSMS::dataTypes::logImType)));
        connect(imChecker, SIGNAL(serviceActive()), this, SLOT(jmsActive()));
        connect(imChecker, SIGNAL(serviceNotActive(bool, QString)), this, SLOT(jmsNotActive(bool, QString)));
        connect(imChecker, SIGNAL(serviceActiving()), this, SLOT(jmsActiving()));
        imChecker->activateServ();
}

void MainJackSMS::checkInstantMessengerReceived(libJackSMS::dataTypes::logImType jmsList) {

    int section = 1;

    try{
        ui->listSmsNames->clearSelection();
        ui->smsListWidget->clearSelection();
        if (ui->RicercaVeloceIM->text() != "")
            ui->RicercaVeloceIM->setText("");
        ui->listSmsNames->setCurrentRow(0);

        QMultiMap<QDateTime,SmsWidget*> mapWidgetsNew;
        libJackSMS::localApi::imLogSaver *manager = new libJackSMS::localApi::imLogSaver(current_user_directory);
        bool save = true;

        if (Opzioni["save-local"] == "no")
            save = false;

        for (libJackSMS::dataTypes::logImType::iterator i = jmsList.begin(); i != jmsList.end(); ++i) {
            if (save) {
                manager->setMessage(i.value());
                manager->appendToLogFile();
                i.value().setId(manager->getSavedId());
            } else {
                //generating temporary id
                i.value().setId(QString::number(Messaggi.size() + 1));
            }

            QMyMessage msg;
            msg.setParsedName(phone2name(i.value().getPhoneNumber()));
            msg.setAccountId(QString::number(0));
            msg.setData(i.value().getDate().getData());
            msg.setMessage(i.value().getText());
            msg.setId(i.value().getId());
            msg.setIsReceived(true);
            msg.setPhone(i.value().getPhoneNumber());
            Messaggi.push_back(msg);
            msg.setReaded(false);
            msg.setAccountName("JackSMS Messenger");
            MessaggiRicevuti.push_back(msg);
            countReceivedUnreaded++;
            ui->tabWidget->setTabText(1, "Conversazioni (" + QString::number(countReceivedUnreaded) + ")");

            SmsWidget *wid = new SmsWidget(msg,icon_jack, true);
            mapWidgetsNew.insert(i.value().getDate().getData(), wid);

            //relays to listening plugins the received message
            jphi->emitJIMreceived(*i);

        }

        section++;

        if (mapWidgetsNew.size() > 0) {

            for(QMultiMap<QDateTime,SmsWidget*>::ConstIterator xx = mapWidgetsNew.begin(); xx != mapWidgetsNew.end(); ++xx){
                QListWidgetItem *item = new QListWidgetItem;
                item->setSizeHint(static_cast<SmsWidget*>(xx.value())->getSize());
                ui->smsListWidget->insertItem(0,item);
                ui->smsListWidget->setItemWidget(item, xx.value());
                ui->listSmsNames->itemAdded(static_cast<SmsWidget*>(xx.value()), true);

                //nascondi se non è la conversazione corretta o se non è "tutte"
                if(ui->listSmsNames->currentRow() > 0)
                    if(static_cast<NameWidget*>(ui->listSmsNames->itemWidget(ui->listSmsNames->currentItem()))->getName() != static_cast<SmsWidget*>(xx.value())->getName())
                        ui->smsListWidget->setItemHidden(item, true);

                //nascondi se è filtrato dal cerca (il cerca dovrebbe essere vuoto dati controlli precedenti)
                if (ui->RicercaVeloceIM->text() != "")
                    if(!static_cast<SmsWidget*>(xx.value())->searchMatch(ui->RicercaVeloceIM->text()))
                        ui->smsListWidget->setItemHidden(item, true);
            }
        }

        ui->listSmsNames->clearSelection();
        ui->smsListWidget->clearSelection();
        if (ui->RicercaVeloceIM->text() != "")
            ui->RicercaVeloceIM->setText("");
        ui->listSmsNames->setCurrentRow(0);

        /*****************************end append im to gui***************************/

        section++;

        setTrayIcon();
        section++;
        popupJms = true;

        if (Opzioni["show-popup-new-jms"] != "no") {

            if (jmsList.size() == 1)
                trayIco->showMessage("JackSMS Messenger", "Ricevuto un nuovo JMS");
            else
                trayIco->showMessage("JackSMS Messenger", "Ricevuti " + QString::number(jmsList.size()) + " nuovi JMS");

        }

        section++;

        if (Opzioni["suono-jms"] == "yes")
            QSound::play("sounds/newjms.wav");

    }catch(...){
        QMessageBox::critical(this, "JackSMS", "JackSMS ha rilevato un errore grave durante la procedura di aggiornamento dei messaggi JMS ricevuti.\nDettagli: unknown error, section: "+QString::number(section));
    }
}

void MainJackSMS::errorUpdates(QString err)
{
    nascondiProssimoDialogRicarica = true;
    on_actionRicarica_servizi_triggered();
}

void MainJackSMS::countdownToGui(){

    countdownToGuiCount--;

    if (countdownToGuiCount == 0) {

        libJackSMS::serverApi::synchronizeVariables(ElencoServiziConfigurati, ElencoServizi);

        WriteAddressBookToGui();
        WriteConfiguredServicesToGui();

        enableUiAfterLogin();
        ui->widgetSchermate->setCurrentIndex(2);
        invioMultiplo = false;
        usaAssociatiPresent = false;
        gestiscimenuSingolo();

        startIm();

        ui->smsListWidget->hideCaricaAltri(true);
        stopWriteMessagesToGui = false;
        WriteMessagesToGui();

        updateChecker = new libJackSMS::serverApi::updateServicesManager(current_login_id, GlobalOptions, ElencoServizi);
        connect(updateChecker, SIGNAL(updatesAvailable(libJackSMS::dataTypes::servicesType, QString, QString)), this, SLOT(updatesAvailable(libJackSMS::dataTypes::servicesType, QString, QString)));
        connect(updateChecker, SIGNAL(criticalError(QString)), this, SLOT(errorUpdates(QString)));
        updateChecker->checkUpdates();
        countdownToGuiCount = COUNTDOWNTOGUICOUNTDEFINE;
    }

}

void MainJackSMS::setTrayIcon(){

    if (countReceivedUnreaded>0){

        trayIco->setIcon(QIcon(":/resource/tray_icon_notify.png"));
    }else{
        trayIco->setIcon(QIcon(":/resource/tray_icon.png"));

    }
}

void MainJackSMS::messagesLoaded(QList<QMyMessage> msgs) {
    Messaggi = msgs;
    iterMess = Messaggi.end();
    countdownToGui();
}

void MainJackSMS::servicesLoaded(libJackSMS::dataTypes::servicesType s) {
    ElencoServizi = s;
    icon_jack = ElencoServizi["40"].getIcon().pixmap(16,16);

    messageLoader* loaderMessages = new messageLoader(current_user_directory, this);
    connect(loaderMessages, SIGNAL(messagesLoaded(QList<QMyMessage>)), this, SLOT(messagesLoaded(QList<QMyMessage>)));
    loaderMessages->loadMessages();

    countdownToGui();
}

void MainJackSMS::optionsLoaded(libJackSMS::dataTypes::optionsType op){
    Opzioni = op;

    if (ui->ricordaPassword->isChecked())
        Opzioni["password"] = ui->password->text();
    else
        Opzioni["password"] = "";

    libJackSMS::localApi::optionManager man(current_user_directory, Opzioni);
    man.save();

}

void MainJackSMS::loginSuccess(QString sessionId) {

    libJackSMS::localApi::userDirectoryManager man(current_user_username);

    if (!man.userDirectoryExists()) {
        man.createUser();
        current_user_directory = man.getUserDir();
    }

    libJackSMS::localApi::cookieManager m(current_user_directory);
    m.deleteCookies();

    current_login_id = sessionId;

    xmlLoader = new libJackSMS::localApi::xmlLoader(current_user_directory);
    connect(xmlLoader, SIGNAL(servicesLoaded(libJackSMS::dataTypes::servicesType)), this, SLOT(servicesLoaded(libJackSMS::dataTypes::servicesType)));
    connect(xmlLoader, SIGNAL(optionsLoaded(libJackSMS::dataTypes::optionsType)), this, SLOT(optionsLoaded(libJackSMS::dataTypes::optionsType)));

    xmlLoader->loadServices();
    xmlLoader->loadOptions();

    loggedIn = true;

    GlobalOptions["auto-login"] = (ui->autoLogin->isChecked() ? "yes" : "no");
    GlobalOptions["default-user"] = libJackSMS::utilities::Base64Encode(current_user_username);

    libJackSMS::localApi::optionManager opt("", GlobalOptions);
    opt.save();

    pingator = new libJackSMS::serverApi::pingator(current_login_id, GlobalOptions);
    connect(pingator, SIGNAL(pinged()), this, SLOT(serverPinged()));
    pingator->launchPing();

    countdownToGui();
}

void MainJackSMS::serverPinged() {
    //std::cout<<"[PING] pingato server"<<std::endl;
}

void MainJackSMS::updatesAvailable(libJackSMS::dataTypes::servicesType serv, QString xml, QString msg) {

    libJackSMS::localApi::serviceManager man;
    man.mergeServices(xml);
    ElencoServizi = serv;

    nascondiProssimoDialogRicarica = true;
    on_actionRicarica_servizi_triggered();

    if (Opzioni["hide-service-update"] != "yes")
        QMessageBox::information(this, "Aggiornamenti JackSMS", msg);
}

void MainJackSMS::newVersionAvailable() {
    // TODO: autoupdate
    QMessageBox::information(this, "JackSMS", "E' disponibile una nuova versione di JackSMS.\nScaricala da www.jacksms.it");
}

void MainJackSMS::accountsReceived(libJackSMS::dataTypes::configuredServicesType accounts) {
    ElencoServiziConfigurati = accounts;

    // Creo il servizio unico onnipresente "JackSMS Messenger"
    libJackSMS::dataTypes::configuredAccount *accountJMS = new libJackSMS::dataTypes::configuredAccount();
    accountJMS->setId("1");
    accountJMS->setName("JackSMS Messenger");
    accountJMS->setService("40");
    accountJMS->setData("data1", current_user_username);
    accountJMS->setData("data2", ui->password->text());

    // Carico le statistiche
    libJackSMS::localApi::statsManager l(current_user_directory);
    l.loadStats(ElencoServiziConfigurati);

    // Droppo i servizi di JMS che hanno come utente lo stesso dell'account che si sta usando
    for(libJackSMS::dataTypes::configuredServicesType::iterator i = ElencoServiziConfigurati.begin(); i != ElencoServiziConfigurati.end(); ++i) {
        if ((i.value().getService() == "40") && (i.value().getData("data1") == current_user_username)) {
            JMSServicesDropped.append(i.key());
            accountJMS->sumStats(i.value());
        }
    }

    for (int i = 0; i < JMSServicesDropped.size(); ++i) {
        ElencoServiziConfigurati.remove(JMSServicesDropped.at(i));
    }

    // Aggiungo il servizio unico onnipresente "JackSMS Messenger"
    ElencoServiziConfigurati.insert("1", *accountJMS);

    countdownToGui();
}

void MainJackSMS::phoneBookReceived(libJackSMS::dataTypes::phoneBookType r) {
    Rubrica = r;
    QStringList l;

    for (libJackSMS::dataTypes::phoneBookType::iterator i = Rubrica.begin(); i != Rubrica.end(); ++i) {
        l << i->getName();

        // metto account associato = "JackSMS Messenger" ai contatti il cui id è stato droppato (associazione al volo, non va sul server)
        if (JMSServicesDropped.contains(i->getAccount())) {
            i->setAccount("1");
        }
    }

    completer = new RecipientCompleter(l);
    connect(completer, SIGNAL(activated(QString)), this, SLOT(recipientPopupSelected(QString)));

    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setCompletionMode(QCompleter::PopupCompletion);
    ui->recipientLine->setCompleter(completer);
    countdownToGui();
}

void MainJackSMS::loginStarted() {
    ui->widgetSchermate->setCurrentIndex(1);
}

void MainJackSMS::recipientPopupSelected(QString selected) {
    elaboraRecipientLine();
}

void MainJackSMS::loginFailed(QString err) {
    ui->autoLogin->setChecked(false);
    GlobalOptions["auto-login"] = "no";
    ui->widgetSchermate->setCurrentIndex(0);
    QMessageBox::critical(this, "JackSMS", err);
    countdownToGuiCount = COUNTDOWNTOGUICOUNTDEFINE;
}

void MainJackSMS::on_loginButton_clicked()
{
    if (ui->password->text().isEmpty()) {
        QMessageBox::critical(this, "JackSMS", "La tua password non puo' essere vuota!");
        return;
    }

    if (ui->username->currentText().isEmpty()) {
        QMessageBox::critical(this, "JackSMS", "Il tuo username non puo' essere vuoto!");
        return;
    }

    current_user_directory = finder->getDataDirectory(ui->username->currentText());
    current_user_username = ui->username->currentText();

    loginClient = new libJackSMS::serverApi::login(current_user_username, ui->password->text(), GlobalOptions);
    connect(loginClient, SIGNAL(loginSuccess(QString)), this, SLOT(loginSuccess(QString)));
    connect(loginClient, SIGNAL(newVersionAvailable()), this, SLOT(newVersionAvailable()));
    connect(loginClient, SIGNAL(accountsReceived(libJackSMS::dataTypes::configuredServicesType)), this, SLOT(accountsReceived(libJackSMS::dataTypes::configuredServicesType)));
    connect(loginClient, SIGNAL(phoneBookReceived(libJackSMS::dataTypes::phoneBookType)), this, SLOT(phoneBookReceived(libJackSMS::dataTypes::phoneBookType)));
    connect(loginClient, SIGNAL(loginFailed(QString)), this, SLOT(loginFailed(QString)));
    connect(loginClient, SIGNAL(loginStarted()), this, SLOT(loginStarted()));
    loginClient->doLogin();
}

void MainJackSMS::disableUibeforeLogin() {
    //ui->actionOpzioni->setEnabled(false);
    ui->actionStatistiche->setEnabled(false);
    ui->actionCrea_backup_configurazione->setEnabled(false);
    ui->actionImporta_Backup->setEnabled(false);
    ui->actionLogout->setEnabled(false);
    ui->actionRicarica_servizi->setEnabled(false);
    ui->actionElimina_cookies->setEnabled(false);
    ui->actionPlugins->setEnabled(false);
    ui->actionCsv->setEnabled(false);
    ui->actionApri_Log->setEnabled(false);
    ui->menuInstruments->setEnabled(false);
    ui->actionApri_cartella_utente->setEnabled(false);
}

void MainJackSMS::enableUiAfterLogin() {
    ui->actionLogout->setEnabled(true);
    ui->menuInstruments->setEnabled(true);
    //ui->actionOpzioni->setEnabled(true);
    ui->actionStatistiche->setEnabled(true);
    ui->actionCrea_backup_configurazione->setEnabled(true);
    ui->actionImporta_Backup->setEnabled(true);
    ui->actionRicarica_servizi->setEnabled(true);
    ui->actionElimina_cookies->setEnabled(true);
    ui->actionPlugins->setEnabled(true);
    ui->actionCsv->setEnabled(true);
    ui->actionApri_cartella_utente->setEnabled(true);
    ui->actionApri_Log->setEnabled(true);
    ui->SalvaNumeroButton->setVisible(false);
    ui->recipientLine->setFocus();
}

void MainJackSMS::on_smsListWidget_currentItemChanged(QListWidgetItem* current, QListWidgetItem* previous)
{

    if (current != NULL) {

        SmsWidget *www = static_cast<SmsWidget*>(ui->smsListWidget->itemWidget(current));

        if (www->isCaricaAltri()) {

            ui->RispondiButton->setVisible(false);
            ui->CitaButton->setVisible(false);
            ui->InoltraButton->setVisible(false);
            ui->SalvaNumeroButton->setVisible(false);

        } else if (!www->getPhoneNum().getIsValid()) {

            ui->RispondiButton->setVisible(false);
            ui->CitaButton->setVisible(false);
            ui->InoltraButton->setVisible(false);
            ui->SalvaNumeroButton->setVisible(false);

        } else {

            if (www->isReceived()) {
                ui->InoltraButton->setVisible(false);
                ui->RispondiButton->setVisible(true);
                ui->CitaButton->setVisible(true);
            } else {
                ui->RispondiButton->setVisible(false);
                ui->CitaButton->setVisible(false);
                ui->InoltraButton->setVisible(true);
            }

            if (isInRubrica(www->getPhoneNum()))
                ui->SalvaNumeroButton->setVisible(false);
            else
                ui->SalvaNumeroButton->setVisible(true);
        }

    } else {
        ui->RispondiButton->setVisible(false);
        ui->CitaButton->setVisible(false);
        ui->InoltraButton->setVisible(false);
        ui->SalvaNumeroButton->setVisible(false);
    }
}

void MainJackSMS::on_RubricaVeloce_currentItemChanged(QListWidgetItem* item, QListWidgetItem*)
{
    if (!invioInCorso) {
        if (item != NULL) {
            //ui->recipientListWidget->setVisible(true);
            contactWidgetFastBook *it=static_cast<contactWidgetFastBook*>(ui->RubricaVeloce->itemWidget(item));
            ui->recipientListWidget->clear();
            QListWidgetItem *newItem = new QListWidgetItem;

            libJackSMS::dataTypes::configuredServicesType::const_iterator ser = ElencoServiziConfigurati.find(it->getContact().getAccount());
            QIcon icon;
            if (ser != ElencoServiziConfigurati.end())
                icon = ElencoServizi[ser->getService()].getIcon();
            else
                icon = QIcon(":/resource/ico_contact.png");
            QRecipientWidget *wi = new QRecipientWidget(it->getName(), it->getContact().getAccount(), it->getContact().getPhone(), icon.pixmap(16, 16));

            connect(wi,SIGNAL(removed(QListWidgetItem*)),this,SLOT(recipientRemove(QListWidgetItem*)));
            ui->recipientListWidget->addItem(newItem);
            newItem->setSizeHint(wi->size());
            ui->recipientListWidget->setItemWidget(newItem,wi);
            wi->setParentItem(newItem);

            //ui->recipientLine->setText(it->getContact().getName()+" <"+it->getContact().getPhone().toString()+">");
            //widthRecipientWidgets+=newItem->sizeHint().width();

            if (Opzioni["set-account"] != "no" && messageType == TYPE_SMS){

                libJackSMS::dataTypes::configuredServicesType::const_iterator i=ElencoServiziConfigurati.find(it->getContact().getAccount());

                if (i!=ElencoServiziConfigurati.end()){
                    int index=ui->comboServizio->findData(i.value().getName(),Qt::UserRole);
                    if (index>=0)
                        ui->comboServizio->setCurrentIndex(index);
                }
            }
            gestiscimenuSingolo();
            resizeRecipientBox();

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
    ReWriteConfiguredServicesToGui();

    //controllo se ci sono dei contatti con account associato = quello che sto cancellando
    try {

        int found = 0;
        for (libJackSMS::dataTypes::phoneBookType::iterator i = Rubrica.begin(); i != Rubrica.end(); ++i) {
            if (i->getAccount() == id)
                found++;
        }

        if (found) {
            cambiaaccount *dialog = new cambiaaccount(this, this, ElencoServizi, ElencoServiziConfigurati, Rubrica, GlobalOptions, id, found);
            dialog->exec();
            dialog->deleteLater();
        }

        for (libJackSMS::dataTypes::phoneBookType::iterator i = Rubrica.begin(); i != Rubrica.end(); ++i) {
            if (i->getAccount() == id) {
                i->setAccount("");
            }
        }

    }catch(...){
        QMessageBox::critical(this,"JackSMS","Errore nel controllo.");
    }

    ReWriteAddressBookToGui();
    ui->EliminaServizioButton->setEnabled(true);
    ui->labelSpinDelAccount->hide();
}

void MainJackSMS::closeEvent(QCloseEvent *event)
{
    if (loggedIn) {
        loginClient->deleteLater();
        pingator->deleteLater();

        if (imChecker != NULL) {
            imChecker->stop();
            imChecker->deleteLater();
        }

        if (updateChecker != NULL)
            if (updateChecker->isRunning())
                updateChecker->abort();

        if (invioInCorso) {
            smsSender->abort();
        }
    }

    if (isMaximized()) {
        GlobalOptions["window-maximized"] = "yes";
    } else {
        GlobalOptions["window-maximized"] = "no";
        GlobalOptions["window-height"] = QString::number(size().height());
        GlobalOptions["window-width"] = QString::number(size().width());
        GlobalOptions["window-xpos"] = QString::number(pos().x());
        GlobalOptions["window-ypos"] = QString::number(pos().y());
    }

    libJackSMS::localApi::optionManager man("",GlobalOptions);
    man.save();
 }

void MainJackSMS::on_actionLogout_triggered()
{
    if (invioInCorso) {
        smsSender->abort();
    }

    loggedIn = false;
    altriMessaggi = true;
    stopWriteMessagesToGui = true;
    loginClient->deleteLater();
    pingator->deleteLater();
    Rubrica.clear();

    if (imChecker != NULL) {
        imChecker->stop();
        imChecker->deleteLater();
    }

    ElencoServizi.clear();
    ElencoServiziConfigurati.clear();

    ui->password->setText(Opzioni["password"]);
    if (Opzioni["password"] == "")
        ui->ricordaPassword->setChecked(false);
    else
        ui->ricordaPassword->setCheckable(true);

    ui->autoLogin->setChecked(false);
    GlobalOptions["auto-login"] = "no";

    if (updateChecker->isRunning())
        updateChecker->abort();

    ui->tabWidget->setCurrentIndex(0);

    if (ui->rubricaBar->currentIndex() != 0)
        ui->rubricaBar->setCurrentIndex(0);

    Messaggi.clear();
    Opzioni.clear();

    ui->recipientListWidget->scrollToTop();
    ui->recipientListWidget->clearSelection();
    ui->recipientListWidget->clear();
    resizeRecipientBox();

    completer->deleteLater();
    ui->recipientLine->setText("");

    ui->comboServizio->clear();

    ui->TestoSMS->setText("");

    ui->RicercaVeloce->setText("");
    ui->RubricaVeloce->scrollToTop();
    ui->RubricaVeloce->clearSelection();
    ui->RubricaVeloce->clear();

    ui->RicercaVeloceIM->setText("");
    ui->smsListWidget->clear();
    ui->smsListWidget->hideCaricaAltri(true);
    ui->tabWidget->setTabText(1, "Conversazioni");
    ui->listSmsNames->clear();

    ui->TextRapidRubrica->setText("");
    ui->rubricaListWidget->scrollToTop();
    ui->rubricaListWidget->clearSelection();
    ui->rubricaListWidget->clear();

    ui->TextRapidServizi->setText("");
    ui->listServiziConfigurati->scrollToTop();
    ui->listServiziConfigurati->clearSelection();
    ui->listServiziConfigurati->clear();

    disableUibeforeLogin();
    ui->widgetSchermate->setCurrentIndex(0);
    ricaricaUtenti();
}

void MainJackSMS::on_TextRapidServizi_textChanged(QString text) {

    if (text == "") {
        ui->ServiziCercaButton->setEnabled(false);
        ui->ServiziCercaButton->setIcon(QIcon(":/resource/findd.png"));
        ui->ServiziCercaButton->setCursor(Qt::ArrowCursor);
    } else {
        ui->ServiziCercaButton->setEnabled(true);
        ui->ServiziCercaButton->setIcon(QIcon(":/resource/finde.png"));
        ui->ServiziCercaButton->setCursor(Qt::PointingHandCursor);
    }

    int c = ui->listServiziConfigurati->count();
    for(int x = 0; x < c; x++){
        QListWidgetItem* item = ui->listServiziConfigurati->item(x);
        accountWidget* w = static_cast<accountWidget*>(ui->listServiziConfigurati->itemWidget(item));
        if (w->getName().contains(text, Qt::CaseInsensitive))
            ui->listServiziConfigurati->setItemHidden(item, false);
        else
            ui->listServiziConfigurati->setItemHidden(item, true);
    }

}

void MainJackSMS::on_RubricaVeloce_itemClicked(QListWidgetItem* item)
{
    on_RubricaVeloce_currentItemChanged(item,NULL);
}

void MainJackSMS::on_password_returnPressed()
{
    ui->loginButton->click();
}

void MainJackSMS::on_actionTutti_i_messaggi_triggered()
{
    QDesktopServices::openUrl(QUrl("http://www.jacksms.it/area-personale.html", QUrl::TolerantMode));
}

void MainJackSMS::on_actionStatistiche_triggered()
{
    StatsDialog *sd = new StatsDialog(Opzioni, ElencoServiziConfigurati, ElencoServizi, this);
    sd->exec();
    sd->deleteLater();
}

void MainJackSMS::on_actionPlugins_triggered()
{
   pluginDialog *pd = new pluginDialog(pluginsList);
   pd->exec();
   pd->deleteLater();
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
    xmlReLoader = new libJackSMS::localApi::xmlLoader(current_user_directory);
    connect(xmlReLoader, SIGNAL(servicesLoaded(libJackSMS::dataTypes::servicesType)), this, SLOT(servicesReLoaded(libJackSMS::dataTypes::servicesType)));
    xmlReLoader->loadServices();
}

void MainJackSMS::servicesReLoaded(libJackSMS::dataTypes::servicesType s) {
    ElencoServizi = s;

    if(!nascondiProssimoDialogRicarica)
        QMessageBox::information(this, "JackSMS","Servizi ricaricati");

    nascondiProssimoDialogRicarica = false;
    xmlReLoader->deleteLater();
}

void MainJackSMS::on_actionElimina_cookies_triggered()
{
    libJackSMS::localApi::cookieManager m(current_user_directory);
    if (m.deleteCookies())
        QMessageBox::information(this,"JackSMS", "Cookies eliminati.");
    else
        QMessageBox::critical(this,"JackSMS", "Errore durante l'eliminazione dei cookie.");
}

void MainJackSMS::on_buttonLostPassword_clicked()
{
    QDesktopServices::openUrl(QUrl("http://www.jacksms.it/recupera-password.html", QUrl::TolerantMode));
}

void MainJackSMS::on_buttonNoAccount_clicked()
{
    QDesktopServices::openUrl(QUrl("http://www.jacksms.it/registrazione.html", QUrl::TolerantMode));
}

void MainJackSMS::on_ModificaServizioButton_clicked()
{
    QListWidgetItem *wid = ui->listServiziConfigurati->currentItem();
    if (wid != NULL){
            QWidget * wi = ui->listServiziConfigurati->itemWidget(wid);
            accountWidget* aw = static_cast<accountWidget*>(wi);
            QString id = aw->getAccountId();
            editAccountDialog *dialog = new editAccountDialog(ElencoServiziConfigurati, ElencoServizi, id, current_login_id, GlobalOptions, this);
            connect(dialog, SIGNAL(rewriteAccunts()), this, SLOT(ReWriteConfiguredServicesToGui()));
            dialog->exec();
            dialog->deleteLater();
    }
}

void MainJackSMS::on_rubricaListWidget_itemDoubleClicked(QListWidgetItem* item)
{
    QString id = static_cast<ContactWidget*>(ui->rubricaListWidget->itemWidget(item))->getContactId();
    editcontattodialog *dial = new editcontattodialog(this, ElencoServizi, ElencoServiziConfigurati, Rubrica, id, GlobalOptions, current_login_id);
    connect(dial, SIGNAL(contactEdited(QString)), this, SLOT(contactEdited(QString)));
    dial->exec();
    dial->deleteLater();
}

void MainJackSMS::contactEdited(QString id)
{
    ReWriteAddressBookToGui();
    updateSmsListAfterContactEdited(Rubrica[id]);
    ui->RicercaVeloce->setText("");
    ui->TextRapidRubrica->setText("");
    refreshCompleter();
}

void MainJackSMS::on_listServiziConfigurati_itemDoubleClicked(QListWidgetItem* item)
{
    QWidget *wi = ui->listServiziConfigurati->itemWidget(item);
    accountWidget *aw = static_cast<accountWidget*>(wi);
    QString id = aw->getAccountId();
    editAccountDialog *dialog = new editAccountDialog(ElencoServiziConfigurati, ElencoServizi, id, current_login_id, GlobalOptions, this);
    connect(dialog, SIGNAL(rewriteAccunts()), this, SLOT(ReWriteConfiguredServicesToGui()));
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

void MainJackSMS::on_ricordaPassword_stateChanged(int stato) {

    if (stato == Qt::Checked) {
        ui->autoLogin->setEnabled(true);
        ui->autoLogin->setFocus();
    } else {
        ui->autoLogin->setChecked(false);
        ui->autoLogin->setEnabled(false);
        ui->loginButton->setFocus();
    }
}


void MainJackSMS::username_returnPressed() {
    managePassword(ui->username->currentText());
    if (ui->password->text() != "")
        ui->loginButton->animateClick();
    else
        ui->password->setFocus();
}

void MainJackSMS::on_autoLogin_stateChanged(int) {
    ui->loginButton->setFocus();
}

void MainJackSMS::svuotaTabSms() {
    svuotaDestinatari();
    ui->TestoSMS->clear();
    ui->LabelCountChars->clear();
}

void MainJackSMS::svuotaDestinatari() {
    gestiscimenuSingolo();
    ui->label->setText("Destinatario");
    ui->recipientListWidget->clear();
    ui->recipientListWidget->hide();
    ui->recipientLine->clear();
    ui->comboServizio->setCurrentIndex(0);
}

void MainJackSMS::caricaAltriMessaggi()
{
    if (altriMessaggi) {
        setCursor(Qt::BusyCursor);
        stopWriteMessagesToGui = false;
        WriteMessagesToGui();
    }
}

void MainJackSMS::on_tabWidget_currentChanged(int index)
{
    if(index == 2)
        ui->RicercaVeloceIM->setFocus();
}

void MainJackSMS::anotherInstanceOpened(const QString &str){
    raise();
    activateWindow();
    showNormal();
    //QMessageBox::information(this,"JackSMS","JackSMS è già aperto.");
}

void MainJackSMS::clearRecipientsStatus(){
    for(int a=0; a<ui->recipientListWidget->count(); ++a){
        QListWidgetItem *current = ui->recipientListWidget->item(a);
        QRecipientWidget *widget = dynamic_cast<QRecipientWidget*>(ui->recipientListWidget->itemWidget(current));
        widget->clearStatus();
    }
}

void MainJackSMS::addRecipients(QList<QRecipientWidget*> l){

    bool inserted = false;

    while(!l.isEmpty()) {

        QRecipientWidget *wi = l.first();
        bool duplicatedNumber = false;

        for(int a = 0; a < ui->recipientListWidget->count(); ++a) {
            QListWidgetItem *current = ui->recipientListWidget->item(a);
            QRecipientWidget *widget = dynamic_cast<QRecipientWidget*>(ui->recipientListWidget->itemWidget(current));
            if (widget->getPhone() == wi->getPhone()){
                duplicatedNumber = true;
                break;
            }
        }

        if (!duplicatedNumber) {
            connect(wi, SIGNAL(removed(QListWidgetItem*)), this, SLOT(recipientRemove(QListWidgetItem*)));
            QListWidgetItem *newItem = new QListWidgetItem;
            ui->recipientListWidget->addItem(newItem);
            newItem->setSizeHint(wi->size());
            ui->recipientListWidget->setItemWidget(newItem, wi);
            wi->setParentItem(newItem);

            inserted = true;
        } else {
            wi->deleteLater();
        }

        l.removeFirst();
    }

    ui->recipientLine->clear();

    if (ui->recipientListWidget->count() > 1) {
        gestiscimenuMultiplo();
    } else if (ui->recipientListWidget->count() == 1) {
        gestiscimenuSingolo();
        //QRecipientWidget* widget = dynamic_cast<QRecipientWidget*>(ui->recipientListWidget->itemWidget(ui->recipientListWidget->item(0)));
        //ui->recipientLine->setText(widget->getName() + " <" + widget->getPhone().toString() + ">");
    }

    if (inserted) {
        resizeRecipientBox();
        on_comboServizio_currentIndexChanged(ui->comboServizio->currentIndex());

        // se c'è un solo nome nella lista e ho il set-account imposto l'account associato
        if (ui->recipientListWidget->count() == 1) {
            if (Opzioni["set-account"] != "no" && messageType == TYPE_SMS) {
                libJackSMS::dataTypes::configuredServicesType::const_iterator serv;
                serv = ElencoServiziConfigurati.find(static_cast<QRecipientWidget*>(ui->recipientListWidget->itemWidget(ui->recipientListWidget->item(0)))->getAccountId());
                if (serv != ElencoServiziConfigurati.end()) {
                    int index = ui->comboServizio->findData(serv.value().getName(), Qt::UserRole);
                    if (index >= 0)
                        ui->comboServizio->setCurrentIndex(index);
                }
            }
        }
    }

    ui->TestoSMS->setFocus();
}

void MainJackSMS::on_buttonAddContacts_clicked()
{
    elaboraRecipientLine();
    ui->recipientLine->clear();

    multipleCheckDialog *mcDialog = new multipleCheckDialog(Rubrica, ElencoServiziConfigurati, ElencoServizi, this, this);
    connect(mcDialog, SIGNAL(addRecipients(QList<QRecipientWidget*>)), this, SLOT(addRecipients(QList<QRecipientWidget*>)));
    mcDialog->exec();
    mcDialog->deleteLater();
}

void MainJackSMS::rubricaBarJMS()
{
    messageType = TYPE_JMS;
    ui->comboServizio->hide();
    ui->comboServizio->setCurrentIndex(0);
    ui->label_2->hide();

    showContactByTypeInFastAbook();

    libJackSMS::dataTypes::configuredServicesType::const_iterator x = ElencoServiziConfigurati.find("1");
    if (x != ElencoServiziConfigurati.end()) {
        QString serviceId = x.value().getService();
        libJackSMS::dataTypes::servicesType::const_iterator i = ElencoServizi.find(serviceId);
        currentMaxLength = i.value().getIntMaxLength();
        currentSingleLength = i.value().getIntSingleLength();
        currentSmsDivisor = i.value().getIntSmsDivisor();
    }
    on_TestoSMS_textChanged();
}

void MainJackSMS::rubricaBarTutti()
{
    messageType = TYPE_SMS;
    ui->comboServizio->show();
    ui->label_2->show();
    showContactByTypeInFastAbook();

    on_comboServizio_currentIndexChanged(ui->comboServizio->currentIndex());
}

void MainJackSMS::on_listSmsNames_currentItemChanged(QListWidgetItem* item, QListWidgetItem* previous)
{

    if (item == NULL) {
        ui->listSmsNames->setCurrentRow(0);
        item = ui->listSmsNames->item(0);
    }

    if (previous == NULL) {
        previous = item;
    }

    int to = ui->smsListWidget->count();
    if (altriMessaggi)
        --to;

    if(ui->listSmsNames->currentRow() > 0){

        // solo un contatto
        QString name = static_cast<NameWidget*>(ui->listSmsNames->itemWidget(item))->getName();

        for(int i = 0; i < to; i++){
            if( name == phone2name(static_cast<SmsWidget*>(ui->smsListWidget->itemWidget(ui->smsListWidget->item(i)))->getPhoneNum())){
                static_cast<SmsWidget*>(ui->smsListWidget->itemWidget(ui->smsListWidget->item(i)))->setNameFilteredHidden(false);
                if (static_cast<SmsWidget*>(ui->smsListWidget->itemWidget(ui->smsListWidget->item(i)))->searchMatch(ui->RicercaVeloceIM->text()))
                    ui->smsListWidget->setItemHidden(ui->smsListWidget->item(i), false);
                else
                    ui->smsListWidget->setItemHidden(ui->smsListWidget->item(i), true);
            }else{
                ui->smsListWidget->setItemHidden(ui->smsListWidget->item(i), true);
                static_cast<SmsWidget*>(ui->smsListWidget->itemWidget(ui->smsListWidget->item(i)))->setNameFilteredHidden(true);
            }
        }

    }else{

        // tutti i contatti
        for(int i = 0; i < to; i++){
            static_cast<SmsWidget*>(ui->smsListWidget->itemWidget(ui->smsListWidget->item(i)))->setNameFilteredHidden(false);
            if (static_cast<SmsWidget*>(ui->smsListWidget->itemWidget(ui->smsListWidget->item(i)))->searchMatch(ui->RicercaVeloceIM->text()))
                ui->smsListWidget->setItemHidden(ui->smsListWidget->item(i), false);
            else
                ui->smsListWidget->setItemHidden(ui->smsListWidget->item(i), true);
        }
    }

    ui->smsListWidget->scrollToTop();
}

void MainJackSMS::testoSmsRichiestoInvio() {
    ui->InviaSMS->setFocus();
    ui->InviaSMS->animateClick();
}

void MainJackSMS::catchSmsListCanc()
{
    return on_EliminaButton_clicked();
}

void MainJackSMS::RecipientTabPressed()
{
    if(completer->popup()->isVisible())
        elaboraRecipientLine();
    else
        ui->TestoSMS->setFocus();
}

void MainJackSMS::on_smsListWidget_itemPressed(QListWidgetItem* current)
{
    if (current != NULL) {

        int lastRow = ui->smsListWidget->row(current);

        if (altriMessaggi && (lastRow == ui->smsListWidget->count()-1)) {
            caricaAltriMessaggi();
            ui->smsListWidget->clearSelection();
            ui->smsListWidget->setCurrentRow(lastRow);
            ui->smsListWidget->clearSelection();
            return;
        }

        SmsWidget* w = static_cast<SmsWidget*>(ui->smsListWidget->itemWidget(current));
        if (!w->isReaded()){
            if (countReceivedUnreaded>0)
                countReceivedUnreaded--;

            if (countReceivedUnreaded>0)
                ui->tabWidget->setTabText(1,"Conversazioni ("+QString::number(countReceivedUnreaded)+")");
            else
                ui->tabWidget->setTabText(1,"Conversazioni");

            w->setReaded(true);

            //cerco il namewidget corrispondente e ne diminuisco gli unread
            NameWidget* namewid;
            for (int i = 1; i < ui->listSmsNames->count(); i++) {
                namewid = dynamic_cast<NameWidget*>(ui->listSmsNames->itemWidget(ui->listSmsNames->item(i)));
                if(namewid->getName() == w->getName()){
                    namewid->decreaseUnreadCount();
                    break;
                }
            }

            //diminuisco anche il contatore di "Tutti i Contatti"
            namewid = dynamic_cast<NameWidget*>(ui->listSmsNames->itemWidget(ui->listSmsNames->item(0)));
            namewid->decreaseUnreadCount();

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

void MainJackSMS::on_RicercaImButton_clicked()
{
    ui->RicercaVeloceIM->setText("");
}

void MainJackSMS::on_RicercaButton_clicked()
{
    ui->TextRapidRubrica->setText("");
}

void MainJackSMS::on_ServiziCercaButton_clicked()
{
    ui->TextRapidServizi->setText("");
}

void MainJackSMS::on_actionGmail_triggered()
{
    importGmailDialog *dialog = new importGmailDialog(current_login_id, GlobalOptions, Rubrica, this);
    connect(dialog, SIGNAL(rewritePhoneBook()), this, SLOT(ReWriteAddressBookToGui()));
    dialog->exec();
    dialog->deleteLater();
}

void MainJackSMS::on_recipientLine_textEdited(QString)
{
    ui->RubricaVeloce->clearSelection();
}

void MainJackSMS::on_RicercaVeloce_textChanged(QString txt)
{
    if (txt == "") {

        ui->RicercaVeloceButton->setEnabled(false);
        ui->RicercaVeloceButton->setIcon(QIcon(":/resource/findd.png"));
        ui->RicercaVeloceButton->setCursor(Qt::ArrowCursor);

    } else {

        ui->RicercaVeloceButton->setEnabled(true);
        ui->RicercaVeloceButton->setIcon(QIcon(":/resource/finde.png"));
        ui->RicercaVeloceButton->setCursor(Qt::PointingHandCursor);

    }

    int c = ui->RubricaVeloce->count();
    for (int x = 0; x < c; x++) {
        QListWidgetItem *item = ui->RubricaVeloce->item(x);
        contactWidgetFastBook *w = static_cast<contactWidgetFastBook*>(ui->RubricaVeloce->itemWidget(item));
        if (w->searchMatch(txt))
            ui->RubricaVeloce->setItemHidden(item, false);
        else
            ui->RubricaVeloce->setItemHidden(item, true);
    }
}

void MainJackSMS::on_RicercaVeloceButton_clicked()
{
    ui->RicercaVeloce->setText("");
}

void MainJackSMS::on_recipientListWidget_itemDoubleClicked(QListWidgetItem* item)
{
    recipientRemove(item);
}

void MainJackSMS::rubricaBarCurrentChanged(int index)
{
    if (index == 0)
        // Tutti
        rubricaBarTutti();

    else if (index == 1)
        // JMS
        rubricaBarJMS();

    // richiedo un nuovo filtraggio
    on_RicercaVeloce_textChanged(ui->RicercaVeloce->text());
}

void MainJackSMS::addServiceToServiceComboBox(accountWidget *acc, bool isJacksmsMessenger)
{
    if (!isJacksmsMessenger) {
        QListWidgetItem *item = new QListWidgetItem;
        item->setSizeHint(acc->size());
        ui->listServiziConfigurati->addItem(item);
        ui->listServiziConfigurati->setItemWidget(item, acc);
    }

    QString sa = QString::number(ElencoServiziConfigurati[acc->getAccountId()].getSentAll());
    if (sa == "0")
        sa = acc->getName();
    else
        sa = acc->getName() + " [" + sa + "]";

    if (isJacksmsMessenger)
        ui->comboServizio->insertItem(0, acc->getIcon(), sa, acc->getName());
    else
        ui->comboServizio->addItem(acc->getIcon(), sa, acc->getName());
}

void MainJackSMS::on_SalvaNumeroButton_clicked()
{
    SmsWidget* smswid = static_cast<SmsWidget*>(ui->smsListWidget->itemWidget(ui->smsListWidget->item(ui->smsListWidget->currentRow())));

    AggiungiContatto *diag = new AggiungiContatto(this, ElencoServiziConfigurati, Rubrica, ElencoServizi, GlobalOptions, current_login_id, smswid->getPhoneNum());
    connect(diag, SIGNAL(contactAdded(QString)), this, SLOT(contactAdded(QString)));
    diag->exec();
    diag->deleteLater();
}

void MainJackSMS::updateSmsListAfterContactAdded(libJackSMS::dataTypes::contact c)
{
    NameWidget* namewid = ui->listSmsNames->findNameWidget(c);

    if (namewid != NULL) {
        namewid->setName(c.getName());

        // se c'è il namewidget ci sono anche dei messaggi già caricati
        ui->smsListWidget->changeNameForGivenContact(c, c.getName());
    }

    checkSalvaButtonStatusToSet();
}

void MainJackSMS::updateSmsListAfterContactEdited(libJackSMS::dataTypes::contact c)
{
    updateSmsListAfterContactAdded(c);
}

void MainJackSMS::updateSmsListAfterContactRemoved(libJackSMS::dataTypes::contact c)
{
    try {
        NameWidget* namewid = ui->listSmsNames->findNameWidget(c);

        if (namewid != NULL) {
            QString newname = c.getPhone().toString();
            if (c.getPhone().getAltName() != "")
                newname = newname + " (" + c.getPhone().getAltName() + ")";

            namewid->setName(newname);

            // se c'è il namewidget ci sono anche dei messaggi già caricati
            ui->smsListWidget->changeNameForGivenContact(c, newname);
        }

        checkSalvaButtonStatusToSet();
    } catch (...) {/*non beccherà mai niente ma sempre meglio non rischiare*/}

}

void MainJackSMS::checkSalvaButtonStatusToSet()
{
    if (ui->smsListWidget->currentRow() != -1) {
        SmsWidget* smswid = static_cast<SmsWidget*>(ui->smsListWidget->itemWidget(ui->smsListWidget->item(ui->smsListWidget->currentRow())));
        if (smswid->isCaricaAltri()) {
            ui->SalvaNumeroButton->setVisible(false);
        } else {
            if (smswid->getPhoneNum().getIsValid()) {
                if (isInRubrica(smswid->getPhoneNum()))
                    ui->SalvaNumeroButton->setVisible(false);
                else
                    ui->SalvaNumeroButton->setVisible(true);
            } else {
                ui->SalvaNumeroButton->setVisible(false);
            }
        }
    }
}

void MainJackSMS::elaboraRecipientLine(bool StealFocus)
{
    QListWidgetItem* item = new QListWidgetItem;
    QString originalStr = ui->recipientLine->text();
    QString name = originalStr;
    libJackSMS::dataTypes::phoneNumber n;
    bool inserted = false;

    QRegExp r;
    r.setPattern("^([^<]+) <([\\+\\.0-9]+)>$");

    if (r.exactMatch(originalStr)) {
        name = r.cap(1);
        n.parse(r.cap(2));
    } else {
        n.parse(originalStr);
    }

    //if (!invioMultiplo && (ui->recipientListWidget->count() == 1))
    //    ui->recipientListWidget->clear();

    bool inRubrica = false;
    QRecipientWidget* wi = NULL;
    for (libJackSMS::dataTypes::phoneBookType::const_iterator i = Rubrica.begin(); i != Rubrica.end(); ++i) {

        if (name.toUpper() == i->getName().toUpper() || i->getPhone() == n) {

            //Controllo se è già presente nella lista
            for (int a = 0; a < ui->recipientListWidget->count(); ++a) {
                QListWidgetItem* current = ui->recipientListWidget->item(a);
                QRecipientWidget* widget = dynamic_cast<QRecipientWidget*>(ui->recipientListWidget->itemWidget(current));
                if (i->getPhone() == widget->getPhone()) {
                    if (ui->recipientListWidget->count() > 0) {
                        QTimer::singleShot(10, ui->recipientLine, SLOT(clear()));
                        if (StealFocus) {
                            QTimer::singleShot(15, ui->recipientLine, SLOT(setFocus()));
                            QTimer::singleShot(20, completer->popup(),SLOT(close()));
                        }
                    }
                    return;
                }
            }

            libJackSMS::dataTypes::configuredServicesType::const_iterator serv = ElencoServiziConfigurati.find(i->getAccount());
            QPixmap icon;

            if (serv != ElencoServiziConfigurati.end())
                icon = ElencoServizi[serv->getService()].getIcon().pixmap(16,16);
            else
                icon = QIcon(":/resource/ico_contact.png").pixmap(16, 16);

            wi = new QRecipientWidget(i->getName(), i->getAccount(), i->getPhone(), icon);

            connect(wi, SIGNAL(removed(QListWidgetItem*)), this, SLOT(recipientRemove(QListWidgetItem*)));

            item->setSizeHint(wi->size());
            ui->recipientListWidget->addItem(item);
            ui->recipientListWidget->setItemWidget(item,wi);
            wi->setParentItem(item);
            inRubrica = true;
            inserted = true;

            if (!invioInCorso) {
                if (ui->recipientListWidget->count() <= 1) {
                    if (Opzioni["set-account"] != "no" && messageType == TYPE_SMS) {
                        if (serv != ElencoServiziConfigurati.end()) {
                            int index = ui->comboServizio->findData(serv.value().getName(), Qt::UserRole);
                            if (index >= 0)
                                ui->comboServizio->setCurrentIndex(index);
                        }
                    }
                }
            }
            break;
        }
    }
    if (!inRubrica && n.getIsValid()) {

        for (int a = 0; a < ui->recipientListWidget->count(); ++a) {
            QListWidgetItem* current = ui->recipientListWidget->item(a);
            QRecipientWidget* widget = dynamic_cast<QRecipientWidget*>(ui->recipientListWidget->itemWidget(current));
            if (n == widget->getPhone()) {
                if (ui->recipientListWidget->count() > 0) {
                    QTimer::singleShot(10, ui->recipientLine, SLOT(clear()));
                    if (StealFocus) {
                        QTimer::singleShot(15, ui->recipientLine, SLOT(setFocus()));
                        QTimer::singleShot(20, completer->popup(),SLOT(close()));
                    }
                }
                return;
            }
        }

        wi = new QRecipientWidget(name, "40", n, ElencoServizi["40"].getIcon().pixmap(16,16));
        connect(wi, SIGNAL(removed(QListWidgetItem*)), this, SLOT(recipientRemove(QListWidgetItem*)));
        item->setSizeHint(wi->size());
        ui->recipientListWidget->addItem(item);
        ui->recipientListWidget->setItemWidget(item,wi);
        wi->setParentItem(item);
        inserted = true;
    }

    if (ui->recipientListWidget->count() > 1)
        gestiscimenuMultiplo();

    if (inserted) {
        on_comboServizio_currentIndexChanged(ui->comboServizio->currentIndex());
        resizeRecipientBox();
    }

    //Qt bug: se "pulisco" recipientLine, poi il qcompleter me lo riempie di nuovo al termine della funzione
    //soluzione..lo pulisco dopo il termine della funzione
    if (ui->recipientListWidget->count() > 0) {
        QTimer::singleShot(10, ui->recipientLine, SLOT(clear()));
        if (StealFocus) {
            QTimer::singleShot(15, ui->recipientLine, SLOT(setFocus()));
            QTimer::singleShot(20, completer->popup(),SLOT(close()));
        }
    }
}

void MainJackSMS::on_username_currentIndexChanged(QString txt)
{
    QString password = finder->getPassword(txt);
    ui->password->setText(password);

    if (password.isEmpty())
        ui->ricordaPassword->setChecked(false);
    else
        ui->ricordaPassword->setChecked(true);
}

void MainJackSMS::on_actionApri_cartella_utente_triggered()
{
    QString path = QDir::toNativeSeparators(libJackSMS::directories::concatDirectoryAndFile(libJackSMS::directories::XmlDirectory(), current_user_directory));
    QDesktopServices::openUrl(QUrl("file:///" + path));
}

void MainJackSMS::on_actionApri_cartella_impostazioni_triggered()
{
    QString path = QDir::toNativeSeparators(libJackSMS::directories::DataDirectory());
    QDesktopServices::openUrl(QUrl("file:///" + path));
}

void MainJackSMS::on_actionApri_Log_triggered()
{
    QString path = QDir::toNativeSeparators(libJackSMS::directories::concatDirectoryAndFile(libJackSMS::directories::DataDirectory(), "live_generated_log.txt"));

    if (QDesktopServices::openUrl(QUrl("file:///" + path)) == false)
        QMessageBox::information(this, "JackSMS", "Il file di log non esiste ancora.\nMandare un messaggio per accedere a questa funzione.");
}

void MainJackSMS::sleepBeforeFound(int _seconds) {
    if (_seconds > 0)
        clickText("Trovata attesa di <b>" + QString::number(_seconds) + "</b> secondi", "Invio in corso");
    else
        clickText("Attesa terminata", "Invio in corso");
}

void MainJackSMS::caricaUtenti() {

    try{
        finder = new libJackSMS::localApi::userFinder;
        finder->findUsers();

        while (finder->nextUser()) {
            ui->username->addItem(QIcon(), finder->currentUsername());
        }

        ui->username->setEditText("");
        ui->password->setText("");

    } catch(libJackSMS::exceptionXmlError) {
        QMessageBox::critical(this, "JackSMS", "Il file degli utenti sembra essere corrotto. Non posso caricare la lista degli utenti e le loro opzioni");
    } catch(libJackSMS::exceptionXmlNotFound) {
        /*non trovare il file significa che non ci sono utenti (tipico dell'installazione fresca) */
    }
}

void MainJackSMS::ricaricaUtenti() {

    ui->username->clearEditText();
    ui->username->clear();
    ui->password->clear();
    ui->ricordaPassword->setChecked(false);
    ui->autoLogin->setChecked(false);

    caricaUtenti();
    manageUserPassAutoLogin();
}

void MainJackSMS::manageUserPassAutoLogin() {

    /*l'utente di default potrebbe non essere impostato, caso tipico del primo avvio*/
    libJackSMS::dataTypes::optionsType::const_iterator i = GlobalOptions.find("default-user");
    if (i != GlobalOptions.end()) {

        if (!i.value().isEmpty()) {

            QString user = libJackSMS::utilities::Base64Decode(i.value());
            ui->username->setEditText(user);
            managePassword(user);

        } else {
            ui->username->setFocus();
        }
    } else {
        ui->username->setFocus();
    }
}

void MainJackSMS::managePassword(QString _user) {
    QString pass = finder->getPassword(_user);

    if (!pass.isEmpty()) {
        // c'è una password
        ui->password->setText(pass);
        ui->autoLogin->setChecked(false);
        ui->ricordaPassword->setChecked(true);
        ui->loginButton->setDefault(true);
        ui->loginButton->setFocus();

        // controllo auto-login
        if (GlobalOptions["auto-login"] == "yes") {
            ui->autoLogin->setChecked(true);
            ui->loginButton->animateClick();
        }

    } else {
        // non c'è settata una password
        ui->autoLogin->setChecked(false);
        ui->ricordaPassword->setChecked(false);
        ui->password->setFocus();
    }
}

void MainJackSMS::refreshCompleter()
{
    QStringList l;
    for (libJackSMS::dataTypes::phoneBookType::const_iterator i = Rubrica.begin(); i != Rubrica.end(); ++i)
        l << i->getName();

    completer->deleteLater();
    completer = new RecipientCompleter(l);
    connect(completer, SIGNAL(activated(QString)), this, SLOT(recipientPopupSelected(QString)));
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setCompletionMode(QCompleter::PopupCompletion);
    ui->recipientLine->setCompleter(completer);
}
