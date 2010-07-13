#ifndef QLABELRESULT_H
#define QLABELRESULT_H

#include <QLabel>

class QLabelResult : public QLabel
{
    Q_OBJECT
public:
    QLabelResult(QWidget * parent);
    void mousePressEvent ( QMouseEvent * event );

signals:
    void clicked();
};

#endif // QLABELRESULT_H
