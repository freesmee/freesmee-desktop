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

#include "qrecipientwidget.h"
#include <QStyle>
#include "faderwidget.h"
#include "qlabelresult.h"

QRecipientWidget::QRecipientWidget(const QString &_contactName, const QString &accId, const libJackSMS::dataTypes::phoneNumber &_number, QPixmap Icon, QWidget *parent) :
    QWidget(parent),
    contactName(_contactName),
    accountId(accId),
    number(_number),
    nameStr(contactName)
 {
    original = Icon;
    hLayout = new QHBoxLayout;
    //hLayout->setMargin(3);
    iconLabel = new QLabel;
    iconLabel->setPixmap(Icon);
    iconLabel->setFixedSize(16, 16);
    iconStatus = new QLabel;
    iconStatus->setFixedSize(16, 16);
    name = new QLabel(contactName);
    name->setCursor(Qt::WhatsThisCursor);
    name->setToolTip(number.toString());
    name->setFixedHeight(16);
    name->adjustSize();
    //name->setFixedWidth(name->width());
    name->setContentsMargins(0, 0, 5, 0);
    removeLabel = new QLabelResult(this);
    removeLabel->setFixedSize(16, 16);
    removeLabel->setPixmap(QIcon(":/resource/rimuovi.png").pixmap(10, 10));
    removeLabel->setCursor(Qt::PointingHandCursor);
    connect(removeLabel, SIGNAL(clicked()), this, SLOT(remove()));
    hLayout->addWidget(removeLabel);
    hLayout->addWidget(iconLabel);
    hLayout->addWidget(name);
    hLayout->addWidget(iconStatus);
    hLayout->setContentsMargins(3, 0, 3, 0);
    setLayout(hLayout);
    setFixedHeight(22);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
    adjustSize();

    FaderWidget *faderWidget = new FaderWidget(this, Qt::white);
    faderWidget->start();
}

void QRecipientWidget::setParentItem(QListWidgetItem * it)
{
    pIt = it;
}

void QRecipientWidget::remove()
{
    emit removed(pIt);
}

void QRecipientWidget::setStatusFailed(QString message)
{
    iconStatus->setPixmap(QIcon(":/resource/jms-not-active.png").pixmap(16, 16));
    iconStatus->setToolTip(message);
    iconStatus->setCursor(Qt::WhatsThisCursor);
}

void QRecipientWidget::restoreOriginalIcon()
{
    iconLabel->setPixmap(original);
}

void QRecipientWidget::setStatusWorking()
{
    iconStatus->setPixmap(QIcon(":/resource/jms-activating.png").pixmap(16, 16));
    iconStatus->setCursor(Qt::WhatsThisCursor);
}

void QRecipientWidget::clearStatus()
{
    iconStatus->clear();
}

void QRecipientWidget::setStatusSuccess(QString message)
{
    iconStatus->setPixmap(QIcon(":/resource/jms-active.png").pixmap(16, 16));
    iconStatus->setToolTip(message);
    iconStatus->setCursor(Qt::WhatsThisCursor);
}

libJackSMS::dataTypes::phoneNumber QRecipientWidget::getPhone() const
{
    return number;
}

void QRecipientWidget::setIcon(QPixmap Icon)
{
    iconLabel->setPixmap(Icon);
}

QString QRecipientWidget::getAccountId() const
{
    return accountId;
}

QString QRecipientWidget::getName() const
{
    return nameStr;
}
