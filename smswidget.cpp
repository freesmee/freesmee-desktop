#include "smswidget.h"
#include <QPalette>
#include <QLabel>
#include <QDateTime>
#include <QDesktopServices>

SmsWidget::SmsWidget(QString _txt,QPixmap _ico,bool received,QDateTime time,QString user,QString service,QString _id,libJackSMS::dataTypes::phoneNumber _number,bool _letto) :
        originalText(_txt),
        type(received),
        id(_id),
        number(_number),
        readed(_letto),
        dateTim(time),
        caricaAltri(false)
{
    name = user;
    labelGroup = new QLabel(user);
    labelGroup->setFont(QFont(labelGroup->font().family(),-1,QFont::Bold,false));
    labelGroup->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    labelGroup->setMaximumSize(10000,18);

    labelIco=new QLabel();
    if (!service.isEmpty())
        service=service+" - ";
    labelService=new QLabel(service);
    labelService->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Fixed);
    labelService->setMaximumSize(10000,18);

    labelIcoReceived=new QLabel();
    labelIcoReceived->setMaximumSize(16,16);
    labelIcoReceived->setMinimumSize(16,16);
    createBubble(_letto, received);

    labelIco->setPixmap(_ico);
    labelIco->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    labelIco->setMaximumSize(16,16);
    labelIco->setMinimumSize(16,16);

    labelTime = new QLabel(time.toString("dd/MM/yyyy hh:mm:ss"));

    labelTime->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Fixed);
    labelTime->setMaximumSize(10000, 18);

    _txt = _txt.replace("<", "&lt;");
    _txt = _txt.replace(">", "&gt;");
    _txt = parseQuotes(_txt);
    _txt = parseLinks(_txt);
    _txt = parseAts(_txt);

    labelTxt = new QLabel(_txt);

    labelTxt->setTextInteractionFlags(Qt::LinksAccessibleByMouse);
    labelTxt->setWordWrap (true);
    labelTxt->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Minimum);
    connect(labelTxt,SIGNAL(linkActivated(QString)),this,SLOT(openUrl(QString)));

    hLayout = new QHBoxLayout;
    vLayout = new QVBoxLayout;

    labelGroup->adjustSize();
    labelIco->adjustSize();
    labelIcoReceived->adjustSize();
    labelTxt->adjustSize();
    labelTime->adjustSize();

    hLayout->addWidget(labelIcoReceived);
    hLayout->addWidget(labelGroup);
    hLayout->addWidget(labelIco);
    hLayout->addWidget(labelService);
    hLayout->addWidget(labelTime);
    vLayout->addLayout(hLayout);
    vLayout->addWidget(labelTxt);

    setLayout(vLayout);
    setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Minimum);
    adjustSize();
}

SmsWidget::SmsWidget(QMyMessage _sms, QPixmap _ico, bool received) :
        originalText(_sms.getMessage()),
        msg(_sms),
        type(received),
        number(_sms.getPhone()),
        readed(_sms.getReaded()),
        caricaAltri(false)
{

    id = _sms.getId();
    name = _sms.getParsedName();

    labelGroup = new QLabel(_sms.getParsedName());
    labelGroup->setFont(QFont(labelGroup->font().family(), -1, QFont::Bold, false));
    labelGroup->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    labelGroup->setMaximumSize(10000, 18);

    labelIco = new QLabel();
    QString service = _sms.getAccountName();
    if (!service.isEmpty())
        service = service + " - ";

    labelService = new QLabel(service);
    labelService->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    labelService->setMaximumSize(10000, 18);

    labelIcoReceived = new QLabel();
    labelIcoReceived->setMaximumSize(16, 16);
    labelIcoReceived->setMinimumSize(16, 16);
    createBubble(_sms.getReaded(), received);

    labelIco->setPixmap(_ico);
    labelIco->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    labelIco->setMaximumSize(16, 16);
    labelIco->setMinimumSize(16, 16);

    labelTime = new QLabel(msg.getData().toString("dd/MM/yyyy hh:mm:ss"));
    labelTime->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    labelTime->setMaximumSize(10000, 18);
    QString _txt = msg.getMessage();
    int l = _txt.length();

    _txt = _txt.replace("<","&lt;");
    _txt = _txt.replace(">","&gt;");
    _txt = parseQuotes(_txt);
    _txt = parseLinks(_txt);
    _txt = parseAts(_txt);

    labelTxt = new QLabel(_txt);
    l = _txt.length();
    labelTxt->setTextInteractionFlags(Qt::LinksAccessibleByMouse);
    labelTxt->setWordWrap(true);
    labelTxt->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    connect(labelTxt, SIGNAL(linkActivated(QString)), this, SLOT(openUrl(QString)));

    hLayout = new QHBoxLayout;
    vLayout = new QVBoxLayout;

    labelGroup->adjustSize();
    labelIco->adjustSize();
    labelIcoReceived->adjustSize();
    labelTxt->adjustSize();
    labelTime->adjustSize();

    hLayout->addWidget(labelIcoReceived);
    hLayout->addWidget(labelGroup);
    hLayout->addWidget(labelIco);
    hLayout->addWidget(labelService);
    hLayout->addWidget(labelTime);
    vLayout->addLayout(hLayout);
    vLayout->addWidget(labelTxt);

    setLayout(vLayout);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    adjustSize();
}

SmsWidget::SmsWidget(QString _txt) :
        originalText(_txt),
        caricaAltri(true)
{
    nameFiltered = false;

    labelTxt = new QLabel(originalText);
    labelTxt->setFont(QFont(labelTxt->font().family(),-1,QFont::Bold,false));
    labelTxt->setAlignment(Qt::AlignCenter);
    labelTxt->setWordWrap(true);
    labelTxt->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Minimum);
    labelTxt->adjustSize();
    setStyleSheet("SmsWidget{"
                  "background-color: #C1FFC1;"
                  "border-style: outset;"
                  "border-width: 2px;"
                  "border-radius: 10px;"
                  "border-color: #4CBB17;"
                  "font: bold 14px;"
                  "min-width: 10em;"
                  "padding: 6px;"
                  "margin-left: 0px;"
                  "margin-top: 10px;"
                  "margin-right: 0px;"
                  "margin-bottom: 0px;"
                  "}");
    setContentsMargins(0, 10, 0, 0);
    setCursor(Qt::PointingHandCursor);
    vLayout = new QVBoxLayout;
    vLayout->addWidget(labelTxt);
    setLayout(vLayout);
    setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Minimum);
    adjustSize();
}

QSize SmsWidget::getSize(){

    //TODO: sistemare!
    /*
    //size-by size a tentativi
    if (size().height()<60)
        return size();
    else if (size().height()<90)
        return QSize(size().width(),size().height()*0.9);
    else if (size().height()<120)
        return QSize(size().width(),size().height()*0.8);
    else if (size().height()<150)
        return QSize(size().width(),size().height()*0.75);
    else if (size().height()<200)
        return QSize(size().width(),size().height()*0.70);
    else
        return QSize(size().width(),size().height()*0.65);
    */
    return size();
}

QString SmsWidget::parseLinks(QString _s) {
    QRegExp r;
    r.setPattern("^(.*)(http|https|ftp|ftps)\\:\\/\\/([^ ]+)(.*)$");
    if (r.exactMatch(_s))
        return parseLinks(r.cap(1)) + "<a href=\"" + r.cap(2) + "://" + r.cap(3) + "\">" + r.cap(2) + "://" + r.cap(3) + "</a> " + parseLinks(r.cap(4));
    else
        return _s;
}

QString SmsWidget::parseAts(QString _s) {
    QRegExp r;
    r.setPattern("^(.*)@([^ .:]+)([: ])(.*)$");
    if (r.exactMatch(_s))
        return parseAts(r.cap(1)) + " <b>@" + r.cap(2) + "</b>" + r.cap(3) + " " + parseAts(r.cap(4));
    else
        return _s;
}

QString SmsWidget::parseQuotes(QString _s) {
    QRegExp r;
    r.setPattern("^(.*)&lt;&lt;(.*)&gt;&gt;(.*)$");
    if (r.exactMatch(_s))
        return parseQuotes(r.cap(1)) + "<font color=\"#0E459C\"><i>&lt;&lt;" + r.cap(2) + "&gt;&gt;</i></font>" + parseQuotes(r.cap(3));
    else
        return _s;
}

QString SmsWidget::getId() const{
    return id;
}
bool SmsWidget::isReceived() const{
    return type;
}
bool SmsWidget::isReaded()const{
    return readed;
}
void SmsWidget::openUrl(QString _url){
    QDesktopServices::openUrl(QUrl(_url, QUrl::TolerantMode));
}

libJackSMS::dataTypes::phoneNumber SmsWidget::getPhoneNum() const{
    return number;
}

void SmsWidget::setReaded(bool _r){
    readed = _r;
    createBubble(_r, type);
}

bool SmsWidget::searchMatch(QString _txt)
{
    /*if (_txt.isEmpty())
        return true;
    if (labelTxt->text().contains(_txt))
        return true;
    if (labelGroup->text().contains(_txt))
        return true;

    return false;*/
    return msg.messageContains(_txt);

}

QString SmsWidget::getText()const{
    return originalText;
}

void SmsWidget::resizeEvent ( QResizeEvent * s ){
}

void::SmsWidget::createBubble(bool isread, bool isreceived){

    if (!isread){
        setStyleSheet("SmsWidget{"
                      "background-color: #FFCC00;"
                      "border-style: outset;"
                      "border-width: 2px;"
                      "border-radius: 10px;"
                      "border-color: #FF9900;"
                      "font: bold 14px;"
                      "min-width: 10em;"
                      "padding: 6px;"
                      "margin-left: 0px;"
                      "margin-top: 1px;"
                      "margin-right: 60px;"
                      "margin-bottom: 1px;"
                      "}");
        setContentsMargins(0, 1, 60, 1);
        labelIcoReceived->setPixmap(QPixmap(":/resource/go-down-notify.png"));
        setCursor(Qt::PointingHandCursor);

    } else {

        setCursor(Qt::ArrowCursor);
        if (isreceived){
            setStyleSheet("SmsWidget{"
                          "background-color: #FEF2BF;"
                          "border-style: outset;"
                          "border-width: 2px;"
                          "border-radius: 10px;"
                          "border-color: #FFCC00;"
                          "font: bold 14px;"
                          "min-width: 10em;"
                          "padding: 6px;"
                          "margin-left: 0px;"
                          "margin-top: 1px;"
                          "margin-right: 60px;"
                          "margin-bottom: 1px;"
                          "}");
            setContentsMargins(0, 1, 60, 1);
            labelIcoReceived->setPixmap(QPixmap(":/resource/go-down.png"));

        } else {

            setStyleSheet("SmsWidget{"
                          "background-color: #DFF1FE;"
                          "border-style: outset;"
                          "border-width: 2px;"
                          "border-radius: 10px;"
                          "border-color: #00477D;"
                          "font: bold 14px;"
                          "min-width: 10em;"
                          "padding: 6px;"
                          "margin-left: 60px;"
                          "margin-top: 1px;"
                          "margin-right: 0px;"
                          "margin-bottom: 1px;"
                          "}");
            setContentsMargins(60, 1, 0, 1);
            labelIcoReceived->setPixmap(QPixmap(":/resource/go-up.png"));
        }
    }
}

void SmsWidget::setNameFilteredHidden(bool _nf){
    nameFiltered = _nf;
}

bool SmsWidget::isNameFilteredHidden(){
    return nameFiltered;
}

QDateTime SmsWidget::getDateTime() const{
    return dateTim;
}

QString SmsWidget::getName() const{
    return name;
}

bool SmsWidget::isCaricaAltri() const{
    return caricaAltri;
}
