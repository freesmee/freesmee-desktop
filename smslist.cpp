#include "smslist.h"

SmsList::SmsList(QWidget *parent) :
    QListWidget(parent)
{
    SmsWidget* wid = new SmsWidget("Carica altri messaggi");
    QListWidgetItem *item = new QListWidgetItem;
    item->setSizeHint(QSize(80,50));
    QListWidget::addItem(item);
    setItemWidget(item, wid);
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
}

void SmsList::hideCaricaAltri(bool hide) {
    if (hide)
        setItemHidden(item(count()-1), true);
    else
        setItemHidden(item(count()-1), false);
}
