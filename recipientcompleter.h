#ifndef RECIPIENTCOMPLETER_H
#define RECIPIENTCOMPLETER_H

#include <QCompleter>

class RecipientCompleter : public QCompleter
{
    Q_OBJECT
public:
    explicit RecipientCompleter(QObject *parent = 0);
    RecipientCompleter(const QStringList& completions, QObject *parent = 0);

signals:

public slots:

};

#endif // RECIPIENTCOMPLETER_H
