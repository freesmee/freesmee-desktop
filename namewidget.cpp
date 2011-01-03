#include "namewidget.h"

NameWidget::NameWidget(QString _name, QString _lastMessage, QDateTime _lastTime, QString _lastId, int _unreadCount)
{
    setStyleSheet("NameWidget{"
                  "background-image: url(:/resource/bg_top.png);"
                  "background-repeat:repeat-x;"
                  "}");
    unreadCount = _unreadCount;
    name = _name;
    if(_name.length() > 16)
        _name = _name.left(13) + "...";

    labelName = new QLabel(_name);
    labelName->setFont(QFont(labelName->font().family(),-1,QFont::Bold,false));
    labelName->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    labelName->setMaximumSize(150,15);

    lastMessage = _lastMessage;
    if(_lastMessage.length() > 20)
        _lastMessage = _lastMessage.left(17) + "...";

    labelLastMessage = new QLabel(_lastMessage);
    labelLastMessage->setFont(QFont(labelLastMessage->font().family(),-1,QFont::Normal,false));
    labelLastMessage->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    labelLastMessage->setMaximumSize(150,15);

    lastTime = _lastTime;
    lastId = _lastId;

    labelIconStatus = new QLabel();

    if(unreadCount == 0)
        labelIconStatus->setPixmap(QPixmap(":/resource/arrow.png"));
    else
        labelIconStatus->setPixmap(QPixmap(":/resource/arrow-unread.png"));

    labelIconStatus->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    labelIconStatus->setMaximumSize(16,16);
    labelIconStatus->setMinimumSize(16,16);

    hLayout = new QHBoxLayout;
    hLayout->setMargin(5);
    vLayout = new QVBoxLayout;
    vLayout->setMargin(5);

    labelName->adjustSize();
    labelLastMessage->adjustSize();
    labelIconStatus->adjustSize();

    vLayout->addWidget(labelName);
    vLayout->addWidget(labelLastMessage);
    hLayout->addLayout(vLayout);
    hLayout->addWidget(labelIconStatus);

    setLayout(hLayout);
    adjustSize();
}

QString NameWidget::getName() const{
    return name;
}

QString NameWidget::getId() const{
    return lastId;
}

QDateTime NameWidget::getTime() const{
    return lastTime;
}

int NameWidget::getUnreadCount() const{
    return unreadCount;
}

void NameWidget::increaseUnreadCount(){
    if(unreadCount == 0)
        setIconUnread();

    unreadCount++;
}

void NameWidget::decreaseUnreadCount(){
    if(unreadCount <= 0)
        return;

    unreadCount--;
    if(unreadCount == 0)
        setIconRead();
}

void NameWidget::setUnreadCount(int _unreadcount){
    if(_unreadcount < 0)
        unreadCount = 0;
    else
        unreadCount = _unreadcount;

    if(unreadCount == 0)
        setIconRead();
    else
        setIconUnread();
}

void NameWidget::setIconUnread(){
    labelIconStatus->setPixmap(QPixmap(":/resource/arrow-unread.png"));
}

void NameWidget::setIconRead(){
    labelIconStatus->setPixmap(QPixmap(":/resource/arrow.png"));
}
