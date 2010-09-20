#include "smswidget.h"
#include <QPalette>
#include <QLabel>
#include <QDateTime>
#include <QDesktopServices>


SmsWidget::SmsWidget(QString _txt,QPixmap _ico,bool received,QDateTime time,QString user,QString service,QString _id,libJackSMS::dataTypes::phoneNumber _number,bool _letto):type(received),id(_id),number(_number),readed(_letto),dateTim(time)
{

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
       if (!_letto){
           labelIcoReceived->setPixmap(QPixmap(":/resource/go-down-notify.png"));
       }
       else{
           if (received)
               labelIcoReceived->setPixmap(QPixmap(":/resource/go-down.png"));
           else
               labelIcoReceived->setPixmap(QPixmap(":/resource/go-up.png"));
       }
       labelIco->setPixmap(_ico);
       labelIco->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
       labelIco->setMaximumSize(16,16);
       labelIco->setMinimumSize(16,16);


       labelTime = new QLabel(time.toString("dd/MM/yyyy hh:mm:ss"));

       labelTime->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Fixed);
       labelTime->setMaximumSize(10000,18);
       _txt=_txt.replace("<","&lt;");
       _txt=_txt.replace(">","&gt;");

       _txt=this->parseLinks(_txt);
       _txt=this->parseAts(_txt);

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
SmsWidget::SmsWidget(QMyMessage _sms,QPixmap _ico,bool received):msg(_sms),type(received),readed(_sms.getReaded()){
    id=_sms.getId();
    labelGroup = new QLabel(_sms.getParsedName());

    labelGroup->setFont(QFont(labelGroup->font().family(),-1,QFont::Bold,false));

    labelGroup->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    labelGroup->setMaximumSize(10000,18);


    labelIco=new QLabel();
    QString service=_sms.getAccountName();
    if (!service.isEmpty())
        service=service+" - ";
    labelService=new QLabel(service);
    labelService->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Fixed);
    labelService->setMaximumSize(10000,18);


    labelIcoReceived=new QLabel();
    labelIcoReceived->setMaximumSize(16,16);
    labelIcoReceived->setMinimumSize(16,16);
    if (!_sms.getReaded()){
        labelIcoReceived->setPixmap(QPixmap(":/resource/go-down-notify.png"));
    }
    else{
        if (received)
            labelIcoReceived->setPixmap(QPixmap(":/resource/go-down.png"));
        else
            labelIcoReceived->setPixmap(QPixmap(":/resource/go-up.png"));
    }
    labelIco->setPixmap(_ico);
    labelIco->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    labelIco->setMaximumSize(16,16);
    labelIco->setMinimumSize(16,16);


    labelTime = new QLabel(msg.getData().toString("dd/MM/yyyy hh:mm:ss"));

    labelTime->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Fixed);
    labelTime->setMaximumSize(10000,18);
    QString _txt=msg.getMessage();
    _txt=_txt.replace("<","&lt;");
    _txt=_txt.replace(">","&gt;");

    _txt=this->parseLinks(_txt);
    _txt=this->parseAts(_txt);

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
QString SmsWidget::parseLinks(QString _s){
    QRegExp r;
    r.setPattern("^(.*)(http|https|ftp|ftps)\\:\\/\\/([^ ]+)(.*)$");
    if (r.exactMatch(_s)){
        return parseLinks(r.cap(1))+"<a href=\""+r.cap(2)+"://"+r.cap(3)+"\">"+r.cap(2)+"://"+r.cap(3)+"</a> "+parseLinks(r.cap(4));
    }else{
        return _s;
    }
}
QString SmsWidget::parseAts(QString _s){
    QRegExp r;
    r.setPattern("^(.*)@([^ ]+)(.*)$");
    if (r.exactMatch(_s)){
        return parseAts(r.cap(1))+"<b><i>@"+r.cap(2)+"</i></b> "+parseAts(r.cap(3));
    }else{
        return _s;
    }
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
    readed=_r;
    if (!_r){
        labelIcoReceived->setPixmap(QPixmap(":/resource/go-down-notify.png"));
    }
    else{
        if (type)
            labelIcoReceived->setPixmap(QPixmap(":/resource/go-down.png"));
        else
            labelIcoReceived->setPixmap(QPixmap(":/resource/go-up.png"));
    }
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
    return labelTxt->text();
}
void SmsWidget::resizeEvent ( QResizeEvent * s ){



}
