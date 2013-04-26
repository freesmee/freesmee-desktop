/*
    Copyright (C) <2011>

    <enrico bacis> <enrico.bacis@gmail.com>
    <ivan vaccari> <grisson@jacksms.it>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef CAPTCHADIALOG_H
#define CAPTCHADIALOG_H

#include <QtGui/QDialog>
#include <QImage>
#include <QByteArray>

namespace Ui
{
    class CaptchaDialog;
}

class CaptchaDialog : public QDialog
{
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
