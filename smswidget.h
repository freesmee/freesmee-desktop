#ifndef SMSWIDGET_H
#define SMSWIDGET_H
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDateTime>
#include <QListWidgetItem>
#include "libjacksms/libJackSMS.h"
#include <QResizeEvent>
#include <QListWidgetItem>
#include <qmymessage.h>
class SmsWidget : public QWidget
{
Q_OBJECT

private:
    QLabel *labelGroup,*labelTxt,*labelTime,*labelIco,*labelIcoReceived,*labelService;
    QVBoxLayout *vLayout;
    QHBoxLayout *hLayout;
    QMyMessage msg;
    bool type;
    QString id;

    libJackSMS::dataTypes::phoneNumber number;
    bool readed;
    QString parseLinks(QString _s);
    QString parseAts(QString _s);

private slots:
     void openUrl(QString _url);
public:
    const QDateTime dateTim;
    SmsWidget(QString _txt,QPixmap _ico,bool received,QDateTime time,QString user,QString service,QString id,libJackSMS::dataTypes::phoneNumber _number,bool _letto=true);
    SmsWidget(QMyMessage _sms,QPixmap _ico,bool received);
    QString getId()const;
    bool isReceived()const;
    bool searchMatch(QString _txt);
    bool isReaded()const;
    void setReaded(bool _r);
    QString getText()const;
    libJackSMS::dataTypes::phoneNumber getPhoneNum() const;
    void resizeEvent ( QResizeEvent *  );


};

#endif // SMSWIDGET_H
