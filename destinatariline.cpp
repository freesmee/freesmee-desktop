#include "destinatariline.h"

DestinatariLine::DestinatariLine(QWidget *parent) :
    QLineEdit(parent)
{
}

bool DestinatariLine::event(QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *ke = static_cast<QKeyEvent *>(event);
        if (ke->key() == Qt::Key_Tab) {
            emit tabKeyPressed();
            return true;
        }
    }
    return QWidget::event(event);
}
