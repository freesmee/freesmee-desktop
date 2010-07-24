

/*
    Copyright (C) <2009>  <ivan vaccari> <grisson@jacksms.it>

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

#include "EncodingTransformAccents.h"
#include <QList>
#include <QString>
#include <QPair>

namespace libJackSMS{

    encodingTransformAccents::encodingTransformAccents(){
    }
    encodingTransformAccents::~encodingTransformAccents(){
    }
    QString encodingTransformAccents::getEncodedAndUrlString(QString _text){
        QByteArray _t=getEncodedString(_text).toLocal8Bit().toPercentEncoding();
        return QString(_t);
    }
    QString encodingTransformAccents::getEncodedString(QString _text){
        QList<QPair<QString,QString> > symbols;
        symbols.push_back(qMakePair(QString("�"),QString("e'")));
        symbols.push_back(qMakePair(QString("�"),QString("e'")));
        symbols.push_back(qMakePair(QString("�"),QString("a'")));
        symbols.push_back(qMakePair(QString("�"),QString("u'")));
        symbols.push_back(qMakePair(QString("�"),QString("i'")));
        symbols.push_back(qMakePair(QString("�"),QString("o'")));
        symbols.push_back(qMakePair(QString("�"),QString("a'")));
        symbols.push_back(qMakePair(QString("�"),QString("i'")));
        symbols.push_back(qMakePair(QString("�"),QString("o'")));
        symbols.push_back(qMakePair(QString("�"),QString("u'")));
        symbols.push_back(qMakePair(QString("�"),QString("E'")));
        symbols.push_back(qMakePair(QString("�"),QString("I'")));
        symbols.push_back(qMakePair(QString("�"),QString("I'")));
        symbols.push_back(qMakePair(QString("�"),QString("I'")));
        symbols.push_back(qMakePair(QString("�"),QString("O'")));
        symbols.push_back(qMakePair(QString("�"),QString("U'")));
        symbols.push_back(qMakePair(QString("�"),QString("U'")));
        symbols.push_back(qMakePair(QString("�"),QString("y'")));
        symbols.push_back(qMakePair(QString("�"),QString("Y'")));

        symbols.push_back(qMakePair(QString(QChar(232)),QString("e'")));
        symbols.push_back(qMakePair(QString(QChar(233)),QString("e'")));
        symbols.push_back(qMakePair(QString(QChar(224)),QString("a'")));
        symbols.push_back(qMakePair(QString(QChar(225)),QString("a'")));
        symbols.push_back(qMakePair(QString(QChar(249)),QString("u'")));
        symbols.push_back(qMakePair(QString(QChar(250)),QString("u'")));
        symbols.push_back(qMakePair(QString(QChar(236)),QString("i'")));
        symbols.push_back(qMakePair(QString(QChar(237)),QString("i'")));
        symbols.push_back(qMakePair(QString(QChar(242)),QString("o'")));
        symbols.push_back(qMakePair(QString(QChar(243)),QString("o'")));

        QList<QPair<QString,QString> >::const_iterator i=symbols.begin();
        QList<QPair<QString,QString> >::const_iterator i_end=symbols.end();

        for(;i!=i_end;++i){
            _text.replace(i->first,i->second);
        }
        return _text;
    }
}
