#include <QtGui/QApplication>
#include "mainjacksms.h"

int main(int argc, char *argv[])
{


    try{
        QApplication a(argc, argv);
        MainJackSMS w;
        w.show();
        return a.exec();
    }catch(std::exception r_e){
        std::cout << "std exception caught: ";
        std::cout << r_e.what() << std::endl;
        std::cout << "typename: ";
        std::cout << typeid( r_e ).name() << std::endl;
    }catch(QtConcurrent::UnhandledException e){
        std::cout << "QtConcurrent::UnhandledException";
    }catch(...){
    std::cout << "unknown exception ";
    }


}
