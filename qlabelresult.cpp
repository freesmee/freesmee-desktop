#include "qlabelresult.h"

QLabelResult::QLabelResult(QWidget * parent)
    :QLabel(parent){
    setText("");

}

void QLabelResult::mousePressEvent ( QMouseEvent * event ){
    emit clicked();
}

