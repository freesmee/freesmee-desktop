/*
    Copyright (C) <2011>

    <enrico bacis> <enrico.bacis@gmail.com>
    <ivan vaccari> <grisson@jacksms.it>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    You can't modify the adv system, to cheat it.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef MAINJACKSMS_H
#define MAINJACKSMS_H

#include <QListWidgetItem>
#include <QtGui/QMainWindow>
#include <list>
#include "Types.h"
#include <QSystemTrayIcon>
#include <QTimer>
#include "pluginListItem.h"
#include "plugins/JackPluginInterfacer.h"
#include "plugins/JackPluginHostInterfacer.h"
//#include "plugins/captcha/JackCaptchaPluginInterfacer.h"
#include "libjacksms/libJackSMS.h"
#include <QMultiMap>
#include "smswidget.h"
#include "accountwidget.h"
#include "contactwidgetfastbook.h"
#include "qrecipientwidget.h"
#include "recipientcompleter.h"
#include "libjacksms/streamer.h"

#define TYPE_SMS 1
#define TYPE_JMS 2

#define IM_NOT_ACTIVE 0
#define IM_ACTIVATING 1
#define IM_ACTIVE     2

namespace Ui
{
    class MainJackSMS;
}

class MainJackSMS : public QMainWindow
{
    Q_OBJECT

signals:
    void abortSendSms();

public:
    Ui::MainJackSMS *ui;
    MainJackSMS(QWidget *parent = 0);
    ~MainJackSMS();
    libJackSMS::dataTypes::servicesType ElencoServizi;
    libJackSMS::dataTypes::configuredServicesType ElencoServiziConfigurati;
    libJackSMS::dataTypes::phoneBookType Rubrica;
    types::QMessageListType Messaggi;
    types::QMessageListType MessaggiRicevuti;
    void DisabilitaUi();
    void AbilitaUi();
    QList<JackPluginInterface*> pluginsList;
    QString current_user_directory;
    QString current_user_username;
    QString current_user_password;
    QString current_login_id;
    libJackSMS::serverApi::login *signin;

    QSize windowSize;
    //QTimer resizeTimer;
    QString result;
    QString phone2name(const libJackSMS::dataTypes::phoneNumber &_number);
    bool isInRubrica(const libJackSMS::dataTypes::phoneNumber &_number);
    bool isInRecipients(const libJackSMS::dataTypes::phoneNumber &_number);

    void updateSmsListAfterContactEdited(libJackSMS::dataTypes::contact c);
    void updateSmsListAfterContactAdded(libJackSMS::dataTypes::contact c);
    void updateSmsListAfterContactRemoved(libJackSMS::dataTypes::contact c);

public slots:
    void anotherInstanceOpened(const QString &str);

private:
    bool servicesAlreadyUpdated;
    bool firstStartFlag;
    bool useSSLtoServer;
    int iterateSendSms(bool first, bool result = false, QString _text = "");
    int smsCount;
    RecipientCompleter *completer;
    void resizeRecipientBox();
    void clearRecipientsStatus();
    QLineEdit *newRecipientLine;
    QListWidgetItem *newRecipientWidget;
    bool usaAssociatiPresent;
    bool popupJms;
    bool recipientAutoEdited;
    QList<QListWidgetItem*> multipleSendRecipients;
    bool invioMultiplo;
    QPixmap icon_freesmee;
    QString lastJmsError;
    int errorSentCounter;
    //void resizeEvent ( QResizeEvent * );
    int messageType;
    void countdownToGui();
    int countdownToGuiCount;
    int imServiceStatus;
    void setTrayIcon();
    int countReceivedUnreaded;
    bool invioInCorso;

    int currentMaxLength;
    int currentSingleLength;
    int currentSmsDivisor;
    libJackSMS::serverApi::updateServicesManager *updateChecker;
    QMap<QString,accountWidget*> mapWidgetsAccount;
    libJackSMS::serverApi::contactManager *deleteContect;
    libJackSMS::serverApi::accountManager *accountManager;
    libJackSMS::serverApi::pingator *pingator;
    libJackSMS::localApi::userFinder *finder;
    QMovie *spinner,*spinnerDC;
    QMenu * menu;
    QList<QString> idList;
    libJackSMS::dataTypes::optionsType Opzioni;
    libJackSMS::dataTypes::optionsType GlobalOptions;
    libJackSMS::smsSender *smsSender;
    libJackSMS::serverApi::login * loginClient;
    libJackSMS::serverApi::smsLogSaver *onlineSmsSaver;
    libJackSMS::serverApi::smsLogFailed *onlineSmsSaverFailCase;
    libJackSMS::dataTypes::logImType messaggiRicevuti;
    libJackSMS::dataTypes::logImType nuoviMessaggiRicevuti;
    libJackSMS::serverApi::Streamer *imChecker;
    libJackSMS::serverApi::cyclicMessengerChecker *backupImChecker;
    QMyMessage ultimoSms;
    QSystemTrayIcon *trayIco;
    QStringList stringList;
    QString selectedService;
    void WriteAddressBookToGui();
    void WriteConfiguredServicesToGui();
    void WriteMessagesToGui();
    void loadPlugins();
    void disableUibeforeLogin();
    void enableUiAfterLogin();
    bool loggedIn;
    bool stopWriteMessagesToGui;

    QString esitoInvio;

    libJackSMS::localApi::xmlLoader *xmlLoader;
    libJackSMS::localApi::xmlLoader *xmlReLoader;
    //libJackSMS::localApi::xmlLoader *initialXmlLoader;

    JackPluginHostInterface* jphi;
    void showContactByTypeInFastAbook();
    //bool firstResize;
    void clickText(QString text,QString defaultStr);

    void updateAccountCountComboBox(QString id);
    void closeEvent(QCloseEvent *event);

    void svuotaDestinatari();
    QTimer resetCounterTimer;

    bool altriMessaggi;
    types::QMessageListType::const_iterator iterMess;
    int tempCount;
    void caricaAltriMessaggi();
    bool nascondiProssimoDialogRicarica;

    void rubricaBarTutti();
    void rubricaBarJMS();
    void addServiceToServiceComboBox(accountWidget *acc, bool isFreesmeeService = false);
    void checkSalvaButtonStatusToSet();
    void elaboraRecipientLine(bool StealFocus = true);
    void pulisciRecipientLine(bool StealFocus = true);

    void caricaUtenti();
    void ricaricaUtenti();
    void manageUserPassAutoLogin();
    void managePassword(QString _user);

    void refreshCompleter();

    // Funzioni per la pubblicità
    void showAdvPanel(QString url);
    void showTestoSmsPanel();

public slots:
    void ReWriteAddressBookToGui();

private slots:
    void on_closeADVButton_clicked();
    void on_actionApri_Log_triggered();
    void on_actionApri_cartella_impostazioni_triggered();
    void on_actionApri_cartella_utente_triggered();
    void on_username_currentIndexChanged(QString );
    void on_SalvaNumeroButton_clicked();
    void on_recipientListWidget_itemDoubleClicked(QListWidgetItem* item);
    void on_RicercaVeloceButton_clicked();
    void on_RicercaVeloce_textChanged(QString txt);
    void on_recipientLine_textEdited(QString);
    void recipientPopupSelected(QString);
    void on_actionGmail_triggered();
    void on_ServiziCercaButton_clicked();
    void on_RicercaButton_clicked();
    void on_RicercaImButton_clicked();
    void on_listSmsNames_currentItemChanged(QListWidgetItem* current, QListWidgetItem* previous);
    void addRecipients(QList<QRecipientWidget*> l);
    void on_buttonAddContacts_clicked();
    void recipientRemove(QListWidgetItem*);
    void on_recipientLine_returnPressed();
    void resetCounters();
    void svuotaTabSms();
    void on_tabWidget_currentChanged(int index);
    void smsSaved(libJackSMS::dataTypes::logSmsMessage sms,QString t);
    void on_autoLogin_stateChanged(int );
    void on_ricordaPassword_stateChanged(int );
    void ReWriteConfiguredServicesToGui();
    void on_actionCsv_triggered();
    void on_listServiziConfigurati_itemDoubleClicked(QListWidgetItem* item);
    void on_rubricaListWidget_itemDoubleClicked(QListWidgetItem* item);
    void on_TextRapidServizi_textChanged(QString text);
    void on_ModificaServizioButton_clicked();
    //void resized();
    void on_buttonNoAccount_clicked();
    void on_buttonLostPassword_clicked();
    void abortLogin();

    void on_actionElimina_cookies_triggered();
    void servicesReLoaded(libJackSMS::dataTypes::servicesType);
    void on_actionRicarica_servizi_triggered();
    void on_buttonStatusJms_clicked();
    void jmsActive();
    void jmsNotActive(bool, QString);
    void jmsBackupNotActive();
    void jmsBackupActive();
    void jmsActivating();
    void on_actionPlugins_triggered();
    void on_actionStatistiche_triggered();
    void on_actionTutti_i_messaggi_triggered();

    void startIm();
    void stopIm();
    void on_password_returnPressed();
    void on_actionLogout_triggered();
    void on_RubricaVeloce_currentItemChanged(QListWidgetItem* current, QListWidgetItem* previous);
    void on_smsListWidget_currentItemChanged(QListWidgetItem* current, QListWidgetItem* previous);
    void on_loginButton_clicked();
    void gestiscimenuSingolo();
    void gestiscimenuMultiplo();
    void on_comboServizio_currentIndexChanged(int index);
    void on_TestoSMS_textChanged();
//    void on_actionImporta_Backup_triggered();
//    void on_actionCrea_backup_configurazione_triggered();
    void on_EliminaButton_clicked();
    void on_RicercaVeloceIM_textChanged(QString );
    void on_RubricaElimina_clicked();
    void on_actionInfo_su_JackSMS_triggered();
    void on_TextRapidRubrica_textChanged(QString );
    void on_RispondiButton_clicked();
    void on_CitaButton_clicked();
    void on_AnnullaSMS_clicked();
    void on_InoltraButton_clicked();
    void on_actionOpzioni_triggered();
    void on_EliminaServizioButton_clicked();
    void on_AggiungiServizioButton_clicked();
    void on_actionEsci_triggered();
    void on_RubricaModifica_clicked();
    void on_InviaSMS_clicked();
    void on_RubricaAggiungi_clicked();
    void ClickBaloon();
    void TrayClicked();

    void displayCaptcha(QByteArray data/*,QSemaphore* sem*/);
    void invioSuccesso(QString _text, int n);
    void invioFallito(QString _text);
    void sleepBeforeFound(int _seconds);
    void checkInstantMessengerReceived(libJackSMS::dataTypes::logImType);
    void deleteContactKo();
    void deleteContactOk(QString);
    void deleteAccountKo();
    void deleteAccountOk(QString);

    void contactAdded(QString);
    void contactEdited(QString);

    void translateGui();
    void popupInvio();
    void newVersionAvailable();
    void loginSuccess(QString);
    void accountsReceived(libJackSMS::dataTypes::configuredServicesType);
    void phoneBookReceived(libJackSMS::dataTypes::phoneBookType);
    void loginFailed(QString);
    void loginStarted();
    void updatesAvailable(libJackSMS::dataTypes::servicesType, QString, QString);
    void servicesLoaded(libJackSMS::dataTypes::servicesType);
    void optionsLoaded(libJackSMS::dataTypes::optionsType);
    //void initialOptionsLoaded(libJackSMS::dataTypes::optionsType);
    void serverPinged();
    void messagesLoaded(QList<QMyMessage>);
    void errorUpdates(QString err);

    void username_returnPressed();
    void testoSmsRichiestoInvio();
    void catchSmsListCanc();
    void RecipientTabPressed();
    void stepWriteMessageToGui();
    void rubricaBarCurrentChanged(int index);

    void advLoaded(bool);
    void slotAdv(QString);
    void on_actionVai_al_Sito_triggered();

    void firstStart();
    void checkServicesUpdate();
    void continueLoadServices();
    void on_RubricaVeloce_itemClicked(QListWidgetItem *item);
    void RubricaVeloceSelected(QListWidgetItem *item);
};

#endif // MAINJACKSMS_H
