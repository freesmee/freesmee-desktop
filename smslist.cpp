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

#include "smslist.h"

SmsList::SmsList(QWidget *parent) :
    QListWidget(parent)
{
    SmsWidget* wid = new SmsWidget("Carica altri messaggi");
    QListWidgetItem *item = new QListWidgetItem;
    item->setSizeHint(QSize(80,50));
    QListWidget::addItem(item);
    setItemWidget(item, wid);
    QListWidget::setItemHidden(item, true);
}

void SmsList::keyPressEvent(QKeyEvent* e){
    if(e->key() == Qt::Key_Delete){
        e->accept();
        emit smsListCanc();
        return;
    }else if(e->matches(QKeySequence::Copy)) {
        QClipboard *clipboard = QApplication::clipboard();
        clipboard->setText(static_cast<SmsWidget*>(itemWidget(item(currentRow())))->getText());
        e->accept();
    }else if(e->key() == Qt::Key_C){
        QClipboard *clipboard = QApplication::clipboard();
        clipboard->setText(static_cast<SmsWidget*>(itemWidget(item(currentRow())))->getText());
        e->accept();
    }

    QListWidget::keyPressEvent(e);
}

void SmsList::addItem(QListWidgetItem *item)
{
    insertItem(count()-1, item);
}

void SmsList::clear() {

    clearSelection();
    scrollToTop();
    QListWidget::clear();

    SmsWidget* wid = new SmsWidget("Carica altri messaggi");
    QListWidgetItem *item = new QListWidgetItem;
    item->setSizeHint(QSize(80,50));
    QListWidget::addItem(item);
    setItemWidget(item, wid);
    QListWidget::setItemHidden(item, true);
}

void SmsList::hideCaricaAltri(bool hide) {
    if (hide)
        QListWidget::setItemHidden(item(count()-1), true);
    else
        QListWidget::setItemHidden(item(count()-1), false);
}

void SmsList::changeNameForGivenNumber(libJackSMS::dataTypes::phoneNumber numero, QString newname)
{
    for (int i = 0; i < count()-1; ++i)
    {
        SmsWidget* smswid = dynamic_cast<SmsWidget*>(itemWidget(item(i)));
        if (smswid->getPhoneNum() == numero)
        {
            smswid->setName(newname);
        }
    }
}

void SmsList::changeNameForGivenContact(libJackSMS::dataTypes::contact c, QString newname)
{
    for (int i = 0; i < count()-1; ++i)
    {
        SmsWidget* smswid = dynamic_cast<SmsWidget*>(itemWidget(item(i)));
        if (smswid->getPhoneNum() == c.getPhone())
            smswid->setName(newname);
    }
}

void SmsList::setItemHidden(const QListWidgetItem *item, bool hide) {
    if (row(item) != (count()-1))
        QListWidget::setItemHidden(item, hide);
}
