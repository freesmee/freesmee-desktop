#ifndef SMSWIDGET_H
#define SMSWIDGET_H
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDateTime>
#include <QListWidgetItem>
#include "libjacksms/libJackSMS.h"
#include <QResizeEvent>
#include <qmymessage.h>

class SmsWidget : public QFrame
{
Q_OBJECT

private:
    QString originalText;
    QLabel *labelGroup,*labelTxt,*labelTime,*labelIco,*labelIcoReceived,*labelService;
    QVBoxLayout *vLayout;
    QHBoxLayout *hLayout;
    QMyMessage msg;
    bool type;
    QString id;
    QString name;
    bool nameFiltered;

    libJackSMS::dataTypes::phoneNumber number;
    bool readed;
    QString parseLinks(QString _s);
    QString parseAts(QString _s);

    void createBubble(bool isread, bool isreceived);
    bool caricaAltri;

private slots:
     void openUrl(QString _url);
public:
    const QDateTime dateTim;
    SmsWidget(QString _txt,QPixmap _ico,bool received,QDateTime time,QString user,QString service,QString id,libJackSMS::dataTypes::phoneNumber _number,bool _letto=true);
    SmsWidget(QMyMessage _sms,QPixmap _ico,bool received);

    //solo per il "Carica Altri Messaggi"
    SmsWidget(QString _txt);
    bool isCaricaAltri()const;

    QString getId()const;
    bool isReceived()const;
    bool searchMatch(QString _txt);
    bool isReaded()const;
    void setReaded(bool _r);
    QString getText()const;
    QString getName()const;
    libJackSMS::dataTypes::phoneNumber getPhoneNum()const;
    void resizeEvent ( QResizeEvent *  );
    QSize getSize();
    void setNameFilteredHidden(bool _nf);
    bool isNameFilteredHidden();
    QDateTime getDateTime()const;


};

#endif // SMSWIDGET_H
