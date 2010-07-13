#include <QImage>
#include <QByteArray>
#ifndef CAPTCHADIALOG_H
#define CAPTCHADIALOG_H

#include <QtGui/QDialog>

namespace Ui {
    class CaptchaDialog;
}

class CaptchaDialog : public QDialog {
    Q_OBJECT
public:
    QString result;
    CaptchaDialog(const QByteArray &_imgData,QString zoomFactor="100%",QWidget *parent=0 );
    ~CaptchaDialog();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::CaptchaDialog *m_ui;
    QImage original;
    QSize dim;
private slots:
    void on_zoomOut_clicked();
    void on_zoomIn_clicked();
    void on_pushButton_clicked();

};

#endif // CAPTCHADIALOG_H
