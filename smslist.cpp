#include "smslist.h"

SmsList::SmsList(QWidget *parent) :
    QListWidget(parent)
{
    caricaAltriPresent = false;
    SmsWidget* wid = new SmsWidget("Carica altri messaggi");
    QListWidgetItem *item = new QListWidgetItem;
    item->setSizeHint(QSize(80,50));
    addItem(item);
    setItemWidget(item, wid);
    caricaAltriPresent = true;
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

void SmsList::addItem(QListWidgetItem *item) {

    if (caricaAltriPresent)
        insertItem(count()-1, item);
    else
        QListWidget::addItem(item);

}

void SmsList::hideCaricaAltri() {
    if (caricaAltriPresent) {
        QListWidgetItem* it = item(count());
        QWidget* wid = itemWidget(it);
        caricaAltriPresent = false;
        takeItem(count());
        delete it;
        delete wid;
    }
}
