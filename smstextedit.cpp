#include "smstextedit.h"

SmsTextEdit::SmsTextEdit(QWidget* parent) :
    QTextEdit(parent)
{
}

void SmsTextEdit::keyPressEvent(QKeyEvent* e){
    if((e->key() == Qt::Key_Return) || (e->key() == Qt::Key_Enter)){
        if (e->modifiers() & Qt::ShiftModifier){
            e->accept();
            emit richiestoInvio();
            return;
        }
    }
    QTextEdit::keyPressEvent(e);
}
