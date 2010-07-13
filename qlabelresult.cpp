#include "qlabelresult.h"

QLabelResult::QLabelResult(QWidget * parent)
    :QLabel(parent){
    setText("adad");

}

void QLabelResult::mousePressEvent ( QMouseEvent * event ){
    emit clicked();
}

