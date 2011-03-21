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
        setItemHidden(item(count()-1), true);
    else
        setItemHidden(item(count()-1), false);
}

void SmsList::changeNameForGivenNumber(libJackSMS::dataTypes::phoneNumber numero, QString newname) {
    for (int i = 0; i < count()-1; ++i) {
        SmsWidget* smswid = dynamic_cast<SmsWidget*>(itemWidget(item(i)));
        if (smswid->getPhoneNum() == numero) {
            smswid->setName(newname);
        }
    }
}

void SmsList::changeNameForGivenContact(libJackSMS::dataTypes::contact c, QString newname) {
    for (int i = 0; i < count()-1; ++i) {
        SmsWidget* smswid = dynamic_cast<SmsWidget*>(itemWidget(item(i)));
        if ((smswid->getPhoneNum() == c.getPhone()) || (smswid->getPhoneNum() == c.getVirtualNumber())) {
            smswid->setName(newname);
        }
    }
}

void SmsList::setItemHidden(const QListWidgetItem *item, bool hide) {
    if (row(item) != (count()-1))
        QListWidget::setItemHidden(item, hide);
}
