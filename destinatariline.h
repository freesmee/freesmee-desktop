#ifndef DESTINATARILINE_H
#define DESTINATARILINE_H

#include <QLineEdit>
#include <QKeyEvent>
#include <QEvent>

class DestinatariLine : public QLineEdit
{
    Q_OBJECT
public:
    explicit DestinatariLine(QWidget *parent = 0);

private:
    bool event(QEvent *event);

signals:
    void tabKeyPressed();

};

#endif // DESTINATARILINE_H
