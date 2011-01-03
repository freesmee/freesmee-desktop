#ifndef SMSTEXTEDIT_H
#define SMSTEXTEDIT_H

#include <QTextEdit>
#include <QKeyEvent>

class SmsTextEdit : public QTextEdit
{
    Q_OBJECT

public:
    explicit SmsTextEdit(QWidget* parent = 0);

private:
    void keyPressEvent(QKeyEvent* e);

signals:
    void richiestoInvio();
};

#endif // SMSTEXTEDIT_H
