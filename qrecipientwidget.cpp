#include "qrecipientwidget.h"
#include <QStyle>
#include "faderwidget.h"
#include "qlabelresult.h"

QRecipientWidget::QRecipientWidget(const QString & _contactName, const QString & accId, const libJackSMS::dataTypes::phoneNumber &_number, QPixmap Icon, QWidget *parent) :
    QWidget(parent),
     contactName(_contactName),
     accountId(accId),
     number(_number),
     nameStr(contactName)
 {
    original = Icon;

    hLayout = new QHBoxLayout;
    hLayout->setMargin(3);
    iconLabel = new QLabel;
    iconLabel->setPixmap(Icon);
    iconLabel->setMaximumSize(16, 16);
    iconStatus = new QLabel;
    iconStatus->setMaximumSize(16, 16);
    //name = new QLabel(contactName + " <" + number.toString() + ">");
    name = new QLabel(contactName + "  ");
    name->setCursor(Qt::WhatsThisCursor);
    name->setToolTip(number.toString());
    name->setMinimumHeight(16);
    name->adjustSize();
    name->setFixedWidth(name->width() + 10);
    removeLabel = new QLabelResult(this);
    removeLabel->setMaximumSize(10, 10);
    removeLabel->setPixmap(QIcon(":/resource/rimuovi.png").pixmap(10, 10));
    removeLabel->setCursor(Qt::PointingHandCursor);
    connect(removeLabel, SIGNAL(clicked()), this, SLOT(remove()));
    hLayout->addWidget(removeLabel);
    hLayout->addWidget(iconLabel);
    hLayout->addWidget(name);
    hLayout->addWidget(iconStatus);
    QFrame *fr = new QFrame;
    fr->setLayout(hLayout);
    setLayout(hLayout);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
    adjustSize();
    FaderWidget *faderWidget = new FaderWidget(this, Qt::white);
    faderWidget->start();
}

void QRecipientWidget::setParentItem(QListWidgetItem * it) {
    pIt = it;
}

void QRecipientWidget::remove() {
    emit removed(pIt);
}

void QRecipientWidget::setStatusFailed(QString message) {
    iconStatus->setPixmap(QIcon(":/resource/jms-not-active.png").pixmap(16, 16));
    iconStatus->setToolTip(message);
    iconStatus->setCursor(Qt::WhatsThisCursor);
}

void QRecipientWidget::restoreOriginalIcon() {
    iconLabel->setPixmap(original);
}

void QRecipientWidget::setStatusWorking() {
    iconStatus->setPixmap(QIcon(":/resource/jms-activing.png").pixmap(16, 16));
    iconStatus->setCursor(Qt::WhatsThisCursor);
}

void QRecipientWidget::clearStatus(){
    iconStatus->clear();
}

void QRecipientWidget::setStatusSuccess(QString message) {
    iconStatus->setPixmap(QIcon(":/resource/jms-active.png").pixmap(16, 16));
    iconStatus->setToolTip(message);
    iconStatus->setCursor(Qt::WhatsThisCursor);
}

libJackSMS::dataTypes::phoneNumber QRecipientWidget::getPhone() const {
    return number;
}

void QRecipientWidget::setIcon(QPixmap Icon) {
    iconLabel->setPixmap(Icon);
}
QString QRecipientWidget::getAccountId() const {
    return accountId;
}

QString QRecipientWidget::getName() const {
    return nameStr;
}
