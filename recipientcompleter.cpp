#include "recipientcompleter.h"

RecipientCompleter::RecipientCompleter(QObject *parent) :
    QCompleter(parent)
{
}

RecipientCompleter::RecipientCompleter(const QStringList &completions, QObject *parent) :
    QCompleter(completions, parent)
{
}
