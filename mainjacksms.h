#ifndef MAINJACKSMS_H
#define MAINJACKSMS_H
#include <QListWidgetItem>
#include <QtGui/QMainWindow>

#include <list>
#include "Types.h"
#include <QSystemTrayIcon>
#include <QTimer>
#include "pluginListItem.h"
//#include "plugins/JackPluginInterfacer.h"
//#include "plugins/captcha/JackCaptchaPluginInterfacer.h"
#include "libjacksms/libJackSMS.h"
#include "threadlogin.h"
#include "threadsendsms.h"
#include "sendsmsoperations.h"
#include "threadsavesmsonline.h"
#include <QSemaphore>
#include "threadinstantmessenger.h"
#include "threaddeletecontactonline.h"
#include <QMultiMap>
#include "smswidget.h"
#include "threaddeleteaccountonline.h"
#include "threadcheckupdates.h"
#include "threadpingserver.h"
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

    QSemaphore * semaforoCaptcha;
    QSemaphore * semaforoGui;
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
    void ReloadConfiguredServices();
    void ReloadRubrica();
    QList<JackPluginInterface*> pluginsList;
    QString current_user_directory;
    QString current_user_username;
    libJackSMS::serverApi::login *signin;
    void ReWriteAddressBookToGui();
    void ReWriteConfiguredServicesToGui();

private:
    bool imServiceActive;
    void setTrayIcon();
    int countReceivedUnreaded;
    //QMap<QString,QPixmap> Icone;
    QString phone2name(const libJackSMS::dataTypes::phoneNumber &_number);
    //QPixmap createIcon(const QString& _idOfService);
    bool invioInCorso;

    QTimer *pingServerTimer;
    int currentMaxLength;
    int currentSingleLength;
    threadCheckUpdates *checkUpdatesThread;
    QMultiMap<QDateTime,SmsWidget*> mapWidgets;
    QMultiMap<QDateTime,SmsWidget*> mapWidgetsReceived;
    threadDeleteContactOnline *deleterThread;
    threadDeleteAccountOnline *accountDeleterThread;
    threadPingServer *pingator;
    libJackSMS::localApi::userFinder *finder;
    sendSmsOperations * sendSmsOp;
    QMovie *spinner,*spinnerDC;
    QMenu * menu;
    QList<QString> idList;
    libJackSMS::dataTypes::optionsType Opzioni;
    libJackSMS::dataTypes::optionsType GlobalOptions;
    threadsendsms * threadSendSMS;
    ThreadLogin * loginThread;
    threadSaveSmsOnline *onlineSmsSaver;
    libJackSMS::dataTypes::logImType messaggiRicevuti;
    threadInstantMessenger *imChecker;
    QMyMessage ultimoSms;
    QSystemTrayIcon *trayIco;
    QStringList stringList;
    QString selectedService;
    void WriteAddressBookToGui();
    void WriteConfiguredServicesToGui();
    void WriteMessagesToGui();
    void WriteImToGui();
    void loadPlugins();
    void disableUibeforeLogin();
    void enableUiAfterLogin();
    bool onlineLogin;
    bool loggedIn;
    void ReWriteMessagesToGui();
    void ReWriteImToGui();
    QString esitoInvio;
private slots:
    void on_buttonStatusJms_clicked();
    void jmsActive();
    void jmsNotActive(bool err);
    void jmsActiving();
    void on_imRicevutiWidget_itemClicked(QListWidgetItem* item);
    void on_actionPlugins_triggered();
    void on_actionStatistiche_triggered();
    void on_actionTutti_i_messaggi_triggered();
    void on_ComboGruppo_currentIndexChanged(QString);
    void startIm();
    void stopIm();
    void on_password_returnPressed();
    void on_RicercaVeloceIM_2_textChanged(QString );
    void on_comboServizio_currentIndexChanged(QString );
    void updatesAvailable();
    void on_RispondiIm_clicked();
    void on_TextRapidServizi_textEdited(QString );
    void on_actionLogout_triggered();
    void on_RubricaVeloce_currentItemChanged(QListWidgetItem* current, QListWidgetItem* previous);
    void on_smsListWidget_currentItemChanged(QListWidgetItem* current, QListWidgetItem* previous);
    void on_accedi_locale_clicked();
    void on_username_currentIndexChanged(int index);
    void endLogin();
    void on_loginButton_clicked();
    void gestiscimenu(QAction*);
    void on_bottoneinviomultiplo_clicked();
    void on_comboServizio_currentIndexChanged(int index);
    void on_comboServizio_activated(QString );
    void on_TestoSMS_textChanged();
    void on_actionImporta_Backup_triggered();
    void on_actionCrea_backup_configurazione_triggered();
    void on_EliminaButton_clicked();
    void on_RicercaVeloceIM_textChanged(QString );
    void on_RubricaElimina_clicked();
    void on_actionInfo_su_JackSMS_triggered();
    void on_TextRapidRubrica_textChanged(QString );
    void on_NumeroDestinatario_textEdited(QString );
    void on_RispondiButton_clicked();
    void on_CitaButton_clicked();
    void CheckIm();
    void on_AnnullaSMS_clicked();
    void on_InoltraButton_clicked();
    void on_RicercaVeloce_textChanged(QString );
    void on_actionOpzioni_triggered();
    void on_EliminaServizioButton_clicked();
    void on_AggiungiServizioButton_clicked();
    void on_actionEsci_triggered();
    void on_RubricaVeloce_itemClicked(QListWidgetItem* item);
    void on_RubricaModifica_clicked();
    void on_InviaSMS_clicked();
    void on_RubricaAggiungi_clicked();
    void ReadStdoutReloadRubrica();
    void ClickBaloon();


    bool eventFilter( QObject *obj, QEvent *ev );
    void TrayClicked();

    void displayCaptcha(const QByteArray & data);
    void eseguiPassoInvio();
    void invioSuccesso(const QString & _text);
    void invioFallito(const QString & _text);
    void checkInstantMessengerReceived();
    void deleteContactKo();
    void deleteContactOk();
    void deleteAccountKo();
    void deleteAccountOk();
    void pingServer();
    void translateGui();
    void popupInvio();
};

#endif // MAINJACKSMS_H
